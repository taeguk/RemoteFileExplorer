#pragma once

#include <string>

namespace remoteFileExplorer
{
namespace server
{
///////////////////////////////////////////////////////////////////////////////
class FileExplorerWatcherInterface
{
public:
	virtual ~FileExplorerWatcherInterface() = default;

	virtual void GetLogicalDriveInfo() = 0;
	virtual void GetDirectoryInfo(const std::wstring& path) = 0;
};

} // namespace server
} // namespace remoteFileExplorer