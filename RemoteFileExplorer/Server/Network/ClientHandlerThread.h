#pragma once

#include <winsock2.h>

#include <cassert>
#include <atomic>
#include <mutex>
#include <thread>

#include "Server/Network/ClientSession.h"

namespace remoteFileExplorer
{
namespace server
{
namespace network
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
	// ListenerThread에서 ClientHandlerThread를 std::vector로 관리하기 때문에,
	//   이동 연산들이 정의가 되있긴 해야한다.
	// 로직 상 절대로 실행될 일이 없기 때문에 당장 문제는 없다.
	// 하지만, 깔끔한 방법은 아니기 때문에 추후 해결이 필요하다.
	ClientHandlerThread(const ClientHandlerThread&) = delete;
	ClientHandlerThread& operator=(const ClientHandlerThread&) = delete;
	ClientHandlerThread(ClientHandlerThread&&) { assert(false); }
	ClientHandlerThread& operator=(ClientHandlerThread&&) { assert(false); }

private:
	int ThreadMain_();

	HANDLE hCompletionPort_;
	std::function<int(SOCKET)> fDestroyClientSession_;
	std::atomic<bool> terminatedFlag_{ false };
	std::thread thread_;  // 무조건 맨 마지막에 와야함. (소멸순서 때문에)
};

} // namespace network
} // namespace server
} // namespace remoteFileExplorer