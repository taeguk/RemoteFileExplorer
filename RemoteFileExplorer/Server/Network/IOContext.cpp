#include "Server/Network/IOContext.h"

namespace remoteFileExplorer
{
namespace server
{
namespace network
{
///////////////////////////////////////////////////////////////////////////////
void IORecvContext::ConsumeBuffer(std::size_t consumedBytes)
{
    assert(consumedBytes <= filledBufferSize_);

    filledBufferSize_ -= consumedBytes;

    for (std::size_t i = 0u, j = consumedBytes;
        i < filledBufferSize_;
        ++i, ++j)
    {
        buffer_[i] = buffer_[j];
    }
}

} // namespace network
} // namespace server
} // namespace remoteFileExplorer