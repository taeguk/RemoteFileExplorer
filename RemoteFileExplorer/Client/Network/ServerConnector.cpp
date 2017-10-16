#include "Client/Network/ServerConnector.h"

#include "Utils/Utils.h"

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
        (void) Disconnect();
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

    auto wsaRAII = utils::CreateRAIIWrapper([this]()
    {
        if (!connected_)
            (void) WSACleanup();
    });

    hSocket_ = WSASocket(PF_INET, SOCK_STREAM, 0, nullptr, 0, 0);
    if (hSocket_ == INVALID_SOCKET)
        return -1;

    auto socketRAII = utils::CreateRAIIWrapper([this]()
    {
        if (!connected_)
            (void) closesocket(hSocket_);
    });

    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.S_un.S_un_b.s_b1 = ipAddress[0];
    serverAddress.sin_addr.S_un.S_un_b.s_b2 = ipAddress[1];
    serverAddress.sin_addr.S_un.S_un_b.s_b3 = ipAddress[2];
    serverAddress.sin_addr.S_un.S_un_b.s_b4 = ipAddress[3];
    serverAddress.sin_port = htons(port);

    if (connect(
        hSocket_,
        (SOCKADDR*) &serverAddress,
        sizeof(serverAddress)) == SOCKET_ERROR)
    {
        return -1;
    }

    connected_ = true;

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
int ServerConnector::Disconnect()
{
    std::lock_guard<decltype(mutex_)> lk(mutex_);

    if (!connected_)
        return -1;

    (void) closesocket(hSocket_);
    (void) WSACleanup();

    connected_ = false;

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
int ServerConnector::Communicate(
    std::uint8_t* buffer,
    std::size_t& bufferSize,
    std::size_t maxBufferSize)
{
    std::lock_guard<decltype(mutex_)> lk(mutex_);

    if (!connected_)
        return -1;

    WSABUF wsabuf;
    DWORD sendBytes = 0;
    DWORD recvBytes = 0;
    DWORD flags = 0;

    // Send the length of message, first.
    common::message_size_t sendMessageLength =
        static_cast<common::message_size_t>(bufferSize);
    wsabuf.buf = reinterpret_cast<char*>(&sendMessageLength);
    wsabuf.len = sizeof(sendMessageLength);
    if (WSASend(hSocket_, &wsabuf, 1, &sendBytes,
        0, nullptr, nullptr) == SOCKET_ERROR)
    {
        return -1;
    }

    // Send the message.
    wsabuf.buf = reinterpret_cast<char*>(buffer);
    wsabuf.len = static_cast<ULONG>(bufferSize);
    if (WSASend(hSocket_, &wsabuf, 1, &sendBytes,
        0, nullptr, nullptr) == SOCKET_ERROR)
    {
        return -1;
    }

    // Receive the length of message, first.
    common::message_size_t recvMessageLength;
    wsabuf.buf = reinterpret_cast<char*>(&recvMessageLength);
    wsabuf.len = sizeof(recvMessageLength);
    while (wsabuf.len > 0)
    {
        flags = 0;
        if (WSARecv(hSocket_, &wsabuf, 1, &recvBytes,
            &flags, nullptr, nullptr) == SOCKET_ERROR || recvBytes == 0)
        {
            return -1;
        }
        wsabuf.buf += recvBytes;
        wsabuf.len -= recvBytes;
    }

    // Receive the message.
    wsabuf.buf = reinterpret_cast<char*>(buffer);
    wsabuf.len = recvMessageLength;
    while (wsabuf.len > 0)
    {
        flags = 0;
        if (WSARecv(hSocket_, &wsabuf, 1, &recvBytes,
            &flags, nullptr, nullptr) == SOCKET_ERROR || recvBytes == 0)
        {
            return -1;
        }
        wsabuf.buf += recvBytes;
        wsabuf.len -= recvBytes;
    }

    bufferSize = recvMessageLength;

    return 0;
}

} // namespace network
} // namespace client
} // namespace remoteFileExplorer