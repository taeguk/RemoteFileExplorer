#pragma once

namespace remoteFileExplorer
{
namespace server
{
///////////////////////////////////////////////////////////////////////////////
class FileExplorerWatcherInterface
{
public:
	virtual ~FileExplorerWatcherInterface() = default;

	virtual void Echo(const char* str) = 0;
};

} // namespace server
} // namespace remoteFileExplorer