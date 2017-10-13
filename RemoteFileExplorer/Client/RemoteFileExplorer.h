#pragma once

#include <memory>

#include "Client/Network/ServerConnector.h"
#include "Common/FileExplorerInterface.h"

namespace remoteFileExplorer
{
namespace client
{
///////////////////////////////////////////////////////////////////////////////
class RPCException : public std::exception
{
public:
	const char* what() const noexcept
	{
		return "Can't communicate with a server.";
	}
};

///////////////////////////////////////////////////////////////////////////////
// Thread Unsafe.
// 동기적으로(blocking으로) RPC를 수행한다.
// RPC 수행 중 문제 상황이 생길 경우, RPCException 예외를 던진다.
class RemoteFileExplorer final : public common::FileExplorerInterface
{
public:
	RemoteFileExplorer(std::size_t bufferSize = 64 * 1024);
	~RemoteFileExplorer();

	int Connect(std::uint8_t ipAddress[4], std::uint16_t port);
	int Disconnect();

	virtual int GetLogicalDriveInfo(
		std::vector<common::LogicalDrive>& drives) override;
	virtual int GetDirectoryInfo(
		const std::wstring& path,
		common::file_count_t offset,
		common::Directory& dir) override;

private:
	network::ServerConnector serverConnector_;
	const std::size_t maxBufferSize_;
	std::uint8_t* buffer_;
};

///////////////////////////////////////////////////////////////////////////////
inline RemoteFileExplorer::RemoteFileExplorer(std::size_t bufferSize)
	: maxBufferSize_(bufferSize)
{
	buffer_ = new std::uint8_t[bufferSize];
}

inline RemoteFileExplorer::~RemoteFileExplorer()
{
	delete[] buffer_;
}

} // namespace client
} // namespace remoteFileExplorer