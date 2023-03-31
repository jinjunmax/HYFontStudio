// TTFSetupDlg.cpp : implementation file
//

#include "stdafx.h"
#include "HYFontSmartShaper.h"
#include "FontSetupDlg.h"


// CFontSetupDlg dialog
IMPLEMENT_DYNAMIC(CFontSetupDlg, CDialog)

CFontSetupDlg::CFontSetupDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFontSetupDlg::IDD, pParent)
{	
	m_pFontCodec  = NULL;

}	// end of CFontSetupDlg::CFontSetupDlg()

CFontSetupDlg::CFontSetupDlg(HYFONTCODEC::CHYFontCodec*	pFontCodec, CWnd* pParent)
	:CDialog(CFontSetupDlg::IDD, pParent)
{
	m_pFontCodec = pFontCodec;

}	// end of CFontSetupDlg::CFontSetupDlg()

CFontSetupDlg::~CFontSetupDlg()
{

}	// end of CFontSetupDlg::~CFontSetupDlg()

void CFontSetupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

}	// end of void CFontSetupDlg::DoDataExchange()

BEGIN_MESSAGE_MAP(CFontSetupDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CFontSetupDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CFontSetupDlg::OnBnClickedCancel)
END_MESSAGE_MAP()

// CFontSetupDlg message handlers
BOOL CFontSetupDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	m_sheet.Create(this, WS_CHILD | WS_VISIBLE, 0);
	//使Tab键消息有效，可以通过Tab键转移输入焦点
	m_sheet.ModifyStyleEx (0, WS_EX_CONTROLPARENT);
	m_sheet.ModifyStyle(0, WS_TABSTOP);
	CRect rcSheet;
	this->GetClientRect(rcSheet);
	m_sheet.SetWindowPos(NULL,rcSheet.left, rcSheet.top, 0, 0, SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);

	return TRUE;
	
}	// end of BOOL CFontSetupDlg::OnInitDialog()

void CFontSetupDlg::OnBnClickedOk()
{
	if (!m_sheet.SaveInfo())
		return;;

	OnOK();

}	// end of void CFontSetupDlg::OnBnClickedOk()

void CFontSetupDlg::OnBnClickedCancel()
{
	OnCancel();

}	// end of void CFontSetupDlg::OnBnClickedCancel()
