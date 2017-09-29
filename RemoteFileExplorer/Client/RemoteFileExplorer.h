#pragma once

#include <memory>

#include "Client/Detail/ServerConnector.h"
#include "Common/FileExplorerInterface.h"
#include "Message/EchoMessage.h"

namespace remoteFileExplorer
{
namespace client
{
///////////////////////////////////////////////////////////////////////////////
// Thread Unsafe.
class RemoteFileExplorer final : public common::FileExplorerInterface
{
public:
	int Connect(std::uint8_t ipAddress[4], std::uint16_t port);
	int Disconnect();

	// 동기적으로(blocking으로) 수행.
	virtual std::string Echo(const char* str) override;

private:
	detail::ServerConnector serverConnector_; // TODO: 헤더파일에서 detail 의존성 걷어내기.
};

} // namespace client
} // namespace remoteFileExplorer