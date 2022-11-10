#pragma once
#include "afxcmn.h"


// CMatrixCodeDlg 对话框

class CMatrixCodeDlg : public CDialog
{
	DECLARE_DYNAMIC(CMatrixCodeDlg)

public:
	CMatrixCodeDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMatrixCodeDlg();

	void SetMatrixCode(CStringArray& szMatrixCode);

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RCCODE_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CRichEditCtrl m_rdcCodeInfo;
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
};
