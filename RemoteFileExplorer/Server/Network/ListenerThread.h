#pragma once

#include <winsock2.h>

#include <mutex>
#include <atomic>
#include <map>
#include <vector>
#include <thread>

#include "Server/FileExplorerServiceInterface.h"
#include "Server/Network/ClientSession.h"
#include "Server/Network/ClientHandlerThread.h"

namespace remoteFileExplorer
{
namespace server
{
namespace network
{
///////////////////////////////////////////////////////////////////////////////
// 서버 소켓을 관리하며, Client들의 접속을 대기하고 수락하는 역할을 한다.
// 그리고 ClientHandlerThread들도 여기서 관리된다.
class ListenerThread final
{
public:
    ListenerThread() = default;
    ~ListenerThread();

    // Non-copyable and Non-moveable.
    ListenerThread(const ListenerThread&) = delete;
    ListenerThread& operator=(const ListenerThread&) = delete;
    ListenerThread(ListenerThread&&) = delete;
    ListenerThread& operator=(ListenerThread&&) = delete;

    int Start(
        std::uint16_t port,
        std::size_t threadNumber,
        std::unique_ptr<FileExplorerServiceInterface> fileExplorerService);

private:
    int ListenLoop_();

    int DestroyClientSession(SOCKET hSocket);

    std::unique_ptr<FileExplorerServiceInterface> fileExplorerService_;
    std::uint16_t port_;
    std::size_t threadNumber_;

    std::map<SOCKET, ClientSession> sessionMap_;
    std::mutex sessionMapMutex_;

    std::atomic<bool> isStarted_{ false };
    SOCKET hServerSocket_;
    HANDLE hCompletionPort_;

    std::vector<ClientHandlerThread> handlerThreads_;
    std::atomic<bool> terminatedFlag_{ false };
    std::thread listenerThread_;  // 무조건 맨 마지막에 와야함. (소멸순서 때문에)
};

} // namespace network
} // namespace server
} // namespace remoteFileExplorer