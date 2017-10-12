#include "Message/Message.h"

#include "Message/GetLogicalDriveInfoMessage.h"
#include "Message/GetDirectoryInfoMessage.h"
#include "Message/Serialization.h"

namespace remoteFileExplorer
{
namespace message
{
///////////////////////////////////////////////////////////////////////////////
int Message::Serialize(std::uint8_t* buffer, std::size_t* bufferSize)
{
	std::size_t givenBufferSize = *bufferSize;

	if (SerializeWithMemcpy(buffer, *bufferSize, messageFlag_) != 0)
		return -1;

	// Return the size of serialized data as bufferSize.
	*bufferSize = givenBufferSize - *bufferSize;

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
/*static*/ std::unique_ptr<Message> Message::Deserialize(
	const std::uint8_t* buffer,
	std::size_t bufferSize)
{
	MessageFlag messageFlag;

	if (DeserializeWithMemcpy(buffer, bufferSize, messageFlag) != 0)
		return nullptr;

	switch (messageFlag)
	{
	case MessageFlag::GetLogicalDriveInfoRequest:
		return GetLogicalDriveInfoRequest::Deserialize(buffer, bufferSize);
		break;
	case MessageFlag::GetLogicalDriveInfoReply:
		return GetLogicalDriveInfoReply::Deserialize(buffer, bufferSize);
		break;
	case MessageFlag::GetDirectoryInfoRequest:
		return GetDirectoryInfoRequest::Deserialize(buffer, bufferSize);
		break;
	case MessageFlag::GetDirectoryInfoReply:
		return GetDirectoryInfoReply::Deserialize(buffer, bufferSize);
		break;
	default:
		return nullptr;
	}
}

} // namespace message
} // namespace remoteFileExplorer
