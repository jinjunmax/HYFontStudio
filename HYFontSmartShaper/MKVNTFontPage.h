#pragma once


// CMKVNTFontPage �Ի���

class CMKVNTFontPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CMKVNTFontPage)

public:
	CMKVNTFontPage();
	virtual ~CMKVNTFontPage();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FNT_MKVNT_FNT_PAGE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedMkvFntMknameEdt();
	virtual BOOL OnInitDialog();
protected:
	CString		m_strENGFamilyName;
	CString		m_strCHSFamilyName;
	CString		m_strPostScriptName;
	CString		m_strENGCopyright;
	CString		m_strCHSCopyright;
	CString     m_strVersion;
};
