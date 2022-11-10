// FontSetupEMProPage.cpp : implementation file
//

#include "stdafx.h"
#include "HYFontSmartShaper.h"
#include "FontSetupEMProPage.h"


// CTTFSetupEMProPage dialog
IMPLEMENT_DYNAMIC(CTTFSetupEMProPage, CPropertyPage)

CTTFSetupEMProPage::CTTFSetupEMProPage()
	: CPropertyPage(CTTFSetupEMProPage::IDD)	
{	

}	// end of CTTFSetupEMProPage::CTTFSetupEMProPage()

CTTFSetupEMProPage::~CTTFSetupEMProPage()
{

}	// end of CTTFSetupEMProPage::~CTTFSetupEMProPage()

void CTTFSetupEMProPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

}	// end of void CTTFSetupEMProPage::DoDataExchange()

BEGIN_MESSAGE_MAP(CTTFSetupEMProPage, CPropertyPage)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// CTTFSetupEMProPage message handlers
BOOL CTTFSetupEMProPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	CString strInf;
	
	CWnd*  pWnd = GetDlgItem(IDC_TSTEM_HD_EM_EDT);
	strInf.Format(_T("%d"),::XSysproxy().m_tagOpeionPrm.usUnitsPerEM);
	pWnd->SetWindowText(strInf);
	pWnd = GetDlgItem(IDC_TSTEM_OS_WEIGHT_EDT);
	strInf.Format(_T("%d"), ::XSysproxy().m_tagOpeionPrm.usWeight);
	pWnd->SetWindowText(strInf);
	pWnd = GetDlgItem(IDC_TSTEM_OS_WIDTH_EDT);
	strInf.Format(_T("%d"), ::XSysproxy().m_tagOpeionPrm.usWidth);
	pWnd->SetWindowText(strInf);
	pWnd = GetDlgItem(IDC_TSTEM_OS_TYASD_EDT);
	strInf.Format(_T("%d"), ::XSysproxy().m_tagOpeionPrm.sTypo_Ascender);
	pWnd->SetWindowText(strInf);
	pWnd = GetDlgItem(IDC_TSTEM_OS_TYDSD_EDT);
	strInf.Format(_T("%d"), ::XSysproxy().m_tagOpeionPrm.sTypo_Descender);
	pWnd->SetWindowText(strInf);		
	pWnd = GetDlgItem(IDC_TSTEM_OS_CPR1_EDT);
	strInf.Format(_T("0x%x"), ::XSysproxy().m_tagOpeionPrm.ulCodePageRang1);
	pWnd->SetWindowText(strInf);
	pWnd = GetDlgItem(IDC_TSTEM_OS_CPR2_EDT);
	strInf.Format(_T("0x%x"), ::XSysproxy().m_tagOpeionPrm.ulCodePageRang2);
	pWnd->SetWindowText(strInf);	

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	
}	// end of BOOL CTTFSetupEMProPage::OnInitDialog()

void CTTFSetupEMProPage::OnDestroy()
{	
	CPropertyPage::OnDestroy();
	
}	// end of void CTTFSetupEMProPage::OnDestroy()

void CTTFSetupEMProPage::SaveInfo()
{
	if (m_hWnd) {
		UpdateData();

		CString strInf;
		CWnd* pWnd = GetDlgItem(IDC_TSTEM_HD_EM_EDT);
		pWnd->GetWindowText(strInf);
		::XSysproxy().m_tagOpeionPrm.usUnitsPerEM = atoi(strInf);
		pWnd = GetDlgItem(IDC_TSTEM_OS_WEIGHT_EDT);
		pWnd->GetWindowText(strInf);
		::XSysproxy().m_tagOpeionPrm.usWeight = atoi(strInf);
		pWnd = GetDlgItem(IDC_TSTEM_OS_WIDTH_EDT);
		pWnd->GetWindowText(strInf);
		::XSysproxy().m_tagOpeionPrm.usWidth = atoi(strInf);
		pWnd = GetDlgItem(IDC_TSTEM_OS_TYASD_EDT);
		pWnd->GetWindowText(strInf);
		::XSysproxy().m_tagOpeionPrm.sTypo_Ascender = atoi(strInf);
		pWnd = GetDlgItem(IDC_TSTEM_OS_TYDSD_EDT);
		pWnd->GetWindowText(strInf);
		::XSysproxy().m_tagOpeionPrm.sTypo_Descender = atoi(strInf);
		pWnd = GetDlgItem(IDC_TSTEM_OS_CPR1_EDT);
		pWnd->GetWindowText(strInf);
		::XSysproxy().m_tagOpeionPrm.ulCodePageRang1 = strtoul((LPTSTR)(LPCTSTR)strInf, NULL, 16);
		pWnd = GetDlgItem(IDC_TSTEM_OS_CPR2_EDT);
		pWnd->GetWindowText(strInf);
		::XSysproxy().m_tagOpeionPrm.ulCodePageRang2 = strtoul((LPTSTR)(LPCTSTR)strInf, NULL, 16);
	}

}	// end of void CTTFSetupEMProPage::SaveInfo()
