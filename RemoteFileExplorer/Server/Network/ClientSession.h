#pragma once

#include <winsock2.h>

#include <memory>

#include "Server/FileExplorerServiceInterface.h"

namespace remoteFileExplorer
{
namespace server
{
namespace network
{
///////////////////////////////////////////////////////////////////////////////
class ClientSession final
{
public:
	ClientSession(
		SOCKET hSocket,
		SOCKADDR_IN address,
		std::unique_ptr<FileExplorerServiceInterface> fileExplorerService);

	SOCKET GetSocketHandle() { return hSocket_; }
	common::FileExplorerInterface& GetFileExplorer() { return *fileExplorerService_; }

private:
	SOCKET hSocket_;
	SOCKADDR_IN address_;
	std::unique_ptr<FileExplorerServiceInterface> fileExplorerService_;
};

/*****************************************************************************/
/****************************** INLINE FUNCTIONS *****************************/
/*****************************************************************************/
inline ClientSession::ClientSession(
	SOCKET hSocket,
	SOCKADDR_IN address,
	std::unique_ptr<FileExplorerServiceInterface> fileExplorerService)
	: hSocket_(hSocket),
	  address_(address),
	  fileExplorerService_(std::move(fileExplorerService))
{
}

} // namespace network
} // namespace server
} // namespace remoteFileExplorer