#pragma once


// CVerifyFntDlg �Ի���

class CVerifyFntDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CVerifyFntDlg)

public:
	CVerifyFntDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CVerifyFntDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VERIFYFNT_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
