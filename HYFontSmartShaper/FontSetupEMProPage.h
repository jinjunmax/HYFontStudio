#pragma once
#include "HYFontSmartShaper.h"
// CTTFSetupEMProPage dialog
class CTTFSetupEMProPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CTTFSetupEMProPage)

public:
	CTTFSetupEMProPage();
	virtual ~CTTFSetupEMProPage();

// Dialog Data
	enum { IDD = IDD_FNT_SETUP_EM_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
public:	
	void	SaveInfo();
	
};
