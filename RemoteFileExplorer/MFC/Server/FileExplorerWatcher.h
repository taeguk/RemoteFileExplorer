#pragma once

#include <windows.h>

#include "Server/FileExplorerWatcherInterface.h"

namespace remoteFileExplorer
{
namespace mfc
{
namespace server
{
///////////////////////////////////////////////////////////////////////////////
// File Explorer 서비스가 실행됬을 때, 그 것을 등록된 윈도우에,
//   Window Message를 Post한다.
class FileExplorerWatcher
    : public remoteFileExplorer::server::FileExplorerWatcherInterface
{
public:
    // Window Message 번호에 대해, base로 부터의 offset을 의미.
    enum
    {
        WmOffsetGetLogicalDriveInfo = 0,
        WmOffsetGetDirectoryInfo,

        WmUpperOffset
    };

    FileExplorerWatcher(HWND hWindow, UINT wmBase)
        : hWindow_(hWindow), wmBase_(wmBase) {}

    virtual void GetLogicalDriveInfo() override;
    virtual void GetDirectoryInfo(
        const std::wstring& path,
        common::file_count_t offset) override;

private:
    HWND hWindow_;
    UINT wmBase_;
};

} // namespace server
} // namespace mfc
} // namespace remoteFileExplorer