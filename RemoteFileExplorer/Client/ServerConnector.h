#pragma once

#include <winsock2.h>

#include <memory>
#include <mutex>

#include "Common/FileExplorerInterface.h"

#pragma comment(lib,"ws2_32.lib")

namespace remoteFileExplorer
{
namespace client
{
///////////////////////////////////////////////////////////////////////////////
class ServerConnector final
{
public:
	~ServerConnector();
	int Connect(std::uint8_t ipAddress[4], std::uint16_t port);
	int Disconnect();
	int Communicate(
		const std::uint8_t* buffer,
		std::size_t* bufferSize,
		std::size_t maxBufferSize);

private:
	SOCKET hSocket_;
	std::mutex mutex_;
	bool connected_{ false };
};

} // namespace client
} // namespace remoteFileExplorer