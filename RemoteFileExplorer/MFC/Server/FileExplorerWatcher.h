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
class FileExplorerWatcher
	: public remoteFileExplorer::server::FileExplorerWatcherInterface
{
public:
	enum
	{
		WmOffsetEcho = 0,

		WmUpperOffset
	};

	FileExplorerWatcher(HWND hWindow, UINT wmBase) : hWindow_(hWindow), wmBase_(wmBase) {}

	virtual void Echo(const char* str) override;

private:
	HWND hWindow_;
	UINT wmBase_;
};

} // namespace server
} // namespace mfc
} // namespace remoteFileExplorer