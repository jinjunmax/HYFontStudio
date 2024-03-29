// CMKVNTMasterPage.cpp: 实现文件
//

#include "stdafx.h"
#include "HYFontSmartShaper.h"
#include "MKVNTMasterPage.h"
#include "afxdialogex.h"


// CMKVNTMasterPage 对话框

IMPLEMENT_DYNAMIC(CMKVNTMasterPage, CPropertyPage)

CMKVNTMasterPage::CMKVNTMasterPage()
	: CPropertyPage(IDD_FNT_MKVNT_MST_PAGE)
{	
	m_strMName	= "Test";
	m_iWeight	= 400;
	m_iWidth	= 5;
	m_iHeight	= 100;
	m_iAscender = 850;
	m_iDescender = -150;
	m_iTypoAscender = 850;
	m_iTypoDescender = 150;

}	// end of CMKVNTMasterPage::CMKVNTMasterPage()

CMKVNTMasterPage::~CMKVNTMasterPage()
{

}	// end of CMKVNTMasterPage::~CMKVNTMasterPage()

void CMKVNTMasterPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MKV_MST_MST_LST, m_lstMaster);
	DDX_Text(pDX, IDC_MKV_MST_MSTNM_EDT, m_strMName);
	DDX_Text(pDX, IDC_MKV_MST_WGHT_EDT, m_iWeight);
	DDX_Text(pDX, IDC_MKV_MST_WDTH_EDT, m_iWidth);
	DDX_Text(pDX, IDC_MKV_MST_HGHT_EDT, m_iHeight);
	DDX_Text(pDX, IDC_MKV_MST_ACNDR_EDT, m_iAscender);
	DDX_Text(pDX, IDC_MKV_MST_DESCNDR_EDT, m_iDescender);
	DDX_Text(pDX, IDC_MKV_MST_TYACNDR_EDT, m_iTypoAscender);
	DDX_Text(pDX, IDC_MKV_MST_TYDESCNDR_EDT, m_iTypoDescender);
	DDX_Check(pDX, IDC_MKV_MST_DEFAULT_CHK, m_bDefault);
	DDX_Text(pDX, IDC_MKV_MST_FILENAME_BTN, m_strFontName);

}	// end of void CMKVNTMasterPage::DoDataExchange()

BEGIN_MESSAGE_MAP(CMKVNTMasterPage, CPropertyPage)
	ON_BN_CLICKED(IDC_MKV_MST_OPNFILE_BTN, &CMKVNTMasterPage::OnBnClickedMkvMstOpnfileBtn)
	ON_BN_CLICKED(IDC_MKV_MST_ADD_BTN, &CMKVNTMasterPage::OnBnClickedMkvMstAddBtn)
	ON_BN_CLICKED(IDC_MKV_MST_DEL_BTN, &CMKVNTMasterPage::OnBnClickedMkvMstDelBtn)
	ON_BN_CLICKED(IDC_MKV_MST_UP_BTN, &CMKVNTMasterPage::OnBnClickedMkvMstUpBtn)
	ON_BN_CLICKED(IDC_MKV_MST_DWN_BTN, &CMKVNTMasterPage::OnBnClickedMkvMstDwnBtn)
	ON_BN_CLICKED(IDC_MKV_MST_MD_BTN, &CMKVNTMasterPage::OnBnClickedMkvMstMdBtn)
	ON_WM_SHOWWINDOW()	
	ON_NOTIFY(NM_SETFOCUS, IDC_MKV_MST_MST_LST, &CMKVNTMasterPage::OnNMSetfocusMkvMstMstLst)
	ON_NOTIFY(NM_CLICK, IDC_MKV_MST_MST_LST, &CMKVNTMasterPage::OnNMClickMkvMstMstLst)
END_MESSAGE_MAP()


// CMKVNTMasterPage 消息处理程序
BOOL CMKVNTMasterPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	
	InitControl();

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE

}	// end of BOOL CMKVNTMasterPage::OnInitDialog()

void CMKVNTMasterPage::OnBnClickedMkvMstOpnfileBtn()
{
	UpdateData();

	TCHAR	szFilters[] = _T("TrueType 文件(*.ttf)|*.ttf|OpenType 文件(*.otf)|*.otf||");
	CFileDialog  openFileDlg(TRUE, _T(""), _T(""), OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilters);
	if (openFileDlg.DoModal() != IDOK)	return;

	m_strFileName = openFileDlg.GetPathName();
	m_Master.m_strFileName = m_strFileName;
	m_strFontName = HY_GetFullNameFromPath(string(m_Master.m_strFileName)).c_str();
	m_Master.m_strFontName = m_strFontName;

	UpdateData(FALSE);

}	// end of void CMKVNTMasterPage::OnBnClickedMkvMstOpnfileBtn()

void CMKVNTMasterPage::OnBnClickedMkvMstAddBtn()
{	
	UpdateData();
	
	if (m_Master.m_strFontName == "") {
		AfxMessageBox("请先选择对应的字库文件！");
		return;
	}

	m_Master.SetDefault();
	m_Master.m_strMasterName = m_strMName;
	m_Master.m_strFileName = m_strFileName;
	m_Master.m_strFontName = m_strFontName;
	m_Master.m_bDefault = m_bDefault;
	m_Master.m_iAcsender = m_iAscender;
	m_Master.m_iDescender = m_iDescender;
	m_Master.m_iTypoAscender = m_iTypoAscender;
	m_Master.m_iTypoDescender = m_iTypoDescender;
	
	CVrAxisRecords& AxsRcrd = ::XSysproxy().AxisRecords;
	int iCmlIndex = 1;
	for (size_t i = 0; i < AxsRcrd.vtVrAxisInfs.size(); i++)
	{
		CVrAxisRecord& asRcd = AxsRcrd.vtVrAxisInfs[i];

		CVrAxisRecord	AxsRecrdValue;
		AxsRecrdValue.m_strName = asRcd.m_strName;
		AxsRecrdValue.m_iNameID = asRcd.m_iNameID;
		AxsRecrdValue.m_strTag = asRcd.m_strTag;
		if (asRcd.m_strTag == AXES_WIDTH) {
			AxsRecrdValue.m_AxisValue.Format = 1;
			AxsRecrdValue.m_AxisValue.Value = m_iWidth;
			AxsRecrdValue.m_AxisValue.axisIndex = i;
			m_Master.m_AxisRecords.vtVrAxisInfs.push_back(AxsRecrdValue);
		}

		if (asRcd.m_strTag == AXES_WEIGHT) {
			AxsRecrdValue.m_AxisValue.Format = 1;
			AxsRecrdValue.m_AxisValue.Value = m_iWeight;
			AxsRecrdValue.m_AxisValue.axisIndex = i;
			m_Master.m_AxisRecords.vtVrAxisInfs.push_back(AxsRecrdValue);
		}		
	}

	::XSysproxy().Masters.m_vtMaster.push_back(m_Master);

	UpdateListCtrl();

}	// end of void CMKVNTMasterPage::OnBnClickedMkvMstAddBtn()

void CMKVNTMasterPage::OnBnClickedMkvMstDelBtn()
{
	int iItmIndex = GetListCtrlSelectItem();
	if (iItmIndex == -1)  return;

	if (iItmIndex == 0){
		if (::XSysproxy().Masters.m_vtMaster.size() == 1)		{
			m_Master.SetDefault();
			::XSysproxy().Masters.m_vtMaster.clear();
		}
		else
		{
			::XSysproxy().Masters.m_vtMaster.erase(::XSysproxy().Masters.m_vtMaster.begin() + iItmIndex);
			m_Master = ::XSysproxy().Masters.m_vtMaster[iItmIndex];
		}
	}
	else if (iItmIndex == ::XSysproxy().Masters.m_vtMaster.size()-1){		
		m_Master = ::XSysproxy().Masters.m_vtMaster[::XSysproxy().Masters.m_vtMaster.size() - 1];
		::XSysproxy().Masters.m_vtMaster.erase(::XSysproxy().Masters.m_vtMaster.begin() + iItmIndex);
		iItmIndex--;
		m_Master = ::XSysproxy().Masters.m_vtMaster[::XSysproxy().Masters.m_vtMaster.size() - 1];
	}
	else{
		::XSysproxy().Masters.m_vtMaster.erase(::XSysproxy().Masters.m_vtMaster.begin() + iItmIndex);
		m_Master = ::XSysproxy().Masters.m_vtMaster[iItmIndex];
	}

	UpdateCtrl();

	int iCount = m_lstMaster.GetItemCount();
	if (iItmIndex>0 && iItmIndex < iCount) {
		m_lstMaster.SetFocus();
		m_lstMaster.SetItemState(iItmIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		m_lstMaster.SetSelectionMark(iItmIndex);
	}

	UpdateData(FALSE);
	
}	// end of void CMKVNTMasterPage::OnBnClickedMkvMstDelBtn()

void CMKVNTMasterPage::OnBnClickedMkvMstMdBtn()
{	
	UpdateData();

	int iItmIndex = GetListCtrlSelectItem();
	if (iItmIndex == -1)  return;

	if (m_Master.m_strFontName == "") {
		AfxMessageBox("请先选择对应的字库文件！");
		return;
	}

	m_Master.SetDefault();
	m_Master.m_strMasterName = m_strMName;
	m_Master.m_strFileName = m_strFileName;
	m_Master.m_strFontName = m_strFontName;
	m_Master.m_bDefault = m_bDefault;
	m_Master.m_iAcsender = m_iAscender;
	m_Master.m_iDescender = m_iDescender;
	m_Master.m_iTypoAscender = m_iTypoAscender;
	m_Master.m_iTypoDescender = m_iTypoDescender;

	int iCmlIndex = 1;
	CVrAxisRecords& AxsRcrd = ::XSysproxy().AxisRecords;	
	for (size_t i = 0; i < AxsRcrd.vtVrAxisInfs.size(); i++)
	{
		CVrAxisRecord& asRcd = AxsRcrd.vtVrAxisInfs[i];
		if (asRcd.m_strTag == AXES_WIDTH) {
			CVrAxisRecord	AxsRecrdValue;
			AxsRecrdValue.m_strTag = AXES_WIDTH;
			AxsRecrdValue.m_AxisValue.Format = 1;
			AxsRecrdValue.m_AxisValue.Value = m_iWidth;
			AxsRecrdValue.m_AxisValue.axisIndex = i;

			m_Master.m_AxisRecords.vtVrAxisInfs.push_back(AxsRecrdValue);
		}

		if (asRcd.m_strTag == AXES_WEIGHT) {
			CVrAxisRecord	AxsRecrdValue;

			AxsRecrdValue.m_strTag = AXES_WEIGHT;
			AxsRecrdValue.m_AxisValue.Format = 1;
			AxsRecrdValue.m_AxisValue.Value = m_iWeight;
			AxsRecrdValue.m_AxisValue.axisIndex = i;

			m_Master.m_AxisRecords.vtVrAxisInfs.push_back(AxsRecrdValue);
		}		
	}

	CVrMaster& Mster = ::XSysproxy().Masters.m_vtMaster[iItmIndex];
	Mster = m_Master;

	UpdateListCtrl();

}	// end of void CMKVNTMasterPage::OnBnClickedMkvMstMdBtn()

void CMKVNTMasterPage::OnBnClickedMkvMstUpBtn()
{
	int iItmIndex = GetListCtrlSelectItem();
	if (iItmIndex == -1)  return;
	if (iItmIndex == 0)  return;

	m_Master.SetDefault();
	m_Master = ::XSysproxy().Masters.m_vtMaster[iItmIndex-1];
	::XSysproxy().Masters.m_vtMaster.erase(::XSysproxy().Masters.m_vtMaster.begin()+iItmIndex-1);
	::XSysproxy().Masters.m_vtMaster.insert(::XSysproxy().Masters.m_vtMaster.begin()+ iItmIndex, m_Master);

	UpdateCtrl();

	int iCount = m_lstMaster.GetItemCount();
	if (iItmIndex>0 && iItmIndex < iCount) {
		iItmIndex--;
		m_lstMaster.SetFocus();
		m_lstMaster.SetItemState(iItmIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		m_lstMaster.SetSelectionMark(iItmIndex);
	}

	//UpdateData(FALSE);

}	// end of void CMKVNTMasterPage::OnBnClickedMkvMstUpBtn()

void CMKVNTMasterPage::OnBnClickedMkvMstDwnBtn()
{
	int iItmIndex = GetListCtrlSelectItem();
	if (iItmIndex == -1)  return;
	if (iItmIndex == ::XSysproxy().Masters.m_vtMaster.size()-1)  return;

	m_Master.SetDefault();
	m_Master = ::XSysproxy().Masters.m_vtMaster[iItmIndex + 1];
	::XSysproxy().Masters.m_vtMaster.erase(::XSysproxy().Masters.m_vtMaster.begin() + iItmIndex + 1);
	::XSysproxy().Masters.m_vtMaster.insert(::XSysproxy().Masters.m_vtMaster.begin() + iItmIndex, m_Master);

	UpdateCtrl();

	int iCount = m_lstMaster.GetItemCount();
	if (iItmIndex>0 && iItmIndex < iCount) {
		m_lstMaster.SetFocus();
		m_lstMaster.SetItemState(++iItmIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		m_lstMaster.SetSelectionMark(iItmIndex);
	}	

}	// end of void CMKVNTMasterPage::OnBnClickedMkvMstDwnBtn()

void CMKVNTMasterPage::InitControl()
{
	int   nColumnCount = m_lstMaster.GetHeaderCtrl()->GetItemCount();
	for (int i = 0; i < nColumnCount; i++){
		m_lstMaster.DeleteColumn(0);
	}

	DefaultMastCtrl();

	LVCOLUMN		tagColumn;
	ZeroMemory(&tagColumn, sizeof(LVCOLUMN));

	tagColumn.mask = LVCF_WIDTH | LVCF_TEXT;
	tagColumn.cx = 120;
	tagColumn.pszText = _T("name");
	m_lstMaster.InsertColumn(0, &tagColumn);

	CVrAxisRecords& AxsRcrd = ::XSysproxy().AxisRecords;
	int iCmlIndex = 1;
	for (size_t i = 0; i < AxsRcrd.vtVrAxisInfs.size(); i++)
	{
		CVrAxisRecord& asRcd =  AxsRcrd.vtVrAxisInfs[i];
		if (asRcd.m_strTag == AXES_WIDTH)	{
			tagColumn.cx = 120;
			tagColumn.pszText = (LPSTR)(LPCSTR)asRcd.m_strName;
			m_lstMaster.InsertColumn(iCmlIndex, &tagColumn);
			iCmlIndex++;

			GetDlgItem(IDC_MKV_MST_WDTH_EDT)->EnableWindow(1);

		}
		if (asRcd.m_strTag == AXES_WEIGHT) {
			tagColumn.cx = 120;
			tagColumn.pszText = (LPSTR)(LPCSTR)asRcd.m_strName;
			m_lstMaster.InsertColumn(iCmlIndex, &tagColumn);
			iCmlIndex++;

			GetDlgItem(IDC_MKV_MST_WGHT_EDT)->EnableWindow(1);
		}
	}

	tagColumn.cx = 120;
	tagColumn.pszText = _T("default");
	m_lstMaster.InsertColumn(iCmlIndex, &tagColumn);
	m_lstMaster.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

}	// end of void CMKVNTMasterPage::InitControl()

void CMKVNTMasterPage::UninitControl()
{


}	// end of void CMKVNTMasterPage::UninitControl()

void CMKVNTMasterPage::UpdateCtrl()
{	
	m_strMName = m_Master.m_strMasterName;
	m_strFileName = m_Master.m_strFileName;
	m_strFontName = m_Master.m_strFontName;
	m_bDefault = m_Master.m_bDefault;
	m_iAscender = m_Master.m_iAcsender;
	m_iDescender = m_Master.m_iDescender;
	m_iTypoAscender = m_Master.m_iTypoAscender;
	m_iTypoDescender = m_Master.m_iTypoDescender;

	for (size_t i = 0; i<m_Master.m_AxisRecords.vtVrAxisInfs.size(); i++){
		CVrAxisRecord& AxisRecord = m_Master.m_AxisRecords.vtVrAxisInfs[i];

		if (AxisRecord.m_strTag == AXES_WIDTH) {
			this->m_iWidth = AxisRecord.m_AxisValue.Value;
		}
		if (AxisRecord.m_strTag == AXES_WEIGHT) {
			this->m_iWeight = AxisRecord.m_AxisValue.Value;;
		}
	}

	UpdateListCtrl();
	UpdateData(FALSE);

}	// end of void CMKVNTMasterPage::UpdateCtrl() 

void CMKVNTMasterPage::UpdateListCtrl()
{
	m_lstMaster.DeleteAllItems();

	size_t sz = ::XSysproxy().Masters.m_vtMaster.size();
	for (size_t i = 0; i<sz; i++) {
		CVrMaster& Master = ::XSysproxy().Masters.m_vtMaster[i];

		LVITEM	tagLvItem;
		ZeroMemory(&tagLvItem, sizeof(LVITEM));

		tagLvItem.mask = LVIF_TEXT;
		tagLvItem.iItem = i;
		tagLvItem.iSubItem = 0;
		tagLvItem.pszText = new char[MAX_PATH];
		tagLvItem.cchTextMax = MAX_PATH;
		ZeroMemory(tagLvItem.pszText, MAX_PATH);
		// name
		sprintf_s(tagLvItem.pszText, MAX_PATH, "%s", (LPSTR)(LPCTSTR)Master.m_strMasterName);
		m_lstMaster.InsertItem(&tagLvItem);
		
		CVrAxisRecords& AxsRcrd = Master.m_AxisRecords;
		int iCmlIndex = 1;
		for (size_t i = 0; i < AxsRcrd.vtVrAxisInfs.size(); i++)
		{
			CVrAxisRecord& asRcd = AxsRcrd.vtVrAxisInfs[i];
			if (asRcd.m_strTag == AXES_WIDTH) {				
				tagLvItem.iSubItem = iCmlIndex;
				sprintf_s(tagLvItem.pszText, MAX_PATH, "%d", asRcd.m_AxisValue.Value);
				iCmlIndex++;

				m_lstMaster.SetItem(&tagLvItem);
			}
			if (asRcd.m_strTag == AXES_WEIGHT) {
				tagLvItem.iSubItem = iCmlIndex;
				sprintf_s(tagLvItem.pszText, MAX_PATH, "%03f", asRcd.m_AxisValue.Value);
				iCmlIndex++;

				m_lstMaster.SetItem(&tagLvItem);
			}
		}
		
		// default
		tagLvItem.iSubItem = 2;
		if (Master.m_bDefault)
			sprintf_s(tagLvItem.pszText, MAX_PATH, "%s", "Yes");
		else
			sprintf_s(tagLvItem.pszText, MAX_PATH, "%s", "No");
		m_lstMaster.SetItem(&tagLvItem);

		delete[] tagLvItem.pszText;
		tagLvItem.pszText = NULL;
	}	

}	// end of void CMKVNTMasterPage::UpdateListCtrl()

int CMKVNTMasterPage::GetListCtrlSelectItem()
{
	int nItem = -1;
	POSITION pos = m_lstMaster.GetFirstSelectedItemPosition();
	if (pos) {
		nItem = m_lstMaster.GetNextSelectedItem(pos);
	}
	return nItem;

}	// end of int CMKVNTMasterPage::GetListCtrlSelectItem()

void CMKVNTMasterPage::DefaultMastCtrl()
{
	GetDlgItem(IDC_MKV_MST_WGHT_EDT)->EnableWindow(0);
	GetDlgItem(IDC_MKV_MST_WDTH_EDT)->EnableWindow(0);

}	// end of void CMKVNTMasterPage::DefaultMastCtrl()

void CMKVNTMasterPage::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CPropertyPage::OnShowWindow(bShow, nStatus);
	if (bShow){
		InitControl();
		UpdateListCtrl();
	}

}	//end of void CMKVNTMasterPage::OnShowWindow()

void CMKVNTMasterPage::OnNMSetfocusMkvMstMstLst(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

}	// end of void CMKVNTMasterPage::OnNMSetfocusMkvMstMstLst()

void CMKVNTMasterPage::OnNMClickMkvMstMstLst(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	int iIndex = GetListCtrlSelectItem();
	if (iIndex < 0)return;

	m_Master = ::XSysproxy().Masters.m_vtMaster[iIndex];

	m_strMName = m_Master.m_strMasterName;
	m_strFileName = m_Master.m_strFileName;
	m_strFontName = m_Master.m_strFontName;
	m_bDefault = m_Master.m_bDefault;
	m_iAscender = m_Master.m_iAcsender;
	m_iDescender = m_Master.m_iDescender;
	m_iTypoAscender = m_Master.m_iTypoAscender;
	m_iTypoDescender = m_Master.m_iTypoDescender;
	
	CVrAxisRecords& AxsRcrd = ::XSysproxy().AxisRecords;
	int iCmlIndex = 1;
	for (size_t i = 0; i < AxsRcrd.vtVrAxisInfs.size(); i++)
	{
		CVrAxisRecord& asRcd = AxsRcrd.vtVrAxisInfs[i];
		if (asRcd.m_strTag == AXES_WIDTH) {			
			int iAxisIndex = m_Master.m_AxisRecords.FindAxisIndexEx(AXES_WIDTH);
			if (iAxisIndex > -1){
				m_iWidth = m_Master.m_AxisRecords.vtVrAxisInfs[iAxisIndex].m_AxisValue.Value;
			}			
		}

		if (asRcd.m_strTag == AXES_WEIGHT) {
			int iAxisIndex = m_Master.m_AxisRecords.FindAxisIndexEx(AXES_WEIGHT);
			if (iAxisIndex > -1) {
				m_iWeight = m_Master.m_AxisRecords.vtVrAxisInfs[iAxisIndex].m_AxisValue.Value;
			}
		}
	}

	UpdateData(FALSE);

	*pResult = 0;

}	// end of void CMKVNTMasterPage::OnNMClickMkvMstMstLst()
