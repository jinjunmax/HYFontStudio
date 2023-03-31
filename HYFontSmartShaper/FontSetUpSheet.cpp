// TTFSetUpDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FontSetUpSheet.h"


// CFontSetUpSheet

IMPLEMENT_DYNAMIC(CFontSetUpSheet, CPropertySheet)

CFontSetUpSheet::CFontSetUpSheet()
{	
	Init();	

}	// end of CFontSetUpSheet::CFontSetUpSheet()

CFontSetUpSheet::CFontSetUpSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	Init();

}	// end of CFontSetUpSheet::CFontSetUpSheet()

CFontSetUpSheet::CFontSetUpSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	Init();

}	// end of CFontSetUpSheet::CFontSetUpSheet()

CFontSetUpSheet::~CFontSetUpSheet()
{
	Uninit();

}	// end of CFontSetUpSheet::~CFontSetUpSheet()

BEGIN_MESSAGE_MAP(CFontSetUpSheet, CPropertySheet)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////
//
void CFontSetUpSheet::Init()
{
	AddPage(&m_EMProPage);	
	AddPage(&m_CFFProPage);
	AddPage(&m_OptionPage);

	m_psh.dwFlags |= PSH_NOAPPLYNOW | PSH_PROPTITLE;
	m_psh.pszCaption = "Simple";
	m_psh.nStartPage = 0;
	
}	// end of void	CFontSetUpSheet::Init()

void CFontSetUpSheet::Uninit()
{

}	// end of void CFontSetUpSheet::Uninit()

// CFontSetUpSheet message handlers
BOOL CFontSetUpSheet::OnInitDialog()
{
	BOOL bResult = CPropertySheet::OnInitDialog();

	CTabCtrl* p =GetTabControl();
	for (int i = 0; i < 3; i++ )
	{
		TCITEMA	tcItem; 
		ZeroMemory(&tcItem,sizeof(TCITEMA));
		tcItem.mask = TCIF_TEXT;
		tcItem.cchTextMax = 10;

		switch(i)
		{
			case 0:	{
					tcItem.pszText = _T("Metrics");
				}
				break;
			case 1:	{
				tcItem.pszText = _T("CFF");
	
				}
				break;
			case 2:	{
				tcItem.pszText = _T("其它选项");
				}
				break;
			default:
				break;
		}
		p->SetItem(i,&tcItem);
	}

	return bResult;

}	// end of BOOL CFontSetUpSheet::OnInitDialog()

BOOL CFontSetUpSheet::SaveInfo()
{
	m_EMProPage.SaveInfo();	
	m_CFFProPage.Save();
	return m_OptionPage.Save();

}	// end of void CFontSetUpSheet::SaveInfo()


