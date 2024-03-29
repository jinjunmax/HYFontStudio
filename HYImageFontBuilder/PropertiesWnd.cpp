
#include "stdafx.h"

#include "PropertiesWnd.h"
#include "Resource.h"
#include "MainFrm.h"
#include "HYImageFontBuilder.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar

CPropertiesWnd::CPropertiesWnd()
{
	m_nComboHeight = 0;

}	// end of CPropertiesWnd::CPropertiesWnd()

CPropertiesWnd::~CPropertiesWnd()
{

}	// end of CPropertiesWnd::~CPropertiesWnd()

BEGIN_MESSAGE_MAP(CPropertiesWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()	
	ON_WM_SETFOCUS()
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar 消息处理程序

void CPropertiesWnd::AdjustLayout()
{
	if (GetSafeHwnd () == NULL || (AfxGetMainWnd() != NULL && AfxGetMainWnd()->IsIconic()))
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);
	m_wndPropList.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);

}	// end of void CPropertiesWnd::AdjustLayout()

int CPropertiesWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 创建组合: 
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_BORDER | CBS_SORT | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
	{
		TRACE0("未能创建属性网格\n");
		return -1;      // 未能创建
	}

	InitPropList();
	
#if  0
	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PROPERTIES);
	m_wndToolBar.LoadToolBar(IDR_PROPERTIES, 0, 0, TRUE /* 已锁定*/);
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_PROPERTIES_HC : IDR_PROPERTIES, 0, 0, TRUE /* 锁定*/);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	m_wndToolBar.SetOwner(this);

	// 所有命令将通过此控件路由，而不是通过主框架路由: 
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);
#endif 

	AdjustLayout();
	return 0;

}	// end of int CPropertiesWnd::OnCreate()

void CPropertiesWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();

}	// end of void CPropertiesWnd::OnSize()

void CPropertiesWnd::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndPropList.SetFocus();

}	// end of void CPropertiesWnd::OnSetFocus()

LRESULT CPropertiesWnd::OnPropertyChanged(WPARAM wparam, LPARAM lparam)
{
	CMFCPropertyGridProperty* pProp = (CMFCPropertyGridProperty*)lparam;
	LPCSTR lpPrpName = pProp->GetName();

	LPCTSTR tValue = (LPCTSTR)(_bstr_t)pProp->GetValue();
	int iCmp = strcmp(lpPrpName, _T("点"));
	if (iCmp==0)
	{
		if (strcmp(tValue,"true")==0)
			::XSysproxy().m_tagDrawParam.iPoint = 1;
		else
			::XSysproxy().m_tagDrawParam.iPoint = 0;
		return 0L;
	}
	
	iCmp = strcmp(lpPrpName, _T("点编号"));
	if (iCmp == 0)
	{
		if (strcmp(tValue, "true") == 0)
			::XSysproxy().m_tagDrawParam.iPointNo = 1;
		else
			::XSysproxy().m_tagDrawParam.iPointNo = 0;
		return 0L;
	}

	iCmp = strcmp(lpPrpName, _T("轮廓"));
	if (iCmp == 0)
	{
		if (strcmp(tValue, "true") == 0)
			::XSysproxy().m_tagDrawParam.iLine = 1;
		else
			::XSysproxy().m_tagDrawParam.iLine = 0;
		return 0L;
	}

	iCmp = strcmp(lpPrpName, _T("参考线"));
	if (iCmp == 0)
	{
		if (strcmp(tValue, "true") == 0)
			::XSysproxy().m_tagDrawParam.iReferLine = 1;
		else
			::XSysproxy().m_tagDrawParam.iReferLine = 0;
		return 0L;
	}

	iCmp = strcmp(lpPrpName, _T("填充"));
	if (iCmp == 0)
	{
		if (strcmp(tValue, "true") == 0)
			::XSysproxy().m_tagDrawParam.iFill = 1;
		else
			::XSysproxy().m_tagDrawParam.iFill = 0;
		return 0L;
	}

	iCmp = strcmp(lpPrpName, _T("显示区域"));
	if (iCmp == 0)
	{	
		if (strcmp(tValue, "Box") == 0)
			::XSysproxy().m_tagDrawParam.iViewType = VW_PARAM_HEAD;
		if (strcmp(tValue, "Hhea") == 0)		
			::XSysproxy().m_tagDrawParam.iViewType = VW_PARAM_HHEA;
		if (strcmp(tValue, "Typo") == 0)	
			::XSysproxy().m_tagDrawParam.iViewType = VW_PARAM_OS_TYPO;
		if (strcmp(tValue, "Win") == 0)		
			::XSysproxy().m_tagDrawParam.iViewType = VW_PARAM_OS_WIN;
	
		return 0L;
	}

	return 0L;

}	// end of LRESULT CPropertiesWnd::OnPropertyChanged()

void CPropertiesWnd::InitPropList()
{
	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();

	CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("显示"));

	CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("点"), _T("true"), _T("预览字形时显示点"));
	pProp->AddOption("false");
	pProp->AddOption("true");
	pProp->AllowEdit(FALSE);	
	if (::XSysproxy().m_tagDrawParam.iPoint)
		pProp->SetValue(_T("true"));
	else 
		pProp->SetValue(_T("false"));
	pGroup1->AddSubItem(pProp);
	
	pProp = new CMFCPropertyGridProperty(_T("点编号"), _T("true"), _T("预览字形时显示点编号"));
	pProp->AddOption("false");
	pProp->AddOption("true");
	pProp->AllowEdit(FALSE);
	if (::XSysproxy().m_tagDrawParam.iPointNo)
		pProp->SetValue(_T("true"));
	else
		pProp->SetValue(_T("false"));
	pGroup1->AddSubItem(pProp);	

	pProp = new CMFCPropertyGridProperty(_T("轮廓"), _T("true"), _T("预览字形时显示轮廓线"));
	pProp->AddOption("false");
	pProp->AddOption("true");
	pProp->AllowEdit(FALSE);
	if (::XSysproxy().m_tagDrawParam.iLine)
		pProp->SetValue(_T("true"));
	else
		pProp->SetValue(_T("false"));
	pGroup1->AddSubItem(pProp);

	pProp = new CMFCPropertyGridProperty(_T("参考线"), _T("true"), _T("预览字形时显示参考线"));
	pProp->AddOption("false");
	pProp->AddOption("true");
	pProp->AllowEdit(FALSE);
	if (::XSysproxy().m_tagDrawParam.iReferLine)
		pProp->SetValue(_T("true"));
	else
		pProp->SetValue(_T("false"));
	pGroup1->AddSubItem(pProp);

	pProp = new CMFCPropertyGridProperty(_T("填充"), _T("true"), _T("预览字形时填充轮廓"));
	pProp->AddOption("false");
	pProp->AddOption("true");
	pProp->AllowEdit(FALSE);
	if (::XSysproxy().m_tagDrawParam.iFill)
		pProp->SetValue(_T("true"));
	else
		pProp->SetValue(_T("false"));
	pGroup1->AddSubItem(pProp);

	pProp = new CMFCPropertyGridProperty(_T("显示区域"), _T("Box"), _T("其中之一: “Box”、“Hhea”、“Typo”或“Win”"));
	pProp->AddOption(_T("Box"));
	pProp->AddOption(_T("Hhea"));
	pProp->AddOption(_T("Typo"));
	pProp->AddOption(_T("Win"));
	pProp->AllowEdit(FALSE);

	if (::XSysproxy().m_tagDrawParam.iViewType == VW_PARAM_HEAD)
		pProp->SetValue(_T("Box"));
	if (::XSysproxy().m_tagDrawParam.iViewType == VW_PARAM_HHEA)
		pProp->SetValue(_T("Hhea"));
	if (::XSysproxy().m_tagDrawParam.iViewType == VW_PARAM_OS_TYPO)
		pProp->SetValue(_T("Typo"));
	if (::XSysproxy().m_tagDrawParam.iViewType == VW_PARAM_OS_WIN)
		pProp->SetValue(_T("Win"));

	pGroup1->AddSubItem(pProp);
	m_wndPropList.AddProperty(pGroup1);

}	// end of void CPropertiesWnd::InitPropList()


