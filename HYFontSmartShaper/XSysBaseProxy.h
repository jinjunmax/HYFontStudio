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
	void					LoadAdvancedTypographicTables(CHYFontCodec* pFntCdc);

	BOOL					MulToUniHL(char* pDest, int& iDestlen, char* pSrc, int iSrclen);
	int						FindString(std::vector<CString>& vt, CString str);

public:
	CString						m_strAppDir;
	CString						m_strSysDir;	
	CWnd*						m_pMainFrame;
	CHYFontCodec				m_hyFontCodec;
	DRAWFONTPARAM				m_tagDrawParam;
	HY_OPTION_PRM				m_tagOpeionPrm;
	double						m_fBaseLine;
	std::vector<float>			m_vtNormalizedVariation;
	std::string					m_strLogFile;
	std::vector<unsigned long>	m_vtOvrrdUni; //旧标准中的编码
};
