#pragma once
#include "afxwin.h"


// CCharsCompareDlg 对话框

class CCharsCompareDlg : public CDialog
{
	DECLARE_DYNAMIC(CCharsCompareDlg)

public:
	CCharsCompareDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCharsCompareDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = ID_CHARS_COMPARE_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCharsCompareAddLst();
	afx_msg void OnBnClickedCharsCompareDelLst();
	afx_msg void OnBnClickedCharsCompareMkLst();

protected:
	int	CompareChar(CHYGlyph &bsglyph, CHYFontCodec& FntCdc);
	CListBox			m_lstFnts;
	std::vector<string>	m_vtFntName;
};
