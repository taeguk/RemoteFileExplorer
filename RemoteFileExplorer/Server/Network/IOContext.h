#pragma once

#include <winsock2.h>

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <utility>

namespace remoteFileExplorer
{
namespace server
{
namespace network
{
///////////////////////////////////////////////////////////////////////////////
enum class IOContextType
{
	Send,
	Recv
};

///////////////////////////////////////////////////////////////////////////////
// Overlapped I/O를 수행할 때, I/O 에 대한 문맥을 담당하는 base class이다.
class IOContext
{
public:
	explicit IOContext(IOContextType type);
	virtual ~IOContext() = default;

	OVERLAPPED& GetOverlappedRef() { return overlapped_; }
	IOContextType GetType() const { return type_; }

	static IOContext* PointerCastFrom(OVERLAPPED* overlapped);

private:
	OVERLAPPED overlapped_;
	IOContextType type_;
};

/*****************************************************************************/
/****************************** INLINE FUNCTIONS *****************************/
/*****************************************************************************/
inline IOContext::IOContext(IOContextType type)
	: type_(type)
{
	memset(&overlapped_, 0, sizeof(overlapped_));
}

/*static*/ inline IOContext* IOContext::PointerCastFrom(OVERLAPPED* overlapped)
{
	return reinterpret_cast<IOContext*>(
		reinterpret_cast<std::uint8_t*>(overlapped) -
		offsetof(IOContext, overlapped_));
}

///////////////////////////////////////////////////////////////////////////////
// 송신에 관련된 I/O Context.
class IOSendContext : public IOContext
{
public:
	IOSendContext(std::size_t bufferSize);
	~IOSendContext() { delete[] buffer_; }

	WSABUF& GetUpdatedWsabufRef();
	std::uint8_t* GetBuffer() { return buffer_; }

	static IOSendContext& TypeCastFrom(IOContext& ioContext);

private:
	std::uint8_t* buffer_;
	std::size_t bufferSize_;
	WSABUF wsabuf_;
};

/*****************************************************************************/
/****************************** INLINE FUNCTIONS *****************************/
/*****************************************************************************/
inline IOSendContext::IOSendContext(std::size_t bufferSize)
	: IOContext(IOContextType::Send), bufferSize_(bufferSize)
{
	buffer_ = new std::uint8_t[bufferSize_];
}

inline WSABUF& IOSendContext::GetUpdatedWsabufRef()
{
	wsabuf_.buf = reinterpret_cast<char*>(buffer_);
	wsabuf_.len = bufferSize_;
	return wsabuf_;
}

/*static*/ inline IOSendContext& IOSendContext::TypeCastFrom(IOContext& ioContext)
{
	assert(ioContext.GetType() == IOContextType::Send);
	return reinterpret_cast<IOSendContext&>(ioContext);
}

///////////////////////////////////////////////////////////////////////////////
// 수신에 관련된 I/O Context.
class IORecvContext : public IOContext
{
public:
	IORecvContext(std::size_t bufferSize = 64 * 1024);
	~IORecvContext() { delete[] buffer_; }

	std::pair<std::uint8_t*, std::size_t> GetBufferStatus();
	WSABUF& GetUpdatedWsabufRef();
	void UpdateBuffer(std::size_t receivedBytes);
	void ConsumeBuffer(std::size_t consumedBytes);
	bool CheckBufferIsFull() const { return filledBufferSize_ >= maxBufferSize_; }
	void ResetBuffer() { filledBufferSize_ = 0; }

	static IORecvContext& TypeCastFrom(IOContext& ioContext);

private:
	const std::size_t maxBufferSize_;
	std::uint8_t* buffer_;
	std::size_t filledBufferSize_{ 0 };
	WSABUF wsabuf_;
};

/*****************************************************************************/
/****************************** INLINE FUNCTIONS *****************************/
/*****************************************************************************/
inline IORecvContext::IORecvContext(std::size_t bufferSize)
	: IOContext(IOContextType::Recv), maxBufferSize_(bufferSize)
{
	buffer_ = new std::uint8_t[maxBufferSize_];
}

inline void IORecvContext::UpdateBuffer(std::size_t receivedBytes)
{
	filledBufferSize_ += receivedBytes;
}

inline std::pair<std::uint8_t*, std::size_t> IORecvContext::GetBufferStatus()
{
	return std::make_pair(buffer_, filledBufferSize_);
}

inline WSABUF& IORecvContext::GetUpdatedWsabufRef()
{
	wsabuf_.buf = reinterpret_cast<char*>(buffer_ + filledBufferSize_);
	wsabuf_.len = maxBufferSize_ - filledBufferSize_;
	return wsabuf_;
}

/*static*/ inline IORecvContext& IORecvContext::TypeCastFrom(IOContext& ioContext)
{
	assert(ioContext.GetType() == IOContextType::Recv);
	return reinterpret_cast<IORecvContext&>(ioContext);
}

} // namespace network
} // namespace server
} // namespace remoteFileExplorer