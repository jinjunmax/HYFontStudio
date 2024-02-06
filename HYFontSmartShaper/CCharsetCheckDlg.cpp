// CCharsetCheckDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "HYFontSmartShaper.h"
#include "afxdialogex.h"
#include "CCharsetCheckDlg.h"


GB18030_2022_UNI::GB18030_2022_UNI()
{


}	//end of GB18030_2022_UNI::GB18030_2022_UNI()

GB18030_2022_UNI::~GB18030_2022_UNI()
{


}	//end of GB18030_2022_UNI::~GB18030_2022_UNI()

// 获取级别一的所有编码（汉字部分）
void GB18030_2022_UNI::GetLeve1(CHYCharset& GBLevel)
{
	for each (CHYCharset chrst in m_vtGB2022)
	{
		CString strRc;
		strRc.LoadString(IDS_CHARSET_GBL1);
		if (chrst.m_strName == string(strRc))
		{
			GBLevel = chrst;
			return;
		}
	}

}	// end of void GB18030_2022_UNI::GetLeve1()

// 获取级别二的所有编码（汉字部分）
void GB18030_2022_UNI::GetLeve2(CHYCharset& GBLevel)
{
	for each (CHYCharset chrst in m_vtGB2022)
	{
		CString strRc;
		strRc.LoadString(IDS_CHARSET_GBL2);
		if (chrst.m_strName == string(strRc))
		{
			GBLevel = chrst;
			return;
		}
	}

}	//end of void GB18030_2022_UNI::GetLeve2()

// 获取级别三所有编码（汉字部分）
void GB18030_2022_UNI::GetLeve3(CHYCharset& GBLevel)
{
	for each (CHYCharset chrst in m_vtGB2022)
	{
		CString strRc;
		strRc.LoadString(IDS_CHARSET_GBL3);
		if (chrst.m_strName == string(strRc))
		{
			GBLevel = chrst;
			return;
		}
	}

}	//end of void GB18030_2022_UNI::GetLeve3()

// 获取GB18030-2022西文和符号部分的编码（GB单字节和双字节编码区部分）
void GB18030_2022_UNI::GetSymbols(CHYCharset& GBSymbol)
{
	for each (CHYCharset chrst in m_vtGB2022)
	{
		CString strRc;
		strRc.LoadString(IDS_CHARSET_GBSYMB);
		if (chrst.m_strName == string(strRc))
		{
			GBSymbol = chrst;
			return;
		}
	}

}	// end of void GB18030_2022_UNI::GetSymbols()

// 获取级别二相对于级别一的增量编码（汉字部分）
void GB18030_2022_UNI::GetLeve2Addition(std::vector<unsigned long>& vtCodes)
{
	vtCodes.clear();

	CHYCharset GBLevel1, GBLevel2;
	GetLeve1(GBLevel1);
	GetLeve2(GBLevel2);

	for each (unsigned long x in GBLevel2.m_vtCodes)
	{
		bool b = false;
		for each (unsigned long y in GBLevel1.m_vtCodes)
		{
			if (x == y) {
				b = true;
			}
		}

		if (!b) {
			vtCodes.push_back(x);
		}
	}

}	//end of void GB18030_2022_UNI::GetLeve2Addition()

// 获取级别三相对于级别二的增量编码（汉字部分）
void GB18030_2022_UNI::GetLeve3Addition(std::vector<unsigned long>& vtCodes)
{
	vtCodes.clear();

	CHYCharset GBLevel2, GBLevel3;
	GetLeve1(GBLevel2);
	GetLeve2(GBLevel3);

	for each (unsigned long x in GBLevel3.m_vtCodes)
	{
		bool b = false;
		for each (unsigned long y in GBLevel2.m_vtCodes)
		{
			if (x == y) {
				b = true;
			}
		}

		if (!b) {
			vtCodes.push_back(x);
		}
	}

}	//end of void GB18030_2022_UNI::GetLeve3Addition()

void GB18030_2022_UNI::GetAllCode(std::vector<unsigned long>& vtCodes)
{
	vtCodes.clear();

	CHYCharset symbol;
	GetSymbols(symbol);
	for each (unsigned long l in symbol.m_vtCodes){
		vtCodes.push_back(l);
	}

	CHYCharset l3;
	GetLeve1(l3);
	for each (unsigned long l in l3.m_vtCodes)	{
		vtCodes.push_back(l);
	}

}	//end of void GB18030_2022_UNI::GetAllCode()
	
///////////////////////////////////////////////////////////////////////////////////////////
// CCharsetCheckDlg 对话框
IMPLEMENT_DYNAMIC(CCharsetCheckDlg, CDialogEx)
CCharsetCheckDlg::CCharsetCheckDlg(CWnd* pParent /*= nullptr*/) 
	: CDialogEx(IDD_CHKCHARSET_DLG, pParent)
{
	m_phyFntCdc = NULL;

}	// end of CCharsetCheckDlg::CCharsetCheckDlg()

CCharsetCheckDlg::CCharsetCheckDlg(CHYFontCodec* pInFntCdc, CWnd* pParent /*= nullptr*/)
	: CDialogEx(IDD_CHKCHARSET_DLG, pParent)
{
	m_phyFntCdc = pInFntCdc;

	string strcode = ::XSysproxy().m_strAppDir + _T("data\\Charset\\GB18030-2022-Lv1.txt");
	CString str;
	if (str.LoadString(IDS_CHARSET_GBL1)) {
		CHYCharset charset;
		charset.m_strName = str;
		::HY_LoadCodeFile((char*)strcode.c_str(), charset.m_vtCodes);
		m_GB2022.m_vtGB2022.push_back(charset);
	}

	strcode = ::XSysproxy().m_strAppDir + _T("data\\Charset\\GB18030-2022-Lv2.txt");
	if (str.LoadString(IDS_CHARSET_GBL2)) {
		CHYCharset charset;
		charset.m_strName = str;
		::HY_LoadCodeFile((char*)strcode.c_str(), charset.m_vtCodes);
		m_GB2022.m_vtGB2022.push_back(charset);
	}

	strcode = ::XSysproxy().m_strAppDir + _T("data\\Charset\\GB18030-2022-Lv3.txt");
	if (str.LoadString(IDS_CHARSET_GBL3)) {
		CHYCharset charset;
		charset.m_strName = str;
		::HY_LoadCodeFile((char*)strcode.c_str(), charset.m_vtCodes);
		m_GB2022.m_vtGB2022.push_back(charset);
	}

	strcode = ::XSysproxy().m_strAppDir + _T("data\\Charset\\GB18030-2022-Symbol.txt");
	if (str.LoadString(IDS_CHARSET_GBSYMB)) {
		CHYCharset charset;
		charset.m_strName = str;
		::HY_LoadCodeFile((char*)strcode.c_str(), charset.m_vtCodes);
		m_GB2022.m_vtGB2022.push_back(charset);
	}

	strcode = ::XSysproxy().m_strAppDir + _T("data\\Charset\\CJK统一汉字基本集.txt");
	if (str.LoadString(IDS_CHARSET_UniCJK)) {
		CHYCharset charset;
		charset.m_strName = str;
		::HY_LoadCodeFile((char*)strcode.c_str(), charset.m_vtCodes);
		m_UniStandard.m_vtCharset.push_back(charset);
	}

	strcode = ::XSysproxy().m_strAppDir + _T("data\\Charset\\CJK统一汉字扩充A.txt");
	if (str.LoadString(IDS_CHARSET_UniCJKA)) {
		CHYCharset charset;
		charset.m_strName = str;
		::HY_LoadCodeFile((char*)strcode.c_str(), charset.m_vtCodes);
		m_UniStandard.m_vtCharset.push_back(charset);
	}

	strcode = ::XSysproxy().m_strAppDir + _T("data\\Charset\\CJK统一汉字扩充B.txt");
	if (str.LoadString(IDS_CHARSET_UniCJKB)) {
		CHYCharset charset;
		charset.m_strName = str;
		::HY_LoadCodeFile((char*)strcode.c_str(), charset.m_vtCodes);
		m_UniStandard.m_vtCharset.push_back(charset);
	}

	strcode = ::XSysproxy().m_strAppDir + _T("data\\Charset\\CJK统一汉字扩充C.txt");
	if (str.LoadString(IDS_CHARSET_UniCJKC)) {
		CHYCharset charset;
		charset.m_strName = str;
		::HY_LoadCodeFile((char*)strcode.c_str(), charset.m_vtCodes);
		m_UniStandard.m_vtCharset.push_back(charset);
	}

	strcode = ::XSysproxy().m_strAppDir + _T("data\\Charset\\CJK统一汉字扩充D.txt");
	if (str.LoadString(IDS_CHARSET_UniCJKD)) {
		CHYCharset charset;
		charset.m_strName = str;
		::HY_LoadCodeFile((char*)strcode.c_str(), charset.m_vtCodes);
		m_UniStandard.m_vtCharset.push_back(charset);
	}

	strcode = ::XSysproxy().m_strAppDir + _T("data\\Charset\\CJK统一汉字扩充E.txt");
	if (str.LoadString(IDS_CHARSET_UniCJKE)) {
		CHYCharset charset;
		charset.m_strName = str;
		::HY_LoadCodeFile((char*)strcode.c_str(), charset.m_vtCodes);
		m_UniStandard.m_vtCharset.push_back(charset);
	}

	strcode = ::XSysproxy().m_strAppDir + _T("data\\Charset\\CJK统一汉字扩充F.txt");
	if (str.LoadString(IDS_CHARSET_UniCJKF)) {
		CHYCharset charset;
		charset.m_strName = str;
		::HY_LoadCodeFile((char*)strcode.c_str(), charset.m_vtCodes);
		m_UniStandard.m_vtCharset.push_back(charset);
	}

}	//end of CCharsetCheckDlg::CCharsetCheckDlg()

CCharsetCheckDlg::~CCharsetCheckDlg()
{

}	//end of CCharsetCheckDlg::~CCharsetCheckDlg()

void CCharsetCheckDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);	
	DDX_Control(pDX, IDC_CCST_LST_CHKVEW, m_lstbxCheckView);
	DDX_Control(pDX, IDC_CCST_CMB_LST01, m_cmbClass01);
	DDX_Control(pDX, IDC_CCST_CMB_LST02, m_cmbClass02);	
}	// end of void CCharsetCheckDlg::DoDataExchange()


BEGIN_MESSAGE_MAP(CCharsetCheckDlg, CDialogEx)
	ON_CBN_SELCHANGE(IDC_CCST_CMB_LST01, &CCharsetCheckDlg::OnCbnSelchangeCcstCmbLst01)
	ON_CBN_SELCHANGE(IDC_CCST_CMB_LST02, &CCharsetCheckDlg::OnCbnSelchangeCcstCmbLst02)
	ON_BN_CLICKED(IDC_CCST_CHK_BTN, &CCharsetCheckDlg::OnBnClickedCcstChkBtn)
	ON_BN_CLICKED(IDC_CCST_EXPT_BTN, &CCharsetCheckDlg::OnBnClickedCcstExptBtn)
END_MESSAGE_MAP()

// CCharsetCheckDlg 消息处理程序
BOOL CCharsetCheckDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	InitCtrl();

	return TRUE;  // return TRUE unless you set the focus to a control	

}	//end of BOOL CCharsetCheckDlg::OnInitDialog()

void CCharsetCheckDlg::InitCtrl()
{
	CString str;
	str.LoadString(IDS_CHARSET_GB);
	m_cmbClass01.AddString(str);
	str.LoadString(IDS_CHARSET_UNI);
	m_cmbClass01.AddString(str);
	m_cmbClass01.SetCurSel(0);

	for (size_t i = 0; i < m_GB2022.m_vtGB2022.size(); i++) {		
		m_cmbClass02.AddString(m_GB2022.m_vtGB2022[i].m_strName.c_str());
	}
	if (m_cmbClass02.GetCount() > 0) {
		m_cmbClass02.SetCurSel(0);
	}

}	//end of void CCharsetCheckDlg::InitCtrl()

void CCharsetCheckDlg::UpdateCtrl()
{
	int iSel = m_cmbClass01.GetCurSel();
	m_cmbClass02.ResetContent();	
	if (iSel == 0) {		
		for (size_t i = 0; i < m_GB2022.m_vtGB2022.size(); i++)	{
			m_cmbClass02.AddString(m_GB2022.m_vtGB2022[i].m_strName.c_str());
		}	
	}
	if (iSel == 1) {
		for (size_t i = 0; i < m_UniStandard.m_vtCharset.size(); i++) {
			m_cmbClass02.AddString(m_UniStandard.m_vtCharset[i].m_strName.c_str());
		}	
	}
	if (m_cmbClass02.GetCount() > 0) {
		m_cmbClass02.SetCurSel(0);		
	}	

}	//end of void CCharsetCheckDlg::UpdataCtrl()

void CCharsetCheckDlg::UpdataListbox()
{
	m_lstbxCheckView.ResetContent();

	size_t st = m_vtMissCodes.size();
	for (size_t i = 0; i < st; i++) {
		string strInf = ::HY_Int_to_Hexstring(m_vtMissCodes[i]);
		m_lstbxCheckView.AddString(strInf.c_str());
	}

}	// end of void CCharsetCheckDlg::UpdataListbox()

void CCharsetCheckDlg::CheckCharset(std::vector<unsigned long>& vtInCodes, std::vector<unsigned long>& vtOutCodes)
{
	vtOutCodes.clear();

	if (m_phyFntCdc) {
		for (size_t x = 0; x < vtInCodes.size(); x++) {
			bool b = false;
			unsigned long ulCode1 = vtInCodes[x];
			if (m_phyFntCdc->FindGryphIndexByUnciodeEx(ulCode1) == -1) {
				vtOutCodes.push_back(ulCode1);
			}
		}
	}

}	//end of void CCharsetCheckDlg::CheckCharset()

void CCharsetCheckDlg::OnCbnSelchangeCcstCmbLst01()
{	
	UpdateCtrl();

}	//end of void CCharsetCheckDlg::OnCbnSelchangeCcstCmbLst01()

void CCharsetCheckDlg::OnCbnSelchangeCcstCmbLst02()
{	
	
}	//end of void CCharsetCheckDlg::OnCbnSelchangeCcstCmbLst02()

void CCharsetCheckDlg::OnBnClickedCcstChkBtn()
{
	m_vtMissCodes.clear();
	if (m_cmbClass01.GetCurSel() == 0) {
		int isel = m_cmbClass02.GetCurSel();
		if (isel != -1) {			
			CheckCharset(m_GB2022.m_vtGB2022[isel].m_vtCodes, m_vtMissCodes);
		}
	}

	if (m_cmbClass02.GetCurSel() == 0) {
		int isel = m_cmbClass02.GetCurSel();
		if (isel != -1) {
			CheckCharset(m_UniStandard.m_vtCharset[isel].m_vtCodes, m_vtMissCodes);
		}
	}

}	// end of void CCharsetCheckDlg::OnBnClickedCcstChkBtn()

void CCharsetCheckDlg::OnBnClickedCcstExptBtn()
{
	if (m_vtMissCodes.size() == 0) return;

	TCHAR	szFilters[] = _T("Text File(*.txt)|*.txt||");
	CFileDialog  saveFileDlg(TRUE, _T(""), _T(""), OFN_LONGNAMES, szFilters);
	if (saveFileDlg.DoModal() != IDOK)	return;

	CString strSrcFontFile = saveFileDlg.GetPathName();

	if (::HY_FileExist((LPSTR)(LPCTSTR)strSrcFontFile)) {
		::DeleteFile((LPCSTR)strSrcFontFile);
	}

	FILE* pWrt = fopen((LPSTR)(LPCTSTR)strSrcFontFile, "w+t");
	if (pWrt) {
		for each (unsigned long l in m_vtMissCodes)	{
			string strInf = ::HY_Int_to_Hexstring(l);
			strInf += "\n";
			fwrite(strInf.c_str(),1, strInf.length(), pWrt);
		} 
		fflush(pWrt);
		fclose(pWrt);
	}

}	// end of void CCharsetCheckDlg::OnBnClickedCcstExptBtn()
