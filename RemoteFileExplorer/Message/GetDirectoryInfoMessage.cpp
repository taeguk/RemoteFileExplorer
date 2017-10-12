#include "Message/GetDirectoryInfoMessage.h"

#include "Message/Serialization.h"
#include "Utils/Utils.h"

namespace remoteFileExplorer
{
namespace message
{
///////////////////////////////////////////////////////////////////////////////
namespace /*unnamed*/
{
using file_count_t = std::uint32_t;
const file_count_t FileCountMax = UINT32_MAX;
} // unnamed namespace

///////////////////////////////////////////////////////////////////////////////
int GetDirectoryInfoRequest::Serialize(std::uint8_t* buffer, std::size_t* bufferSize)
{
	std::size_t givenBufferSize = *bufferSize;
	std::size_t preSerializedSize = givenBufferSize;

	if (ClientMessage::Serialize(buffer, &preSerializedSize) != 0)
		return -1;

	std::size_t remainedBufferSize = givenBufferSize - preSerializedSize;
	buffer += preSerializedSize;

	// Put directory's path.
	{
		std::string u8_path = utils::wstring_to_utf8(path_);

		if (SerializeString(buffer, remainedBufferSize, u8_path) != 0)
			return -1;
	}

	// Put start offset for getting files in directory.
	{
		if (SerializeWithMemcpy(
			buffer,
			remainedBufferSize,
			offset_) != 0)
		{
			return -1;
		}
	}
	
	// Return the size of serialized data as bufferSize.
	*bufferSize = givenBufferSize - remainedBufferSize;

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
int GetDirectoryInfoReply::Serialize(std::uint8_t* buffer, std::size_t* bufferSize)
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
		// Put directory's path.
		{
			std::string u8_path = utils::wstring_to_utf8(dir_.path);

			if (SerializeString(buffer, remainedBufferSize, u8_path) != 0)
				return -1;
		}

		// Put the number of files in a directory.
		{
			auto numFileInfos = dir_.fileInfos.size();

			if (numFileInfos > FileCountMax)
				return -1;

			if (SerializeWithMemcpy(
				buffer,
				remainedBufferSize,
				static_cast<file_count_t>(numFileInfos)) != 0)
			{
				return -1;
			}
		}

		// Put directory's file informations.
		for (const auto& f : dir_.fileInfos)
		{
			// Put file's name.
			{
				std::string u8_name = utils::wstring_to_utf8(f.fileName);

				if (SerializeString(buffer, remainedBufferSize, u8_name) != 0)
					return -1;
			}

			// TODO: 현재 serialization이 "memory endian 방식이 같은 system 끼리만"
			//       호환되는 잠재적 issue 존재. (다른 곳도)
			// Put file's type.
			if (SerializeWithMemcpy(buffer, remainedBufferSize, f.fileType) != 0)
				return -1;

			// Put file's attribute.
			if (SerializeWithMemcpy(buffer, remainedBufferSize, f.fileAttr) != 0)
				return -1;

			// Put file's modified date.
			if (SerializeWithMemcpy(buffer, remainedBufferSize, f.modifiedDate) != 0)
				return -1;

			// Put file's size.
			if (SerializeWithMemcpy(buffer, remainedBufferSize, f.fileSize) != 0)
				return -1;
		}
	}
	
	// Return the size of serialized data as bufferSize.
	*bufferSize = givenBufferSize - remainedBufferSize;

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
/*static*/ std::unique_ptr<GetDirectoryInfoRequest>
GetDirectoryInfoRequest::Deserialize(
	const std::uint8_t* buffer,
	std::size_t bufferSize)
{
	std::string u8_path;
	std::uint32_t offset;

	if (DeserializeString(buffer, bufferSize, u8_path) != 0)
		return nullptr;

	// Get start offset for getting files in directory.
	if (DeserializeWithMemcpy(buffer, bufferSize, offset) != 0)
		return nullptr;

	if (bufferSize != 0)
		return nullptr;

	return std::make_unique<GetDirectoryInfoRequest>(
		utils::utf8_to_wstring(std::move(u8_path)),
		offset);
}

///////////////////////////////////////////////////////////////////////////////
/*static*/ std::unique_ptr<GetDirectoryInfoReply>
GetDirectoryInfoReply::Deserialize(
	const std::uint8_t* buffer,
	std::size_t bufferSize)
{
	std::int8_t statusCode;
	common::Directory dir;

	// Get the status code.
	if (DeserializeWithMemcpy(buffer, bufferSize, statusCode) != 0)
		return nullptr;

	if (statusCode == 0)
	{
		// Get directory's path.
		{
			std::string u8_path;

			if (DeserializeString(buffer, bufferSize, u8_path) != 0)
				return nullptr;

			dir.path = utils::utf8_to_wstring(std::move(u8_path));
		}

		// Get the count of files in a directory.
		file_count_t numFileInfos;
		{
			if (DeserializeWithMemcpy(buffer, bufferSize, numFileInfos) != 0)
				return nullptr;

			dir.fileInfos.reserve(numFileInfos);
		}

		// Get directory's file informations.
		for (file_count_t i = 0; i < numFileInfos; ++i)
		{
			common::FileInformation f;

			// Get file's name.
			{
				std::string u8_name;

				if (DeserializeString(buffer, bufferSize, u8_name) != 0)
					return nullptr;

				f.fileName = utils::utf8_to_wstring(std::move(u8_name));
			}

			// TODO: 현재 serialization이 "memory endian 방식이 같은 system 끼리만"
			//       호환되는 잠재적 issue 존재. (다른 곳도)
			// Get file's type.
			if (DeserializeWithMemcpy(buffer, bufferSize, f.fileType) != 0)
				return nullptr;

			// Get file's attribute.
			if (DeserializeWithMemcpy(buffer, bufferSize, f.fileAttr) != 0)
				return nullptr;

			// Get file's modified date.
			if (DeserializeWithMemcpy(buffer, bufferSize, f.modifiedDate) != 0)
				return nullptr;

			// Get file's size.
			if (DeserializeWithMemcpy(buffer, bufferSize, f.fileSize) != 0)
				return nullptr;

			dir.fileInfos.push_back(std::move(f));
		}
	}

	if (bufferSize != 0)
		return nullptr;

	return std::make_unique<GetDirectoryInfoReply>(statusCode, std::move(dir));
}

} // namespace message
} // namespace remoteFileExplorer