#pragma once
#include "afxwin.h"

#define PLANE00_BASIC_LATIN				"Basic Latin"
#define PLANE00_LATIN_1_SUPPLEMENT		"Latin-1 Supplement"
#define PLANE00_LATIN_EXTENDED_A		"Latin Extended-A"
#define PLANE00_LATIN_EXTENDED_B		"Latin Extended-B"

class  CUnicodeSection
{
	public:
		CUnicodeSection();
		~CUnicodeSection();
	public:
		CString				m_strEngName;
		CString				m_strHanName;
		unsigned long		uniStart;
		unsigned long		uniEnd;
		int					HitCout;
};

class  CUnicodeMapping
{
public:
	CUnicodeMapping();
	~CUnicodeMapping();

	void	Init();
	void	Hit(unsigned long uni);
public:
	std::vector<CUnicodeSection> vtUniSecitons;

};








// CFontInfDlg 对话框
class CFontInfDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFontInfDlg)

public:
	CFontInfDlg(CWnd* pParent = NULL);   // 标准构造函数
	CFontInfDlg(CHYFontCodec* p,CWnd* pParent = NULL);
	virtual ~CFontInfDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FNT_INF_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();

	void	FontMetricsInf();
	void	FontUnicodeSection();
protected:
	CListBox		m_lstFontInf;
	CHYFontCodec*	m_pFontDeCodec;
};
