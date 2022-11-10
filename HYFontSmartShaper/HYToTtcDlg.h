#pragma once


// CHYToTtcDlg dialog

class CHYToTtcDlg : public CDialog
{
	DECLARE_DYNAMIC(CHYToTtcDlg)

public:
	CHYToTtcDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CHYToTtcDlg();

// Dialog Data
	enum { IDD = IDD_FNT_TOTTC_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
