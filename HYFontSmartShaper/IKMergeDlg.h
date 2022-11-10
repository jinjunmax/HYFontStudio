#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CIKMergeDlg dialog

class CIKMergeDlg : public CDialog
{
	DECLARE_DYNAMIC(CIKMergeDlg)

public:
	CIKMergeDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CIKMergeDlg();

// Dialog Data
	enum { IDD = IDD_IK_MRG_DLG };

public:
	void	SetDefault();
	void	InitParam();
	void	UninitParam();
	void	IniCtrl();
	void	UninitCtrl();
	void	UpdateListCtrl();
	int		GetListCtrlSelectItem();
	void	PrintMergeInfo(CString strInfo);
	void	ResetPrintMergeInfo();
	int		IkMerge(CString strDstIK);
	int		IkS2B(CString strSIKFile,BOOL bHead);	

	CListCtrl						m_lstIkIFiles;
	CListBox						m_lstbMergerInfo;	
	CStringArray					m_aryIKFiles;	
	HYFONTCODEC::CIKFont			m_IKMergeFont;
	CIKHeader						m_MergeIKHd;	

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:	
	afx_msg void OnLvnItemchangedIkMrgLst(NMHDR *pNMHDR, LRESULT *pResult);	
	afx_msg void OnBnClickedIkMrgDelBtn();
	afx_msg void OnBnClickedIkMrgBtn();
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedIkMrgUpBtn();
	afx_msg void OnBnClickedIkMrgDownBtn();	
	afx_msg void OnBnClickedIkMrgIkfBtn();
	afx_msg void OnBnClickedIkMrgClearBtn();
};
