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
int HandleClientPacket(
	ClientSession& session,
	const std::uint8_t* recvBuffer,
	std::size_t recvBufferSize,
	std::uint8_t* sendBuffer,
	std::size_t* sendBufferSize);

///////////////////////////////////////////////////////////////////////////////
std::unique_ptr<message::ServerMessage> HandleClientMessage(
	ClientSession& session,
	message::ClientMessage& clientMessage);

} // namespace network
} // namespace server
} // namespace remoteFileExplorer