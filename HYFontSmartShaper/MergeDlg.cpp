// Merge1Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "HYFontSmartShaper.h"
#include "MergeDlg.h"
#include "HYFontFunc.h"


// CMergeDlg dialog
IMPLEMENT_DYNAMIC(CMergeDlg, CDialog)

CMergeDlg::CMergeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMergeDlg::IDD, pParent)
	, m_strSrc1(_T(""))
	, m_strSrc2(_T(""))
	, m_strDst(_T(""))
	, m_strTxt(_T(""))
	, m_strMTxt(_T(""))
{
	m_FontFlag = -1;

}	// end of CMergeDlg::CMergeDlg(CWnd* pParent /*=NULL*/)

CMergeDlg::~CMergeDlg()
{

}	// end of CMergeDlg::~CMergeDlg()

void CMergeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MMRG_FNTS_LST, m_lsCtrlFnts);
	DDX_Text(pDX, IDC_MRG_SRC1_EDT, m_strSrc1);
	DDX_Text(pDX, IDC_MRG_SRC2_EDT, m_strSrc2);
	DDX_Text(pDX, IDC_MRG_DST_EDT, m_strDst);
	DDX_Text(pDX, IDC_MRG_SRC2TXT_EDT, m_strTxt);
	DDX_Text(pDX, IDC_MMRG_SRC2TXT_EDT, m_strMTxt);
	DDX_Control(pDX, IDC_MRG_PRS, m_prsMerge);
	
}	// end of void CMergeDlg::DoDataExchange()

BEGIN_MESSAGE_MAP(CMergeDlg, CDialog)
	ON_BN_CLICKED(IDC_MRG_SRC1_BTN, &CMergeDlg::OnBnClickedMrg1Src1Btn)
	ON_BN_CLICKED(IDC_MRG_SRC2_BTN, &CMergeDlg::OnBnClickedMrg1Src2Btn)
	ON_BN_CLICKED(IDC_MRG_DST_BTN, &CMergeDlg::OnBnClickedMrg1DstBtn)
	ON_BN_CLICKED(IDC_MGR_MERGE_BTN, &CMergeDlg::OnBnClickedMgr1MergeBtn)
	ON_BN_CLICKED(IDC_MRG_SRC2TXT_BTN, &CMergeDlg::OnBnClickedMrg1TxtBtn)
	ON_BN_CLICKED(IDC_MMRG_ADD_BTN, &CMergeDlg::OnBnClickedMmrgAddBtn)
	ON_BN_CLICKED(IDC_MMRG_DEL_BTN, &CMergeDlg::OnBnClickedMmrgDelBtn)
	ON_BN_CLICKED(IDC_MMRG_UP_BTN, &CMergeDlg::OnBnClickedMmrgUpBtn)
	ON_BN_CLICKED(IDC_MMRG_DWN_BTN, &CMergeDlg::OnBnClickedMmrgDwnBtn)
	ON_BN_CLICKED(IDC_MMRG_MRG_BTN, &CMergeDlg::OnBnClickedMmrgMrgBtn)	
	ON_BN_CLICKED(IDC_MMRG_SRC2TXT_BTN, &CMergeDlg::OnBnClickedMmrgSrc2txtBtn)
END_MESSAGE_MAP()

// CMergeDlg message handlers
BOOL CMergeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	IniCtrl();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}	// end of BOOL CMergeDlg::OnInitDialog()

void CMergeDlg::OnBnClickedMrg1Src1Btn()
{
	TCHAR	szFilters[]=_T("字库文件(*.ttf;*otf)|*.ttf;*otf|TrueType 文件(*.ttf)|*.ttf|OpenType 文件(*.otf)|*.otf||");
	CFileDialog  openFileDlg(TRUE, _T(""), _T(""), OFN_LONGNAMES|OFN_FILEMUSTEXIST,szFilters);
	if (openFileDlg.DoModal() != IDOK)	return;

	m_strSrc1 = openFileDlg.GetPathName();

	UpdateData(FALSE);
	
}	// end of void CMergeDlg::OnBnClickedMrg1Src1Btn()

void CMergeDlg::OnBnClickedMrg1Src2Btn()
{
	TCHAR	szFilters[]=_T("字库文件(*.ttf;*otf)|*.ttf;*otf|TrueType 文件(*.ttf)|*.ttf|OpenType 文件(*.otf)|*.otf||");
	CFileDialog  openFileDlg(TRUE, _T(""), _T(""), OFN_LONGNAMES|OFN_FILEMUSTEXIST,szFilters);
	if (openFileDlg.DoModal() != IDOK)	return;

	m_strSrc2 = openFileDlg.GetPathName();

	UpdateData(FALSE);
	
}	// end of void CMergeDlg::OnBnClickedMrg1Src2Btn()

void CMergeDlg::OnBnClickedMrg1TxtBtn()
{
	TCHAR	szFilters[]=_T("字表文件(*.txt)|*.txt|码表文件(*.mb)|*.mb|Name表文件(*.ne)|*.ne||");
	CFileDialog  openFileDlg(TRUE, _T(""), _T(""), OFN_LONGNAMES|OFN_FILEMUSTEXIST,szFilters);
	if (openFileDlg.DoModal() != IDOK)	return;

	m_strTxt = openFileDlg.GetPathName();

	UpdateData(FALSE);	

}	// end of void CMergeDlg::OnBnClickedMrg1TxtBtn()

void CMergeDlg::OnBnClickedMrg1DstBtn()
{
	TCHAR	szFilters[]=_T("字库文件(*.ttf;*otf)|*.ttf;*otf|TrueType 文件(*.ttf)|*.ttf|OpenType 文件(*.otf)|*.otf||");
	CFileDialog  saveFileDlg(FALSE, _T(""), _T(""), OFN_LONGNAMES,szFilters);
	saveFileDlg.DoModal();
	m_strDst = saveFileDlg.GetPathName();

	UpdateData(FALSE);
	
}	// end of void CMergeDlg::OnBnClickedMrg1DstBtn()

void CMergeDlg::OnBnClickedMgr1MergeBtn()
{
	UpdateData();
	m_prsMerge.SetPos(0);

	CWaitCursor wc;
	CHYFontCodec Dst;

	CHYFontCodec SRC1;
	string strError;
	if (SRC1.Decode((LPSTR)(LPCSTR)m_strSrc1) != HY_NOERROR) {
		strError = "字库 " + ::HY_GetFileNameFromPath((LPSTR)(LPCSTR)m_strSrc1) + "解析失败。";
		AfxMessageBox(strError.c_str());
		return;
	}

	m_prsMerge.SetPos(10);

	Dst.m_vtHYGlyphs = SRC1.m_vtHYGlyphs;
	Dst.m_iFontType = SRC1.m_iFontType;
	Dst.m_HYOS2.version = SRC1.m_HYOS2.version;

	CHYFontCodec SRC2;
	if (SRC2.Decode((LPSTR)(LPCSTR)m_strSrc2) != HY_NOERROR) {
		strError = "字库 " + ::HY_GetFileNameFromPath(string(m_strSrc2)) + "解析失败。";
		AfxMessageBox(strError.c_str());
		return;
	}
	if (SRC2.m_iFontType != Dst.m_iFontType) {
		strError = "字库 " + ::HY_GetFileNameFromPath(string(m_strSrc2)) + "格式不符。";
		AfxMessageBox(strError.c_str());
		return;
	}
	if (m_strTxt != "") {
		std::vector<CHYGlyph> outHYGlyphs;
		GetSubset(SRC2.m_vtHYGlyphs, outHYGlyphs, string(m_strTxt));
		CHYFontFunc::MergeGlyphs(Dst.m_vtHYGlyphs, outHYGlyphs);
	}
	else {
		CHYFontFunc::MergeGlyphs(Dst.m_vtHYGlyphs, SRC2.m_vtHYGlyphs);
	}

	m_prsMerge.SetPos(90);	

	size_t stGlyphNum = Dst.m_vtHYGlyphs.size();
	Dst.m_HYhead.indexToLocFormat = 1;
	Dst.m_HYhead.flags = 3;

	std::vector<unsigned long> ulTableFlag;
	ulTableFlag.push_back(CMAP_TAG);
	ulTableFlag.push_back(DSIG_TAG);
	ulTableFlag.push_back(HEAD_TAG);
	ulTableFlag.push_back(HHEA_TAG);
	ulTableFlag.push_back(HMTX_TAG);
	ulTableFlag.push_back(MAXP_TAG);
	ulTableFlag.push_back(NAME_TAG);
	ulTableFlag.push_back(OS2_TAG);
	ulTableFlag.push_back(POST_TAG);
	ulTableFlag.push_back(VHEA_TAG);
	ulTableFlag.push_back(VMTX_TAG);
	ulTableFlag.push_back(GASP_TAG);

	TableData* pLyData = SRC1.m_mulpTableData.FindTableData(GSUB_TAG);
	if (pLyData) {
		Dst.m_mulpTableData.vtTableData.push_back(*pLyData);
		ulTableFlag.push_back(GSUB_TAG);
	}
	pLyData = SRC1.m_mulpTableData.FindTableData(GPOS_TAG);
	if (pLyData) {
		Dst.m_mulpTableData.vtTableData.push_back(*pLyData);
		ulTableFlag.push_back(GPOS_TAG);
	}

	if (Dst.m_iFontType == FONTTYPE_OTF) {
		ulTableFlag.push_back(CFF_TAG);
		if (Dst.m_HYCFFInfo.TopDICT.IsCIDFont) {
			Dst.m_HYCFFInfo.TopDICT.Ros.strRegistry = ::XSysproxy().m_tagOpeionPrm.Registry;
			Dst.m_HYCFFInfo.TopDICT.Ros.strOrdering = ::XSysproxy().m_tagOpeionPrm.Ordering;
			Dst.m_HYCFFInfo.TopDICT.Ros.Supplement = atol(::XSysproxy().m_tagOpeionPrm.Supplement);

			if (!Dst.m_HYCFFInfo.TopDICT.IsCIDFont) {
				for (size_t i = 0; i < stGlyphNum; i++) {
					const CHYGlyph& glyph = Dst.m_vtHYGlyphs[i];
					Dst.m_HYCFFInfo.stnStrings.InsertString(glyph.strPostName);
				}
			}
		}
	}
	else if (Dst.m_iFontType == FONTTYPE_TTF) {
		ulTableFlag.push_back(GLYF_TAG);
		ulTableFlag.push_back(LOCA_TAG);
		ulTableFlag.push_back(PREP_TAG);
	}

	if (::XSysproxy().m_tagOpeionPrm.bFontname) {
		::XSysproxy().MakeFontName(Dst);
	}
	else {
		Dst.m_HYName = SRC1.m_HYName;
	}
	::XSysproxy().InitEncodeOption(Dst);
	::XSysproxy().SetEncodeOption(Dst, SRC1);

	if (Dst.Encode((LPSTR)(LPCSTR)m_strDst, ulTableFlag, ::XSysproxy().m_tagOpeionPrm) == HY_NOERROR)
		AfxMessageBox(_T("字库生成完成"));
	else
		AfxMessageBox(_T("字库生成失败"));

	m_prsMerge.SetPos(100);

}	// end of void CMergeDlg::OnBnClickedMgr1MergeBtn()

void CMergeDlg::OnBnClickedMmrgSrc2txtBtn()
{
	TCHAR	szFilters[] = _T("字表文件(*.txt)|*.txt|码表文件(*.mb)|*.mb|Name表文件(*.ne)|*.ne||");
	CFileDialog  openFileDlg(TRUE, _T(""), _T(""), OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilters);
	if (openFileDlg.DoModal() != IDOK)	return;

	m_strMTxt = openFileDlg.GetPathName();

	UpdateData(FALSE);

}	// end of void CMergeDlg::OnBnClickedMmrgSrc2txtBtn()

void CMergeDlg::OnBnClickedMmrgAddBtn()
{	
	TCHAR	szFilters[] = _T("TrueType File(*.ttf)|*.ttf|OpenType File(*.otf)|*.otf||");

	CFileDialog	Opendlg(TRUE, _T(""), _T(""), OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilters);
	if (Opendlg.DoModal() != IDOK)	return;

	CString strFileName = Opendlg.GetPathName();
	m_vtFntName.push_back((LPCTSTR)(LPCSTR)strFileName);
	UpdateListCtrl();
	
	SetListCtrlSelectItem(m_vtFntName.size() - 1);

}	// end of void CMergeDlg::OnBnClickedMmrgAddBtn()

void CMergeDlg::OnBnClickedMmrgDelBtn()
{
	int	iIndx =	GetListCtrlSelectItem();
	if (iIndx >= 0 && iIndx < m_vtFntName.size()) {
		m_vtFntName.erase(m_vtFntName.begin() + iIndx);
	}

	UpdateListCtrl();
	SetListCtrlSelectItem(iIndx);
	
}	// end of void CMergeDlg::OnBnClickedMmrgDelBtn()

void CMergeDlg::OnBnClickedMmrgUpBtn()
{
	int	iIndx = GetListCtrlSelectItem();
	if (iIndx > 0) {	
		string SelFntName = m_vtFntName[iIndx];
		m_vtFntName.erase(m_vtFntName.begin() + iIndx);
		m_vtFntName.insert(m_vtFntName.begin() + --iIndx, SelFntName);
	}

	UpdateListCtrl();
	SetListCtrlSelectItem(iIndx);
	
}	// end of void CMergeDlg::OnBnClickedMmrgUpBtn()

void CMergeDlg::OnBnClickedMmrgDwnBtn()
{
	int	iIndx = GetListCtrlSelectItem();
	if (iIndx < m_vtFntName.size() - 1) {
		string SelFntName = m_vtFntName[iIndx];
		m_vtFntName.erase(m_vtFntName.begin() + iIndx);
		m_vtFntName.insert(m_vtFntName.begin() + ++iIndx, SelFntName);
	}

	UpdateListCtrl();
	SetListCtrlSelectItem(iIndx);
	
}	// end of void CMergeDlg::OnBnClickedMmrgDwnBtn()

void CMergeDlg::OnBnClickedMmrgMrgBtn()
{
	TCHAR	szFilters[] = _T("TrueType File(*.ttf)|*.ttf|OpenType File(*.otf)|*.otf||");
	CFileDialog	Opendlg(FALSE, _T(""), _T(""), OFN_LONGNAMES, szFilters);
	if (Opendlg.DoModal() != IDOK)	return;
	CString strFileName = Opendlg.GetPathName();
	
	MergeFont(string(strFileName),m_vtFntName);
	
}	// end of void CMergeDlg::OnBnClickedMmrgMrgBtn()

void CMergeDlg::MergeFont(string strDst, std::vector<string> vtFntName)
{
	CWaitCursor wc;
	CHYFontCodec Dst;
	string strError;

	m_prsMerge.SetPos(0);
	if (vtFntName.size() == 0) return;

	Dst.m_HYhead.unitsPerEm = ::XSysproxy().m_tagOpeionPrm.usUnitsPerEM;
	Dst.m_HYOS2.sTypoAscender = ::XSysproxy().m_tagOpeionPrm.sTypo_Ascender;
	Dst.m_HYOS2.sTypoDescender = ::XSysproxy().m_tagOpeionPrm.sTypo_Descender;
	Dst.m_HYOS2.ulCodePageRange1 = ::XSysproxy().m_tagOpeionPrm.ulCodePageRang1;
	Dst.m_HYOS2.ulCodePageRange2 = ::XSysproxy().m_tagOpeionPrm.ulCodePageRang2;
	
	for (size_t i = 0; i < vtFntName.size(); i++) {
		CHYFontCodec SRCTmp;
		if (SRCTmp.Decode(vtFntName[i].c_str()) != HY_NOERROR) {
			strError = "字库 " + ::HY_GetFileNameFromPath(vtFntName[i]) + "解析失败。";
			AfxMessageBox(strError.c_str());
			return;
		}
		if (i == 0) {
			Dst.m_iFontType = SRCTmp.m_iFontType;
			if (::XSysproxy().m_tagOpeionPrm.bFontname) {
				::XSysproxy().MakeFontName(Dst);
			}
			else {
				Dst.m_HYName = SRCTmp.m_HYName;
			}
		}
		else {
			if (SRCTmp.m_iFontType != Dst.m_iFontType) {
				strError = "字库 " + ::HY_GetFileNameFromPath(vtFntName[i]) + "格式不符。";
				AfxMessageBox(strError.c_str());
				return;
			}
		}

		if (SRCTmp.m_HYhead.unitsPerEm != Dst.m_HYhead.unitsPerEm) {
			strError = "字库 " + ::HY_GetFileNameFromPath(vtFntName[i]) + "EM与设置不符";
			AfxMessageBox(strError.c_str());
			return;
		}

		if (m_strMTxt != "") {
			std::vector<CHYGlyph> outHYGlyphs;
			GetSubset(SRCTmp.m_vtHYGlyphs, outHYGlyphs, string(m_strMTxt));
			CHYFontFunc::MergeGlyphs(Dst.m_vtHYGlyphs, outHYGlyphs);
		}
		else {
			CHYFontFunc::MergeGlyphs(Dst.m_vtHYGlyphs, SRCTmp.m_vtHYGlyphs);
		}	

		m_prsMerge.SetPos(((i+1)*90/ vtFntName.size()));
	}

	size_t stGlyphNum = Dst.m_vtHYGlyphs.size();	
	Dst.m_HYhead.indexToLocFormat = 1;
	Dst.m_HYhead.flags = 3;	

	std::vector<unsigned long> ulTableFlag;
	ulTableFlag.push_back(CMAP_TAG);
	ulTableFlag.push_back(DSIG_TAG);
	ulTableFlag.push_back(HEAD_TAG);
	ulTableFlag.push_back(HHEA_TAG);
	ulTableFlag.push_back(HMTX_TAG);
	ulTableFlag.push_back(MAXP_TAG);
	ulTableFlag.push_back(NAME_TAG);
	ulTableFlag.push_back(OS2_TAG);
	ulTableFlag.push_back(POST_TAG);
	ulTableFlag.push_back(VHEA_TAG);
	ulTableFlag.push_back(VMTX_TAG);
	ulTableFlag.push_back(GASP_TAG);

	if (Dst.m_iFontType == FONTTYPE_OTF) {
		ulTableFlag.push_back(CFF_TAG);
		if (Dst.m_HYCFFInfo.TopDICT.IsCIDFont) {
			Dst.m_HYCFFInfo.TopDICT.Ros.strRegistry = ::XSysproxy().m_tagOpeionPrm.Registry;
			Dst.m_HYCFFInfo.TopDICT.Ros.strOrdering = ::XSysproxy().m_tagOpeionPrm.Ordering;
			Dst.m_HYCFFInfo.TopDICT.Ros.Supplement = atol(::XSysproxy().m_tagOpeionPrm.Supplement);

			if (!Dst.m_HYCFFInfo.TopDICT.IsCIDFont) {
				for (size_t i = 0; i < stGlyphNum; i++) {
					const CHYGlyph& glyph = Dst.m_vtHYGlyphs[i];
					Dst.m_HYCFFInfo.stnStrings.InsertString(glyph.strPostName);
				}
			}
		}
	}
	else if (Dst.m_iFontType == FONTTYPE_TTF) {
		ulTableFlag.push_back(GLYF_TAG);
		ulTableFlag.push_back(LOCA_TAG);
		ulTableFlag.push_back(PREP_TAG);
	}

	::XSysproxy().InitEncodeOption(Dst);

	Dst.MakeHYCodeMap();
	if (Dst.Encode((char*)strDst.c_str(), ulTableFlag, ::XSysproxy().m_tagOpeionPrm) == HY_NOERROR)	
		AfxMessageBox(_T("字库生成完成"));
	else
		AfxMessageBox(_T("字库生成失败"));

	m_prsMerge.SetPos(100);

}	//end of void CMergeDlg::MergeFont()

void CMergeDlg::IniCtrl()
{
	LVCOLUMN		tagColumn;
	ZeroMemory(&tagColumn, sizeof(LVCOLUMN));

	tagColumn.mask = LVCF_WIDTH | LVCF_TEXT;
	tagColumn.cx = 250;
	tagColumn.pszText = _T("字体文件");
	m_lsCtrlFnts.InsertColumn(0, &tagColumn);
	m_lsCtrlFnts.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	m_prsMerge.SetRange(1, 100);
	m_prsMerge.SetStep(1);

}	// end of void	CMergeDlg::IniCtrl()

void CMergeDlg::UninitCtrl()
{

}	// end of void	CMergeDlg::UninitCtrl()

void CMergeDlg::UpdateListCtrl()
{
	m_lsCtrlFnts.DeleteAllItems();
	for (size_t i = 0; i < m_vtFntName.size(); i++) {
		LVITEM	tagLvItem;
		ZeroMemory(&tagLvItem, sizeof(LVITEM));

		tagLvItem.mask = LVIF_TEXT | LVIF_STATE;
		tagLvItem.iItem = i;
		tagLvItem.iSubItem = 0;
		tagLvItem.pszText = new char[MAX_PATH];
		tagLvItem.cchTextMax = MAX_PATH;
		tagLvItem.stateMask = -1;
		tagLvItem.state = 0;

		ZeroMemory(tagLvItem.pszText, MAX_PATH);
		strcpy_s(tagLvItem.pszText, MAX_PATH, ::HY_GetFullNameFromPath(m_vtFntName[i]).data());
		m_lsCtrlFnts.InsertItem(&tagLvItem);

		delete[] tagLvItem.pszText;
	}
	m_lsCtrlFnts.SetFocus();

}	// end of void	CMergeDlg::UpdateListCtrl()

int	CMergeDlg::GetListCtrlSelectItem()
{
	int nItem = -1;
	POSITION pos = m_lsCtrlFnts.GetFirstSelectedItemPosition();
	if (pos){
		nItem = m_lsCtrlFnts.GetNextSelectedItem(pos);
	}
	return nItem;

}	// end of int CMergeDlg::GetListCtrlSelect1Item()

void CMergeDlg::SetListCtrlSelectItem(int iIndex)
{
	size_t sz = m_vtFntName.size();

	if (iIndex == -1) 	iIndex = 0;
	if (iIndex == sz)	iIndex = sz - 1;
	
	if (sz > iIndex)
		m_lsCtrlFnts.SetItemState(iIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	else
		m_lsCtrlFnts.SetItemState(iIndex - 1 > 0 ? iIndex - 1 : 0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

}	// end of void CMergeDlg::SetListCtrlSelectItem()

void CMergeDlg::GetSubset(std::vector<CHYGlyph>& InGlyphs, std::vector<CHYGlyph>& outHYGlyphs,string strsbsFile)
{	
	string strExt = HY_GetExtNameFromPath(HY_StringToLowercase(strsbsFile));
	if (strExt == ".mb") {
		std::vector<unsigned long> vtUnicode;
		::HY_LoadCodeFile((char*)strsbsFile.c_str(), vtUnicode);
		CHYFontFunc::GetSubsetbyUni(InGlyphs, vtUnicode, outHYGlyphs);
	}
	if (strExt == ".ne") {
		std::vector<string> vtName;
		::HY_LoadPsNameFile((char*)strsbsFile.c_str(), vtName);
		CHYFontFunc::GetSubsetbyName(InGlyphs, vtName, outHYGlyphs);
	}
	if (strExt == ".txt") {
		std::vector<unsigned long> vtUnicode;
		::HY_LoadCharTableFile((char*)strsbsFile.c_str(), vtUnicode);
		CHYFontFunc::GetSubsetbyUni(InGlyphs, vtUnicode, outHYGlyphs);
	}

}	// end of void CMergeDlg::GetSubset()


