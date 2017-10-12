#include "Server/Network/ClientHandlerThread.h"

#include "Server/Network/ClientPacketHandler.h"
#include "Server/Network/SocketBuffer.h"

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
	DWORD bytesTransferred;
	ClientSession* clientSession = nullptr;  // TODO: dangling 위험 해결하기.
	OVERLAPPED* dummy = nullptr;

	while (!terminatedFlag_.load())
	{
		BOOL success = GetQueuedCompletionStatus(
			hCompletionPort_,    // Completion Port
			&bytesTransferred,   // 전송된 바이트수
			(PULONG_PTR) &clientSession,   // ClientSession 포인터
			&dummy, // OVERLAPPED 구조체 포인터.
			10  // 10ms. TODO: 하드코딩 안하기.
		);

		// TODO: 검토.
		if (!success && dummy == nullptr)
		{
			// 아마도 time-out.
			continue;
		}

		SOCKET hSocket = clientSession->GetSocketHandle();

		if (!success || bytesTransferred == 0) //EOF 전송시.
		{
			// 순서 중요!!!
			if (fDestroyClientSession_(hSocket) != 0)
			{

			}
			closesocket(hSocket);
			//SocketBuffer::ReleaseBuffer(socketBuffer);
			continue;
		}

		clientSession->UpdateReceiveBuffer(bytesTransferred);

		std::uint8_t* recvBuffer;
		std::size_t recvBufferSize;

		std::tie(recvBuffer, recvBufferSize) =
			clientSession->GetReceiveBufferStatus();

		std::size_t processedBufferSize = 0;

		while (true)
		{
			if (recvBufferSize < sizeof(std::uint32_t))
				break;

			// TODO: 시스템 엔디안 표기법 종속성에 의한 잠재적 이슈 존재.
			std::uint32_t messageSize =
				*reinterpret_cast<std::uint32_t*>(recvBuffer);

			recvBuffer += sizeof(std::uint32_t);
			recvBufferSize -= sizeof(std::uint32_t);
			//processedBufferSize += sizeof(std::uint32_t);

			if (recvBufferSize < messageSize)
				break;

			recvBuffer += messageSize;
			recvBufferSize -= messageSize;
			processedBufferSize += (messageSize + sizeof(std::uint32_t));

			// TODO: 제대로 관리.
			std::uint8_t* sendBuffer = new std::uint8_t[64 * 1024];
			std::size_t sendBufferSize = 64 * 1024;

			if (HandleClientPacket(
				*clientSession,
				recvBuffer - messageSize,
				recvBufferSize + messageSize,
				sendBuffer,
				&sendBufferSize) != 0)
			{
				// Ignore failure.
				continue;
			}

			WSABUF wsabuf;
			DWORD sendBytes;

			// Send the length of message.
			std::uint32_t sendMessageLength =
				static_cast<std::uint32_t>(sendBufferSize);
			wsabuf.buf = reinterpret_cast<char*>(&sendMessageLength);
			wsabuf.len = sizeof(std::uint32_t);
			// TODO: 실패 시, socket 닫는쪽으로 예외처리하기. (다른 곳도)
			//       Send byte 수 검사.
			//       Send도 비동기적으로 바꾸기.
			int a = WSASend(hSocket, &wsabuf, 1,
				&sendBytes, 0, nullptr, nullptr);

			int aa = WSAGetLastError();

			// Send the message.
			wsabuf.buf = reinterpret_cast<char*>(sendBuffer);
			wsabuf.len = sendBufferSize;
			int b =  WSASend(hSocket, &wsabuf, 1,
				&sendBytes, 0, nullptr, nullptr);

			int bb = WSAGetLastError();

			delete[] sendBuffer;
		}

		clientSession->ConsumeReceiveBuffer(processedBufferSize);

		if (clientSession->ReceiveBufferIsFull())
		{
			// TODO: 실패 message 전송해주기.
			// Message를 담기에 receiver buffer의 크기가 부족한 경우,
			// Message를 그냥 버리고, 새로운 메세지를 받는다.
			clientSession->ResetReceiveBuffer();
		}

		DWORD flags = 0;

		WSARecv(
			hSocket,                     // 클라이언트 소켓
			&clientSession->GetUpdatedWsabufRef(),     // 버퍼
			1,		                     // 버퍼의 수
			nullptr,
			&flags,
			&clientSession->GetInitializedOVELAPPED(), // OVERLAPPED 구조체 포인터
			nullptr);
	}

	return 0;
}

} // namespace network
} // namespace server
} // namespace remoteFileExplorer