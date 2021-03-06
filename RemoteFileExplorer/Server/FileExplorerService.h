#pragma once

#include <memory>
#include <mutex>

#include "Server/FileExplorerServiceInterface.h"
#include "Server/FileExplorerWatcherInterface.h"

namespace remoteFileExplorer
{
namespace server
{
///////////////////////////////////////////////////////////////////////////////
// 파일 탐색기 서비스를 제공한다.
// 실제로 로컬에서 파일 탐색기에 관련된 기능들을 수행하는 역할을 한다.
// Thread Safe
class FileExplorerService : public FileExplorerServiceInterface
{
public:
    FileExplorerService(std::unique_ptr<FileExplorerWatcherInterface> watcher);
    FileExplorerService(const FileExplorerService& other);

    virtual std::unique_ptr<FileExplorerServiceInterface> Clone() const override;

    virtual int GetLogicalDriveInfo(
        std::vector<common::LogicalDrive>& drives) override;
    virtual int GetDirectoryInfo(
        const std::wstring& path,
        common::file_count_t offset,
        common::Directory& dir) override;

private:
    mutable std::mutex mutex_;  // cachedDir_에 대한 동기화를 위해 사용된다.
    common::Directory cachedDir_{};
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

inline FileExplorerService::FileExplorerService(const FileExplorerService& other)
{
    std::lock_guard<decltype(other.mutex_)> lk(other.mutex_);
    cachedDir_ = other.cachedDir_;
    watcher_ = other.watcher_;
}

inline std::unique_ptr<FileExplorerServiceInterface> FileExplorerService::Clone() const
{
    return std::make_unique<FileExplorerService>(*this);
}

} // namespace server
} // namespace remoteFileExplorer