#pragma once

#include <memory>

#include "Common/FileExplorerInterface.h"

namespace remoteFileExplorer
{
namespace server
{
///////////////////////////////////////////////////////////////////////////////
// 파일 탐색기 서비스에 대한 인터페이스.
// 실제로 로컬에서 파일 탐색기에 관련된 기능들을 수행하는 것에 대한 인터페이스.
class FileExplorerServiceInterface : public common::FileExplorerInterface
{
public:
	// 새로운 클라이언트가 접속하면, 파일 탐색기 서비스 객체가 복제되어
	//   그 클라이언트와 연계되게 된다.
	// 그리고 연계된 서비스 객체는 그 클라이언트를 위한 맞춤형 객체로 진화할 것이다.
	//   (물론 지금 당장은 필요성이 없지만, 추후를 고려한 설계이다.)
	virtual std::unique_ptr<FileExplorerServiceInterface> Clone() const = 0;
};

} // namespace server
} // namespace remoteFileExplorer