// MakeTTCDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "HYFontSmartShaper.h"
#include "MakeTTCDlg.h"
#include "afxdialogex.h"
#include "HYTTC.h"


// CMakeTTCDlg 对话框

IMPLEMENT_DYNAMIC(CMakeTTCDlg, CDialog)

CMakeTTCDlg::CMakeTTCDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_MTTC_DLG, pParent)
{

}	// end of CMakeTTCDlg::CMakeTTCDlg()

CMakeTTCDlg::~CMakeTTCDlg()
{

}	// end of CMakeTTCDlg::~CMakeTTCDlg()

void CMakeTTCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_MTTC_FNTS_LST, m_lstFnts);
}	// end of void CMakeTTCDlg::DoDataExchange()

BEGIN_MESSAGE_MAP(CMakeTTCDlg, CDialog)
	ON_BN_CLICKED(IDC_MTTC_ADD_BTN, &CMakeTTCDlg::OnBnClickedMttcAddBtn)
	ON_BN_CLICKED(IDC_MTTC_DEL_BTN, &CMakeTTCDlg::OnBnClickedMttcDelBtn)	
	ON_BN_CLICKED(IDC_MTTC_MKE_BTN, &CMakeTTCDlg::OnBnClickedMttcMkeBtn)
	ON_LBN_SELCHANGE(IDC_MTTC_FNTS_LST, &CMakeTTCDlg::OnLbnSelchangeMttcFntsLst)
END_MESSAGE_MAP()

// CMakeTTCDlg 消息处理程序
void CMakeTTCDlg::OnBnClickedMttcAddBtn()
{	
	TCHAR	szFilters[] = _T("Font File(*.ttf;*otf)|*.ttf;*.otf||");
	CFileDialog  openFileDlg(TRUE, _T(""), _T(""), OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilters);
	if (openFileDlg.DoModal() != IDOK)	return;

	CString strFileName = openFileDlg.GetPathName();
	m_vtFntName.push_back((LPCTSTR)(LPCSTR)strFileName);

	string str = HY_GetFileNameFromPath((LPCTSTR)(LPCSTR)strFileName);	
	m_lstFnts.AddString(str.c_str());	

}	//end of void CMakeTTCDlg::OnBnClickedMttcAddBtn()

void CMakeTTCDlg::OnBnClickedMttcDelBtn()
{
	int iIndx = m_lstFnts.GetCurSel();
	if (iIndx == LB_ERR) return;

	m_vtFntName.erase(m_vtFntName.begin()+ iIndx);
	m_lstFnts.DeleteString(iIndx);

	this->UpdateData(FALSE);
	
}	// end of void CMakeTTCDlg::OnBnClickedMttcDelBtn()

void CMakeTTCDlg::OnBnClickedMttcMkeBtn()
{
	TCHAR	szFilters[] = _T("TTC 文件(*.ttc)|*.ttc||");
	CFileDialog  saveFileDlg(FALSE, _T(""), _T(""), OFN_LONGNAMES, szFilters);
	if (saveFileDlg.DoModal() != IDOK)	return;

	CString strTTCFileName = saveFileDlg.GetPathName();

	CWaitCursor wc;

	CHYTTC	ttc;
	int iRet = ttc.Font2TTC((LPCTSTR)(LPCSTR)strTTCFileName, m_vtFntName);

	if (iRet == HY_NOERROR)
	{
		AfxMessageBox("ttc 文件生成完成。");
	}
	else
	{
		AfxMessageBox("ttc 文件生成失败！");
	}

}	// end of void CMakeTTCDlg::OnBnClickedMttcMkeBtn()

void CMakeTTCDlg::OnLbnSelchangeMttcFntsLst()
{
	
}	// end of void CMakeTTCDlg::OnLbnSelchangeMttcFntsLst()

void CMakeTTCDlg::UpdateListCtrl()
{


}	// end of void CMakeTTCDlg::UpdateListCtrl()

BOOL CMakeTTCDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE

}	// end of BOOL CMakeTTCDlg::OnInitDialog()
