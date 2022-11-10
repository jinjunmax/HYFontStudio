// CharsCompareDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "HYFontSmartShaper.h"
#include "CharsCompareDlg.h"
#include "afxdialogex.h"


// CCharsCompareDlg 对话框

IMPLEMENT_DYNAMIC(CCharsCompareDlg, CDialog)

CCharsCompareDlg::CCharsCompareDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ID_CHARS_COMPARE_DLG, pParent)
{

}

CCharsCompareDlg::~CCharsCompareDlg()
{

}

void CCharsCompareDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHARS_COMPARE_LST, m_lstFnts);
}	// end of 


BEGIN_MESSAGE_MAP(CCharsCompareDlg, CDialog)
	ON_BN_CLICKED(IDC_CHARS_COMPARE_ADD_LST, &CCharsCompareDlg::OnBnClickedCharsCompareAddLst)
	ON_BN_CLICKED(IDC_CHARS_COMPARE_DEL_LST, &CCharsCompareDlg::OnBnClickedCharsCompareDelLst)
	ON_BN_CLICKED(IDC_CHARS_COMPARE_MK_LST, &CCharsCompareDlg::OnBnClickedCharsCompareMkLst)
END_MESSAGE_MAP()


// CCharsCompareDlg 消息处理程序

void CCharsCompareDlg::OnBnClickedCharsCompareAddLst()
{
	TCHAR	szFilters[] = _T("TrueType File(*.ttf)|*.ttf|OpenType File(*.otf)|*.otf||");
	CFileDialog  openFileDlg(TRUE, _T(""), _T(""), OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilters);
	if (openFileDlg.DoModal() != IDOK)	return;

	CString strFileName = openFileDlg.GetPathName();
	m_vtFntName.push_back((LPCTSTR)(LPCSTR)strFileName);

	string str = HY_GetFileNameFromPath((LPCTSTR)(LPCSTR)strFileName);
	m_lstFnts.AddString(str.c_str());

}	// end of void CCharsCompareDlg::OnBnClickedCharsCompareAddLst()

void CCharsCompareDlg::OnBnClickedCharsCompareDelLst()
{
	int iIndx = m_lstFnts.GetCurSel();
	if (iIndx == LB_ERR) return;

	m_vtFntName.erase(m_vtFntName.begin() + iIndx);
	m_lstFnts.DeleteString(iIndx);

	this->UpdateData(FALSE);

}	// end of void CCharsCompareDlg::OnBnClickedCharsCompareDelLst()

void CCharsCompareDlg::OnBnClickedCharsCompareMkLst()
{
	CWaitCursor wc;

	size_t  stFontsSize = m_vtFntName.size();
	if (stFontsSize < 1)
	{
		AfxMessageBox("字符数据比较最少需要两个字库。");
		return;
	}
	
	int rt = HY_NOERROR;
	CString strErrInf = "";
	CHYFontCodec FntBase;
	rt = FntBase.Decode((char*)m_vtFntName[0].data());
	if (rt != HY_NOERROR)
	{		
		strErrInf.Format("字库%s解析失败", m_vtFntName[0].data());
		AfxMessageBox(strErrInf);
		return;
	}

	std::vector<CHYFontCodec> vtFntCdc;
	for (size_t i = 1; i < stFontsSize; i++)
	{
		CHYFontCodec fntCdc;
		fntCdc.Decode((char*)m_vtFntName[i].data());
		if (rt != HY_NOERROR)
		{
			strErrInf.Format("字库%s解析失败", m_vtFntName[0].data());
			AfxMessageBox(strErrInf);
			vtFntCdc.clear();

			return;
		}

		vtFntCdc.push_back(fntCdc);
	}

	string strDir = ::HY_GetDirFromPath(m_vtFntName[0]) + "\\异体字unicode表.txt";
	FILE* pCreatFile1 = fopen(strDir.c_str(), "w");

	strDir = ::HY_GetDirFromPath(m_vtFntName[0]) + "\\异体字字表.txt";
	FILE* pCreatFile2 = fopen(strDir.c_str(), "w,ccs = UNICODE");

	strDir = ::HY_GetDirFromPath(m_vtFntName[0]) + "\\不存在字表.txt";
	FILE* pCreatFile3 = fopen(strDir.c_str(), "w");

	for (int i = 0; i < FntBase.m_HYMaxp.numGlyphs; i++)
	{
		CHYGlyph& bsGyph = FntBase.m_vtHYGlyphs[i];

		if (bsGyph.vtUnicode.size() > 0)
		{
			if (bsGyph.vtContour.size() > 0)
			{
				CString strTrace;
				bool bEqual = true;
				for (int j = 0; j < stFontsSize-1; j++)
				{
					int iRt = CompareChar(bsGyph, vtFntCdc[j]);
					// 如果比较字库没有对应的字符？
					if (iRt == -1) {
						strTrace.Format("GID = %04d unicode = %x, 没有对应字符！\n", i, bsGyph.vtUnicode[0]);

						fwrite((LPTSTR)(LPCTSTR)strTrace, strTrace.GetLength(), 1, pCreatFile3);
						
						break;
					}

					if (iRt == 0)
					{
						for (int j = 0; j < bsGyph.vtUnicode.size(); j++)
						{
							//string strChar =HY_Unicode_to_AnsiString(bsGyph.vtUnicode[j])+"\n";
							//fwrite(strChar.c_str(), strChar.length(), 1,pCreatFile2);							
							fwrite((WCHAR*)&bsGyph.vtUnicode[j], sizeof(WCHAR), 1, pCreatFile2);

							strTrace.Format("%x\n",bsGyph.vtUnicode[j]);
							fwrite((LPTSTR)(LPCTSTR)strTrace, strTrace.GetLength(), 1, pCreatFile1);
						}
					}
				}
			}
		}
	}


	if(pCreatFile1 != NULL)
	{
		fflush(pCreatFile1);
		fclose(pCreatFile1);
	}

	if (pCreatFile2 != NULL)
	{
		fflush(pCreatFile2);
		fclose(pCreatFile2);
	}

	if (pCreatFile3 != NULL)
	{
		fflush(pCreatFile3);
		fclose(pCreatFile3);
	}


}	// end of void CCharsCompareDlg::OnBnClickedCharsCompareMkLst()

int CCharsCompareDlg::CompareChar(CHYGlyph &bsglyph, CHYFontCodec& FntCdc)
{
	int iIndex = FntCdc.FindGryphIndexByUnciode(bsglyph.vtUnicode[0]);
	if (iIndex == -1) return -1;
	
	CHYGlyph& cmprGlyph = FntCdc.m_vtHYGlyphs[iIndex];
	
	if (bsglyph.vtContour.size() != cmprGlyph.vtContour.size()) return 0;

	int ibsptNumer = bsglyph.GetTotalPointsNumber();
	if (ibsptNumer != cmprGlyph.GetTotalPointsNumber()) return 0;
	
	size_t stCnturNum = bsglyph.vtContour.size();
	for (size_t i = 0; i < stCnturNum; i++)
	{
		CHYContour&  bsCnt = bsglyph.vtContour[i];
		CHYContour&  cmprCnt = bsglyph.vtContour[i];

		size_t stptNumer = bsCnt.vtHYPoints.size();
		if (stptNumer != cmprCnt.vtHYPoints.size()) return 0;

		for (size_t j = 0; j < stptNumer; j++)
		{
			CHYPoint& bspt = bsCnt.vtHYPoints[j];
			CHYPoint& cmprpt = bsCnt.vtHYPoints[j];

			if (bspt.x != cmprpt.x) return 0;
			if (bspt.y != cmprpt.y) return 0;
			if (bspt.flag != cmprpt.flag) return 0;
		}
	}

	return 1;

}	// end of void CCharsCompareDlg::CompareChar()