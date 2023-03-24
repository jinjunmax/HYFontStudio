#pragma once


// CParamSetupDlg 对话框

class CParamSetupDlg : public CDialog
{
	DECLARE_DYNAMIC(CParamSetupDlg)

public:
	CParamSetupDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CParamSetupDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PRM_SETUP_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	int m_iTopEdge;
	int m_iLeftEdge;
	int m_iBottomEdge;
	int m_iRightEdge;
	int m_iRows;
	int m_icolomns;
	int m_iGrayThre;
	int m_iAvrdis;
	int m_iDisWidth;
	int m_iSingleBorder;
	int m_iGridThres;
	int m_iGridLenThres;
	CString m_strGridImgNumber;
	CString m_strGridImgDir;
	int m_ismthPrm;	// 图像平滑参数
};
