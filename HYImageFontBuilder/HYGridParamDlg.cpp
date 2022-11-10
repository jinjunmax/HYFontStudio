// HYGridParamDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "HYImageFontBuilder.h"
#include "HYGridParamDlg.h"
#include "afxdialogex.h"
#include "HYImageFontBuilderDoc.h"
//#include "HYImageFontBuilderView.h"

extern class CHYImageFontBuilderView;
// CHYGridParamDlg 对话框
IMPLEMENT_DYNAMIC(CHYGridParamDlg, CDialog)

CHYGridParamDlg::CHYGridParamDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_GRIDPARAM_DLG, pParent)
	, m_iMaxArea(0)
	, m_iMinArea(0)
	, m_iMaxAngle(100)
	, m_iMinAngle(80)
	, m_strMtxCode(_T(""))
	, Leftedge(10)
	, Topedge(1000)
	, Rightedge(10)
	, Btmedge(10)
	, m_iColum(4)
	, m_iRow(5)
	, m_iGrayThres(200)
{
	m_pImageGridSetWnd = NULL;

}	//end of CHYGridParamDlg::CHYGridParamDlg()

CHYGridParamDlg::~CHYGridParamDlg()
{

}	// end of CHYGridParamDlg::~CHYGridParamDlg()

void CHYGridParamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_MXR_EDT, m_iMaxArea);
	DDX_Text(pDX, IDC_MNR_EDT, m_iMinArea);
	DDX_Text(pDX, IDC_MXANGL_EDT, m_iMaxAngle);
	DDX_Text(pDX, IDC_MNANGL_EDT, m_iMinAngle);

	DDX_Text(pDX, IDC_MTXCDINF_EDT, m_strMtxCode);
	DDX_Text(pDX, IDC_LEFTEDT, Leftedge);
	DDX_Text(pDX, IDC_TOPEDT, Topedge);
	DDX_Text(pDX, IDC_RIGHTEDT, Rightedge);
	DDX_Text(pDX, IDC_BTMEDT, Btmedge);
	DDX_Text(pDX, IDC_ROWEDT, m_iColum);
	DDX_Text(pDX, IDC_COLEDT, m_iRow);
	DDX_Text(pDX, IDC_EDT_GRYTHRE, m_iGrayThres);
}	// end of void CHYGridParamDlg::DoDataExchange()

BEGIN_MESSAGE_MAP(CHYGridParamDlg, CDialog)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON1, &CHYGridParamDlg::OnBnClickedButton1)
	ON_MESSAGE(MSG_TRACERECT, &CHYGridParamDlg::OnTraceRect)
	ON_BN_CLICKED(IDC_TEST_BTN, &CHYGridParamDlg::OnBnClickedTestBtn)
	ON_BN_CLICKED(IDOK, &CHYGridParamDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCLE, &CHYGridParamDlg::OnBnClickedCancle)	
	ON_BN_CLICKED(IDC_BTN_CHKRCT, &CHYGridParamDlg::OnBnClickedBtnChkrct)
	ON_BN_CLICKED(IDC_RCINF_BTN, &CHYGridParamDlg::OnBnClickedRcinfBtn)
	ON_BN_CLICKED(IDC_BTN_CHKTEST, &CHYGridParamDlg::OnBnClickedBtnChktest)
END_MESSAGE_MAP()

BOOL CHYGridParamDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	// TODO:  在此添加额外的初始化
	CRect rcPrvw;
	CWnd* pWnd = GetDlgItem(IDC_PREVIEW_STC);
	if (pWnd)
	{
		if (pWnd->GetSafeHwnd())
		{
			pWnd->GetWindowRect(rcPrvw);
			ScreenToClient(rcPrvw);

			rcPrvw.right = rcPrvw.left + PVEEM+2;
			rcPrvw.bottom = rcPrvw.top + PVEEM+2;
			
			m_pImageGridSetWnd = new CImageGridSetWnd();
			if (m_pImageGridSetWnd->CreateEx(0, NULL, NULL, WS_BORDER|WS_CHILD | WS_TABSTOP | WS_VISIBLE, rcPrvw, this, 90001))
			{
				m_pImageGridSetWnd->ShowWindow(SW_SHOW);
			}
		}
	}

	return TRUE;

}	// end of BOOL CHYGridParamDlg::OnInitDialog()

// CHYGridParamDlg 消息处理程序
void CHYGridParamDlg::OnDestroy()
{
	CDialog::OnDestroy();

	if (m_pImageGridSetWnd)
	{
		if (m_pImageGridSetWnd->GetSafeHwnd())
		{
			m_pImageGridSetWnd->DestroyWindow();
		}

		delete[] m_pImageGridSetWnd;
	}

	m_pImageGridSetWnd = NULL;

}	// end of void CHYGridParamDlg::OnDestroy()

LRESULT	CHYGridParamDlg::OnTraceRect(WPARAM waram, LPARAM lparam)
{
	CRect&  rc = ::XSysproxy().m_rctTraceRect;

	m_iMaxArea = rc.Height()*rc.Width()* 1.2f;
	m_iMinArea = rc.Height()*rc.Width()* 0.5f;

	UpdateData(FALSE);

	return 0L;

}	// end of LRESULT	CHYGridParamDlg::OnTraceRect()

// 打开测试图像
void CHYGridParamDlg::OnBnClickedButton1()
{
	TCHAR	szFilters[] = _T("图像 文件(*.jpg;*bmp;*png)|*.jpg;*bmp;*.png||");
	CFileDialog  openFileDlg(TRUE, _T(""), _T(""), OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilters);
	if (openFileDlg.DoModal() != IDOK)	
	{	
		return;
	}

	m_strImage = openFileDlg.GetPathName();
	if (m_pImageGridSetWnd)
	{
		if (m_pImageGridSetWnd->m_hWnd)
		{
			m_pImageGridSetWnd->SetImageFile(m_strImage);
		}
	}
	this->SetWindowText(m_strImage);

}	// end of void CHYGridParamDlg::OnBnClickedButton1()

void CHYGridParamDlg::OnBnClickedButton2()
{
	//TCHAR	szFilters[] = _T("图像 文件(*.*)|*.*||");
	//CFileDialog  openFileDlg(TRUE, _T(""), _T(""), OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilters);

	//AfxMessageBox("test");
	//openFileDlg.DoModal();
	
	/*
	if (openFileDlg.DoModal() != IDOK)
	{
		AfxMessageBox("文件打开失败！");
		return;
	}
	*/

}	//end of void CHYGridParamDlg::OnBnClickedButton2()

void CHYGridParamDlg::OnBnClickedTestBtn()
{
	if (m_strImage.IsEmpty()) return;
	CWaitCursor wc;
	XDoc().OpcvImgTest((LPTSTR)(LPCTSTR)m_strImage,m_iMinArea, m_iMaxArea, m_iMinAngle, m_iMaxAngle);

}	// end of void CHYGridParamDlg::OnBnClickedTestBtn()

//显示单图矩阵信息
void CHYGridParamDlg::OnBnClickedRcinfBtn()
{
	//XDoc().makeMatirxCode();
	m_strMtxCode = _T("");
	CStringArray szMatirxInf;	
	XDoc().OpcvMatirxCodeTest((LPTSTR)(LPCTSTR)m_strImage, m_iMinArea, m_iMaxArea, m_iMinAngle, m_iMaxAngle, m_strMtxCode);

	UpdateData(FALSE);	

}	// end of void CHYGridParamDlg::OnBnClickedRcinfBtn()

void CHYGridParamDlg::OnBnClickedBtnChkrct()
{
	TCHAR	szFilters[] = _T("图像 文件(*.jpg;*.bmp;*.png)|*.jpg;*.bmp;*.png||");
	DWORD	MAXFILE = 2048;
	CFileDialog  openFileDlg(TRUE, _T(""), _T(""), OFN_ALLOWMULTISELECT | OFN_READONLY | OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilters);
	openFileDlg.m_ofn.lpstrFile = new TCHAR[MAXFILE*(MAX_PATH + 1) + 1];
	openFileDlg.m_ofn.nMaxFile = MAXFILE * (MAX_PATH + 1) + 1;
	if (openFileDlg.DoModal() != IDOK)	return;

	CStringArray szFiles;
	POSITION pos = openFileDlg.GetStartPosition();
	while (pos != NULL)
	{
		CString szPathName = openFileDlg.GetNextPathName(pos);
		szFiles.Add(szPathName);
	}

	CWaitCursor wc;
	XDoc().OpcvImgTest1(szFiles, m_iMinArea, m_iMaxArea, m_iMinAngle, m_iMaxAngle);
	AfxMessageBox("矩形测试完成");
	
}	// end of void CHYGridParamDlg::OnBnClickedBtnChkrct()

void CHYGridParamDlg::OnBnClickedOk()
{	
	UpdateData();

	::XSysproxy().m_iMaxAngle	= m_iMaxAngle;
	::XSysproxy().m_iMinAngle	= m_iMinAngle;
	::XSysproxy().m_iMaxArea	= m_iMaxArea;
	::XSysproxy().m_iMinArea	= m_iMinArea;	

	CDialog::OnOK();

}	// end of void CHYGridParamDlg::OnBnClickedOk()

void CHYGridParamDlg::OnBnClickedCancle()
{	
	CDialog::OnCancel();

}	// end of void CHYGridParamDlg::OnBnClickedCancle()

void CHYGridParamDlg::OnBnClickedBtnChktest()
{
	if (m_strImage.IsEmpty()) return;
	
	UpdateData();

	int iDirect = GetCheckedRadioButton(IDC_RD_DIRCT_H, IDC_RD_DIRCT_V)-5000;

	CWaitCursor wc;
	::XSysproxy().m_hyFontCodec.SetDefault();

	CString strFeatureFile;
	strFeatureFile.Format(_T("%sdata\\Profile.xml"), ::XSysproxy().m_strAppDir);
	::XSysproxy().LoadFontFeature(strFeatureFile, ::XSysproxy().m_hyFontCodec);
	::XSysproxy().ImageToGlyph((LPTSTR)(LPCSTR)m_strImage, Topedge, Leftedge, Rightedge, Btmedge, m_iColum, m_iRow,m_iGrayThres, iDirect);
	::XSysproxy().m_hyFontCodec.CountFontBound();


	TCHAR		szFilters[] = _T("字表文件(*.txt)|*.txt||");
	CFileDialog  openFileDlg(TRUE, _T(""), _T(""), OFN_READONLY | OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilters);
	if (openFileDlg.DoModal() != IDOK)	return;

	::XSysproxy().m_vtUnicode.clear();
	CString strTxtPath = openFileDlg.GetPathName();
	::HY_TXTFileToUniArray((LPTSTR)(LPCTSTR)strTxtPath, ::XSysproxy().m_vtUnicode);
	

	TCHAR szFilters1[] = _T("字库文件(*.ttf)|*.ttf||");
	CFileDialog  openFileDlg1(FALSE, _T(""), _T(""), OFN_READONLY | OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilters1);
	if (openFileDlg1.DoModal() != IDOK)	return;

	// unicode编码匹配字形
	for (size_t i = 0; i <::XSysproxy().m_vtUnicode.size(); i++)
	{
		if (i < ::XSysproxy().m_hyFontCodec.m_vtHYGlyphs.size())
		{
			::XSysproxy().m_hyFontCodec.m_vtHYGlyphs[i].vtUnicode.push_back(::XSysproxy().m_vtUnicode[i]);
		}
	}

	//::XSysproxy().m_hyFontCodec.VerifyFont();

	size_t stGlyphNum = ::XSysproxy().m_hyFontCodec.m_vtHYGlyphs.size();
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
	ulTableFlag.push_back(GASP_TAG);
	ulTableFlag.push_back(GLYF_TAG);
	ulTableFlag.push_back(LOCA_TAG);
	ulTableFlag.push_back(PREP_TAG);
	
	CString strFontName = openFileDlg1.GetPathName();
	::XSysproxy().m_hyFontCodec.Encode((LPSTR)(LPCTSTR)strFontName, ulTableFlag);

}	// end of void CHYGridParamDlg::OnBnClickedBtnChktest()
