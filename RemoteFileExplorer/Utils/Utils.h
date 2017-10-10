#pragma once

#include <codecvt>
#include <string>
#include <type_traits>

namespace remoteFileExplorer
{
namespace utils
{

///////////////////////////////////////////////////////////////////////////////
template <typename E>
constexpr typename std::underlying_type<E>::type to_underlying(E e) noexcept {
	return static_cast<typename std::underlying_type<E>::type>(e);
}

///////////////////////////////////////////////////////////////////////////////
// convert UTF-8 string to wstring
std::wstring utf8_to_wstring(const std::string& u8_str)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
	return myconv.from_bytes(u8_str);
}

// convert wstring to UTF-8 string
std::string wstring_to_utf8(const std::wstring& str)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
	return myconv.to_bytes(str);
}

///////////////////////////////////////////////////////////////////////////////
template <typename Functor>
class RAIIWrapper
{
public:
	explicit RAIIWrapper(Functor&& functor)
		: functor_(std::move(functor))
	{
	}
	~RAIIWrapper()
	{
		functor_();
	}

private:
	Functor functor_;
};

template <typename Functor>
static auto CreateRAIIWrapper(Functor&& functor)
	-> RAIIWrapper<Functor>
{
	return RAIIWrapper<Functor>(std::forward<Functor>(functor));
}

/*
///////////////////////////////////////////////////////////////////////////////
template <typename ElementType>
class StaticVector
{
public:
	explicit StaticVector(std::size_t count)
		: count_(count)
	{
		void* mem = ::operator new[](count_ * sizeof(ElementType));
	}

private:
	ElementType* elements_;
	std::size_t count_;
};
*/

} // namespace utils
} // namespace remoteFileExplorer