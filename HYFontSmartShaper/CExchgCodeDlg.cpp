// CExchgCodeDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "HYFontSmartShaper.h"
#include "CExchgCodeDlg.h"
#include "afxdialogex.h"


// CExchgCodeDlg 对话框

IMPLEMENT_DYNAMIC(CExchgCodeDlg, CDialogEx)

CExchgCodeDlg::CExchgCodeDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_EXCHGCDE_DLG, pParent)
	, m_strSrcFontFile(_T(""))
	, m_strCodeFile(_T(""))
	, m_strDstFontFile(_T(""))
{

}	//end of CExchgCodeDlg::CExchgCodeDlg()

CExchgCodeDlg::~CExchgCodeDlg()
{

}	//end of CExchgCodeDlg::~CExchgCodeDlg()

void CExchgCodeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EXCHGCDE_FNT_EDT, m_strSrcFontFile);
	DDX_Text(pDX, IDC_EXCHGCDE_CDE_EDT, m_strCodeFile);
	DDX_Text(pDX, IDC_EXCHGCDE_SVFNT_EDT, m_strDstFontFile);
}	//end of void CExchgCodeDlg::DoDataExchange()

BEGIN_MESSAGE_MAP(CExchgCodeDlg, CDialogEx)
	ON_BN_CLICKED(IDC_EXCHGCDE_FNT_BTN, &CExchgCodeDlg::OnBnClickedExchgcdeFntBtn)
	ON_BN_CLICKED(IDC_EXCHGCDE_CDE_BTN, &CExchgCodeDlg::OnBnClickedExchgcdeCdeBtn)
	ON_BN_CLICKED(IDC_EXCHGCDE_SVFNT_BTN, &CExchgCodeDlg::OnBnClickedExchgcdeSvfntBtn)
	ON_BN_CLICKED(IDC_EXCHGCDE_RUN_BTN, &CExchgCodeDlg::OnBnClickedExchgcdeRunBtn)
END_MESSAGE_MAP()

// CExchgCodeDlg 消息处理程序
BOOL CExchgCodeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CheckRadioButton(IDC_EXCHGCDE_FUL_RD, IDC_EXCHGCDE_EXCHG_RD, IDC_EXCHGCDE_DEL_RD);



	
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE

}	//end of BOOL CExchgCodeDlg::OnInitDialog()

void CExchgCodeDlg::OnBnClickedExchgcdeFntBtn()
{
	TCHAR	szFilters[] = _T("TrueType File(*.ttf)|*.ttf|OpenType File(*.otf)|*.otf||");
	CFileDialog  openFileDlg(TRUE, _T(""), _T(""), OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilters);
	if (openFileDlg.DoModal() != IDOK)	return;

	m_strSrcFontFile = openFileDlg.GetPathName();	

	UpdateData(FALSE);

}	//end of void CExchgCodeDlg::OnBnClickedExchgcdeFntBtn()

void CExchgCodeDlg::OnBnClickedExchgcdeCdeBtn()
{
	TCHAR	szFilters[] = _T("TrueType File(*.txt)|*.txt||");
	CFileDialog  openFileDlg(TRUE, _T(""), _T(""), OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilters);
	if (openFileDlg.DoModal() != IDOK)	return;

	m_strCodeFile = openFileDlg.GetPathName();

	UpdateData(FALSE);

}	// end of void CExchgCodeDlg::OnBnClickedExchgcdeCdeBtn()

void CExchgCodeDlg::OnBnClickedExchgcdeSvfntBtn()
{
	TCHAR	szFilters[] = _T("TrueType File(*.ttf)|*.ttf|OpenType File(*.otf)|*.otf||");
	CFileDialog  openFileDlg(TRUE, _T(""), _T(""), OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilters);
	if (openFileDlg.DoModal() != IDOK)	return;

	m_strDstFontFile = openFileDlg.GetPathName();

	UpdateData(FALSE);

}	// end of void CExchgCodeDlg::OnBnClickedExchgcdeSvfntBtn()

void CExchgCodeDlg::OnBnClickedExchgcdeRunBtn()
{	
	UpdateData(TRUE);

	if (m_strSrcFontFile == _T("")) {
		AfxMessageBox("字体名称为空!");
		return;	
	}

	if (m_strCodeFile == _T("")) {
		AfxMessageBox("码表名称为空!");
		return;
	}

	if (m_strDstFontFile == _T("")) {
		AfxMessageBox("保存字库为空!");
		return;
	}
	int iCheck = GetCheckedRadioButton(IDC_EXCHGCDE_FUL_RD, IDC_EXCHGCDE_EXCHG_RD); 
	if (iCheck == IDC_EXCHGCDE_FUL_RD) {
		UpdateCode(m_strSrcFontFile, m_strDstFontFile, m_strCodeFile);
	}
	if (iCheck == IDC_EXCHGCDE_DEL_RD) {
		DelCode(m_strSrcFontFile, m_strDstFontFile, m_strCodeFile);
	}
	if (iCheck == IDC_EXCHGCDE_EXCHG_RD) {
		ExChangeCode(m_strSrcFontFile, m_strDstFontFile, m_strCodeFile);
	}

	AfxMessageBox("处理完成");

}	// end of void CExchgCodeDlg::OnBnClickedExchgcdeRunBtn()

void CExchgCodeDlg::ReadFont(CString strInFntFile, CHYFontCodec& Decode)
{
	Decode.SetDefault();

	if (Decode.OpenFile(strInFntFile) == HY_NOERROR)
	{
		// 获取字典目录
		Decode.DecodeTableDirectory();
		Decode.Decodehead();
		Decode.Decodemaxp();
		Decode.Decodecmap();

		// clearup
		Decode.m_HYCodeMap.vtHYCodeMap.clear();

		CHYCodeMapItem  mapItm;
		int	iGlyphNum = Decode.m_HYMaxp.numGlyphs;
		for (int i = 0; i < iGlyphNum; i++) {
			if (i == 0) {
				mapItm.ulGlyphNo = 0xffff;
				mapItm.iGlyphIndex = 0;
				Decode.m_HYCodeMap.vtHYCodeMap.push_back(mapItm);
			}
			else {
				std::vector<unsigned long> szUnicode;
				Decode.FindGryphUncidoByIndex(i, szUnicode);
				size_t stUnicodeNum = szUnicode.size();
				for (size_t j = 0; j < stUnicodeNum; j++) {
					mapItm.ulGlyphNo = szUnicode[j];
					mapItm.iGlyphIndex = i;
					Decode.m_HYCodeMap.vtHYCodeMap.push_back(mapItm);
				}
			}
		}
		Decode.m_HYCodeMap.QuickSortbyUnicode();
	}

}	// end of void CExchgCodeDlg::ReadFont()

void CExchgCodeDlg::SaveFont(CString strSaveFntFile, CHYFontCodec& Encode,TableDataS& mulpTableData)
{
	if (Encode.CreateFile(strSaveFntFile) != HY_NOERROR) {
		AfxMessageBox("生成失败");
		return;
	}

	__time64_t	 tspace = 2082844799;
	CTime tCurTime = CTime::GetCurrentTime();
	__time64_t  t64CurTime = tCurTime.GetTime();
	t64CurTime += tspace;
	char p[8] = { 0 };
	CopyMemory((PBYTE)p, &t64CurTime, sizeof(__time64_t));
	for (int i = 0; i < 8; i++) {
		Encode.m_HYhead.modified[i] = p[7 - i];
	}
	Encode.MakeCmap();

	Encode.EncodeTableDirectory();
	for (int i = 0; i < Encode.m_HYTbDirectory.numTables; i++)
	{
		CHYTableEntry& tbEntry = Encode.m_HYTbDirectory.vtTableEntry[i];		
		if (tbEntry.tag == CMAP_TAG) {
			Encode.Encodecmap(Encode.m_pFontFile, Encode.m_HYTbDirectory);
		}
		else if (tbEntry.tag == HEAD_TAG) {
			Encode.Encodehead();
		}
		else {
			tbEntry.offset = ftell(Encode.m_pFontFile);			
			TableData* pData = mulpTableData.FindTableData(tbEntry.tag);
			if (pData) {
				for (int i = 0; i < pData->ulDataSize; i++) {
					fwrite(&pData->vtData[i], 1, 1, Encode.m_pFontFile);
				}
			}
			tbEntry.length = ftell(Encode.m_pFontFile) - tbEntry.offset;
			::HY_4ByteAlign(Encode.m_pFontFile, tbEntry.length);
		}
	}

	// 对每一个表生成校验码
	fflush(Encode.m_pFontFile);
	for (int i = 0; i < Encode.m_HYTbDirectory.numTables; i++) {
		CHYTableEntry& HYEntry = Encode.m_HYTbDirectory.vtTableEntry[i];
		int CheckBufSz = (HYEntry.length + 3) / 4 * 4;
		char* pCheckBuf = new char[CheckBufSz];
		fseek(Encode.m_pFontFile, HYEntry.offset, SEEK_SET);
		fread(pCheckBuf, CheckBufSz, 1, Encode.m_pFontFile);
		HYEntry.checkSum = HY_CalcFontTableChecksum((unsigned long*)pCheckBuf, HYEntry.length);
		if (pCheckBuf) delete[] pCheckBuf;
	}
	fflush(Encode.m_pFontFile);
	Encode.EncodeTableDirectory();
	Encode.CloseFile();

	Encode.SetCheckSumAdjustment(strSaveFntFile);

}	//end of void CExchgCodeDlg::SaveFont()

void CExchgCodeDlg::UpdateCode(CString strSrcFontName, CString strDstFontName, CString strCodeFile)
{


}	// end of void CExchgCodeDlg::UpdateCode()

void CExchgCodeDlg::DelCode(CString strSrcFontName, CString strDstFontName, CString strCodeFile)
{
	std::vector<unsigned long> vtUnicode;
	::HY_LoadCodeFile((char*)string(strCodeFile).c_str(), vtUnicode);

	CHYFontCodec Decode;
	ReadFont(strSrcFontName, Decode);

	CHYFontCodec Encode;
	Encode.m_iFontType = Decode.m_iFontType;
	Encode.m_HYhead = Decode.m_HYhead;	
	Encode.m_HYCodeMap = Decode.m_HYCodeMap;
	Encode.m_HYTbDirectory = Decode.m_HYTbDirectory;

	::XSysproxy().DelCode(Encode, vtUnicode);

	SaveFont(strDstFontName, Encode, Decode.m_mulpTableData);

}	// end of void CExchgCodeDlg::DelCode()

void CExchgCodeDlg::ExChangeCode(CString strSrcFontName, CString strDstFontName, CString strCodeFile)
{
	std::vector<unsigned long> vtUnicode1;
	std::vector<unsigned long> vtUnicode2;
	LoadExchgCodeFile(strCodeFile, vtUnicode1, vtUnicode2);

	CHYFontCodec Decode;
	ReadFont(strSrcFontName, Decode);

	CHYFontCodec Encode;
	Encode.m_iFontType = Decode.m_iFontType;
	Encode.m_HYhead = Decode.m_HYhead;
	Encode.m_HYCodeMap = Decode.m_HYCodeMap;
	Encode.m_HYTbDirectory = Decode.m_HYTbDirectory;

	::XSysproxy().ExchangeCode(Encode,vtUnicode1,vtUnicode2);
	SaveFont(strDstFontName, Encode, Decode.m_mulpTableData);

}	// end of void CExchgCodeDlg::ExChangeCode()

void CExchgCodeDlg::LoadExchgCodeFile(CString strSrcFontName, std::vector<unsigned long>& vtUnicode1, std::vector<unsigned long>& vtUnicode2)
{
	unsigned long			dwOffset = 0;
	unsigned long size = ::HY_FileExist(string(strSrcFontName).c_str());
	if (size == 0)			return;

	ifstream in(strSrcFontName);
	string line;
	if (in) {
		while (getline(in, line)) { // line中不包括每行的换行符
			if (line.length() == 0) continue;
			std::vector<string> vtTmp;
			::HY_SpliteString(line.c_str(),";", vtTmp);
			if (vtTmp.size() != 2) continue;

			unsigned long uluni1 = strtol(vtTmp[0].c_str(),NULL,16);
			vtUnicode1.push_back(uluni1);
			unsigned long uluni2 = strtol(vtTmp[1].c_str(),NULL, 16);
			vtUnicode2.push_back(uluni2);
		}
	}

}	// end of void CExchgCodeDlg::LoadExchgCodeFile()


