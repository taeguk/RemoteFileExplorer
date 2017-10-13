#include "Server/Network/ListenerThread.h"

#include "Server/Network/IOContext.h"

namespace remoteFileExplorer
{
namespace server
{
namespace network
{
///////////////////////////////////////////////////////////////////////////////
ListenerThread::ListenerThread(
	std::uint16_t port,
	std::size_t threadNumber,
	std::unique_ptr<FileExplorerServiceInterface> fileExplorerService)
	: port_(port),
	  threadNumber_(threadNumber),
	  fileExplorerService_(std::move(fileExplorerService))
{
	listenerThread_ = std::thread(&ListenerThread::ThreadMain_, this);
}

///////////////////////////////////////////////////////////////////////////////
ListenerThread::~ListenerThread()
{
	// 먼저, client handler thread들을 종료시킨다.
	handlerThreads_.clear();

	// 그다음, listener thread를 종료시킨다.
	terminatedFlag_.store(true);
	listenerThread_.join();
}

///////////////////////////////////////////////////////////////////////////////
int ListenerThread::ThreadMain_()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return -1;

	hCompletionPort_ =
		CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);

	// Client Handler Thread 들 생성.
	handlerThreads_.reserve(threadNumber_);
	for (std::size_t i = 0; i < threadNumber_; ++i)
	{
		handlerThreads_.emplace_back(
			hCompletionPort_,
			[this](SOCKET hSocket) -> int
			{
				return this->DestroyClientSession(hSocket);
			});
	}

	SOCKET hServerSocket =
		WSASocket(AF_INET, SOCK_STREAM, 0, nullptr, 0, WSA_FLAG_OVERLAPPED);

	SOCKADDR_IN serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_port = htons(port_);

	bind(hServerSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress));

	listen(hServerSocket, static_cast<int>(handlerThreads_.size()));

	// 소켓을 non-blocking으로 설정.
	u_long cmdArg = 1;
	if (ioctlsocket(hServerSocket, FIONBIO, &cmdArg) == SOCKET_ERROR)
		return -1;

	fd_set fdSet;
	FD_ZERO(&fdSet);
	FD_SET(hServerSocket, &fdSet);
	timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 10 * 1000; // 10ms

	while (!terminatedFlag_.load())
	{
		SOCKET hClientSocket;
		SOCKADDR_IN clientAddress;
		int addrLen = sizeof(clientAddress);

		FD_ZERO(&fdSet);
		FD_SET(hServerSocket, &fdSet);

		int selectRet = select(1, &fdSet, nullptr, nullptr, &timeout);

		if (selectRet == 0)
			continue; // timeout
		else if (selectRet == SOCKET_ERROR)
			return -1; // 에러.

		hClientSocket = accept(
			hServerSocket,
			(SOCKADDR*) &clientAddress,
			&addrLen);

		ClientSession* clientSession;
		{
			std::lock_guard<decltype(sessionMapMutex_)> lk(sessionMapMutex_);

			auto result = sessionMap_.insert(std::make_pair(hClientSocket,
				ClientSession(hClientSocket, clientAddress,
					fileExplorerService_->Clone())
			));

			if (!result.second)
			{
				// 이미 세션이 존재하는 경우.
				// 상대방이 연결을 끊고나서 바로 다시 연결을 요청하는 경우,
				//    아주 아주 드문 확률로 발생할 수 도 있다.
				closesocket(hClientSocket);
				continue;
			}

			clientSession = &(result.first->second);
		}
		
		// Client Session(socket)을 I/O completion 포트와 연결.
		if (CreateIoCompletionPort(
			(HANDLE) hClientSocket,
			hCompletionPort_,
			(ULONG_PTR) clientSession,
			0) == nullptr)
		{
			DestroyClientSession(hClientSocket);
			continue;
		}

		IORecvContext* recvContext = new IORecvContext();
		DWORD flags = 0;

		// 비동기 수신 요청.
		int ret = WSARecv(
			hClientSocket,
			&recvContext->GetUpdatedWsabufRef(),
			1,
			nullptr,
			&flags,
			&recvContext->GetOverlappedRef(),
			nullptr
		);

		// 수신 요청 실패 시,
		if (ret == SOCKET_ERROR && WSAGetLastError() != ERROR_IO_PENDING)
		{
			// 순서 중요!!!
			(void) DestroyClientSession(hClientSocket);
			delete recvContext;
			continue;
		}
	}

	closesocket(hServerSocket);

	(void) WSACleanup();

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
int ListenerThread::DestroyClientSession(SOCKET hSocket)
{
	std::lock_guard<decltype(sessionMapMutex_)> lk(sessionMapMutex_);

	auto it = sessionMap_.find(hSocket);
	if (it == std::end(sessionMap_))
		return -1;

	sessionMap_.erase(it);
	closesocket(hSocket);

	return 0;
}

} // namespace network
} // namespace server
} // namespace remoteFileExplorer