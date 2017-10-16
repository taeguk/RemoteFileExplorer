#include "Server/Network/ClientHandlerThread.h"

#include "Server/ClientMessageHandler.h"
#include "Server/Network/IOContext.h"

namespace remoteFileExplorer
{
namespace server
{
namespace network
{
///////////////////////////////////////////////////////////////////////////////
ClientHandlerThread::ClientHandlerThread(
    HANDLE hCompletionPort,
    const std::function<int(SOCKET)>& fDestroyClientSession)
    : hCompletionPort_(hCompletionPort),
      fDestroyClientSession_(fDestroyClientSession)
{
    thread_ = std::thread(&ClientHandlerThread::ThreadMain_, this);
}

///////////////////////////////////////////////////////////////////////////////
ClientHandlerThread::~ClientHandlerThread()
{
    terminatedFlag_.store(true);
    thread_.join();
}

///////////////////////////////////////////////////////////////////////////////
int ClientHandlerThread::ThreadMain_()
{
    // fDestroyClientSession_ 를 호출하고 나면 clientSession은 invalidate된다.
    // 이 점에 주의하도록 하자!!
    ClientSession* clientSession = nullptr;
    IOContext* ioContext = nullptr;
    OVERLAPPED* overlapped = nullptr;

    while (!terminatedFlag_.load())
    {
        DWORD bytesTransferred;

        // I/O 작업 완료 통지를 받는다.
        BOOL success = GetQueuedCompletionStatus(
            hCompletionPort_,
            &bytesTransferred,
            (PULONG_PTR) &clientSession,
            &overlapped,
            10  // 10ms.
        );

        // Time Out.
        if (!success && overlapped == nullptr)
            continue;

        IOContext* ioContext = IOContext::PointerCastFrom(overlapped);
        SOCKET hSocket = clientSession->GetSocketHandle();

        // I/O 동작 실패 or 소켓 닫힘 (연결 끊김)
        if (!success || bytesTransferred == 0)
        {
            // 순서 중요!!!
            (void) fDestroyClientSession_(hSocket);
            delete ioContext;
            continue;
        }

        // 완료된 I/O가 송신일 경우,
        if (ioContext->GetType() == IOContextType::Send)
        {
            delete ioContext;
        }
        // 완료된 I/O가 수신일 경우,
        else
        {
            IORecvContext* recvContext =
                &IORecvContext::TypeCastFrom(*ioContext);

            // 수신 버퍼에 byteTransferred 만큼이 수신됐음을 알린다.
            recvContext->UpdateBuffer(bytesTransferred);

            std::uint8_t* recvBuffer;
            std::size_t recvBufferSize;

            // 현재 '수신 버퍼의 포인터'와
            //   '수신 버퍼의 데이터가 차있는 부분의 크기'를 얻는다.
            std::tie(recvBuffer, recvBufferSize) =
                recvContext->GetBufferStatus();

            // 수신 버퍼중 처리가 완료된 부분의 크기를 저장하는 변수.
            std::size_t processedBufferSize = 0;
            bool sessionDestroyFlag = false;

            // 수신 버퍼 내에 저장되어 있는 메세지들을 식별하고 처리한다.
            while (true)
            {
                // 수신버퍼로 부터 message size를 읽기 시도한다.
                if (recvBufferSize < sizeof(common::message_size_t))
                    break;

                common::message_size_t messageSize =
                    *reinterpret_cast<common::message_size_t*>(recvBuffer);

                recvBuffer += sizeof(common::message_size_t);
                recvBufferSize -= sizeof(common::message_size_t);

                // 아직 message가 수신버퍼에 다 안찼을 경우,
                //   처리가 불가능하므로 중지한다.
                if (recvBufferSize < messageSize)
                    break;

                recvBuffer += messageSize;
                recvBufferSize -= messageSize;
                processedBufferSize +=
                    (messageSize + sizeof(common::message_size_t));

                // Thread Local Storage에 위치하는 버퍼. (message 처리시 필요함.)
                thread_local const std::size_t MaxThreadBufferSize = 64 * 1024;
                thread_local std::uint8_t threadBuffer[MaxThreadBufferSize];
                std::size_t threadBufferSize = MaxThreadBufferSize;

                // Message를 처리한다.
                if (HandleClientMessage(
                    *clientSession,
                    recvBuffer - messageSize,
                    recvBufferSize + messageSize,
                    threadBuffer,
                    threadBufferSize) != 0)
                {
                    sessionDestroyFlag = true;
                    break;
                }

                // Request 처리 결과를 전송하기 위해 준비한다.
                IOSendContext* sendContext = new IOSendContext(
                    sizeof(common::message_size_t) + threadBufferSize);
                std::uint8_t* sendBuffer = sendContext->GetBuffer();

                *reinterpret_cast<common::message_size_t*>(sendBuffer)
                    = static_cast<common::message_size_t>(threadBufferSize);

                std::memcpy(
                    sendBuffer + sizeof(common::message_size_t),
                    threadBuffer,
                    threadBufferSize);

                // Reply 메세지를 비동기 송신한다.
                int ret = WSASend(
                    hSocket,
                    &sendContext->GetUpdatedWsabufRef(),
                    1,
                    nullptr,
                    0,
                    &sendContext->GetOverlappedRef(),
                    nullptr);

                // 송신 요청이 실패 했을 경우,
                if (ret == SOCKET_ERROR && WSAGetLastError() != ERROR_IO_PENDING)
                {
                    sessionDestroyFlag = true;
                    break;
                }
            }

            // 클라이언트와 세션을 유지하기에 심각한 에러가 발생했을 경우,
            //   클라이언트와의 연결을 끊는다. (세션을 종료한다.)
            if (sessionDestroyFlag)
            {
                // 순서 중요!!!
                (void) fDestroyClientSession_(hSocket);
                delete recvContext;
                continue;
            }

            // 수신버퍼 중 processedBufferSize 만큼을 처리했음을 알린다.
            recvContext->ConsumeBuffer(processedBufferSize);

            // 수신버퍼가 꽉찬 경우이다.
            // 이 경우는 message 한 개의 크기가 수신버퍼의 최대크기보다 큰 경우다.
            // 이런 경우는 처리가 불가능하므로, 클라이언트와의 연결을 끊는다.
            if (recvContext->CheckBufferIsFull())
            {
                (void) fDestroyClientSession_(hSocket);
                delete recvContext;
                continue;
            }

            // 비동기 수신 요청
            DWORD flags = 0;
            int ret = WSARecv(
                hSocket,
                &recvContext->GetUpdatedWsabufRef(),
                1,
                nullptr,
                &flags,
                &recvContext->GetOverlappedRef(),
                nullptr);

            // 수신 요청이 실패 했을 경우,
            if (ret == SOCKET_ERROR && WSAGetLastError() != ERROR_IO_PENDING)
            {
                // 순서 중요!!!
                (void) fDestroyClientSession_(hSocket);
                delete recvContext;
                continue;
            }
        }
    }

    return 0;
}

} // namespace network
} // namespace server
} // namespace remoteFileExplorer