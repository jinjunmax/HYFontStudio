// FontVADHDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "HYFontSmartShaper.h"
#include "FontVADHDlg.h"
#include "afxdialogex.h"


// CFontVADHDlg 对话框
IMPLEMENT_DYNAMIC(CFontVADHDlg, CDialog)

CFontVADHDlg::CFontVADHDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_FNT_VRADH_DLG, pParent)	
	, m_uiTsb(0)
	, m_strSrcFntFile(_T(""))
	, m_strPSNameFile(_T(""))
{

}	// end of CFontVADHDlg::CFontVADHDlg()

CFontVADHDlg::~CFontVADHDlg()
{
}	// end of CFontVADHDlg::~CFontVADHDlg()

void CFontVADHDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_VADH_TSB_EDT, m_uiTsb);

	DDX_Text(pDX, IDC_VADH_SFNT_EDT, m_strSrcFntFile);
	DDX_Text(pDX, IDC_VADH_PSNM_EDT, m_strPSNameFile);
}	// end of void CFontVADHDlg::DoDataExchange()

BEGIN_MESSAGE_MAP(CFontVADHDlg, CDialog)
	ON_BN_CLICKED(IDC_VADH_SFNT_BTN, &CFontVADHDlg::OnBnClickedVadhSfntBtn)	
	ON_BN_CLICKED(IDC_VADH_BUDL_BTN, &CFontVADHDlg::OnBnClickedVadhBudlBtn)
	ON_BN_CLICKED(IDC_VADH_PSNM_CHK, &CFontVADHDlg::OnBnClickedVadhPsnmChk)
	ON_BN_CLICKED(IDC_VADH_PSNM_BTN, &CFontVADHDlg::OnBnClickedVadhPsnmBtn)
END_MESSAGE_MAP()

// CFontVADHDlg 消息处理程序
void CFontVADHDlg::OnBnClickedVadhSfntBtn()
{
	UpdateData();

	TCHAR	szFilters[] = _T("TrueType 文件(*.ttf)|*.ttf||");
	CFileDialog  openFileDlg(TRUE, _T(""), _T(""), OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilters);
	if (openFileDlg.DoModal() != IDOK)	return;

	m_strSrcFntFile = openFileDlg.GetPathName();

	UpdateData(FALSE);
	
}	// end of void CFontVADHDlg::OnBnClickedVadhSfntBtn()

void CFontVADHDlg::OnBnClickedVadhPsnmBtn()
{
	UpdateData();

	TCHAR	szFilters[] = _T("PSName 文件(*.txt)|*.txt||");
	CFileDialog  openFileDlg(TRUE, _T(""), _T(""), OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilters);
	if (openFileDlg.DoModal() != IDOK)	return;

	m_strPSNameFile = openFileDlg.GetPathName();

	UpdateData(FALSE);

}	// end of void CFontVADHDlg::OnBnClickedVadhPsnmBtn()

void CFontVADHDlg::OnBnClickedVadhBudlBtn()
{
	CHAR szFilters[] = _T("true type 文件(*.ttf)|*.ttf||");
	CFileDialog  saveFileDlg(FALSE, _T(""), _T(""), OFN_LONGNAMES, szFilters);
	if (saveFileDlg.DoModal() != IDOK)	return;
	CString strDstFileName = saveFileDlg.GetPathName();

	if (m_strSrcFntFile == "")	{
		AfxMessageBox("源字库为空");
		return;
	}

	UpdateData();
	CWaitCursor	wc;
	HYFONTCODEC::CHYFontCodec		HYFontCode;
	if (HYFontCode.Decode((LPSTR)(LPCSTR)m_strSrcFntFile) != HY_NOERROR)
	{
		AfxMessageBox("字库解析失败！");
		return;
	}
	SetVertHeight(HYFontCode);

	if (IsDlgButtonChecked(IDC_VADH_PSNM_CHK)) {
		if (m_strPSNameFile == "") {
			AfxMessageBox("psname文件名不能为空");
			return;
		}
		std::vector<std::string> vtPsName;
		int Result = ::HY_LoadPsNameFile((LPSTR)(LPCSTR)m_strPSNameFile, vtPsName);
		if (Result != HY_NOERROR) {
			AfxMessageBox("psname文件读取错误");
			return;
		}		
		ChangeVertHeight(HYFontCode, vtPsName,m_uiTsb);
	}
	else {
		ChangeVertHeight(HYFontCode,m_uiTsb);
	}	

	std::vector<unsigned long> vtFlag;
	HYFontCode.m_HYTbDirectory.GetTableFlags(vtFlag);

	::XSysproxy().InitHmtxTb(HYFontCode);
	HYFontCode.MakeHYCodeMap();
	HYFontCode.MakeVerticalMetrics();
	if (HYFontCode.Encode((LPSTR)(LPCSTR)strDstFileName, vtFlag) == HY_NOERROR) {
		AfxMessageBox("处理完成");	
	}
	else {
		AfxMessageBox("处理失败");
	}
}	// end of void CFontVADHDlg::OnBnClickedVadhBudlBtn()

void CFontVADHDlg::SetVertHeight(HYFONTCODEC::CHYFontCodec& HYFontCode)
{		
	if (!HYFontCode.FindFlag(VHEA_TAG) || !HYFontCode.FindFlag(VMTX_TAG)) {
		for (int i = 0; i < HYFontCode.m_HYMaxp.numGlyphs; i++)
		{
			CHYGlyph& glyh = HYFontCode.m_vtHYGlyphs[i];			
			glyh.advanceHeight = HYFontCode.m_HYOS2.sTypoAscender - HYFontCode.m_HYOS2.sTypoDescender;
			glyh.topSB = HYFontCode.m_HYOS2.sTypoAscender - glyh.maxY;
		}
	}

	bool bIsInsertLast = TRUE;
	CHYTableEntry VHEAEntry;
	VHEAEntry.tag = VHEA_TAG;	
	int EntryNum = (int)HYFontCode.m_HYTbDirectory.vtTableEntry.size();
	for (int x = 0; x < EntryNum; x++){
		if (HYFontCode.m_HYTbDirectory.vtTableEntry[x].tag > VHEA_TAG){
			HYFontCode.m_HYTbDirectory.vtTableEntry.insert(HYFontCode.m_HYTbDirectory.vtTableEntry.begin() + x, VHEAEntry);
			bIsInsertLast = FALSE;
		}
	}
	if (bIsInsertLast){
		HYFontCode.m_HYTbDirectory.vtTableEntry.push_back(VHEAEntry);
	}
	bIsInsertLast = TRUE;
	VHEAEntry.tag = VMTX_TAG;
	for (int x = 0; x < EntryNum; x++){
		if (HYFontCode.m_HYTbDirectory.vtTableEntry[x].tag > VMTX_TAG)	{
			HYFontCode.m_HYTbDirectory.vtTableEntry.insert(HYFontCode.m_HYTbDirectory.vtTableEntry.begin() + x, VHEAEntry);
		}
	}
	if (bIsInsertLast){
		HYFontCode.m_HYTbDirectory.vtTableEntry.push_back(VHEAEntry);
	}

	HYFontCode.m_HYTbDirectory.numTables = HYFontCode.m_HYTbDirectory.vtTableEntry.size();

}	// end of void CFontVADHDlg::SetVertHeight()

void CFontVADHDlg::ChangeVertHeight(HYFONTCODEC::CHYFontCodec& HYFontCode,short Tsb)
{
	for (int i = 0; i < HYFontCode.m_HYMaxp.numGlyphs; i++)
	{
		CHYGlyph& glyh = HYFontCode.m_vtHYGlyphs[i];
		if (glyh.vtContour.size() == 0 && glyh.vtComponents.size() == 0) continue;

		if (glyh.vtUnicode.size() > 0) {
			if (::HY_Iszh(glyh.vtUnicode[0])) {				
				glyh.topSB = Tsb;
				glyh.advanceHeight = (glyh.maxY - glyh.minY) + (Tsb << 1);
			}
		}
	}

}	// end of void CFontVADHDlg::ChangeVertHeight()

void CFontVADHDlg::ChangeVertHeight(HYFONTCODEC::CHYFontCodec& HYFontCode, std::vector<std::string> vtPsName, short Tsb)
{
	for (int i = 0; i < vtPsName.size(); i++)
	{
		int index = HYFontCode.FindGryphIndexByGlyName(vtPsName[i]);
		if (index != -1) {
			CHYGlyph& glyh = HYFontCode.m_vtHYGlyphs[index];
			if (glyh.vtContour.size() == 0 && glyh.vtComponents.size() == 0) continue;
			glyh.topSB = Tsb;
			glyh.advanceHeight = (glyh.maxY - glyh.minY) + (Tsb << 1);
		}
	}

}	// end of void CFontVADHDlg::ChangeVertHeight()

BOOL CFontVADHDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	//CheckDlgButton(IDC_VADH_PSNM_CHK, BST_CHECKED);
	GetDlgItem(IDC_VADH_PSNM_EDT)->EnableWindow(FALSE);
	GetDlgItem(IDC_VADH_PSNM_BTN)->EnableWindow(FALSE);
	GetDlgItem(IDC_VADH_PSNM_STC)->EnableWindow(FALSE);

	return TRUE;  
				  
}	// end of BOOL CFontVADHDlg::OnInitDialog()

void CFontVADHDlg::OnBnClickedVadhPsnmChk()
{
	UINT isCheck = IsDlgButtonChecked(IDC_VADH_PSNM_CHK);
	if(isCheck>0){
		GetDlgItem(IDC_VADH_PSNM_EDT)->EnableWindow();
		GetDlgItem(IDC_VADH_PSNM_BTN)->EnableWindow();
		GetDlgItem(IDC_VADH_PSNM_STC)->EnableWindow();		
	}
	else {
		GetDlgItem(IDC_VADH_PSNM_EDT)->EnableWindow(FALSE);
		GetDlgItem(IDC_VADH_PSNM_BTN)->EnableWindow(FALSE);
		GetDlgItem(IDC_VADH_PSNM_STC)->EnableWindow(FALSE);
	}

}	// end void CFontVADHDlg::OnBnClickedVadhPsnmChk()




