#pragma once

#ifndef __AFXWIN_H__
#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

#include "MFC/resource.h"       // 주 기호입니다.

namespace remoteFileExplorer
{
namespace mfc
{
///////////////////////////////////////////////////////////////////////////////
class CRemoteFileExplorerApp : public CWinApp
{
public:
    CRemoteFileExplorerApp();

    virtual BOOL InitInstance() override;
    virtual int ExitInstance() override;

    UINT  m_nAppLook;

protected:
    afx_msg void OnAppAbout();
    DECLARE_MESSAGE_MAP()
};

extern CRemoteFileExplorerApp theApp;

} // namespace mfc
} // namespace remoteFileExplorer