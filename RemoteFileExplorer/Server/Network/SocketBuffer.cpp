#include "Server/Network/SocketBuffer.h"

namespace remoteFileExplorer
{
namespace server
{
namespace network
{
///////////////////////////////////////////////////////////////////////////////
/*static*/ SocketBuffer* SocketBuffer::AcquireBuffer()
{
	SocketBuffer* socketBuffer = new SocketBuffer;
	socketBuffer->Reset();
	return socketBuffer;
}

///////////////////////////////////////////////////////////////////////////////
/*static*/ int SocketBuffer::ReleaseBuffer(SocketBuffer* socketBuffer)
{
	delete socketBuffer;
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
void SocketBuffer::Reset()
{
	memset(&overlapped, 0, sizeof(overlapped));
	wsabuf.buf = reinterpret_cast<char*>(buffer);
	wsabuf.len = MaxBufferSize;
}

} // namespace network
} // namespace server
} // namespace remoteFileExplorer