#include "FileExplorerWatcher.h"

#include <string>

namespace remoteFileExplorer
{
namespace mfc
{
namespace server
{
///////////////////////////////////////////////////////////////////////////////
void FileExplorerWatcher::GetLogicalDriveInfo()
{
	::PostMessage(
		hWindow_,
		wmBase_ + WmOffsetGetLogicalDriveInfo,
		0,
		0);
}

///////////////////////////////////////////////////////////////////////////////
void FileExplorerWatcher::GetDirectoryInfo(
	const std::wstring& path,
	std::uint32_t offset)
{
	::PostMessage(
		hWindow_,
		wmBase_ + WmOffsetGetDirectoryInfo,
		(WPARAM) new std::pair<std::wstring, std::uint32_t>(path, offset),
		0);
}

} // namespace server
} // namespace mfc
} // namespace remoteFileExplorer