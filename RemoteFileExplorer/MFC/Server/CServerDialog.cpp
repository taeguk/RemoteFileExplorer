// CServerDialog.cpp : 구현 파일입니다.
//

#include "MFC/stdafx.h"

#include "MFC/Server/CServerDialog.h"

#include <afxdialogex.h>

#include "Server/FileExplorerService.h"

namespace remoteFileExplorer
{
namespace mfc
{
namespace server
{

// 이거 없으면 타이핑 너무 힘듬;;
using namespace remoteFileExplorer::server;

// CServerDialog 대화 상자입니다.

IMPLEMENT_DYNAMIC(CServerDialog, CDialogEx)

CServerDialog::CServerDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SERVER, pParent)
{
}

CServerDialog::~CServerDialog()
{
}

BOOL CServerDialog::OnInitDialog()
{
	if (CDialogEx::OnInitDialog() == FALSE) {
		return FALSE;
	}

	server_ = std::make_unique<Server>(
		std::make_unique<FileExplorerService>(
			std::make_unique<FileExplorerWatcher>(GetSafeHwnd(), FeWmBase)));

	logListCtrl_.InsertColumn(0, _T("Category"), LVCFMT_CENTER, 90);
	logListCtrl_.InsertColumn(1, _T("Log"), LVCFMT_LEFT, 90);
	logListCtrl_.InsertColumn(2, _T("Etc"), LVCFMT_LEFT, 90);

	portEdit_.SetWindowTextW(_T("9622"));

	return TRUE;
}

void CServerDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MFCBUTTON_SERVER_CONTROL, controlButton_);
	DDX_Control(pDX, IDC_EDIT_LISTEN_PORT, portEdit_);
	DDX_Control(pDX, IDC_LIST_SERVER_LOG, logListCtrl_);
}


BEGIN_MESSAGE_MAP(CServerDialog, CDialogEx)
	ON_BN_CLICKED(IDC_MFCBUTTON_SERVER_CONTROL,
		&CServerDialog::OnBnClickedMfcbuttonServerControl)
	ON_MESSAGE(CServerDialog::FeWmBase + FileExplorerWatcher::WmOffsetGetLogicalDriveInfo,
		&CServerDialog::OnGetLogicalDriveInfo)
	ON_MESSAGE(CServerDialog::FeWmBase + FileExplorerWatcher::WmOffsetGetDirectoryInfo,
		&CServerDialog::OnGetDirectoryInfo)
END_MESSAGE_MAP()


// CServerDialog 메시지 처리기입니다.
void CServerDialog::OnBnClickedMfcbuttonServerControl()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (status_ == ServerStatus::STARTED)
	{
		server_->Stop();
		controlButton_.SetWindowTextW(_T("Run Server"));

		status_ = ServerStatus::STOPPED;
	}
	else if (status_ == ServerStatus::STOPPED)
	{
		CString cstr;
		portEdit_.GetWindowTextW(cstr);

		int len = cstr.GetLength();
		if (len == 0 || len > 5)
		{
			AfxMessageBox(_T("Invalid Port Number!"));
			return;
		}
		int num = _ttoi(cstr.GetBuffer());
		if (num > UINT16_MAX)
		{
			AfxMessageBox(_T("Invalid Port Number!"));
			return;
		}
		std::uint16_t port = static_cast<std::uint16_t>(num);

		if (server_->Start(port) != 0)
		{
			AfxMessageBox(_T("Fail to start server!"));
			return;
		}

		controlButton_.SetWindowTextW(_T("Stop Server"));

		status_ = ServerStatus::STARTED;
	}
}

LRESULT CServerDialog::OnGetLogicalDriveInfo(WPARAM wParam, LPARAM lParam)
{
	//TODO: 아래같이 할까 말까?
	/*
	if (status_ != ServerStatus::STARTED)
	{
		AfxMessageBox(_T("FUCK!"));
		return 0;
	}
	*/

	int nIndex = logListCtrl_.InsertItem(0, _T("Get Logical Drive Info"));
	logListCtrl_.SetItemText(nIndex, 1, _T(""));
	logListCtrl_.SetItemText(nIndex, 2, _T(""));

	return 0;
}

LRESULT CServerDialog::OnGetDirectoryInfo(WPARAM wParam, LPARAM lParam)
{
	using ParamType = std::pair<std::wstring, std::uint32_t>;

	auto param =
		std::unique_ptr<ParamType>(reinterpret_cast<ParamType*>(wParam));

	int nIndex = logListCtrl_.InsertItem(0, _T("Get Directory Info"));
	logListCtrl_.SetItemText(nIndex, 1, CString(param->first.c_str()));
	logListCtrl_.SetItemText(nIndex, 2, CString(
		std::to_string(param->second).c_str()));

	return 0;
}

} // namespace server
} // namespace mfc
} // namespace remoteFileExplorer