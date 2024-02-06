// FongSVGDlg.cpp : implementation file
//
#include "stdafx.h"
#include "HYFontSmartShaper.h"
#include "FongSVGDlg.h"

// CFongSVGDlg dialog

IMPLEMENT_DYNAMIC(CFongSVGDlg, CDialog)

CFongSVGDlg::CFongSVGDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFongSVGDlg::IDD, pParent)
	, m_strFontFile(_T(""))
	, m_strSVGFile(_T(""))
{

}	//end of CFongSVGDlg::CFongSVGDlg()

CFongSVGDlg::~CFongSVGDlg()
{

}	// end of CFongSVGDlg::~CFongSVGDlg()

void CFongSVGDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_SVG_OPENFNT_EDT2, m_strFontFile);
	DDX_Text(pDX, IDC_SVG_SAVESVG_EDT, m_strSVGFile);

}	// end of void CFongSVGDlg::DoDataExchange()

BEGIN_MESSAGE_MAP(CFongSVGDlg, CDialog)
	ON_BN_CLICKED(IDC_SVG_OPENFNT_BTN, &CFongSVGDlg::OnBnClickedSvgOpenfntBtn)
	ON_BN_CLICKED(IDC_SVG_SAVESVG_BTN, &CFongSVGDlg::OnBnClickedSvgSavesvgBtn)
END_MESSAGE_MAP()

// CFongSVGDlg message handlers
void CFongSVGDlg::OnBnClickedSvgOpenfntBtn()
{
	TCHAR	szFilters[]=_T("TrueType 文件(*.ttf)|*.ttf|OpenType 文件(*.otf)|*.otf||");
	CFileDialog  openFileDlg(TRUE, _T(""), _T(""), OFN_LONGNAMES|OFN_FILEMUSTEXIST,szFilters);
	if (openFileDlg.DoModal() != IDOK)	return;

	m_strFontFile = openFileDlg.GetPathName();

	this->UpdateData(FALSE);

}	// end of void CFongSVGDlg::OnBnClickedSvgOpenfntBtn()

void CFongSVGDlg::OnBnClickedSvgSavesvgBtn()
{
	TCHAR	szFilters[]=_T("SVG 文件(*.svg)|*.svg||");
	CFileDialog  SaveFileDlg(FALSE, _T(""), _T(""), OFN_LONGNAMES,szFilters);
	if (SaveFileDlg.DoModal() != IDOK)	return;

	m_strSVGFile = SaveFileDlg.GetPathName();
	UpdateData(FALSE);

	CWaitCursor wc;
	::FontToSVG((LPTSTR)(LPCTSTR)m_strFontFile,(LPTSTR)(LPCTSTR)m_strSVGFile);

}	// end of void CFongSVGDlg::OnBnClickedSvgSavesvgBtn()
