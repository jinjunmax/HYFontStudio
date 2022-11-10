// ResetPsNameDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "HYFontSmartShaper.h"
#include "ResetPsNameDlg.h"
#include "afxdialogex.h"


// CResetPsNameDlg 对话框

IMPLEMENT_DYNAMIC(CResetPsNameDlg, CDialogEx)

CResetPsNameDlg::CResetPsNameDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_RSTNM_DLG, pParent)
	, m_strSrcFnt(_T(""))
	, m_strNameFile(_T(""))
	, m_strDstFile(_T(""))
{

}

CResetPsNameDlg::~CResetPsNameDlg()
{
}

void CResetPsNameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_RSTNM_SRCFNT_EDT, m_strSrcFnt);
	DDX_Text(pDX, IDC_RSTNM_NAME_EDT, m_strNameFile);
	DDX_Text(pDX, IDC_RSTNM_DSTFNT_EDT, m_strDstFile);
}


BEGIN_MESSAGE_MAP(CResetPsNameDlg, CDialogEx)
	ON_BN_CLICKED(IDC_RSTNM_SRCFNT_BTN, &CResetPsNameDlg::OnBnClickedRstnmSrcfntBtn)
	ON_BN_CLICKED(IDC_RSTNM_NAME_BTN, &CResetPsNameDlg::OnBnClickedRstnmNameBtn)
	ON_BN_CLICKED(IDC_RSTNM_DSTFNT_BTN, &CResetPsNameDlg::OnBnClickedRstnmDstfntBtn)
	ON_BN_CLICKED(IDC_RSTNM_PRS_BTN, &CResetPsNameDlg::OnBnClickedRstnmPrsBtn)
END_MESSAGE_MAP()

BOOL CResetPsNameDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE

}	// end of BOOL CResetPsNameDlg::OnInitDialog()

// CResetPsNameDlg 消息处理程序
void CResetPsNameDlg::OnBnClickedRstnmSrcfntBtn()
{
	TCHAR	szFilters[] = _T("TrueType File(*.ttf)|*.ttf||");
	CFileDialog  openFileDlg(TRUE, _T(""), _T(""), OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilters);
	if (openFileDlg.DoModal() != IDOK)	return;

	m_strSrcFnt = openFileDlg.GetPathName();

	UpdateData(FALSE);

}	// end of void CResetPsNameDlg::OnBnClickedRstnmSrcfntBtn()

void CResetPsNameDlg::OnBnClickedRstnmNameBtn()
{
	TCHAR	szFilters[] = _T("PsName File(*.txt)|*.txt||");
	CFileDialog  openFileDlg(TRUE, _T(""), _T(""), OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilters);
	if (openFileDlg.DoModal() != IDOK)	return;

	m_strNameFile = openFileDlg.GetPathName();

	UpdateData(FALSE);

}	// end of void CResetPsNameDlg::OnBnClickedRstnmNameBtn()

void CResetPsNameDlg::OnBnClickedRstnmDstfntBtn()
{
	TCHAR	szFilters[] = _T("TrueType File(*.ttf)|*.ttf||");
	CFileDialog  svFileDlg(FALSE, _T(""), _T(""), OFN_LONGNAMES, szFilters);
	if (svFileDlg.DoModal() != IDOK)	return;

	m_strDstFile = svFileDlg.GetPathName();

	UpdateData(FALSE);

}	// end of void CResetPsNameDlg::OnBnClickedRstnmDstfntBtn()

void CResetPsNameDlg::OnBnClickedRstnmPrsBtn()
{
	UpdateData(TRUE);

	if (m_strSrcFnt.IsEmpty()) {
		AfxMessageBox("源字库文件名为空");
		return;
	}

	if (m_strNameFile.IsEmpty()) {
		AfxMessageBox("name文件名为空");
		return;
	}

	if (m_strDstFile.IsEmpty()) {
		AfxMessageBox("目标字库文件名为空");
		return;
	}

	CHYFontCodec	srcFntCdec;
	if (HY_NOERROR != srcFntCdec.Decode((LPSTR)(LPCTSTR)m_strSrcFnt)){
		AfxMessageBox("字库文件解析失败");
		return;
	}

	std::vector<string> vtpsName;
	if (HY_NOERROR != ::HY_LoadPsNameFile((LPSTR)(LPCTSTR)m_strNameFile, vtpsName)) {
		AfxMessageBox("name文件解析失败");
		return;
	}

	// GID 0 固定为.notdef，这里不做处理
	int iMaxName = vtpsName.size();
	for (int i = 1; i < srcFntCdec.m_HYMaxp.numGlyphs; i++) {
		CHYGlyph& glyph = srcFntCdec.m_vtHYGlyphs[i];
		if (i!=iMaxName) {
			glyph.strPostName = vtpsName[i];			
		}
	}

	std::vector<unsigned long> vtFlag;
	srcFntCdec.GetTableFlag(vtFlag);

	HY_OPTION_PRM	tagOpnPrm;
	ZeroMemory(&tagOpnPrm,sizeof(HY_OPTION_PRM));
	tagOpnPrm.bCmplCMAP = true;

	if (srcFntCdec.Encode((LPSTR)(LPCTSTR)m_strDstFile, vtFlag, tagOpnPrm) == HY_NOERROR){
		AfxMessageBox("处理完成");
	}
	else{
		AfxMessageBox("处理失败");
	}

}	// end of void CResetPsNameDlg::OnBnClickedRstnmPrsBtn()
