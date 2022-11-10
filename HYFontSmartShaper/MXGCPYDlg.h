#pragma once


// CMXGCPYDlg dialog

class CMXGCPYDlg : public CDialog
{
	DECLARE_DYNAMIC(CMXGCPYDlg)

public:
	CMXGCPYDlg(CWnd* pParent = NULL);   // standard constructor
	CMXGCPYDlg(HYFONTCODEC::CHYFontCodec* fontCodec,CWnd* pParent = NULL);
	virtual ~CMXGCPYDlg();

// Dialog Data
	enum { IDD = IDD_MX_GCPY_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();

public:
	HYFONTCODEC::CHYFontCodec* m_pCodec;
	

	CThumFontWnd*	m_pBThumFontWnd;
	CThumFontWnd*	m_pEThumFontWnd;

	void		InitThumWnd();
	void		UnInitThumWnd();
	UINT m_iBGID;
	UINT m_iEGID;
	afx_msg void OnEnChangeMxBgidEdt();
	afx_msg void OnEnChangeMxEgidEdt();
	afx_msg void OnBnClickedOk();
};
