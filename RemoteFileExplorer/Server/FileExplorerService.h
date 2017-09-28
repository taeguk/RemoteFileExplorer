#pragma once

#include <memory>

#include "Server/FileExplorerServiceInterface.h"
#include "Server/FileExplorerWatcherInterface.h"

namespace remoteFileExplorer
{
namespace server
{
///////////////////////////////////////////////////////////////////////////////
/* TODO: Multithread Safe 해야함. (Reentrant하면 더 좋고.) */
class FileExplorerService : public FileExplorerServiceInterface
{
public:
	FileExplorerService(std::unique_ptr<FileExplorerWatcherInterface> watcher);

	virtual std::unique_ptr<FileExplorerServiceInterface> Clone() const override;

	virtual std::string Echo(const char* str) override;

private:
	std::shared_ptr<FileExplorerWatcherInterface> watcher_;
};

/*****************************************************************************/
/****************************** INLINE FUNCTIONS *****************************/
/*****************************************************************************/
inline FileExplorerService::FileExplorerService(
	std::unique_ptr<FileExplorerWatcherInterface> watcher)
	: watcher_(std::move(watcher))
{
}

inline std::unique_ptr<FileExplorerServiceInterface> FileExplorerService::Clone() const
{
	return std::make_unique<FileExplorerService>(*this);
}

} // namespace server
} // namespace remoteFileExplorer