// VFntSetUpDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "HYFontSmartShaper.h"
#include "VFntSetUpDlg.h"
#include "afxdialogex.h"


// CVFntSetUpDlg 对话框

IMPLEMENT_DYNAMIC(CVFntSetUpDlg, CDialog)

CVFntSetUpDlg::CVFntSetUpDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_VFNT_AXIS_DLG, pParent)
{

}

CVFntSetUpDlg::~CVFntSetUpDlg()
{
}

void CVFntSetUpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CVFntSetUpDlg, CDialog)
END_MESSAGE_MAP()


// CVFntSetUpDlg 消息处理程序
