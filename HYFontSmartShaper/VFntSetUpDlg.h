#pragma once


// CVFntSetUpDlg �Ի���

class CVFntSetUpDlg : public CDialog
{
	DECLARE_DYNAMIC(CVFntSetUpDlg)

public:
	CVFntSetUpDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CVFntSetUpDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VFNT_AXIS_DLG };
#endif	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
