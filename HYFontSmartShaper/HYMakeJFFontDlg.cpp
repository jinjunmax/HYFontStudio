// HYMakeJFFontDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "HYFontSmartShaper.h"
#include "HYMakeJFFontDlg.h"
#include "afxdialogex.h"

// CHYMakeJFFontDlg 对话框

IMPLEMENT_DYNAMIC(CHYMakeJFFontDlg, CDialog)

CHYMakeJFFontDlg::CHYMakeJFFontDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_FNT_MKJF_DLG, pParent)
{

}	// end of CHYMakeJFFontDlg::CHYMakeJFFontDlg()

CHYMakeJFFontDlg::~CHYMakeJFFontDlg()
{
		
}	// end of CHYMakeJFFontDlg::~CHYMakeJFFontDlg()

void CHYMakeJFFontDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

}	// end of void CHYMakeJFFontDlg::DoDataExchange()

BEGIN_MESSAGE_MAP(CHYMakeJFFontDlg, CDialog)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_MJF_OPEN_BTN, &CHYMakeJFFontDlg::OnBnClickedMjfOpenBtn)
END_MESSAGE_MAP()

// CHYMakeJFFontDlg 消息处理程序
BOOL CHYMakeJFFontDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString strCharPath;
	strCharPath.Format("%s\\data\\6763F.txt", ::XSysproxy().m_strAppDir);
	HY_LoadCharTableFile((LPSTR)(LPCSTR)strCharPath, m_vtFUni);

	strCharPath.Format("%s\\data\\6763J.txt", ::XSysproxy().m_strAppDir);
	HY_LoadCharTableFile((LPSTR)(LPCSTR)strCharPath, m_vtJUni);

	strCharPath.Format("%s\\data\\9169.txt", ::XSysproxy().m_strAppDir);
	HY_LoadCharTableFile((LPSTR)(LPCSTR)strCharPath, m_vtWUni);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}	// end of BOOL CHYMakeJFFontDlg::OnInitDialog()

void CHYMakeJFFontDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 在此处添加消息处理程序代码

}	// end of void CHYMakeJFFontDlg::OnDestroy()

void CHYMakeJFFontDlg::OnBnClickedMjfOpenBtn()
{
	TCHAR	szFilters[] = _T("turetype 文件(*.ttf)|*.ttf||");
	CFileDialog  openFileDlg(TRUE, _T(""), _T(""), OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilters);
	if (openFileDlg.DoModal() != IDOK)	return;

	CString strOpenFile = openFileDlg.GetPathName();
	int err = 0;
	int iFilestat = ::HY_FileExist((LPSTR)(LPCSTR)strOpenFile);
	if (iFilestat <= 0){
		AfxMessageBox("文件不存在");
		return;
	}	
	if (m_vtFUni.size() == 0){
		AfxMessageBox("读取繁体字表失败！");
		return;
	}
	if (m_vtJUni.size() == 0){
		AfxMessageBox("读取简体字表失败！");
		return;
	}

	CWaitCursor	wc;
	CString strJPath, strFPath;
	std::string strDir = HY_GetDirFromPath(std::string(strOpenFile));
	std::string strFileName = HY_GetFileNameFromPath(std::string(strOpenFile));
	std::string strExtName = HY_GetExtNameFromPath(std::string(strOpenFile));

	size_t stOffset = strFileName.length() - 1;
	strFileName.replace(stOffset,1,"J");
	strJPath.Format("%s%s%s", strDir.c_str(), strFileName.c_str(), strExtName.c_str());
	strFileName.replace(stOffset, 1, "F");
	strFPath.Format("%s%s%s", strDir.c_str(), strFileName.c_str(), strExtName.c_str());
	MakeJF(strOpenFile, strJPath, strFPath);

	AfxMessageBox("包装完成");

}	//end of void CHYMakeJFFontDlg::OnBnClickedMjfOpenBtn()

void CHYMakeJFFontDlg::MakeJF(CString strWFontFile,CString strJPath,CString strFPath)
{
	CHYFontCodec		FontWDecode;	
	DecodeW(strWFontFile, FontWDecode);	

	std::vector<GlyfData> vtXiWen;
	RemoveHan(FontWDecode, vtXiWen);

	std::vector<GlyfData> vtJGlyfData;
	MakeJ(FontWDecode, strJPath,vtJGlyfData,vtXiWen);

	std::vector<GlyfData> vtFGlyfData;
	MakeF(FontWDecode, strFPath, vtFGlyfData, vtXiWen);

	FontWDecode.CloseFile();

}	// end of bool CHYMakeJFFontDlg::MakeJF()

void CHYMakeJFFontDlg::MakeJ(CHYFontCodec& FontWDecode, CString strJPath, std::vector<GlyfData>& vtJ, std::vector<GlyfData>& vtXiWen)
{
	m_HYTbDirectory = FontWDecode.m_HYTbDirectory;
	for (size_t i = 0; i < m_HYTbDirectory.numTables; i++) {
		m_HYTbDirectory.vtTableEntry[i].checkSum = 0;
		m_HYTbDirectory.vtTableEntry[i].length = 0;
		m_HYTbDirectory.vtTableEntry[i].offset = 0;
	}

	Extraglyf(FontWDecode, vtJ, m_vtJUni);

	//合库
	std::vector<GlyfData> vtFullJ;
	vtFullJ = vtXiWen;

	size_t st = vtJ.size();
	for (size_t i = 0; i < st; i++) {
		vtFullJ.push_back(vtJ[i]);
	}

	MakeHYCodeMap(vtFullJ);
	MakeName(FontWDecode.m_HYName, m_HYNewName, MAKE_J);
	m_HYNewCmap = FontWDecode.m_HYCmap;

	BulidFont(FontWDecode, (LPSTR)(LPCSTR)strJPath, vtFullJ);

}	// end of void CHYMakeJFFontDlg::MakeJ()

void CHYMakeJFFontDlg::MakeF(CHYFontCodec& FontWDecode, CString strFPath, std::vector<GlyfData>& vtF, std::vector<GlyfData>& vtXiWen)
{
	m_HYTbDirectory = FontWDecode.m_HYTbDirectory;
	for (size_t i = 0; i < m_HYTbDirectory.numTables; i++) {
		m_HYTbDirectory.vtTableEntry[i].checkSum = 0;
		m_HYTbDirectory.vtTableEntry[i].length = 0;
		m_HYTbDirectory.vtTableEntry[i].offset = 0;
	}

	Extraglyf(FontWDecode, vtF, m_vtFUni);

	size_t stSize = vtF.size();
	for (size_t i = 0; i < stSize; i++)
	{
		unsigned long uni = m_vtJUni[i];

		vtF[i].vtUnicode.clear();

		std::stringstream ss;
		ss << "uni" << uppercase << std::hex << uni << endl;

		vtF[i].strName = ss.str();
		vtF[i].vtUnicode.push_back(uni);
	}

	//合库
	std::vector<GlyfData> vtFullF;
	vtFullF = vtXiWen;

	size_t st = vtF.size();
	for (size_t i = 0; i < st; i++) {
		vtFullF.push_back(vtF[i]);
	}

	MakeHYCodeMap(vtFullF);
	MakeName(FontWDecode.m_HYName, m_HYNewName, MAKE_F);
	BulidFont(FontWDecode, (LPSTR)(LPCSTR)strFPath, vtFullF);

}	// end of void CHYMakeJFFontDlg::MakeF()

bool CHYMakeJFFontDlg::DecodeW(CString strWFontFile, CHYFontCodec& FontWDecode)
{
	if (FontWDecode.OpenFile(strWFontFile) != HY_NOERROR) {
		AfxMessageBox("读取字库文件失败！");
		return false;
	}

	FontWDecode.DecodeTableDirectory();
	if (FontWDecode.m_HYTbDirectory.version.value != 1 && FontWDecode.m_HYTbDirectory.version.fract != 0) {
		AfxMessageBox("暂不支持除ttf之外的字库！包括可变字库。");
		return false;
	}

	if (FontWDecode.m_HYTbDirectory.FindTableEntry(CMAP_TAG) == -1)		return false;
	FontWDecode.Decodecmap();
	if (FontWDecode.m_HYTbDirectory.FindTableEntry(MAXP_TAG) == -1)		return false;
	FontWDecode.Decodemaxp();
	if (FontWDecode.m_HYTbDirectory.FindTableEntry(HEAD_TAG) == -1)		return false;
	FontWDecode.Decodehead();
	if (FontWDecode.m_HYTbDirectory.FindTableEntry(LOCA_TAG) == -1)		return false;
	FontWDecode.Decodeloca();
	if (FontWDecode.m_HYTbDirectory.FindTableEntry(HHEA_TAG) == -1)		return false;
	FontWDecode.Decodehhea();
	if (FontWDecode.m_HYTbDirectory.FindTableEntry(HMTX_TAG) == -1)		return false;
	FontWDecode.Decodehmtx();
	if (FontWDecode.m_HYTbDirectory.FindTableEntry(POST_TAG) == -1)		return false;
	FontWDecode.Decodepost();
	if (FontWDecode.m_HYTbDirectory.FindTableEntry(VHEA_TAG) == -1)		return false;
	FontWDecode.Decodevhea();
	if (FontWDecode.m_HYTbDirectory.FindTableEntry(VMTX_TAG) == -1)		return false;
	FontWDecode.Decodevmtx();
	if (FontWDecode.m_HYTbDirectory.FindTableEntry(NAME_TAG) == -1)		return false;
	FontWDecode.Decodename();

	// 当前字库不包括VDMX
	if (FontWDecode.m_HYTbDirectory.FindTableEntry(VDMX_TAG) != -1) {
		int iIndex = FontWDecode.m_HYTbDirectory.FindTableEntry(VDMX_TAG);
		FontWDecode.m_HYTbDirectory.vtTableEntry.erase(FontWDecode.m_HYTbDirectory.vtTableEntry.begin() + iIndex);
		FontWDecode.m_HYTbDirectory.numTables--;
	}
	// 当前字库不包括HDMX
	if (FontWDecode.m_HYTbDirectory.FindTableEntry(HDMX_TAG) != -1) {
		int iIndex = FontWDecode.m_HYTbDirectory.FindTableEntry(HDMX_TAG);
		FontWDecode.m_HYTbDirectory.vtTableEntry.erase(FontWDecode.m_HYTbDirectory.vtTableEntry.begin() + iIndex);
		FontWDecode.m_HYTbDirectory.numTables--;
	}
	//LTSH 在中文字库中没有实际意义
	if (FontWDecode.m_HYTbDirectory.FindTableEntry(LTSH_TAG) != -1) {
		int iIndex = FontWDecode.m_HYTbDirectory.FindTableEntry(LTSH_TAG);
		FontWDecode.m_HYTbDirectory.vtTableEntry.erase(FontWDecode.m_HYTbDirectory.vtTableEntry.begin() + iIndex);
		FontWDecode.m_HYTbDirectory.numTables--;
	}

	return true;

}	// end of bool CHYMakeJFFontDlg::DecodeW()

void CHYMakeJFFontDlg::RemoveHan(CHYFontCodec& FontWDecode, std::vector<GlyfData>& vtXiWen)
{
	vtXiWen.clear();

	int iTableEntryIndx = FontWDecode.m_HYTbDirectory.FindTableEntry(GLYF_TAG);
	if (iTableEntryIndx == -1)		return;

	CHYTableEntry& entry = FontWDecode.m_HYTbDirectory.vtTableEntry[iTableEntryIndx];
	fseek(FontWDecode.m_pFontFile, entry.offset, SEEK_SET);

	for (size_t i = 0; i < FontWDecode.m_HYMaxp.numGlyphs; i++) {
		std::vector<unsigned long> vtUnicode;
		FontWDecode.FindGryphUncidoByIndex(i, vtUnicode);

		bool b = false;
		if (vtUnicode.size() == 0) b = true;
		else {
			if (::CheckUniRepeat(vtUnicode[0], m_vtWUni) == 0)
				b = true;
		}

		// 如果glyph中的unicode编码，不在uni字表数组里就保留下来。
		if (b) {
			GlyfData glyfData;
			if (i == 0)
				glyfData.vtUnicode.push_back(0xffff);
			else
				glyfData.vtUnicode = vtUnicode;

			glyfData.pDataLen = FontWDecode.m_HYloca.vtLoca[i + 1] - FontWDecode.m_HYloca.vtLoca[i];
			if (glyfData.pDataLen > 0) {
				fseek(FontWDecode.m_pFontFile, entry.offset + FontWDecode.m_HYloca.vtLoca[i], SEEK_SET);
				for (int j = 0; j<glyfData.pDataLen; j++) {
					byte bData = 0;
					fread(&bData, 1, 1, FontWDecode.m_pFontFile);
					glyfData.vtData.push_back(bData);
				}

				fseek(FontWDecode.m_pFontFile, entry.offset + FontWDecode.m_HYloca.vtLoca[i], SEEK_SET);
				// contour numbers			
				fread(&glyfData.CnturNum, 2, 1, FontWDecode.m_pFontFile);
				glyfData.CnturNum = hy_cdr_int16_to(glyfData.CnturNum);
				//Minimum x for coordinate data 
				fread(&glyfData.xMin, 2, 1, FontWDecode.m_pFontFile);
				glyfData.xMin = hy_cdr_int16_to(glyfData.xMin);
				//Minimum y for coordinate data 
				fread(&glyfData.yMin, 2, 1, FontWDecode.m_pFontFile);
				glyfData.yMin = hy_cdr_int16_to(glyfData.yMin);
				//Maximum x for coordinate data 
				fread(&glyfData.xMax, 2, 1, FontWDecode.m_pFontFile);
				glyfData.xMax = hy_cdr_int16_to(glyfData.xMax);
				//Maximum y for coordinate data 
				fread(&glyfData.yMax, 2, 1, FontWDecode.m_pFontFile);
				glyfData.yMax = hy_cdr_int16_to(glyfData.yMax);
			}

			glyfData.strName = FontWDecode.m_HYPost.FindNameByGID(i);
			if (i<FontWDecode.m_HYHhea.numberOfHMetrics) {
				glyfData.usAdvWidth = FontWDecode.m_HYHmtx.vtLonghormetric[i].advanceWidth;
				glyfData.sLsb = FontWDecode.m_HYHmtx.vtLonghormetric[i].lsb;
			}
			else {
				glyfData.usAdvWidth = FontWDecode.m_HYHmtx.vtLonghormetric[FontWDecode.m_HYHhea.numberOfHMetrics - 1].advanceWidth;
				glyfData.sLsb = FontWDecode.m_HYHmtx.vtLeftsidebearing[i - FontWDecode.m_HYHhea.numberOfHMetrics];
			}

			if (FontWDecode.m_HYVhea.numOfLongVerMetrics > 0) {
				if (i<FontWDecode.m_HYVhea.numOfLongVerMetrics) {
					glyfData.usAdvHeight = FontWDecode.m_HYVmtx.vtLongVrtmetric[i].advanceHeight;
					glyfData.sTsb = FontWDecode.m_HYVmtx.vtLongVrtmetric[i].tsb;
				}
				else {
					glyfData.usAdvHeight = FontWDecode.m_HYVmtx.vtLongVrtmetric[FontWDecode.m_HYVhea.numOfLongVerMetrics - 1].advanceHeight;
					glyfData.sTsb = FontWDecode.m_HYVmtx.vtTopsidebearing[i - FontWDecode.m_HYVhea.numOfLongVerMetrics];
				}
			}
			vtXiWen.push_back(glyfData);
		}
	}

}	// bool CHYMakeJFFontDlg::RemoveHan()

void CHYMakeJFFontDlg::MakeName(CHYName& inName, CHYName& outName, int iJFFlag)
{	
	outName.SetDefault();
	
	size_t st = inName.vtLangTargeRecord.size();	
	for (size_t i = 0; i < st; i++)
	{
		LANGTAGRECORD&  inlgnRecord = inName.vtLangTargeRecord[i];
		outName.vtLangTargeRecord.push_back(inlgnRecord);
	}

	st = inName.vtNameRecord.size();
	std::string strFamilyEN = inName.GetFamilyNameEN();
	std::string strSubFamileEN = inName.GetSubFamilyEN();
	std::string strFamilyCN = inName.GetFamilyNameCN();
	std::string strSubFamileCN = inName.GetSubFamilyCN();

	std::string strJFEN = "";
	std::string strJFCN = "";
	if (iJFFlag == MAKE_J)
	{
		strJFEN = _T("J");
		strJFCN = _T("简");
	}
	if (iJFFlag == MAKE_F)
	{
		strJFEN = _T("F");
		strJFCN = _T("繁");
	}
	NAMERECORD outnmRecord;

	// 1.0.0 
	outnmRecord.platformID = 1;
	outnmRecord.encodingID = 0;
	outnmRecord.languageID = 0;
	outnmRecord.nameID = 0;			
	outnmRecord.strContent = inName.GetNamestring(outnmRecord.platformID, outnmRecord.encodingID, outnmRecord.languageID,0);
	outName.vtNameRecord.push_back(outnmRecord);			
	outnmRecord.nameID = 1;
	outnmRecord.strContent = strFamilyEN;
	outName.vtNameRecord.push_back(outnmRecord);
	outnmRecord.nameID = 2;
	outnmRecord.strContent = strJFEN;
	outName.vtNameRecord.push_back(outnmRecord);
	outnmRecord.nameID = 3;
	outnmRecord.strContent = "Hanyi "+ strFamilyEN + "-"+ strJFEN +" v9.00.00";
	outName.vtNameRecord.push_back(outnmRecord);
	outnmRecord.nameID = 4;
	outnmRecord.strContent = strFamilyEN + " "+strJFEN;
	outName.vtNameRecord.push_back(outnmRecord);
	outnmRecord.nameID = 5;
	outnmRecord.strContent = inName.GetNamestring(outnmRecord.platformID, outnmRecord.encodingID, outnmRecord.languageID, 5);
	outName.vtNameRecord.push_back(outnmRecord);
	outnmRecord.nameID = 6;
	outnmRecord.strContent = strFamilyEN + "-"+ strJFEN;
	outName.vtNameRecord.push_back(outnmRecord);
	outnmRecord.nameID = 7;
	outnmRecord.strContent = inName.GetNamestring(outnmRecord.platformID, outnmRecord.encodingID, outnmRecord.languageID, 7);
	outName.vtNameRecord.push_back(outnmRecord);
	outnmRecord.nameID = 8;
	outnmRecord.strContent = inName.GetNamestring(outnmRecord.platformID, outnmRecord.encodingID, outnmRecord.languageID, 8);
	outName.vtNameRecord.push_back(outnmRecord);
	outnmRecord.nameID = 11;
	outnmRecord.strContent = inName.GetNamestring(outnmRecord.platformID, outnmRecord.encodingID, outnmRecord.languageID, 11);
	outName.vtNameRecord.push_back(outnmRecord);
	outnmRecord.nameID = 13;
	outnmRecord.strContent = inName.GetNamestring(outnmRecord.platformID, outnmRecord.encodingID, outnmRecord.languageID, 13);
	outName.vtNameRecord.push_back(outnmRecord);
	outnmRecord.nameID = 14;
	outnmRecord.strContent = inName.GetNamestring(outnmRecord.platformID, outnmRecord.encodingID, outnmRecord.languageID, 14);
	outName.vtNameRecord.push_back(outnmRecord);

	//1.25.33
	outnmRecord.platformID = 1;
	outnmRecord.encodingID = 25;
	outnmRecord.languageID = 33;
	outnmRecord.nameID = 0;
	outnmRecord.strContent = inName.GetNamestring(outnmRecord.platformID, outnmRecord.encodingID, outnmRecord.languageID, 0);
	outName.vtNameRecord.push_back(outnmRecord);
	outnmRecord.nameID = 1;
	outnmRecord.strContent = strFamilyCN;
	outName.vtNameRecord.push_back(outnmRecord);
	outnmRecord.nameID = 2;
	outnmRecord.strContent = strJFCN;
	outName.vtNameRecord.push_back(outnmRecord);	
	outnmRecord.nameID = 4;
	outnmRecord.strContent = strFamilyCN + " " + strJFCN;
	outName.vtNameRecord.push_back(outnmRecord);
	outnmRecord.nameID = 7;
	outnmRecord.strContent = inName.GetNamestring(outnmRecord.platformID, outnmRecord.encodingID, outnmRecord.languageID, 7);
	outName.vtNameRecord.push_back(outnmRecord);
	outnmRecord.nameID = 8;
	outnmRecord.strContent = inName.GetNamestring(outnmRecord.platformID, outnmRecord.encodingID, outnmRecord.languageID, 8);
	outName.vtNameRecord.push_back(outnmRecord);	
	outnmRecord.nameID = 13;
	outnmRecord.strContent = inName.GetNamestring(outnmRecord.platformID, outnmRecord.encodingID, outnmRecord.languageID, 13);
	outName.vtNameRecord.push_back(outnmRecord);

	//3.1.1033
	outnmRecord.platformID = 3;
	outnmRecord.encodingID = 1;
	outnmRecord.languageID = 1033;
	outnmRecord.nameID = 0;
	outnmRecord.strContent = inName.GetNamestring(outnmRecord.platformID, outnmRecord.encodingID, outnmRecord.languageID, 0);
	outName.vtNameRecord.push_back(outnmRecord);
	outnmRecord.nameID = 1;
	outnmRecord.strContent = strFamilyEN + " " + strJFEN;
	outName.vtNameRecord.push_back(outnmRecord);
	outnmRecord.nameID = 2;
	outnmRecord.strContent = inName.GetNamestring(outnmRecord.platformID, outnmRecord.encodingID, outnmRecord.languageID, 2);
	outName.vtNameRecord.push_back(outnmRecord);
	outnmRecord.nameID = 3;
	outnmRecord.strContent = "Hanyi " + strFamilyEN + "-" + strJFEN +" v9.00.00";
	outName.vtNameRecord.push_back(outnmRecord);
	outnmRecord.nameID = 4;
	outnmRecord.strContent = strFamilyEN + " " + strJFEN;
	outName.vtNameRecord.push_back(outnmRecord);
	outnmRecord.nameID = 5;
	outnmRecord.strContent = inName.GetNamestring(outnmRecord.platformID, outnmRecord.encodingID, outnmRecord.languageID, 5);
	outName.vtNameRecord.push_back(outnmRecord);
	outnmRecord.nameID = 6;
	outnmRecord.strContent = strFamilyEN + "-" + strJFEN;
	outName.vtNameRecord.push_back(outnmRecord);
	outnmRecord.nameID = 7;
	outnmRecord.strContent = inName.GetNamestring(outnmRecord.platformID, outnmRecord.encodingID, outnmRecord.languageID, 7);
	outName.vtNameRecord.push_back(outnmRecord);
	outnmRecord.nameID = 8;
	outnmRecord.strContent = inName.GetNamestring(outnmRecord.platformID, outnmRecord.encodingID, outnmRecord.languageID, 8);
	outName.vtNameRecord.push_back(outnmRecord);
	outnmRecord.nameID = 11;
	outnmRecord.strContent = inName.GetNamestring(outnmRecord.platformID, outnmRecord.encodingID, outnmRecord.languageID, 11);
	outName.vtNameRecord.push_back(outnmRecord);
	outnmRecord.nameID = 13;
	outnmRecord.strContent = inName.GetNamestring(outnmRecord.platformID, outnmRecord.encodingID, outnmRecord.languageID, 13);
	outName.vtNameRecord.push_back(outnmRecord);
	outnmRecord.nameID = 14;
	outnmRecord.strContent = inName.GetNamestring(outnmRecord.platformID, outnmRecord.encodingID, outnmRecord.languageID, 14);
	outName.vtNameRecord.push_back(outnmRecord);
	outnmRecord.nameID = 16;
	outnmRecord.strContent = inName.GetNamestring(outnmRecord.platformID, outnmRecord.encodingID, outnmRecord.languageID, 16);
	outName.vtNameRecord.push_back(outnmRecord);
	outnmRecord.nameID = 17;
	outnmRecord.strContent = strJFEN;
	outName.vtNameRecord.push_back(outnmRecord);
	
	//3.1.2052
	outnmRecord.platformID = 3;
	outnmRecord.encodingID = 1;
	outnmRecord.languageID = 2052;
	outnmRecord.nameID = 0;
	outnmRecord.strContent = inName.GetNamestring(outnmRecord.platformID, outnmRecord.encodingID, outnmRecord.languageID, 0);
	outName.vtNameRecord.push_back(outnmRecord);
	outnmRecord.nameID = 1;
	outnmRecord.strContent = strFamilyCN + " " + strJFCN;
	outName.vtNameRecord.push_back(outnmRecord);
	outnmRecord.nameID = 2;
	outnmRecord.strContent = inName.GetNamestring(outnmRecord.platformID, outnmRecord.encodingID, outnmRecord.languageID, 2);
	outName.vtNameRecord.push_back(outnmRecord);
	outnmRecord.nameID = 4;
	outnmRecord.strContent = strFamilyCN + " " + strJFCN;
	outName.vtNameRecord.push_back(outnmRecord);
	outnmRecord.nameID = 7;
	outnmRecord.strContent = inName.GetNamestring(outnmRecord.platformID, outnmRecord.encodingID, outnmRecord.languageID, 7);
	outName.vtNameRecord.push_back(outnmRecord);
	outnmRecord.nameID = 8;
	outnmRecord.strContent = inName.GetNamestring(outnmRecord.platformID, outnmRecord.encodingID, outnmRecord.languageID, 8);
	outName.vtNameRecord.push_back(outnmRecord);
	outnmRecord.nameID = 13;
	outnmRecord.strContent = inName.GetNamestring(outnmRecord.platformID, outnmRecord.encodingID, outnmRecord.languageID, 13);
	outName.vtNameRecord.push_back(outnmRecord);
	outnmRecord.nameID = 16;
	outnmRecord.strContent = inName.GetNamestring(outnmRecord.platformID, outnmRecord.encodingID, outnmRecord.languageID, 16);
	outName.vtNameRecord.push_back(outnmRecord);
	outnmRecord.nameID = 17;
	outnmRecord.strContent = strJFCN;
	outName.vtNameRecord.push_back(outnmRecord);

}	// end of bool CHYMakeJFFontDlg::MakeName(CHYFontCodec&	inDecode, CHYFontCodec&	Encode)

void CHYMakeJFFontDlg::BulidFont(CHYFontCodec& FontDecodec,char* strNewTTFFile, std::vector<GlyfData>& vtGlyfData)
{
	FILE* pCreatFile = NULL;
	int err;
	pCreatFile = fopen(strNewTTFFile, "w+b");
	if (pCreatFile == 0){
		_get_errno(&err);
		_set_errno(0);
		return;
	}

	WriteTableEntry(pCreatFile);

	for (size_t i = 0; i != m_HYTbDirectory.vtTableEntry.size(); i++)
	{
		CHYTableEntry&	HYEntry = m_HYTbDirectory.vtTableEntry[i];
		char* pTabelData = NULL;		
		if (HYEntry.tag == LOCA_TAG){
			BulidLOCAL(pCreatFile, vtGlyfData);
		}
		else if (HYEntry.tag == GLYF_TAG){
			BulidGlyph(pCreatFile, vtGlyfData);			
		}
		else if (HYEntry.tag == HEAD_TAG){
			BulidHead(FontDecodec,pCreatFile,vtGlyfData);
		}
		else if (HYEntry.tag == MAXP_TAG){
			BulidMAXP(FontDecodec, pCreatFile, vtGlyfData);
		}
		else if (HYEntry.tag == CMAP_TAG){
			BulidCMAP(pCreatFile, vtGlyfData);
		}
		else if (HYEntry.tag == HHEA_TAG){
			BulidHHEA(FontDecodec, pCreatFile, vtGlyfData);
		}
		else if (HYEntry.tag == HMTX_TAG){
			BulidHMTX(pCreatFile, vtGlyfData);
		}
		else if (HYEntry.tag == NAME_TAG) {
			Bulidname(pCreatFile);		
		}
		else if (HYEntry.tag == VHEA_TAG){
			BulidVHEA(FontDecodec, pCreatFile, vtGlyfData);
		}
		else if (HYEntry.tag == VMTX_TAG){
			BulidVMTX(pCreatFile, vtGlyfData);
		}
		else if (HYEntry.tag == POST_TAG){
			BulidPOST(FontDecodec, pCreatFile, vtGlyfData);
		}		
		else{
			int iEntryIndex = FontDecodec.m_HYTbDirectory.FindTableEntry(HYEntry.tag);
			CHYTableEntry& tbDecEntry = FontDecodec.m_HYTbDirectory.vtTableEntry[iEntryIndex];

			int iLength = (tbDecEntry.length + 3) / 4 * 4;
			char* pTabelData = new char[iLength];
			memset(pTabelData, 0, iLength);
			fseek(FontDecodec.m_pFontFile, tbDecEntry.offset, SEEK_SET);
			fread(pTabelData, tbDecEntry.length, 1, FontDecodec.m_pFontFile);

			HYEntry.length = tbDecEntry.length;
			HYEntry.offset = ftell(pCreatFile);		
			fseek(pCreatFile, HYEntry.offset, SEEK_SET);
			fwrite(pTabelData, iLength, 1, pCreatFile);			
			if (pTabelData)
				delete[] pTabelData;
		}
	}

	// 关闭源文件
	//FontDecodec.CloseFile();

	fflush(pCreatFile);
	for (int i = 0; i<m_HYTbDirectory.numTables; i++)
	{
		CHYTableEntry&	HYEntry = m_HYTbDirectory.vtTableEntry[i];
		int CheckBufSz = (HYEntry.length + 3) / 4 * 4;
		char* pCheckBuf = new char[CheckBufSz];
		memset(pCheckBuf,0, CheckBufSz);

		fseek(pCreatFile, HYEntry.offset, SEEK_SET);
		fread(pCheckBuf, CheckBufSz, 1, pCreatFile);

		HYEntry.checkSum = HY_CalcFontTableChecksum((unsigned long*)pCheckBuf, HYEntry.length);
		if (pCheckBuf) delete[] pCheckBuf;
	}
	WriteTableEntry(pCreatFile);
	// 关闭目标文件
	::fflush(pCreatFile);
	::fclose(pCreatFile);

	SetCheckSumAdjustment(strNewTTFFile);

}	// end of void CHYMakeJFFontDlg::BulidFont()

void CHYMakeJFFontDlg::DecodeTableDirectory(CHYFontCodec& FntDecodec, FILE* pFntFile)
{	
	CHYTableDirectory&  TBDirtry = FntDecodec.m_HYTbDirectory;
	TBDirtry.SetDefault();

	if (pFntFile)
	{
		// sfnt version
		fread(&TBDirtry.version.value, 2, 1, pFntFile);
		TBDirtry.version.value = hy_cdr_int16_to(TBDirtry.version.value);
		fread(&TBDirtry.version.fract, 2, 1, pFntFile);
		TBDirtry.version.fract = hy_cdr_int16_to(TBDirtry.version.fract);

		//numTables
		fread(&TBDirtry.numTables, 2, 1, pFntFile);
		TBDirtry.numTables = hy_cdr_int16_to(TBDirtry.numTables);
		// searchRange
		fread(&TBDirtry.searchRange, 2, 1, pFntFile);
		TBDirtry.searchRange = hy_cdr_int16_to(TBDirtry.searchRange);
		// entrySelector
		fread(&TBDirtry.entrySelector, 2, 1, pFntFile);
		TBDirtry.entrySelector = hy_cdr_int16_to(TBDirtry.entrySelector);
		//rangeShift
		fread(&TBDirtry.rangeShift, 2, 1, pFntFile);
		TBDirtry.rangeShift = hy_cdr_int16_to(TBDirtry.rangeShift);

		CHYTableEntry	HYEntry;
		for (int i = 0; i<TBDirtry.numTables; i++)
		{
			//tag							
			fread(&HYEntry.tag, 4, 1, pFntFile);
			HYEntry.tag = hy_cdr_int32_to(HYEntry.tag);

			// checkSum
			fread(&HYEntry.checkSum, 4, 1, pFntFile);
			HYEntry.checkSum = hy_cdr_int32_to(HYEntry.checkSum);
			//offset
			fread(&HYEntry.offset, 4, 1, pFntFile);
			HYEntry.offset = hy_cdr_int32_to(HYEntry.offset);
			//length
			fread(&HYEntry.length, 4, 1, pFntFile);
			HYEntry.length = hy_cdr_int32_to(HYEntry.length);

			TBDirtry.vtTableEntry.push_back(HYEntry);
		}	
	}

}	// end of int CHYMakeJFFontDlg::DecodeTableDirectory()

int	CHYMakeJFFontDlg::SetCheckSumAdjustment(const char* pFontFile)
{	
	CHYFontCodec FontDecodec;
	unsigned long size = ::HY_FileExist(pFontFile);
	if (size == 0)
		return HY_ERR_FILE_ZERO;
	if (size == -1)
		return HY_ERR_FILE_NOEXIST;
	if (size == -2)
		return HY_ERR_FILE_STAT;
	if (size == -3)
		return HY_ERR_FILE_OTHER;

	FILE* pFntFile = fopen(pFontFile, "r+b");
	if (pFntFile == 0) return -1;

	char* pFileBuf = new char[size];
	fread(pFileBuf, size, 1, pFntFile);

	unsigned long	ulcheckSum = HY_CalcFontTableChecksum((unsigned long*)pFileBuf, size);
	ulcheckSum = 0xB1B0AFBA - ulcheckSum;
	ulcheckSum = hy_cdr_int32_to(ulcheckSum);

	fseek(pFntFile, 0, SEEK_SET);
	DecodeTableDirectory(FontDecodec, pFntFile);
	int iEntryIndex = FontDecodec.m_HYTbDirectory.FindTableEntry(HEAD_TAG);	
	
	CHYTableEntry& tbEntry = FontDecodec.m_HYTbDirectory.vtTableEntry[iEntryIndex];
	unsigned long HeadTableOffset = tbEntry.offset + 8;
	fseek(pFntFile, HeadTableOffset, SEEK_SET);

	size_t sz = fwrite(&ulcheckSum, 4, 1, pFntFile);

	if (pFntFile){
		fflush(pFntFile);
		fclose(pFntFile);
		_set_errno(0);
	}
	pFntFile = 0;

	if (pFileBuf)
		delete[] pFileBuf;
	return HY_NOERROR;
	

}	// end of void CHYMakeJFFontDlg::SetCheckSumAdjustment()

void CHYMakeJFFontDlg::WriteTableEntry(FILE* pFile)
{
	unsigned short	searchRange = 0;
	searchRange = (unsigned short)(log((double)m_HYTbDirectory.numTables)/log(2.0));
	searchRange = (unsigned short)(pow(2.0, searchRange));
	searchRange = searchRange * 16;

	m_HYTbDirectory.searchRange = searchRange;
	m_HYTbDirectory.entrySelector = (unsigned short)(log((float)(m_HYTbDirectory.searchRange / 16)) / log(2.0));
	m_HYTbDirectory.rangeShift = m_HYTbDirectory.numTables * 16 - m_HYTbDirectory.searchRange;

	fseek(pFile, 0, SEEK_SET);

	unsigned short	usTmp;
	unsigned long	ulTmp;
	// sfnt version
	usTmp = hy_cdr_int16_to(m_HYTbDirectory.version.value);
	fwrite(&usTmp, 2, 1, pFile);
	usTmp = hy_cdr_int16_to(m_HYTbDirectory.version.fract);
	fwrite(&usTmp, 2, 1, pFile);
	//numTables
	usTmp = hy_cdr_int16_to(m_HYTbDirectory.numTables);
	fwrite(&usTmp, 2, 1, pFile);
	// searchRange
	usTmp = hy_cdr_int16_to(m_HYTbDirectory.searchRange);
	fwrite(&usTmp, 2, 1, pFile);
	// entrySelector
	usTmp = hy_cdr_int16_to(m_HYTbDirectory.entrySelector);
	fwrite(&usTmp, 2, 1, pFile);
	//rangeShift
	usTmp = hy_cdr_int16_to(m_HYTbDirectory.rangeShift);
	fwrite(&usTmp, 2, 1, pFile);
	for (int i = 0; i<m_HYTbDirectory.numTables; i++){
		CHYTableEntry&	HYEntry = m_HYTbDirectory.vtTableEntry[i];
		//tag		
		ulTmp = hy_cdr_int32_to(HYEntry.tag);
		fwrite(&ulTmp, 4, 1, pFile);
		// checkSum
		ulTmp = hy_cdr_int32_to(HYEntry.checkSum);
		fwrite(&ulTmp, 4, 1, pFile);
		//offset
		ulTmp = hy_cdr_int32_to(HYEntry.offset);
		fwrite(&ulTmp, 4, 1, pFile);
		//length
		ulTmp = hy_cdr_int32_to(HYEntry.length);
		fwrite(&ulTmp, 4, 1, pFile);
	}

}	// end of void CHYMakeJFFontDlg::WriteTableEntry()

void CHYMakeJFFontDlg::BulidLOCAL(FILE* pCreatFile, std::vector<GlyfData>& vtGlyfData)
{	 
	int iEntryIndex = m_HYTbDirectory.FindTableEntry(LOCA_TAG);
	CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
	tbEntry.offset = ftell(pCreatFile);
	fseek(pCreatFile, tbEntry.offset, SEEK_SET);	
	
	long loffset = 0, ulGlyphOffset = 0;
	size_t st = vtGlyfData.size();
	for (size_t i = 0; i != st; i++){
		loffset = hy_cdr_int32_to(ulGlyphOffset);
		fwrite(&loffset, 4, 1, pCreatFile);

		ulGlyphOffset += vtGlyfData[i].pDataLen;
	}

	loffset = hy_cdr_int32_to(ulGlyphOffset);
	fwrite(&loffset, 4, 1, pCreatFile);

	fflush(pCreatFile);

	tbEntry.length = ftell(pCreatFile) - tbEntry.offset;	
	
}	// end of void CHYMakeJFFontDlg::BulidLOCAL()

void CHYMakeJFFontDlg::BulidGlyph(FILE* pCreatFile, std::vector<GlyfData>& vtGlyfData)
{
	int iEntryIndex = m_HYTbDirectory.FindTableEntry(GLYF_TAG);
	CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
	tbEntry.offset = ftell(pCreatFile);
	fseek(pCreatFile, tbEntry.offset, SEEK_SET);

	size_t st = vtGlyfData.size();
	char* pError = 0;
	for (size_t i = 0; i != st; i++){
		for (size_t j = 0; j < vtGlyfData[i].pDataLen; j++)	{	
			int iret = fwrite(&vtGlyfData[i].vtData[j], 1, 1, pCreatFile);
			if (iret < 1){			
				pError = strerror(errno);
			}
		}
	}

	fflush(pCreatFile);

	tbEntry.length = ftell(pCreatFile) - tbEntry.offset;
	int iTail = 4 - tbEntry.length % 4;
	if (tbEntry.length % 4>0){
		char c = 0;
		for (int i = 0; i<iTail; i++){
			fwrite(&c, 1, 1, pCreatFile);
		}
	}

}	// end of void CHYMakeJFFontDlg::BulidGlyph()

void CHYMakeJFFontDlg::BulidHead(CHYFontCodec& FontCodec, FILE* pCreatFile, std::vector<GlyfData>& vtGlyfData)
{
	int iEntryIndex = m_HYTbDirectory.FindTableEntry(HEAD_TAG);
	CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
	tbEntry.offset = ftell(pCreatFile);
	fseek(pCreatFile, tbEntry.offset, SEEK_SET);

	//Table version number
	unsigned short usTmp = 0;
	HYFIXED version;
	version.value = 1;
	version.fract = 0;
	usTmp = hy_cdr_int16_to(version.value);
	fwrite(&usTmp, 2, 1, pCreatFile);
	usTmp = hy_cdr_int16_to(version.fract);
	fwrite(&usTmp, 2, 1, pCreatFile);

	//fontRevision			
	usTmp = hy_cdr_int16_to(version.value);
	fwrite(&usTmp, 2, 1, pCreatFile);
	usTmp = hy_cdr_int16_to(version.fract);
	fwrite(&usTmp, 2, 1, pCreatFile);

	unsigned long ulTmp = 0;
	fwrite(&ulTmp, 4, 1, pCreatFile);

	// magicNumber	
	ulTmp = hy_cdr_int32_to(0x5F0F3CF5);
	fwrite(&ulTmp, 4, 1, pCreatFile);
	//flags
	usTmp = hy_cdr_int16_to(FontCodec.m_HYhead.flags);
	fwrite(&usTmp, 2, 1, pCreatFile);
	//unitsPerEm
	usTmp = hy_cdr_int16_to(FontCodec.m_HYhead.unitsPerEm);
	fwrite(&usTmp, 2, 1, pCreatFile);

	__time64_t	 tspace = 2082844799;
	CTime tCurTime = CTime::GetCurrentTime();
	__time64_t  t64CurTime = tCurTime.GetTime();
	t64CurTime += tspace;
	char p[8] = { 0 };
	CopyMemory((PBYTE)p, &t64CurTime, sizeof(__time64_t));

	char created[8] = { 0 };
	char modified[8] = { 0 };

	//created			
	fwrite(created, 8, 1, pCreatFile);
	//modified
	fwrite(modified, 8, 1, pCreatFile);

	short xMin=0, yMin=0, xMax=0, yMax=0;
	CountFontBound(xMin, yMin, xMax, yMax,vtGlyfData);

	usTmp = hy_cdr_int16_to(xMin);
	fwrite(&usTmp, 2, 1, pCreatFile);	
	usTmp = hy_cdr_int16_to(yMin);
	fwrite(&usTmp, 2, 1, pCreatFile);	
	usTmp = hy_cdr_int16_to(xMax);
	fwrite(&usTmp, 2, 1, pCreatFile);	
	usTmp = hy_cdr_int16_to(yMax);
	fwrite(&usTmp, 2, 1, pCreatFile);

	// macStyle
	usTmp = hy_cdr_int16_to(FontCodec.m_HYhead.macStyle);
	fwrite(&usTmp, 2, 1, pCreatFile);
	//lowestRecPPEM
	usTmp = hy_cdr_int16_to(FontCodec.m_HYhead.lowestRecPPEM);
	fwrite(&usTmp, 2, 1, pCreatFile);
	//fontDirectionHint
	usTmp = hy_cdr_int16_to(FontCodec.m_HYhead.fontDirectionHint);
	fwrite(&usTmp, 2, 1, pCreatFile);
	//indexToLocFormat	
	usTmp = hy_cdr_int16_to(FontCodec.m_HYhead.indexToLocFormat);
	fwrite(&usTmp, 2, 1, pCreatFile);
	//glyphDataFormat	
	usTmp = hy_cdr_int16_to(FontCodec.m_HYhead.glyphDataFormat);
	fwrite(&usTmp, 2, 1, pCreatFile);

	tbEntry.length = ftell(pCreatFile) - tbEntry.offset;
	int iTail = 4 - tbEntry.length % 4;
	if (tbEntry.length % 4>0)
	{
		char c = 0;
		for (int i = 0; i<iTail; i++)
		{
			fwrite(&c, 1, 1, pCreatFile);
		}
	}

}	// end of void CHYMakeJFFontDlg::BulidHead()

void CHYMakeJFFontDlg::BulidMAXP(CHYFontCodec& FontCodec, FILE* pCreatFile, std::vector<GlyfData>& vtGlyfData)
{	
	unsigned short	usTmp = 0;
	unsigned long	ulTmp = 0;

	int iEntryIndex = m_HYTbDirectory.FindTableEntry(MAXP_TAG);
	if (iEntryIndex == -1) return;

	CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
	tbEntry.offset = ftell(pCreatFile);
	fseek(pCreatFile, tbEntry.offset, SEEK_SET);

	unsigned short	usGlyphNum = (unsigned short)vtGlyfData.size();
	//Table version number	
	usTmp = hy_cdr_int16_to(FontCodec.m_HYMaxp.version.value);
	fwrite(&usTmp, 2, 1, pCreatFile);	
	fwrite(&FontCodec.m_HYMaxp.version.fract, 2, 1, pCreatFile);

	//numGlyphs	
	usTmp = hy_cdr_int16_to(usGlyphNum);
	fwrite(&usTmp, 2, 1, pCreatFile);

	long MaxTContours=0;
	for (unsigned short i = 0; i < usGlyphNum; i++) {		
		if (MaxTContours < (long)vtGlyfData[i].CnturNum)
			MaxTContours = (long)vtGlyfData[i].CnturNum;
	}
	//maxPoints		
	usTmp = hy_cdr_int16_to(FontCodec.m_HYMaxp.maxPoints);
	fwrite(&usTmp, 2, 1, pCreatFile);
	//maxContours		
	usTmp = hy_cdr_int16_to((unsigned short)MaxTContours);
	fwrite(&usTmp, 2, 1, pCreatFile);
	//maxCompositePoints				
	usTmp = hy_cdr_int16_to((unsigned short)FontCodec.m_HYMaxp.maxCompositePoints);
	fwrite(&usTmp, 2, 1, pCreatFile);
	//maxCompositeContours
	usTmp = hy_cdr_int16_to((unsigned short)FontCodec.m_HYMaxp.maxCompositeContours);
	fwrite(&usTmp, 2, 1, pCreatFile);
	//maxZones
	usTmp = hy_cdr_int16_to(2);
	fwrite(&usTmp, 2, 1, pCreatFile);
	//maxTwilightPoints
	usTmp = hy_cdr_int16_to(4);
	fwrite(&usTmp, 2, 1, pCreatFile);
	//maxStorage
	usTmp = hy_cdr_int16_to(32);
	fwrite(&usTmp, 2, 1, pCreatFile);
	//maxFunctionDefs
	usTmp = hy_cdr_int16_to(96);
	fwrite(&usTmp, 2, 1, pCreatFile);
	//maxInstructionDefs
	usTmp = hy_cdr_int16_to(96);
	fwrite(&usTmp, 2, 1, pCreatFile);
	//maxStackElements
	usTmp = hy_cdr_int16_to(256);
	fwrite(&usTmp, 2, 1, pCreatFile);
	//maxSizeOfInstructions
	usTmp = hy_cdr_int16_to(8);
	fwrite(&usTmp, 2, 1, pCreatFile);
	//maxComponentElements				
	usTmp = hy_cdr_int16_to(unsigned short(FontCodec.m_HYMaxp.maxComponentElements));
	fwrite(&usTmp, 2, 1, pCreatFile);
	//maxComponentDepth
	usTmp = hy_cdr_int16_to(unsigned short(FontCodec.m_HYMaxp.maxComponentDepth));
	fwrite(&usTmp, 2, 1, pCreatFile);	

	tbEntry.length = ftell(pCreatFile) - tbEntry.offset;
	int iTail = 4 - tbEntry.length % 4;
	if (tbEntry.length % 4>0){
		char c = 0;
		for (int i = 0; i<iTail; i++){
			fwrite(&c, 1, 1, pCreatFile);
		}
	}	

}	// end of void	CHYMakeJFFontDlg::BulidMAXP()

void CHYMakeJFFontDlg::BulidCMAP(FILE* pCreatFile, std::vector<GlyfData>& vtGlyfData)
{
	unsigned long	ulTmp = 0;
	unsigned short  usTmp = 0;

	int iEntryIndex = m_HYTbDirectory.FindTableEntry(CMAP_TAG);
	if (iEntryIndex == -1)  return;

	CHYTableEntry& tbTableEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
	tbTableEntry.offset = ftell(pCreatFile);
	fseek(pCreatFile, tbTableEntry.offset, SEEK_SET);

	usTmp = hy_cdr_int16_to(m_HYNewCmap.version);
	fwrite(&usTmp, 2, 1, pCreatFile);
	usTmp = hy_cdr_int16_to(m_HYNewCmap.numSubTable);
	fwrite(&usTmp, 2, 1, pCreatFile);

	int i = 0;
	for (i = 0; i<m_HYNewCmap.numSubTable; i++){
		CMAP_TABLE_ENTRY&	tbEntry = m_HYNewCmap.vtCamp_tb_entry[i];	

		// platformID
		usTmp = hy_cdr_int16_to(tbEntry.plat_ID);
		fwrite(&usTmp, 2, 1, pCreatFile);
		// encodingID
		usTmp = hy_cdr_int16_to(tbEntry.plat_encod_ID);
		fwrite(&usTmp, 2, 1, pCreatFile);
		// offset				
		tbEntry.offset = tbTableEntry.offset;
		ulTmp = hy_cdr_int32_to(tbEntry.offset);
		fwrite(&ulTmp, 4, 1, pCreatFile);
	}

	std::vector<char> vtCmp0;
	std::vector<char> vtCmp2;
	std::vector<char> vtCmp4;
	std::vector<char> vtCmp6;
	std::vector<char> vtCmp8;
	std::vector<char> vtCmp10;
	std::vector<char> vtCmp12;
	std::vector<char> vtCmp13;
	std::vector<char> vtCmp14;

	for (i = 0; i<m_HYNewCmap.numSubTable; i++) {
		CMAP_TABLE_ENTRY&	tbEntry = m_HYNewCmap.vtCamp_tb_entry[i];
		switch (tbEntry.format)	{
			case CMAP_ENCODE_FT_0:
				EncodeCmapFmt0(pCreatFile, tbEntry, vtCmp0);
				break;		
			case CMAP_ENCODE_FT_4:
				EncodeCmapFmt4(pCreatFile, tbEntry, vtCmp4);			
				break;
			case CMAP_ENCODE_FT_12:
				EncodeCmapFmt12(pCreatFile, tbEntry, vtCmp12);
				break;
			default:
				break;
		}
	}

	unsigned long ulCmapEndPos = ftell(pCreatFile);
	fflush(pCreatFile);

	fseek(pCreatFile, tbTableEntry.offset + 4, SEEK_SET);
	for (i = 0; i<m_HYNewCmap.numSubTable; i++){
		CMAP_TABLE_ENTRY&	tbEntry = m_HYNewCmap.vtCamp_tb_entry[i];
		usTmp = hy_cdr_int16_to(tbEntry.plat_ID);
		fwrite(&usTmp, 2, 1, pCreatFile);
		usTmp = hy_cdr_int16_to(tbEntry.plat_encod_ID);
		fwrite(&usTmp, 2, 1, pCreatFile);
		ulTmp = hy_cdr_int32_to(tbEntry.offset);
		fwrite(&ulTmp, 4, 1, pCreatFile);
	}

	fflush(pCreatFile);
	fseek(pCreatFile, ulCmapEndPos, SEEK_SET);

	tbTableEntry.length = ftell(pCreatFile) - tbTableEntry.offset;
	int iTail = tbTableEntry.length % 4;
	if (iTail>0){
		char c = 0;
		for (int i = 0; i<iTail; i++){
			fwrite(&c, 1, 1, pCreatFile);
		}
	}

}	// end of void CHYMakeJFFontDlg::BulidCMAP()

void CHYMakeJFFontDlg::EncodeCmapFmt0(FILE* pFontFile, CMAP_TABLE_ENTRY& entry, std::vector<char>& vtCmap)
{
	if (pFontFile){
		if (vtCmap.size() == 0)	{
			CMAP_ENCODE_FORMAT_0&		CEF0 = entry.Format0;
			CEF0.SetDefault();

			CEF0.format = CMAP_ENCODE_FT_0;
			vtCmap.push_back(CEF0.format >> 8);
			vtCmap.push_back(CEF0.format & 0x00ff);
			// length
			CEF0.length = 262;
			vtCmap.push_back(CEF0.length >> 8);
			vtCmap.push_back(CEF0.length & 0x00ff);
			// language
			CEF0.language = 0;
			vtCmap.push_back(CEF0.language >> 8);
			vtCmap.push_back(CEF0.language & 0x00ff);
			// glyphIdArray
			if (m_HYCodeMap.vtHYCodeMap.size() >= 256){
				// 第一个必须是GID == 0;
				vtCmap.push_back(0);
				vtCmap.push_back(0);

				for (int i = 1; i<256; i++){
					std::vector<int> vtGIDs;
					int iGID = m_HYCodeMap.FindIndexByUnicode(i);
					if (iGID>0 && iGID<256){
						vtCmap.push_back(iGID >> 8);
						vtCmap.push_back(iGID & 0x0f);
					}
					else{
						vtCmap.push_back(0);
						vtCmap.push_back(0);
					}
				}
			}
		}

		entry.offset = ftell(pFontFile) - entry.offset;
		for (size_t i = 0; i<vtCmap.size(); i++){
			fwrite(&vtCmap[i], 1, 1, pFontFile);
		}
	}

}	// end of int CHYMakeJFFontDlg::EncodeCmapFmt0()

void CHYMakeJFFontDlg::EncodeCmapFmt4(FILE* pFontFile, CMAP_TABLE_ENTRY& entry, std::vector<char>& vtCmap)
{
	entry.Format4.SetDefault();

	if (pFontFile){
		if (vtCmap.size() == 0)	{
			std::list<GroupRecord>		groupList;
			int		eov = m_HYCodeMap.vtHYCodeMap.size();
			int		ix = 0;

			while (ix != eov && m_HYCodeMap.vtHYCodeMap[ix].ulGlyphNo <= 0xffff) {
				GroupRecord				group;
				int						nextIx = NextGroup(ix, eov, group);

				if (group.startCode < 0xffff) {	// This format does not code above 0xfffe
					if (group.endCode > 0xfffe)
						group.endCode = 0xfffe;

					groupList.push_back(group);
				}
				ix = nextIx;
			}

			std::list <GroupRecord>::iterator		gix = groupList.begin();
			std::list <GroupRecord>::iterator		glast = groupList.begin();
			if (gix != groupList.end())
				++gix;

			std::list <GroupRecord>::iterator		gend = groupList.end();
			ULONG									lastCode = 0;
			int										entryCount = 0;
			int										groupCount = 0;

			gix = groupList.begin();
			while (gix != gend)	{
				if (!gix->contiguous)
					entryCount += gix->endCode - gix->startCode + 1;

				lastCode = gix->endCode;
				++gix;
				groupCount += 1;
			}

			if (lastCode != 0xffff)		// We are required to terminate the table with a group containing 0xffff
				groupCount += 1;

			size_t length = 2 * (8 + groupCount * 4 + entryCount);
			if (length>65535) return;
				

			std::vector <unsigned short>	startTable(groupCount);
			std::vector <unsigned short>	endTable(groupCount);
			std::vector <short>				idDelta(groupCount);
			std::vector <unsigned short>	idRangeOffset(groupCount);
			std::vector <unsigned short>	glyphIdArray;
			size_t							groupIx = 0;

			entryCount = 0;
			gix = groupList.begin();

			while (gix != gend)	{
				size_t				entries = gix->endCode - gix->startCode + 1;
				startTable[groupIx] = (unsigned short)gix->startCode;
				endTable[groupIx] = (unsigned short)gix->endCode;

				if (gix->contiguous){	
					// If the glyphIDs in the range are consecutive then we don't need entries in the GlyphIdArray.
					idDelta[groupIx] = (short)gix->startGID - (short)gix->startCode;
					idRangeOffset[groupIx] = 0xffff;
				}
				else{
					//Theoretically the table can be compressed by finding multiple ranges with the same differences between each
					// code point in the range and the corresponding glyphID, and then adjusting the bias via idDelta. We're not going to
					// bother. It's computationally expensive and probably doesn't yield much savings for most circumstances.
					idDelta[groupIx] = 0;
					idRangeOffset[groupIx] = (unsigned short)entryCount;

					unsigned long		code = gix->startCode;
					for (size_t i = 0; i<entries; ++i){
						int iGID = m_HYCodeMap.FindIndexByUnicode(code++);
						if (iGID != -1)
							glyphIdArray.push_back((unsigned short)iGID);
						else
							glyphIdArray.push_back(0);
					}

					if (entryCount + entries > 0xffff) 
						return;

					entryCount += static_cast <unsigned short> (entries);
				}
				++groupIx;
				++gix;
			}

			if (lastCode != 0xffff){
				startTable[groupIx] = 0xffff;
				endTable[groupIx] = 0xffff;
				idDelta[groupIx] = 1;
				idRangeOffset[groupIx] = 0xffff;
			}

			//format
			entry.Format4.format = CMAP_ENCODE_FT_4;
			vtCmap.push_back(entry.Format4.format >> 8);
			vtCmap.push_back(entry.Format4.format & 0x00ff);

			//length
			entry.Format4.length = (unsigned short)length;
			vtCmap.push_back(entry.Format4.length >> 8);
			vtCmap.push_back(entry.Format4.length & 0x00ff);

			//language
			entry.Format4.language = 0;
			vtCmap.push_back(0);
			vtCmap.push_back(0);

			//segCountX2
			entry.Format4.segCountX2 = (unsigned short)groupCount * 2;
			vtCmap.push_back(entry.Format4.segCountX2 >> 8);
			vtCmap.push_back(entry.Format4.segCountX2 & 0x00ff);

			//searchRange
			entry.Format4.searchRange = (unsigned short)(2 * pow(2, floor((log((double)groupCount) / log(2.0)))));
			vtCmap.push_back(entry.Format4.searchRange >> 8);
			vtCmap.push_back(entry.Format4.searchRange & 0x00ff);

			//entrySelector
			entry.Format4.entrySelector = (unsigned short)(log(entry.Format4.searchRange / 2.0) / log(2.0));
			vtCmap.push_back(entry.Format4.entrySelector >> 8);
			vtCmap.push_back(entry.Format4.entrySelector & 0x00ff);

			//rangeShift
			entry.Format4.rangeShift = entry.Format4.segCountX2 - entry.Format4.searchRange;
			vtCmap.push_back(entry.Format4.rangeShift >> 8);
			vtCmap.push_back(entry.Format4.rangeShift & 0x00ff);

			//endCount
			unsigned short usTmp;
			for (int i = 0; i<groupCount; i++){
				usTmp = endTable[i];
				vtCmap.push_back(usTmp >> 8);
				vtCmap.push_back(usTmp & 0x00ff);
			}

			//reservedPad
			entry.Format4.reservedPad = 0;
			vtCmap.push_back(0);
			vtCmap.push_back(0);

			//startCount
			for (int i = 0; i<groupCount; i++){
				usTmp = startTable[i];

				vtCmap.push_back(usTmp >> 8);
				vtCmap.push_back(usTmp & 0x00ff);
			}

			//idDelta
			for (int i = 0; i<groupCount; i++){
				usTmp = idDelta[i];

				vtCmap.push_back(usTmp >> 8);
				vtCmap.push_back(usTmp & 0x00ff);
			}

			//idRangeOffset
			for (int i = 0; i<groupCount; i++){
				if (idRangeOffset[i] == 0xffff){
					usTmp = 0;
				}
				else{
					usTmp = static_cast <unsigned short> (2 * (idRangeOffset[i] + groupCount - i));
				}

				vtCmap.push_back(usTmp >> 8);
				vtCmap.push_back(usTmp & 0x00ff);
			}

			//glyphIdArray
			for (size_t i = 0; i < glyphIdArray.size(); i++){
				usTmp = glyphIdArray[i];
				vtCmap.push_back(usTmp >> 8);
				vtCmap.push_back(usTmp & 0x00ff);
			}
		}

		entry.offset = ftell(pFontFile) - entry.offset;
		for (size_t i = 0; i<vtCmap.size(); i++){
			fwrite(&vtCmap[i], 1, 1, pFontFile);
		}
	}

}	// end of int CHYMakeJFFontDlg::EncodeCmapFmt4()

int CHYMakeJFFontDlg::NextGroup(int ix, int eov, GroupRecord& group)
{
	int				entryCount = 0;
	bool			haveContiguous = false;		// Neither contiguous nor discontiguous yet
	bool			contiguous = true;			// Default to true for single glyph range.

	std::vector<CHYCodeMapItem>& vtMapItm = m_HYCodeMap.vtHYCodeMap;
	if (ix != eov){
		unsigned long 		startCharCode = vtMapItm[ix].ulGlyphNo;
		int					startGID = vtMapItm[ix].iGlyphIndex;
		int					hold = ix;
		++ix;
		++entryCount;

		group.startCode = startCharCode;
		group.startGID = startGID;
		group.endCode = startCharCode;

		while (ix != eov && vtMapItm[ix].ulGlyphNo == startCharCode + 1){
			int			gid = vtMapItm[ix].iGlyphIndex;
			bool		successive = gid == startGID + 1;

			startCharCode = vtMapItm[ix].ulGlyphNo;
			++group.endCode;

			if (haveContiguous){
				if (successive != contiguous){
					ix = hold;
					entryCount -= 1;
					group.endCode -= 1;
					break;
				}
			}
			else
				contiguous = successive;

			haveContiguous = true;
			startGID = gid;

			hold = ++ix;
			++entryCount;
		}
	}

	group.contiguous = contiguous;
	return ix;

}	// end of CHYMakeJFFontDlg::NextGroup() 

void CHYMakeJFFontDlg::EncodeCmapFmt12(FILE* pFontFile, CMAP_TABLE_ENTRY& entry, std::vector<char>& vtCmap)
{
	entry.Format12.SetDefault();
	if (pFontFile)	{
		if (vtCmap.size() == 0)		{
			//std::vector<CMAP_ENCODE_F12_GROUP> sztagF12Group;
			int iGlyphIndex = -1, iExpectGlyphIndex = -1;
			ULONG ulUnicode = 0, ulExpectUnicode = 0;
			size_t i = 0, j = 0;

			size_t iCharNumber = m_HYCodeMap.vtHYCodeMap.size();
			while (i<iCharNumber){
				CMAP_ENCODE_FORMAT_12_GROUP	 tagF12Group;
				CHYCodeMapItem& HYMapItem = m_HYCodeMap.vtHYCodeMap[i];
				iGlyphIndex = HYMapItem.iGlyphIndex;
				ulUnicode = HYMapItem.ulGlyphNo;

				tagF12Group.startCharCode = ulUnicode;
				tagF12Group.startGlyphID = iGlyphIndex;

				for (j = i + 1; j<iCharNumber; j++)	{
					CHYCodeMapItem& HYMapItem = m_HYCodeMap.vtHYCodeMap[j];
					iExpectGlyphIndex = HYMapItem.iGlyphIndex;
					ulExpectUnicode = HYMapItem.ulGlyphNo;

					if (iExpectGlyphIndex == 0) continue;

					iGlyphIndex++;
					ulUnicode++;

					// Index和unicode都必须是连续的,否则段结束
					if (iExpectGlyphIndex != iGlyphIndex || ulExpectUnicode != ulUnicode){
						tagF12Group.endCharCode = ulUnicode - 1;
						entry.Format12.vtGroup.push_back(tagF12Group);
						i = j;
						break;
					}
				}

				// 数组末尾
				if (iCharNumber == 1){
					tagF12Group.startCharCode = 65535;
					tagF12Group.endCharCode = 65535;
					tagF12Group.startGlyphID = 0;
					entry.Format12.vtGroup.push_back(tagF12Group);
					break;
				}
				else if (j == iCharNumber){
					if (tagF12Group.endCharCode == 0){
						if (m_HYCodeMap.vtHYCodeMap[j - 1].iGlyphIndex == 0)
							tagF12Group.endCharCode = m_HYCodeMap.vtHYCodeMap[j - 2].ulGlyphNo;
						else
							tagF12Group.endCharCode = m_HYCodeMap.vtHYCodeMap[j - 1].ulGlyphNo;

						entry.Format12.vtGroup.push_back(tagF12Group);
					}
					break;
				}
			}

			// 向CMAP_ENCODE_F12赋值	
			entry.Format12.format = CMAP_ENCODE_FT_12;
			vtCmap.push_back((entry.Format12.format & 0xff00) >> 8);
			vtCmap.push_back(entry.Format12.format & 0x00ff);

			entry.Format12.reserved = 0;
			vtCmap.push_back(0);
			vtCmap.push_back(0);

			// 16 == format + reserved + length + language + nGroups
			// 12 == startCharCode + endCharCode + startGlyphID
			entry.Format12.length = 16 + entry.Format12.vtGroup.size() * 12;
			vtCmap.push_back((entry.Format12.length & 0xff000000) >> 24);
			vtCmap.push_back((entry.Format12.length & 0x00ff0000) >> 16);
			vtCmap.push_back((entry.Format12.length & 0x0000ff00) >> 8);
			vtCmap.push_back(entry.Format12.length & 0x000000ff);

			entry.Format12.language = 0;
			vtCmap.push_back(0);
			vtCmap.push_back(0);
			vtCmap.push_back(0);
			vtCmap.push_back(0);

			entry.Format12.nGroups = (unsigned long)entry.Format12.vtGroup.size();
			vtCmap.push_back((entry.Format12.nGroups & 0xff000000) >> 24);
			vtCmap.push_back((entry.Format12.nGroups & 0x00ff0000) >> 16);
			vtCmap.push_back((entry.Format12.nGroups & 0x0000ff00) >> 8);
			vtCmap.push_back(entry.Format12.nGroups & 0x000000ff);

			unsigned long ulTmp = 0;
			size_t stFrmt12 = entry.Format12.vtGroup.size();
			for (size_t i = 0; i<stFrmt12; i++){
				std::vector<CMAP_ENCODE_FORMAT_12_GROUP>& vtGroups = entry.Format12.vtGroup;

				ulTmp = vtGroups[i].startCharCode;
				vtCmap.push_back((ulTmp & 0xff000000) >> 24);
				vtCmap.push_back((ulTmp & 0x00ff0000) >> 16);
				vtCmap.push_back((ulTmp & 0x0000ff00) >> 8);
				vtCmap.push_back(ulTmp & 0x000000ff);

				ulTmp = vtGroups[i].endCharCode;
				vtCmap.push_back((ulTmp & 0xff000000) >> 24);
				vtCmap.push_back((ulTmp & 0x00ff0000) >> 16);
				vtCmap.push_back((ulTmp & 0x0000ff00) >> 8);
				vtCmap.push_back(ulTmp & 0x000000ff);

				ulTmp = vtGroups[i].startGlyphID;
				vtCmap.push_back((ulTmp & 0xff000000) >> 24);
				vtCmap.push_back((ulTmp & 0x00ff0000) >> 16);
				vtCmap.push_back((ulTmp & 0x0000ff00) >> 8);
				vtCmap.push_back(ulTmp & 0x000000ff);				
			}
		}

		entry.offset = ftell(pFontFile) - entry.offset;
		for (size_t i = 0; i<vtCmap.size(); i++){
			fwrite(&vtCmap[i], 1, 1, pFontFile);
		}
	}

}	// end of int CHYMakeJFFontDlg::EncodeCmapFmt12()

void CHYMakeJFFontDlg::MakeHYCodeMap(std::vector<GlyfData>& vtGlyfData)
{
	m_HYCodeMap.vtHYCodeMap.clear();

	CHYCodeMapItem  mapItm;
	size_t	stGlyphNum = vtGlyfData.size();
	for (size_t i = 0; i<stGlyphNum; i++){
		std::vector<unsigned  long>& vtUnicode = vtGlyfData[i].vtUnicode;
		size_t stUnicodeNum = vtUnicode.size();

		for (size_t j = 0; j<stUnicodeNum; j++)	{
			CHYCodeMapItem  mapItm;
			mapItm.ulGlyphNo = vtUnicode[j];
			mapItm.iGlyphIndex = i;
			m_HYCodeMap.vtHYCodeMap.push_back(mapItm);
		}
	}

	m_HYCodeMap.QuickSortbyUnicode();

}	// end of int CHYMakeJFFontDlg::MakeHYCodeMap()

void CHYMakeJFFontDlg::BulidHHEA(CHYFontCodec& FontCodec, FILE* pCreatFile, std::vector<GlyfData>& vtGlyfData)
{	
	unsigned short	usTmp = 0;
	unsigned long	ulTmp = 0;

	int iEntryIndex = m_HYTbDirectory.FindTableEntry(HHEA_TAG);
	if (iEntryIndex == -1) return;

	CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
	tbEntry.offset = ftell(pCreatFile);
	fseek(pCreatFile, tbEntry.offset, SEEK_SET);

	// version 	
	usTmp = hy_cdr_int16_to(FontCodec.m_HYHhea.version.value);
	fwrite(&usTmp, 2, 1, pCreatFile);	
	usTmp = hy_cdr_int16_to(FontCodec.m_HYHhea.version.fract);
	fwrite(&usTmp, 2, 1, pCreatFile);
	//Ascender	
	usTmp = hy_cdr_int16_to(FontCodec.m_HYHhea.Ascender);
	fwrite(&usTmp, 2, 1, pCreatFile);
	//Descender	
	usTmp = hy_cdr_int16_to(FontCodec.m_HYHhea.Descender);
	fwrite(&usTmp, 2, 1, pCreatFile);
	//LineGap			
	usTmp = hy_cdr_int16_to(FontCodec.m_HYHhea.LineGap);
	fwrite(&usTmp, 2, 1, pCreatFile);
	//advanceWidthMax			
	usTmp = hy_cdr_int16_to(GetAdvancMaxWidth(vtGlyfData));
	fwrite(&usTmp, 2, 1, pCreatFile);
	//minLeftSideBearing	
	usTmp = hy_cdr_int16_to(FindMinLeftSideBearing(vtGlyfData));
	fwrite(&usTmp, 2, 1, pCreatFile);
	//minRightSideBearing
	usTmp = hy_cdr_int16_to(FindMinRightSideBearing(vtGlyfData));
	fwrite(&usTmp, 2, 1, pCreatFile);
	//xMaxExtent
	short xMaxExtent = FindMinLeftSideBearing(vtGlyfData) + (FindXmax(vtGlyfData) - FindXmin(vtGlyfData));
	usTmp = hy_cdr_int16_to(xMaxExtent);
	fwrite(&usTmp, 2, 1, pCreatFile);
	//caretSlopeRise		
	usTmp = hy_cdr_int16_to(1);
	fwrite(&usTmp, 2, 1, pCreatFile);
	//caretSlopeRun
	usTmp = 0;
	fwrite(&usTmp, 2, 1, pCreatFile);
	//caretOffset
	usTmp = 0;
	fwrite(&usTmp, 2, 1, pCreatFile);
	usTmp = 0;
	//reserved1			
	fwrite(&usTmp, 2, 1, pCreatFile);
	//reserved2			
	fwrite(&usTmp, 2, 1, pCreatFile);
	//reserved3			
	fwrite(&usTmp, 2, 1, pCreatFile);
	//reserved4			
	fwrite(&usTmp, 2, 1, pCreatFile);
	//metricDataFormat
	usTmp = 0;
	fwrite(&usTmp, 2, 1, pCreatFile);
	//numberOfHMetrics
	size_t stGlyphs = vtGlyfData.size();
	int iGlyphNum = (int)stGlyphs;
	unsigned short numberOfHMetrics = 0;
	if (iGlyphNum>0){
		int	iBaseadvanceWidth = vtGlyfData[--iGlyphNum].usAdvWidth;
		while (--iGlyphNum >= 0){
			if (vtGlyfData[iGlyphNum].usAdvWidth == iBaseadvanceWidth)
				numberOfHMetrics++;
			else
				break;
		}
		numberOfHMetrics = (unsigned short)stGlyphs - numberOfHMetrics;
	}
	usTmp = hy_cdr_int16_to(numberOfHMetrics);
	fwrite(&usTmp, 2, 1, pCreatFile);

	tbEntry.length = ftell(pCreatFile) - tbEntry.offset;
	int iTail = 4 - tbEntry.length % 4;
	if (tbEntry.length % 4>0){
		char c = 0;
		for (int i = 0; i<iTail; i++){
			fwrite(&c, 1, 1, pCreatFile);
		}
	}

}	// end of void CHYMakeJFFontDlg::BulidHHEA()

int CHYMakeJFFontDlg::FindXmin(std::vector<GlyfData>& vtGlyfData)
{
	int iXmin = 0;
	size_t st = vtGlyfData.size();
	if (st > 0){
		iXmin = vtGlyfData[0].xMin;		
		for (size_t i = 1; i<st; i++){
			if (vtGlyfData[i].xMin<iXmin){
				iXmin = vtGlyfData[i].xMin;				
			}
		}		
	}

	return iXmin;

}	// end of int CHYMakeJFFontDlg::FindXmin()

int	CHYMakeJFFontDlg::FindXmax(std::vector<GlyfData>& vtGlyfData)
{
	int iXmax = 0;
	size_t st = vtGlyfData.size();
	if (st > 0)	{	
		iXmax = vtGlyfData[0].xMax;
		for (size_t i = 1; i<st; i++){
			if (vtGlyfData[i].xMax>iXmax){
				iXmax = vtGlyfData[i].xMax;
			}
		}	
	}

	return iXmax;

}	// end of int CHYMakeJFFontDlg::FindXmax()

int	CHYMakeJFFontDlg::FindYmin(std::vector<GlyfData>& vtGlyfData)
{
	int iYmin = 0;
	size_t st = vtGlyfData.size();
	if (st > 0){		
		iYmin = vtGlyfData[0].yMin;
		for (size_t i = 1; i<st; i++){
			if (vtGlyfData[i].yMin<iYmin){
				iYmin = vtGlyfData[i].yMin;				
			}
		}		
	}

	return iYmin;

}	// end of int CHYMakeJFFontDlg::FindYmin()

int	CHYMakeJFFontDlg::FindYmax(std::vector<GlyfData>& vtGlyfData)
{
	int iYmax = 0;
	size_t st = vtGlyfData.size();
	if (st > 0)	{	
		iYmax = vtGlyfData[0].yMax;
		for (size_t i = 1; i<st; i++){
			if (vtGlyfData[i].yMax>iYmax){
				iYmax = vtGlyfData[i].yMax;				
			}
		}
	}

	return iYmax;

}	// end of int CHYMakeJFFontDlg::FindYmax()

int CHYMakeJFFontDlg::FindMinLeftSideBearing(std::vector<GlyfData>& vtGlyfData)
{
	size_t stGlyphNum = vtGlyfData.size();
	if (stGlyphNum == 0) return -1;
	short minLSB = vtGlyfData[0].xMin;
	for (size_t i = 0; i<stGlyphNum; i++) {
		short tmp = vtGlyfData[i].xMin;
		if (tmp<minLSB) {
			minLSB = tmp;
		}
	}
	return minLSB;

}	// end of int CHYMakeJFFontDlg::FindMinLeftSideBearing()

int CHYMakeJFFontDlg::FindMinRightSideBearing(std::vector<GlyfData>& vtGlyfData)
{
	size_t stGlyphNum = vtGlyfData.size();
	if (stGlyphNum == 0) return -1;
	short minRSB = vtGlyfData[0].usAdvWidth - vtGlyfData[0].xMin - (vtGlyfData[0].xMax- vtGlyfData[0].xMin);
	for (size_t i = 0; i<stGlyphNum; i++){
		short tmp = vtGlyfData[i].usAdvWidth - vtGlyfData[i].xMin - (vtGlyfData[i].xMax - vtGlyfData[i].xMin);
		if (tmp<minRSB){
			minRSB = tmp;
		}
	}
	return minRSB;

}	// end of int CHYMakeJFFontDlg::FindMinRightSideBearing()

unsigned short 	 CHYMakeJFFontDlg::GetAdvancMaxWidth(std::vector<GlyfData>& vtGlyfData)
{
	size_t sz = vtGlyfData.size();
	unsigned short iAVDW = -1;
	if (sz>0)
	{
		iAVDW = vtGlyfData[0].usAdvWidth;
		for (size_t i = 1; i<sz; i++){
			if (vtGlyfData[i].usAdvWidth>iAVDW){
				iAVDW = vtGlyfData[i].usAdvWidth;
			}
		}
	}
	return iAVDW;

}	// end of int CHYMakeJFFontDlg::GetAdvancMaxWidth();

void CHYMakeJFFontDlg::BulidHMTX(FILE* pCreatFile, std::vector<GlyfData>& vtGlyfData)
{
	unsigned short	usTmp = 0;
	unsigned long	ulTmp = 0;

	int iEntryIndex = m_HYTbDirectory.FindTableEntry(HMTX_TAG);
	if (iEntryIndex == -1) return;

	CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
	tbEntry.offset = ftell(pCreatFile);
	fseek(pCreatFile, tbEntry.offset, SEEK_SET);

	size_t			szGlyps= vtGlyfData.size();	
	int				iGlyphNum = szGlyps;
	unsigned short numberOfHMetrics = 0;
	if (iGlyphNum>0) {
		int	iBaseadvanceWidth = vtGlyfData[--iGlyphNum].usAdvWidth;
		while (--iGlyphNum >= 0) {
			if (vtGlyfData[iGlyphNum].usAdvWidth == iBaseadvanceWidth)
				numberOfHMetrics++;
			else
				break;
		}
		numberOfHMetrics = (unsigned short)szGlyps - numberOfHMetrics;
	}

	for (size_t i = 0; i<szGlyps; i++){
		if (i<numberOfHMetrics){
			usTmp = hy_cdr_int16_to(vtGlyfData[i].usAdvWidth);
			fwrite(&usTmp, 2, 1, pCreatFile);
			usTmp = hy_cdr_int16_to(vtGlyfData[i].sLsb);
			fwrite(&usTmp, 2, 1, pCreatFile);
		}
		else{
			usTmp = hy_cdr_int16_to(vtGlyfData[i].sLsb);
			fwrite(&usTmp, 2, 1, pCreatFile);
		}
	}

	tbEntry.length = ftell(pCreatFile) - tbEntry.offset;
	int iTail = 4 - tbEntry.length % 4;
	if (tbEntry.length % 4>0){
		char c = 0;
		for (int i = 0; i<iTail; i++){
			fwrite(&c, 1, 1, pCreatFile);
		}
	}	

}	// end of void CHYMakeJFFontDlg::BulidHMTX()

void	CHYMakeJFFontDlg::Bulidname(FILE* pCreatFile)
{
	unsigned short	usTmp = 0;
	unsigned long	ulTmp = 0;

	int iEntryIndex = m_HYTbDirectory.FindTableEntry(NAME_TAG);
	if (iEntryIndex == -1) return;

	CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
	tbEntry.offset = ftell(pCreatFile);
	fseek(pCreatFile, tbEntry.offset, SEEK_SET);

	//format
	fwrite(&usTmp, 2, 1, pCreatFile);

	//count
	usTmp = hy_cdr_int16_to((unsigned short)m_HYNewName.vtNameRecord.size());
	fwrite(&usTmp, 2, 1, pCreatFile);

	//stringOffset
	unsigned short usOffset = 6 + 12 * (unsigned short)m_HYNewName.vtNameRecord.size();
	usTmp = hy_cdr_int16_to(usOffset);
	fwrite(&usTmp, 2, 1, pCreatFile);


	//Name Records
	unsigned short usStringSize = 0;
	char* pString = new char[260 * 4 * m_HYNewName.vtNameRecord.size()];

	for (size_t i = 0; i<m_HYNewName.vtNameRecord.size(); i++)
	{
		NAMERECORD& tagNameRecord = m_HYNewName.vtNameRecord[i];

		if (
			(tagNameRecord.platformID == 3 && tagNameRecord.encodingID == 1) ||
			(tagNameRecord.platformID == 0 && tagNameRecord.encodingID == 3) ||
			(tagNameRecord.platformID == 0 && tagNameRecord.encodingID == 4) ||
			(tagNameRecord.platformID == 0 && tagNameRecord.encodingID == 6)
			)
		{
			char strRecord[1040] = { 0 };
			int strLen = 1040;

			::MulToWCharBL(strRecord, strLen, tagNameRecord.strContent.c_str(), strlen(tagNameRecord.strContent.c_str()));
			tagNameRecord.length = strLen;

			memcpy(pString + usStringSize, strRecord, strLen);
		}
		else
		{
			tagNameRecord.length = (unsigned short)strlen(tagNameRecord.strContent.c_str());
			memcpy(pString + usStringSize, tagNameRecord.strContent.c_str(), tagNameRecord.length);
		}
		usStringSize += tagNameRecord.length;
	}

	size_t stOffset = 0;
	for (size_t i = 0; i<m_HYNewName.vtNameRecord.size(); i++)
	{
		NAMERECORD& tagNameRecord = m_HYNewName.vtNameRecord[i];

		//platformID
		usTmp = hy_cdr_int16_to(tagNameRecord.platformID);
		fwrite(&usTmp, 2, 1, pCreatFile);

		//encodingID
		usTmp = hy_cdr_int16_to(tagNameRecord.encodingID);
		fwrite(&usTmp, 2, 1, pCreatFile);

		//languageID
		usTmp = hy_cdr_int16_to(tagNameRecord.languageID);
		fwrite(&usTmp, 2, 1, pCreatFile);

		//nameID
		usTmp = hy_cdr_int16_to(tagNameRecord.nameID);
		fwrite(&usTmp, 2, 1, pCreatFile);

		//length				
		usTmp = hy_cdr_int16_to(tagNameRecord.length);
		fwrite(&usTmp, 2, 1, pCreatFile);

		//offset				
		usTmp = hy_cdr_int16_to(stOffset);
		fwrite(&usTmp, 2, 1, pCreatFile);
		stOffset += tagNameRecord.length;
	}

	fwrite(pString, usStringSize, 1, pCreatFile);
	if (pString)
		delete[] pString;

	tbEntry.length = ftell(pCreatFile) - tbEntry.offset;
	int iTail = 4 - tbEntry.length % 4;
	if (tbEntry.length % 4>0)
	{
		char c = 0;
		for (int i = 0; i<iTail; i++)
		{
			fwrite(&c, 1, 1, pCreatFile);
		}
	}

}	// end of int CHYMakeJFFontDlg::Bulidname()

void CHYMakeJFFontDlg::BulidVHEA(CHYFontCodec& FontCodec, FILE* pCreatFile, std::vector<GlyfData>& vtGlyfData)
{	
	unsigned short	usTmp = 0;
	unsigned long	ulTmp = 0;

	int iEntryIndex = m_HYTbDirectory.FindTableEntry(VHEA_TAG);
	if (iEntryIndex == -1) return;

	CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
	tbEntry.offset = ftell(pCreatFile);
	fseek(pCreatFile, tbEntry.offset, SEEK_SET);

	//Table version number	
	usTmp = hy_cdr_int16_to(FontCodec.m_HYVhea.version.value);
	fwrite(&usTmp, 2, 1, pCreatFile);
	usTmp = hy_cdr_int16_to(FontCodec.m_HYVhea.version.fract);
	fwrite(&usTmp, 2, 1, pCreatFile);
	// ascent
	usTmp = hy_cdr_int16_to(FontCodec.m_HYVhea.ascent);
	fwrite(&usTmp, 2, 1, pCreatFile);
	// descent
	usTmp = hy_cdr_int16_to(FontCodec.m_HYVhea.descent);
	fwrite(&usTmp, 2, 1, pCreatFile);
	// lineGap
	usTmp = hy_cdr_int16_to(FontCodec.m_HYVhea.lineGap);
	fwrite(&usTmp, 2, 1, pCreatFile);
	// advanceHeightMax
	usTmp = hy_cdr_int16_to(FontCodec.m_HYVhea.advanceHeightMax);
	fwrite(&usTmp, 2, 1, pCreatFile);
	// minTopSideBearing
	usTmp = hy_cdr_int16_to(FontCodec.m_HYVhea.minTop);
	fwrite(&usTmp, 2, 1, pCreatFile);
	//minBottomSideBearing
	usTmp = hy_cdr_int16_to(FontCodec.m_HYVhea.minBottom);
	fwrite(&usTmp, 2, 1, pCreatFile);
	//yMaxExtent
	usTmp = hy_cdr_int16_to(FontCodec.m_HYVhea.yMaxExtent);
	fwrite(&usTmp, 2, 1, pCreatFile);
	//caretSlopeRise
	usTmp = hy_cdr_int16_to(FontCodec.m_HYVhea.caretSlopeRise);
	fwrite(&usTmp, 2, 1, pCreatFile);
	// caretSlopeRun
	usTmp = hy_cdr_int16_to(FontCodec.m_HYVhea.caretSlopeRun);
	fwrite(&usTmp, 2, 1, pCreatFile);
	//caretOffset
	usTmp = hy_cdr_int16_to(FontCodec.m_HYVhea.caretOffset);
	fwrite(&usTmp, 2, 1, pCreatFile);
	//reserved
	usTmp = hy_cdr_int16_to(FontCodec.m_HYVhea.reserved1);
	fwrite(&usTmp, 2, 1, pCreatFile);
	//reserved
	usTmp = hy_cdr_int16_to(FontCodec.m_HYVhea.reserved2);
	fwrite(&usTmp, 2, 1, pCreatFile);
	//reserved
	usTmp = hy_cdr_int16_to(FontCodec.m_HYVhea.reserved3);
	fwrite(&usTmp, 2, 1, pCreatFile);
	//reserved
	usTmp = hy_cdr_int16_to(FontCodec.m_HYVhea.reserved4);
	fwrite(&usTmp, 2, 1, pCreatFile);
	//metricDataFormat
	usTmp = hy_cdr_int16_to(FontCodec.m_HYVhea.metricDataFormat);
	fwrite(&usTmp, 2, 1, pCreatFile);
	//numOfLongVerMetrics
	int			szGlyphNum = (int)vtGlyfData.size();
	unsigned short numberOfVMetrics = 0;
	if (szGlyphNum>0) {
		int	iBaseadvanceHeight = vtGlyfData[--szGlyphNum].usAdvHeight;
		while (--szGlyphNum >= 0) {
			if (vtGlyfData[szGlyphNum].usAdvHeight == iBaseadvanceHeight)
				numberOfVMetrics++;
			else
				break;
		}
		numberOfVMetrics = (unsigned short)vtGlyfData.size() - numberOfVMetrics;
	}

	usTmp = hy_cdr_int16_to(numberOfVMetrics);
	fwrite(&usTmp, 2, 1, pCreatFile);

	tbEntry.length = ftell(pCreatFile) - tbEntry.offset;
	int iTail = 4 - tbEntry.length % 4;
	if (tbEntry.length % 4>0){
		char c = 0;
		for (int i = 0; i<iTail; i++){
			fwrite(&c, 1, 1, pCreatFile);
		}
	}

}	// end of void CHYMakeJFFontDlg::BulidVHEA()

void CHYMakeJFFontDlg::BulidVMTX(FILE* pCreatFile, std::vector<GlyfData>& vtGlyfData)
{	
	unsigned short	usTmp = 0;
	unsigned long	ulTmp = 0;

	int iEntryIndex = m_HYTbDirectory.FindTableEntry(VMTX_TAG);
	if (iEntryIndex == -1) return;

	CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
	tbEntry.offset = ftell(pCreatFile);
	fseek(pCreatFile, tbEntry.offset, SEEK_SET);

	size_t			stGlyps = (int)vtGlyfData.size();
	int				iGlyphNum = stGlyps;
	unsigned short numberOfVMetrics = 0;
	if (iGlyphNum>0) {
		int	iBaseadvanceHeight = vtGlyfData[--iGlyphNum].usAdvHeight;
		while (--iGlyphNum >= 0) {
			if (vtGlyfData[iGlyphNum].usAdvHeight == iBaseadvanceHeight)
				numberOfVMetrics++;
			else
				break;
		}
		numberOfVMetrics = (unsigned short)stGlyps - numberOfVMetrics;
	}
	
	for (size_t i = 0; i<stGlyps; i++) {
		if (i<numberOfVMetrics) {
			usTmp = hy_cdr_int16_to(vtGlyfData[i].usAdvHeight);
			fwrite(&usTmp, 2, 1, pCreatFile);
			usTmp = hy_cdr_int16_to(vtGlyfData[i].sTsb);
			fwrite(&usTmp, 2, 1, pCreatFile);
		}
		else {
			usTmp = hy_cdr_int16_to(vtGlyfData[i].sTsb);
			fwrite(&usTmp, 2, 1, pCreatFile);
		}
	}
	tbEntry.length = ftell(pCreatFile) - tbEntry.offset;
	int iTail = 4 - tbEntry.length % 4;
	if (tbEntry.length % 4>0){
		char c = 0;
		for (int i = 0; i<iTail; i++)	{
			fwrite(&c, 1, 1, pCreatFile);
		}
	}

}	// end of void CHYMakeJFFontDlg::BulidVMTX()

void CHYMakeJFFontDlg::BulidPOST(CHYFontCodec& FontCodec, FILE* pCreatFile, std::vector<GlyfData>& vtGlyfData)
{	
	unsigned short	usTmp = 0;
	unsigned long	ulTmp = 0;

	int iEntryIndex = m_HYTbDirectory.FindTableEntry(POST_TAG);
	if (iEntryIndex == -1) return;

	CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
	tbEntry.offset = ftell(pCreatFile);
	fseek(pCreatFile, tbEntry.offset, SEEK_SET);

	//Version
	usTmp = hy_cdr_int16_to(FontCodec.m_HYPost.Format.value);
	fwrite(&usTmp, 2, 1, pCreatFile);
	usTmp = hy_cdr_int16_to(FontCodec.m_HYPost.Format.fract);
	fwrite(&usTmp, 2, 1, pCreatFile);

	// italicAngle
	usTmp = hy_cdr_int16_to(FontCodec.m_HYPost.italicAngle.value);
	fwrite(&usTmp, 2, 1, pCreatFile);
	usTmp = hy_cdr_int16_to(FontCodec.m_HYPost.italicAngle.fract);
	fwrite(&usTmp, 2, 1, pCreatFile);

	//underlinePosition
	usTmp = hy_cdr_int16_to(FontCodec.m_HYPost.underlinePosition);
	fwrite(&usTmp, 2, 1, pCreatFile);

	//UnderlineThickness
	usTmp = hy_cdr_int16_to(FontCodec.m_HYPost.underlineThickness);
	fwrite(&usTmp, 2, 1, pCreatFile);

	//isFixedPitch
	ulTmp = hy_cdr_int32_to(FontCodec.m_HYPost.isFixedPitch);
	fwrite(&ulTmp, 4, 1, pCreatFile);

	ulTmp = 0;
	//minMemType42
	fwrite(&ulTmp, 4, 1, pCreatFile);
	//maxMemType42
	fwrite(&ulTmp, 4, 1, pCreatFile);
	//pCreatFile
	fwrite(&ulTmp, 4, 1, pCreatFile);
	//maxMemType1
	fwrite(&ulTmp, 4, 1, pCreatFile);

	if (FontCodec.m_HYPost.Format.value == 2 && FontCodec.m_HYPost.Format.fract == 0){
		unsigned short numberOfGlyphs = (unsigned short)vtGlyfData.size();
		// numberOfGlyphs
		usTmp = hy_cdr_int16_to(numberOfGlyphs);
		fwrite(&usTmp, 2, 1, pCreatFile);

		CHYPost pstTmp;
		//glyphNameIndex
		for (int i = 0; i <numberOfGlyphs; i++)	{
			unsigned short usPostNameID = pstTmp.InsertName(vtGlyfData[i].strName);
			usTmp = hy_cdr_int16_to(usPostNameID);
			fwrite(&usTmp, 2, 1, pCreatFile);
		}
		//names
		size_t st = pstTmp.m_vtStandString.size();
		for (size_t i = 258; i <st; i++){
			const char* strData = pstTmp.m_vtStandString[i].data();
			size_t t = strlen(strData);

			fwrite((byte*)&t, 1, 1, pCreatFile);
			fwrite(strData, t, 1, pCreatFile);
		}
	}

	tbEntry.length = ftell(pCreatFile) - tbEntry.offset;
	int iTail = 4 - tbEntry.length % 4;
	if (tbEntry.length % 4>0){
		char c = 0;
		for (int i = 0; i<iTail; i++){
			fwrite(&c, 1, 1, pCreatFile);
		}
	}

}	// end of void CHYMakeJFFontDlg::BulidPOST()

bool CHYMakeJFFontDlg::Extraglyf(CHYFontCodec& FontWDecode,
								std::vector<GlyfData>& vtGlyfData,
								std::vector<unsigned long>&	vtUni)
{
	int iTableEntryIndx = FontWDecode.m_HYTbDirectory.FindTableEntry(GLYF_TAG);
	if (iTableEntryIndx == -1)		return false;

	CHYTableEntry& entry = FontWDecode.m_HYTbDirectory.vtTableEntry[iTableEntryIndx];
	fseek(FontWDecode.m_pFontFile, entry.offset, SEEK_SET);

	size_t stJUni = vtUni.size();
	for (size_t i = 0; i < stJUni; i++){
		GlyfData glyfData;
		int index = FontWDecode.FindGryphIndexByUnciode(vtUni[i]);
		if (index == -1)continue;

		glyfData.pDataLen = FontWDecode.m_HYloca.vtLoca[index + 1] - FontWDecode.m_HYloca.vtLoca[index];
		if (glyfData.pDataLen == 0) continue;
		fseek(FontWDecode.m_pFontFile, entry.offset + FontWDecode.m_HYloca.vtLoca[index], SEEK_SET);
		for (int j = 0; j<glyfData.pDataLen; j++){
			byte bData = 0;
			fread(&bData, 1, 1, FontWDecode.m_pFontFile);
			glyfData.vtData.push_back(bData);
		}

		fseek(FontWDecode.m_pFontFile, entry.offset + FontWDecode.m_HYloca.vtLoca[index], SEEK_SET);
		// contour numbers		
		fread(&glyfData.CnturNum, 2, 1, FontWDecode.m_pFontFile);
		glyfData.CnturNum = hy_cdr_int16_to(glyfData.CnturNum);
		//Minimum x for coordinate data 
		fread(&glyfData.xMin, 2, 1, FontWDecode.m_pFontFile);
		glyfData.xMin = hy_cdr_int16_to(glyfData.xMin);
		//Minimum y for coordinate data 
		fread(&glyfData.yMin, 2, 1, FontWDecode.m_pFontFile);
		glyfData.yMin = hy_cdr_int16_to(glyfData.yMin);
		//Maximum x for coordinate data 
		fread(&glyfData.xMax, 2, 1, FontWDecode.m_pFontFile);
		glyfData.xMax = hy_cdr_int16_to(glyfData.xMax);
		//Maximum y for coordinate data 
		fread(&glyfData.yMax, 2, 1, FontWDecode.m_pFontFile);
		glyfData.yMax = hy_cdr_int16_to(glyfData.yMax);

		glyfData.vtUnicode.push_back(vtUni[i]);
		glyfData.strName = FontWDecode.m_HYPost.FindNameByGID(index);

		if (index<FontWDecode.m_HYHhea.numberOfHMetrics){
			glyfData.usAdvWidth = FontWDecode.m_HYHmtx.vtLonghormetric[index].advanceWidth;
			glyfData.sLsb = FontWDecode.m_HYHmtx.vtLonghormetric[index].lsb;
		}
		else{
			glyfData.usAdvWidth = FontWDecode.m_HYHmtx.vtLonghormetric[FontWDecode.m_HYHhea.numberOfHMetrics - 1].advanceWidth;
			glyfData.sLsb = FontWDecode.m_HYHmtx.vtLeftsidebearing[index - FontWDecode.m_HYHhea.numberOfHMetrics];
		}

		if (FontWDecode.m_HYVhea.numOfLongVerMetrics > 0){
			if (index<FontWDecode.m_HYVhea.numOfLongVerMetrics){
				glyfData.usAdvHeight = FontWDecode.m_HYVmtx.vtLongVrtmetric[index].advanceHeight;
				glyfData.sTsb = FontWDecode.m_HYVmtx.vtLongVrtmetric[index].tsb;
			}
			else{
				glyfData.usAdvHeight = FontWDecode.m_HYVmtx.vtLongVrtmetric[FontWDecode.m_HYVhea.numOfLongVerMetrics - 1].advanceHeight;
				glyfData.sTsb = FontWDecode.m_HYVmtx.vtTopsidebearing[index - FontWDecode.m_HYVhea.numOfLongVerMetrics];
			}
		}
		vtGlyfData.push_back(glyfData);
	}

	return true;

}	// end of bool CHYMakeJFFontDlg::Extraglyf()

bool CHYMakeJFFontDlg::CheckGid(std::vector<int>& vtgid, int iGid)
{
	if (iGid == -1) return false;
	for (size_t i = 0; i < vtgid.size(); i++){
		if (vtgid[i] == iGid) return false;
	}

	return true;

}	// end of BOOL CHYMakeJFFontDlg::CheckGid()

void CHYMakeJFFontDlg::CountFontBound(short& xMin,
										short& yMin,
										short& xMax,
										short& yMax,
										std::vector<GlyfData>& vtglfData)
{
	if (vtglfData.size() > 0)	{
		xMin = vtglfData[0].xMin!= 0 ? vtglfData[0].xMin : 32767;
		yMin = vtglfData[0].yMin!= 0 ? vtglfData[0].yMin : 32767;
		xMax = vtglfData[0].xMax != 0 ? vtglfData[0].xMax : -32768;
		yMax = vtglfData[0].yMax != 0 ? vtglfData[0].yMax : -32768;

		for (size_t i = 1; i<vtglfData.size(); i++){
			if (vtglfData[i].xMin<xMin)
				xMin = vtglfData[i].xMin;
			if (vtglfData[i].yMin<yMin)
				yMin = vtglfData[i].yMin;
			if (vtglfData[i].xMax>xMax)
				xMax = vtglfData[i].xMax;
			if (vtglfData[i].yMax>yMax)
				yMax = vtglfData[i].yMax;
		}
	}

}	// end of int CHYMakeJFFontDlg::CountFontBound()	

