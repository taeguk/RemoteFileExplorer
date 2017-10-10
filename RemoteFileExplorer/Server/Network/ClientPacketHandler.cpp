#include "Server/Network/ClientPacketHandler.h"

#include "Message/GetLogicalDriveInfoMessage.h"
#include "Message/GetDirectoryInfoMessage.h"

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

	std::unique_ptr<message::ServerMessage> serverMessage =
		HandleClientMessage(session, clientMessage);

	if (serverMessage == nullptr)
		return -1;

	*bufferSize = maxBufferSize;
	if (serverMessage->Serialize(buffer, bufferSize) != 0)
		return -1;

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
std::unique_ptr<message::ServerMessage> HandleClientMessage(
	ClientSession& session,
	message::ClientMessage& clientMessage)
{
	using namespace message;

	switch (clientMessage.GetMessageFlag())
	{
	case MessageFlag::GetLogicalDriveInfoRequest:
	{
		GetLogicalDriveInfoRequest& message =
			GetLogicalDriveInfoRequest::TypeCastFromMessage(clientMessage);

		common::FileExplorerInterface& fileExplorer = session.GetFileExplorer();
		std::vector<common::LogicalDrive> drives;

		if (fileExplorer.GetLogicalDriveInfo(drives) != 0)
			return nullptr;

		return std::make_unique<GetLogicalDriveInfoReply>(std::move(drives));
	}
	break;
	case MessageFlag::GetDirectoryInfoRequest:
	{
		GetDirectoryInfoRequest& message =
			GetDirectoryInfoRequest::TypeCastFromMessage(clientMessage);

		common::FileExplorerInterface& fileExplorer = session.GetFileExplorer();
		common::Directory dir;

		if (fileExplorer.GetDirectoryInfo(message.GetPathRef(), dir) != 0)
			return nullptr;

		return std::make_unique<GetDirectoryInfoReply>(std::move(dir));
	}
	break;
	default:
		return nullptr;
	}
}

} // namespace network
} // namespace server
} // namespace remoteFileExplorer