#pragma once

#include <winsock2.h>

#include <memory>
#include <mutex>

#include "Common/FileExplorerInterface.h"

namespace remoteFileExplorer
{
namespace client
{
namespace network
{
///////////////////////////////////////////////////////////////////////////////
// Server와의 연결 및 통신을 담당한다.
// Thread Safe
class ServerConnector final
{
public:
    ~ServerConnector();
    int Connect(std::uint8_t ipAddress[4], std::uint16_t port);
    int Disconnect();
    int Communicate(
        std::uint8_t* buffer,
        std::size_t& bufferSize,
        std::size_t maxBufferSize);

private:
    SOCKET hSocket_;
    std::mutex mutex_;
    bool connected_{ false };
};

} // namespace network
} // namespace client
} // namespace remoteFileExplorer