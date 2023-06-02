
// HYFontSmartShaperView.cpp : CHYFontSmartShaperView 类的实现
//

#include "stdafx.h"
#include "HYFontSmartShaper.h"
#include "HYFontSmartShaperDoc.h"
#include "HYFontSmartShaperView.h"
#include "FontSetupDlg.h"
#include "IKMergeDlg.h"
#include "IKSplitDlg.h"
#include "FongSVGDlg.h"
#include "MergeDlg.h"
#include "HYToTtcDlg.h"
#include "FontExtract.h"
#include "FontInfDlg.h"
#include "VerifyFntDlg.h"
#include "FontDumpDlg.h"
#include "HYMakeJFFontDlg.h"
#include "FontVADHDlg.h"
#include "MKVNTDlg.h"
#include "HYTTC.h"
#include "MakeTTCDlg.h"
#include "CharsCompareDlg.h"
#include "FontExtract.h"
#include "ResetPsNameDlg.h"
#include "TXTCmpDlg.h"
#include "CHYEmjioDlg.h"
#include "CExchgCodeDlg.h"
#include <atltime.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static CHYFontSmartShaperView*  g_pShaperView;
static void LoadFontThread(void* pParam);
static void LoadFontStepThread(void* pParam);

// CHYFontSmartShaperView

IMPLEMENT_DYNCREATE(CHYFontSmartShaperView, CFormView)

BEGIN_MESSAGE_MAP(CHYFontSmartShaperView, CFormView)	
	ON_BN_CLICKED(IDC_MN_FNTST_BTN, &CHYFontSmartShaperView::OnBnClickedMnFntstBtn)
	ON_WM_DESTROY()
	ON_COMMAND(ID_FILE_OPEN, &CHYFontSmartShaperView::OnFileOpen)
	ON_WM_VSCROLL()	
	ON_WM_MOUSEWHEEL()
	ON_MESSAGE(MSG_PREVIEW_WND, OnThumSelet)
	ON_MESSAGE(MSG_POPVIEW_WND, OnPopWnd)	
	ON_CONTROL_RANGE(BN_CLICKED, IDC_MN_PT_CHK, IDC_MN_FL_CHK, &CHYFontSmartShaperView::GetDrawParam)	
	ON_BN_CLICKED(IDC_MN_SEEK_BTN, &CHYFontSmartShaperView::OnBnClickedMnSeekBtn)
	ON_COMMAND(ID_FNT_ENCODE_PARAM, &CHYFontSmartShaperView::OnFntEncodeParam)
	ON_BN_CLICKED(IDC_MN_CVTS_BTN, &CHYFontSmartShaperView::OnBnClickedMnCvtsBtn)
	ON_BN_CLICKED(IDC_MN_CVTM_BTN, &CHYFontSmartShaperView::OnBnClickedMnCvtmBtn)
	ON_COMMAND(ID_FILE_SAVE, &CHYFontSmartShaperView::OnFileSave)	
	ON_COMMAND(ID_IK_MERGE, &CHYFontSmartShaperView::OnIkMerge)
	ON_COMMAND(ID_IK_SPLIT, &CHYFontSmartShaperView::OnIkSplit)
	ON_CBN_SELCHANGE(IDC_MN_VMTYP_CMB, &CHYFontSmartShaperView::OnCbnSelchangeMnVmtypCmb)
	ON_COMMAND(ID_FNT_CVTPRAM_MU, &CHYFontSmartShaperView::OnFntCvtpramMu)	
	ON_BN_CLICKED(IDC_MN_CVT_TTF_RD, &CHYFontSmartShaperView::OnBnClickedMnCvtTtfRd)
	ON_BN_CLICKED(IDC_MN_CVT_OTF_RD, &CHYFontSmartShaperView::OnBnClickedMnCvtOtfRd)
	ON_COMMAND(ID_FNT_MERGE, &CHYFontSmartShaperView::OnFntMerge)
	ON_BN_SETFOCUS(IDC_MN_CVT_TTF_RD, &CHYFontSmartShaperView::OnBnSetfocusMnCvtTtfRd)
	ON_BN_SETFOCUS(IDC_MN_CVT_OTF_RD, &CHYFontSmartShaperView::OnBnSetfocusMnCvtOtfRd)
	ON_COMMAND(ID_EMOJI_EXPORT, &CHYFontSmartShaperView::OnEmojiExport)
	ON_COMMAND(ID_SVG_CVT_MU, &CHYFontSmartShaperView::OnSvgCvtMu)
	ON_COMMAND(ID_SVG_EOT_MU, &CHYFontSmartShaperView::OnSvgEotMu)
	
	ON_COMMAND(ID_TO_WOFF_MU, &CHYFontSmartShaperView::OnToWoffMu)
	ON_MESSAGE(VM_UPDATEALLVIEWS,&CHYFontSmartShaperView::OnUpdateAllViews)
	ON_WM_DROPFILES()
	ON_WM_HSCROLL()	
	ON_COMMAND(ID_CHA_CONVERT, &CHYFontSmartShaperView::OnChnvert)
	ON_COMMAND(ID_FLNM2CDNM, &CHYFontSmartShaperView::OnFlnm2cdnm)
	ON_COMMAND(ID_CDNM2FLNM, &CHYFontSmartShaperView::OnCdnm2flnm)
	ON_BN_CLICKED(IDC_MN_CUTM_BTN, &CHYFontSmartShaperView::OnBnClickedMnCutmBtn)	
	ON_COMMAND(ID_MN_CHA, &CHYFontSmartShaperView::OnMnCha)
	ON_COMMAND(ID_MN_FNTINF, &CHYFontSmartShaperView::OnMnFntinf)	
	ON_COMMAND(ID_MN_CODEDOT, &CHYFontSmartShaperView::OnMnCodedot)
	ON_COMMAND(ID_MN_MKJF, &CHYFontSmartShaperView::OnMnMkjf)
	ON_COMMAND(ID_FNT_VADH, &CHYFontSmartShaperView::OnFntVadh)
	ON_COMMAND(ID_MN_ADUNI, &CHYFontSmartShaperView::OnMnAduni)
	ON_COMMAND(ID_MKVNT_MN, &CHYFontSmartShaperView::OnMkvntMn)
	ON_COMMAND(ID_MN_JSZ_CODETCHAR, &CHYFontSmartShaperView::OnMnJszCodetchar)	
	ON_COMMAND(ID_VERIFYVNT_MN, &CHYFontSmartShaperView::OnVerifyvntMn)
	ON_COMMAND(ID_TTC_TTCTOFONT, &CHYFontSmartShaperView::OnTtcTtctofont)
	ON_COMMAND(ID_TTC_FONTTOTTC, &CHYFontSmartShaperView::OnTtcFonttottc)	
	ON_COMMAND(ID_FNT_EXTRCHR, &CHYFontSmartShaperView::OnFntExtrchr)
	ON_COMMAND(ID_FNT_EXTRCD, &CHYFontSmartShaperView::OnFntExtrcd)
	ON_COMMAND(ID_FNT_EXTRNAME, &CHYFontSmartShaperView::OnFntExtrname)	
	ON_COMMAND(ID_NOBOX_MNU, &CHYFontSmartShaperView::OnNoboxMnu)	
	ON_COMMAND(ID_FNT_RSETNAME, &CHYFontSmartShaperView::OnFntRsetname)
	ON_COMMAND(ID_MN_EXPTNAME, &CHYFontSmartShaperView::OnMnExptname)
	ON_COMMAND(ID_FLNM2NUMNM, &CHYFontSmartShaperView::OnFlnm2numnm)	
	ON_EN_CHANGE(IDC_MN_ADW_EDT, &CHYFontSmartShaperView::OnEnChangeMnAdwEdt)
	ON_EN_CHANGE(IDC_MN_ADH_EDT, &CHYFontSmartShaperView::OnEnChangeMnAdhEdt)
	ON_COMMAND(ID_MN_CMPTXT, &CHYFontSmartShaperView::OnMnCmptxt)
	ON_COMMAND(ID_MN_CHKERTTF, &CHYFontSmartShaperView::OnMnChkerttf)
	ON_COMMAND(ID_MN_CHKEROTF, &CHYFontSmartShaperView::OnMnChkerotf)
	ON_COMMAND(ID_EMOJI_MK, &CHYFontSmartShaperView::OnEmojiMk)
	ON_COMMAND(ID_MN_CODEMAP, &CHYFontSmartShaperView::OnMnCodemap)
	ON_COMMAND(ID_FNT_CLEARCODE, &CHYFontSmartShaperView::OnFntResetcode)
	ON_COMMAND(ID_FNT_BUDCIDOTF, &CHYFontSmartShaperView::OnFntBudcidotf)	
END_MESSAGE_MAP()

// CHYFontSmartShaperView 构造/析构

CHYFontSmartShaperView::CHYFontSmartShaperView()
	: CFormView(CHYFontSmartShaperView::IDD)
	, m_strSeek(_T(""))	
	, m_adsAdw(0)
	, m_adsAdh(0)
{
	// TODO: 在此处添加构造代码
	m_iThumClumnNums			= 0;
	m_iThumRowNums				= 0;
	m_iSelectGID				=-1;	
	m_hLoadFontEvent			= 0;
	m_hdLoadFontThread			= 0;
	m_hdLoadFontStepThread		= 0;

	m_pFntSrcWnd				= NULL;
	m_pFntDstWnd				= NULL;
	m_pCompareFontWnd			= NULL;
	m_pFntSrcZoomWnd			= NULL;
	m_pFntDstZoomWnd			= NULL;
	m_pCmprZoomWnd				= NULL;

}	// end of CHYFontSmartShaperView::CHYFontSmartShaperView()

CHYFontSmartShaperView::~CHYFontSmartShaperView()
{
	if (m_hLoadFontEvent)
	{	
		CloseHandle(m_hLoadFontEvent);
	}
	m_hLoadFontEvent = 0;

}	// end of CHYFontSmartShaperView::~CHYFontSmartShaperView()

void CHYFontSmartShaperView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MN_V_SCRL, m_srblThum);
	DDX_Control(pDX, IDC_MN_CNVTR_PRSS, m_prssProgress);
	DDX_Control(pDX, IDC_MN_SEEK_CMB, m_cmbSeek);
	DDX_Text(pDX, IDC_MN_SEEK_EDT, m_strSeek);
	DDX_Text(pDX, IDC_MN_ADW_EDT, m_adsAdw);
	DDV_MinMaxUInt(pDX, m_adsAdw, 0, 65535);
	DDX_Text(pDX, IDC_MN_ADH_EDT, m_adsAdh);
	DDV_MinMaxUInt(pDX, m_adsAdh, 0, 65535);

}	// end of void CHYFontSmartShaperView::DoDataExchange()

BOOL CHYFontSmartShaperView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CFormView::PreCreateWindow(cs);

}	// end of BOOL CHYFontSmartShaperView::PreCreateWindow()

void CHYFontSmartShaperView::OnInitialUpdate()
{	
	::XSysproxy().m_pMainFrame = this;

	RECT				rcMainFrame;
	::SystemParametersInfo(SPI_GETWORKAREA,0,&rcMainFrame,0);

	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	GetParentFrame()->MoveWindow(0,0,rcMainFrame.right-rcMainFrame.left,rcMainFrame.bottom-rcMainFrame.top);

	SetLayout();
 	InitCtrl();
 	g_pShaperView = this;		
 	m_hLoadFontEvent = CreateEvent(NULL,TRUE,FALSE,NULL);  	

}	// end of void CHYFontSmartShaperView::OnInitialUpdate()

void CHYFontSmartShaperView::OnDestroy()
{
	CFormView::OnDestroy();

	if (m_hLoadFontEvent)
	{
		int i=0;
		while (TRUE)
		{
			if (WaitForSingleObject(m_hLoadFontEvent,10) == WAIT_TIMEOUT)
			{
				break;
			}
			else 
			{
				if (i==150)
					break;
			}

			i++;
		}
	}	
 	
	UnInitCtrl();

}	// end of void CHYFontSmartShaperView::OnDestroy()

// CHYFontSmartShaperView 消息处理程序

static void LoadFontThread(void* pParam)
 {	 
	 g_pShaperView->LoadFontThreadFun();	 

 }	// end of UINT LoadFontThread()
	
 static void LoadFontStepThread(void* pParam)
 {
	g_pShaperView->LoadFontStepThreadFun();	 

 }	// end of static unsigned LoadFontStepThread()

void CHYFontSmartShaperView::OnFileOpen()
{	
	TCHAR szFilters[]= _T("字库 文件(*.otf;*.ttf)|*.otf;*ttf|OpenType 文件(*.otf)|*.otf|Truetype 文件(*.ttf)|*.ttf||");

	CFileDialog  OpenFileDlg(TRUE, _T(""), _T(""), OFN_READONLY|OFN_LONGNAMES|OFN_FILEMUSTEXIST,szFilters);		
	if (OpenFileDlg.DoModal() != IDOK) return;	
	m_strFontFileName = OpenFileDlg.GetPathName();

	AfxGetApp()->m_pMainWnd->SetWindowText(_T("汉仪字库 ") + m_strFontFileName);
	SetDefault();

	if (m_hLoadFontEvent)
		 SetEvent(m_hLoadFontEvent);

	m_hdLoadFontThread = (HANDLE) _beginthread(LoadFontThread,0,NULL);
	m_hdLoadFontStepThread = (HANDLE)_beginthread(LoadFontStepThread,0,NULL);	

}	// end of void CHYFontSmartShaperView::OnFileOpen()

void CHYFontSmartShaperView::OnFileSave()
{
	CWaitCursor wc;
	int iCheckNo = GetCheckedRadioButton(IDC_MN_CVT_TTF_RD, IDC_MN_CVT_OTF_RD);
	CString strFileName = "";

	if (m_FontEnCodec.m_vtHYGlyphs.size() == 0) return;

	::XSysproxy().SetEncodeOption(m_FontEnCodec, m_FontDeCodec);
	m_FontEnCodec.m_mulpTableData.setDefault();
	m_FontEnCodec.m_mulpTableData = m_FontDeCodec.m_mulpTableData;

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

	if (::XSysproxy().m_tagOpeionPrm.bCmplVert) {
		ulTableFlag.push_back(VHEA_TAG);
		ulTableFlag.push_back(VMTX_TAG);
	}
	if (m_FontDeCodec.FindFlag(GPOS_TAG))
	{
		ulTableFlag.push_back(GPOS_TAG);
	}

	if (::XSysproxy().m_tagOpeionPrm.bCmplLayout)
	{
		ulTableFlag.push_back(GSUB_TAG);
	}
	else 
	{
		if (m_FontDeCodec.FindFlag(GSUB_TAG))
		{
			ulTableFlag.push_back(GSUB_TAG);
		}
	}
	
	if (iCheckNo == IDC_MN_CVT_TTF_RD) {		
		TCHAR szFilters[] = _T("Truetype 文件(*.ttf)|*.ttf||");
		CFileDialog  saveFileDlg(FALSE, _T(""), _T(""), OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilters);

		if (saveFileDlg.DoModal() != IDOK)	return;
		strFileName = saveFileDlg.GetPathName();
		ulTableFlag.push_back(GLYF_TAG);
		ulTableFlag.push_back(LOCA_TAG);
		ulTableFlag.push_back(PREP_TAG);		
	}
	else if (iCheckNo == IDC_MN_CVT_OTF_RD) {
		TCHAR szFilters[] = _T("OpenType 文件(*.otf)|*.otf||");
		CFileDialog  saveFileDlg(FALSE, _T(""), _T(""), OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilters);
		if (saveFileDlg.DoModal() != IDOK)		return;

		strFileName = saveFileDlg.GetPathName();
		ulTableFlag.push_back(CFF_TAG);	
		ulTableFlag.push_back(VORG_TAG);
	}

	if (m_FontEnCodec.Encode((LPTSTR)(LPCTSTR)strFileName, ulTableFlag, ::XSysproxy().m_tagOpeionPrm) == HY_NOERROR)
		AfxMessageBox(_T("字库生成完成"));
	else
		AfxMessageBox(_T("字库生成失败"));	

}	// end of void CHYFontSmartShaperView::OnFileSave()

void CHYFontSmartShaperView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (pScrollBar == NULL ) return;

	int minpos;
	int maxpos;
	pScrollBar->GetScrollRange(&minpos, &maxpos); 
	maxpos = pScrollBar->GetScrollLimit();
	// Get the current position of scroll box.
	int curpos = pScrollBar->GetScrollPos();
	// Determine the new position of scroll box.
	switch (nSBCode){
	case SB_TOP:      // Scroll to far left.
		curpos = minpos;
		break;
	case SB_BOTTOM:      // Scroll to far right.
		curpos = maxpos;
		break;
	case SB_ENDSCROLL:   // End scroll.
		break;
	case SB_LINEUP:      // Scroll left.
		if (curpos > minpos)
			curpos--;
		break;
	case SB_LINEDOWN:   // Scroll right.
		if (curpos < maxpos)
			curpos++;
		break;
	case SB_PAGEUP: {    // Scroll one page left.		
			// Get the page size. 
			SCROLLINFO   info;
			pScrollBar->GetScrollInfo(&info, SIF_ALL);
			if (curpos > minpos)
				curpos = max(minpos, curpos - (int) info.nPage);
		}
		break;
	case SB_PAGEDOWN: {      // Scroll one page right.		
			// Get the page size. 
			SCROLLINFO   info;
			pScrollBar->GetScrollInfo(&info, SIF_ALL);
			if (curpos < maxpos)
				curpos = min(maxpos, curpos + (int) info.nPage);
		}
		break; 
	case SB_THUMBPOSITION: // Scroll to absolute position. nPos is the position
		curpos = nPos;      // of the scroll box at the end of the drag operation.
		break;
	case SB_THUMBTRACK:   // Drag scroll box to specified position. nPos is the
		curpos = nPos;     // position that the scroll box has been dragged to.
		break;
	}

	// Set the new position of the thumb (scroll box).
	pScrollBar->SetScrollPos(curpos);
	int iCurPos = curpos*(m_iThumClumnNums*m_iThumRowNums);
	UpdateHYThumWnds(iCurPos);		
	SetSelectHYThumWnds(m_iSelectGID - iCurPos);

	CFormView::OnVScroll(nSBCode, nPos, pScrollBar);

}	// end of void CHYFontSmartShaperView::OnVScroll()

BOOL CHYFontSmartShaperView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{	
	int iMin=0,iMax=0;
	m_srblThum.GetScrollRange(&iMin,&iMax);		
	if (iMax>0){
		int curpos = m_srblThum.GetScrollPos();
		if (zDelta==120)
			curpos--;
		if (zDelta==-120)
			curpos++;	

		if (curpos<iMin) curpos=iMin;
		if (curpos>iMax) curpos=iMax;	
		
		m_srblThum.SetScrollPos(curpos);

		int iCurPos = curpos*(m_iThumClumnNums*m_iThumRowNums);
		UpdateHYThumWnds(iCurPos);		
		SetSelectHYThumWnds(m_iSelectGID - iCurPos);
	}

	return CFormView::OnMouseWheel(nFlags, zDelta, pt);

}	// end of BOOL CHYFontSmartShaperView::OnMouseWheel()

LRESULT CHYFontSmartShaperView::OnThumSelet(WPARAM waram, LPARAM lparam)
{
	int iMax=m_srblThum.GetScrollLimit();	
	if (iMax<0) return -1;
	
	SetSelectHYThumWnds((int)waram);
	m_iSelectGID = (int)waram+m_srblThum.GetScrollPos()*m_iThumClumnNums*m_iThumRowNums;	
	ShowSrcWnd(m_iSelectGID);
	ShowDstWnd(m_iSelectGID);

	return HY_NOERROR;

}	// end of LRESULT CHYFontSmartShaperView::OnPreView()

LRESULT CHYFontSmartShaperView::OnPopWnd(WPARAM waram, LPARAM lparam)
{
	if (m_iSelectGID==-1) return 0;
	switch((int)waram){
		case 50000:{
				m_pFntSrcZoomWnd->SetActiveWindow();
				m_pFntSrcZoomWnd->ShowWindow(SW_SHOWNORMAL);
				int iGlyphNum = m_FontDeCodec.m_vtHYGlyphs.size();
				if (m_iSelectGID<iGlyphNum)	{		 
					m_pFntSrcZoomWnd->SetGlyphInfo(m_FontDeCodec,m_iSelectGID,::XSysproxy().m_tagDrawParam);
				}				
			}			
			break;
		case 50001:{
				m_pFntDstZoomWnd->SetActiveWindow();
				m_pFntDstZoomWnd->ShowWindow(SW_SHOWNORMAL);
				int iGlyphNum = m_FontEnCodec.m_vtHYGlyphs.size();
				if (m_iSelectGID<iGlyphNum)	{		 
					m_pFntDstZoomWnd->SetGlyphInfo(m_FontEnCodec,m_iSelectGID,::XSysproxy().m_tagDrawParam);
				}													
			}
			break;
		case 50002:{
				m_pCmprZoomWnd->SetActiveWindow();
				m_pCmprZoomWnd->ShowWindow(SW_SHOWNORMAL);
				int iGlyphNum = m_FontEnCodec.m_vtHYGlyphs.size();
				if (m_iSelectGID<iGlyphNum)	{		 					
					m_pCmprZoomWnd->SetGlyphInfo(m_FontDeCodec,m_FontEnCodec,m_iSelectGID,::XSysproxy().m_tagDrawParam);
				}				
			}
			break;
		default:
			break;	
	}
	
	return HY_NOERROR;

}	// end of LRESULT CHYFontSmartShaperView::OnPopWnd()

void CHYFontSmartShaperView::OnBnClickedMnSeekBtn()
{	
	UpdateData(TRUE);
	
	int iCurSel = m_cmbSeek.GetCurSel();
	int iGlyphIndex = -1;
	CString strMsg;	

	switch(iCurSel){
		case 0:	// gid
			if (m_strSeek.IsEmpty()){
				AfxMessageBox(_T("请输入需要查找的GID"));
				break;
			}

			iGlyphIndex = strtol((LPSTR)(LPCTSTR)m_strSeek,NULL,0);
			if (iGlyphIndex<0 || (size_t)iGlyphIndex>=m_FontDeCodec.m_vtHYGlyphs.size()){
				strMsg.Format(_T("GID编号介于0 - %d之间,请重新输入"),(int)m_FontDeCodec.m_vtHYGlyphs.size());
				AfxMessageBox(strMsg);
				iGlyphIndex = -1;
			}
			break;
		case 1: { // uni 
				if (m_strSeek.IsEmpty()){
					AfxMessageBox(_T("请输入需要查找的Unicode"));
					break;
				}

				unsigned long uni =strtol((LPSTR)(LPCTSTR)m_strSeek,NULL,0);
				iGlyphIndex = m_FontDeCodec.FindGryphIndexByUnciodeEx(uni);
				if (iGlyphIndex == -1){
					strMsg.Format(_T("没有找到unicode编码为0x%x的字形,请重新输入"),uni);
					AfxMessageBox(strMsg);					
				}
			}
			break;
		case 2: {// ansi
			
				if (m_strSeek.IsEmpty()){
					AfxMessageBox(_T("请输入需要查找的字符"));
					break;
				}
				WCHAR	wChar[4];
				int		wCharlen=4;
				::XSysproxy().MToWChar(wChar,wCharlen,(LPSTR)(LPCTSTR)m_strSeek);
				if (wCharlen>2)	{
					AfxMessageBox(_T("请确保只输入一个字符"));
					break;
				}

				DWORD 	Unicode=wChar[0];	
				iGlyphIndex = m_FontDeCodec.FindGryphIndexByUnciodeEx(Unicode);
				if (iGlyphIndex == -1){
					strMsg.Format(_T("没有找到字形(%s),请重新输入"),m_strSeek);
					AfxMessageBox(strMsg);					
				}			
			}
			break;
		default:
			break;
	}

	if (iGlyphIndex!= -1){
		int iThumWndNums =  m_iThumClumnNums*m_iThumRowNums;
		int iThumTmp = (iGlyphIndex/iThumWndNums)*iThumWndNums;	

		UpdateHYThumWnds(iThumTmp);
		SetSelectHYThumWnds(iGlyphIndex-iThumTmp);
		ShowSrcWnd(iGlyphIndex);
		m_iSelectGID = iGlyphIndex;

		m_srblThum.SetScrollPos(iThumTmp/iThumWndNums);
	}	

}	// end of void CHYFontSmartShaperView::OnBnClickedMnSeekBtn()

void CHYFontSmartShaperView::OnFntEncodeParam()
{
	CFontSetupDlg		FntStupDlg(&m_FontEnCodec);
	FntStupDlg.DoModal();

}	// end of void CHYFontSmartShaperView::OnFntEncodeParam()

void CHYFontSmartShaperView::OnBnClickedMnFntstBtn()
{
	CFontSetupDlg		FntStupDlg(&m_FontEnCodec);
	FntStupDlg.DoModal();

}	// end of void CHYFontSmartShaperView::OnBnClickedMnFntstBtn()

void CHYFontSmartShaperView::OnBnClickedMnCvtsBtn()
{	
	if (m_iSelectGID<0 || m_iSelectGID>=(int)m_FontEnCodec.m_vtHYGlyphs.size()) return;

	CHYGlyph&		SrcGryph	= m_FontDeCodec.m_vtHYGlyphs[m_iSelectGID];
	CHYGlyph&		DstGryph	= m_FontEnCodec.m_vtHYGlyphs[m_iSelectGID];

	DstGryph.SetDefault();		
	DstGryph.advanceHeight = SrcGryph.advanceHeight;
	DstGryph.advanceWidth = SrcGryph.advanceWidth;
	DstGryph.vtUnicode = SrcGryph.vtUnicode;
	DstGryph.strPostName = SrcGryph.strPostName;
	if (m_FontDeCodec.m_HYTbDirectory.version.value==1&&m_FontDeCodec.m_HYTbDirectory.version.fract==0)
	{			
		if (TTFConvert(SrcGryph,DstGryph,m_FontDeCodec.m_HYhead))
		{
			ShowDstWnd(m_iSelectGID);
		}		
	}
	else if (m_FontDeCodec.m_HYTbDirectory.version.value==0x4f54&&m_FontDeCodec.m_HYTbDirectory.version.fract==0x544f)
	{
		if (OTFConvert(SrcGryph,DstGryph,m_FontDeCodec.m_HYhead))
		{
			ShowDstWnd(m_iSelectGID);
		}
	}	

}	// end of void CHYFontSmartShaperView::OnBnClickedMnCvtsBtn()

void CHYFontSmartShaperView::OnBnClickedMnCvtmBtn()
{
	UpdateData();

	CWaitCursor wc;
	unsigned long uluni1 = 0x2B820;
	unsigned long uluni2 = 0x2BD2D;
	unsigned long uluni3 = 0x2C1CD;

	size_t stGlyhNum = m_FontDeCodec.m_vtHYGlyphs.size();
	int iNum = 0;
	string strlog;
	for(size_t i=0; i<stGlyhNum; i++){		
		CHYGlyph		SrcGryph	= m_FontDeCodec.m_vtHYGlyphs[i];
		CHYGlyph&		DstGryph	= m_FontEnCodec.m_vtHYGlyphs[i];
		DstGryph = SrcGryph;
		if (::XSysproxy().m_tagOpeionPrm.bsetADW) {
			if (m_adsAdw != 0) SetAdWidth(m_adsAdw, DstGryph);
		}
/*
		if (::XSysproxy().m_tagOpeionPrm.bsetADH) {
			if (m_adsAdh != 0) SetAdHeight(m_adsAdh, DstGryph);
		}
*/

		m_prssProgress.SetPos(((i+1)*100)/stGlyhNum);
	}

	for(size_t i=0; i<stGlyhNum; i++){
		m_FontEnCodec.CountGlyphBox(i,m_FontEnCodec.m_vtHYGlyphs);
	}
#ifdef BIGFILE
	m_FontDeCodec.m_vtHYGlyphs.clear();
#endif // BIGFILE

	ShowDstWnd(m_iSelectGID);

}	// end of void CHYFontSmartShaperView::OnBnClickedMnCvtmBtn()

void CHYFontSmartShaperView::SetAdWidth(UINT adw, CHYGlyph& Glyphs)
{
	char  buffer[200] = { 0 };
	BOOL B = FALSE;
	if (Glyphs.vtUnicode.size() > 0) {
		unsigned long  unicode = Glyphs.vtUnicode[0];
		if (unicode >= 0x2E80 && unicode <= 0x2EFF) B = TRUE;
		if (unicode >= 0x2F00 && unicode <= 0x2FDF) B = TRUE;
		if (unicode >= 0x3000 && unicode <= 0x303F) B = TRUE;
		if (unicode >= 0x31C0 && unicode <= 0x31EF) B = TRUE;
		if (unicode >= 0x3300 && unicode <= 0x33FF) B = TRUE;
		if (unicode >= 0x3400 && unicode <= 0x4DBF) B = TRUE;
		if (unicode >= 0x4E00 && unicode <= 0x9FFF) B = TRUE;
		if (unicode >= 0xF900 && unicode <= 0xFAFF) B = TRUE;
		if (unicode >= 0x20000 && unicode <= 0x2FA1F) B = TRUE;
	}

	if (B) {		
		int xoffset = ::HY_RealRount((Glyphs.advanceWidth - (int)adw) / 2.0);
		for (int i = 0; i < Glyphs.vtContour.size(); i++) {
			CHYContour& cnt = Glyphs.vtContour[i];
			size_t stloop = cnt.vtHYPoints.size();
			for (size_t j = 0; j < stloop; j++) {
				cnt.vtHYPoints[j].x -= xoffset;
			}
		}
		Glyphs.advanceWidth = adw;
	}

}	// end of void CHYFontSmartShaperView::SetAdWidth()

void CHYFontSmartShaperView::SetAdHeight(UINT adh, CHYGlyph& Glyphs)
{
	// 增大或缩小中文竖排字间距
	char  buffer[200] = { 0 };
	BOOL B = FALSE;
	if (Glyphs.vtUnicode.size() > 0) {
		unsigned long  unicode = Glyphs.vtUnicode[0];
		if (unicode >= 0x2E80 && unicode <= 0x2EFF) B = TRUE;
		if (unicode >= 0x2F00 && unicode <= 0x2FDF) B = TRUE;
		if (unicode >= 0x3000 && unicode <= 0x303F) B = TRUE;
		if (unicode >= 0x31C0 && unicode <= 0x31EF) B = TRUE;
		if (unicode >= 0x3300 && unicode <= 0x33FF) B = TRUE;
		if (unicode >= 0x3400 && unicode <= 0x4DBF) B = TRUE;
		if (unicode >= 0x4E00 && unicode <= 0x9FFF) B = TRUE;
		if (unicode >= 0xF900 && unicode <= 0xFAFF) B = TRUE;
		if (unicode >= 0x20000 && unicode <= 0x2FA1F) B = TRUE;
	}
	if (B) {
		Glyphs.topSB = Glyphs.topSB - (Glyphs.advanceHeight-adh)/2.0 + 0.5;
		Glyphs.advanceHeight = adh; 		
	}

}	// end of void CHYFontSmartShaperView::SetAdHeight()              

void CHYFontSmartShaperView::OnBnClickedMnCutmBtn()
{
	CWaitCursor wc;
	TCHAR		szFilters[] = "Truetype File(*.ttf)|*.ttf|Opentype File(*.otf)|*.otf||";
	CFileDialog  OpenFileDlg(TRUE, NULL, NULL, OFN_EXPLORER, szFilters);
	if (OpenFileDlg.DoModal() != IDOK)	return;

	CString szPathName = OpenFileDlg.GetPathName();
	CHYFontCodec decode;
	CString strError;

	int iRtn = decode.Decode((LPSTR)(LPCSTR)szPathName);
	if (iRtn != HY_NOERROR) {
		strError.Format("解码错误 %s", HY_GetFileNameFromPath(string(szPathName)));
		AfxMessageBox(strError);
		return;
	}

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
	//ulTableFlag.push_back(VHEA_TAG);
	//ulTableFlag.push_back(VMTX_TAG);
	ulTableFlag.push_back(GASP_TAG);
	ulTableFlag.push_back(GLYF_TAG);
	ulTableFlag.push_back(LOCA_TAG);
	ulTableFlag.push_back(PREP_TAG);

	for (int i = 0; i < decode.m_vtHYGlyphs.size(); i++)
	{
		CHYGlyph& glyph = decode.m_vtHYGlyphs[i];
		for (int j = 0; j < glyph.vtContour.size(); j++)
		{
			CHYContour& cntur = glyph.vtContour[j];
			for (int x = 0; x < cntur.vtHYPoints.size(); x++)
			{
				CHYPoint& pt = cntur.vtHYPoints[x];
				pt.y += 30;
			}
		}
	}

	::XSysproxy().InitEncodeOption(decode);

	CString strSaveFileName;
	strSaveFileName.Format("%s%s-01.ttf", HY_GetDirFromPath(string(szPathName)), HY_GetFileNameFromPath(string(szPathName)));

	if (decode.Encode((LPTSTR)(LPCTSTR)strSaveFileName, ulTableFlag, ::XSysproxy().m_tagOpeionPrm) == HY_NOERROR)
		AfxMessageBox(_T("字库生成完成"));
	else
		AfxMessageBox(_T("字库生成失败"));	

}	// end of void CHYFontSmartShaperView::OnBnClickedMnCutmBtn()

void CHYFontSmartShaperView::OnIkMerge()
{	
	CIKMergeDlg		dlg;
	dlg.DoModal();

}	// end of void CHYFontSmartShaperView::OnIkMerge()

void CHYFontSmartShaperView::OnIkSplit()
{
	CIKSplitDlg		dlg;
	dlg.DoModal();

}	// end of void CHYFontSmartShaperView::OnIkSplit()

void CHYFontSmartShaperView::OnCbnSelchangeMnVmtypCmb()
{	
//	::XSysproxy().m_tagDrawParam.iViewType = m_cmbViewType.GetCurSel();

	ShowSrcWnd(m_iSelectGID);
	ShowDstWnd(m_iSelectGID);

}	// end of void CHYFontSmartShaperView::OnCbnSelchangeMnVmtypCmb()

void CHYFontSmartShaperView::OnFntCvtpramMu()
{

}	// end of void CHYFontSmartShaperView::OnFntCvtpramMu()

BOOL CHYFontSmartShaperView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	return CFormView::PreTranslateMessage(pMsg);

}	// end of BOOL CHYFontSmartShaperView::PreTranslateMessage()


/////////////////////////////////////////////////////////////////////////////////////
//
void CHYFontSmartShaperView::InitCtrl()
{
	InitWindows();
	InitHYShowListWnd();

	m_prssProgress.SetRange32(0,100);
	m_prssProgress.SetPos(0);
	m_prssProgress.SetStep(1);

	m_cmbSeek.SetCurSel(0);

	SetDrawParam();	
	CheckRadioButton(IDC_MN_CVT_TTF_RD,IDC_MN_CVT_OTF_RD, IDC_MN_CVT_TTF_RD);
	m_FontEnCodec.m_iFontType = FONTTYPE_TTF;

}	// end of void CHYFontSmartShaperView::InitCtrl()

void CHYFontSmartShaperView::UnInitCtrl()
{
	UnitWindows();
	UninitHYShowListWnd();

}	// end of void CHYFontSmartShaperView::UnInitCtrl()

void CHYFontSmartShaperView::InitWindows()
{
	CRect	rc;
	CWnd* pSrc = GetDlgItem(IDC_MN_SRC_VIEW_STC);
	if (pSrc){
		if (pSrc->m_hWnd){
			pSrc->GetWindowRect(rc);
			ScreenToClient(rc);

			m_pFntSrcWnd = new CFontShowWnd();
			if (m_pFntSrcWnd->CreateEx(0,NULL,NULL,WS_CHILD|WS_TABSTOP|WS_VISIBLE, rc, this, 50000)){			
				m_pFntSrcWnd->ShowWindow(SW_SHOW);
			}			
		}
	}

	CWnd* pDst = GetDlgItem(IDC_MN_DST_VIEW_STC);
	if (pDst){
		if (pDst->m_hWnd){
			pDst->GetWindowRect(rc);
			ScreenToClient(rc);

			m_pFntDstWnd = new CFontShowWnd();
			if (m_pFntDstWnd->CreateEx(0,NULL,NULL,WS_CHILD|WS_TABSTOP|WS_VISIBLE, rc, this, 50001)){			
				m_pFntDstWnd->ShowWindow(SW_SHOW);
			}
		}
	}

	CWnd*  pCmb= GetDlgItem(IDC_MN_CMB_RVIEW_STC);
	if (pCmb){
		if (pCmb->m_hWnd){
			pCmb->GetWindowRect(rc);
			ScreenToClient(rc);

			m_pCompareFontWnd = new CCompareFontWnd();
			if (m_pCompareFontWnd->CreateEx(0,NULL,NULL,WS_CHILD|WS_TABSTOP|WS_VISIBLE, rc, this, 50002)){			
				m_pCompareFontWnd->ShowWindow(SW_SHOW);
			}
		}
	}

	TCHAR tWndClassName[10] = {0};
#ifdef DEBUG
	strcpy_s(tWndClassName, 10, _T("AfxWnd90d"));
#else 
	strcpy_s(tWndClassName, 10, _T("AfxWnd90"));
#endif
	RECT				rcMainFrame;
	::SystemParametersInfo(SPI_GETWORKAREA,0,&rcMainFrame,0);
	m_pFntSrcZoomWnd = new CFontZoomWnd();
	if (m_pFntSrcZoomWnd->CreateEx(WS_EX_APPWINDOW,tWndClassName,_T("原始字形窗口"), WS_MINIMIZEBOX|WS_SYSMENU|WS_CAPTION|WS_POPUP|WS_OVERLAPPED,rcMainFrame,GetDesktopWindow(),NULL)){
		m_pFntSrcZoomWnd->ShowWindow(SW_HIDE);
	}

	m_pFntDstZoomWnd = new CFontZoomWnd();
	if (m_pFntDstZoomWnd->CreateEx(WS_EX_APPWINDOW,tWndClassName,_T("变化后字形窗口"),WS_MINIMIZEBOX|WS_SYSMENU|WS_CAPTION|WS_POPUP|WS_OVERLAPPED,rcMainFrame,GetDesktopWindow(),NULL)){			
		m_pFntDstZoomWnd->ShowWindow(SW_HIDE);
	}

	m_pCmprZoomWnd = new CFontCmprZoomWnd();
	if (m_pCmprZoomWnd->CreateEx(WS_EX_APPWINDOW,tWndClassName,_T("字形比对窗口"),WS_MINIMIZEBOX|WS_SYSMENU|WS_CAPTION|WS_POPUP|WS_OVERLAPPED,rcMainFrame,GetDesktopWindow(),NULL)){			
		m_pCmprZoomWnd->ShowWindow(SW_HIDE);
	}

}	// end of void CHYFontSmartShaperView::InitWindows()

void CHYFontSmartShaperView::UnitWindows()
{
	if (m_pCmprZoomWnd)	{
		if (m_pCmprZoomWnd->m_hWnd)	{
			m_pCmprZoomWnd->DestroyWindow();
		}
		delete m_pCmprZoomWnd;
		m_pCmprZoomWnd = NULL;
	}

	if (m_pFntSrcZoomWnd){
		if (m_pFntSrcZoomWnd->m_hWnd){
			m_pFntSrcZoomWnd->DestroyWindow();
		}
		delete m_pFntSrcZoomWnd;
		m_pFntSrcZoomWnd = NULL;
	}

	if (m_pFntDstZoomWnd){
		if (m_pFntDstZoomWnd->m_hWnd){
			m_pFntDstZoomWnd->DestroyWindow();
		}
		delete m_pFntDstZoomWnd;
		m_pFntDstZoomWnd = NULL;
	}

	if(m_pCompareFontWnd){
		if (m_pCompareFontWnd->m_hWnd){
			m_pCompareFontWnd->DestroyWindow();
		}
		delete m_pCompareFontWnd;
		m_pCompareFontWnd = NULL;
	}

	if(m_pFntDstWnd){
		if (m_pFntDstWnd->m_hWnd){
			m_pFntDstWnd->DestroyWindow();
		}
		delete m_pFntDstWnd;
		m_pFntDstWnd = NULL;
	}	
	
	if(m_pFntSrcWnd){
		if (m_pFntSrcWnd->m_hWnd){
			m_pFntSrcWnd->DestroyWindow();
		}
		delete m_pFntSrcWnd;
		m_pFntSrcWnd = NULL;
	}

}	// end of void CHYFontSmartShaperView::UnitWindows()

void CHYFontSmartShaperView::InitScrollBar()
{	
	SCROLLINFO		tagScrollInfo;
	ZeroMemory(&tagScrollInfo, sizeof(SCROLLINFO));

	tagScrollInfo.fMask		=	SIF_ALL;
	tagScrollInfo.nMin		=	0;	
	tagScrollInfo.nMax		=	m_FontDeCodec.m_HYMaxp.numGlyphs/(m_iThumClumnNums*m_iThumRowNums);
	tagScrollInfo.nPage		=	1;
	tagScrollInfo.nPos		=	0;
	tagScrollInfo.nTrackPos	=	0;

	m_srblThum.SetScrollInfo(&tagScrollInfo);
	m_srblThum.SetScrollPos(0);

}	// end of void CHYFontSmartShaperView::InitScrollBar()

void CHYFontSmartShaperView::SetDefault()
{	
	m_iSelectGID = 0;
	m_FontDeCodec.SetDefault();
	m_FontEnCodec.SetDefault();
	int iCheckNo = GetCheckedRadioButton(IDC_MN_CVT_TTF_RD, IDC_MN_CVT_OTF_RD);	
	if (iCheckNo=IDC_MN_CVT_TTF_RD)
		m_FontEnCodec.m_iFontType = FONTTYPE_TTF;
	if (iCheckNo=IDC_MN_CVT_OTF_RD)
		m_FontEnCodec.m_iFontType = FONTTYPE_OTF;

}	// end of void CHYFontSmartShaperView::SetDefault()

void CHYFontSmartShaperView::InitHYShowListWnd()
{
	UninitHYShowListWnd();

	// count column	
	CRect	rcArea;
	rcArea.SetRectEmpty();
	CWnd* pThumAreaWnd = (CWnd*)GetDlgItem(IDC_MN_THUMVIEW_STC);
	if (pThumAreaWnd)	{
		if(pThumAreaWnd->m_hWnd){
			pThumAreaWnd->GetWindowRect(rcArea);
			ScreenToClient(rcArea);
			rcArea.right -= 20;

			m_iThumClumnNums = (int)((float)rcArea.Width()/(WNDSZ_THUM_EM));
			m_iThumRowNums = (int)((float)rcArea.Height()/(WNDSZ_THUM_EM));
			if (rcArea.Width()-m_iThumClumnNums*WNDSZ_THUM_EM > WNDSZ_THUM_EM>>1){
				m_iThumClumnNums++;
			}
			if (rcArea.Height()-m_iThumRowNums*WNDSZ_THUM_EM > WNDSZ_THUM_EM>>1){
				m_iThumRowNums++;
			}

			CRect	rcSingleThum;
			rcSingleThum.SetRectEmpty();
			//rcArea.SetRectEmpty();			
			DWORD dwWNDID = 6000;
			for (int i = 0; i < m_iThumRowNums; i++ ){
				rcSingleThum.top = rcArea.top+i*WNDSZ_THUM_EM;
				for (int j=0; j<m_iThumClumnNums; j++)	{
					rcSingleThum.left =  rcArea.left + j * WNDSZ_THUM_EM;
					rcSingleThum.bottom	= rcSingleThum.top + WNDSZ_THUM_EM;
					rcSingleThum.right	= rcSingleThum.left + WNDSZ_THUM_EM;

					CRect rcTmp = rcSingleThum;
					rcTmp.DeflateRect(2,2);

					CThumFontWnd* pHYThumWnd = new CThumFontWnd();
					if (pHYThumWnd->CreateEx(0,NULL,NULL,WS_CHILD|WS_TABSTOP|WS_VISIBLE, 
						rcTmp,this,dwWNDID)){
						pHYThumWnd->m_iThumsIndex = dwWNDID-6000;
						pHYThumWnd->ShowWindow(SW_SHOW);
						m_vtThumFnontWnd.push_back(pHYThumWnd);			
						dwWNDID++;
					}
				}
			}
		}
	}

}	// end of void	CHYFontSmartShaperView::InitHYShowListWnd()

void	CHYFontSmartShaperView::UninitHYShowListWnd()
{
	for (size_t i=0; i<m_vtThumFnontWnd.size(); i++){
		CThumFontWnd* pHYThumWnd = m_vtThumFnontWnd[i];
		if (pHYThumWnd)	{
			if (pHYThumWnd->m_hWnd) pHYThumWnd->DestroyWindow();
			delete pHYThumWnd;
		}
		pHYThumWnd = NULL;
	}

	m_vtThumFnontWnd.clear();

}	// end of void	CHYFontSmartShaperView::UninitHYShowListWnd()

void	CHYFontSmartShaperView::UpdateHYThumWnds(int iStart)
{
	size_t  iWndSize					= m_vtThumFnontWnd.size();
	int		iOffset						= iStart;

	size_t iGlyphNums					= m_FontDeCodec.m_vtHYGlyphs.size();
	for (size_t i=0; i<iWndSize; i++){
		CThumFontWnd* pThumWnd			= m_vtThumFnontWnd[i];
		if (iOffset>=(int)iGlyphNums){
			if (pThumWnd){
				pThumWnd->SetGlyphInfo(m_FontDeCodec,-1);
			}
		}
		else {
			if (pThumWnd){				
				unsigned short&		sEm		= m_FontDeCodec.m_HYhead.unitsPerEm;
				short&				sBaseline =	m_FontDeCodec.m_HYOS2.sTypoDescender;
				pThumWnd->SetGlyphInfo(m_FontDeCodec,iOffset);
			}
		}
		iOffset++;
	}

}	// end of void	CHYFontSmartShaperView::UpdateHYThumWnds()

void	CHYFontSmartShaperView::FlashHYThumWnds()
{
	size_t iWndSize		= m_vtThumFnontWnd.size();
	for (size_t i=0; i<iWndSize; i++){		
		CThumFontWnd* pHYThumWnd = m_vtThumFnontWnd[i];
		if (pHYThumWnd){
			pHYThumWnd->ShowWindow(SW_SHOW);
			pHYThumWnd->SetGlyphInfo(m_FontDeCodec,-1);
		}	
	}

}	// end of void	CHYFontSmartShaperView::FlashHYThumWnds()

void CHYFontSmartShaperView::SetSelectHYThumWnds(int iSltIndex)
{	
	size_t iWndSize		= m_vtThumFnontWnd.size();
	for (size_t i=0; i<iWndSize; i++){
		CThumFontWnd* pHYThumWnd = m_vtThumFnontWnd[i];
		if (pHYThumWnd){
			if (i == (size_t)iSltIndex)	{
				pHYThumWnd->SetSelect(TRUE);
			}
			else{
				pHYThumWnd->SetSelect(FALSE);
			}			
		}	
	}

}	// end of void CHYFontSmartShaperView::SetSelectHYThumWnds()

void CHYFontSmartShaperView::SetLayout()
{
	CRect rc;
	GetClientRect(rc);
	int iMWD	= rc.Width();
	int iMHT	= rc.Height();

	int iFontWnd = iMWD/3;
	// 原始字体外框
	CWnd* pWnd = GetDlgItem(IDC_MN_SRC_VIEW_STC);
	if ( pWnd ){
		if (pWnd->m_hWnd)
			pWnd->MoveWindow(0,5,iFontWnd-2,iFontWnd-2);
	}	
	// 处理后字体外框
	pWnd = GetDlgItem(IDC_MN_DST_VIEW_STC);
	if (pWnd){
		if (pWnd->m_hWnd)
			pWnd->MoveWindow(iFontWnd,5,iFontWnd-2,iFontWnd-2);
	}	
	// 组合字体外框
	pWnd = GetDlgItem(IDC_MN_CMB_RVIEW_STC);
	if (pWnd){
		if (pWnd->m_hWnd)
			pWnd->MoveWindow(iFontWnd*2,5,iFontWnd-2,iFontWnd-2);
	}
	// 进度条	
	CRect rcPress;
	rcPress.top		= rc.top+iFontWnd+10;
	rcPress.left	= rc.left;
	rcPress.right	= rc.right;
	rcPress.bottom	= rcPress.top+20;
	pWnd = GetDlgItem(IDC_MN_CNVTR_PRSS);
	if (pWnd){
		if (pWnd->m_hWnd){			
			pWnd->MoveWindow(rcPress);		
		}
	}
	pWnd = GetDlgItem(IDC_MN_CNVTR_STC);
	if (pWnd){
		if (pWnd->m_hWnd){			
			pWnd->MoveWindow(rcPress);		
		}
	}

	// 相关设置部分
	// 转换部分
	CRect rcCntrl;
	rcCntrl.top		= rcPress.bottom+10;
	rcCntrl.right	= rc.right;
	rcCntrl.left	= rc.left+(rc.Width()-200);	
	rcCntrl.bottom	= rc.bottom;	
	pWnd = GetDlgItem(IDC_MN_CNTRL_STC);
	if (pWnd){
		if (pWnd->m_hWnd){			
			pWnd->MoveWindow(rcCntrl);		
		}
		//pWnd->ShowWindow(SW_HIDE);
	}

	CRect rcCVTStc;
	rcCVTStc.top	= rcCntrl.top+10;
	rcCVTStc.left	= rcCntrl.left+5;
	rcCVTStc.right	= rcCntrl.left+45;
	rcCVTStc.bottom	= rcCntrl.top+25;
	pWnd = GetDlgItem(IDC_MN_CVT_STC);
	if (pWnd){
		if (pWnd->m_hWnd){			
			pWnd->MoveWindow(rcCVTStc);			
		}
		//pWnd->ShowWindow(SW_HIDE);
	}

	CRect rcCVTTFRD;
	rcCVTTFRD.top		= rcCVTStc.top-5;
	rcCVTTFRD.left		= rcCVTStc.right+5;
	rcCVTTFRD.right		= rcCVTTFRD.left+50;
	rcCVTTFRD.bottom	= rcCVTStc.bottom+5;
	pWnd = GetDlgItem(IDC_MN_CVT_TTF_RD);
	if (pWnd){
		if (pWnd->m_hWnd){	
			pWnd->MoveWindow(rcCVTTFRD);		
		}
		//pWnd->ShowWindow(SW_HIDE);
	}

	CRect rcCVOTFRD;
	rcCVOTFRD.top		= rcCVTTFRD.top;
	rcCVOTFRD.left		= rcCVTTFRD.right+5;
	rcCVOTFRD.right		= rcCVOTFRD.left+50;
	rcCVOTFRD.bottom	= rcCVTTFRD.bottom;
	pWnd = GetDlgItem(IDC_MN_CVT_OTF_RD);
	if (pWnd){
		if (pWnd->m_hWnd){
			pWnd->MoveWindow(rcCVOTFRD);
		}
		//pWnd->ShowWindow(SW_HIDE);
	}

	CRect rcDWStc;
	rcDWStc.top		= rcCVTStc.bottom+5;
	rcDWStc.left	= rcCVTStc.left;
	rcDWStc.right	= rcCVTStc.right;
	rcDWStc.bottom	= rcDWStc.top+25;
	pWnd = GetDlgItem(IDC_MN_DW_STC);
	if (pWnd){
		if (pWnd->m_hWnd){
			pWnd->MoveWindow(rcDWStc);
		}		
	}

	CRect rcPTRD;
	rcPTRD.top		= rcDWStc.top-5;
	rcPTRD.left		= rcCVTStc.right+5;
	rcPTRD.right	= rcPTRD.left+50;
	rcPTRD.bottom	= rcDWStc.bottom-5;
	pWnd = GetDlgItem(IDC_MN_PT_CHK);
	if (pWnd){
		if (pWnd->m_hWnd){
			pWnd->MoveWindow(rcPTRD);
		}
	}

	CRect rcPTNORD;
	rcPTNORD.top		= rcPTRD.top;
	rcPTNORD.left		= rcPTRD.right+5;
	rcPTNORD.right		= rcPTNORD.left+50;
	rcPTNORD.bottom		= rcPTRD.bottom;
	pWnd = GetDlgItem(IDC_MN_PTN_CHK);
	if (pWnd){
		if (pWnd->m_hWnd){
			pWnd->MoveWindow(rcPTNORD);
		}
	}

	CRect rcLNRD;
	rcLNRD.top			= rcPTRD.bottom;
	rcLNRD.left			= rcPTRD.left;
	rcLNRD.right		= rcLNRD.left+50;
	rcLNRD.bottom		= rcLNRD.top+25;
	pWnd = GetDlgItem(IDC_MN_LN_CHK);
	if (pWnd){
		if (pWnd->m_hWnd){
			pWnd->MoveWindow(rcLNRD);
		}
	}

	CRect rcREFLNRD;
	rcREFLNRD.top			= rcLNRD.top;
	rcREFLNRD.left			= rcLNRD.right+5;
	rcREFLNRD.right			= rcREFLNRD.left+60;
	rcREFLNRD.bottom		= rcLNRD.bottom;
	pWnd = GetDlgItem(IDC_MN_RFRLN_CHK);
	if (pWnd){
		if (pWnd->m_hWnd){
			pWnd->MoveWindow(rcREFLNRD);
		}
	}

	CRect rcFLRD;
	rcFLRD.top			= rcLNRD.bottom;
	rcFLRD.left			= rcLNRD.left;
	rcFLRD.right		= rcFLRD.left+50;
	rcFLRD.bottom		= rcFLRD.top+25;
	pWnd = GetDlgItem(IDC_MN_FL_CHK);
	if (pWnd){
		if (pWnd->m_hWnd){
			pWnd->MoveWindow(rcFLRD);
		}
	}

	CRect rcSeekStc;
	rcSeekStc.top			= rcFLRD.bottom;
	rcSeekStc.left			= rcDWStc.left;
	rcSeekStc.right			= rcSeekStc.left+40;
	rcSeekStc.bottom		= rcSeekStc.top+25;
	pWnd = GetDlgItem(IDC_MN_SEEK_STC);
	if (pWnd){
		if (pWnd->m_hWnd){
			pWnd->MoveWindow(rcSeekStc);
		}
	}

	CRect rcSeekCMB;
	rcSeekCMB.top			= rcSeekStc.top;
	rcSeekCMB.left			= rcSeekStc.right;
	rcSeekCMB.right			= rcSeekCMB.left+50;
	rcSeekCMB.bottom		= rcSeekCMB.top+20;
	pWnd = GetDlgItem(IDC_MN_SEEK_CMB);
	if (pWnd){
		if (pWnd->m_hWnd){
			pWnd->MoveWindow(rcSeekCMB);
		}
	}

	CRect rcSeekEdt;
	rcSeekEdt.top			= rcSeekCMB.top;
	rcSeekEdt.left			= rcSeekCMB.right;
	rcSeekEdt.right			= rcSeekEdt.left+50;
	rcSeekEdt.bottom		= rcSeekEdt.top+20;
	pWnd = GetDlgItem(IDC_MN_SEEK_EDT);
	if (pWnd){
		if (pWnd->m_hWnd){
			pWnd->MoveWindow(rcSeekEdt);
		}
	}

	CRect rcSeekBTN;
	rcSeekBTN.top			= rcSeekEdt.top;
	rcSeekBTN.left			= rcSeekEdt.right+10;
	rcSeekBTN.right			= rcSeekBTN.left+40;
	rcSeekBTN.bottom		= rcSeekBTN.top+20;
	pWnd = GetDlgItem(IDC_MN_SEEK_BTN);
	if (pWnd){
		if (pWnd->m_hWnd){
			pWnd->MoveWindow(rcSeekBTN);		
		}
	}	

	// 设置宽度
	CRect rcADWStc;
	rcADWStc.top = rcSeekStc.bottom;
	rcADWStc.left = rcSeekStc.left;
	rcADWStc.right = rcSeekStc.right;
	rcADWStc.bottom = rcADWStc.top + 25;
	pWnd = GetDlgItem(IDC_MN_ADW_STC);
	if (pWnd) {
		if (pWnd->m_hWnd) {
			pWnd->MoveWindow(rcADWStc);
		}
		//pWnd->ShowWindow(SW_HIDE);
	}

	CRect rcADWEdt;
	rcADWEdt.top = rcADWStc.top;
	rcADWEdt.left = rcADWStc.right;
	rcADWEdt.right = rcADWEdt.left + 50;
	rcADWEdt.bottom = rcADWStc.top + 20;
	CRichEditCtrl* AdwEdtCtrl = (CRichEditCtrl*)GetDlgItem(IDC_MN_ADW_EDT);
	if (AdwEdtCtrl) {
		if (AdwEdtCtrl->m_hWnd) {
			AdwEdtCtrl->MoveWindow(rcADWEdt);
			AdwEdtCtrl->SetEventMask(AdwEdtCtrl->GetEventMask() |
				ENM_CHANGE);
		}

		//AdwEdtCtrl->ShowWindow(SW_HIDE);
	}

	CRect rcADWNTSEdt;
	rcADWNTSEdt.top = rcADWEdt.top;
	rcADWNTSEdt.left = rcADWEdt.right;
	rcADWNTSEdt.right = rcADWNTSEdt.left + 100;
	rcADWNTSEdt.bottom = rcADWNTSEdt.top + 40;
	pWnd = GetDlgItem(IDC_MN_ADWNOTES_STC);
	if (pWnd) {
		if (pWnd->m_hWnd) {
			pWnd->MoveWindow(rcADWNTSEdt);
		}

		//pWnd->ShowWindow(SW_HIDE);
	}	

	// 设置高度
	CRect rcADHStc;
	rcADHStc.top = rcADWStc.bottom+15;
	rcADHStc.left = rcADWStc.left;
	rcADHStc.right = rcADWStc.right;
	rcADHStc.bottom = rcADHStc.top+25;
	pWnd = GetDlgItem(IDC_MN_ADH_STC);
	if (pWnd) {
		if (pWnd->m_hWnd) {
			pWnd->MoveWindow(rcADHStc);
		}

		//pWnd->ShowWindow(SW_HIDE);
	}

	CRect rcADHEdt;
	rcADHEdt.top = rcADHStc.top;
	rcADHEdt.left = rcADHStc.right;
	rcADHEdt.right = rcADHEdt.left + 50;
	rcADHEdt.bottom = rcADHStc.bottom;
	pWnd = GetDlgItem(IDC_MN_ADH_EDT);
	if (pWnd) {
		if (pWnd->m_hWnd) {
			pWnd->MoveWindow(rcADHEdt);
		}

		//pWnd->ShowWindow(SW_HIDE);
	}

	CRect rcADHNTSEdt;
	rcADHNTSEdt.top = rcADHStc.top;
	rcADHNTSEdt.left = rcADHEdt.right;
	rcADHNTSEdt.right = rcADHNTSEdt.left + 100;
	rcADHNTSEdt.bottom = rcADHStc.top+40;
	pWnd = GetDlgItem(IDC_MN_ADHNOTES_STC);
	if (pWnd) {
		if (pWnd->m_hWnd) {
			pWnd->MoveWindow(rcADHNTSEdt);
		}

		//pWnd->ShowWindow(SW_HIDE);
	}


	//窗口显示模式 
	CRect rcVMTSTC;
	rcVMTSTC.top	= rcADHStc.bottom+25;
	rcVMTSTC.left	= rcADHStc.left;
	rcVMTSTC.right	= rcADHStc.right;
	rcVMTSTC.bottom	= rcVMTSTC.top+25;
	pWnd = GetDlgItem(IDC_MN_VMTYP_STC);
	if (pWnd){
		if (pWnd->m_hWnd){
			pWnd->MoveWindow(rcVMTSTC);		
		}

		//pWnd->ShowWindow(SW_HIDE);
	}

	CRect rcVMTCMB;
	rcVMTCMB.top	= rcVMTSTC.top;
	rcVMTCMB.left	= rcVMTSTC.right;
	rcVMTCMB.right	= rcVMTCMB.left+100;
	rcVMTCMB.bottom	= rcVMTCMB.top+20;
	pWnd = GetDlgItem(IDC_MN_VMTYP_CMB);
	if (pWnd){
		if (pWnd->m_hWnd){
			pWnd->MoveWindow(rcVMTCMB);		
		}

		//pWnd->ShowWindow(SW_HIDE);
	}

	CRect rcCVTSBtn;
	rcCVTSBtn.top			= rcVMTSTC.bottom+5;
	rcCVTSBtn.left			= rcVMTSTC.left;
	rcCVTSBtn.right			= rcCVTSBtn.left+80;
	rcCVTSBtn.bottom		= rcCVTSBtn.top+20;
	pWnd = GetDlgItem(IDC_MN_CVTS_BTN);
	if (pWnd){
		if (pWnd->m_hWnd){
			pWnd->MoveWindow(rcCVTSBtn);
		}		
	}

	CRect rcCVTMBtn;
	rcCVTMBtn.top			= rcCVTSBtn.top;
	rcCVTMBtn.left			= rcCVTSBtn.right+10;
	rcCVTMBtn.right			= rcCVTMBtn.left+80;
	rcCVTMBtn.bottom		= rcCVTSBtn.bottom;
	pWnd = GetDlgItem(IDC_MN_CVTM_BTN);
	if (pWnd){
		if (pWnd->m_hWnd){
			pWnd->MoveWindow(rcCVTMBtn);
		}
	}

	CRect rcFntSetBtn;
	rcFntSetBtn.top			= rcCVTSBtn.bottom+5;
	rcFntSetBtn.left		= rcCVTSBtn.left;
	rcFntSetBtn.right		= rcFntSetBtn.left+80;
	rcFntSetBtn.bottom		= rcFntSetBtn.top+20;
	pWnd = GetDlgItem(IDC_MN_FNTST_BTN);
	if (pWnd){
		if (pWnd->m_hWnd){
			pWnd->MoveWindow(rcFntSetBtn);
		}
	}

	CRect rcCUTBtn;
	rcCUTBtn.top = rcFntSetBtn.top;
	rcCUTBtn.left = rcCVTMBtn.left;
	rcCUTBtn.right = rcCVTMBtn.right;
	rcCUTBtn.bottom = rcFntSetBtn.bottom;
	pWnd = GetDlgItem(IDC_MN_CUTM_BTN);
	if (pWnd){
		if (pWnd->m_hWnd){
			pWnd->MoveWindow(rcCUTBtn);
		}
	}

	// 缩略图预览区域	
	CRect rcTnumVidw;
	rcTnumVidw.top		= rcCntrl.top;
	rcTnumVidw.left		= rc.left;
	rcTnumVidw.right	= rcTnumVidw.left+rc.Width()-215;
	rcTnumVidw.bottom	= rcCntrl.bottom;
	pWnd = GetDlgItem(IDC_MN_THUMVIEW_STC);
	if (pWnd){
		if (pWnd->m_hWnd){
			pWnd->MoveWindow(rcTnumVidw);
			pWnd->EnableWindow(FALSE);
		}
	}

	CRect rcTnumVScrl;
	rcTnumVScrl.top		= rcTnumVidw.top;
	rcTnumVScrl.left	= rcTnumVidw.right-20;
	rcTnumVScrl.right	= rcTnumVScrl.left+20;
	rcTnumVScrl.bottom	= rcTnumVidw.bottom;
	pWnd = GetDlgItem(IDC_MN_V_SCRL);
	if (pWnd){
		if (pWnd->m_hWnd){
			pWnd->MoveWindow(rcTnumVScrl);
		}
	}
	
}	// void CHYFontSmartShaperView::SetLayout()

void CHYFontSmartShaperView::LoadFontThreadFun()
{	
	//DWORD dw1 = GetTickCount();
	if (m_FontDeCodec.Decode((LPTSTR)(LPCTSTR)m_strFontFileName)==HY_NOERROR){		
		ShowSrcWnd(m_iSelectGID);
		UpdateHYThumWnds(m_iSelectGID);
		SetSelectHYThumWnds(m_iSelectGID);

		m_FontEnCodec.m_vtHYGlyphs.resize(m_FontDeCodec.m_vtHYGlyphs.size());
	}
	else{
		SetSelectHYThumWnds(-1);
	}

	if (m_hLoadFontEvent)
		ResetEvent(m_hLoadFontEvent);
	
	::XSysproxy().UpdateAllViews(MSG_FONT_LOAD_FINISH,0L);

}	// end of void CHYFontSmartShaperView::LoadFontThreadFun()

void CHYFontSmartShaperView::LoadFontStepThreadFun()
{
	if (!m_hLoadFontEvent) return;
	
	while(TRUE){
		DWORD dwEvent = ::WaitForSingleObject(m_hLoadFontEvent,10);
		if (dwEvent != WAIT_OBJECT_0) 
			break;

		m_prssProgress.SetPos(m_FontDeCodec.m_iStepNo);		
	}	
	InitScrollBar();

	#ifdef MYTRACE
	for (size_t i = 0; i < m_FontDeCodec.m_HYgvar.glyphCount; i++)
	{
		glyphVariationData& gvData = m_FontDeCodec.m_HYgvar.vtGlyphVariationData[i];

		TRACE("glyph name = %s\n", m_FontDeCodec.m_vtHYGlyphs[i].strPostName.c_str());
		for (int j = 0; j < (gvData.tupleVariationCount&COUNT_MASK); j++){
			TupleVariationHeader& vHead = gvData.vttupleVariationHeaders[j];
			CHYTuple SharTpl = m_FontDeCodec.m_HYgvar.sharedTuples[vHead.iSharedTupleInx];

			for (int x = 0; x < m_FontDeCodec.m_HYgvar.axisCount; x++){
				CString strTmp = m_FontDeCodec.m_HYName.GetNamestring(m_FontDeCodec.m_HYfvar.vtaxes[x].axisNameID).c_str();
				float ftmp = HY_F2DOT14_to_float(SharTpl.coordinates[x]);
				
				if (vHead.intermediateStartTuple.coordinates.size() > 0){
					float ftmp1 = HY_F2DOT14_to_float(vHead.intermediateStartTuple.coordinates[x]);
					float ftmp2 = HY_F2DOT14_to_float(vHead.intermediateEndTuple.coordinates[x]);
					TRACE("coord axis=%s intermediateStart=%.2f intermediateEnd=%.2f value=%.2f\n", 
						strTmp,ftmp1,ftmp2,ftmp);
				}
				else if (vHead.peakTuple.coordinates.size()>0){
					float ftmp1 = HY_F2DOT14_to_float(vHead.peakTuple.coordinates[x]);
					TRACE("coord axis=%s peak=%.2f value=%.2f\n",
						strTmp, ftmp1,ftmp);
				}
				else{
					TRACE("coord axis = %s value = %.2f\n", strTmp, ftmp);
				}
				
			}

			int iIndex = 0;			
			if (vHead.prvtpointNumbers.usPointNumer > 0) {
				for (int x = 0; x < vHead.prvtpointNumbers.usPointNumer; x++) {
					iIndex += vHead.prvtpointNumbers.vtPointIndex[x];
					TRACE("Pt=%d x=%d y=%d\n", iIndex, vHead.vtVariationPoint_X[x], vHead.vtVariationPoint_Y[x]);
				}
			}
			else if (gvData.SharepointNumers.usPointNumer > 0) {	
				for (int x = 0; x < gvData.SharepointNumers.usPointNumer; x++) {
					iIndex += gvData.SharepointNumers.vtPointIndex[x];
					TRACE("Pt=%d x=%d y=%d\n", iIndex, vHead.vtVariationPoint_X[x], vHead.vtVariationPoint_Y[x]);
				}
			}
			else if (vHead.vtVariationPoint_X.size() > 0 && vHead.vtVariationPoint_Y.size()) {
				size_t vtVPtnumers = vHead.vtVariationPoint_X.size();
				for (size_t x = 0; x < vtVPtnumers; x++) {
					TRACE("Pt=%d x=%d y=%d\n", x, vHead.vtVariationPoint_X[x], vHead.vtVariationPoint_Y[x]);
				}
			}
		}
	}
	#endif
	
}	// end of void CHYFontSmartShaperView::LoadFontStepThreadFun()

void	CHYFontSmartShaperView::ShowSrcWnd(int iGid)
{
#ifdef BIGFILE
	return;
#endif

	if (iGid== -1) return;

	int iGlyphNum = m_FontDeCodec.m_vtHYGlyphs.size();
	if (iGid<iGlyphNum){		 
		m_pFntSrcWnd->SetGlyphInfo(m_FontDeCodec,iGid,::XSysproxy().m_tagDrawParam);
	}	

}	// end of void	CHYFontSmartShaperView::ShowSrcWnd()

void	CHYFontSmartShaperView::ShowDstWnd(int iGid)
{
#ifdef BIGFILE 
	return;
#endif

	if (iGid== -1) return;
	int iGlyphNum = m_FontEnCodec.m_vtHYGlyphs.size();
	if (iGid<iGlyphNum){ 		
		m_FontEnCodec.m_HYhead = m_FontDeCodec.m_HYhead;
		m_FontEnCodec.m_HYHhea = m_FontDeCodec.m_HYHhea;
		m_FontEnCodec.m_HYOS2 = m_FontDeCodec.m_HYOS2;

		m_pFntDstWnd->SetGlyphInfo(m_FontEnCodec,iGid,::XSysproxy().m_tagDrawParam);
		m_pCompareFontWnd->SetGlyphInfo(m_FontDeCodec,m_FontEnCodec,iGid,::XSysproxy().m_tagDrawParam);
	}	

}	// end of void	CHYFontSmartShaperView::ShowDstWnd()

void CHYFontSmartShaperView::SetDrawParam()
{	
	CButton* pCheckBtn = NULL;
	
	pCheckBtn = (CButton*)GetDlgItem(IDC_MN_PT_CHK);
	if (pCheckBtn){
		if (pCheckBtn->m_hWnd){
			if (::XSysproxy().m_tagDrawParam.iPoint)
				pCheckBtn->SetCheck(BST_CHECKED);
			else 
				pCheckBtn->SetCheck(BST_UNCHECKED);
		}
	}

	pCheckBtn = (CButton*)GetDlgItem(IDC_MN_PTN_CHK);
	if (pCheckBtn){
		if (pCheckBtn->m_hWnd){
			pCheckBtn->SetCheck(::XSysproxy().m_tagDrawParam.iPointNo);
		}
	}

	pCheckBtn = (CButton*)GetDlgItem(IDC_MN_LN_CHK);
	if (pCheckBtn){
		if (pCheckBtn->m_hWnd){		
			pCheckBtn->SetCheck(::XSysproxy().m_tagDrawParam.iLine);
		}
	}

	pCheckBtn = (CButton*)GetDlgItem(IDC_MN_RFRLN_CHK);
	if (pCheckBtn){
		if (pCheckBtn->m_hWnd){
			pCheckBtn->SetCheck(::XSysproxy().m_tagDrawParam.iReferLine);		
		}
	}

	pCheckBtn = (CButton*)GetDlgItem(IDC_MN_FL_CHK);
	if (pCheckBtn){
		if (pCheckBtn->m_hWnd){			
			pCheckBtn->SetCheck(::XSysproxy().m_tagDrawParam.iFill);			
		}
	}

	UpdateData(FALSE);

}	// end of void CHYFontSmartShaperView::SetDrawParam()

void CHYFontSmartShaperView::GetDrawParam(UINT iID)
{
	UpdateData(TRUE);

	CButton* pCheckBtn = (CButton*)GetDlgItem(iID);
	switch(iID)	{
	case IDC_MN_PT_CHK:
		if (pCheckBtn){
			if (pCheckBtn->m_hWnd){
				::XSysproxy().m_tagDrawParam.iPoint = pCheckBtn->GetCheck();					
			}
		}
		break;

	case IDC_MN_PTN_CHK:
		if (pCheckBtn){
			if(pCheckBtn->m_hWnd){
				::XSysproxy().m_tagDrawParam.iPointNo = pCheckBtn->GetCheck();
			}
		}		
		break;

	case IDC_MN_LN_CHK:
		if (pCheckBtn){
			if(pCheckBtn->m_hWnd){
				::XSysproxy().m_tagDrawParam.iLine = pCheckBtn->GetCheck();
			}
		}
		break;

	case IDC_MN_RFRLN_CHK:
		if (pCheckBtn){
			if(pCheckBtn->m_hWnd){
				::XSysproxy().m_tagDrawParam.iReferLine = pCheckBtn->GetCheck();
			}
		}
		break;
	case IDC_MN_FL_CHK:
		if (pCheckBtn){
			if(pCheckBtn->m_hWnd){
				::XSysproxy().m_tagDrawParam.iFill = pCheckBtn->GetCheck();
			}
		}
	default:
		break;
	}

	ShowSrcWnd(m_iSelectGID);
	ShowDstWnd(m_iSelectGID);

}	// end of void CHYFontSmartShaperView::GetDrawParam()

BOOL CHYFontSmartShaperView::TTFConvert(CHYGlyph& SrcGryph, CHYGlyph& DstGryph, CHYHead &SrcHead)
{	
	int iChecked = GetCheckedRadioButton(IDC_MN_CVT_TTF_RD, IDC_MN_CVT_OTF_RD);
	switch (iChecked)
	{
		case IDC_MN_CVT_TTF_RD:{
				DstGryph = SrcGryph;				
			}			
			break;
		case IDC_MN_CVT_OTF_RD:	{				
				AfxMessageBox("暂不支持曲线格式转换");				
			}
			break;
		default:
			break;
	}
	
	return FALSE;

}	// end of int CHYFontSmartShaperView::TTFConvert()

void  CHYFontSmartShaperView::Smooth(CHYGlyph& inglyph, CHYGlyph& Outglyph)
{
	std::vector<CHYPoint> vtTmpPnt;
	Outglyph.SetDefault();	
	size_t st = inglyph.vtContour.size();
	for (size_t i = 0; i < st; i++){
		CHYContour outCntur;
		vtTmpPnt.clear();
		CHYContour& inCntur = inglyph.vtContour[i];

#if 1
		GetOTFSawtooth(inCntur.vtHYPoints, vtTmpPnt);
		fitting(vtTmpPnt, outCntur.vtHYPoints);
#else
		GetOTFSawtooth(inCntur.vtHYPoints, outCntur.vtHYPoints);
#endif 
		Outglyph.vtContour.push_back(outCntur);
	}

	Outglyph.fontFlag = inglyph.fontFlag;
	Outglyph.CountBoundBox();
	Outglyph.advanceHeight = inglyph.advanceHeight;
	Outglyph.advanceWidth = inglyph.advanceWidth;
	Outglyph.glyfType = inglyph.glyfType;

	int iTmp = 0;

}	// end of void  CHYFontSmartShaperView::Smooth()

// 找到具有典型锯齿状的线段（每个点挨的很密）
void  CHYFontSmartShaperView::GetOTFSawtooth(std::vector<CHYPoint>& vtInPnts, std::vector<CHYPoint>& vtOutPnts)
{	
	double	disThres = 3; //两个点相距disThres个像素以内就符合锯齿线段

	size_t stPnts = vtInPnts.size();	
	CHYPoint fstPnt = vtInPnts[0];
	vtOutPnts.push_back(fstPnt);

	double dis = 0.0;
	int	PntNum = 0;
	BOOL b = FALSE;	
	for (size_t i = 1; i < stPnts; i++) {
		CHYPoint nxtPnts = vtInPnts[i];
		if (nxtPnts.flag == POINT_FLG_ANCHOR) {			
			double dSlope = CHYFontFunc::GetSlope(fstPnt, nxtPnts);
			if (dSlope != 0) {
				vtOutPnts.push_back(nxtPnts);
			}			
		}
		else {
			CHYPoint offpnt1, offpnt2;
			std::vector<CHYPoint> vtBPoints;

			vtBPoints.push_back(fstPnt);			
			// 线外点			
			offpnt1 = nxtPnts;
			vtBPoints.push_back(nxtPnts);
			// 线外点
			nxtPnts = vtInPnts[++i];
			offpnt2 = nxtPnts;
			vtBPoints.push_back(nxtPnts);
			// 锚点
			nxtPnts = vtInPnts[++i];			
			vtBPoints.push_back(nxtPnts);
			
			// 如果曲线弧度过小，保留中心点
			double dB1 = CHYFontFunc::DispointToLine(fstPnt, nxtPnts, offpnt1);
			double dB2 = CHYFontFunc::DispointToLine(fstPnt, nxtPnts, offpnt2);

			double dBDis = CHYFontFunc::Distance(fstPnt, nxtPnts);
			// 如果曲线起点和终点过近，保留曲线的断点。			
			if (dBDis < disThres||(dB1 < 2 && dB2 < 2)) {
				vtOutPnts.push_back(vtBPoints[0]);				
				vtOutPnts.push_back(vtBPoints[3]);				
			}
			else {				
				for (int x = 0; x < 4; x++)
					vtOutPnts.push_back(vtBPoints[x]);
			}	
		}
		fstPnt = nxtPnts;
	}

}	// end of void  CHYFontSmartShaperView::GetOTFSawtooth()

// 遍历轮廓寻找，平滑拟合轮廓
void CHYFontSmartShaperView::fitting(std::vector<CHYPoint>& vtInPnts, std::vector<CHYPoint>& vtOutPnts)
{	
	std::vector<CHYPoint> vtTmpPnts1,vtTmpPnts2, vtTmpPnts3;

	size_t stPnts = vtInPnts.size();
	CHYPoint fstPnt = vtInPnts[0];
	vtTmpPnts1.push_back(fstPnt);

	double dis = 0.0;
	int	PntNum = 0;
	BOOL b = FALSE;	

	for (size_t i = 1; i < stPnts;) {
		CHYPoint nxtPnts = vtInPnts[i];
		if (nxtPnts.flag == POINT_FLG_ANCHOR) {
			if (i == stPnts - 1) {
				SmoothAndfitting(vtTmpPnts1, vtTmpPnts2);
				for each (CHYPoint var in vtTmpPnts2) {
					vtOutPnts.push_back(var);
				}
				vtTmpPnts2.clear();
				break;
			}
			else {
				double dSlope = CHYFontFunc::GetSlope(fstPnt, nxtPnts);
				if (dSlope != 0) {
					vtTmpPnts1.push_back(nxtPnts);
				}	
				i++;
			}
		}
		else {			
			SmoothAndfitting(vtTmpPnts1, vtTmpPnts2);
			for each (CHYPoint var in vtTmpPnts2){
				vtOutPnts.push_back(var);
			}

			// 线外点			
			vtOutPnts.push_back(nxtPnts);
			// 线外点
			nxtPnts = vtInPnts[++i];
			vtOutPnts.push_back(nxtPnts);
			// 锚点
			nxtPnts = vtInPnts[++i];
			vtOutPnts.push_back(nxtPnts);

			vtTmpPnts1.clear();
			vtTmpPnts1.push_back(nxtPnts);

			vtTmpPnts2.clear();
		}
		fstPnt = nxtPnts;		
	}
	
}	// end of void CHYFontSmartShaperView::fitting()

void CHYFontSmartShaperView::SmoothAndfitting(std::vector<CHYPoint>& InvtPnt, std::vector<CHYPoint>& OutvtPnt)
{
	std::vector<CHYPoint> vtTmpPnts1, vtTmpPnts2, vtTmpPnts3;

#if 0
	CHYFontFunc::BezierFitting(InvtPnt, vtTmpPnts1, 3, 1, 1);
	for each (CHYPoint var in vtTmpPnts1) {
		OutvtPnt.push_back(var);
	}
	vtTmpPnts1.clear();
#else

	CHYFontFunc::Mean5_3(InvtPnt, vtTmpPnts1,10);
	CHYFontFunc::BezierFitting(vtTmpPnts1, vtTmpPnts2, 3, 1, 1);
	for each (CHYPoint var in vtTmpPnts2) {
		OutvtPnt.push_back(var);
	}
#endif

}	// end of void CHYFontSmartShaperView::SmoothAndfitting()

BOOL CHYFontSmartShaperView::OTFConvert(CHYGlyph& SrcGryph, CHYGlyph& DstGryph, CHYHead &SrcHead)
{	
	int iChecked = GetCheckedRadioButton(IDC_MN_CVT_TTF_RD, IDC_MN_CVT_OTF_RD);

	switch (iChecked)
	{
		case IDC_MN_CVT_OTF_RD:{
#if 1
			DstGryph = SrcGryph;
#else 
			Smooth(SrcGryph, DstGryph);
			int i = 0;
#endif 
		}
			break;
		case IDC_MN_CVT_TTF_RD:{
			OTF2TTF(SrcGryph, DstGryph);
		}					
			break;
		default:
			break;
	}

	return TRUE;

}	// end of int CHYFontSmartShaperView::OTFConvert()

void CHYFontSmartShaperView::OTF2TTF(CHYGlyph& otfGryph,CHYGlyph& ttfGryph)
{	
	size_t stCntur =  otfGryph.vtContour.size();
	for (size_t i = 0; i < stCntur; i++)
	{
		CHYContour& otfCntur = otfGryph.vtContour[i];
		size_t j = 0;
		size_t stpts = otfCntur.vtHYPoints.size();

		CHYContour ttfCntur;

		while (j < stpts)
		{			
			if (otfCntur.vtHYPoints[j].flag == POINT_FLG_CONTROL)
			{
				CHYPoint ptCntl = CHYFontFunc::cubicToQuadratic(otfCntur.vtHYPoints[j - 1], otfCntur.vtHYPoints[j], otfCntur.vtHYPoints[j + 1], otfCntur.vtHYPoints[j + 2]);
				ttfCntur.vtHYPoints.push_back(ptCntl);				
				ttfCntur.vtHYPoints.push_back(otfCntur.vtHYPoints[j + 2]);
				j += 2;
			}
			else
			{
				ttfCntur.vtHYPoints.push_back(otfCntur.vtHYPoints[j]);
			}
			j++;
		}
		ttfGryph.vtContour.push_back(ttfCntur);
		
	}

	ttfGryph.glyfType = GLYF_TYPE_SIMPLE;
	ttfGryph.fontFlag = FONTTYPE_TTF;

}	// end of BOOL CHYFontSmartShaperView::OTF2TTF()

BOOL CHYFontSmartShaperView::NameTranscodeToWCHAR(CHYName& name)
{		
	size_t szNameRcdNum = name.vtNameRecord.size();
	for(size_t i=0; i<szNameRcdNum; i++)
	{
		NAMERECORD& tagNameRcrd = name.vtNameRecord[i];
		if (tagNameRcrd.platformID==3 && tagNameRcrd.encodingID== 1)
		{
			char Dest[2048] = {0};
			int iDestLen = 2048;

			WCHAR	wpString[MAX_PATH]	= {0};
			int		iWStringLen			= 0;	

			iWStringLen = MultiByteToWideChar(CP_ACP,0, tagNameRcrd.strContent.c_str(), -1, NULL, 0);
			if (iWStringLen == 0 )					return FALSE;		
			if (iWStringLen >= MAX_PATH/2)			return FALSE;

			::MultiByteToWideChar(CP_ACP,0,tagNameRcrd.strContent.c_str(),strlen(tagNameRcrd.strContent.c_str()),wpString,iWStringLen);
			iWStringLen = wcslen(wpString);

			iDestLen = sizeof(WCHAR) * iWStringLen;
			// 大端模式
			short sTmp = 0;
			for ( int x = 0; x < iWStringLen ; x++ )
			{			
				sTmp = htons(wpString[x]);
				CopyMemory(Dest+x*2, &sTmp, sizeof(WCHAR));
			}
			
			tagNameRcrd.strContent = Dest;
		}
	}

	return TRUE;

}	// end of void CHYFontSmartShaperView::NameTranscodeToWCHAR()

void CHYFontSmartShaperView::MakeFontSaveInfo()
{
	
}	// end of void CHYFontSmartShaperView::MakeFontSaveInfo()

void CHYFontSmartShaperView::ScaleGlyph(CHYGlyph& Gryph,float fScale)
{
	size_t szCunt = Gryph.vtContour.size();
	for (size_t i=0; i<szCunt; i++)
	{			
		CHYContour& cntur = Gryph.vtContour[i];
		size_t szPoint = cntur.vtHYPoints.size();
		for (size_t j=0; j<szPoint; j++)
		{
			CHYPoint& pt = cntur.vtHYPoints[j];			
			pt.x = HY_RealRount(pt.x*fScale);
			pt.y = HY_RealRount(pt.y*fScale);			
		}		
	}	
	Gryph.advanceHeight = HY_RealRount(Gryph.advanceHeight*fScale);
	Gryph.advanceWidth = HY_RealRount(Gryph.advanceWidth*fScale);
	Gryph.minX = HY_RealRount(Gryph.minX*fScale);
	Gryph.minY = HY_RealRount(Gryph.minY*fScale);
	Gryph.maxX = HY_RealRount(Gryph.maxX*fScale);
	Gryph.maxY = HY_RealRount(Gryph.maxY*fScale);

}	// end of void CHYFontSmartShaperView::ScaleGlyph()

// CHYFontSmartShaperView 诊断

#ifdef _DEBUG
void CHYFontSmartShaperView::AssertValid() const
{
	CFormView::AssertValid();
}

void CHYFontSmartShaperView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CHYFontSmartShaperDoc* CHYFontSmartShaperView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CHYFontSmartShaperDoc)));
	return (CHYFontSmartShaperDoc*)m_pDocument;
}
#endif //_DEBUG

void CHYFontSmartShaperView::OnBnClickedMnCvtTtfRd()
{
	int RD = GetCheckedRadioButton(IDC_MN_CVT_TTF_RD,IDC_MN_CVT_OTF_RD);
	if (m_FontEnCodec.m_iFontType==FONTTYPE_OTF && RD==IDC_MN_CVT_TTF_RD){
		m_FontEnCodec.m_vtHYGlyphs.clear();
		m_FontEnCodec.m_iFontType = FONTTYPE_TTF;
		m_FontEnCodec.m_vtHYGlyphs.resize(m_FontDeCodec.m_vtHYGlyphs.size());
		ShowDstWnd(m_iSelectGID);
	}

}	// end of void CHYFontSmartShaperView::OnBnClickedMnCvtTtfRd()

void CHYFontSmartShaperView::OnBnClickedMnCvtOtfRd()
{	
	int RD = GetCheckedRadioButton(IDC_MN_CVT_TTF_RD,IDC_MN_CVT_OTF_RD);

	if (m_FontEnCodec.m_iFontType==FONTTYPE_TTF && RD==IDC_MN_CVT_OTF_RD){
		m_FontEnCodec.m_vtHYGlyphs.clear();
		m_FontEnCodec.m_iFontType = FONTTYPE_OTF;
		m_FontEnCodec.m_vtHYGlyphs.resize(m_FontDeCodec.m_vtHYGlyphs.size());

		ShowDstWnd(m_iSelectGID);		
	}

}	// end of void CHYFontSmartShaperView::OnBnClickedMnCvtTtfRd()

void CHYFontSmartShaperView::OnFntMerge()
{	
	CMergeDlg dlg;
	dlg.DoModal();

}	// end of void CHYFontSmartShaperView::OnFntMerge()

void CHYFontSmartShaperView::OnBnSetfocusMnCvtTtfRd()
{	
	int i = 0;

}	// end of void CHYFontSmartShaperView::OnBnSetfocusMnCvtTtfRd()

void CHYFontSmartShaperView::OnBnSetfocusMnCvtOtfRd()
{
	// TODO: Add your control notification handler code here
	int i = 0;
}	// end of void CHYFontSmartShaperView::OnBnSetfocusMnCvtOtfRd()

void CHYFontSmartShaperView::OnSvgCvtMu()
{
	CFongSVGDlg	 Dlg;
	Dlg.DoModal();

}	// end of void CHYFontSmartShaperView::OnSvgCvtMu()

void CHYFontSmartShaperView::OnSvgEotMu()
{
	TCHAR	szFilters[]=_T("TrueType 文件(*.ttf)|*.ttf|OpenType 文件(*.otf)|*.otf||");
	CFileDialog  openFileDlg(TRUE, _T(""), _T(""), OFN_LONGNAMES|OFN_FILEMUSTEXIST,szFilters);
	if (openFileDlg.DoModal() != IDOK)	return;

	CString strFontFile = openFileDlg.GetPathName();
	CString strEotFile;

	HY_GetDirFromPath(std::string(strFontFile));

	strEotFile.Format("%s%s.eot",HY_GetDirFromPath(std::string(strFontFile)).data(),HY_GetFileNameFromPath(std::string(strFontFile)).data());

	CWaitCursor wc;
	int iRet = ::FontToEOT((LPTSTR)(LPCTSTR)strFontFile,(LPTSTR)(LPCTSTR)strEotFile);

	if (iRet==HY_NOERROR) AfxMessageBox("成功生成Eot文件");
	else AfxMessageBox("生成Eot文件失败");	

}	// end of void CHYFontSmartShaperView::OnSvgEotMu()

void CHYFontSmartShaperView::OnToWoffMu()
{
	TCHAR	szFilters[]=_T("TrueType 文件(*.ttf)|*.ttf|OpenType 文件(*.otf)|*.otf||");
	CFileDialog  openFileDlg(TRUE, _T(""), _T(""), OFN_LONGNAMES|OFN_FILEMUSTEXIST,szFilters);
	if (openFileDlg.DoModal() != IDOK)	return;

	CString strFontFile = openFileDlg.GetPathName();
	CString strWoffFile;

	HY_GetDirFromPath(std::string(strFontFile));

	strWoffFile.Format("%s%s.woff",HY_GetDirFromPath(std::string(strFontFile)).data(),HY_GetFileNameFromPath(std::string(strFontFile)).data());

	CWaitCursor wc;	
	HYFONTCODEC::WoffCodec				wfcdc;
	int iRet = wfcdc.Sfnt2Woff((LPTSTR)(LPCTSTR)strFontFile, (LPTSTR)(LPCTSTR)strWoffFile, "");

	if (iRet==HY_NOERROR) AfxMessageBox("成功生成woff文件");
	else AfxMessageBox("生成woff文件失败");	

}	// end of void CHYFontSmartShaperView::OnToWoffMu()

void CHYFontSmartShaperView::OnDropFiles(HDROP hDropInfo)
{
	// TODO: Add your message handler code here and/or call default

	CFormView::OnDropFiles(hDropInfo);
}

void CHYFontSmartShaperView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{	
	CFormView::OnHScroll(nSBCode, nPos, pScrollBar);

}	// end of void CHYFontSmartShaperView::OnHScroll()

LRESULT  CHYFontSmartShaperView::OnUpdateAllViews(WPARAM waram, LPARAM lparam)
{
	switch(waram)
	{
		case MSG_FONT_LOAD_FINISH:
			break;	

		default:
			break;
	}

	return 0L;

}	// end of LRESULT  CHYFontSmartShaperView::OnUpdateAllViews()

void CHYFontSmartShaperView::OnChaConvert()
{


}	// end of void CHYFontSmartShaperView::OnChaConvert()

// 文件名转字表
void CHYFontSmartShaperView::OnFlnm2cdnm()
{
	TCHAR	szFilters[] = _T("图像 文件(*.jpg;*.bmp;*.png)|*.jpg;*.bmp;*.png||");
	DWORD	MAXFILE = 2048;	
	CFileDialog  openFileDlg(TRUE, _T(""), _T(""), OFN_ALLOWMULTISELECT | OFN_READONLY | OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilters);
	openFileDlg.m_ofn.lpstrFile = new TCHAR[MAXFILE*(MAX_PATH + 1) + 1];
	openFileDlg.m_ofn.nMaxFile = MAXFILE * (MAX_PATH + 1) + 1;
	if (openFileDlg.DoModal() != IDOK)	return;

	//CStringArray  aryFlies;
	std::vector<string> vtloadFlies;
	int nCount = 0;
	POSITION pos = openFileDlg.GetStartPosition();
	while (pos != NULL){
		CString szPathName = openFileDlg.GetNextPathName(pos);
		vtloadFlies.push_back(std::string(szPathName));
	}

	size_t iFliesNum = vtloadFlies.size();
	if (iFliesNum == 0){
		AfxMessageBox("请选择图像文件!");
		return;
	}

	CString strCodePath;
	strCodePath.Format("%s%s", ::HY_GetDirFromPath(std::string(vtloadFlies[0])).c_str(),"code.txt");

	DeleteFile(strCodePath);
	CFile	wrCode(strCodePath, CFile::modeCreate| CFile::modeWrite| CFile::typeText);
	for (size_t i = 0; i < iFliesNum; i++){
		std::string strFileName = ::HY_GetFileNameFromPath(std::string(vtloadFlies[i]));
		std::string strExtName = ::HY_GetExtNameFromPath(std::string(vtloadFlies[i]));
		std::string strDir = ::HY_GetDirFromPath(std::string(vtloadFlies[i]));

		wrCode.Write(strFileName.c_str(), strlen(strFileName.c_str()));
	}

	wrCode.Flush();
	wrCode.Close();

	AfxMessageBox("处理结束！");
	
}	// end of void CHYFontSmartShaperView::OnFlnm2cdnm()

// 文件名转码表
void CHYFontSmartShaperView::OnFlnm2numnm()
{
	TCHAR	szFilters[] = _T("图像 文件(*.jpg;*.bmp;*.png)|*.jpg;*.bmp;*.png||");
	DWORD	MAXFILE = 2048;
	CFileDialog  openFileDlg(TRUE, _T(""), _T(""), OFN_ALLOWMULTISELECT | OFN_READONLY | OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilters);
	openFileDlg.m_ofn.lpstrFile = new TCHAR[MAXFILE * (MAX_PATH + 1) + 1];
	openFileDlg.m_ofn.nMaxFile = MAXFILE * (MAX_PATH + 1) + 1;
	if (openFileDlg.DoModal() != IDOK)	return;

	//CStringArray  aryFlies;
	std::vector<string> vtloadFlies;
	int nCount = 0;
	POSITION pos = openFileDlg.GetStartPosition();
	while (pos != NULL) {
		CString szPathName = openFileDlg.GetNextPathName(pos);
		vtloadFlies.push_back(std::string(szPathName));
	}

	size_t iFliesNum = vtloadFlies.size();
	if (iFliesNum == 0) {
		AfxMessageBox("请选择图像文件!");
		return;
	}

	CString strCodePath;
	strCodePath.Format("%s%s", ::HY_GetDirFromPath(std::string(vtloadFlies[0])).c_str(), "code.txt");

	DeleteFile(strCodePath);
	CFile	wrCode(strCodePath, CFile::modeCreate | CFile::modeWrite | CFile::typeText);
	for (size_t i = 0; i < iFliesNum; i++) {
		std::string strFileName = ::HY_GetFileNameFromPath(std::string(vtloadFlies[i]));
		std::string strExtName = ::HY_GetExtNameFromPath(std::string(vtloadFlies[i]));
		std::string strDir = ::HY_GetDirFromPath(std::string(vtloadFlies[i]));

		strFileName += "\n";
		wrCode.Write(strFileName.c_str(), strlen(strFileName.c_str()));		
	}

	wrCode.Flush();
	wrCode.Close();

	AfxMessageBox("处理结束！");

}	// end of void CHYFontSmartShaperView::OnFlnm2numnm()

// 码表转文件名
void CHYFontSmartShaperView::OnCdnm2flnm()
{
	AfxMessageBox("功能尚未实现。");
	
}	// end of void CHYFontSmartShaperView::OnCdnm2flnm()

void CHYFontSmartShaperView::OnMnVfntInf()
{	
	

}	// end of void CHYFontSmartShaperView::OnMnVfntInf()

void CHYFontSmartShaperView::OnVerifyvntMn()
{
	CVerifyFntDlg	dlg(this);
	dlg.DoModal();

}	// end of void CHYFontSmartShaperView::OnVerifyvntMn()

void CHYFontSmartShaperView::OnMnFntinf()
{
	CFontInfDlg dlg(&m_FontDeCodec, this);
	dlg.DoModal();

}	// end of void CHYFontSmartShaperView::OnMnFntinf()

// 多行码表用逗号分割
void CHYFontSmartShaperView::OnMnCodedot()
{	
	TCHAR	szFilters[] = _T("text 文件(*.txt)|*.txt||");
	CFileDialog  openFileDlg(TRUE, _T(""), _T(""), OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilters);
	if (openFileDlg.DoModal() != IDOK)	return;

	CString 		strInFile = openFileDlg.GetPathName();
	CString			strOutFile;

	strOutFile.Format("%s%s_dot.txt", HY_GetDirFromPath((LPSTR)(LPCSTR)strInFile).c_str(),HY_GetFileNameFromPath((LPSTR)(LPCSTR)strInFile).c_str());

	CStdioFile rdFile(strInFile, CFile::modeRead);

	std::vector<CString> vtRead;
	//逐行读取字符串
	CString strRead;
	CString strRead1;

	CStringArray ARY;
	while (rdFile.ReadString(strRead))
	{
		strRead1.Format("uni%s ", strRead);

		ARY.Add(strRead1);
		//vtRead.push_back(strRead1);
	}

	CFile wrFile(strOutFile, CFile::modeCreate|CFile::modeWrite);

	for (size_t i = 0; i < ARY.GetCount(); i++)
	{
		CString strTmp = ARY[i];

		//strTmp.Trim("\r\n");
		wrFile.Write(strTmp, strTmp.GetLength());
	}
	wrFile.Flush();
	wrFile.Close();

}	// end of void CHYFontSmartShaperView::OnMnCodedot()

void CHYFontSmartShaperView::OnMnMkjf()
{
	CHYMakeJFFontDlg	dlg;
	dlg.DoModal();

}	// end of  void CHYFontSmartShaperView::OnMnMkjf()

void CHYFontSmartShaperView::OnFntVadh()
{	
	CFontVADHDlg Dlg(this);
	Dlg.DoModal();

}	// end of void CHYFontSmartShaperView::OnFntVadh()

void CHYFontSmartShaperView::OnMnCha()
{
	TCHAR	szFilters[] = _T("Text 文件(*.txt)|*.txt||");

	CFileDialog  openFileDlg(TRUE, _T(""), _T(""), OFN_READONLY | OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilters);
	if (openFileDlg.DoModal() != IDOK)	return;

	CString strFile = openFileDlg.GetPathName();

	CString 		strInFile = openFileDlg.GetPathName();
	CString			strOutFile;

	strOutFile.Format("%s%s_vert.txt", HY_GetDirFromPath((LPSTR)(LPCSTR)strInFile).c_str(), HY_GetFileNameFromPath((LPSTR)(LPCSTR)strInFile).c_str());

	CStdioFile rdFile(strInFile, CFile::modeRead);

	std::vector<CString> vtRead;
	//逐行读取字符串
	CString strRead;
	CString strRead1;

	CStringArray ARY;
	while (rdFile.ReadString(strRead)) {
		strRead1.Format("%s.vert\n", strRead);
		ARY.Add(strRead1);
	}

	CStdioFile wrFile(strOutFile, CFile::modeCreate | CFile::modeWrite);

	for (size_t i = 0; i < ARY.GetCount(); i++) {
		CString strTmp = ARY[i];
		wrFile.WriteString(strTmp);
	}
	wrFile.Flush();
	wrFile.Close();

	AfxMessageBox("文件处理完成");

}	// end of void CHYFontSmartShaperView::OnMnCha()

void CHYFontSmartShaperView::OnMnAduni()
{
	TCHAR	szFilters[] = _T("码表文件(*.txt)|*.txt||");
	CFileDialog  openFileDlg(TRUE, _T(""), _T(""), OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilters);
	if (openFileDlg.DoModal() != IDOK)	return;

	CString 		strInFile = openFileDlg.GetPathName();
	CString			strOutFile;

	strOutFile.Format("%s%s_psname.txt", HY_GetDirFromPath((LPSTR)(LPCSTR)strInFile).c_str(), HY_GetFileNameFromPath((LPSTR)(LPCSTR)strInFile).c_str());

	CStdioFile rdFile(strInFile, CFile::modeRead);

	std::vector<CString> vtRead;
	//逐行读取字符串
	CString strRead;
	CString strRead1;

	CStringArray ARY;
	while (rdFile.ReadString(strRead)){
		strRead1.Format("uni%s\n", strRead);
		ARY.Add(strRead1);	
	}

	CStdioFile wrFile(strOutFile, CFile::modeCreate | CFile::modeWrite);

	for (size_t i = 0; i < ARY.GetCount(); i++)	{
		CString strTmp = ARY[i];
		wrFile.WriteString(strTmp);
	}
	wrFile.Flush();
	wrFile.Close();

	AfxMessageBox("文件处理完成");

}	// end of void CHYFontSmartShaperView::OnMnCha()

void CHYFontSmartShaperView::OnMkvntMn()
{
	CMKVNTDlg		dlg(this);
	dlg.DoModal();

}	// end of void CHYFontSmartShaperView::OnMkvntMn()

void CHYFontSmartShaperView::OnMnJszCodetchar()
{
	TCHAR	szFilters[] = _T("text 文件(*.txt)|*.txt||");
	CFileDialog  openFileDlg(TRUE, _T(""), _T(""), OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilters);
	if (openFileDlg.DoModal() != IDOK)	return;

	CString strFontFile = openFileDlg.GetPathName();

	CStdioFile		stdFile(strFontFile, CFile::modeRead | CFile::typeText);
	CString strOut;// = "d:\\test.txt";
	//strOut.Format("%s%s-char.txt", HY_GetDirFromPath((LPSTR)(LPCTSTR)strFontFile), HY_GetFileNameFromPath((LPSTR)(LPCTSTR)strFontFile));
	strOut.Format("%s%s-char.txt", "d:\\", "test.txt");

	CStdioFile		stdOutFile(strOut, CFile::modeCreate | CFile::modeWrite| CFile::typeText);
	CString strLine;
	while (stdFile.ReadString(strLine)){
		std::vector<string> vtInChars;
		std::vector<string> vtOutChars;
		HY_SpliteString(strLine," ", vtInChars);

		size_t stChars = vtInChars.size();
		CString strOutLine;
		for (size_t i = 0; i < stChars; i++){
			std::string strIn = vtInChars[i];
			std::string	strout;			
			unsigned long ulunicode = strtol(strIn.c_str(),NULL,16);

			strout = HY_Unicode_to_AnsiString(ulunicode);			
			strOutLine += strout.c_str();
		}
		strOutLine += "\n";
		stdOutFile.WriteString(strOutLine);		
	}

} //end of void CHYFontSmartShaperView::OnMnJszCodetchar()

void CHYFontSmartShaperView::OnChnvert()
{
	TCHAR	szFilters[] = _T("text 文件(*.txt)|*.txt||");
	CFileDialog  openFileDlg(TRUE, _T(""), _T(""), OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilters);
	if (openFileDlg.DoModal() != IDOK)	return;
	CString strFontFile = openFileDlg.GetPathName();
	CStdioFile		stdFile(strFontFile, CFile::modeRead | CFile::typeText);
	CString strOut;
	strOut.Format("%s%s-chnGsubVert.txt", HY_GetDirFromPath((LPSTR)(LPCTSTR)strFontFile).c_str(), HY_GetFileNameFromPath((LPSTR)(LPCTSTR)strFontFile).c_str());
	
	CStdioFile		stdOutFile(strOut, CFile::modeCreate | CFile::modeWrite | CFile::typeText);
	CString strLine;
	while (stdFile.ReadString(strLine)) {
		CString strOutLine;
		//strOutLine.Format("sub \\%s by \\%s.vert;\n", strLine, strLine);
		//临时用于calt
		strOutLine.Format("sub \\uni%s \\uni%s ' by \\uni2E80;\n", strLine, strLine);
		stdOutFile.WriteString(strOutLine);
	}
	stdOutFile.Flush();
	stdOutFile.Close();

	AfxMessageBox("ok");

}	// end of void CHYFontSmartShaperView::OnChnvert()

void CHYFontSmartShaperView::OnTtcTtctofont()
{
	TCHAR	szFilters[] = _T("TTC 文件(*.ttc)|*.ttc||");
	CFileDialog  openFileDlg(TRUE, _T(""), _T(""), OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilters);
	if (openFileDlg.DoModal() != IDOK)	return;
	CString strttcFile = openFileDlg.GetPathName();

	CHYTTC		HYTTC;
	HYTTC.TTC2Font((LPSTR)(LPCTSTR)strttcFile);

}	// end of void CHYFontSmartShaperView::OnTtcTtctofont()

void CHYFontSmartShaperView::OnTtcFonttottc()
{
	CMakeTTCDlg dlg;
	dlg.DoModal();	

}	// end of void CHYFontSmartShaperView::OnTtcFonttottc()

void CHYFontSmartShaperView::OnCharsCompareMn()
{
	CCharsCompareDlg dlg;
	dlg.DoModal();

}	// end of void CHYFontSmartShaperView::OnCharsCompareMn()

// 根据字表提取subsetfont
void CHYFontSmartShaperView::OnFntExtrchr()
{
	CWaitCursor wc;
	TCHAR	szFilters[] = _T("Opentype文件(*.otf)|*.otf|Truetype 文件(*.ttf)|*.ttf||");
	CFileDialog  openFileDlg(TRUE, _T(""), _T(""), OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilters);
	if (openFileDlg.DoModal() != IDOK)	return;

	std::string strSrcFont = std::string(openFileDlg.GetPathName());
	std::string strnewFont = HY_GetDirFromPath(strSrcFont) + "new_" + HY_GetFullNameFromPath(strSrcFont);
	
	TCHAR	szFilters1[] = _T("Text 文件(*.txt)|*.txt||");
	CFileDialog  openFileDlg1(TRUE, _T(""), _T(""), OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilters1);
	if (openFileDlg1.DoModal() != IDOK)	return;
	std::string strSrcTXT = std::string(openFileDlg1.GetPathName());

	std::vector<unsigned long> vtUni;
	::HY_TXTFileToUniArray((char*)strSrcTXT.c_str(), vtUni);	
	
	GetFntSubset((char*)strSrcFont.c_str(),vtUni,strnewFont.c_str());

}	// end of void CHYFontSmartShaperView::OnFntExtrchr()

// 根据码表提取subsetfont
void CHYFontSmartShaperView::OnFntExtrcd()
{	
	CWaitCursor wc;
	TCHAR	szFilters[] = _T("Opentype文件(*.otf)|*.otf|Truetype 文件(*.ttf)|*.ttf||");
	CFileDialog  openFileDlg(TRUE, _T(""), _T(""), OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilters);
	if (openFileDlg.DoModal() != IDOK)	return;

	std::string strSrcTTF = std::string(openFileDlg.GetPathName());
	std::string strnewTTF = HY_GetDirFromPath(strSrcTTF) + "new_" + HY_GetFullNameFromPath(strSrcTTF);

	TCHAR	szFilters1[] = _T("Text 文件(*.txt)|*.txt||");
	CFileDialog  openFileDlg1(TRUE, _T(""), _T(""), OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilters1);
	if (openFileDlg1.DoModal() != IDOK)	return;

	std::string strSrcTXT = std::string(openFileDlg1.GetPathName());
	std::vector<unsigned long> vtUni;
	::HY_LoadCodeFile((char*)strSrcTXT.c_str(), vtUni);	

	GetFntSubset((char*)strSrcTTF.c_str(), vtUni, strnewTTF.c_str());

}	// end of void CHYFontSmartShaperView::OnFntExtrcd()

void	CHYFontSmartShaperView::GetFntSubset(char* strSrcFnt, std::vector<unsigned long> vtUni,const char* strDstFnt)
{
	CHYFontCodec	Decode;
	if (Decode.Decode(strSrcFnt) != HY_NOERROR) {
		AfxMessageBox("字库解码失败");
		return;
	}

	CHYFontCodec	Encode;
	Encode.m_iFontType = Decode.m_iFontType;
	Encode.m_HYOS2.version = Decode.m_HYOS2.version;


	//miss char 是必须的
	Encode.m_vtHYGlyphs.push_back(Decode.m_vtHYGlyphs[0]);	
	CHYFontFunc::GetSubsetbyUni(Decode.m_vtHYGlyphs, vtUni, Encode.m_vtHYGlyphs);

	//::XSysproxy().InitEncodeOption(Encode);
	::XSysproxy().SetEncodeOption(Encode, Decode);

	size_t stGlyphNum = Encode.m_vtHYGlyphs.size();
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

	if (Encode.m_iFontType == FONTTYPE_TTF) {
		ulTableFlag.push_back(GLYF_TAG);
		ulTableFlag.push_back(LOCA_TAG);
		ulTableFlag.push_back(PREP_TAG);

		for (size_t i = 0; i < stGlyphNum; i++) {
			const CHYGlyph& glyph = Encode.m_vtHYGlyphs[i];
			Encode.m_HYPost.InsertName(glyph.strPostName);
		}
	}
	else if (Encode.m_iFontType == FONTTYPE_OTF) {
		ulTableFlag.push_back(CFF_TAG);

		if (Encode.m_HYCFFInfo.TopDICT.IsCIDFont) {
			for (size_t i = 0; i < stGlyphNum; i++) {
				const CHYGlyph& glyph = Encode.m_vtHYGlyphs[i];
				Encode.m_HYCFFInfo.stnStrings.InsertString(glyph.strPostName);
			}
		}
	}

	Encode.MakeHYCodeMap();
	if (Encode.Encode((char*)strDstFnt, ulTableFlag, ::XSysproxy().m_tagOpeionPrm) == HY_NOERROR)
		AfxMessageBox(_T("字库生成完成"));
	else
		AfxMessageBox(_T("字库生成失败"));

}	// end of void	CHYFontSmartShaperView::GetFntSubset()

// 根据postscrpit提取subsetfont。
void CHYFontSmartShaperView::OnFntExtrname()
{
	// TODO: 在此添加命令处理程序代码
	TCHAR	szFilters[] = _T("Truetype 文件(*.ttf)|*.ttf||");
	CFileDialog  openFileDlg(TRUE, _T(""), _T(""), OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilters);
	if (openFileDlg.DoModal() != IDOK)	return;

	std::string strSrcTTF = std::string(openFileDlg.GetPathName());
	std::string strnewTTF = HY_GetDirFromPath(strSrcTTF) + "new" + HY_GetFullNameFromPath(strSrcTTF);

	TCHAR	szFilters1[] = _T("Text 文件(*.txt)|*.txt||");
	CFileDialog  openFileDlg1(TRUE, _T(""), _T(""), OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilters1);
	if (openFileDlg1.DoModal() != IDOK)	return;

	std::string strSrcTXT = std::string(openFileDlg1.GetPathName());

	CFontExtract	FontExtract; //text 文件是name表
	int iRn = FontExtract.ExtractFont(strSrcTTF.data(), strnewTTF.data(), strSrcTXT.data(), 2);
	if (iRn == HY_NOERROR)
		AfxMessageBox("提取完成");
	else
		AfxMessageBox("提取失败");

}	// end of void CHYFontSmartShaperView::OnFntExtrname()

void CHYFontSmartShaperView::OnNoboxMnu()
{
	WinExec("HYImageFontBuilder.exe",SW_SHOW);

}	// end of void CHYFontSmartShaperView::OnNoboxMnu()

//重置字库postscript name
void CHYFontSmartShaperView::OnFntRsetname()
{
	CResetPsNameDlg	dlg;
	dlg.DoModal();

}	// end of void CHYFontSmartShaperView::OnFntRsetname()

//导出字库postscript name
void CHYFontSmartShaperView::OnMnExptname()
{
	TCHAR	szFilters[] = _T("TrueType File(*.ttf)|*.ttf|OpenType File(*.otf)|*.otf||");
	CFileDialog  openFileDlg(TRUE, _T(""), _T(""), OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilters);
	if (openFileDlg.DoModal() != IDOK)	return;

	CString strFileName = openFileDlg.GetPathName();
	CHYFontCodec	fntCdc;
if (HY_NOERROR != fntCdc.Decode((LPSTR)(LPCSTR)strFileName)) {
	AfxMessageBox("字库解析失败");
	return;
}

string strPsnameFile = ::HY_GetDirFromPath((LPSTR)(LPCSTR)strFileName) + ::HY_GetFileNameFromPath((LPSTR)(LPCSTR)strFileName) + ".txt";
CStdioFile	stdFile(strPsnameFile.c_str(), CFile::modeWrite | CFile::typeText | CFile::modeCreate);
for (int i = 0; i < fntCdc.m_HYMaxp.numGlyphs; i++) {
	stdFile.WriteString(fntCdc.m_vtHYGlyphs[i].strPostName.c_str());
	stdFile.WriteString("\n");
}
stdFile.Flush();
stdFile.Close();

}	// end of void CHYFontSmartShaperView::OnMnExptname()

void CHYFontSmartShaperView::OnEnChangeMnAdwEdt()
{
	if (!::XSysproxy().m_tagOpeionPrm.bsetADW) {
		AfxMessageBox("如果要调整中文字宽，需要在设置中勾选自定义宽度");
	}

}	// end of void CHYFontSmartShaperView::OnEnChangeMnAdwEdt()

void CHYFontSmartShaperView::OnEnChangeMnAdhEdt()
{
	if (!::XSysproxy().m_tagOpeionPrm.bsetADH) {
		AfxMessageBox("如果要调整中文字高，需要在设置中勾选自定义高度");
	}

}	// end of void CHYFontSmartShaperView::OnEnChangeMnAdhEdt()

void CHYFontSmartShaperView::OnMnCmptxt()
{
	CTXTCmpDlg dlg;
	dlg.DoModal();

}	// end of void CHYFontSmartShaperView::OnMnCmptxt()
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
static std::string& ltrim(std::string& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(),
		std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}


void CHYFontSmartShaperView::OnMnChkerttf()
{
	CTime tm = CTime::GetCurrentTime();
	string strlog = to_string(tm.GetYear()) + "-" + to_string(tm.GetMonth())
		+ "-" + to_string(tm.GetDay()) + "  "
		+ to_string(tm.GetHour()) + ":"
		+ to_string(tm.GetMinute()) + ":"
		+ to_string(tm.GetSecond()) + "\n";		

	string strlogFile = ::XSysproxy().m_strLogFile;

	::DeleteFile(strlogFile.c_str());
	HY_WriteLog(strlogFile, strlog, true);

	::DeleteFile("d:\\test.txt");
	fstream file("d:\\test.txt", ios::out);

	size_t stGlyhNum = m_FontDeCodec.m_vtHYGlyphs.size();
	int iNum = 0;
	for (size_t i = 0; i < stGlyhNum; i++) {
		CHYGlyph& SrcGryph = m_FontDeCodec.m_vtHYGlyphs[i];
		std::vector<int> cntrIndx;
		std::vector<CHYContour> vtResult;
		if (::XSysproxy().GetBugChar(SrcGryph, cntrIndx, vtResult))
		{
			strlog = SrcGryph.strPostName + "\n";

			stringstream ss;
			if (SrcGryph.vtUnicode.size() > 0)
			{
				ss << hex << SrcGryph.vtUnicode[0];
				string str = ss.str();
				file << str << endl;
			}
			
			for (int a = 0; a < cntrIndx.size(); a++)
			{
				strlog += "contourIndex=" + std::to_string(cntrIndx[a]) + "\n";

				CHYContour& cntur = vtResult[a];
				for (int b = 0; b < cntur.vtHYPoints.size(); b++)
				{
					strlog += std::to_string(cntur.vtHYPoints[b].x) + "," + std::to_string(cntur.vtHYPoints[b].y) + " ";
				}
				strlog += "\n";				
				HY_WriteLog(strlogFile, strlog);
			}
			iNum++;
		}
	}

	strlog = "Check number=" + std::to_string(iNum);
	HY_WriteLog(strlogFile, strlog);
	file.close();

}	// end of void CHYFontSmartShaperView::OnMnChkerttf()

void CHYFontSmartShaperView::OnMnChkerotf()
{
	// TODO: 在此添加命令处理程序代码

}	// end of void CHYFontSmartShaperView::OnMnChkerotf()

void CHYFontSmartShaperView::OnEmojiExport()
{
	TCHAR	szFilters[] = _T("TrueType 文件(*.ttf)|*.ttf||");
	CFileDialog  openFileDlg(TRUE, _T(""), _T(""), OFN_READONLY | OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilters);
	if (openFileDlg.DoModal() != IDOK)	return;
	{
		CWaitCursor wc;
		CString strFileName = openFileDlg.GetPathName();

		char pDir[MAX_PATH] = { 0 };
		std::string strDir = HY_GetDirFromPath(std::string(strFileName));
		std::string strSubDir = HY_GetFileNameFromPath(std::string(strFileName));
		sprintf_s(pDir, "%s%s\\", strDir.data(), strSubDir.data());

		::EmojiToImage((LPTSTR)(LPCTSTR)strFileName, pDir);
	}

}	// end of void CHYFontSmartShaperView::OnEmojiExport()

void CHYFontSmartShaperView::OnEmojiMk()
{
	CHYEmjioDlg dlg;
	dlg.DoModal();

}	// end of void CHYFontSmartShaperView::OnEmojiMk()

void CHYFontSmartShaperView::OnMnCodemap()
{
	TCHAR	szFilters[] = _T("Text 文件(*.txt)|*.txt||");
	CFileDialog  openFileDlg(TRUE, _T(""), _T(""), OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilters);
	if (openFileDlg.DoModal() != IDOK)	return;

	std::string strSrcTXT = std::string(openFileDlg.GetPathName());
	std::vector<unsigned long> vtUni;
	::HY_LoadCodeFile((char*)strSrcTXT.c_str(), vtUni);

	size_t sz = m_FontEnCodec.m_vtHYGlyphs.size();
	int iUIndex = 0;
	for (size_t i =	1; i < sz; i++)
	{		
		CHYGlyph& glyph =  m_FontEnCodec.m_vtHYGlyphs[i];
		if (iUIndex < vtUni.size())
		{
			glyph.vtUnicode.clear();
			glyph.vtUnicode.push_back(vtUni[iUIndex++]);
		}
	}

	AfxMessageBox("处理完成");
	
}	// end of void CHYFontSmartShaperView::OnMnCodemap()

void CHYFontSmartShaperView::OnFntResetcode()
{
	CExchgCodeDlg dlg;
	dlg.DoModal();

}	// end of void CHYFontSmartShaperView::OnFntResetcode()

void CHYFontSmartShaperView::OnFntBudcidotf()
{
	TCHAR		szFilters[] = "Opentype File(*.otf)|*.otf||";
	DWORD	MAXFILE = 20480;
	TCHAR* pc = new TCHAR[MAXFILE * (MAX_PATH + 1)];
	ZeroMemory(pc, MAXFILE * (MAX_PATH + 1));

	std::vector<CString>		szSelFiles;
	CFileDialog  openFileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT | OFN_EXPLORER, szFilters);
	openFileDlg.m_ofn.lpstrFile = pc;
	openFileDlg.m_ofn.nMaxFile = MAXFILE;
	if (openFileDlg.DoModal() != IDOK)	return;

	POSITION pos = openFileDlg.GetStartPosition();
	CWaitCursor wc;
	while (pos != NULL)
	{
		std::vector<unsigned long> ulTableFlag;
		CHYFontCodec decode;
		CHYFontCodec encode;

		CString szPathName = openFileDlg.GetNextPathName(pos);
		int iRtn = decode.Decode((LPSTR)(LPCSTR)szPathName);
		if (iRtn != HY_NOERROR) {
			continue;
		}

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
		ulTableFlag.push_back(CFF_TAG);
		if (decode.m_HYTbDirectory.FindTableEntry(GSUB_TAG) != -1)
		{
			::XSysproxy().m_tagOpeionPrm.bCmplLayout = FALSE;			
		}
		else {
			::XSysproxy().m_tagOpeionPrm.bCmplLayout = TRUE;			
		}
		ulTableFlag.push_back(GSUB_TAG);

		if (decode.m_HYTbDirectory.FindTableEntry(GPOS_TAG) != -1)
		{
			ulTableFlag.push_back(GPOS_TAG);
		}
#if 0
		encode.m_iFontType = decode.m_iFontType;
		encode.m_HYhead = decode.m_HYhead;
		encode.m_HYHhea = decode.m_HYHhea;
		encode.m_HYName = decode.m_HYName;
		encode.m_HYOS2 = decode.m_HYOS2;
		encode.m_HYMaxp = decode.m_HYMaxp;
		encode.MakeCmap();
#endif 
		encode.m_HYCodeMap = decode.m_HYCodeMap;
		encode.m_vtHYGlyphs = decode.m_vtHYGlyphs;
		encode.m_mulpTableData = decode.m_mulpTableData;
		
		CString  strNewName;		
		strNewName.Format("%s%s-01.otf", HY_GetDirFromPath(string(szPathName)).c_str(), HY_GetFileNameFromPath(string(szPathName)).c_str());

		::XSysproxy().SetEncodeOption(encode, decode);
		encode.Encode((LPTSTR)(LPCTSTR)strNewName, ulTableFlag, ::XSysproxy().m_tagOpeionPrm);
	}

}	//end of void CHYFontSmartShaperView::OnFntBudcidotf()