// MXGCPYDlg.cpp : implementation file
//

#include "stdafx.h"
#include "HYFontSmartShaper.h"
#include "MXGCPYDlg.h"

// CMXGCPYDlg dialog
IMPLEMENT_DYNAMIC(CMXGCPYDlg, CDialog)

CMXGCPYDlg::CMXGCPYDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMXGCPYDlg::IDD, pParent)
	, m_iBGID(0)
	, m_iEGID(0)
{


}	// end of CMXGCPYDlg::CMXGCPYDlg()

CMXGCPYDlg::CMXGCPYDlg(HYFONTCODEC::CHYFontCodec* fontCodec,CWnd* pParent /*= NULL*/)
	: CDialog(CMXGCPYDlg::IDD, pParent)
	, m_iBGID(0)
	, m_iEGID(0)
{
	m_pCodec		= fontCodec;
	m_pBThumFontWnd	= NULL;
	m_pEThumFontWnd	= NULL;

}	// end of CMXGCPYDlg::CMXGCPYDlg()

CMXGCPYDlg::~CMXGCPYDlg()
{

}	// end of CMXGCPYDlg::~CMXGCPYDlg()

void CMXGCPYDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_MX_BGID_EDT, m_iBGID);
	DDX_Text(pDX, IDC_MX_EGID_EDT, m_iEGID);
}	// end of void CMXGCPYDlg::DoDataExchange()

BEGIN_MESSAGE_MAP(CMXGCPYDlg, CDialog)
	ON_WM_DESTROY()	
	ON_EN_CHANGE(IDC_MX_BGID_EDT, &CMXGCPYDlg::OnEnChangeMxBgidEdt)
	ON_EN_CHANGE(IDC_MX_EGID_EDT, &CMXGCPYDlg::OnEnChangeMxEgidEdt)
	ON_BN_CLICKED(IDOK, &CMXGCPYDlg::OnBnClickedOk)
END_MESSAGE_MAP()

// CMXGCPYDlg message handlers
BOOL CMXGCPYDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	InitThumWnd();

	return TRUE;

}	// end of BOOL CMXGCPYDlg::OnInitDialog()

void CMXGCPYDlg::OnDestroy()
{
	CDialog::OnDestroy();

	UnInitThumWnd();
	
}	// end of void CMXGCPYDlg::OnDestroy()

void CMXGCPYDlg::InitThumWnd()
{
	CRect rcThum;
	CWnd* pWnd = GetDlgItem(IDC_MX_BGID_STC);
	if (pWnd)
	{
		if (pWnd->GetSafeHwnd())
		{
			pWnd->GetWindowRect(rcThum);
			ScreenToClient(rcThum);

			m_pBThumFontWnd = new CThumFontWnd();
			if (m_pBThumFontWnd->CreateEx(0,NULL,NULL,WS_CHILD|WS_TABSTOP|WS_VISIBLE, rcThum,this,90001))
			{
				m_pBThumFontWnd->ShowWindow(SW_SHOW);				
			}
		}
	}

	pWnd = GetDlgItem(IDC_MX_EGID_STC);
	if (pWnd)
	{
		if (pWnd->GetSafeHwnd())
		{
			pWnd->GetWindowRect(rcThum);
			ScreenToClient(rcThum);

			m_pEThumFontWnd = new CThumFontWnd();
			if (m_pEThumFontWnd->CreateEx(0,NULL,NULL,WS_CHILD|WS_TABSTOP|WS_VISIBLE, rcThum,this,90002))
			{
				m_pEThumFontWnd->ShowWindow(SW_SHOW);				
			}
		}
	}

}	// end of void CMXGCPYDlg::InitThumWnd()

void CMXGCPYDlg::UnInitThumWnd()
{
	if (m_pEThumFontWnd)
	{
		if (m_pEThumFontWnd->GetSafeHwnd())
		{
			m_pEThumFontWnd->DestroyWindow();
		}
		delete m_pEThumFontWnd;
		m_pEThumFontWnd = NULL;
	}

	if (m_pBThumFontWnd)
	{
		if (m_pBThumFontWnd->GetSafeHwnd())
		{
			m_pBThumFontWnd->DestroyWindow();
		}
		delete m_pBThumFontWnd;
		m_pBThumFontWnd = NULL;
	}

}	// end of void CMXGCPYDlg::UnInitThumWnd()

void CMXGCPYDlg::OnEnChangeMxBgidEdt()
{	
	UpdateData();


	m_pBThumFontWnd->SetGlyphInfo(*m_pCodec,m_iBGID);

}	// end of void CMXGCPYDlg::OnEnChangeMxBgidEdt()

void CMXGCPYDlg::OnEnChangeMxEgidEdt()
{	
	UpdateData();

	m_pEThumFontWnd->SetGlyphInfo(*m_pCodec,m_iEGID);


}	// end of void CMXGCPYDlg::OnEnChangeMxEgidEdt()

void CMXGCPYDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}
