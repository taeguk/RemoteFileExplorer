#pragma once

#include <string>

#include "Common/CommonType.h"

namespace remoteFileExplorer
{
namespace server
{
///////////////////////////////////////////////////////////////////////////////
// 파일 탐색기 서비스가 실행 될때, 실행과정을 추적하기 위한 인터페이스이다.
// 파일 탐색기 서비스는 요청을 수행하면서, 그 실행 기록을
//   이 인터페이스 객체를 통해서 전달한다.
class FileExplorerWatcherInterface
{
public:
    virtual ~FileExplorerWatcherInterface() = default;

    virtual void GetLogicalDriveInfo() = 0;
    virtual void GetDirectoryInfo(
        const std::wstring& path,
        common::file_count_t offset) = 0;
};

} // namespace server
} // namespace remoteFileExplorer