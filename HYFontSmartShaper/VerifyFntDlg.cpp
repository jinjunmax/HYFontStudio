// VerifyFntDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "HYFontSmartShaper.h"
#include "VerifyFntDlg.h"
#include "afxdialogex.h"


// CVerifyFntDlg 对话框

IMPLEMENT_DYNAMIC(CVerifyFntDlg, CDialogEx)

CVerifyFntDlg::CVerifyFntDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_VERIFYFNT_DLG, pParent)
{

}

CVerifyFntDlg::~CVerifyFntDlg()
{
}

void CVerifyFntDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CVerifyFntDlg, CDialogEx)
END_MESSAGE_MAP()

// CVerifyFntDlg 消息处理程序
BOOL CVerifyFntDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
				  
}	// end of BOOL CVerifyFntDlg::OnInitDialog()
