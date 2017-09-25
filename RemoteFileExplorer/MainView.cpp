
// MainView.cpp : CMainView 클래스의 구현
//

#include "stdafx.h"
#include "RemoteFileExplorer.h"
#include "MainView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainView

CMainView::CMainView()
{
}

CMainView::~CMainView()
{
}


BEGIN_MESSAGE_MAP(CMainView, CWnd)
    ON_WM_PAINT()
END_MESSAGE_MAP()



// CMainView 메시지 처리기

BOOL CMainView::PreCreateWindow(CREATESTRUCT& cs)
{
    if (!CWnd::PreCreateWindow(cs))
        return FALSE;

    cs.dwExStyle |= WS_EX_CLIENTEDGE;
    cs.style &= ~WS_BORDER;
    cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
        ::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1), NULL);

    return TRUE;
}

void CMainView::OnPaint()
{
    CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

                       // TODO: 여기에 메시지 처리기 코드를 추가합니다.

                       // 그리기 메시지에 대해서는 CWnd::OnPaint()를 호출하지 마십시오.
}

