#pragma once


// CVFntSetUpDlg 对话框

class CVFntSetUpDlg : public CDialog
{
	DECLARE_DYNAMIC(CVFntSetUpDlg)

public:
	CVFntSetUpDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CVFntSetUpDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VFNT_AXIS_DLG };
#endif	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
