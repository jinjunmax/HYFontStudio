// MKVNTInstncPage.cpp : 实现文件
//

#include "stdafx.h"
#include "HYFontSmartShaper.h"
#include "MKVNTInstncPage.h"
#include "afxdialogex.h"


// CMKVNTInstncPage 对话框

IMPLEMENT_DYNAMIC(CMKVNTInstncPage, CPropertyPage)

CMKVNTInstncPage::CMKVNTInstncPage()
	: CPropertyPage(IDD_FNT_MKVNT_INSTNC_PAGE)
{
	m_iWeight = 400;
	m_iWidth = 5;
	m_iAxWeight = 0;
	m_iAxWidth = 0;
	m_iAxHeight = 0;
	m_strStyleName = "";

}	// end of CMKVNTInstncPage::CMKVNTInstncPage()

CMKVNTInstncPage::~CMKVNTInstncPage()
{

}	// end of CMKVNTInstncPage::~CMKVNTInstncPage()

void CMKVNTInstncPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MKV_INSTNC_INSTNC_LST, m_lstInstance);
	DDX_Text(pDX, IDC_MKV_INSTNC_STYNM_EDT, m_strStyleName);
	DDX_Text(pDX, IDC_MKV_INSTNC_WGHT_EDT, m_iWeight);
	DDX_Text(pDX, IDC_MKV_INSTNC_WDTH_EDT, m_iWidth);
	DDX_Text(pDX, IDC_MKV_INSTNC_AXWGHT_EDT, m_iAxWeight);
	DDX_Text(pDX, IDC_MKV_INSTNC_AXWDTH_EDT, m_iAxWidth);
	DDX_Text(pDX, IDC_MKV_INSTNC_AXHGHT_EDT, m_iAxHeight);

}	// end of void CMKVNTInstncPage::DoDataExchange(CDataExchange* pDX)

BEGIN_MESSAGE_MAP(CMKVNTInstncPage, CPropertyPage)
	ON_BN_CLICKED(IDC_MKV_INSTNC_ADD_BTN, &CMKVNTInstncPage::OnBnClickedMkvInstncAddBtn)
	ON_BN_CLICKED(IDC_MKV_INSTNC_DEL_BTN, &CMKVNTInstncPage::OnBnClickedMkvInstncDelBtn)
	ON_BN_CLICKED(IDC_MKV_INSTNC_UP_BTN, &CMKVNTInstncPage::OnBnClickedMkvInstncUpBtn)
	ON_BN_CLICKED(IDC_MKV_INSTNC_DWN_BTN, &CMKVNTInstncPage::OnBnClickedMkvInstncDwnBtn)
	ON_BN_CLICKED(IDC_MKV_INSTNC_MDFY_BTN, &CMKVNTInstncPage::OnBnClickedMkvInstncMdfyBtn)
	ON_BN_CLICKED(IDC_MKV_INSTNC_FMMASTER_BTN, &CMKVNTInstncPage::OnBnClickedMkvInstncFmmasterBtn)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_MKV_INSTNC_INSTNC_LST, &CMKVNTInstncPage::OnLvnItemchangedMkvInstncInstncLst)
END_MESSAGE_MAP()

// CMKVNTInstncPage 消息处理程序
void CMKVNTInstncPage::OnBnClickedMkvInstncAddBtn()
{	
	UpdateData();

	m_Instance.SetDefault();
	m_Instance.m_strStyleName = m_strStyleName;
	m_Instance.m_iWeight = m_iWeight;
	m_Instance.m_iWidth = m_iWidth;

	CVrAxisRecords& AxsRcrd = ::XSysproxy().AxisRecords;
	int iCmlIndex = 1;
	for (size_t i = 0; i < AxsRcrd.vtVrAxisInfs.size(); i++)
	{
		CVrAxisRecord& asRcd = AxsRcrd.vtVrAxisInfs[i];
		if (asRcd.m_strTag == AXES_WIDTH) {
			CVrAxisRecord	AxsRecrdValue;
			AxsRecrdValue.m_strTag = AXES_WIDTH;
			AxsRecrdValue.m_AxisValue.Format = 1;
			AxsRecrdValue.m_AxisValue.Value = m_iAxWidth;
			AxsRecrdValue.m_AxisValue.axisIndex = i;

			m_Instance.m_AxisRecords.vtVrAxisInfs.push_back(AxsRecrdValue);
		}

		if (asRcd.m_strTag == AXES_WEIGHT) {
			CVrAxisRecord	AxsRecrdValue;

			AxsRecrdValue.m_strTag = AXES_WEIGHT;
			AxsRecrdValue.m_AxisValue.Format = 1;
			AxsRecrdValue.m_AxisValue.Value = m_iAxWeight;
			AxsRecrdValue.m_AxisValue.axisIndex = i;

			m_Instance.m_AxisRecords.vtVrAxisInfs.push_back(AxsRecrdValue);
		}
	}

	::XSysproxy().Instances.m_vtinstances.push_back(m_Instance);

	UpdateListCtrl();

}	//end of void CMKVNTInstncPage::OnBnClickedMkvInstncAddBtn()

void CMKVNTInstncPage::OnBnClickedMkvInstncDelBtn()
{
	// TODO: 在此添加控件通知处理程序代码
	int iItmIndex = GetListCtrlSelectItem();
	if (iItmIndex == -1)  return;

	if (iItmIndex == 0) {
		if (::XSysproxy().Instances.m_vtinstances.size() == 1)	{
			m_Instance.SetDefault();
			::XSysproxy().Instances.m_vtinstances.clear();
		}
		else{
			::XSysproxy().Instances.m_vtinstances.erase(::XSysproxy().Instances.m_vtinstances.begin() + iItmIndex);
			m_Instance = ::XSysproxy().Instances.m_vtinstances[iItmIndex];
		}
	}
	else if (iItmIndex == ::XSysproxy().Instances.m_vtinstances.size() - 1) {
		m_Instance = ::XSysproxy().Instances.m_vtinstances[::XSysproxy().Instances.m_vtinstances.size() - 1];
		::XSysproxy().Instances.m_vtinstances.erase(::XSysproxy().Instances.m_vtinstances.begin() + iItmIndex);
		iItmIndex--;
		m_Instance = ::XSysproxy().Instances.m_vtinstances[::XSysproxy().Instances.m_vtinstances.size() - 1];
	}
	else {
		::XSysproxy().Instances.m_vtinstances.erase(::XSysproxy().Instances.m_vtinstances.begin() + iItmIndex);
		m_Instance = ::XSysproxy().Instances.m_vtinstances[iItmIndex];
	}
	
	UpdateCtrl();

	int iCount = m_lstInstance.GetItemCount();
	if (iItmIndex>0 && iItmIndex < iCount) {
		m_lstInstance.SetFocus();
		m_lstInstance.SetItemState(iItmIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		m_lstInstance.SetSelectionMark(iItmIndex);
	}

	UpdateData(FALSE);

}	// end of void CMKVNTInstncPage::OnBnClickedMkvInstncDelBtn()

void CMKVNTInstncPage::OnBnClickedMkvInstncMdfyBtn()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();

	int iItmIndex = GetListCtrlSelectItem();
	if (iItmIndex == -1)  return;

	m_Instance.SetDefault();
	m_Instance.m_strStyleName = m_strStyleName;
	m_Instance.m_iWeight = m_iWeight;
	m_Instance.m_iWidth = m_iWidth;

	int iCmlIndex = 1;
	CVrAxisRecords& AxsRcrd = ::XSysproxy().AxisRecords;
	for (size_t i = 0; i < AxsRcrd.vtVrAxisInfs.size(); i++)
	{
		CVrAxisRecord& asRcd = AxsRcrd.vtVrAxisInfs[i];
		if (asRcd.m_strTag == AXES_WIDTH) {
			CVrAxisRecord	AxsRecrdValue;
			AxsRecrdValue.m_strTag = AXES_WIDTH;
			AxsRecrdValue.m_AxisValue.Format = 1;
			AxsRecrdValue.m_AxisValue.Value = m_iAxWidth;
			AxsRecrdValue.m_AxisValue.axisIndex = i;

			m_Instance.m_AxisRecords.vtVrAxisInfs.push_back(AxsRecrdValue);
		}

		if (asRcd.m_strTag == AXES_WEIGHT) {
			CVrAxisRecord	AxsRecrdValue;

			AxsRecrdValue.m_strTag = AXES_WEIGHT;
			AxsRecrdValue.m_AxisValue.Format = 1;
			AxsRecrdValue.m_AxisValue.Value = m_iAxWeight;
			AxsRecrdValue.m_AxisValue.axisIndex = i;

			m_Instance.m_AxisRecords.vtVrAxisInfs.push_back(AxsRecrdValue);
		}
	}

	CVrInstance& Instance = ::XSysproxy().Instances.m_vtinstances[iItmIndex];
	Instance = m_Instance;

	UpdateListCtrl();

}	//end of void CMKVNTInstncPage::OnBnClickedMkvInstncMdfyBtn()

void CMKVNTInstncPage::OnBnClickedMkvInstncUpBtn()
{
	// TODO: 在此添加控件通知处理程序代码
	int iItmIndex = GetListCtrlSelectItem();
	if (iItmIndex == -1)  return;
	if (iItmIndex == 0)  return;

	m_Instance.SetDefault();
	m_Instance = ::XSysproxy().Instances.m_vtinstances[iItmIndex - 1];
	::XSysproxy().Instances.m_vtinstances.erase(::XSysproxy().Instances.m_vtinstances.begin() + iItmIndex - 1);
	::XSysproxy().Instances.m_vtinstances.insert(::XSysproxy().Instances.m_vtinstances.begin() + iItmIndex, m_Instance);

	UpdateCtrl();

	int iCount = m_lstInstance.GetItemCount();
	if (iItmIndex>0 && iItmIndex < iCount) {
		iItmIndex--;
		m_lstInstance.SetFocus();
		m_lstInstance.SetItemState(iItmIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		m_lstInstance.SetSelectionMark(iItmIndex);
	}	

}	//end of void CMKVNTInstncPage::OnBnClickedMkvInstncUpBtn()

void CMKVNTInstncPage::OnBnClickedMkvInstncDwnBtn()
{
	int iItmIndex = GetListCtrlSelectItem();
	if (iItmIndex == -1)  return;
	if (iItmIndex == ::XSysproxy().Instances.m_vtinstances.size() - 1)  return;

	m_Instance.SetDefault();
	m_Instance = ::XSysproxy().Instances.m_vtinstances[iItmIndex + 1];
	::XSysproxy().Instances.m_vtinstances.erase(::XSysproxy().Instances.m_vtinstances.begin() + iItmIndex + 1);
	::XSysproxy().Instances.m_vtinstances.insert(::XSysproxy().Instances.m_vtinstances.begin() + iItmIndex, m_Instance);

	UpdateCtrl();

	int iCount = m_lstInstance.GetItemCount();
	if (iItmIndex>0 && iItmIndex < iCount) {
		m_lstInstance.SetFocus();
		m_lstInstance.SetItemState(++iItmIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		m_lstInstance.SetSelectionMark(iItmIndex);
	}

}	// end of void CMKVNTInstncPage::OnBnClickedMkvInstncDwnBtn()

BOOL CMKVNTInstncPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  在此添加额外的初始化
	InitControl();
	UpdateData();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE

}	// end of BOOL CMKVNTInstncPage::OnInitDialog() 

void	CMKVNTInstncPage::InitControl()
{
	int   nColumnCount = m_lstInstance.GetHeaderCtrl()->GetItemCount();
	for (int i = 0; i < nColumnCount; i++) {
		m_lstInstance.DeleteColumn(0);
	}

	DefaultInstanceCtrl();

	LVCOLUMN		tagColumn;
	ZeroMemory(&tagColumn, sizeof(LVCOLUMN));

	tagColumn.mask = LVCF_WIDTH | LVCF_TEXT;
	tagColumn.cx = 120;
	tagColumn.pszText = _T("name");
	m_lstInstance.InsertColumn(0, &tagColumn);

	CVrAxisRecords& AxsRcrd = ::XSysproxy().AxisRecords;
	int iCmlIndex = 1;
	for (size_t i = 0; i < AxsRcrd.vtVrAxisInfs.size(); i++){
		CVrAxisRecord& asRcd = AxsRcrd.vtVrAxisInfs[i];
		if (asRcd.m_strTag == AXES_WIDTH) {
			tagColumn.cx = 120;
			tagColumn.pszText = (LPSTR)(LPCSTR)asRcd.m_strName;
			m_lstInstance.InsertColumn(iCmlIndex, &tagColumn);
			iCmlIndex++;
			
			GetDlgItem(IDC_MKV_INSTNC_AXWDTH_EDT)->EnableWindow(1);
		}
		if (asRcd.m_strTag == AXES_WEIGHT) {
			tagColumn.cx = 120;
			tagColumn.pszText = (LPSTR)(LPCSTR)asRcd.m_strName;
			m_lstInstance.InsertColumn(iCmlIndex, &tagColumn);
			iCmlIndex++;
			
			GetDlgItem(IDC_MKV_INSTNC_AXWGHT_EDT)->EnableWindow(1);
		}
	}

	m_lstInstance.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

}	// end of void	InitControl()

void	CMKVNTInstncPage::UninitControl()
{


}	// end of void	UninitControl()

void	CMKVNTInstncPage::UpdateCtrl()
{
	m_strStyleName = m_Instance.m_strStyleName;
	m_iWeight = m_Instance.m_iWeight;
	m_iWidth = m_Instance.m_iWidth;

	for (size_t i = 0; i<m_Instance.m_AxisRecords.vtVrAxisInfs.size(); i++) {
		CVrAxisRecord& AxisRecord = m_Instance.m_AxisRecords.vtVrAxisInfs[i];

		if (AxisRecord.m_strTag == AXES_WIDTH) {
			this->m_iAxWidth = AxisRecord.m_AxisValue.Value;
		}
		if (AxisRecord.m_strTag == AXES_WEIGHT) {
			this->m_iAxWeight = AxisRecord.m_AxisValue.Value;;
		}
	}

	UpdateListCtrl();

	UpdateData(FALSE);

}	// end of void	CMKVNTInstncPage::UpdateCtrl() 

void	CMKVNTInstncPage::UpdateListCtrl()
{
	m_lstInstance.DeleteAllItems();

	size_t sz = ::XSysproxy().Instances.m_vtinstances.size();
	for (size_t i = 0; i<sz; i++) {
		CVrInstance& Instance = ::XSysproxy().Instances.m_vtinstances[i];

		LVITEM	tagLvItem;
		ZeroMemory(&tagLvItem, sizeof(LVITEM));

		tagLvItem.mask = LVIF_TEXT;
		tagLvItem.iItem = i;
		tagLvItem.iSubItem = 0;
		tagLvItem.pszText = new char[MAX_PATH];
		tagLvItem.cchTextMax = MAX_PATH;
		ZeroMemory(tagLvItem.pszText, MAX_PATH);
		// name
		sprintf_s(tagLvItem.pszText, MAX_PATH, "%s", (LPSTR)(LPCTSTR)Instance.m_strStyleName);
		m_lstInstance.InsertItem(&tagLvItem);

		CVrAxisRecords& AxsRcrd = Instance.m_AxisRecords;
		int iCmlIndex = 1;
		for (size_t i = 0; i < AxsRcrd.vtVrAxisInfs.size(); i++)
		{
			CVrAxisRecord& asRcd = AxsRcrd.vtVrAxisInfs[i];
			if (asRcd.m_strTag == AXES_WIDTH) {
				tagLvItem.iSubItem = iCmlIndex;
				sprintf_s(tagLvItem.pszText, MAX_PATH, "%d", asRcd.m_AxisValue.Value);
				iCmlIndex++;

				m_lstInstance.SetItem(&tagLvItem);
			}
			if (asRcd.m_strTag == AXES_WEIGHT) {
				tagLvItem.iSubItem = iCmlIndex;
				sprintf_s(tagLvItem.pszText, MAX_PATH, "%03f", asRcd.m_AxisValue.Value);
				iCmlIndex++;

				m_lstInstance.SetItem(&tagLvItem);
			}
		}	

		delete[] tagLvItem.pszText;
		tagLvItem.pszText = NULL;
	}

	UpdateData(FALSE);

}	// end of void	UpdateListCtrl()

int		CMKVNTInstncPage::GetListCtrlSelectItem()
{
	int nItem = -1;
	POSITION pos = m_lstInstance.GetFirstSelectedItemPosition();
	if (pos) {
		nItem = m_lstInstance.GetNextSelectedItem(pos);
	}
	return nItem;

}	// end of int GetListCtrlSelectItem()

void	CMKVNTInstncPage::DefaultInstanceCtrl()
{	
	GetDlgItem(IDC_MKV_INSTNC_AXWGHT_EDT)->EnableWindow(0);
	GetDlgItem(IDC_MKV_INSTNC_AXWDTH_EDT)->EnableWindow(0);

}	// end of void DefaultInstanceCtrl()

void CMKVNTInstncPage::OnBnClickedMkvInstncFmmasterBtn()
{	
	for (size_t i = 0; i < ::XSysproxy().Masters.m_vtMaster.size(); i++){
		CVrMaster& Master = ::XSysproxy().Masters.m_vtMaster[i];

		if (::XSysproxy().Instances.FindInstancesIdx(Master.m_strMasterName) != -1){
			AfxMessageBox("已经存在同名实例样式名称！");
			return;		
		}

		if (::XSysproxy().Instances.FindInstancesIdx(Master.m_AxisRecords) != -1){
			AfxMessageBox("已经存在同名轴值！");
			return;
		}
	}

	for (size_t i = 0; i < ::XSysproxy().Masters.m_vtMaster.size(); i++) {
		CVrMaster& Master = ::XSysproxy().Masters.m_vtMaster[i];

		m_Instance.SetDefault();
		m_Instance.m_strStyleName = Master.m_strMasterName;
		m_Instance.m_iWeight = m_iWeight;
		m_Instance.m_iWidth = m_iWidth;
		m_Instance.m_AxisRecords = Master.m_AxisRecords;

		::XSysproxy().Instances.m_vtinstances.push_back(m_Instance);
	}

	UpdateListCtrl();


}	// end of void CMKVNTInstncPage::OnBnClickedMkvInstncFmmasterBtn()

void CMKVNTInstncPage::OnLvnItemchangedMkvInstncInstncLst(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	int iIndex = GetListCtrlSelectItem();
	if (iIndex < 0)return;

	m_Instance = ::XSysproxy().Instances.m_vtinstances[iIndex];

	this->m_strStyleName = m_Instance.m_strStyleName;
	this->m_iWeight = m_Instance.m_iWeight;
	this->m_iWidth = m_Instance.m_iWidth;
	
	CVrAxisRecords& AxsRcrd = ::XSysproxy().AxisRecords;
	int iCmlIndex = 1;
	for (size_t i = 0; i < AxsRcrd.vtVrAxisInfs.size(); i++)
	{
		CVrAxisRecord& asRcd = AxsRcrd.vtVrAxisInfs[i];
		if (asRcd.m_strTag == AXES_WIDTH) {
			int iAxisIndex = m_Instance.m_AxisRecords.FindAxisIndexEx(AXES_WIDTH);
			if (iAxisIndex > -1) {
				this->m_iAxWidth = m_Instance.m_AxisRecords.vtVrAxisInfs[iAxisIndex].m_AxisValue.Value;
			}
		}

		if (asRcd.m_strTag == AXES_WEIGHT) {
			int iAxisIndex = m_Instance.m_AxisRecords.FindAxisIndexEx(AXES_WEIGHT);
			if (iAxisIndex > -1) {
				this->m_iAxWeight = m_Instance.m_AxisRecords.vtVrAxisInfs[iAxisIndex].m_AxisValue.Value;
			}
		}
	}

	UpdateData(FALSE);

	*pResult = 0;

}	// end of void CMKVNTInstncPage::OnLvnItemchangedMkvInstncInstncLst()
