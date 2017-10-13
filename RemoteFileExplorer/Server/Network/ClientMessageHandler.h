#pragma once

#include "Server/Network/ClientSession.h"
#include "Message/Message.h"

namespace remoteFileExplorer
{
namespace server
{
namespace network
{
///////////////////////////////////////////////////////////////////////////////
// Client에게서 온 message를 처리한다.
// 그리고 그 처리 결과에 해당하는 message를 sendBuffer에 담아서 반환한다.
int HandleClientMessage(
    ClientSession& session,
    const std::uint8_t* recvBuffer,
    std::size_t recvBufferSize,
    std::uint8_t* sendBuffer,
    std::size_t& sendBufferSize);

///////////////////////////////////////////////////////////////////////////////
// Client에게서 온 message를 처리하는 또 다른 overload 버전.
// 이 버전은 buffer 형태가 아닌 좀 더 구체화된 message 객체 형태로 사용된다.
std::unique_ptr<message::ServerMessage> HandleClientMessage(
    ClientSession& session,
    message::ClientMessage& clientMessage);

} // namespace network
} // namespace server
} // namespace remoteFileExplorer