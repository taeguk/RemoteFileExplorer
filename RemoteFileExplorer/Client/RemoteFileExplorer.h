#pragma once

#include <memory>

#include "Client/Network/ServerConnector.h"
#include "Common/FileExplorerInterface.h"

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
	virtual int GetLogicalDriveInfo(
		std::vector<common::LogicalDrive>& drives) override;
	virtual int GetDirectoryInfo(
		const std::wstring& path,
		std::uint32_t offset,
		common::Directory& dir) override;

private:
	network::ServerConnector serverConnector_;
};

} // namespace client
} // namespace remoteFileExplorer