#pragma once


// CMergeDlg dialog
class CMergeDlg : public CDialog
{
	DECLARE_DYNAMIC(CMergeDlg)

public:
	CMergeDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMergeDlg();

// Dialog Data
	enum { IDD = IDD_MERGE_DLG };

protected:	
	
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();
public:
	afx_msg void OnBnClickedMrg1Src1Btn();
	afx_msg void OnBnClickedMrg1Src2Btn();
	afx_msg void OnBnClickedMrg1DstBtn();
	afx_msg void OnBnClickedMgr1MergeBtn();	
	afx_msg void OnBnClickedMrg1TxtBtn();	
	afx_msg void OnBnClickedMmrgSrc2txtBtn();
	afx_msg void OnBnClickedMmrgAddBtn();
	afx_msg void OnBnClickedMmrgDelBtn();
	afx_msg void OnBnClickedMmrgUpBtn();
	afx_msg void OnBnClickedMmrgDwnBtn();
	afx_msg void OnBnClickedMmrgMrgBtn();
protected:
	void	IniCtrl();
	void	UninitCtrl();
	void	UpdateListCtrl();
	int		GetListCtrlSelectItem();
	void 	SetListCtrlSelectItem(int iIndex);

	void	GetSubset(std::vector<CHYGlyph>& InGlyphs, std::vector<CHYGlyph>& outHYGlyphs, string strsbsFile);
	void	MergeFont(string strDst, std::vector<string> vtFntName);
	
public:
	CString				m_strSrc1;
	CString				m_strSrc2;
	CString				m_strDst;
	CString				m_strTxt;
	CListCtrl			m_lsCtrlFnts;
	std::vector<string>	m_vtFntName;
	int					m_FontFlag;
	CString				m_strMTxt;
	CProgressCtrl		m_prsMerge;	
};
