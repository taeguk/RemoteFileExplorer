#pragma once

#include <afxwin.h>
#include <afxcmn.h>
#include <afxbutton.h>

#include "MFC/resource.h"
#include "MFC/Server/FileExplorerWatcher.h"
#include "Server/Server.h"

namespace remoteFileExplorer
{
namespace mfc
{
namespace server
{
///////////////////////////////////////////////////////////////////////////////
enum class ServerStatus
{
    STARTED,
    STOPPED
};

///////////////////////////////////////////////////////////////////////////////
// Server의 UI를 담당한다.
class CServerDialog : public CDialogEx
{
    DECLARE_DYNAMIC(CServerDialog)

public:
    CServerDialog(CWnd* pParent = nullptr);
    virtual ~CServerDialog();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_SERVER };
#endif

protected:
    virtual BOOL OnInitDialog() override;
    virtual void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV 지원입니다.

    DECLARE_MESSAGE_MAP()
    afx_msg void OnBnClickedMfcbuttonServerControl();
    afx_msg LRESULT OnGetLogicalDriveInfo(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnGetDirectoryInfo(WPARAM wParam, LPARAM lParam);

private:
    // File Explorer Watcher로 부터 전달되는 Window Message의 Base 값.
    static const UINT FeWmBase{ WM_APP };

    std::unique_ptr<remoteFileExplorer::server::Server> server_;
    ServerStatus status_{ ServerStatus::STOPPED };
    CMFCButton controlButton_;
    CEdit portEdit_;
    CListCtrl logListCtrl_;
};

} // namespace server
} // namespace mfc
} // namespace remoteFileExplorer