#pragma once
#include "afxwin.h"

// CFontSetupOptionPage 对话框
class CFontSetupOptionPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CFontSetupOptionPage)

public:
	CFontSetupOptionPage();
	virtual ~CFontSetupOptionPage();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FNT_SETUP_OPTION_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()

protected:
	void		Init();
	void		Uninit();

	BOOL		m_bReVert;
	BOOL		m_bRePsName;
	BOOL		m_bReSortUni;
	BOOL		m_bYiTiZi;
	BOOL		m_brmPoint;
	BOOL		m_bAlignpnt;
	BOOL		m_bRename;
	BOOL		m_bCustomCmap;
	BOOL		m_bSetAdw;
	BOOL		m_bSetAdh;
	BOOL		m_bLayout;
	BOOL		m_bCorrect;
	BOOL		m_bHanyi;
	BOOL		m_bKangxi;

	CString		m_strCHSFaimlyName;
	CString		m_strCHSSubName;
	CString		m_strENGFamilyName;
	CString		m_strENGSubName;
	CString		m_strVersion;

public:
	void			Save();
	CHYFontCodec*	m_pHYFoncCodec;

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();	
	afx_msg void OnBnClickedPtnRenameChk();
	
	
};
