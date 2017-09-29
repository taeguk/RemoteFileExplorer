#include "Server/Detail/ClientHandlerThread.h"

#include "Server/Detail/ClientPacketHandler.h"
#include "Server/Detail/SocketBuffer.h"

namespace remoteFileExplorer
{
namespace server
{
namespace detail
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
	ClientSession* clientSession;  // TODO: dangling 위험 해결하기.
	SocketBuffer* socketBuffer;

	while (!terminatedFlag_.load())
	{
		BOOL success = GetQueuedCompletionStatus(
			hCompletionPort_,    // Completion Port
			&bytesTransferred,   // 전송된 바이트수
			(PULONG_PTR) &clientSession,
			(LPOVERLAPPED*) &socketBuffer, // OVERLAPPED 구조체 포인터.
			10  // 10ms. TODO: 하드코딩 안하기.
		);

		if (!success)
		{
			// 아마도 time-out.
			continue;
		}

		SOCKET hSocket = clientSession->GetSocketHandle();

		if (bytesTransferred == 0) //EOF 전송시.
		{
			// 순서 중요!!!
			fDestroyClientSession_(hSocket);
			closesocket(hSocket);
			SocketBuffer::ReleaseBuffer(socketBuffer);
			continue;
		}

		std::size_t bufferSize = bytesTransferred;
		HandleClientPacket(
			*clientSession,
			socketBuffer->buffer,
			&bufferSize,
			SocketBuffer::MaxBufferSize);

		socketBuffer->wsabuf.len = (ULONG) bufferSize;
		WSASend(hSocket, &(socketBuffer->wsabuf),
			1, nullptr, 0, nullptr, nullptr);

		DWORD flags = 0;

		socketBuffer->Reset();
		WSARecv(
			hSocket,                     // 클라이언트 소켓
			&(socketBuffer->wsabuf),     // 버퍼
			1,		                     // 버퍼의 수
			nullptr,
			&flags,
			&(socketBuffer->overlapped), // OVERLAPPED 구조체 포인터
			nullptr);
	}

	return false;
}

} // namespace detail
} // namespace server
} // namespace remoteFileExplorer