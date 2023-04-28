#pragma once


// CExchgCodeDlg 对话框

class CExchgCodeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CExchgCodeDlg)

public:
	CExchgCodeDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CExchgCodeDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EXCHGCDE_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedExchgcdeFntBtn();
	afx_msg void OnBnClickedExchgcdeCdeBtn();
	afx_msg void OnBnClickedExchgcdeSvfntBtn();
	afx_msg void OnBnClickedExchgcdeRunBtn();

	CString m_strSrcFontFile;
	CString m_strCodeFile;
	CString m_strDstFontFile;
protected:
	void ReadFont(CString strInFntFile, CHYFontCodec& Decode);
	void SaveFont(CString strSaveFntFile, CHYFontCodec& Encode, TableDataS& mulpTableData);
	void UpdateCode(CString strSrcFontName, CString strDstFontName, CString strCodeFile);
	void DelCode(CString strSrcFontName, CString strDstFontName, CString strCodeFile);
	void ExChangeCode(CString strSrcFontName, CString strDstFontName, CString strCodeFile);
	void LoadExchgCodeFile(CString strSrcFontName, std::vector<unsigned long>& vtUnicode1, std::vector<unsigned long>& vtUnicode2);
};