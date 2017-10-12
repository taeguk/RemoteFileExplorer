// TODO: #ifndef 식으로 바꾸기 (다른곳도)
#pragma once

#include <ctime>
#include <string>
#include <vector>

#include "Common/FileSystem.h"

namespace remoteFileExplorer
{
namespace common
{
///////////////////////////////////////////////////////////////////////////////
class FileExplorerInterface
{
public:
	virtual ~FileExplorerInterface() = default;

	virtual int GetLogicalDriveInfo(std::vector<LogicalDrive>& drives) = 0;
	virtual int GetDirectoryInfo(
		const std::wstring& path,
		std::uint32_t offset,
		Directory& dir) = 0;
};

} // namespace common
} // namespace remoteFileExplorer