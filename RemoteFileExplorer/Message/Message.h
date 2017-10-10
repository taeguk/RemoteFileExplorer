#pragma once

#include <cstdint>
#include <cstddef>
#include <cassert>
#include <memory>

#include "Utils/utils.h"

// TODO: line length 80자 제한 지키도록 바꾸기. (다른곳도)

// TODO: Message쪽 설계 다시 생각해보기.....

namespace remoteFileExplorer
{
namespace message
{
///////////////////////////////////////////////////////////////////////////////
enum class MessageFlag : std::uint8_t
{
	/* 무조건 명시적으로 값을 지정하도록 한다. */
	/* Client Message (Client -> Server) 의 경우 flag 값이 짝수이다. */
	/* Server Message (Server -> Client) 의 경우 flag 값이 홀수이다. */
	/* 네이밍 규칙 --- 기본적으로는 아래를 따른다.
	     Client Message : xxxReqeust , Server Message : xxxReply */

	GetLogicalDriveInfoRequest = 0x00,
	GetLogicalDriveInfoReply = 0x01,

	GetDirectoryInfoRequest = 0x02,
	GetDirectoryInfoReply = 0x03,

	// 추후 프로토콜의 확장, Message Flag의 부족등을 고려한 예약 값들.
	_ReservedForExtension1 = 0xFE,
	_ReservedForExtension2 = 0xFF
};

///////////////////////////////////////////////////////////////////////////////
class Message
{
public:
	explicit Message(MessageFlag messageFlag) : messageFlag_(messageFlag) {}
	virtual ~Message() = default;

	bool IsClientMessage() const { return utils::to_underlying(messageFlag_) % 2 == 0; }
	bool IsServerMessage() const { return utils::to_underlying(messageFlag_) % 2 == 1; }

	MessageFlag GetMessageFlag() const { return messageFlag_; }

	virtual int Serialize(std::uint8_t* buffer, std::size_t* bufferSize) = 0;

	static std::unique_ptr<Message> Deserialize(
		const std::uint8_t* buffer,
		std::size_t bufferSize);

private:
	/******************** DATA FIELDS ********************/
	MessageFlag messageFlag_;
};

///////////////////////////////////////////////////////////////////////////////
// Client가 Server에게 보내는 Message
class ClientMessage : public Message
{
public:
	explicit ClientMessage(MessageFlag messageFlag) : Message(messageFlag) {}

	static ClientMessage& TypeCastFromMessage(Message& message);
};

///////////////////////////////////////////////////////////////////////////////
// Server가 Client에게 보내는 Message
class ServerMessage : public Message
{
public:
	explicit ServerMessage(MessageFlag messageFlag) : Message(messageFlag) {}

	static ServerMessage& TypeCastFromMessage(Message& message);
};

/*****************************************************************************/
/****************************** INLINE FUNCTIONS *****************************/
/*****************************************************************************/
/*static*/ inline ClientMessage& ClientMessage::TypeCastFromMessage(Message& message)
{
	assert(message.IsClientMessage());
	return reinterpret_cast<ClientMessage&>(message);
}

/*static*/ inline ServerMessage& ServerMessage::TypeCastFromMessage(Message& message)
{
	assert(message.IsServerMessage());
	return reinterpret_cast<ServerMessage&>(message);
}

} // namespace message
} // namespace remoteFileExplorer