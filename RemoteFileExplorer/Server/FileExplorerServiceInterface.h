#pragma once

#include <string>
#include <memory>
#include "Common/FileExplorerInterface.h"

namespace remoteFileExplorer
{
namespace server
{

class FileExplorerServiceInterface : public common::FileExplorerInterface
{
public:
	virtual std::unique_ptr<FileExplorerServiceInterface> Clone() const = 0;
};

} // namespace server
} // namespace remoteFileExplorer