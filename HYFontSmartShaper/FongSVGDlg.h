#pragma once


// CFongSVGDlg dialog

class CFongSVGDlg : public CDialog
{
	DECLARE_DYNAMIC(CFongSVGDlg)

public:
	CFongSVGDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFongSVGDlg();

// Dialog Data
	enum { IDD = IDD_FNT_SVG_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSvgOpenfntBtn();
	afx_msg void OnBnClickedSvgSavesvgBtn();
	CString m_strFontFile;
	CString m_strSVGFile;
};
