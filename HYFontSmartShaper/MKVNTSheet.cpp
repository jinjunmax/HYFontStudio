// CMKVNTSheet.cpp: 实现文件
//

#include "stdafx.h"
#include "HYFontSmartShaper.h"
#include "MKVNTSheet.h"


// CMKVNTSheet

IMPLEMENT_DYNAMIC(CMKVNTSheet, CPropertySheet)

CMKVNTSheet::CMKVNTSheet()
{
	Init();
	//m_strCaption = _T("可变字库生成");

}	// end of CMKVNTSheet::CMKVNTSheet()

CMKVNTSheet::CMKVNTSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	Init();

}	// end of CMKVNTSheet::CMKVNTSheet()

CMKVNTSheet::CMKVNTSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	Init();

}	// end of CMKVNTSheet::CMKVNTSheet()

CMKVNTSheet::~CMKVNTSheet()
{
	Uninit();

}	// end of CMKVNTSheet::~CMKVNTSheet()

void CMKVNTSheet::Init()
{
	AddPage(&FntPage);
	AddPage(&AxisPage);
	AddPage(&MasterPage);
	AddPage(&InstancePage);	

	m_psh.dwFlags |= PSH_NOAPPLYNOW | PSH_PROPTITLE;
	m_psh.pszCaption = "Simple";
	m_psh.nStartPage = 0;

}	// end of void CMKVNTSheet::Init()

void CMKVNTSheet::Uninit()
{


}	// end of void CMKVNTSheet::Uninit()

BEGIN_MESSAGE_MAP(CMKVNTSheet, CPropertySheet)
END_MESSAGE_MAP()

// CMKVNTSheet 消息处理程序
BOOL CMKVNTSheet::OnInitDialog()
{
	BOOL bResult = CPropertySheet::OnInitDialog();

	CTabCtrl* p = GetTabControl();
	for (int i = 0; i < 4; i++)	{
		TCITEMA	tcItem;
		ZeroMemory(&tcItem, sizeof(TCITEMA));
		tcItem.mask = TCIF_TEXT;
		tcItem.cchTextMax = 10;

		switch (i){
			case 0:{
				tcItem.pszText = _T("Font");
			}
			break;
			case 1:	{
				tcItem.pszText = _T("Axes");
			}
			break;
			case 2:	{
				tcItem.pszText = _T("Masters");
			}
			break;
			case 3:	{
				tcItem.pszText = _T("Instances");
			}
			break;
		}
		p->SetItem(i, &tcItem);
	}

	return bResult;

}	// end of BOOL CMKVNTSheet::OnInitDialog()
