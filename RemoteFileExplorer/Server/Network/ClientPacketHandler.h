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
int HandleClientMessage(
	ClientSession& session,
	message::ClientMessage& clientMessage,
	std::unique_ptr<message::ServerMessage>& serverMessage);

} // namespace network
} // namespace server
} // namespace remoteFileExplorer