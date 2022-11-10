#pragma once

#include "ImageGridSetWnd.h"
#include "MatrixCodeDlg.h"

// CHYGridParamDlg 对话框

class CHYGridParamDlg : public CDialog
{
	DECLARE_DYNAMIC(CHYGridParamDlg)

public:
	CHYGridParamDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CHYGridParamDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GRIDPARAM_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg	LRESULT	OnTraceRect(WPARAM waram, LPARAM lparam);

	CImageGridSetWnd*	m_pImageGridSetWnd;
	CString				m_strImage;
	CMatrixCodeDlg		m_mtrxDlg;

	int			m_iMaxArea;
	int			m_iMinArea;
	int			m_iMaxAngle;
	int			m_iMinAngle;

	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedTestBtn();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancle();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedBtnChkrct();
	afx_msg void OnBnClickedRcinfBtn();
	CString m_strMtxCode;
	afx_msg void OnBnClickedBtnChktest();
	int Leftedge;
	int Topedge;
	int Rightedge;
	int Btmedge;
	int m_iColum;
	int m_iRow;
	int m_iGrayThres;
};
