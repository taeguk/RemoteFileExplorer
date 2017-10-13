#pragma once

#include <ctime>
#include <string>
#include <vector>

#include "Common/CommonType.h"
#include "Common/FileSystem.h"

namespace remoteFileExplorer
{
namespace common
{
///////////////////////////////////////////////////////////////////////////////
// RPC (Remote Procedure Call)을 위한 핵심 인터페이스.
// RPC Client와 Server 측 모두 이 인터페이스를 상속받아 구현해야한다.
// RPC Client 측에서는, Server에 프로시저 호출을 요청하고 그 결과를 반환해야한다.
// RPC Server 측에서는, 로컬에서 프로시저를 수행하고 그 결과를 반환한다.
// RPC 명세를 인터페이스를 통해 관리함으로서, RPC 명세의 일관성을
//   C++ 타입 시스템으로 검증할 수 있다. (컴파일할 때, 일관성이 검사된다는 뜻.)
class FileExplorerInterface
{
public:
	virtual ~FileExplorerInterface() = default;

	virtual int GetLogicalDriveInfo(std::vector<LogicalDrive>& drives) = 0;
	virtual int GetDirectoryInfo(
		const std::wstring& path,
		file_count_t offset,
		Directory& dir) = 0;
};

} // namespace common
} // namespace remoteFileExplorer