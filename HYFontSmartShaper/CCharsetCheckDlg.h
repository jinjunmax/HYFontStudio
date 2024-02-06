#pragma once

class CHYCharset
{
public:
	CHYCharset() {};
	~CHYCharset() {};
public:	
	std::vector<unsigned long> m_vtCodes;
	std::string m_strName;	// 字符集名称
};

// Unicode标准
class UnicodeStandard
{
public:
	UnicodeStandard() {};
	~UnicodeStandard() {};
public:
	std::vector<CHYCharset> m_vtCharset;	// 字符集
};

// GB18030_2022标准 unicode编码
class GB18030_2022_UNI
{
public:
	GB18030_2022_UNI();
	~GB18030_2022_UNI();

public:
	// 国标字符集，unicode编码
	std::vector<CHYCharset> m_vtGB2022;
public:
	// 获取级别一的所有编码（汉字部分）
	void GetLeve1(CHYCharset& GBLevel);
	// 获取级别二的所有编码（汉字部分）
	void GetLeve2(CHYCharset& GBLevel);
	// 获取级别三所有编码（汉字部分）
	void GetLeve3(CHYCharset& GBLevel);
	// 获取GB18030-2022西文和符号部分的编码（GB单字节和双字节编码区部分）
	void GetSymbols(CHYCharset& GBSymbol);
	// 获取级别二相对于级别一的增量编码（汉字部分）
	void GetLeve2Addition(std::vector<unsigned long>& vtCodes);
	// 获取级别三相对于级别二的增量编码（汉字部分）
	void GetLeve3Addition(std::vector<unsigned long>& vtCodes);
	// 获取GB18030-2022所有汉字编码
	void GetAllCode(std::vector<unsigned long>& vtCodes);	
};

// CCharsetCheckDlg 对话框
class CCharsetCheckDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCharsetCheckDlg)

public:
	CCharsetCheckDlg(CWnd* pParent = nullptr);
	CCharsetCheckDlg(CHYFontCodec* pInFntCdc, CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CCharsetCheckDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHKCHARSET_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();


protected:
	void	InitCtrl();
	void	UpdateCtrl();
	void	UpdataListbox();
	void	CheckCharset(std::vector<unsigned long>& vtInCodes, std::vector<unsigned long>& vtOutCodes);	

protected:
	GB18030_2022_UNI	m_GB2022;
	UnicodeStandard		m_UniStandard;
	CHYFontCodec*		m_phyFntCdc;
	std::vector<unsigned long> m_vtMissCodes;

	CListBox m_lstbxCheckView;
	CComboBox m_cmbClass01;
	CComboBox m_cmbClass02;

public:
	afx_msg void OnCbnSelchangeCcstCmbLst01();
	afx_msg void OnCbnSelchangeCcstCmbLst02();
	afx_msg void OnBnClickedCcstChkBtn();	
	afx_msg void OnBnClickedCcstExptBtn();
};
