// ParamSetupDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "HYImageFontBuilder.h"
#include "ParamSetupDlg.h"
#include "afxdialogex.h"


// CParamSetupDlg 对话框

IMPLEMENT_DYNAMIC(CParamSetupDlg, CDialog)

CParamSetupDlg::CParamSetupDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_PRM_SETUP_DLG, pParent)
	, m_iTopEdge(0)
	, m_iLeftEdge(0)
	, m_iBottomEdge(0)
	, m_iRightEdge(0)
	, m_iRows(0)
	, m_icolomns(0)
	, m_iGrayThre(0)
	, m_iAvrdis(0)
	, m_iDisWidth(0)
	, m_iSingleBorder(0)
	, m_iGridThres(0)
	, m_iGridLenThres(0)
	, m_strGridImgNumber(_T("0x20000"))
	, m_strGridImgDir(_T("D:\\"))
{

}

CParamSetupDlg::~CParamSetupDlg()
{
}

void CParamSetupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PMST_TOP_EDT, m_iTopEdge);
	DDX_Text(pDX, IDC_PMST_LEFT_EDT, m_iLeftEdge);
	DDX_Text(pDX, IDC_PMST_BTM_EDT, m_iBottomEdge);
	DDX_Text(pDX, IDC_PMST_RIGHT_EDT, m_iRightEdge);
	DDX_Text(pDX, IDC_PMST_COL_EDT, m_iRows);
	DDX_Text(pDX, IDC_PMST_ROW_EDT, m_icolomns);
	DDX_Text(pDX, IDC_PMST_GRYTHRE_EDT, m_iGrayThre);
	DDX_Text(pDX, IDC_PMST_DISTHRE_EDT, m_iAvrdis);
	DDX_Text(pDX, IDC_PMST_WTHTHRE_EDT, m_iDisWidth);
	DDX_Text(pDX, IDC_PMST_SINGLEBORD_EDT, m_iSingleBorder);
	DDX_Text(pDX, IDC_PMST_GRIDTHRES_EDT, m_iGridThres);
	DDX_Text(pDX, IDC_PMST_GRIDLENTHRES_EDT, m_iGridLenThres);
	DDX_Text(pDX, IDC_SPLT_IMGNO, m_strGridImgNumber);
	DDX_Text(pDX, IDC_SPLT_SAVEDIR, m_strGridImgDir);
}

BEGIN_MESSAGE_MAP(CParamSetupDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CParamSetupDlg::OnBnClickedOk)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CParamSetupDlg 消息处理程序
void CParamSetupDlg::OnBnClickedOk()
{	
	UpdateData(TRUE);

	XSysproxy().m_tagScanPrm.iScanDirection = GetCheckedRadioButton(IDC_PMST_RD_DIRCT_H, IDC_PMST_RD_DIRCT_V) - 2005;
	XSysproxy().m_tagScanPrm.iTopEdge = m_iTopEdge;
	XSysproxy().m_tagScanPrm.iLeftEdge = m_iLeftEdge;
	XSysproxy().m_tagScanPrm.iBottomEdge = m_iBottomEdge;
	XSysproxy().m_tagScanPrm.iRightEdge = m_iRightEdge;
	XSysproxy().m_tagScanPrm.iRows = m_iRows;
	XSysproxy().m_tagScanPrm.iColmns = m_icolomns;
	XSysproxy().m_tagScanPrm.iGrayThre = m_iGrayThre;
	XSysproxy().m_tagScanPrm.iGridThres = m_iGridThres;
	XSysproxy().m_tagScanPrm.iAvrdis = m_iAvrdis;
	XSysproxy().m_tagScanPrm.iDisWidth = m_iDisWidth;
	XSysproxy().m_tagScanPrm.iSingleBorder = m_iSingleBorder;
	XSysproxy().m_tagScanPrm.iGridLenThres = m_iGridLenThres;
	XSysproxy().m_tagScanPrm.iGridImgNumber = strtol(m_strGridImgNumber,NULL,16);

	ZeroMemory(XSysproxy().m_tagScanPrm.szGridImgDir,MAX_PATH);
	strcpy_s(XSysproxy().m_tagScanPrm.szGridImgDir,MAX_PATH, (LPSTR)(LPCSTR)m_strGridImgDir);
	CDialog::OnOK();

}	// end of void CParamSetupDlg::OnBnClickedOk()

BOOL CParamSetupDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (XSysproxy().m_tagScanPrm.iScanDirection == 0)
	{
		CheckRadioButton(IDC_PMST_RD_DIRCT_H, IDC_PMST_RD_DIRCT_V, IDC_PMST_RD_DIRCT_H);
	}
	else if (XSysproxy().m_tagScanPrm.iScanDirection == 1)
	{
		CheckRadioButton(IDC_PMST_RD_DIRCT_H, IDC_PMST_RD_DIRCT_V, IDC_PMST_RD_DIRCT_V);
	}

	m_iTopEdge = XSysproxy().m_tagScanPrm.iTopEdge;
	m_iLeftEdge = XSysproxy().m_tagScanPrm.iLeftEdge;
	m_iBottomEdge = XSysproxy().m_tagScanPrm.iBottomEdge;
	m_iRightEdge = XSysproxy().m_tagScanPrm.iRightEdge;
	m_iRows = XSysproxy().m_tagScanPrm.iRows;
	m_icolomns = XSysproxy().m_tagScanPrm.iColmns;
	m_iGrayThre = XSysproxy().m_tagScanPrm.iGrayThre;
	m_iAvrdis = XSysproxy().m_tagScanPrm.iAvrdis;
	m_iDisWidth = XSysproxy().m_tagScanPrm.iDisWidth;
	m_iSingleBorder = XSysproxy().m_tagScanPrm.iSingleBorder;
	m_iGridThres = XSysproxy().m_tagScanPrm.iGridThres;
	m_iGridLenThres = XSysproxy().m_tagScanPrm.iGridLenThres;
	m_strGridImgNumber.Format("0x%X", XSysproxy().m_tagScanPrm.iGridImgNumber);
	m_strGridImgDir.Format("%s", XSysproxy().m_tagScanPrm.szGridImgDir);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE

}	// end of BOOL CParamSetupDlg::OnInitDialog()

void CParamSetupDlg::OnDestroy()
{
	CDialog::OnDestroy();


}	// end of void CParamSetupDlg::OnDestroy()
