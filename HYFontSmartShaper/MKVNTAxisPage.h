#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CMKVNTAxisPage 对话框

class CMKVNTAxisPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CMKVNTAxisPage)

public:
	CMKVNTAxisPage();
	virtual ~CMKVNTAxisPage();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FNT_MKVNT_AXS_PAGE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedMkvAxsAddBtn();
	afx_msg void OnBnClickedMkvAxsDelBtn();
	virtual BOOL OnInitDialog();

public:
	void	InitControl();
	void	UninitControl();
	void	UpdateListCtrl();
	int		GetListCtrlSelectItem();

	CComboBox				m_cmbAxesTag;
	BOOL					m_bHidden;
	CListCtrl				m_lstAxes;
	std::vector<CString>	m_vtAxesTag;
	CRegistryAxises			m_RgstryAxis;

	afx_msg void OnNMSetfocusMkvAxsAxsLst(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedMkvAxsAxsLst(NMHDR *pNMHDR, LRESULT *pResult);
};
