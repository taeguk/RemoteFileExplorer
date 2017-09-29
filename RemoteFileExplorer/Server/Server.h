#pragma once

#include <mutex>
#include <atomic>
#include <memory>

#include "Server/FileExplorerServiceInterface.h"

namespace remoteFileExplorer
{
namespace server
{
// ListenerThread의 정의부 없이 std::unique_ptr<T>을 사용하기 위한 전방 선언.
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

} // namespace server
} // namespace remoteFileExplorer