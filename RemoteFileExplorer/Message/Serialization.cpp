#include "Message/Serialization.h"

namespace remoteFileExplorer
{
namespace message
{
///////////////////////////////////////////////////////////////////////////////
namespace /*unnamed*/
{
using string_len_t = std::uint16_t;
const string_len_t StringLenMax = UINT16_MAX;
} // unnamed namespace

///////////////////////////////////////////////////////////////////////////////
int SerializeString(
	std::uint8_t*& buffer,
	std::size_t& remainedBufferSize,
	const std::string& str)
{
	std::size_t bytes = str.length();

	if (bytes > StringLenMax)
		return -1;

	// Put the length of the string.
	{
		if (remainedBufferSize < sizeof(string_len_t))
			return -1;

		*reinterpret_cast<string_len_t*>(buffer) =
			static_cast<string_len_t>(bytes);

		buffer += sizeof(string_len_t);
		remainedBufferSize -= sizeof(string_len_t);
	}

	// Put the string.
	{
		if (remainedBufferSize < bytes)
			return -1;

		std::memcpy(buffer, str.c_str(), bytes);

		buffer += bytes;
		remainedBufferSize -= bytes;
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
int DeserializeString(
	const std::uint8_t*& buffer,
	std::size_t& remainedBufferSize,
	std::string& str)
{
	string_len_t length;

	// Get the length of the string.
	{
		if (remainedBufferSize < sizeof(string_len_t))
			return -1;

		length = *reinterpret_cast<const string_len_t*&>(buffer);

		buffer += sizeof(string_len_t);
		remainedBufferSize -= sizeof(string_len_t);
	}

	// Get the string.
	{
		if (remainedBufferSize < length)
			return -1;

		str.clear();
		str.reserve(length);
		str.assign(buffer, buffer + length);

		buffer += length;
		remainedBufferSize -= length;
	}

	return 0;
}

} // namespace message
} // namespace remoteFileExplorer