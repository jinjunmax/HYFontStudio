// FontSetupCFFProPage.cpp : implementation file
//

#include "stdafx.h"
#include "HYFontSmartShaper.h"
#include "FontSetupCFFProPage.h"


// CFontSetupCFFProPage dialog

IMPLEMENT_DYNAMIC(CFontSetupCFFProPage, CPropertyPage)

CFontSetupCFFProPage::CFontSetupCFFProPage()
	: CPropertyPage(CFontSetupCFFProPage::IDD)
{	

}	// end of CFontSetupCFFProPage::CFontSetupCFFProPage()

CFontSetupCFFProPage::~CFontSetupCFFProPage()
{
}	// end of CFontSetupCFFProPage::~CFontSetupCFFProPage()

void CFontSetupCFFProPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

}	// end of void CFontSetupCFFProPage::DoDataExchange()

BEGIN_MESSAGE_MAP(CFontSetupCFFProPage, CPropertyPage)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_TSC_ISCIDY_RAD, &CFontSetupCFFProPage::OnBnClickedTscIscidyRad)
	ON_BN_CLICKED(IDC_TSC_ISCIDN_RAD, &CFontSetupCFFProPage::OnBnClickedTscIscidnRad)
	ON_EN_CHANGE(IDC_TSC_RGST_EDIT, &CFontSetupCFFProPage::OnEnChangeTscRgstEdit)
	ON_EN_CHANGE(IDC_TSC_ORDER_EDIT, &CFontSetupCFFProPage::OnEnChangeTscOrderEdit)	
	ON_EN_CHANGE(IDC_TSC_SUPPLE_EDIT, &CFontSetupCFFProPage::OnEnChangeTscSuppleEdit)	
END_MESSAGE_MAP()

// CFontSetupCFFProPage message handlers
BOOL CFontSetupCFFProPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	InitCtrl();

	return TRUE;  // return TRUE unless you set the focus to a control

}	// end of BOOL CFontSetupCFFProPage::OnInitDialog()

void CFontSetupCFFProPage::OnDestroy()
{
	CPropertyPage::OnDestroy();

}	// end of void CFontSetupCFFProPage::OnDestroy()

void CFontSetupCFFProPage::OnBnClickedTscIscidyRad()
{
	GetDlgItem(IDC_TSC_RGST_EDIT)->EnableWindow(TRUE);
	GetDlgItem(IDC_TSC_ORDER_EDIT)->EnableWindow(TRUE);
	GetDlgItem(IDC_TSC_SUPPLE_EDIT)->EnableWindow(TRUE);
	CheckRadioButton(IDC_TSC_ISCIDY_RAD, IDC_TSC_ISCIDN_RAD,IDC_TSC_ISCIDY_RAD);
	::XSysproxy().m_tagOpeionPrm.bCIDFnt = TRUE;	

}	// end of void CFontSetupCFFProPage::OnBnClickedTscIscidyRad()

void CFontSetupCFFProPage::OnBnClickedTscIscidnRad()
{
	GetDlgItem(IDC_TSC_RGST_EDIT)->EnableWindow(FALSE);
	GetDlgItem(IDC_TSC_ORDER_EDIT)->EnableWindow(FALSE);
	GetDlgItem(IDC_TSC_SUPPLE_EDIT)->EnableWindow(FALSE);
	CheckRadioButton(IDC_TSC_ISCIDY_RAD, IDC_TSC_ISCIDN_RAD,IDC_TSC_ISCIDN_RAD);	
	::XSysproxy().m_tagOpeionPrm.bCIDFnt = FALSE;

}	// end of void CFontSetupCFFProPage::OnBnClickedTscIscidnRad()

void CFontSetupCFFProPage::OnEnChangeTscRgstEdit()
{	
	CString str;
	GetDlgItem(IDC_TSC_RGST_EDIT)->GetWindowText(str);
	ZeroMemory(::XSysproxy().m_tagOpeionPrm.Registry, MAX_PATH);
	strcpy_s(::XSysproxy().m_tagOpeionPrm.Registry, MAX_PATH,(LPTSTR)(LPCTSTR)str);

}	// end of void CFontSetupCFFProPage::OnBnClickedTscIscidnRad()

void CFontSetupCFFProPage::OnEnChangeTscOrderEdit()
{
	CString str;
	GetDlgItem(IDC_TSC_ORDER_EDIT)->GetWindowText(str);
	ZeroMemory(::XSysproxy().m_tagOpeionPrm.Ordering, MAX_PATH);
	strcpy_s(::XSysproxy().m_tagOpeionPrm.Ordering, MAX_PATH, (LPTSTR)(LPCTSTR)str);

}	// end of void CFontSetupCFFProPage::OnEnChangeTscOrderEdit()

void CFontSetupCFFProPage::OnEnChangeTscSuppleEdit()
{
	CString str;
	GetDlgItem(IDC_TSC_SUPPLE_EDIT)->GetWindowText(str);
	ZeroMemory(::XSysproxy().m_tagOpeionPrm.Supplement, MAX_PATH);
	strcpy_s(::XSysproxy().m_tagOpeionPrm.Supplement, MAX_PATH, (LPTSTR)(LPCTSTR)str);

}	// end of void CFontSetupCFFProPage::OnEnChangeTscSuppleEdit()

void CFontSetupCFFProPage::InitCtrl()
{
	if (::XSysproxy().m_tagOpeionPrm.bCIDFnt){
 		GetDlgItem(IDC_TSC_RGST_EDIT)->EnableWindow(TRUE);
 		GetDlgItem(IDC_TSC_ORDER_EDIT)->EnableWindow(TRUE);
 		GetDlgItem(IDC_TSC_SUPPLE_EDIT)->EnableWindow(TRUE);
 		CheckRadioButton(IDC_TSC_ISCIDY_RAD, IDC_TSC_ISCIDN_RAD,IDC_TSC_ISCIDY_RAD);
 	}
	else {
		GetDlgItem(IDC_TSC_RGST_EDIT)->EnableWindow(TRUE);
		GetDlgItem(IDC_TSC_ORDER_EDIT)->EnableWindow(TRUE);
		GetDlgItem(IDC_TSC_SUPPLE_EDIT)->EnableWindow(TRUE);
		CheckRadioButton(IDC_TSC_ISCIDY_RAD, IDC_TSC_ISCIDN_RAD,IDC_TSC_ISCIDN_RAD);
	}

	GetDlgItem(IDC_TSC_RGST_EDIT)->SetWindowText(::XSysproxy().m_tagOpeionPrm.Registry);
	GetDlgItem(IDC_TSC_ORDER_EDIT)->SetWindowText(::XSysproxy().m_tagOpeionPrm.Ordering);
 	GetDlgItem(IDC_TSC_SUPPLE_EDIT)->SetWindowText(::XSysproxy().m_tagOpeionPrm.Supplement);

}	// end of void	CFontSetupCFFProPage::InitCtrl()

void CFontSetupCFFProPage::UninitCtrl()
{

}	// end of void	CFontSetupCFFProPage::UninitCtrl()

void CFontSetupCFFProPage::Save()
{
	if (m_hWnd) {
		UpdateData();

		CString str;
		GetDlgItem(IDC_TSC_RGST_EDIT)->GetWindowText(str);
		ZeroMemory(::XSysproxy().m_tagOpeionPrm.Registry, MAX_PATH);
		strcpy_s(::XSysproxy().m_tagOpeionPrm.Registry, MAX_PATH, (LPTSTR)(LPCTSTR)str);

		GetDlgItem(IDC_TSC_ORDER_EDIT)->GetWindowText(str);
		ZeroMemory(::XSysproxy().m_tagOpeionPrm.Ordering, MAX_PATH);
		strcpy_s(::XSysproxy().m_tagOpeionPrm.Ordering, MAX_PATH, (LPTSTR)(LPCTSTR)str);

		GetDlgItem(IDC_TSC_SUPPLE_EDIT)->GetWindowText(str);
		ZeroMemory(::XSysproxy().m_tagOpeionPrm.Supplement, MAX_PATH);
		strcpy_s(::XSysproxy().m_tagOpeionPrm.Supplement, MAX_PATH, (LPTSTR)(LPCTSTR)str);
	}
}	// end of void CFontSetupCFFProPage::SetFontEndoc()





