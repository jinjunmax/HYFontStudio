#pragma once
#include "afxwin.h"


// CCharsCompareDlg �Ի���

class CCharsCompareDlg : public CDialog
{
	DECLARE_DYNAMIC(CCharsCompareDlg)

public:
	CCharsCompareDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCharsCompareDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = ID_CHARS_COMPARE_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
