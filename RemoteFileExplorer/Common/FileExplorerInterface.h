// TODO: #ifndef 식으로 바꾸기 (다른곳도)
#pragma once

#include <string>

namespace remoteFileExplorer
{
namespace common
{

class FileExplorerInterface
{
public:
	virtual ~FileExplorerInterface() = default;

	// 테스트용도.
	virtual std::string Echo(const char* str) = 0;
};

} // namespace common
} // namespace remoteFileExplorer