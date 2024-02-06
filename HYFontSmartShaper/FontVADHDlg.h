#pragma once


// CFontVADHDlg 对话框

class CFontVADHDlg : public CDialog
{
	DECLARE_DYNAMIC(CFontVADHDlg)

public:
	CFontVADHDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CFontVADHDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FNT_VRADH_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedVadhSfntBtn();	
	afx_msg void OnBnClickedVadhBudlBtn();
	afx_msg void OnBnClickedVadhPsnmChk();	
	afx_msg void OnBnClickedVadhPsnmBtn();
public:
	virtual BOOL OnInitDialog();
	void	SetVertHeight(HYFONTCODEC::CHYFontCodec& HYFontCode);
	void	ChangeVertHeight(HYFONTCODEC::CHYFontCodec& HYFontCode,short Tsb);
	void	ChangeVertHeight(HYFONTCODEC::CHYFontCodec& HYFontCode,std::vector<std::string> vtPsName, short Tsb);

protected:
	UINT		m_uiTsb;
	CString		m_strSrcFntFile;
	CString		m_strPSNameFile;
	
};
