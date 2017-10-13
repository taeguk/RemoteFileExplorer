#include "MFC/stdafx.h"

#include "MFC/CMainView.h"

#include "MFC/resource.h"

namespace remoteFileExplorer
{
namespace mfc
{
IMPLEMENT_DYNCREATE(CMainView, CFormView)

///////////////////////////////////////////////////////////////////////////////
CMainView::CMainView()
    : CFormView(IDD_MAINVIEW)
{
}

///////////////////////////////////////////////////////////////////////////////
CMainView::~CMainView()
{
}

///////////////////////////////////////////////////////////////////////////////
void CMainView::DoDataExchange(CDataExchange* pDX)
{
    CFormView::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_BUTTON_SERVER, serverButton_);
    DDX_Control(pDX, IDC_BUTTON_CLIENT, clientButton_);
}

///////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void CMainView::AssertValid() const
{
    CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CMainView::Dump(CDumpContext& dc) const
{
    CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

///////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CMainView, CFormView)
    ON_WM_SIZE()
    ON_BN_CLICKED(IDC_BUTTON_SERVER, &CMainView::OnBnClickedButtonServer)
    ON_BN_CLICKED(IDC_BUTTON_CLIENT, &CMainView::OnBnClickedButtonClient)
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
void CMainView::OnSize(UINT nType, int cx, int cy)
{
    CFormView::OnSize(nType, cx, cy);

    CRect clientRect;

    GetClientRect(&clientRect);

    switch (programMode_)
    {
    case ProgramMode::ServerMode:
        serverDialog_.MoveWindow(&clientRect);
        break;
    case ProgramMode::ClientMode:
        clientDialog_.MoveWindow(&clientRect);
        break;
    default:
        ;/* Nothing to do */
    }
}

///////////////////////////////////////////////////////////////////////////////
void CMainView::OnBnClickedButtonServer()
{
    CRect clientRect;
    GetClientRect(&clientRect);

    serverButton_.ShowWindow(SW_HIDE);
    clientButton_.ShowWindow(SW_HIDE);

    serverDialog_.Create(server::CServerDialog::IDD, this);
    serverDialog_.ShowWindow(SW_SHOW);
    serverDialog_.MoveWindow(&clientRect);
    serverDialog_.SetFocus();

    programMode_ = ProgramMode::ServerMode;
}

///////////////////////////////////////////////////////////////////////////////
void CMainView::OnBnClickedButtonClient()
{
    CRect clientRect;
    GetClientRect(&clientRect);

    serverButton_.ShowWindow(SW_HIDE);
    clientButton_.ShowWindow(SW_HIDE);

    clientDialog_.Create(client::CClientDialog::IDD, this);
    clientDialog_.ShowWindow(SW_SHOW);
    clientDialog_.MoveWindow(&clientRect);
    clientDialog_.SetFocus();

    programMode_ = ProgramMode::ClientMode;
}

} // namespace mfc
} // namespace remoteFileExplorer