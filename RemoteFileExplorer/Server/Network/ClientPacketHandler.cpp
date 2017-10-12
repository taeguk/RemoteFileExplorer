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
	const std::uint8_t* recvBuffer,
	std::size_t recvBufferSize,
	std::uint8_t* sendBuffer,
	std::size_t* sendBufferSize)
{
	// Get message by deserializing packet.
	std::unique_ptr<message::Message> message =
		message::Message::Deserialize(recvBuffer, recvBufferSize);

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

	if (serverMessage->Serialize(sendBuffer, sendBufferSize) != 0)
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

		std::int8_t statusCode = 0;
		std::vector<common::LogicalDrive> drives;

		if (fileExplorer.GetLogicalDriveInfo(drives) != 0)
			statusCode = -1;

		return std::make_unique<GetLogicalDriveInfoReply>(statusCode, std::move(drives));
	}
	break;
	case MessageFlag::GetDirectoryInfoRequest:
	{
		GetDirectoryInfoRequest& message =
			GetDirectoryInfoRequest::TypeCastFromMessage(clientMessage);

		common::FileExplorerInterface& fileExplorer = session.GetFileExplorer();

		std::int8_t statusCode = 0;
		common::Directory dir;

		if (fileExplorer.GetDirectoryInfo(
			message.GetPathRef(),
			message.GetOffset(),
			dir) != 0)
		{
			statusCode = -1;
		}

		return std::make_unique<GetDirectoryInfoReply>(statusCode, std::move(dir));
	}
	break;
	default:
		return nullptr;
	}
}

} // namespace network
} // namespace server
} // namespace remoteFileExplorer