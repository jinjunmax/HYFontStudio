// FontSetupOptionPage.cpp : 实现文件
//

#include "stdafx.h"
#include "HYFontSmartShaper.h"
#include "FontSetupOptionPage.h"
#include "afxdialogex.h"


// CFontSetupOptionPage 对话框

IMPLEMENT_DYNAMIC(CFontSetupOptionPage, CPropertyPage)

CFontSetupOptionPage::CFontSetupOptionPage()
	: CPropertyPage(IDD_FNT_SETUP_OPTION_DLG)	
	, m_strCHSFaimlyName(_T(""))
	, m_strCHSSubName(_T(""))
	, m_strENGFamilyName(_T(""))
	, m_strENGSubName(_T(""))
	, m_strVersion(_T(""))
	, m_bRename(FALSE)	
	, m_bCustomCmap(TRUE)
	, m_bSetAdw(FALSE)
	, m_bSetAdh(FALSE)
	, m_bLayout(FALSE)
	, m_bCorrect(FALSE)
	, m_bHanyi(TRUE)
	, m_cmtADH(0)
	, m_bOldStandard(FALSE)
{
	m_bReVert = TRUE;
	m_bRePsName = FALSE;
	m_bReSortUni = FALSE;
	m_bYiTiZi = FALSE;
	m_brmPoint = FALSE;
	m_pHYFoncCodec = NULL;

}	// end of CFontSetupOptionPage::CFontSetupOptionPage()

CFontSetupOptionPage::~CFontSetupOptionPage()
{

}	// end of CFontSetupOptionPage::~CFontSetupOptionPage()

void CFontSetupOptionPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Check(pDX, IDC_PTN_VERT_CHK, m_bReVert);
	DDX_Check(pDX, IDC_PTN_PSNM_CHK, m_bRePsName);
	DDX_Check(pDX, IDC_PTN_SRTUNI_CHK, m_bReSortUni);
	DDX_Check(pDX, IDC_PTN_YITIZI_CHK, m_bYiTiZi);
	DDX_Check(pDX, IDC_PTN_RMRPPNT_CHK, m_brmPoint);	
	DDX_Check(pDX, IDC_PTN_RENAME_CHK, m_bRename);
	DDX_Check(pDX, IDC_PTN_CMP_CHK, m_bCustomCmap);
	DDX_Check(pDX, IDC_PTN_SETADW_CHK, m_bSetAdw);
	DDX_Check(pDX, IDC_PTN_SETADH_CHK, m_bSetAdh);
	DDX_Text(pDX, IDC_PTN_CHS_EDT, m_strCHSFaimlyName);
	DDX_Text(pDX, IDC_PTN_CHSSUB_EDT, m_strCHSSubName);
	DDX_Text(pDX, IDC_PTN_ENG_EDT, m_strENGFamilyName);
	DDX_Text(pDX, IDC_PTN_ENGSUB_EDT, m_strENGSubName);
	DDX_Text(pDX, IDC_PTN_VER_EDT, m_strVersion);
	DDX_Check(pDX, IDC_PTN_LYT_CHK, m_bLayout);
	DDX_Check(pDX, IDC_PTN_CORRECT_CHK, m_bCorrect);
	DDX_Check(pDX, IDC_PTN_HYSND_CHK, m_bHanyi);
	DDX_Check(pDX, IDC_PTN_KANGXI_CHK, m_bKangxi);
	DDX_Text(pDX, IDC_PTN_SETADH_EDT, m_cmtADH);
	DDX_Check(pDX, IDC_PTN_OLDSTND_CHK, m_bOldStandard);
}	// end of void CFontSetupOptionPage::DoDataExchange()

BEGIN_MESSAGE_MAP(CFontSetupOptionPage, CPropertyPage)
	ON_WM_DESTROY()	
	ON_BN_CLICKED(IDC_PTN_RENAME_CHK, &CFontSetupOptionPage::OnBnClickedPtnRenameChk)	
	ON_BN_CLICKED(IDC_PTN_SETADH_CHK, &CFontSetupOptionPage::OnBnClickedPtnSetadhChk)
	ON_BN_CLICKED(IDC_PTN_OLDSTND_CHK, &CFontSetupOptionPage::OnBnClickedPtnOldstndChk)
END_MESSAGE_MAP()

void	CFontSetupOptionPage::Init()
{
	m_bReVert	=	::XSysproxy().m_tagOpeionPrm.bCmplVert;
	m_bRePsName	=	::XSysproxy().m_tagOpeionPrm.bRePsName;
	m_bReSortUni=	::XSysproxy().m_tagOpeionPrm.bReSortUni;
	m_bYiTiZi	=	::XSysproxy().m_tagOpeionPrm.bYitizi;	
	m_bRename	=	::XSysproxy().m_tagOpeionPrm.bFontname;	
	m_bCustomCmap = ::XSysproxy().m_tagOpeionPrm.bCmplCMAP;
	m_bSetAdw	= ::XSysproxy().m_tagOpeionPrm.bsetADW;
	m_bSetAdh	= ::XSysproxy().m_tagOpeionPrm.bsetADH;
	m_bOldStandard = ::XSysproxy().m_tagOpeionPrm.bOldStandard;
	if (m_bSetAdh)
	{
		GetDlgItem(IDC_PTN_SETADH_EDT)->EnableWindow();
	}
	else
	{
		GetDlgItem(IDC_PTN_SETADH_EDT)->CloseWindow();
	}
	m_cmtADH = ::XSysproxy().m_tagOpeionPrm.usSetADH;

	m_bLayout = ::XSysproxy().m_tagOpeionPrm.bCmplLayout;
	m_bCorrect = ::XSysproxy().m_tagOpeionPrm.bCnturCorrect;
	m_bHanyi = ::XSysproxy().m_tagOpeionPrm.bHanyi;
	m_bKangxi = ::XSysproxy().m_tagOpeionPrm.bKangXi;
	if (m_bRename)	{
		GetDlgItem(IDC_PTN_CHS_EDT)->EnableWindow();
		GetDlgItem(IDC_PTN_CHSSUB_EDT)->EnableWindow();
		GetDlgItem(IDC_PTN_ENG_EDT)->EnableWindow();
		GetDlgItem(IDC_PTN_ENGSUB_EDT)->EnableWindow();
		GetDlgItem(IDC_PTN_VER_EDT)->EnableWindow();
	}
	else{
		GetDlgItem(IDC_PTN_CHS_EDT)->EnableWindow(FALSE);
		GetDlgItem(IDC_PTN_CHSSUB_EDT)->EnableWindow(FALSE);
		GetDlgItem(IDC_PTN_ENG_EDT)->EnableWindow(FALSE);
		GetDlgItem(IDC_PTN_ENGSUB_EDT)->EnableWindow(FALSE);
		GetDlgItem(IDC_PTN_VER_EDT)->EnableWindow(FALSE);
	}

	m_strCHSFaimlyName = ::XSysproxy().m_tagOpeionPrm.CHSFaimlyName;
	m_strCHSSubName = ::XSysproxy().m_tagOpeionPrm.CHSSubName;
	m_strENGFamilyName = ::XSysproxy().m_tagOpeionPrm.ENGFaimlyName;
	m_strENGSubName = ::XSysproxy().m_tagOpeionPrm.ENGSubName;
	m_strVersion = ::XSysproxy().m_tagOpeionPrm.Version;

}	// end of void	CFontSetupOptionPage::Init()

void	CFontSetupOptionPage::Uninit()
{

}	// end of void	CFontSetupOptionPage::Uninit()

void	CFontSetupOptionPage::Save()
{	
	if (m_hWnd) {
		UpdateData();

		::XSysproxy().m_tagOpeionPrm.bCmplVert = m_bReVert;
		::XSysproxy().m_tagOpeionPrm.bRePsName = m_bRePsName;
		::XSysproxy().m_tagOpeionPrm.bReSortUni = m_bReSortUni;
		::XSysproxy().m_tagOpeionPrm.bYitizi = m_bYiTiZi;
		::XSysproxy().m_tagOpeionPrm.bFontname = m_bRename;		
		::XSysproxy().m_tagOpeionPrm.bCmplCMAP = m_bCustomCmap;
		::XSysproxy().m_tagOpeionPrm.bsetADW = m_bSetAdw;
		::XSysproxy().m_tagOpeionPrm.bsetADH = m_bSetAdh;
		::XSysproxy().m_tagOpeionPrm.bCmplLayout = m_bLayout;
		::XSysproxy().m_tagOpeionPrm.bCnturCorrect = m_bCorrect;	
		::XSysproxy().m_tagOpeionPrm.bHanyi = m_bHanyi;
		::XSysproxy().m_tagOpeionPrm.bKangXi = m_bKangxi;
		::XSysproxy().m_tagOpeionPrm.usSetADH = m_cmtADH;
		::XSysproxy().m_tagOpeionPrm.bOldStandard = m_bOldStandard;
		if (m_bRename) {
			ZeroMemory(::XSysproxy().m_tagOpeionPrm.CHSFaimlyName,MAX_PATH);
			strcpy(::XSysproxy().m_tagOpeionPrm.CHSFaimlyName, (LPCSTR)(LPCTSTR)m_strCHSFaimlyName);
			ZeroMemory(::XSysproxy().m_tagOpeionPrm.CHSSubName, MAX_PATH);
			strcpy(::XSysproxy().m_tagOpeionPrm.CHSSubName, (LPCSTR)(LPCTSTR)m_strCHSSubName);			
			ZeroMemory(::XSysproxy().m_tagOpeionPrm.ENGFaimlyName, MAX_PATH);
			strcpy(::XSysproxy().m_tagOpeionPrm.ENGFaimlyName, (LPCSTR)(LPCTSTR)m_strENGFamilyName);			 
			ZeroMemory(::XSysproxy().m_tagOpeionPrm.ENGSubName, MAX_PATH);
			strcpy(::XSysproxy().m_tagOpeionPrm.ENGSubName, (LPCSTR)(LPCTSTR)m_strENGSubName);
			ZeroMemory(::XSysproxy().m_tagOpeionPrm.Version, MAX_PATH);
			strcpy(::XSysproxy().m_tagOpeionPrm.Version, (LPCSTR)(LPCTSTR)m_strVersion);			
		}
	}	

}	// end of void	CFontSetupOptionPage::Save()

// CFontSetupOptionPage 消息处理程序
BOOL CFontSetupOptionPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	Init();

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE

}	// end of BOOL CFontSetupOptionPage::OnInitDialog()

void CFontSetupOptionPage::OnDestroy()
{
	CPropertyPage::OnDestroy();

	// TODO: 在此处添加消息处理程序代码

}	// end of void CFontSetupOptionPage::OnDestroy()

void CFontSetupOptionPage::OnBnClickedPtnRenameChk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	if (m_bRename) {
		GetDlgItem(IDC_PTN_CHS_EDT)->EnableWindow();
		GetDlgItem(IDC_PTN_CHSSUB_EDT)->EnableWindow();
		GetDlgItem(IDC_PTN_ENG_EDT)->EnableWindow();
		GetDlgItem(IDC_PTN_ENGSUB_EDT)->EnableWindow();
		GetDlgItem(IDC_PTN_VER_EDT)->EnableWindow();
	}
	else {
		GetDlgItem(IDC_PTN_CHS_EDT)->EnableWindow(FALSE);
		GetDlgItem(IDC_PTN_CHSSUB_EDT)->EnableWindow(FALSE);
		GetDlgItem(IDC_PTN_ENG_EDT)->EnableWindow(FALSE);
		GetDlgItem(IDC_PTN_ENGSUB_EDT)->EnableWindow(FALSE);
		GetDlgItem(IDC_PTN_VER_EDT)->EnableWindow(FALSE);
	}

} // end of void CFontSetupOptionPage::OnBnClickedPtnRenameChk()

void CFontSetupOptionPage::OnBnClickedPtnSetadhChk()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_bSetAdh)
	{
		GetDlgItem(IDC_PTN_SETADH_EDT)->EnableWindow();
	}
	else
	{
		GetDlgItem(IDC_PTN_SETADH_EDT)->CloseWindow();
	}
}	// end of void CFontSetupOptionPage::OnBnClickedPtnSetadhChk()

void CFontSetupOptionPage::OnBnClickedPtnOldstndChk()
{
	// TODO: 在此添加控件通知处理程序代码

}	// end of void CFontSetupOptionPage::OnBnClickedPtnOldstndChk()
