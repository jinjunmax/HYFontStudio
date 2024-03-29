#pragma once

#include "MKVNTFontPage.h"
#include "MKVNTAxisPage.h"
#include "MKVNTMasterPage.h"
#include "MKVNTInstncPage.h"

// CMKVNTSheet
class CMKVNTSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CMKVNTSheet)

public:
	CMKVNTSheet();
	CMKVNTSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CMKVNTSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CMKVNTSheet();
public:
	void			Init();
	void			Uninit();

	CMKVNTFontPage		FntPage;
	CMKVNTAxisPage		AxisPage;
	CMKVNTMasterPage	MasterPage;
	CMKVNTInstncPage	InstancePage;
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};


