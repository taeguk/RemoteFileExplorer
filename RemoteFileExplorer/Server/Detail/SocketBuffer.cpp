#include "Server/Detail/SocketBuffer.h"

namespace remoteFileExplorer
{
namespace server
{
namespace detail
{
///////////////////////////////////////////////////////////////////////////////
/*static*/ SocketBuffer* SocketBuffer::AcquireBuffer()
{
	SocketBuffer* socketBuffer = new SocketBuffer;
	memset(&(socketBuffer->overlapped), 0, sizeof(socketBuffer->overlapped));
	socketBuffer->wsabuf.len = MaxBufferSize;
	socketBuffer->wsabuf.buf = (char*)socketBuffer->buffer;
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
	wsabuf.len = 1024;
}

} // namespace detail
} // namespace server
} // namespace remoteFileExplorer