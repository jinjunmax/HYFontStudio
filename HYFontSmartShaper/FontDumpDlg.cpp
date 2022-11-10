// FontDumpDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "HYFontSmartShaper.h"
#include "FontDumpDlg.h"
#include "afxdialogex.h"


// CFontDumpDlg 对话框

IMPLEMENT_DYNAMIC(CFontDumpDlg, CDialog)

CFontDumpDlg::CFontDumpDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DMP_DLG, pParent)
	, m_strFileName(_T(""))
{
	m_vtTBFlag.clear();

}	// end of CFontDumpDlg::CFontDumpDlg()

CFontDumpDlg::~CFontDumpDlg()
{

}	// end of CFontDumpDlg::~CFontDumpDlg()

void CFontDumpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_DMP_FILENM_EDT, m_strFileName);
}


BEGIN_MESSAGE_MAP(CFontDumpDlg, CDialog)
	ON_BN_CLICKED(IDC_DMP_FILENM_BTN, &CFontDumpDlg::OnBnClickedDmpFilenmBtn)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_DMP_AVAR_CHK, IDC_DMP_VVAR_CHK, OnTableCheck)
	ON_BN_CLICKED(IDC_DMP_DUMP_BTN, &CFontDumpDlg::OnBnClickedDmpDumpBtn)
END_MESSAGE_MAP()


// CFontDumpDlg 消息处理程序
void CFontDumpDlg::OnBnClickedDmpFilenmBtn()
{
	TCHAR	szFilters[] = _T("TrueType 文件(*.ttf)|*.ttf|OpenType 文件(*.otf)|*.otf||");
	CFileDialog  openFileDlg(TRUE, _T(""), _T(""), OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilters);
	if (openFileDlg.DoModal() != IDOK)	return;

	m_strFileName = openFileDlg.GetPathName();

	UpdateData(FALSE);

}	// end of void CFontDumpDlg::OnBnClickedDmpFilenmBtn()

void CFontDumpDlg::OnTableCheck(UINT uID)
{

}	// end of BOOL CFontDumpDlg::OnTableCheck()

BOOL CFontDumpDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE

}	// end of BOOL CFontDumpDlg::OnInitDialog()

void CFontDumpDlg::OnBnClickedDmpDumpBtn()
{
	UpdateData();

	m_vtTBFlag.clear();

	if (((CButton*)GetDlgItem(IDC_DMP_AVAR_CHK))->GetCheck() == BST_CHECKED){
		m_vtTBFlag.push_back(AVAR_TAG);
	}
	if (((CButton*)GetDlgItem(IDC_DMP_BASE_CHK))->GetCheck() == BST_CHECKED){
		m_vtTBFlag.push_back(BASE_TAG);
	}
	if (((CButton*)GetDlgItem(IDC_DMP_CBDT_CHK))->GetCheck() == BST_CHECKED){
		m_vtTBFlag.push_back(CBDT_TAG);
	}
	if (((CButton*)GetDlgItem(IDC_DMP_CBLC_CHK))->GetCheck() == BST_CHECKED){
		m_vtTBFlag.push_back(CBLC_TAG);
	}
	if (((CButton*)GetDlgItem(IDC_DMP_CFF_CHK))->GetCheck() == BST_CHECKED){
		m_vtTBFlag.push_back(CFF_TAG);
	}
	if (((CButton*)GetDlgItem(IDC_DMP_CFF2_CHK))->GetCheck() == BST_CHECKED){
		m_vtTBFlag.push_back(CFF2_TAG);
	}
	if (((CButton*)GetDlgItem(IDC_DMP_CAMP_CHK))->GetCheck() == BST_CHECKED){
		m_vtTBFlag.push_back(CMAP_TAG);
	}
	if (((CButton*)GetDlgItem(IDC_DMP_COLR_CHK))->GetCheck() == BST_CHECKED){
		m_vtTBFlag.push_back(COLR_TAG);
	}
	if (((CButton*)GetDlgItem(IDC_DMP_CPAL_CHK))->GetCheck() == BST_CHECKED){
		m_vtTBFlag.push_back(CPAL_TAG);
	}
	if (((CButton*)GetDlgItem(IDC_DMP_cvar_CHK))->GetCheck() == BST_CHECKED){
		m_vtTBFlag.push_back(CVAR_TAG);
	}
	if (((CButton*)GetDlgItem(IDC_DMP_CVT_CHK))->GetCheck() == BST_CHECKED){
		m_vtTBFlag.push_back(CVT_TAG);
	}
	if (((CButton*)GetDlgItem(IDC_DMP_DSIG_CHK))->GetCheck() == BST_CHECKED){
		m_vtTBFlag.push_back(DSIG_TAG);
	}
	if (((CButton*)GetDlgItem(IDC_DMP_EBDT_CHK))->GetCheck() == BST_CHECKED){
		m_vtTBFlag.push_back(EBDT_TAG);
	}
	if (((CButton*)GetDlgItem(IDC_DMP_EBLC_CHK))->GetCheck() == BST_CHECKED){
		m_vtTBFlag.push_back(EBLC_TAG);
	}
	if (((CButton*)GetDlgItem(IDC_DMP_EBSC_CHK))->GetCheck() == BST_CHECKED){
		m_vtTBFlag.push_back(EBSC_TAG);
	}
	if (((CButton*)GetDlgItem(IDC_DMP_FPGM_CHK))->GetCheck() == BST_CHECKED){
		m_vtTBFlag.push_back(FPGM_TAG);
	}
	if (((CButton*)GetDlgItem(IDC_DMP_FVAR_CHK))->GetCheck() == BST_CHECKED){
		m_vtTBFlag.push_back(FVAR_TAG);
	}
	if (((CButton*)GetDlgItem(IDC_DMP_GASP_CHK))->GetCheck() == BST_CHECKED){
		m_vtTBFlag.push_back(GASP_TAG);
	}
	if (((CButton*)GetDlgItem(IDC_DMP_GDEF_CHK))->GetCheck() == BST_CHECKED){
		m_vtTBFlag.push_back(GDEF_TAG);
	}
	if (((CButton*)GetDlgItem(IDC_DMP_GLYF_CHK))->GetCheck() == BST_CHECKED){
		m_vtTBFlag.push_back(GLYF_TAG);
	}
	if (((CButton*)GetDlgItem(IDC_DMP_GPOS_CHK))->GetCheck() == BST_CHECKED){
		m_vtTBFlag.push_back(GPOS_TAG);
	}
	if (((CButton*)GetDlgItem(IDC_DMP_GSUB_CHK))->GetCheck() == BST_CHECKED){
		m_vtTBFlag.push_back(GSUB_TAG);
	}
	if (((CButton*)GetDlgItem(IDC_DMP_GVAR_CHK))->GetCheck() == BST_CHECKED){
		m_vtTBFlag.push_back(GVAR_TAG);
	}
	if (((CButton*)GetDlgItem(IDC_DMP_HDMX_CHK))->GetCheck() == BST_CHECKED){
		m_vtTBFlag.push_back(HDMX_TAG);
	}
	if (((CButton*)GetDlgItem(IDC_DMP_HEAD_CHK))->GetCheck() == BST_CHECKED){
		m_vtTBFlag.push_back(HEAD_TAG);
	}
	if (((CButton*)GetDlgItem(IDC_DMP_HHEA_CHK))->GetCheck() == BST_CHECKED){
		m_vtTBFlag.push_back(HHEA_TAG);
	}
	if (((CButton*)GetDlgItem(IDC_DMP_HMTX_CHK))->GetCheck() == BST_CHECKED){
		m_vtTBFlag.push_back(HMTX_TAG);
	}
	if (((CButton*)GetDlgItem(IDC_DMP_HVAR_CHK))->GetCheck() == BST_CHECKED){
		m_vtTBFlag.push_back(HVAR_TAG);
	}
	if (((CButton*)GetDlgItem(IDC_DMP_JSTF_CHK))->GetCheck() == BST_CHECKED){
		m_vtTBFlag.push_back(JSTF_TAG);
	}
	if (((CButton*)GetDlgItem(IDC_DMP_KERN_CHK))->GetCheck() == BST_CHECKED){
		m_vtTBFlag.push_back(KERN_TAG);
	}
	if (((CButton*)GetDlgItem(IDC_DMP_LOCA_CHK))->GetCheck() == BST_CHECKED){
		m_vtTBFlag.push_back(LOCA_TAG);
	}
	if (((CButton*)GetDlgItem(IDC_DMP_LTSH_CHK))->GetCheck() == BST_CHECKED){
		m_vtTBFlag.push_back(LTSH_TAG);
	}
	if (((CButton*)GetDlgItem(IDC_DMP_MATH_CHK))->GetCheck() == BST_CHECKED){
		m_vtTBFlag.push_back(MATH_TAG);
	}
	if (((CButton*)GetDlgItem(IDC_DMP_MAXP_CHK))->GetCheck() == BST_CHECKED){
		m_vtTBFlag.push_back(MAXP_TAG);
	}
	if (((CButton*)GetDlgItem(IDC_DMP_MERG_CHK))->GetCheck() == BST_CHECKED){
		m_vtTBFlag.push_back(MERG_TAG);
	}
	if (((CButton*)GetDlgItem(IDC_DMP_META_CHK))->GetCheck() == BST_CHECKED){
		m_vtTBFlag.push_back(META_TAG);
	}
	if (((CButton*)GetDlgItem(IDC_DMP_MVAR_CHK))->GetCheck() == BST_CHECKED){
		m_vtTBFlag.push_back(MVAR_TAG);
	}
	if (((CButton*)GetDlgItem(IDC_DMP_NAME_CHK))->GetCheck() == BST_CHECKED){
		m_vtTBFlag.push_back(NAME_TAG);
	}
	if (((CButton*)GetDlgItem(IDC_DMP_OS2_CHK))->GetCheck() == BST_CHECKED){
		m_vtTBFlag.push_back(OS2_TAG);
	}
	if (((CButton*)GetDlgItem(IDC_DMP_PCLT_CHK))->GetCheck() == BST_CHECKED){
		m_vtTBFlag.push_back(PCLT_TAG);
	}
	if (((CButton*)GetDlgItem(IDC_DMP_POST_CHK))->GetCheck() == BST_CHECKED){
		m_vtTBFlag.push_back(POST_TAG);
	}
	if (((CButton*)GetDlgItem(IDC_DMP_PREP_CHK))->GetCheck() == BST_CHECKED){
		m_vtTBFlag.push_back(PREP_TAG);
	}
	if (((CButton*)GetDlgItem(IDC_DMP_SBIX_CHK))->GetCheck() == BST_CHECKED){
		m_vtTBFlag.push_back(SBIX_TAG);
	}
	if (((CButton*)GetDlgItem(IDC_DMP_STAT_CHK))->GetCheck() == BST_CHECKED){
		m_vtTBFlag.push_back(STAT_TAG);
	}
	if (((CButton*)GetDlgItem(IDC_DMP_SVG_CHK))->GetCheck() == BST_CHECKED){
		m_vtTBFlag.push_back(SVG_TAG);
	}
	if (((CButton*)GetDlgItem(IDC_DMP_VDMX_CHK))->GetCheck() == BST_CHECKED){
		m_vtTBFlag.push_back(VDMX_TAG);
	}
	if (((CButton*)GetDlgItem(IDC_DMP_VHEA_CHK))->GetCheck() == BST_CHECKED){
		m_vtTBFlag.push_back(VHEA_TAG);
	}
	if (((CButton*)GetDlgItem(IDC_DMP_VMTX_CHK))->GetCheck() == BST_CHECKED){
		m_vtTBFlag.push_back(VMTX_TAG);
	}
	if (((CButton*)GetDlgItem(IDC_DMP_VORG_CHK))->GetCheck() == BST_CHECKED){
		m_vtTBFlag.push_back(VORG_TAG);
	}
	if (((CButton*)GetDlgItem(IDC_DMP_VVAR_CHK))->GetCheck() == BST_CHECKED){
		m_vtTBFlag.push_back(VVAR_TAG);
	}

	TCHAR	szFilters[] = _T("xml 文件(*.xml)|*.xml||");
	CFileDialog  SaveFileDlg(FALSE, _T(""), _T(""), OFN_LONGNAMES, szFilters);
	if (SaveFileDlg.DoModal() != IDOK)	return;

	CString strDumpFile = SaveFileDlg.GetPathName();

	CHYFontCodec  fontCodec;
	fontCodec.Dump((LPSTR)(LPCTSTR)m_strFileName, (LPSTR)(LPCTSTR)strDumpFile, m_vtTBFlag);

		
}	// end of void CFontDumpDlg::OnBnClickedDmpDumpBtn()
