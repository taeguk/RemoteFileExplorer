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
///////////////////////////////////////////////////////////////////////////////
using namespace remoteFileExplorer::server; // 타이핑을 줄이기 위해서..

IMPLEMENT_DYNAMIC(CServerDialog, CDialogEx)

///////////////////////////////////////////////////////////////////////////////
CServerDialog::CServerDialog(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_SERVER, pParent)
{
}

///////////////////////////////////////////////////////////////////////////////
CServerDialog::~CServerDialog()
{
}

///////////////////////////////////////////////////////////////////////////////
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

///////////////////////////////////////////////////////////////////////////////
void CServerDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_MFCBUTTON_SERVER_CONTROL, controlButton_);
    DDX_Control(pDX, IDC_EDIT_LISTEN_PORT, portEdit_);
    DDX_Control(pDX, IDC_LIST_SERVER_LOG, logListCtrl_);
}

///////////////////////////////////////////////////////////////////////////////
BOOL CServerDialog::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
        return TRUE;

    return CDialogEx::PreTranslateMessage(pMsg);
}

///////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CServerDialog, CDialogEx)
    ON_BN_CLICKED(IDC_MFCBUTTON_SERVER_CONTROL,
        &CServerDialog::OnBnClickedMfcbuttonServerControl)
    ON_MESSAGE(CServerDialog::FeWmBase +
        FileExplorerWatcher::WmOffsetGetLogicalDriveInfo,
        &CServerDialog::OnGetLogicalDriveInfo)
    ON_MESSAGE(CServerDialog::FeWmBase +
        FileExplorerWatcher::WmOffsetGetDirectoryInfo,
        &CServerDialog::OnGetDirectoryInfo)
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
void CServerDialog::OnBnClickedMfcbuttonServerControl()
{
    if (status_ == ServerStatus::STARTED)
    {
        server_->Stop();
        controlButton_.SetWindowTextW(_T("Run Server"));

        status_ = ServerStatus::STOPPED;
        logListCtrl_.DeleteAllItems();
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

        // Log List Control Column Width 조정.
        assert(logListCtrl_.GetHeaderCtrl()->GetItemCount() == 3);
        const double rate[3] = { 0.3, 0.55, 0.15 };
        CRect rect;
        logListCtrl_.GetClientRect(rect);
        for (int i = 0; i < 3; ++i)
            logListCtrl_.SetColumnWidth(i, rect.Width() * rate[i]);
        logListCtrl_.SetRedraw(true);
        logListCtrl_.UpdateWindow();
    }
}

///////////////////////////////////////////////////////////////////////////////
LRESULT CServerDialog::OnGetLogicalDriveInfo(WPARAM wParam, LPARAM lParam)
{
    int nIndex = logListCtrl_.InsertItem(0, _T("Get Logical Drive Info"));
    logListCtrl_.SetItemText(nIndex, 1, _T(""));
    logListCtrl_.SetItemText(nIndex, 2, _T(""));

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
LRESULT CServerDialog::OnGetDirectoryInfo(WPARAM wParam, LPARAM lParam)
{
    using ParamType = std::pair<std::wstring, common::file_count_t>;

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
