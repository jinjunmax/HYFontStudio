#pragma once
#include "afxcmn.h"


// CMatrixCodeDlg �Ի���

class CMatrixCodeDlg : public CDialog
{
	DECLARE_DYNAMIC(CMatrixCodeDlg)

public:
	CMatrixCodeDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CMatrixCodeDlg();

	void SetMatrixCode(CStringArray& szMatrixCode);

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RCCODE_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CRichEditCtrl m_rdcCodeInfo;
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
};
