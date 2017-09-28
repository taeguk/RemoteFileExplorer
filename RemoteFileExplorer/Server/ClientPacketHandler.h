#pragma once

#include "Server/ClientSession.h"
#include "Message/Message.h"

namespace remoteFileExplorer
{
namespace server
{
int HandleClientPacket(
	ClientSession& session,
	std::uint8_t* buffer,
	std::size_t* bufferSize,
	std::size_t maxBufferSize);

namespace detail
{
int HandleClientMessage(
	ClientSession& session,
	message::ClientMessage& clientMessage,
	std::unique_ptr<message::ServerMessage>& serverMessage);
} // namespace detail

} // namespace server
} // namespace remoteFileExplorer