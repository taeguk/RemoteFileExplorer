#pragma once

#include <winsock2.h>

#include <cstddef>
#include <cstdint>

namespace remoteFileExplorer
{
namespace server
{
namespace network
{
///////////////////////////////////////////////////////////////////////////////
struct SocketBuffer final
{
	static const std::size_t MaxBufferSize = 1024; // TODO: 자동으로 조정방법?

	static SocketBuffer* AcquireBuffer();
	static int ReleaseBuffer(SocketBuffer* socketBuffer);

	void Reset();

	OVERLAPPED overlapped;  // 반드시 맨앞에 와야함.
	std::uint8_t buffer[MaxBufferSize];
	WSABUF wsabuf;
};

} // namespace network
} // namespace server
} // namespace remoteFileExplorer