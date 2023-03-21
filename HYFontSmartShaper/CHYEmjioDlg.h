#pragma once


// CHYEmjioDlg 对话框
class CHYEmjioDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CHYEmjioDlg)

public:
	CHYEmjioDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CHYEmjioDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EMJ_DLG };
#endif

protected:
	void	XmlToHead(CMarkup& mkEmoji, CHYFontCodec& FntEncode);
	void	XmlToHhea(CMarkup& mkEmoji, CHYFontCodec& FntEncode);
	void	XmlTomaxp(CMarkup& mkEmoji, CHYFontCodec& FntEncode);
	void	XmlToOS(CMarkup& mkEmoji, CHYFontCodec& FntEncode);
	void	XmlToNoImageChar(CMarkup& mkEmoji, CHYFontCodec& FontEnCodec);
	void	XmlToEmoji(CMarkup& mkEmoji, CHYFontCodec& FntEncode);
	void	SetTables(CHYFontCodec& FntEncode);
	void	loadCodeFile(CString strFile,CHYFontCodec& Encode);
	void	loadHMFile(CString strFile, CHYFontCodec& Encode);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedEmjCngfileBtn();
	afx_msg void OnBnClickedEmjCdefileBtn();
	afx_msg void OnBnClickedEmjMkBtn();	
	afx_msg void OnBnClickedEmjHmefileBtn();

	CString m_strCnfgFile;
	CString m_strCodeFile;
	CString m_strHMtricFile;
	CHYFontCodec m_Encode;
};
