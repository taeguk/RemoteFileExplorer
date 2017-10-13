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

///////////////////////////////////////////////////////////////////////////////
int RemoteFileExplorer::GetLogicalDriveInfo(
	std::vector<common::LogicalDrive>& drives)
{
	std::size_t bufferSize = maxBufferSize_;

	message::GetLogicalDriveInfoRequest request;
	if (request.Serialize(buffer_, bufferSize) != 0)
		throw RPCException();

	if (serverConnector_.Communicate(buffer_, bufferSize, maxBufferSize_) != 0)
		throw RPCException();

	auto replyNotCasted = message::Message::Deserialize(buffer_, bufferSize);

	if (replyNotCasted->GetMessageFlag() !=
		message::GetLogicalDriveInfoReply::_MessageFlag)
	{
		throw RPCException();
	}

	auto& reply =
		message::GetLogicalDriveInfoReply::TypeCastFrom(*replyNotCasted);

	drives = reply.GetLogicalDrivesRvalueRef();

	return reply.GetStatusCode();
}

///////////////////////////////////////////////////////////////////////////////
int RemoteFileExplorer::GetDirectoryInfo(
	const std::wstring& path,
	common::file_count_t offset,
	common::Directory& dir)
{
	std::size_t bufferSize = maxBufferSize_;

	message::GetDirectoryInfoRequest request{ std::wstring(path), offset };
	if (request.Serialize(buffer_, bufferSize) != 0)
		throw RPCException();

	if (serverConnector_.Communicate(buffer_, bufferSize, maxBufferSize_) != 0)
		throw RPCException();

	auto replyNotCasted = message::Message::Deserialize(buffer_, bufferSize);

	if (replyNotCasted->GetMessageFlag() !=
		message::GetDirectoryInfoReply::_MessageFlag)
	{
		throw RPCException();
	}

	auto& reply =
		message::GetDirectoryInfoReply::TypeCastFrom(*replyNotCasted);
	
	dir = reply.GetDirectoryRvalueRef();

	return reply.GetStatusCode();
}

} // namespace client
} // namespace remoteFileExplorer