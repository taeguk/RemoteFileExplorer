#include "RemoteFileExplorer.h"

namespace remoteFileExplorer
{
namespace client
{
///////////////////////////////////////////////////////////////////////////////
int RemoteFileExplorer::Connect(std::uint8_t ipAddress[4], std::uint16_t port)
{
	if (serverConnector_.Connect(ipAddress, port) == 0)
		return 0;
	else
		return -1;
}

///////////////////////////////////////////////////////////////////////////////
int RemoteFileExplorer::Disconnect()
{
	if (serverConnector_.Disconnect())
		return 0;
	else
		return -1;
}

///////////////////////////////////////////////////////////////////////////////
std::string RemoteFileExplorer::Echo(const char * str)
{
	static std::uint8_t buffer[1024]; // TODO: 제대로된 버퍼관리 설계하기.

	// TODO: 연결된 상태인지 여부 체크하기.

	message::EchoRequestMessage request(str);
	std::size_t bufferSize = 1024;
	request.Serialize(buffer, &bufferSize);  // TODO: 예외처리 (다른곳도)
	serverConnector_.Communicate(buffer, &bufferSize, 1024);
	auto replyNotCasted = message::Message::Deserialize(buffer, bufferSize);

	if (replyNotCasted->GetMessageFlag() != message::EchoReplyMessage::_MessageFlag)
		return "(((ERROR)))";  // TODO: 예외처리 방법 강구.

	// TODO: 나중에 Message::Cast<T>(notCasted) 이런식으로 할 수 있게 수정하자.
	auto& reply =
		message::EchoReplyMessage::TypeCastFromMessage(*replyNotCasted);

	return reply.GetString();
}

} // namespace client
} // namespace remoteFileExplorer