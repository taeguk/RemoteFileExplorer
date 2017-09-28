#pragma once

#include <afxwin.h>
#include <afxcmn.h>
#include <afxbutton.h>

#include "MFC/resource.h"
#include "Client/RemoteFileExplorer.h"

namespace remoteFileExplorer
{
namespace mfc
{
namespace client
{

enum class ClientStatus
{
	CONNECTED,
	DISCONNECTED
};

// CClientDialog 대화 상자입니다.

class CClientDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CClientDialog)

public:
	CClientDialog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CClientDialog();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CLIENT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedMfcbuttonClientControl();
	afx_msg void OnBnClickedMfcbuttonClientEcho();

private:
	remoteFileExplorer::client::RemoteFileExplorer remoteFileExplorer_;
	ClientStatus status_{ ClientStatus::DISCONNECTED };
	CIPAddressCtrl ipAddressCtrl_;
	CEdit portEdit_;
	CMFCButton controlButton_;
	CEdit echoStringEdit_;
	CMFCButton echoButton_;
	CListBox echoResultListBox_;
};

} // namespace client
} // namespace mfc
} // namespace remoteFileExplorer