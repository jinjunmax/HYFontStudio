#pragma once

#include "FontSetupEMProPage.h"
#include "FontSetupCFFProPage.h"
#include "FontSetupOptionPage.h"

// CTTFSetUpDlg
class CFontSetUpSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CFontSetUpSheet)

public:
	CFontSetUpSheet();
	CFontSetUpSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CFontSetUpSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CFontSetUpSheet();
public:	
	void			SaveInfo();
	virtual BOOL	OnInitDialog();
	void			Init();
	void			Uninit();
public:
	CTTFSetupEMProPage			m_EMProPage;
	CFontSetupCFFProPage		m_CFFProPage;
	CFontSetupOptionPage		m_OptionPage;
	HYFONTCODEC::CHYFontCodec*	m_pHYFoncCodec;
protected:
	DECLARE_MESSAGE_MAP()
};


