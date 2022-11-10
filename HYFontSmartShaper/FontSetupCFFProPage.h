#pragma once


// CFontSetupCFFProPage dialog

class CFontSetupCFFProPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CFontSetupCFFProPage)

public:
	CFontSetupCFFProPage();
	virtual ~CFontSetupCFFProPage();

// Dialog Data
	enum { IDD = IDD_FNT_SETUP_CFF_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();

public:
	afx_msg void OnBnClickedTscIscidyRad();
	afx_msg void OnBnClickedTscIscidnRad();
	afx_msg void OnEnChangeTscRgstEdit();
	afx_msg void OnEnChangeTscOrderEdit();
	afx_msg void OnEnChangeTscSuppleEdit();

protected:
	void	InitCtrl();
	void	UninitCtrl();

protected:	
	std::string					m_strRegistory;
	std::string					m_strOrder;
	std::string					m_strSupple;
	int							m_iSupple;
public:
	void Save();
};
