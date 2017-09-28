#include "FileExplorerWatcher.h"

#include <string>

namespace remoteFileExplorer
{
namespace mfc
{
namespace server
{
///////////////////////////////////////////////////////////////////////////////
void FileExplorerWatcher::Echo(const char* str)
{
	::PostMessage(
		hWindow_, wmBase_ + WmOffsetEcho,
		(WPARAM) new std::string(str),
		0);
}

} // namespace server
} // namespace mfc
} // namespace remoteFileExplorer