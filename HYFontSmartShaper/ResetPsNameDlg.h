#pragma once


// CResetPsNameDlg 对话框
class CResetPsNameDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CResetPsNameDlg)

public:
	CResetPsNameDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CResetPsNameDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RSTNM_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedRstnmSrcfntBtn();
	afx_msg void OnBnClickedRstnmNameBtn();
	afx_msg void OnBnClickedRstnmDstfntBtn();

	CString m_strSrcFnt;
	CString m_strNameFile;
	CString m_strDstFile;
	afx_msg void OnBnClickedRstnmPrsBtn();
};
