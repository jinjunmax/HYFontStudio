// MKVNTAxisPage.cpp : 实现文件
//

#include "stdafx.h"
#include "HYFontSmartShaper.h"
#include "MKVNTAxisPage.h"
#include "afxdialogex.h"


// CMKVNTAxisPage 对话框
const CString AxseWeight="Weight";
const CString AxseWidth = "Width";

IMPLEMENT_DYNAMIC(CMKVNTAxisPage, CPropertyPage)

CMKVNTAxisPage::CMKVNTAxisPage()
	: CPropertyPage(IDD_FNT_MKVNT_AXS_PAGE)
	, m_bHidden(FALSE)
{

}	// end of CMKVNTAxisPage::CMKVNTAxisPage()

CMKVNTAxisPage::~CMKVNTAxisPage()
{

}	// end of CMKVNTAxisPage::~CMKVNTAxisPage()

void CMKVNTAxisPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_MKV_AXS_TAG_EDT, m_cmbAxesTag);
	DDX_Check(pDX, IDC_MKV_AXS_HID_CHK, m_bHidden);
	DDX_Control(pDX, IDC_MKV_AXS_AXS_LST, m_lstAxes);
}	// end of void CMKVNTAxisPage::DoDataExchange(CDataExchange* pDX)

BEGIN_MESSAGE_MAP(CMKVNTAxisPage, CPropertyPage)
	ON_BN_CLICKED(IDC_MKV_AXS_ADD_BTN, &CMKVNTAxisPage::OnBnClickedMkvAxsAddBtn)
	ON_BN_CLICKED(IDC_MKV_AXS_DEL_BTN, &CMKVNTAxisPage::OnBnClickedMkvAxsDelBtn)
	ON_NOTIFY(NM_SETFOCUS, IDC_MKV_AXS_AXS_LST, &CMKVNTAxisPage::OnNMSetfocusMkvAxsAxsLst)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_MKV_AXS_AXS_LST, &CMKVNTAxisPage::OnLvnItemchangedMkvAxsAxsLst)
END_MESSAGE_MAP()

// CMKVNTAxisPage 消息处理程序
void CMKVNTAxisPage::OnBnClickedMkvAxsAddBtn()
{
	UpdateData(TRUE);

	int iIndex = m_cmbAxesTag.GetCurSel();
	if (iIndex < 0) return;	
	
	CString strTag = m_RgstryAxis.vtRegistryAxise[iIndex].m_strTag;
	int iAxisIndex = ::XSysproxy().AxisRecords.FindAxisIndexEx(strTag);
	if (iAxisIndex > -1){
		AfxMessageBox("已经存在同名轴值");
		return;
	}
	
	CVrAxisRecord AXisReord;
	AXisReord.m_strName = m_RgstryAxis.vtRegistryAxise[iIndex].m_strName;
	AXisReord.m_strTag = m_RgstryAxis.vtRegistryAxise[iIndex].m_strTag;
	AXisReord.m_bHidden = m_bHidden;
	::XSysproxy().AxisRecords.vtVrAxisInfs.push_back(AXisReord);

	UpdateListCtrl();	
	
	int iSelIndex = ::XSysproxy().AxisRecords.vtVrAxisInfs.size() - 1;	
	m_lstAxes.SetFocus();
	int iTest = m_lstAxes.SetItemState(iSelIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	m_lstAxes.SetSelectionMark(iSelIndex);

	UpdateData(FALSE);

}	// end of void CMKVNTAxisPage::OnBnClickedMkvAxsAddBtn()

void CMKVNTAxisPage::OnBnClickedMkvAxsDelBtn()
{
	int iIndex = GetListCtrlSelectItem();
	if (iIndex < 0) return;
	
	CVrAxisRecords& vtAxisRecord = ::XSysproxy().AxisRecords;	
	vtAxisRecord.vtVrAxisInfs.erase(vtAxisRecord.vtVrAxisInfs.begin() + iIndex);

	UpdateListCtrl();
	int iCount = m_lstAxes.GetItemCount();

	if (iIndex>0&& iIndex <iCount){
		m_lstAxes.SetFocus();
		m_lstAxes.SetItemState(iIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		m_lstAxes.SetSelectionMark(iIndex);
	}

	UpdateData(FALSE);

}	// end of void CMKVNTAxisPage::OnBnClickedMkvAxsDelBtn()

BOOL CMKVNTAxisPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	InitControl();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}	//end of BOOL CMKVNTAxisPage::OnInitDialog()

//////////////////////////////////////////////////////////////////////////
void CMKVNTAxisPage::InitControl()
{
	m_cmbAxesTag.ResetContent();
	for (size_t i = 0; i < m_RgstryAxis.vtRegistryAxise.size(); i++){
		m_cmbAxesTag.AddString(m_RgstryAxis.vtRegistryAxise[i].m_strName);
	}	
	m_cmbAxesTag.SetCurSel(0);

	LVCOLUMN		tagColumn;
	ZeroMemory(&tagColumn, sizeof(LVCOLUMN));

	tagColumn.mask = LVCF_WIDTH | LVCF_TEXT;
	tagColumn.cx = 120;
	tagColumn.pszText = _T("Name");
	m_lstAxes.InsertColumn(0, &tagColumn);
	tagColumn.cx = 120;
	tagColumn.pszText = _T("tag");
	m_lstAxes.InsertColumn(1, &tagColumn);
	tagColumn.cx = 120;
	tagColumn.pszText = _T("Hidden");
	m_lstAxes.InsertColumn(2, &tagColumn);
	tagColumn.cx = 120;
	tagColumn.pszText = _T("Font");
	m_lstAxes.InsertColumn(3, &tagColumn);
	m_lstAxes.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);	

}	// end of void CMKVNTAxisPage::InitControl()

void CMKVNTAxisPage::UninitControl()
{


}	// end of void CMKVNTAxisPage::UninitControl()

void CMKVNTAxisPage::UpdateListCtrl()
{	
	m_lstAxes.DeleteAllItems();

	size_t sz = ::XSysproxy().AxisRecords.vtVrAxisInfs.size();
	for (size_t i = 0; i<sz; i++){
		CVrAxisRecord& AxisRecord = ::XSysproxy().AxisRecords.vtVrAxisInfs[i];

		LVITEM	tagLvItem;
		ZeroMemory(&tagLvItem, sizeof(LVITEM));

		tagLvItem.mask = LVIF_TEXT;
		tagLvItem.iItem = i;
		tagLvItem.iSubItem = 0;
		tagLvItem.pszText = new char[MAX_PATH];
		tagLvItem.cchTextMax = MAX_PATH;
		ZeroMemory(tagLvItem.pszText, MAX_PATH);		
		// name
		sprintf_s(tagLvItem.pszText, MAX_PATH, "%s", (LPSTR)(LPCTSTR)AxisRecord.m_strName);
		m_lstAxes.InsertItem(&tagLvItem);
		// Axes tag
		tagLvItem.iSubItem = 1;
		sprintf_s(tagLvItem.pszText, MAX_PATH, "%s", (LPSTR)(LPCTSTR)AxisRecord.m_strTag);
		m_lstAxes.SetItem(&tagLvItem);
		// Hidden
		tagLvItem.iSubItem = 2;
		if (AxisRecord.m_bHidden)
			sprintf_s(tagLvItem.pszText, MAX_PATH, "%s", "Yes");
		else 
			sprintf_s(tagLvItem.pszText, MAX_PATH, "%s", "No");
		m_lstAxes.SetItem(&tagLvItem);

		delete[] tagLvItem.pszText;
		tagLvItem.pszText = NULL;
	}

	UpdateData(FALSE);

}	// end of void CMKVNTAxisPage::UpdateListCtrl()

int	CMKVNTAxisPage::GetListCtrlSelectItem()
{
	int nItem = -1;
	POSITION pos = m_lstAxes.GetFirstSelectedItemPosition();
	if (pos){
		nItem = m_lstAxes.GetNextSelectedItem(pos);
	}
	return nItem;

}	// end of int	CMKVNTAxisPage::GetListCtrlSelectItem()

void CMKVNTAxisPage::OnNMSetfocusMkvAxsAxsLst(NMHDR *pNMHDR, LRESULT *pResult)
{	
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	

}	// end of void CMKVNTAxisPage::OnNMSetfocusMkvAxsAxsLst()

void CMKVNTAxisPage::OnLvnItemchangedMkvAxsAxsLst(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	int iSelIndex = GetListCtrlSelectItem();
	if (iSelIndex == -1) {
		*pResult = 0;
		return;
	}

	CVrAxisRecord& AxRcrd = ::XSysproxy().AxisRecords.vtVrAxisInfs[iSelIndex];
	m_bHidden = AxRcrd.m_bHidden;
	int iIndex = m_RgstryAxis.FindAxisEx(AxRcrd.m_strTag);
	m_cmbAxesTag.SetCurSel(iIndex);

	UpdateData(FALSE);

	*pResult = 0;

}	// end of void CMKVNTAxisPage::OnLvnItemchangedMkvAxsAxsLst()
