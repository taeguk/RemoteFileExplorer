#pragma once

#include <string>

#include "Message/Message.h"
#include "Common/FileSystem.h"

namespace remoteFileExplorer
{
namespace message
{
///////////////////////////////////////////////////////////////////////////////
class GetLogicalDriveInfoRequest : public ClientMessage
{
public:
	explicit GetLogicalDriveInfoRequest();

	virtual int Serialize(std::uint8_t* buffer, std::size_t* bufferSize) override;

	static GetLogicalDriveInfoRequest& TypeCastFromMessage(Message& message);
	static std::unique_ptr<GetLogicalDriveInfoRequest> Deserialize(
		const std::uint8_t* buffer,
		std::size_t bufferSize);

	static const MessageFlag _MessageFlag =
		MessageFlag::GetLogicalDriveInfoRequest;

private:
};

///////////////////////////////////////////////////////////////////////////////
class GetLogicalDriveInfoReply : public ServerMessage
{
public:
	explicit GetLogicalDriveInfoReply(std::int8_t statusCode,
		std::vector<common::LogicalDrive>&& drives);

	virtual int Serialize(std::uint8_t* buffer, std::size_t* bufferSize) override;
	std::int8_t GetStatusCode() const { return statusCode_; }
	std::vector<common::LogicalDrive>&&
		GetLogicalDrivesRvalueRef() { return std::move(drives_); }

	static GetLogicalDriveInfoReply& TypeCastFromMessage(Message& message);
	static std::unique_ptr<GetLogicalDriveInfoReply> Deserialize(
		const std::uint8_t* buffer,
		std::size_t bufferSize);

	static const MessageFlag _MessageFlag =
		MessageFlag::GetLogicalDriveInfoReply;

private:
	std::int8_t statusCode_;
	std::vector<common::LogicalDrive> drives_;
};

/*****************************************************************************/
/****************************** INLINE FUNCTIONS *****************************/
/*****************************************************************************/
inline GetLogicalDriveInfoRequest::GetLogicalDriveInfoRequest()
	: ClientMessage(_MessageFlag)
{
}

inline GetLogicalDriveInfoReply::GetLogicalDriveInfoReply(
	std::int8_t statusCode,
	std::vector<common::LogicalDrive>&& drives)
	: ServerMessage(_MessageFlag),
	  statusCode_(statusCode),
	  drives_(std::move(drives))
{
}

/*static*/ inline GetLogicalDriveInfoRequest&
GetLogicalDriveInfoRequest::TypeCastFromMessage(Message& message)
{
	assert(message.GetMessageFlag() == _MessageFlag);
	return reinterpret_cast<GetLogicalDriveInfoRequest&>(message);
}

/*static*/ inline GetLogicalDriveInfoReply&
GetLogicalDriveInfoReply::TypeCastFromMessage(Message& message)
{
	assert(message.GetMessageFlag() == _MessageFlag);
	return reinterpret_cast<GetLogicalDriveInfoReply&>(message);
}

} // namespace message
} // namespace remoteFileExplorer