#pragma once


// CFontDumpDlg 对话框

class CFontDumpDlg : public CDialog
{
	DECLARE_DYNAMIC(CFontDumpDlg)

public:
	CFontDumpDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CFontDumpDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DMP_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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
