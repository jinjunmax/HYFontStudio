#pragma once


// CMKVNTMasterPage 对话框
class CMKVNTMasterPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CMKVNTMasterPage)

public:
	CMKVNTMasterPage();
	virtual ~CMKVNTMasterPage();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CMKVNTMasterPage };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedMkvMstOpnfileBtn();
	afx_msg void OnBnClickedMkvMstAddBtn();
	afx_msg void OnBnClickedMkvMstDelBtn();
	afx_msg void OnBnClickedMkvMstUpBtn();
	afx_msg void OnBnClickedMkvMstDwnBtn();
	afx_msg void OnBnClickedMkvMstMdBtn();

protected:
	void	InitControl();
	void	UninitControl();
	void	UpdateCtrl();
	void	UpdateListCtrl();
	int		GetListCtrlSelectItem();
	void	DefaultMastCtrl();
protected:
	CListCtrl				m_lstMaster;
	CString					m_strMName;
	CString					m_strFontName;
	CString					m_strFileName;
	int						m_iWeight;
	int						m_iWidth;
	int						m_iHeight;
	int						m_iAscender;
	int						m_iDescender;
	int						m_iTypoAscender;
	int						m_iTypoDescender;
	BOOL					m_bDefault;	
	CVrMaster				m_Master;
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);	
	afx_msg void OnNMSetfocusMkvMstMstLst(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickMkvMstMstLst(NMHDR *pNMHDR, LRESULT *pResult);
};
