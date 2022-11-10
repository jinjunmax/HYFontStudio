// HYImgProcDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "HYImageFontBuilder.h"
#include "HYImgProcDlg.h"
#include "afxdialogex.h"


// CHYImgProcDlg 对话框

IMPLEMENT_DYNAMIC(CHYImgProcDlg, CDialog)

CHYImgProcDlg::CHYImgProcDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_IMGPROC_DLG, pParent)
	, m_bInvert(FALSE)
	, m_bMono(FALSE)
{
	m_strFGImageName = "";
	m_strBGImageName = "";
	m_OutImg.release();
	m_strCurFile = "";

}	// end of CHYImgProcDlg::CHYImgProcDlg()

CHYImgProcDlg::~CHYImgProcDlg()
{

}	//end of CHYImgProcDlg::~CHYImgProcDlg()

void CHYImgProcDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_IMGP_PREVIEW_STC, m_prwWnd);
	DDX_Control(pDX, IDC_IMGP_CONTRAST_SLD, m_sldContrast);
	DDX_Control(pDX, IDC_IMGP_LIGHT_SLD, m_sldLight);
	DDX_Control(pDX, IDC_IMGP_MONO_SLD, m_sldBWThres);
	DDX_Check(pDX, IDC_IMGP_INVERT_CHK, m_bInvert);
	DDX_Check(pDX, IDC_IMGP_BW_CHK, m_bMono);
	DDX_Control(pDX, IDC_IMGPBATCH_PRS, m_psctrlImgpress);
}	// end of void CHYImgProcDlg::DoDataExchange()

BEGIN_MESSAGE_MAP(CHYImgProcDlg, CDialog)
	ON_WM_DESTROY()	
	ON_BN_CLICKED(IDC_IMGPBATCH_BTN, &CHYImgProcDlg::OnBnClickedImgpbatch)
	ON_COMMAND(ID_IMGPRD_OPENFILE_MU, &CHYImgProcDlg::OnImgprdOpenfileMu)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_IMGP_INVERT_CHK, &CHYImgProcDlg::OnBnClickedImgpInvertChk)	
	ON_WM_HSCROLL()	
	ON_BN_CLICKED(IDC_IMGP_BW_CHK, &CHYImgProcDlg::OnBnClickedImgpBwChk)
	ON_WM_TIMER()
	ON_MESSAGE(WM_SCRLMSG, OnSCRLMsgHandler)
	ON_MESSAGE(WM_STPMSG, OnStepMsgHandler)
	ON_COMMAND_RANGE(IDC_IMGP_BG_RD, IDC_IMGP_WG_RD, &CHYImgProcDlg::OnRadioButton)
	ON_COMMAND(ID_IMGPRD_SAVEFILE_MU, &CHYImgProcDlg::OnImgprdSavefileMu)
END_MESSAGE_MAP()

// CHYImgProcDlg 消息处理程序
BOOL CHYImgProcDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_Menu.LoadMenu(IDR_IMGPRD_MN);
	this->SetMenu(&m_Menu);	

	// 黑色背景
	if (::XSysproxy().m_tagImgOptim.BKType == 0) {		
		CheckRadioButton(IDC_IMGP_BG_RD, IDC_IMGP_WG_RD, IDC_IMGP_BG_RD);
	}	//白色背景
	else if (::XSysproxy().m_tagImgOptim.BKType == 1) {		
		CheckRadioButton(IDC_IMGP_BG_RD, IDC_IMGP_WG_RD, IDC_IMGP_WG_RD);
	}

	m_sldContrast.SetRange(-50, 250);	
	m_sldContrast.SetTicFreq(1);
	m_sldContrast.SetPageSize(10);
	m_sldLight.SetRange(-150,150);
	m_sldLight.SetTicFreq(1);
	m_sldLight.SetPageSize(10);
	m_sldBWThres.SetRange(0, 255);
	m_sldBWThres.SetTicFreq(1);
	m_sldBWThres.SetPageSize(10);

	m_bMono = ::XSysproxy().m_tagImgOptim.bMono;
	m_bInvert = ::XSysproxy().m_tagImgOptim.InVert;
	m_sldContrast.SetPos(::XSysproxy().m_tagImgOptim.Contrast);
	m_sldLight.SetPos(::XSysproxy().m_tagImgOptim.Light);
	m_sldBWThres.SetPos(::XSysproxy().m_tagImgOptim.MonoThres);

	m_iLight = ::XSysproxy().m_tagImgOptim.Light;
	m_iContrast=::XSysproxy().m_tagImgOptim.Contrast;
	m_iBWThres = ::XSysproxy().m_tagImgOptim.MonoThres;

	SetDlgItemInt(IDC_IMGP_CONTRAST_EDT, m_iContrast);
	SetDlgItemInt(IDC_IMGP_LIGHT_EDT, m_iLight);
	SetDlgItemInt(IDC_IMGP_MONO_EDT, m_iBWThres);

	m_psctrlImgpress.SetRange(0,100);
	m_psctrlImgpress.SetPos(0);

	// 黑色
	if (::XSysproxy().m_tagImgOptim.BKType == 0) {
		m_strBGImageName = ::XSysproxy().m_strBlackBkName;
	}	//白色
	else if (::XSysproxy().m_tagImgOptim.BKType == 1) {
		m_strBGImageName = ::XSysproxy().m_strWhiteBkName;
	}

	MakeBGImage(cv::String(m_strBGImageName), m_BGimg);	

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE

}	// end of BOOL CHYImgProcDlg::OnInitDialog()

void CHYImgProcDlg::OnDestroy()
{
	UpdateData();

	::XSysproxy().m_tagImgOptim.Contrast = m_sldContrast.GetPos();
	::XSysproxy().m_tagImgOptim.Light =	m_sldLight.GetPos();
	::XSysproxy().m_tagImgOptim.MonoThres = m_sldBWThres.GetPos();
	::XSysproxy().m_tagImgOptim.bMono = m_bMono;
	::XSysproxy().m_tagImgOptim.InVert = m_bInvert;

	int iBKType = GetCheckedRadioButton(IDC_IMGP_BG_RD, IDC_IMGP_WG_RD)- IDC_IMGP_BG_RD;
	::XSysproxy().m_tagImgOptim.BKType = iBKType;

	KillTimer(m_iTimerID);

	CDialog::OnDestroy();	

}	// end of void CHYImgProcDlg::OnDestroy()

void CHYImgProcDlg::OnBnClickedImgpbatch()
{
	CWaitCursor wc;

	TCHAR szFilters[] = _T("图像文件(*.bmp;*.jpg;*.png;*.TIF)|*.bmp;*.jpg;*.png;*.TIF||");
	DWORD	MAXFILE = 204800;
	CFileDialog  OpenFileDlg(TRUE, _T(""), _T(""), OFN_READONLY | OFN_LONGNAMES | OFN_FILEMUSTEXIST| OFN_ALLOWMULTISELECT, szFilters,this,0,FALSE);	
	OpenFileDlg.m_ofn.lpstrFile = new TCHAR[MAXFILE*(MAX_PATH + 1) + 1];
	OpenFileDlg.m_ofn.nMaxFile = MAXFILE * (MAX_PATH + 1) + 1;

	if (OpenFileDlg.DoModal() != IDOK) return;	

	std::vector<string> vtloadFlies;
	int nCount = 0;
	POSITION pos = OpenFileDlg.GetStartPosition();
	while (pos != NULL)	{
		CString szPathName = OpenFileDlg.GetNextPathName(pos);
		vtloadFlies.push_back(std::string(szPathName));
	}			
	cv::Mat BGimg;
	size_t stNums = vtloadFlies.size();
	for (size_t i = 0; i < stNums; i++)	{
		m_strCurFile = vtloadFlies[i];
		MakeBGImage(cv::String(m_strBGImageName), BGimg);
		MergeBackground(cv::String(m_strCurFile), BGimg);

		m_OutImg.release();
		AdjustImage(BGimg, m_OutImg);
		string saveFile = HY_GetDirFromPath(m_strCurFile) + HY_GetFileNameFromPath(m_strCurFile) + ".bmp";
		cv::imwrite(cv::String(saveFile), m_OutImg);

		int iStp = (((i + 1) * 100.0 / stNums) );
		SendMessage(WM_STPMSG, iStp,0L);
	}

	SetDlgItemText(IDC_IMGP_CURFILE_EDT, "处理完成");
	UpdateWindow();

}	// end of void CHYImgProcDlg::OnBnClickedImgpbatch()

void CHYImgProcDlg::OnImgprdOpenfileMu()
{
	UpdateData();

	TCHAR szFilters[] = _T("图像文件(*.bmp;*.jpg;*.png;*.TIF)|*.bmp;*.jpg;*.png;*.TIF||");
	CFileDialog  OpenFileDlg(TRUE, _T(""), _T(""), OFN_READONLY | OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilters);

	if (OpenFileDlg.DoModal() != IDOK) return;

	m_strFGImageName = OpenFileDlg.GetPathName();
	m_BGimg.release();

	MakeBGImage(cv::String(m_strBGImageName), m_BGimg);
	MergeBackground(cv::String(m_strFGImageName), m_BGimg);
	
	cv::Mat OutImg;
	AdjustImage(m_BGimg, OutImg);
	m_prwWnd.SetMat(OutImg);
	
}	// end of void CHYImgProcDlg::OnImgprdOpenfileMu()

void CHYImgProcDlg::OnImgprdSavefileMu()
{
	TCHAR szFilters[] = _T("jpeg文件(*.jpg)|*.jpg||");

	CFileDialog  OpenFileDlg(FALSE, _T(""), _T(""), OFN_READONLY | OFN_LONGNAMES, szFilters);

	if (OpenFileDlg.DoModal() != IDOK) return;

	CString saveFileName = OpenFileDlg.GetPathName();	

	cv::Mat OutImg;
	AdjustImage(m_BGimg, OutImg);
	cv::imwrite(cv::String(saveFileName), OutImg);

}	// end of void CHYImgProcDlg::OnImgprdSavefileMu()

void CHYImgProcDlg::MakeBGImage(cv::String strBGImage, cv::Mat& BGimg)
{
	BGimg.release();

	cv::Mat		tmpGimg = cv::imread(strBGImage);
	if (tmpGimg.channels() == 3) {
		cv::cvtColor(tmpGimg, BGimg, COLOR_BGR2GRAY);
	}
	else if (tmpGimg.channels() == 4) {
		cv::cvtColor(tmpGimg, BGimg, COLOR_BGR2GRAY);
	}
	else {
		tmpGimg.copyTo(BGimg);
	}

}	// end of void CHYImgProcDlg::MakeBGImage()

void CHYImgProcDlg::MergeBackground(cv::String strFGimg, cv::Mat& BGimg)
{
	if (strFGimg == "") return;
	if (BGimg.empty()) return;

	cv::Mat tmpImg = cv::imread(strFGimg);
	cv::Mat FKImg;
	if (tmpImg.channels() == 3) {
		cv::cvtColor(tmpImg, FKImg, COLOR_BGR2GRAY);
	}
	else if (tmpImg.channels() == 4) {
		cv::cvtColor(tmpImg, FKImg, COLOR_BGRA2GRAY);
	}
	else {
		tmpImg.copyTo(FKImg);
	}

	MergeBackground(FKImg, BGimg);

}	// end of void CHYImgProcDlg::MergeBackground()

void CHYImgProcDlg::MergeBackground(cv::Mat& FGimg, cv::Mat& BGimg)
{
	cv::Mat		Tmp;
	int FGMaxLength = FGimg.rows > FGimg.cols ? FGimg.rows : FGimg.cols;
	if (FGMaxLength > 1000) {
		double ftScale = 1000.00 / (double)FGMaxLength;
		cv::resize(FGimg, Tmp, cv::Size(0, 0), ftScale, ftScale, cv::INTER_LINEAR);
	}
	else {
		Tmp = FGimg.clone();
	}

	int ioffsetX = (BGimg.cols - Tmp.cols) / 2;
	int ioffsetY = (BGimg.rows - Tmp.rows) / 2;
	Mat ROIx = BGimg(cv::Rect(ioffsetX, ioffsetY, Tmp.cols, Tmp.rows));
	Tmp.copyTo(ROIx);

}	// end of void CHYImgProcDlg::MergeBackground()

void CHYImgProcDlg::AdjustImage(cv::Mat& InImg, cv::Mat& OutImg)
{	
	cv::Mat			inImg = InImg.clone();

	HY_IMG_OPTIM	tagImgOptim;
	tagImgOptim.bMono = m_bMono;
	tagImgOptim.MonoThres = m_sldBWThres.GetPos();
	tagImgOptim.Contrast = m_sldContrast.GetPos();
	tagImgOptim.InVert = m_bInvert;
	tagImgOptim.Light = m_sldLight.GetPos();

	cv::Mat tmpImg;
	if (tagImgOptim.InVert)	{
		SetInverse(inImg, tmpImg);
	}
	else{
		tmpImg = inImg.clone();
	}
	
	SetContrastLight(tmpImg, OutImg, tagImgOptim.Contrast, tagImgOptim.Light);

	if (tagImgOptim.bMono) {
		tmpImg = OutImg.clone();
		SetBKThres(tmpImg, OutImg, tagImgOptim.MonoThres);
	}

}	// end of void AdjustImage()

void CHYImgProcDlg::SetContrastLight(cv::Mat& inImg, cv::Mat& outImg, int iContrast,int iLight)
{	
	outImg = Mat::zeros(inImg.size(), inImg.type());
	
	double dbContrast = (iContrast+100.00)/100.00;
	for (int y = 0; y < inImg.rows; y++){
		for (int x = 0; x < inImg.cols; x++) {
			float v = inImg.at<uchar>(y, x);
			outImg.at<uchar>(y, x) = saturate_cast<uchar>(v*dbContrast + iLight);
		}
	}

}	// end of void CHYImgProcDlg::SetContrastLight()

void CHYImgProcDlg::SetBKThres(cv::Mat& inImg, cv::Mat& outImg, int ivalue)
{
	outImg = Mat::zeros(inImg.size(), inImg.type());
	for (int y = 0; y < inImg.rows; y++) {
		for (int x = 0; x < inImg.cols; x++) {
			outImg.at<uchar>(y, x) = inImg.at<uchar>(y, x)>ivalue ? 255:0;
		}
	}

}	// end of void CHYImgProcDlg::SetBKThres()

void CHYImgProcDlg::SetInverse(cv::Mat& inImg, cv::Mat& outImg)
{
	outImg = Mat::zeros(inImg.size(), inImg.type());

	int rows = inImg.rows;
	int cols = inImg.cols;

	//单通道图像的反色
	for (int row = 0; row<rows; row++) {
		for (int col = 0; col<cols; col++) {
			outImg.at<uchar>(row, col) = 255 - inImg.at<uchar>(row, col);
		}
	}

}	// end of void CHYImgProcDlg::SetInverse()

// 反色处理
void CHYImgProcDlg::OnBnClickedImgpInvertChk()
{	
	UpdateData(); 

	cv::Mat OutImg;
	AdjustImage(m_BGimg, OutImg);
	m_prwWnd.SetMat(OutImg);

	UpdateData(FALSE);

}	// end of void CHYImgProcDlg::OnBnClickedImgpInvertChk()

void CHYImgProcDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{	
	int minpos = 0;
	int maxpos = 0;
	int curpos = 0;

	if (pScrollBar->GetDlgCtrlID() == IDC_IMGP_CONTRAST_SLD) {
		m_sldContrast.GetRange(minpos, maxpos);
		curpos = m_sldContrast.GetPos();
	}
	else if (pScrollBar->GetDlgCtrlID() == IDC_IMGP_LIGHT_SLD){
		m_sldLight.GetRange(minpos, maxpos);
		curpos = m_sldLight.GetPos();
	}
	else if (pScrollBar->GetDlgCtrlID() == IDC_IMGP_MONO_SLD){
		m_sldBWThres.GetRange(minpos, maxpos);
		curpos = m_sldBWThres.GetPos();
	}

	switch (nSBCode){
		case SB_LEFT: // Scroll to far left.
			curpos = minpos;
			break;
		case SB_RIGHT: // Scroll to far right.
			curpos = maxpos;
			break;
		case SB_ENDSCROLL: // End scroll.
			break;
		case SB_LINELEFT: // Scroll left.
			if (curpos > minpos)
				curpos--;
			break;
		case SB_LINERIGHT: // Scroll right.
			if (curpos < maxpos)
				curpos++;
			break;
		// Scroll one page left.
		case SB_PAGELEFT:{
			// Get the page size.				
			int ipage = m_sldContrast.GetPageSize();				

			if (curpos > minpos)
				curpos = max(minpos, curpos - ipage);
		}
		break;
		// Scroll one page right.
		case SB_PAGERIGHT:{
			// Get the page size.
			int ipage = m_sldContrast.GetPageSize();
			if (curpos < maxpos)
				curpos = min(maxpos, curpos + ipage);
		}
		break;
		case SB_THUMBPOSITION: // Scroll to absolute position. nPos is the position
			curpos = nPos;      // of the scroll box at the end of the drag operation.
			break;
		case SB_THUMBTRACK: // Drag scroll box to specified position. nPos is the
			curpos = nPos;   // position that the scroll box has been dragged to.
			break;
	}

	// Set the new position of the thumb (scroll box).
	if (pScrollBar->GetDlgCtrlID() == IDC_IMGP_CONTRAST_SLD) {
		m_sldContrast.SetPos(curpos);		
	}
	else if (pScrollBar->GetDlgCtrlID() == IDC_IMGP_LIGHT_SLD) {
		m_sldLight.SetPos(curpos);
	}
	else if (pScrollBar->GetDlgCtrlID() == IDC_IMGP_MONO_SLD) {
		m_sldBWThres.SetPos(curpos);		
	}

	SendMessage(WM_SCRLMSG);

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);	

}	// end of void CHYImgProcDlg::OnHScroll()

LRESULT CHYImgProcDlg::OnSCRLMsgHandler(WPARAM, LPARAM)
{	
	m_iContrast = m_sldContrast.GetPos();
	m_iLight = m_sldLight.GetPos();
	m_iBWThres = m_sldBWThres.GetPos();

	SetDlgItemInt(IDC_IMGP_CONTRAST_EDT, m_iContrast);
	SetDlgItemInt(IDC_IMGP_LIGHT_EDT, m_iLight);
	SetDlgItemInt(IDC_IMGP_MONO_EDT, m_iBWThres);		

	cv::Mat OutImg;
	AdjustImage(m_BGimg, OutImg);
	m_prwWnd.SetMat(OutImg);

	UpdateWindow();
	
	return 0L;

}	// end of LRESULT CHYImgProcDlg::MyMsgHandler()

LRESULT CHYImgProcDlg::OnStepMsgHandler(WPARAM wm, LPARAM lm)
{
	m_psctrlImgpress.SetPos((int)wm);

	if (!m_OutImg.empty()){
		m_prwWnd.SetMat(m_OutImg);
	}	

	std::string strFullName = HY_GetFullNameFromPath(m_strCurFile);
	SetDlgItemText(IDC_IMGP_CURFILE_EDT, strFullName.c_str());
	UpdateWindow();
	return 0L;

}	// end of LRESULT CHYImgProcDlg::OnStepMsgHandler()

void CHYImgProcDlg::OnBnClickedImgpBwChk()
{
	UpdateData();

	cv::Mat OutImg;
	AdjustImage(m_BGimg, OutImg);
	m_prwWnd.SetMat(OutImg);

	UpdateData(FALSE);
	
}	// end of void CHYImgProcDlg::OnBnClickedImgpBwChk()

void CHYImgProcDlg::OnRadioButton(UINT nID)
{
	UpdateData();

	if (nID == IDC_IMGP_BG_RD)	{
		m_strBGImageName = ::XSysproxy().m_strBlackBkName;
	}
	if (nID == IDC_IMGP_WG_RD) {
		m_strBGImageName = ::XSysproxy().m_strWhiteBkName;
	}

	MakeBGImage(cv::String(m_strBGImageName), m_BGimg);
	MergeBackground(cv::String(m_strFGImageName), m_BGimg);
	
	cv::Mat OutImg;
	AdjustImage(m_BGimg, OutImg);
	m_prwWnd.SetMat(OutImg);

}	// end of void CHYImgProcDlg::OnRadioButton()



