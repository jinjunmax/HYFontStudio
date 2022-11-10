#pragma once
#include "afxwin.h"


// CMakeTTCDlg 对话框

class CMakeTTCDlg : public CDialog
{
	DECLARE_DYNAMIC(CMakeTTCDlg)

public:
	CMakeTTCDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMakeTTCDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MTTC_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedMttcAddBtn();
	afx_msg void OnBnClickedMttcDelBtn();	
	afx_msg void OnBnClickedMttcMkeBtn();

protected:
	void		UpdateListCtrl();	

public:
	CListBox			m_lstFnts;	
	std::vector<string>	m_vtFntName;

	afx_msg void OnLbnSelchangeMttcFntsLst();
	virtual BOOL OnInitDialog();
};
