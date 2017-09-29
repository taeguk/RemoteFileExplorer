#pragma once

#include "Server/Detail/ClientSession.h"
#include "Message/Message.h"

namespace remoteFileExplorer
{
namespace server
{
namespace detail
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

} // namespace detail
} // namespace server
} // namespace remoteFileExplorer