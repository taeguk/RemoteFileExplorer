#pragma once

#include <string>

#include "Message/Message.h"
#include "Common/CommonType.h"
#include "Common/FileSystem.h"

namespace remoteFileExplorer
{
namespace message
{
///////////////////////////////////////////////////////////////////////////////
class GetDirectoryInfoRequest : public ClientMessage
{
public:
	explicit GetDirectoryInfoRequest(std::wstring&& path, common::file_count_t offset);

	virtual int Serialize(std::uint8_t* buffer, std::size_t* bufferSize) override;
	const std::wstring& GetPathRef() const { return path_; }
	common::file_count_t GetOffset() const { return offset_; }

	static GetDirectoryInfoRequest& TypeCastFromMessage(Message& message);
	static std::unique_ptr<GetDirectoryInfoRequest> Deserialize(
		const std::uint8_t* buffer,
		std::size_t bufferSize);

	static const MessageFlag _MessageFlag =
		MessageFlag::GetDirectoryInfoRequest;

private:
	std::wstring path_;
	common::file_count_t offset_;
};

///////////////////////////////////////////////////////////////////////////////
class GetDirectoryInfoReply : public ServerMessage
{
public:
	explicit GetDirectoryInfoReply(common::status_code_t statusCode, common::Directory&& dir);

	virtual int Serialize(std::uint8_t* buffer, std::size_t* bufferSize) override;
	common::status_code_t GetStatusCode() const { return statusCode_; }
	common::Directory&& GetDirectoryRvalueRef() { return std::move(dir_); }

	static GetDirectoryInfoReply& TypeCastFromMessage(Message& message);
	static std::unique_ptr<GetDirectoryInfoReply> Deserialize(
		const std::uint8_t* buffer,
		std::size_t bufferSize);

	static const MessageFlag _MessageFlag =
		MessageFlag::GetDirectoryInfoReply;

private:
	common::status_code_t statusCode_;
	common::Directory dir_;
};

/*****************************************************************************/
/****************************** INLINE FUNCTIONS *****************************/
/*****************************************************************************/
inline GetDirectoryInfoRequest::GetDirectoryInfoRequest(
	std::wstring&& path,
	common::file_count_t offset)
	: ClientMessage(_MessageFlag),
	  path_(std::move(path)),
	  offset_(offset)
{
}

inline GetDirectoryInfoReply::GetDirectoryInfoReply(
	common::status_code_t statusCode,
	common::Directory&& dir)
	: ServerMessage(_MessageFlag),
	  statusCode_(statusCode),
	  dir_(std::move(dir))
{
}

/*static*/ inline GetDirectoryInfoRequest&
GetDirectoryInfoRequest::TypeCastFromMessage(Message& message)
{
	assert(message.GetMessageFlag() == _MessageFlag);
	return reinterpret_cast<GetDirectoryInfoRequest&>(message);
}

/*static*/ inline GetDirectoryInfoReply&
GetDirectoryInfoReply::TypeCastFromMessage(Message& message)
{
	assert(message.GetMessageFlag() == _MessageFlag);
	return reinterpret_cast<GetDirectoryInfoReply&>(message);
}

} // namespace message
} // namespace remoteFileExplorer