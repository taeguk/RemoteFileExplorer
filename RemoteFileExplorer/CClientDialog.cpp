// CClientDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "CClientDialog.h"
#include "afxdialogex.h"


// CClientDialog 대화 상자입니다.

IMPLEMENT_DYNAMIC(CClientDialog, CDialogEx)

CClientDialog::CClientDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_CLIENT, pParent)
{

}

CClientDialog::~CClientDialog()
{
}

void CClientDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CClientDialog, CDialogEx)
END_MESSAGE_MAP()


// CClientDialog 메시지 처리기입니다.
