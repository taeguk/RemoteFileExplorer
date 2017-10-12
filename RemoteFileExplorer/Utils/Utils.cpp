#include "Utils/Utils.h"

namespace remoteFileExplorer
{
namespace utils
{
///////////////////////////////////////////////////////////////////////////////
std::wstring utf8_to_wstring(const std::string& u8_str)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	return conv.from_bytes(u8_str);
}

std::string wstring_to_utf8(const std::wstring& str)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	return conv.to_bytes(str);
}

} // namespace utils
} // namespace remoteFileExplorer