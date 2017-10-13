#include "Message/GetLogicalDriveInfoMessage.h"

#include "Common/CommonType.h"
#include "Message/Serialization.h"
#include "Utils/Utils.h"

namespace remoteFileExplorer
{
namespace message
{
///////////////////////////////////////////////////////////////////////////////
namespace /*unnamed*/
{
using drive_count_t = std::uint8_t;
const drive_count_t DriveCountMax = UINT8_MAX;
} // unnamed namespace

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

	// Put the status code.
	{
		if (SerializeWithMemcpy(
			buffer,
			remainedBufferSize,
			statusCode_) != 0)
		{
			return -1;
		}
	}

	// 정상일때만,
	if (statusCode_ == 0)
	{
		// Put the number of drives.
		{
			auto numDrives = drives_.size();

			if (numDrives > DriveCountMax)
				return -1;

			if (SerializeWithMemcpy(
				buffer,
				remainedBufferSize,
				static_cast<drive_count_t>(numDrives)) != 0)
			{
				return -1;
			}
		}

		// Put drives' informations.
		for (const auto& drive : drives_)
		{
			// Put drive's letter.
			{
				if (remainedBufferSize < 1)
					return -1;

				*(buffer++) = drive.letter;
				--remainedBufferSize;
			}

			// Put drive's name.
			{
				std::string u8_driveName = utils::wstring_to_utf8(drive.driveName);

				if (SerializeString(buffer, remainedBufferSize, u8_driveName) != 0)
					return -1;
			}
		}
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
	common::status_code_t statusCode;
	std::vector<common::LogicalDrive> drives;

	// Get the status code.
	if (DeserializeWithMemcpy(buffer, bufferSize, statusCode) != 0)
		return nullptr;

	if (statusCode == 0)
	{
		// Get the count of files in a directory.
		drive_count_t numDrives;
		{
			if (DeserializeWithMemcpy(buffer, bufferSize, numDrives) != 0)
				return nullptr;

			drives.reserve(numDrives);
		}

		for (drive_count_t i = 0; i < numDrives; ++i)
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

			drives.push_back(std::move(drive));
		}
	}

	if (bufferSize != 0)
		return nullptr;

	return std::make_unique<GetLogicalDriveInfoReply>(statusCode, std::move(drives));
}

} // namespace message
} // namespace remoteFileExplorer