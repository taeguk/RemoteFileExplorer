#include "Server/FileExplorerService.h"

namespace remoteFileExplorer
{
namespace server
{
///////////////////////////////////////////////////////////////////////////////
std::string FileExplorerService::Echo(const char* str)
{
	watcher_->Echo(str);
	return str;
}

} // namespace server
} // namespace remoteFileExplorer