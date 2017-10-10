#include "Server/FileExplorerService.h"

#include <windows.h>

namespace remoteFileExplorer
{
namespace server
{
///////////////////////////////////////////////////////////////////////////////
namespace /*unnamed*/
{
inline time_t filetime_to_time_t(const FILETIME& ft)
{
    ULARGE_INTEGER ull;
    ull.LowPart = ft.dwLowDateTime;
    ull.HighPart = ft.dwHighDateTime;

    return ull.QuadPart / 10000000ULL - 11644473600ULL;
}
} // unnamed namespace

///////////////////////////////////////////////////////////////////////////////
int FileExplorerService::GetLogicalDriveInfo(
	std::vector<common::LogicalDrive>& drives)
{
	using common::LogicalDrive;

	drives.clear();

	DWORD driveMask = GetLogicalDrives();
	if (driveMask == 0)
		return -1;

	wchar_t letter = L'A';
	wchar_t drivePath[4] = L"?:\\";
	wchar_t driveName[MAX_PATH + 1];
	for (; driveMask; driveMask >>= 1, ++letter)
	{
		if (driveMask & 1)
		{
			LogicalDrive drive;
			drive.letter = letter;

			drivePath[0] = letter;

			if (!GetVolumeInformationW(
				drivePath,
				driveName,
				ARRAYSIZE(driveName),
				nullptr, nullptr, nullptr, nullptr, 0))
			{
				// Volume 정보를 얻을 수 없을 때는 그 볼륨을 무시한다.
				continue;
			}

			drive.driveName = driveName;

			drives.push_back(std::move(drive));
		}
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
int FileExplorerService::GetDirectoryInfo(
	const std::wstring & path,
	common::Directory & dir)
{
	using common::FileInformation;
	using common::FileType;
	using common::FileAttribute;

	WIN32_FIND_DATA ffd;
	HANDLE hFind;

	hFind = FindFirstFileW(
		(std::wstring(path.c_str()) += L"/*").c_str(), &ffd);

	if (hFind == INVALID_HANDLE_VALUE)
		return -1;

	dir.path = path;

	do
	{
		FileInformation fileInfo;
		//
		fileInfo.fileName = ffd.cFileName;
		//
		fileInfo.modifiedDate =
			filetime_to_time_t(ffd.ftLastWriteTime);
		//
		fileInfo.fileAttr = FileAttribute::NoFlag;

		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)
		{
			fileInfo.fileAttr |= FileAttribute::System;
		}
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
		{
			fileInfo.fileAttr |= FileAttribute::Hidden;
		}

		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			fileInfo.fileType = FileType::Directory;
		}
		else
		{
			fileInfo.fileType = FileType::File;

			LARGE_INTEGER filesize;
			filesize.LowPart = ffd.nFileSizeLow;
			filesize.HighPart = ffd.nFileSizeHigh;

			fileInfo.fileSize = filesize.QuadPart;
		}

		dir.fileInfos.push_back(std::move(fileInfo));

	} while (FindNextFileW(hFind, &ffd));

	if (GetLastError() != ERROR_NO_MORE_FILES)
		return -1;

	FindClose(hFind);

	return 0;
}

} // namespace server
} // namespace remoteFileExplorer