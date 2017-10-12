#pragma once

#include <winsock2.h>

#include <memory>
#include <cassert>
#include <vector>

#include "Server/FileExplorerServiceInterface.h"

namespace remoteFileExplorer
{
namespace server
{
namespace network
{
///////////////////////////////////////////////////////////////////////////////
class ClientSession final : private OVERLAPPED
{
public:
	ClientSession(
		SOCKET hSocket,
		SOCKADDR_IN address,
		std::unique_ptr<FileExplorerServiceInterface> fileExplorerService);

	SOCKET GetSocketHandle() { return hSocket_; }
	common::FileExplorerInterface& GetFileExplorer() { return *fileExplorerService_; }

	void UpdateReceiveBuffer(std::size_t receivedBytes)
	{
		filledBufferSize_ += receivedBytes;
	}

	std::pair<std::uint8_t*, std::size_t> GetReceiveBufferStatus()
	{
		return std::make_pair(buffer_, filledBufferSize_);
	}

	void ConsumeReceiveBuffer(std::size_t consumedBytes)
	{
		assert(consumedBytes <= filledBufferSize_);

		filledBufferSize_ -= consumedBytes;
		for (std::size_t i = 0u, j = consumedBytes; i < filledBufferSize_; ++i, ++j)
		{
			buffer_[i] = buffer_[j];
		}
	}

	WSABUF& GetUpdatedWsabufRef()
	{
		wsabuf_.buf = reinterpret_cast<char*>(buffer_ + filledBufferSize_);
		wsabuf_.len = MaxBufferSize_ - filledBufferSize_;
		return wsabuf_;
	}

	bool ReceiveBufferIsFull() const { return filledBufferSize_ >= MaxBufferSize_; }

	void ResetReceiveBuffer()
	{
		filledBufferSize_ = 0;
	}

	OVERLAPPED& GetInitializedOVELAPPED()
	{
		OVERLAPPED& overlapped = *this;
		memset(&overlapped, 0, sizeof(overlapped));
		return overlapped;
	}

private:
	static const std::size_t MaxBufferSize_{ 32 * 1024 };

	WSABUF wsabuf_;
	std::uint8_t buffer_[MaxBufferSize_];
	std::size_t filledBufferSize_{ 0 };

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