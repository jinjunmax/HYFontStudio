#pragma once
#include "afxcmn.h"


// CMonoImgDlg 对话框

class CMonoImgDlg : public CDialog
{
	DECLARE_DYNAMIC(CMonoImgDlg)

public:
	CMonoImgDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMonoImgDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MONO_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedSrcpathBtn();	
	afx_msg void OnBnClickedDstpathBtn();	
	afx_msg void OnBnClickedCvntBtn();

public:
	UINT			m_unMonoWidth;
	CProgressCtrl	m_prgsConvert;
	CString			m_strSrcPath;
	CString			m_strDstPath;
	
	std::vector<string>	vtSrcFile;
};
