#pragma once


// CFontVADHDlg �Ի���

class CFontVADHDlg : public CDialog
{
	DECLARE_DYNAMIC(CFontVADHDlg)

public:
	CFontVADHDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CFontVADHDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FNT_VRADH_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedVadhSfntBtn();	
	afx_msg void OnBnClickedVadhBudlBtn();
	afx_msg void OnBnClickedVadhPsnmChk();	
	afx_msg void OnBnClickedVadhPsnmBtn();
public:
	virtual BOOL OnInitDialog();
protected:
	UINT		m_uiTsb;
	CString		m_strSrcFntFile;
	CString		m_strPSNameFile;
	
};
