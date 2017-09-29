#include "Client/Network/ServerConnector.h"

namespace remoteFileExplorer
{
namespace client
{
namespace network
{
///////////////////////////////////////////////////////////////////////////////
ServerConnector::~ServerConnector()
{
	if (connected_)
	{
		// Disconnect가 실패하더라도 무시한다.
		Disconnect();
	}
}

///////////////////////////////////////////////////////////////////////////////
int ServerConnector::Connect(std::uint8_t ipAddress[4], std::uint16_t port)
{
	std::lock_guard<decltype(mutex_)> lk(mutex_);

	if (connected_)
		return -1;

	WSADATA wsaData;
	SOCKADDR_IN serverAddress;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return -1;

	hSocket_ = WSASocket(PF_INET, SOCK_STREAM, 0, nullptr, 0, WSA_FLAG_OVERLAPPED);
	if (hSocket_ == INVALID_SOCKET)
		return -1;

	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.S_un.S_un_b.s_b1 = ipAddress[0];
	serverAddress.sin_addr.S_un.S_un_b.s_b2 = ipAddress[1];
	serverAddress.sin_addr.S_un.S_un_b.s_b3 = ipAddress[2];
	serverAddress.sin_addr.S_un.S_un_b.s_b4 = ipAddress[3];
	serverAddress.sin_port = htons(port);

	if (connect(hSocket_, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
		return -1;

	connected_ = true;

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
int ServerConnector::Disconnect()
{
	std::lock_guard<decltype(mutex_)> lk(mutex_);

	if (!connected_)
		return -1;

	closesocket(hSocket_);
	WSACleanup();

	return 0;
}

int ServerConnector::Communicate(
	const std::uint8_t* buffer,
	std::size_t* bufferSize,
	std::size_t maxBufferSize)
{
	std::lock_guard<decltype(mutex_)> lk(mutex_);  // TODO: lock waiting 줄이기.

	if (!connected_)
		return -1;

	WSABUF dataBuf;
	DWORD sendBytes = 0;
	DWORD recvBytes = 0;
	DWORD flags = 0;

	dataBuf.len = static_cast<ULONG>(*bufferSize);
	dataBuf.buf = reinterpret_cast<char*>(
		const_cast<std::uint8_t*>(buffer));

	if (WSASend(hSocket_, &dataBuf, 1, &sendBytes,
		0, nullptr, nullptr) == SOCKET_ERROR)
	{
		return -1;
	}

	dataBuf.len = static_cast<ULONG>(maxBufferSize);

	if (WSARecv(hSocket_, &dataBuf, 1, &recvBytes,
		&flags, nullptr, nullptr) == SOCKET_ERROR)
	{
		return -1;
	}

	*bufferSize = recvBytes;

	return 0;
}

} // namespace network
} // namespace client
} // namespace remoteFileExplorer