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

enum class ServerStatus
{
	STARTED,
	STOPPED
};

// CServerDialog 대화 상자입니다.

class CServerDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CServerDialog)

public:
	CServerDialog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CServerDialog();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SERVER };
#endif

protected:
	virtual BOOL OnInitDialog() override;
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedMfcbuttonServerControl();
	afx_msg LRESULT OnGetLogicalDriveInfo(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetDirectoryInfo(WPARAM wParam, LPARAM lParam);

private:
	static const UINT FeWmBase{ WM_APP }; // file explorer window message base

	// TODO: 동적할당 해야하지 않을까...?
	std::unique_ptr<remoteFileExplorer::server::Server> server_;
	ServerStatus status_{ ServerStatus::STOPPED };
	CMFCButton controlButton_;
	CEdit portEdit_;
	CListCtrl logListCtrl_;
};

} // namespace server
} // namespace mfc
} // namespace remoteFileExplorer