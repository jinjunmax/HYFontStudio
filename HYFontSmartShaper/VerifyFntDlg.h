#pragma once


// CVerifyFntDlg 对话框

class CVerifyFntDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CVerifyFntDlg)

public:
	CVerifyFntDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CVerifyFntDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VERIFYFNT_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
