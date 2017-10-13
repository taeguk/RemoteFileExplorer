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
// 특정 디렉토리 안에 있는 파일들의 정보를 요청하는 message.
class GetDirectoryInfoRequest : public ClientMessage
{
public:
	explicit GetDirectoryInfoRequest(
		std::wstring&& path,
		common::file_count_t offset);
	
	const std::wstring& GetPathRef() const { return path_; }
	common::file_count_t GetOffset() const { return offset_; }

	virtual int Serialize(
		std::uint8_t* buffer,
		std::size_t& bufferSize) override;

	static std::unique_ptr<GetDirectoryInfoRequest> Deserialize(
		const std::uint8_t* buffer,
		std::size_t bufferSize);
	static GetDirectoryInfoRequest& TypeCastFrom(Message& message);

	static const MessageFlag _MessageFlag =
		MessageFlag::GetDirectoryInfoRequest;

private:
	std::wstring path_;
	// 디렉토리 내에 파일의 개수가 많으면, 여러가지 문제로 인해
	//   모든 파일들의 정보를 한번에 송신하기 어렵다.
	// 따라서, 현재 여러 번에 걸쳐서 송신하는 전략을 취하고 있으며,
	//   이를 위해서 정보 요청시에, 어디까지 받았는지를 의미하는 offset정보를
	//   같이 보내야만 한다.
	common::file_count_t offset_;
};

///////////////////////////////////////////////////////////////////////////////
// 특정 디렉토리 안에 있는 파일들의 정보를 반환하는 message.
class GetDirectoryInfoReply : public ServerMessage
{
public:
	explicit GetDirectoryInfoReply(
		common::status_code_t statusCode,
		common::Directory&& dir);

	common::status_code_t GetStatusCode() const { return statusCode_; }
	common::Directory&& GetDirectoryRvalueRef() { return std::move(dir_); }

	virtual int Serialize(
		std::uint8_t* buffer,
		std::size_t& bufferSize) override;

	static GetDirectoryInfoReply& TypeCastFrom(Message& message);
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
GetDirectoryInfoRequest::TypeCastFrom(Message& message)
{
	assert(message.GetMessageFlag() == _MessageFlag);
	return reinterpret_cast<GetDirectoryInfoRequest&>(message);
}

/*static*/ inline GetDirectoryInfoReply&
GetDirectoryInfoReply::TypeCastFrom(Message& message)
{
	assert(message.GetMessageFlag() == _MessageFlag);
	return reinterpret_cast<GetDirectoryInfoReply&>(message);
}

} // namespace message
} // namespace remoteFileExplorer