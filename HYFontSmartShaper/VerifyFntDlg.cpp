// VerifyFntDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "HYFontSmartShaper.h"
#include "VerifyFntDlg.h"
#include "afxdialogex.h"


// CVerifyFntDlg �Ի���

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

// CVerifyFntDlg ��Ϣ�������
BOOL CVerifyFntDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	return TRUE;  // return TRUE unless you set the focus to a control
				  
}	// end of BOOL CVerifyFntDlg::OnInitDialog()
