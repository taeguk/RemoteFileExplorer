#pragma once

#include <ctime>
#include <string>
#include <vector>

#include "Utils/Utils.h"

namespace remoteFileExplorer
{
namespace server
{
namespace fileSystem
{
///////////////////////////////////////////////////////////////////////////////
enum class FileType
{
    File,
    Directory,
    Unknown
};

///////////////////////////////////////////////////////////////////////////////
enum class FileAttribute
{
    NoFlag = 0,
    Hidden = 0x1,
    System = 0x2
};

inline FileAttribute operator|(FileAttribute lhs, FileAttribute rhs)
{
    return static_cast<FileAttribute>(
		utils::to_underlying(lhs) | utils::to_underlying(rhs));
}

inline FileAttribute& operator|=(FileAttribute& lhs, FileAttribute rhs)
{
    lhs = static_cast<FileAttribute>(
		utils::to_underlying(lhs) | utils::to_underlying(rhs));
    return lhs;
}

inline FileAttribute operator&(FileAttribute lhs, FileAttribute rhs)
{
    return static_cast<FileAttribute>(
		utils::to_underlying(lhs) & utils::to_underlying(rhs));
}

inline FileAttribute& operator&=(FileAttribute lhs, FileAttribute rhs)
{
    lhs = static_cast<FileAttribute>(
		utils::to_underlying(lhs) & utils::to_underlying(rhs));
    return lhs;
}

///////////////////////////////////////////////////////////////////////////////
struct FileInformation
{
    std::wstring fileName;
    FileType fileType;
    FileAttribute fileAttr;
    std::time_t modifiedDate;
    std::uintmax_t fileSize;
};

///////////////////////////////////////////////////////////////////////////////
struct Directory
{
    std::wstring path;
    std::vector<FileInformation> fileInfos;
};

///////////////////////////////////////////////////////////////////////////////
struct LogicalDrive
{
    wchar_t letter;
    std::wstring driveName;
};

///////////////////////////////////////////////////////////////////////////////
int GetDirectoryInfo(const std::wstring& path, Directory& dir);

///////////////////////////////////////////////////////////////////////////////
int GetLogicalDriveInfo(std::vector<LogicalDrive>& drives);

} // namespace fileSystem
} // namespace server
} // namespace remoteFileExplorer