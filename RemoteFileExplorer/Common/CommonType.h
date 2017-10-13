#pragma once

#include <cstdint>

namespace remoteFileExplorer
{
namespace common
{
// 프로젝트 전역적으로 활용되는 데이터 타입들을 정의한다.
using message_size_t = std::uint32_t;
using file_count_t = std::uint32_t;
using status_code_t = std::int8_t;

} // namespace common
} // namespace remoteFileExplorer