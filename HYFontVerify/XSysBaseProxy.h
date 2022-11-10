#pragma once

class CXSysBaseProxy
{
public:
	CXSysBaseProxy(void);
	virtual ~CXSysBaseProxy(void);

public:
	BOOL					Init(CString strHelpFile);
	void					Uninit();

protected:
	void					InitParam();
	void					UninitParm();

public:
	BOOL					IsWndValid(CWnd* pWnd);
	LRESULT					UpdateAllViews(UINT nMsg, LPARAM lParam);

	void					LoadLocalProfile();
	void					StoreLocalProfile();
public:
	CString						m_strAppDir;
	CString						m_strSysDir;	
	CWnd*						m_pMainFrame;
};
