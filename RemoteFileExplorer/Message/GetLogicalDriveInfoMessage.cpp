#include "Message/GetLogicalDriveInfoMessage.h"

#include "Message/Serialization.h"
#include "Utils/Utils.h"

namespace remoteFileExplorer
{
namespace message
{
///////////////////////////////////////////////////////////////////////////////
int GetLogicalDriveInfoRequest::Serialize(std::uint8_t* buffer, std::size_t* bufferSize)
{
	if (ClientMessage::Serialize(buffer, bufferSize) != 0)
		return -1;

	// Nothing to serialize.

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
int GetLogicalDriveInfoReply::Serialize(std::uint8_t* buffer, std::size_t* bufferSize)
{
	std::size_t givenBufferSize = *bufferSize;
	std::size_t preSerializedSize = givenBufferSize;

	if (ServerMessage::Serialize(buffer, &preSerializedSize) != 0)
		return -1;

	std::size_t remainedBufferSize = givenBufferSize - preSerializedSize;
	buffer += preSerializedSize;

	// Put drive's letter.
	{
		if (remainedBufferSize < 1)
			return -1;

		*(buffer++) = drive_.letter;
		--remainedBufferSize;
	}

	// Put drive's name.
	{
		std::string u8_driveName = utils::wstring_to_utf8(drive_.driveName);

		if (SerializeString(buffer, remainedBufferSize, u8_driveName) != 0)
			return -1;
	}
	
	// Return the size of serialized data as bufferSize.
	*bufferSize = givenBufferSize - remainedBufferSize;

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
/*static*/ std::unique_ptr<GetLogicalDriveInfoRequest>
GetLogicalDriveInfoRequest::Deserialize(
	const std::uint8_t* buffer,
	std::size_t bufferSize)
{
	if (bufferSize != 0)
		return nullptr;

	return std::make_unique<GetLogicalDriveInfoRequest>();
}

///////////////////////////////////////////////////////////////////////////////
/*static*/ std::unique_ptr<GetLogicalDriveInfoReply>
GetLogicalDriveInfoReply::Deserialize(
	const std::uint8_t* buffer,
	std::size_t bufferSize)
{
	common::LogicalDrive drive;

	// Get drive's letter.
	{
		if (bufferSize < 1)
			return nullptr;
		drive.letter = *(buffer++);
		--bufferSize;
	}

	// Get drive's name.
	{
		std::string u8_driveName;

		if (DeserializeString(buffer, bufferSize, u8_driveName) != 0)
			return nullptr;

		drive.driveName = utils::utf8_to_wstring(std::move(u8_driveName));
	}

	if (bufferSize != 0)
		return nullptr;

	return std::make_unique<GetLogicalDriveInfoReply>(std::move(drive));
}

} // namespace message
} // namespace remoteFileExplorer