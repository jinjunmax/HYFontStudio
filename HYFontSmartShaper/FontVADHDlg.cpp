// FontVADHDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "HYFontSmartShaper.h"
#include "FontVADHDlg.h"
#include "afxdialogex.h"


// CFontVADHDlg 对话框
IMPLEMENT_DYNAMIC(CFontVADHDlg, CDialog)

CFontVADHDlg::CFontVADHDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_FNT_VRADH_DLG, pParent)	
	, m_uiTsb(0)
	, m_strSrcFntFile(_T(""))
	, m_strPSNameFile(_T(""))
{

}	// end of CFontVADHDlg::CFontVADHDlg()

CFontVADHDlg::~CFontVADHDlg()
{
}	// end of CFontVADHDlg::~CFontVADHDlg()

void CFontVADHDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_VADH_TSB_EDT, m_uiTsb);

	DDX_Text(pDX, IDC_VADH_SFNT_EDT, m_strSrcFntFile);
	DDX_Text(pDX, IDC_VADH_PSNM_EDT, m_strPSNameFile);
}	// end of void CFontVADHDlg::DoDataExchange()

BEGIN_MESSAGE_MAP(CFontVADHDlg, CDialog)
	ON_BN_CLICKED(IDC_VADH_SFNT_BTN, &CFontVADHDlg::OnBnClickedVadhSfntBtn)	
	ON_BN_CLICKED(IDC_VADH_BUDL_BTN, &CFontVADHDlg::OnBnClickedVadhBudlBtn)
	ON_BN_CLICKED(IDC_VADH_PSNM_CHK, &CFontVADHDlg::OnBnClickedVadhPsnmChk)
	ON_BN_CLICKED(IDC_VADH_PSNM_BTN, &CFontVADHDlg::OnBnClickedVadhPsnmBtn)
END_MESSAGE_MAP()

// CFontVADHDlg 消息处理程序
void CFontVADHDlg::OnBnClickedVadhSfntBtn()
{
	UpdateData();

	TCHAR	szFilters[] = _T("TrueType 文件(*.ttf)|*.ttf||");
	CFileDialog  openFileDlg(TRUE, _T(""), _T(""), OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilters);
	if (openFileDlg.DoModal() != IDOK)	return;

	m_strSrcFntFile = openFileDlg.GetPathName();

	UpdateData(FALSE);
	
}	// end of void CFontVADHDlg::OnBnClickedVadhSfntBtn()

void CFontVADHDlg::OnBnClickedVadhPsnmBtn()
{
	UpdateData();

	TCHAR	szFilters[] = _T("PSName 文件(*.txt)|*.txt||");
	CFileDialog  openFileDlg(TRUE, _T(""), _T(""), OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilters);
	if (openFileDlg.DoModal() != IDOK)	return;

	m_strPSNameFile = openFileDlg.GetPathName();

	UpdateData(FALSE);

}	// end of void CFontVADHDlg::OnBnClickedVadhPsnmBtn()

void CFontVADHDlg::OnBnClickedVadhBudlBtn()
{
	CHAR szFilters[] = _T("true type 文件(*.ttf)|*.ttf||");
	CFileDialog  saveFileDlg(FALSE, _T(""), _T(""), OFN_LONGNAMES, szFilters);
	if (saveFileDlg.DoModal() != IDOK)	return;
	CString strDstFileName = saveFileDlg.GetPathName();

	if (m_strSrcFntFile == "")	{
		AfxMessageBox("源字库为空");
		return;
	}

	UpdateData();

	CWaitCursor	wc;
	HYFONTCODEC::CFontExtract	FontExtract;
	int iRt;
	if (IsDlgButtonChecked(IDC_VADH_PSNM_CHK)){ 
		if (m_strPSNameFile == "") {
			AfxMessageBox("源字库为空");
			return;
		}
		iRt = FontExtract.ModifyVMTX3((LPSTR)(LPCSTR)m_strSrcFntFile, (LPSTR)(LPCSTR)m_strPSNameFile,(LPSTR)(LPCSTR)strDstFileName, m_uiTsb);
	}
	else {
		iRt = FontExtract.ModifyVMTX2((LPSTR)(LPCSTR)m_strSrcFntFile, (LPSTR)(LPCSTR)strDstFileName, m_uiTsb);
	}

	if (iRt == HY_NOERROR){
		AfxMessageBox("字库处理成功！");
	} 
	else{
		AfxMessageBox("字库处理失败！");
	}

}	// end of void CFontVADHDlg::OnBnClickedVadhBudlBtn()

BOOL CFontVADHDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CheckDlgButton(IDC_VADH_PSNM_CHK, BST_CHECKED);

	return TRUE;  
				  
}	// end of BOOL CFontVADHDlg::OnInitDialog()

void CFontVADHDlg::OnBnClickedVadhPsnmChk()
{
	UINT isCheck = IsDlgButtonChecked(IDC_VADH_PSNM_CHK);
	if(isCheck>0){
		GetDlgItem(IDC_VADH_PSNM_EDT)->EnableWindow();
		GetDlgItem(IDC_VADH_PSNM_EDT)->EnableWindow();
		GetDlgItem(IDC_VADH_PSNM_STC)->EnableWindow();		
	}
	else {
		GetDlgItem(IDC_VADH_PSNM_EDT)->EnableWindow(FALSE);
		GetDlgItem(IDC_VADH_PSNM_EDT)->EnableWindow(FALSE);
		GetDlgItem(IDC_VADH_PSNM_STC)->EnableWindow(FALSE);
	}

}	// end void CFontVADHDlg::OnBnClickedVadhPsnmChk()




