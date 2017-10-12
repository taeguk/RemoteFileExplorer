#include "Client/RemoteFileExplorer.h"

#include "Message/GetLogicalDriveInfoMessage.h"
#include "Message/GetDirectoryInfoMessage.h"

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
	if (serverConnector_.Disconnect() == 0)
		return 0;
	else
		return -1;
}

int RemoteFileExplorer::GetLogicalDriveInfo(
	std::vector<common::LogicalDrive>& drives)
{
	static std::uint8_t buffer[4096]; // TODO: 제대로된 버퍼관리 설계하기.
	const std::size_t maxBufferSize = 4096;
	std::size_t bufferSize = 4096;

	// TODO: 연결된 상태인지 여부 체크하기.

	message::GetLogicalDriveInfoRequest request;
	if (request.Serialize(buffer, &bufferSize) != 0)
		return -1;

	if (serverConnector_.Communicate(buffer, &bufferSize, maxBufferSize) != 0)
		return -1;

	auto replyNotCasted = message::Message::Deserialize(buffer, bufferSize);

	if (replyNotCasted->GetMessageFlag() !=
		message::GetLogicalDriveInfoReply::_MessageFlag)
	{
		return -1;
	}

	// TODO: 나중에 Message::Cast<T>(notCasted) 이런식으로 할 수 있게 수정하자.
	auto& reply =
		message::GetLogicalDriveInfoReply::TypeCastFromMessage(*replyNotCasted);

	drives = reply.GetLogicalDrivesRvalueRef();

	return reply.GetStatusCode();
}

int RemoteFileExplorer::GetDirectoryInfo(
	const std::wstring& path,
	std::uint32_t offset,
	common::Directory& dir)
{
	static const std::size_t MaxBufferSize = 64 * 1024;
	static std::uint8_t buffer[MaxBufferSize]; // TODO: 제대로된 버퍼관리 설계하기.
	std::size_t bufferSize = MaxBufferSize;

	// TODO: 연결된 상태인지 여부 체크하기.

	message::GetDirectoryInfoRequest request{ std::wstring(path), offset };
	if (request.Serialize(buffer, &bufferSize) != 0)
		return -1;

	serverConnector_.Communicate(buffer, &bufferSize, MaxBufferSize);

	auto replyNotCasted = message::Message::Deserialize(buffer, bufferSize);

	if (replyNotCasted->GetMessageFlag() !=
		message::GetDirectoryInfoReply::_MessageFlag)
	{
		return -1;
	}

	// TODO: 나중에 Message::Cast<T>(notCasted) 이런식으로 할 수 있게 수정하자.
	auto& reply =
		message::GetDirectoryInfoReply::TypeCastFromMessage(*replyNotCasted);
	
	dir = reply.GetDirectoryRvalueRef();

	return reply.GetStatusCode();
}

} // namespace client
} // namespace remoteFileExplorer