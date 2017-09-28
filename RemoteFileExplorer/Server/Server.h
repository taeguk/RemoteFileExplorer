#pragma once

#include <mutex>

// remove later
#include <atomic>
#include <winsock2.h>
#include <map>
#include <vector>
#include <thread>
#include <vector>
#include "Server/ClientSession.h"
#include "Server/FileExplorerServiceInterface.h"

namespace remoteFileExplorer
{
namespace server
{
namespace detail
{
	class ListenerThread;
	struct ListenerThreadDeleter
	{
		void operator() (ListenerThread* ptr) const;
	};
} // namespace detail

///////////////////////////////////////////////////////////////////////////////
class Server final
{
public:
	Server(std::unique_ptr<FileExplorerServiceInterface> fileExplorerService);
	~Server();

	// Non-copyable and Non-moveable.
	Server(const Server&) = delete;
	Server& operator=(const Server&) = delete;
	Server(Server&&) = delete;
	Server& operator=(Server&&) = delete;

	int Start(std::uint16_t port, std::size_t threadNumber = 0 /* 0 means default number */);
	int Stop();

private:
	std::mutex mutex_;
	std::atomic<bool> started_{ false };
	std::unique_ptr<FileExplorerServiceInterface> fileExplorerService_;
	std::unique_ptr<detail::ListenerThread, detail::ListenerThreadDeleter>
		listenerThread_{ nullptr };
};

/*****************************************************************************/
/****************************** INLINE FUNCTIONS *****************************/
/*****************************************************************************/
inline Server::Server(std::unique_ptr<FileExplorerServiceInterface> fileExplorerService)
	: fileExplorerService_(std::move(fileExplorerService))
{}

namespace detail
{
///////////////////////////////////////////////////////////////////////////////
struct SocketBuffer final
{
	static const std::size_t MaxBufferSize = 1024; // TODO: 자동으로 조정방법?

	static SocketBuffer* AcquireBuffer();
	static int ReleaseBuffer(SocketBuffer* socketBuffer);

	void Reset();

	OVERLAPPED overlapped;  // 반드시 맨앞에 와야함.
	std::uint8_t buffer[MaxBufferSize];
	WSABUF wsabuf;
};

///////////////////////////////////////////////////////////////////////////////
class ClientHandlerThread final
{
public:
	ClientHandlerThread(
		HANDLE hCompletionPort,
		const std::function<int(SOCKET)>& fDestroyClientSession);
	~ClientHandlerThread();

	// Non-copyable and Non-moveable.
	ClientHandlerThread(const ClientHandlerThread&) = delete;
	ClientHandlerThread& operator=(const ClientHandlerThread&) = delete;
	ClientHandlerThread(ClientHandlerThread&&) {}// = delete; // TODO:
	ClientHandlerThread& operator=(ClientHandlerThread&&) {}// = delete; // TODO:

private:
	int ThreadMain_();

	HANDLE hCompletionPort_;
	std::function<int(SOCKET)> fDestroyClientSession_;
	std::atomic<bool> terminatedFlag_{ false };
	std::thread thread_;  // 무조건 맨 마지막에 와야함. (소멸순서 때문에)
};

///////////////////////////////////////////////////////////////////////////////
class ListenerThread final
{
public:
	ListenerThread(
		std::uint16_t port,
		std::size_t threadNumber,
		std::unique_ptr<FileExplorerServiceInterface> fileExplorerService);
	~ListenerThread();

	// Non-copyable and Non-moveable.
	ListenerThread(const ListenerThread&) = delete;
	ListenerThread& operator=(const ListenerThread&) = delete;
	ListenerThread(ListenerThread&&) = delete;
	ListenerThread& operator=(ListenerThread&&) = delete;

private:
	int ThreadMain_();

	int DestroyClientSession(SOCKET hSocket);

	std::unique_ptr<FileExplorerServiceInterface> fileExplorerService_;
	std::uint16_t port_;
	std::size_t threadNumber_;

	std::map<SOCKET, ClientSession> sessionMap_;

	HANDLE hCompletionPort_;

	std::vector<detail::ClientHandlerThread> handlerThreads_;
	std::atomic<bool> terminatedFlag_{ false };
	std::thread listenerThread_;  // 무조건 맨 마지막에 와야함. (소멸순서 때문에)
};
} // namespace detail

} // namespace server
} // namespace remoteFileExplorer