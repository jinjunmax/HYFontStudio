#pragma once
#include "afxcmn.h"


// CMakePrgrssDlg �Ի���

class CMakePrgrssDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMakePrgrssDlg)

public:
	CMakePrgrssDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CMakePrgrssDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PRGRESS_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT OnMainPrsView(WPARAM waram, LPARAM lparam);
	afx_msg LRESULT OnSubPrsView(WPARAM waram, LPARAM lparam);

public:
	CProgressCtrl m_MainPrgrss;
	CProgressCtrl m_SubPrgrss;
	afx_msg void OnPaint();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
