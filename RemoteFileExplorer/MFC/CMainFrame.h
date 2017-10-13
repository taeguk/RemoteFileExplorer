#pragma once

#include "MFC/CMainView.h"

namespace remoteFileExplorer
{
namespace mfc
{
///////////////////////////////////////////////////////////////////////////////
class CMainFrame : public CFrameWnd
{
    DECLARE_DYNAMIC(CMainFrame)

public:
    CMainFrame();
    virtual ~CMainFrame();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual BOOL OnCmdMsg(
        UINT nID,
        int nCode,
        void* pExtra,
        AFX_CMDHANDLERINFO* pHandlerInfo);

    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSetFocus(CWnd *pOldWnd);
    afx_msg void OnApplicationLook(UINT id);
    afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
    DECLARE_MESSAGE_MAP()

private:
    CStatusBar statusBar_;
    CMainView* mainView_;
};

} // namespace mfc
} // namespace remoteFileExplorer