#include "Message/Message.h"

#include "Message/EchoMessage.h"

namespace remoteFileExplorer
{
namespace message
{
///////////////////////////////////////////////////////////////////////////////
int Message::Serialize(std::uint8_t* buffer, std::size_t* bufferSize)
{
	const std::size_t requiredSize = sizeof(_MessageFlag);
	
	if (*bufferSize < requiredSize)
		return -1;

	std::uint8_t rawMessageFlag = utils::to_underlying(_MessageFlag);
	*buffer = rawMessageFlag;
	*bufferSize = requiredSize;

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
/*static*/ std::unique_ptr<Message> Message::Deserialize(
	const std::uint8_t* buffer,
	std::size_t bufferSize)
{
	MessageFlag messageFlag = static_cast<MessageFlag>(*buffer);
	++buffer;
	--bufferSize;

	switch (messageFlag)
	{
	case MessageFlag::EchoRequest:
		return EchoRequestMessage::Deserialize(buffer, bufferSize);
		break;
	case MessageFlag::EchoReply:
		return EchoReplyMessage::Deserialize(buffer, bufferSize);
		break;
	default:
		return nullptr;
	}
}

} // namespace message
} // namespace remoteFileExplorer
