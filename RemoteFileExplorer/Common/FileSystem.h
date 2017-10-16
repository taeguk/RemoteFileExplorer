#pragma once

#include <ctime>
#include <string>
#include <vector>

#include "Utils/Utils.h"

namespace remoteFileExplorer
{
namespace common
{
///////////////////////////////////////////////////////////////////////////////
enum class FileType : std::uint8_t
{
    Directory,
    File,
    Unknown
};

///////////////////////////////////////////////////////////////////////////////
enum class FileAttribute : std::uint8_t
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
    std::uintmax_t fileSize;  // DirectoryÀÇ °æ¿ì 0.
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
    char letter;
    std::wstring driveName;
};

} // namespace common
} // namespace remoteFileExplorer