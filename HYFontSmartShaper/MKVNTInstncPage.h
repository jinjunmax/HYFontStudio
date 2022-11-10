#pragma once


// CMKVNTInstncPage 对话框

class CMKVNTInstncPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CMKVNTInstncPage)

public:
	CMKVNTInstncPage();
	virtual ~CMKVNTInstncPage();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FNT_MKVNT_INSTNC_PAGE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedMkvInstncAddBtn();
	afx_msg void OnBnClickedMkvInstncDelBtn();
	afx_msg void OnBnClickedMkvInstncUpBtn();
	afx_msg void OnBnClickedMkvInstncDwnBtn();
	virtual BOOL OnInitDialog();

	void	InitControl();
	void	UninitControl();
	void	UpdateCtrl();
	void	UpdateListCtrl();
	int		GetListCtrlSelectItem();
	void	DefaultInstanceCtrl();

protected:
	CString			m_strStyleName;
	int				m_iWeight;
	int				m_iWidth;
	int				m_iAxWeight;
	int				m_iAxWidth;
	int				m_iAxHeight;
	CListCtrl		m_lstInstance;
	CVrInstance		m_Instance;

public:
	afx_msg void OnBnClickedMkvInstncMdfyBtn();
	afx_msg void OnBnClickedMkvInstncFmmasterBtn();
	afx_msg void OnLvnItemchangedMkvInstncInstncLst(NMHDR *pNMHDR, LRESULT *pResult);
};
