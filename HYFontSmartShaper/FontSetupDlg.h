#pragma once

#include "FontSetupSheet.h"
#include "FontSetupEMProPage.h"

// CFontSetupDlg dialog

class CFontSetupDlg : public CDialog
{
	DECLARE_DYNAMIC(CFontSetupDlg)

public:
	CFontSetupDlg(CWnd* pParent = NULL);   // standard constructor
	CFontSetupDlg(HYFONTCODEC::CHYFontCodec*	pFontCodec, CWnd* pParent = NULL);   // standard constructor

	virtual ~CFontSetupDlg();

// Dialog Data
	enum { IDD = IDD_FNT_SETUP_DLG };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support	

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

public:
	CFontSetUpSheet				m_sheet;
	HYFONTCODEC::CHYFontCodec*	m_pFontCodec;
};

