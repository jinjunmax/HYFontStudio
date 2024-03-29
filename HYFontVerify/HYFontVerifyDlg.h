
// HYFontVerifyDlg.h: 头文件
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CHYFontVerifyDlg 对话框
class CHYFontVerifyDlg : public CDialog
{
// 构造
public:
	CHYFontVerifyDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HYFONTVERIFY_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	CMenu m_MainMn;
	
	std::vector<CFontVerifyResult>	m_vtFntVrfResult;
	std::vector<int>				m_vtReusltIndex;
	CHYFontCodec					fontCodec;
	// 生成的消息映射函数
	virtual BOOL	OnInitDialog();
	afx_msg void	OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void	OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg	LRESULT OnUpdateAllViews(WPARAM waram, LPARAM lparam);
	DECLARE_MESSAGE_MAP()

public:	
	afx_msg void	OnMnOpenfile();
	afx_msg void	OnNMCustomdrawLstFile(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void	OnNMCustomdrawLstFntinf(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void	OnBnClickedHckErr();
	afx_msg void	OnBnClickedHckWrn();
	afx_msg void	OnNMSetfocusLstFile(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void	OnNMClickLstFile(NMHDR *pNMHDR, LRESULT *pResult);

public:
	void			UpdateFontFileLst();
	void			UpdateFontInofLst(int iResultIndex);
	void			FixFont(const char* strFontFile, const char* strNewFontFile);
protected:
	CListBox		m_lstBxTraceInf;	
	CListCtrl		m_lstCntrlFontResult;
	CListCtrl		m_lstCntrlFntInf;
	BOOL			m_bError;
	BOOL			m_bWarn;
	BOOL			m_bAllInf;
	int				m_TimerID;
public:	
	afx_msg void	OnFntfixMn();
	afx_msg void	OnTimer(UINT_PTR nIDEvent);
	afx_msg void	OnDestroy();
	
	afx_msg void	OnBnClickedHckInf();
};
