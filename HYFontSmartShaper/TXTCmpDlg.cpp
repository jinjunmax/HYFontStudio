// TXTCmpDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "HYFontSmartShaper.h"
#include "TXTCmpDlg.h"
#include "afxdialogex.h"


// CTXTCmpDlg 对话框

IMPLEMENT_DYNAMIC(CTXTCmpDlg, CDialogEx)
CTXTCmpDlg::CTXTCmpDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TXTCMP_DLG, pParent)
	, m_strFile1(_T(""))
	, m_strFile2(_T(""))
{

}	// end of CTXTCmpDlg::CTXTCmpDlg()

CTXTCmpDlg::~CTXTCmpDlg()
{

}	// end of CTXTCmpDlg::~CTXTCmpDlg()

void CTXTCmpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_TCMPFL1_RD, m_strFile1);
	DDX_Text(pDX, IDC_TCMPFL2_RD, m_strFile2);
}	// end of void CTXTCmpDlg::DoDataExchange()

BEGIN_MESSAGE_MAP(CTXTCmpDlg, CDialogEx)
	ON_BN_CLICKED(IDC_TCMPFL1_BTN, &CTXTCmpDlg::OnBnClickedTcmpfl1Btn)
	ON_BN_CLICKED(IDC_TCMPFL2_BTN, &CTXTCmpDlg::OnBnClickedTcmpfl2Btn)
	ON_BN_CLICKED(IDC_TCMPDST_BTN, &CTXTCmpDlg::OnBnClickedTcmpdstBtn)
END_MESSAGE_MAP()

// CTXTCmpDlg 消息处理程序
BOOL CTXTCmpDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CheckRadioButton(IDC_TCMPZB_RD, IDC_TCMPNB_RD, IDC_TCMPZB_RD);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE

}	// end of BOOL CTXTCmpDlg::OnInitDialog()

void CTXTCmpDlg::OnBnClickedTcmpfl1Btn()
{
	TCHAR	szFilters[] = _T("TXT(*.txt)|*.txt||");
	CFileDialog  openFileDlg(TRUE, _T(""), _T(""), OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilters);
	if (openFileDlg.DoModal() != IDOK)	return;

	m_strFile1 = openFileDlg.GetPathName();

	UpdateData(FALSE);

}	// end of void CTXTCmpDlg::OnBnClickedTcmpfl1Btn()

void CTXTCmpDlg::OnBnClickedTcmpfl2Btn()
{
	TCHAR	szFilters[] = _T("TXT(*.txt)|*.txt||");
	CFileDialog  openFileDlg(TRUE, _T(""), _T(""), OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilters);
	if (openFileDlg.DoModal() != IDOK)	return;

	m_strFile2 = openFileDlg.GetPathName();

	UpdateData(FALSE);

}	// end of void CTXTCmpDlg::OnBnClickedTcmpfl2Btn()

void CTXTCmpDlg::OnBnClickedTcmpdstBtn()
{
	UpdateData();

	int CtrlID = GetCheckedRadioButton(IDC_TCMPZB_RD, IDC_TCMPNB_RD);
	switch (CtrlID)
	{
	case IDC_TCMPZB_RD:
		break;
	case IDC_TCMPMB_RD:
		break;
	case IDC_TCMPNB_RD:
		break;
	default:
		break;
	}

}	// end of void CTXTCmpDlg::OnBnClickedTcmpdstBtn()

void CTXTCmpDlg::CmprLine(CString strFL1, CString strFL2, std::vector<string>& vtDiff , std::vector<string>& vtSame)
{


}	// end of void CTXTCmpDlg::CmprLine()

void CTXTCmpDlg::CmprChar(CString strFL1, CString strFL2, std::vector<string>& vtDiff, std::vector<string>& vtSame)
{
	
	

}	// end of void CTXTCmpDlg::CmprChar()
