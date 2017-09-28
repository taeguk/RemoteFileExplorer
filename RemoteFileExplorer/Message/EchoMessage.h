#pragma once

#include <string>

#include "Message/Message.h"

namespace remoteFileExplorer
{
namespace message
{
///////////////////////////////////////////////////////////////////////////////
class EchoRequestMessage : public ClientMessage
{
public:
	explicit EchoRequestMessage(const std::string& string);

	virtual int Serialize(std::uint8_t* buffer, std::size_t* bufferSize) override;
	std::string GetString() const { return string_; }

	static EchoRequestMessage& TypeCastFromMessage(Message& message);
	static std::unique_ptr<EchoRequestMessage> Deserialize(
		const std::uint8_t* buffer,
		std::size_t bufferSize);

	static const MessageFlag _MessageFlag = MessageFlag::EchoRequest;

private:
	std::string string_;
};

///////////////////////////////////////////////////////////////////////////////
class EchoReplyMessage : public ServerMessage
{
public:
	explicit EchoReplyMessage(const std::string& string);

	virtual int Serialize(std::uint8_t* buffer, std::size_t* bufferSize) override;
	std::string GetString() const { return string_; }

	static EchoReplyMessage& TypeCastFromMessage(Message& message);
	static std::unique_ptr<EchoReplyMessage> Deserialize(
		const std::uint8_t* buffer,
		std::size_t bufferSize);

	static const MessageFlag _MessageFlag = MessageFlag::EchoReply;

private:
	std::string string_;
};

/*****************************************************************************/
/****************************** INLINE FUNCTIONS *****************************/
/*****************************************************************************/
inline EchoRequestMessage::EchoRequestMessage(const std::string& string)
	: ClientMessage(_MessageFlag), string_(string)
{
}

inline EchoReplyMessage::EchoReplyMessage(const std::string& string)
	: ServerMessage(_MessageFlag), string_(string)
{
}

/*static*/ inline EchoRequestMessage& EchoRequestMessage::TypeCastFromMessage(Message& message)
{
	assert(message.GetMessageFlag() == _MessageFlag);
	return reinterpret_cast<EchoRequestMessage&>(message);
}

/*static*/ inline EchoReplyMessage& EchoReplyMessage::TypeCastFromMessage(Message& message)
{
	assert(message.GetMessageFlag() == _MessageFlag);
	return reinterpret_cast<EchoReplyMessage&>(message);
}

} // namespace message
} // namespace remoteFileExplorer