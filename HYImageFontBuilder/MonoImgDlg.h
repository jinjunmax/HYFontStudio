#pragma once
#include "afxcmn.h"


// CMonoImgDlg �Ի���

class CMonoImgDlg : public CDialog
{
	DECLARE_DYNAMIC(CMonoImgDlg)

public:
	CMonoImgDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CMonoImgDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MONO_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
