#pragma once

class Gridclip
{
public:
	int iPos1;
	int iPos2;
	int iSum;
};

class Gridclip1
{
public:
	int iPos;	
	int iValue;
};

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
	void					LoadFontFeature(CString strProfileName, HYFONTCODEC::CHYFontCodec& hyFontCodec);
	void					StoreFontFeature(CString strProfileName, HYFONTCODEC::CHYFontCodec& hyFontCodec);	
	void					LoadAdvancedTypographicTables(HYFONTCODEC::CHYFontCodec& hyFontCodec);

	BOOL					MulToUniHL(char* pDest, int& iDestlen, char* pSrc, int iSrclen);
	void					FindPeakAndTrough(std::vector<int>& vtSum, std::vector<int>& vtPeak, std::vector<int>& vtTrough);
	void					FindPeak(std::vector<int>& vtSum, std::vector<int>& vtPeak);
	void					FindTrough(std::vector<int>& vtSum, std::vector<int>& vtTrough);

	void					LinearSmooth5(std::vector<int>& in, std::vector<int>& out, int N);
	void					LinearSmooth7(std::vector<int>& in, std::vector<int>& out, int N);
	void					QuadraticSmooth5(std::vector<int>& in, std::vector<int>& out, int N);
	void					FindMargin(std::vector<int>& vtSum, std::vector<int>& Margin, int TopNum);
	void					FindMargin1(std::vector<int>& vtSum, std::vector<int>& Margin, int TopNum);
	double					Angle(CPoint* pt1, CPoint* pt2, CPoint* ptCen);
	int						FindMaxValuePos(std::vector<int>& vt, int Pos1, int Pos2);

public:
	CString						m_strAppDir;
	CString						m_strSysDir;
	CString						m_strBlackBkName;
	CString						m_strWhiteBkName;

	CWnd*						m_pMainFrame;
	HYFONTCODEC::CHYFontCodec	m_hyFontCodec;
	DRAWFONTPARAM				m_tagDrawParam;
	HY_OPTION_PRM				m_tagOpeionPrm;
	HY_SCAN_PRM					m_tagScanPrm;
	HY_IMG_OPTIM				m_tagImgOptim;
	double						m_fBaseLine;
	CString						m_strBulidInf;
	std::vector<float>			m_vtNormalizedVariation;
	CRect						m_rctTraceRect;

	int							m_iMinArea;
	int							m_iMaxArea;
	int							m_iMinAngle;
	int							m_iMaxAngle;
	
};
