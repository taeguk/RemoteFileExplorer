
// MainView.h : CMainView 클래스의 인터페이스
//


#pragma once


// CMainView 창

class CMainView : public CWnd
{
    // 생성입니다.
public:
    CMainView();

    // 특성입니다.
public:

    // 작업입니다.
public:

    // 재정의입니다.
protected:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

    // 구현입니다.
public:
    virtual ~CMainView();

    // 생성된 메시지 맵 함수
protected:
    afx_msg void OnPaint();
    DECLARE_MESSAGE_MAP()
};

