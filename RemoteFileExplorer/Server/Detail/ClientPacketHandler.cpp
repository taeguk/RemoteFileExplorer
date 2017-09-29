#include "Server/Detail/ClientPacketHandler.h"

#include "Message/EchoMessage.h"

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
	std::size_t maxBufferSize)
{
	// Get message by deserializing packet.
	std::unique_ptr<message::Message> message =
		message::Message::Deserialize(buffer, *bufferSize);

	// Process only correct client message.
	// Don't process invalid packet or server message.
	if (message == nullptr || !message->IsClientMessage())
		return -1;

	message::ClientMessage& clientMessage =
		reinterpret_cast<message::ClientMessage&>(*message);

	std::unique_ptr<message::ServerMessage> serverMessage = nullptr;

	if (detail::HandleClientMessage(session, clientMessage, serverMessage) != 0)
		return -1;

	*bufferSize = maxBufferSize;
	if (serverMessage->Serialize(buffer, bufferSize) != 0)
		return -1;

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
int HandleClientMessage(
	ClientSession& session,
	message::ClientMessage& clientMessage,
	std::unique_ptr<message::ServerMessage>& serverMessage)
{
	using namespace message;

	switch (clientMessage.GetMessageFlag())
	{
	case MessageFlag::EchoRequest:
	{
		EchoRequestMessage& message =
			EchoRequestMessage::TypeCastFromMessage(clientMessage);

		common::FileExplorerInterface& fileExplorer = session.GetFileExplorer();
		std::string result = fileExplorer.Echo(message.GetString().c_str());

		serverMessage = std::make_unique<EchoReplyMessage>(std::move(result));
	}
	break;
	default:
		return -1;
	}

	return 0;
}

} // namespace detail
} // namespace server
} // namespace remoteFileExplorer