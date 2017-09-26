// CServerDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "CServerDialog.h"
#include "afxdialogex.h"


// CServerDialog 대화 상자입니다.

IMPLEMENT_DYNAMIC(CServerDialog, CDialogEx)

CServerDialog::CServerDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SERVER, pParent)
{

}

CServerDialog::~CServerDialog()
{
}

void CServerDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CServerDialog, CDialogEx)
END_MESSAGE_MAP()


// CServerDialog 메시지 처리기입니다.
