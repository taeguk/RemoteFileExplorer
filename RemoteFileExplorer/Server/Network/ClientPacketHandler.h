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
	std::uint8_t* buffer,
	std::size_t* bufferSize,
	std::size_t maxBufferSize);

///////////////////////////////////////////////////////////////////////////////
std::unique_ptr<message::ServerMessage> HandleClientMessage(
	ClientSession& session,
	message::ClientMessage& clientMessage);

} // namespace network
} // namespace server
} // namespace remoteFileExplorer