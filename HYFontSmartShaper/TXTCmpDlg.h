#pragma once


// CTXTCmpDlg 对话框

class CTXTCmpDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTXTCmpDlg)

public:
	CTXTCmpDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CTXTCmpDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TXTCMP_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedTcmpfl1Btn();
	afx_msg void OnBnClickedTcmpfl2Btn();
	afx_msg void OnBnClickedTcmpdstBtn();

	void	CmprCodeFile(CString strFL1, CString strFL2, std::vector<string>& vtDiff, std::vector<string>& vtSame);
	void	CmprCharFile(CString strFL1, CString strFL2, std::vector<string>& vtDiff, std::vector<string>& vtSame);

public:
	CString m_strFile1;
	CString m_strFile2;
};
