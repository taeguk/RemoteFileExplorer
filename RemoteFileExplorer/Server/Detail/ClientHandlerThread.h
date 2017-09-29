#pragma once

#include <winsock2.h>

#include <mutex>
#include <atomic>
#include <thread>

#include "Server/Detail/ClientSession.h"

namespace remoteFileExplorer
{
namespace server
{
namespace detail
{
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

} // namespace detail
} // namespace server
} // namespace remoteFileExplorer