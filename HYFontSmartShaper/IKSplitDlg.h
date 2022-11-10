#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CIKSplitDlg dialog
class CIKSplitDlg : public CDialog
{
	DECLARE_DYNAMIC(CIKSplitDlg)

public:
	CIKSplitDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CIKSplitDlg();

// Dialog Data
	enum { IDD = IDD_IK_SPLT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	void	SetDefault();
	void	InitParam();
	void	UninitParam();
	void	IniCtrl();
	void	UpdateCtrl();
	void	UninitCtrl();
	void	UpdateListCtrl();
	int		GetListCtrlSelectItem();
	void	PrintMergeInfo(CString strInfo);
	void	ResetPrintMergeInfo();

	void	Prepare27533();	
	void	IkMerge();
	void	IkSmlTo27533(CString strSIKFile, CString strSTXT,BOOL bHead);
	void	Extract(CString strDstIKFile, int iExtractOrder);
public:

	afx_msg void OnBnClickedIkSpltBtn();
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedIkSpltCtxtBtn();
	afx_msg void OnBnClickedIkSpltIkfBtn();
	afx_msg void OnBnClickedIkSpltTxtfBtn();
	afx_msg void OnLvnItemchangedIkSpltLst(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedIkSpltAddBtn();
	afx_msg void OnBnClickedIkSpltDelBtn();
	afx_msg void OnCbnSelchangeIkSporderCmb();

public:	
	CString							m_strSrcIK;
	CString							m_strSrcTXT;
	CStringArray					m_aryIKFiles;
	CStringArray					m_aryTxtFiles;
	CListBox						m_lstbMergerInfo;
	HYFONTCODEC::CIKFont			m_ikf27533;
	std::vector<unsigned long>		m_vtS27533;
	std::vector<unsigned long>		m_vtP27533;
	std::vector<unsigned long>		m_vtCustom;
	CComboBox						m_cmbExtractOrder;	
	CString							m_strCustom;
	CListCtrl						m_lstIkIFiles;	
};
