#pragma once


// CFontDumpDlg �Ի���

class CFontDumpDlg : public CDialog
{
	DECLARE_DYNAMIC(CFontDumpDlg)

public:
	CFontDumpDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CFontDumpDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DMP_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedDmpFilenmBtn();
	afx_msg void OnTableCheck(UINT uID);

public:
	std::vector<unsigned long> m_vtTBFlag;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedDmpDumpBtn();
	CString m_strFileName;
};
