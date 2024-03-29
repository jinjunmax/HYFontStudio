
// HYImageFontBuilderView.cpp: CHYImageFontBuilderView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "HYImageFontBuilder.h"
#endif

#include "HYImageFontBuilderDoc.h"
#include "HYImageFontBuilderView.h"
#include "CPreviewWnd.h"
#include "HYGridParamDlg.h"
#include "ParamSetupDlg.h"
#include "MonoImgDlg.h"
#include "HYImgProcDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static CHYImageFontBuilderView*  g_pFontBuilderView;
static void LoadImageThread(void* pParam);
static void SplitImageThread(void* pParam);
static std::vector<CString>		g_vtSelFiles;

// CHYImageFontBuilderView

IMPLEMENT_DYNCREATE(CHYImageFontBuilderView, CFormView)

BEGIN_MESSAGE_MAP(CHYImageFontBuilderView, CFormView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CHYImageFontBuilderView::OnFilePrintPreview)
	ON_MESSAGE(MSG_UPDATETHUM, &CHYImageFontBuilderView::OnUpdateThum)
	ON_MESSAGE(MSG_PREVIEW_WND, &CHYImageFontBuilderView::OnGlyphPreView)
	ON_MESSAGE(MSG_PRS_SHOW, &CHYImageFontBuilderView::OnShowPRSWnd)
	ON_MESSAGE(VM_UPDATEALLVIEWS, &CHYImageFontBuilderView::OnUpdateAllViews)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_COMMAND(ID_MN_SETUP, &CHYImageFontBuilderView::OnScanParamSetup)
	ON_COMMAND(ID_IMGTEST, &CHYImageFontBuilderView::OnGridParam)
	ON_COMMAND(ID_FILE_OPEN, &CHYImageFontBuilderView::OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE, &CHYImageFontBuilderView::OnFileSave)
	
	ON_WM_TIMER()
	ON_COMMAND(ID_EXPORT_MONOIMG_MU, &CHYImageFontBuilderView::OnExportMonoimgMu)
	ON_COMMAND(ID_IMAGPOC_MU, &CHYImageFontBuilderView::OnImagpocMu)
	ON_COMMAND(ID_SPLITIMG_MU, &CHYImageFontBuilderView::OnSplitimgMu)
END_MESSAGE_MAP()

// CHYImageFontBuilderView 构造/析构

CHYImageFontBuilderView::CHYImageFontBuilderView()
	: CFormView(IDD_HYIMAGEFONTBUILDER_FORM)
{
	// TODO: 在此处添加构造代码
	m_pbmp			= NULL;
	m_pbmpGraphics	= NULL;
	m_pGlyphView	= NULL;
	g_vtSelFiles.clear();

}	// end of CHYImageFontBuilderView::CHYImageFontBuilderView()

CHYImageFontBuilderView::~CHYImageFontBuilderView()
{
	g_vtSelFiles.clear();

}	// end of CHYImageFontBuilderView::~CHYImageFontBuilderView()

void CHYImageFontBuilderView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);

}	// end of void CHYImageFontBuilderView::DoDataExchange()

BOOL CHYImageFontBuilderView::PreCreateWindow(CREATESTRUCT& cs)
{	

	return CFormView::PreCreateWindow(cs);

}	// end of BOOL CHYImageFontBuilderView::PreCreateWindow()

void CHYImageFontBuilderView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	
	CString strFeatureFile;
	strFeatureFile.Format(_T("%sdata\\Profile.xml"), ::XSysproxy().m_strAppDir);
	::XSysproxy().LoadFontFeature(strFeatureFile, m_FontEnCodec);

	CRect rcClient;
	GetClientRect(rcClient);
	m_rcBgRect.X = static_cast<float>(rcClient.left);
	m_rcBgRect.Y = static_cast<float>(rcClient.top);
	m_rcBgRect.Width = static_cast<float>(rcClient.Width());
	m_rcBgRect.Height = static_cast<float>(rcClient.Height());
	m_pbmp = ::new Gdiplus::Bitmap((int)m_rcBgRect.Width, (int)m_rcBgRect.Height);
	m_pbmpGraphics = ::new Gdiplus::Graphics(m_pbmp);
	m_pbmpGraphics->SetSmoothingMode(SmoothingModeHighQuality);

	InitCtrl();
	g_pFontBuilderView = this;

}	// end of void CHYImageFontBuilderView::OnInitialUpdate()

void CHYImageFontBuilderView::OnDestroy()
{
	CFormView::OnDestroy();

	UnInitCtrl();

	if (m_pbmpGraphics) ::delete m_pbmpGraphics;
	m_pbmpGraphics = NULL;
	if (m_pbmp) ::delete m_pbmp;
	m_pbmp = NULL;

	if (m_pGlyphView)
	{
		if (m_pGlyphView->m_hWnd)
		{
			m_pGlyphView->DestroyWindow();
		}
		delete m_pGlyphView;
		m_pGlyphView = NULL;
	}

}	// end of void CHYImageFontBuilderView::OnDestroy()

LRESULT CHYImageFontBuilderView::OnUpdateThum(WPARAM waram, LPARAM lparam)
{
	UpdateHYThumWnds(0);

	return 0L;

}	// end of LRESULT CHYImageFontBuilderView::OnUpdateThum()

LRESULT CHYImageFontBuilderView::OnGlyphPreView(WPARAM waram, LPARAM lparam)
{
	if (m_pGlyphView)
	{
		if (m_pGlyphView->m_hWnd)
		{
			CHYFontCodec& FntEncode = ::XSysproxy().m_hyFontCodec;
			m_pGlyphView->SetGlyphInfo(FntEncode, (int)waram,::XSysproxy().m_tagDrawParam);
			m_pGlyphView->ShowWindow(SW_SHOW);
		}
	}

	return 0L;

}	// end of LRESULT CHYImageFontBuilderView::OnGlyphPreView()

LRESULT CHYImageFontBuilderView::OnShowPRSWnd(WPARAM waram, LPARAM lparam)
{
	if (lparam == SW_SHOW)
	{
		if (m_pPrqressDlg)
		{
			if (m_pPrqressDlg->m_hWnd)
			{
				m_pPrqressDlg->ShowWindow(SW_SHOW);
			}
		}
	}
	else
	{
		if (m_pPrqressDlg)
		{
			if (m_pPrqressDlg->m_hWnd)
			{
				m_pPrqressDlg->ShowWindow(SW_HIDE);
			}
		}
	}

	return 0L;

}	// end of LRESULT CHYImageFontBuilderView::OnShowPRSWnd()

LRESULT  CHYImageFontBuilderView::OnUpdateAllViews(WPARAM waram, LPARAM lparam)
{
	switch (waram)
	{
		case MSG_PRS_MAIN:
			m_pPrqressDlg->SendMessage(MSG_PRS_MAIN, 0, lparam);
			break;
		case MSG_PRS_SUB:
			m_pPrqressDlg->SendMessage(MSG_PRS_SUB, 0, lparam);
			break;
		default:
			break;
	}

	return 0L;

}	// end of LRESULT  CHYImageFontBuilderView::OnUpdateAllViews()

// CHYImageFontBuilderView 打印
void CHYImageFontBuilderView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CHYImageFontBuilderView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);

}	// end of BOOL CHYImageFontBuilderView::OnPreparePrinting()

void CHYImageFontBuilderView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程

}	// end of void CHYImageFontBuilderView::OnBeginPrinting()

void CHYImageFontBuilderView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程

}	// end of void CHYImageFontBuilderView::OnEndPrinting()

void CHYImageFontBuilderView::OnPrint(CDC* pDC, CPrintInfo* /*pInfo*/)
{
	// TODO: 在此处添加自定义打印代码

}	// end of void CHYImageFontBuilderView::OnPrint()

void CHYImageFontBuilderView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);

}	// end of void CHYImageFontBuilderView::OnRButtonUp()

void CHYImageFontBuilderView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}	// end of void CHYImageFontBuilderView::OnContextMenu()


// CHYImageFontBuilderView 诊断

#ifdef _DEBUG
void CHYImageFontBuilderView::AssertValid() const
{
	CFormView::AssertValid();
}

void CHYImageFontBuilderView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CHYImageFontBuilderDoc* CHYImageFontBuilderView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CHYImageFontBuilderDoc)));
	return (CHYImageFontBuilderDoc*)m_pDocument;
}
#endif //_DEBUG


// CHYImageFontBuilderView 消息处理程序
void CHYImageFontBuilderView::InitCtrl()
{	
	InitHYShowListWnd();

	m_FontEnCodec.m_iFontType = FONTTYPE_TTF;

	m_pPrqressDlg = new CMakePrgrssDlg();
	m_pPrqressDlg->Create(IDD_PRGRESS_DLG, NULL);
	m_pPrqressDlg->ShowWindow(SW_HIDE);	

}	// end of void CHYImageFontBuilderView::InitCtrl()

void CHYImageFontBuilderView::UnInitCtrl()
{
	if (m_pPrqressDlg)
	{
		if (m_pPrqressDlg->m_hWnd)
		{
			m_pPrqressDlg->DestroyWindow();
		}

		delete m_pPrqressDlg;
		m_pPrqressDlg = NULL;
	}

	UninitHYShowListWnd();

}	// end of void CHYImageFontBuilderView::UnInitCtrl()

void CHYImageFontBuilderView::InitHYShowListWnd()
{
	UninitHYShowListWnd();

	// count column	
	CRect	rcArea;
	rcArea.SetRectEmpty();

	int		iWndUnit = 50;
	if (m_hWnd)
	{
		GetWindowRect(rcArea);
		ScreenToClient(rcArea);
		rcArea.right -= 20;

		m_iThumClumnNums = (int)((float)rcArea.Width() / iWndUnit);
		m_iThumRowNums = (int)((float)rcArea.Height() / iWndUnit);

		if (rcArea.Width() - m_iThumClumnNums * iWndUnit > iWndUnit >> 1)
		{
			m_iThumClumnNums++;
		}

		if (rcArea.Height() - m_iThumRowNums * iWndUnit > iWndUnit >> 1)
		{
			m_iThumRowNums++;
		}

		if (m_pGlyphView)
		{
			if (m_pGlyphView->m_hWnd)
			{
				m_pGlyphView->DestroyWindow();
			}

			delete m_pGlyphView;
			m_pGlyphView = NULL;
		}

		RECT rcClient;
		this->GetClientRect(&rcClient);
		CRect	rcPreView;
		rcPreView.left = (rcClient.right - rcClient.left)/2 - 300;
		rcPreView.right = rcPreView.left + 600;
		rcPreView.top = (rcClient.bottom - rcClient.top)/2 - 300;
		rcPreView.bottom = rcPreView.top + 600;

		bool bCreate = false;
		m_pGlyphView = new CPreviewWnd(Color(255,255,255,255));
		bCreate = m_pGlyphView->CreateEx(0, NULL, NULL, WS_OVERLAPPED |WS_CAPTION|WS_CHILD|WS_SYSMENU, rcPreView, this, 7000);
		m_pGlyphView->ShowWindow(SW_HIDE);
		
		CRect	rcSingleThum;
		rcSingleThum.SetRectEmpty();
		DWORD dwWNDID = 6000;
		for (int i = 0; i < m_iThumRowNums; i++)
		{
			rcSingleThum.top = rcArea.top + i * iWndUnit;
			for (int j = 0; j < m_iThumClumnNums; j++)
			{
				rcSingleThum.left = rcArea.left + j * iWndUnit;
				rcSingleThum.bottom = rcSingleThum.top + iWndUnit;
				rcSingleThum.right = rcSingleThum.left + iWndUnit;

				CThumFontWnd* pHYThumWnd = new CThumFontWnd();
				if (pHYThumWnd->CreateEx(0, NULL, NULL, WS_CHILD | WS_TABSTOP | WS_VISIBLE, rcSingleThum, this, dwWNDID))
				{
					pHYThumWnd->m_iThumsIndex = dwWNDID - 6000;
					pHYThumWnd->ShowWindow(SW_SHOW);
					m_vtThumFnontWnd.push_back(pHYThumWnd);
					dwWNDID++;
				}
			}
		}	
	}

}	// end of void	CHYImageFontBuilderView::InitHYShowListWnd()

void CHYImageFontBuilderView::UninitHYShowListWnd()
{
	for (size_t i = 0; i < m_vtThumFnontWnd.size(); i++)
	{
		CThumFontWnd* pHYThumWnd = m_vtThumFnontWnd[i];
		if (pHYThumWnd)
		{
			if (pHYThumWnd->m_hWnd) pHYThumWnd->DestroyWindow();
			delete pHYThumWnd;
		}
		pHYThumWnd = NULL;
	}

	m_vtThumFnontWnd.clear();

}	// end of void	CHYImageFontBuilderView::UninitHYShowListWnd()

void CHYImageFontBuilderView::UpdateHYThumWnds(int iStart)
{
	CHYFontCodec& FntEncode = ::XSysproxy().m_hyFontCodec;

	size_t  iWndSize = m_vtThumFnontWnd.size();
	int		iOffset = iStart;

	size_t iGlyphNums = FntEncode.m_vtHYGlyphs.size();
	for (size_t i = 0; i < iWndSize; i++)
	{	
		CThumFontWnd* pThumWnd = m_vtThumFnontWnd[i];
		if (iOffset >= (int)iGlyphNums)
		{
			if (pThumWnd)
			{
				pThumWnd->SetGlyphInfo(FntEncode, -1);
			}
		}
		else
		{
			if (pThumWnd)
			{
				unsigned short&		sEm = FntEncode.m_HYhead.unitsPerEm;
				short&				sBaseline = FntEncode.m_HYOS2.sTypoDescender;
				pThumWnd->SetGlyphInfo(FntEncode, iOffset);
			}
		}
		iOffset++;
	}

}	// end of void CHYImageFontBuilderView::UpdateHYThumWnds()

void CHYImageFontBuilderView::FlashHYThumWnds()
{
	CHYFontCodec& FntEncode = ::XSysproxy().m_hyFontCodec;
	FntEncode.m_vtHYGlyphs.clear();

	size_t  iWndSize = m_vtThumFnontWnd.size();
	for (size_t i = 0; i < iWndSize; i++)
	{
		m_vtThumFnontWnd[i]->Flush();
	}

}	// end of void CHYImageFontBuilderView::FlashHYThumWnds()

void CHYImageFontBuilderView::SetSelectHYThumWnds(int iSltIndex)
{


}	// end of void CHYImageFontBuilderView::SetSelectHYThumWnds()

void CHYImageFontBuilderView::OnPaint()
{
	CPaintDC dc(this);	

	Gdiplus::Graphics graphics(dc.GetSafeHdc());
	if (m_pbmpGraphics)
	{
		if (m_pbmp)
		{
			Pen 			pn(Color(255,0,0,0),0.5f);
			// 横线
			Gdiplus::Point p1, p2;
			for (int i = 1; i < m_iThumRowNums; i++)
			{				
				p1.X = 0;
				p1.Y = i * 50;

				p2.X = p1.X + m_rcBgRect.Width;
				p2.Y = i * 50;
				m_pbmpGraphics->DrawLine(&pn, p1, p2);
			}

			if (p1.Y + 50 < m_rcBgRect.Height)
			{
				p1.X = 0;
				p1.Y += 50;

				p2.X = p1.X + m_rcBgRect.Width;
				p2.Y += 50;
				m_pbmpGraphics->DrawLine(&pn, p1, p2);
			}

			// 竖线			
			for (int i = 1; i < m_iThumClumnNums; i++)
			{
				p1.X = i * 50;
				p1.Y = 0;

				p2.X = i * 50;
				p2.Y = p1.Y + m_rcBgRect.Height;
				m_pbmpGraphics->DrawLine(&pn, p1, p2);
			}

			if (p1.X + 50 < m_rcBgRect.Width)
			{
				p1.X += 50;
				p1.Y = 0;

				p2.X += 50;
				p2.Y = p1.Y + m_rcBgRect.Height;
				m_pbmpGraphics->DrawLine(&pn, p1, p2);
			}

			/*Important! Create a CacheBitmap object for quick drawing*/
			Gdiplus::CachedBitmap cachedBmp(m_pbmp, &graphics);
			graphics.SetSmoothingMode(SmoothingModeHighQuality);
			graphics.DrawCachedBitmap(&cachedBmp, 0, 0);
		}
	}

}	// end of void CHYImageFontBuilderView::OnPaint()

int CHYImageFontBuilderView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;
	

	return 0;

}	// end of int CHYImageFontBuilderView::OnCreate()

void CHYImageFontBuilderView::OnDraw(CDC* pDC)
{
	int i = 0;

} // end of void CHYImageFontBuilderView::OnDraw()

void CHYImageFontBuilderView::OnScanParamSetup()
{
	CParamSetupDlg	ParamSetupDlg;
	ParamSetupDlg.DoModal();

}	// end of void CHYImageFontBuilderView::OnScanParamSetup()

void CHYImageFontBuilderView::OnGridParam()
{
	CHYGridParamDlg		Dlg;
	Dlg.DoModal();

}	// end of void CHYImageFontBuilderView::OnGridParam()

void CHYImageFontBuilderView::OnFileOpen()
{
	TCHAR		szFilters[] = _T("BMP文件(*.bmp)|*.bmp|JPG 文件(*.jpg)|*.jpg|PNG 文件(*.png)|*.png||");

	DWORD	MAXFILE = 20480;
	TCHAR*	pc = new TCHAR[MAXFILE*(MAX_PATH + 1)];
	ZeroMemory(pc, MAXFILE*(MAX_PATH + 1));

	CFileDialog  openFileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT | OFN_EXPLORER,szFilters);

	openFileDlg.m_ofn.lpstrFile = pc;	
	openFileDlg.m_ofn.nMaxFile = MAXFILE;	

	if (openFileDlg.DoModal() != IDOK)	return;

	::XSysproxy().m_hyFontCodec.m_vtHYGlyphs.clear();
	::XSysproxy().m_hyFontCodec.m_iFontType = FONTTYPE_TTF;
	g_vtSelFiles.clear();
	try
	{
		int nCount = 0;
		POSITION pos = openFileDlg.GetStartPosition();

		while (pos != NULL)
		{
			CString szPathName = openFileDlg.GetNextPathName(pos);
			g_vtSelFiles.push_back(szPathName);
		}
	}
	catch (Exception ext)
	{
		cv::String  inf = ext.msg;
	}

	m_pPrqressDlg->m_MainPrgrss.SetPos(0);
	m_pPrqressDlg->m_SubPrgrss.SetPos(0);	

	m_hdImgToGlyphThread = (HANDLE)_beginthread(LoadImageThread, 0, NULL);

}	// end of void CHYImageFontBuilderView::OnFileOpen()

void CHYImageFontBuilderView::OnSplitimgMu()
{
	TCHAR		szFilters[] = "BMP File(*.bmp)|*.bmp|JPG File(*.jpg)|*.jpg|PNG File(*.png)|*.png||";

	DWORD	MAXFILE = 20480;
	TCHAR* pc = new TCHAR[MAXFILE * (MAX_PATH + 1)];
	ZeroMemory(pc, MAXFILE * (MAX_PATH + 1));

	CFileDialog  openFileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT | OFN_EXPLORER, szFilters);
	openFileDlg.m_ofn.lpstrFile = pc;
	openFileDlg.m_ofn.nMaxFile = MAXFILE;
	if (openFileDlg.DoModal() != IDOK)	return;
	g_vtSelFiles.clear();

	try
	{
		int nCount = 0;
		POSITION pos = openFileDlg.GetStartPosition();

		while (pos != NULL)
		{
			CString szPathName = openFileDlg.GetNextPathName(pos);
			g_vtSelFiles.push_back(szPathName);
		}
	}
	catch (Exception ext){
		cv::String  inf = ext.msg;
	}

	m_pPrqressDlg->m_MainPrgrss.SetPos(0);
	m_pPrqressDlg->m_SubPrgrss.SetPos(0);

	m_hdSplitImgThread = (HANDLE)_beginthread(SplitImageThread, 0, NULL);

}	// end of void CHYImageFontBuilderView::OnSplitimgMu()

static void LoadImageThread(void* pParam)
{
	g_pFontBuilderView->ImgToGlyphThreadFun();

}	// end of UINT LoadImageThread()

static void SplitImageThread(void* pParam)
{
	g_pFontBuilderView->SplitImageThreadFun();

}	// end of static void SpiltImageThread()

void CHYImageFontBuilderView::ImgToGlyphThreadFun()
{
	SendMessage(MSG_PRS_SHOW, 0, SW_SHOW);

	size_t st = g_vtSelFiles.size();
	for (size_t i = 0; i < st; i++)
	{
		CString szPathName = g_vtSelFiles[i];

		int iTopEdge = ::XSysproxy().m_tagScanPrm.iTopEdge;
		int iLefeEdge = ::XSysproxy().m_tagScanPrm.iLeftEdge;
		int iBottomEdge = ::XSysproxy().m_tagScanPrm.iBottomEdge;
		int iRightEdge = ::XSysproxy().m_tagScanPrm.iRightEdge;
		int iColmn = ::XSysproxy().m_tagScanPrm.iColmns;
		int iRow = ::XSysproxy().m_tagScanPrm.iRows;
		int iThres = ::XSysproxy().m_tagScanPrm.iGrayThre;
		int iDirect = ::XSysproxy().m_tagScanPrm.iScanDirection;

		::XSysproxy().ImageToGlyph((LPSTR)(LPCTSTR)szPathName, iTopEdge, iLefeEdge, iRightEdge, iBottomEdge, iColmn, iRow, iThres, iDirect);
		::XSysproxy().UpdateAllViews(MSG_PRS_MAIN, (LPARAM)((i + 1) * 100 / st));
	}

	SendMessage(MSG_PRS_SHOW, 0, SW_HIDE);

	UpdateHYThumWnds(0);

}	// end of void CHYImageFontBuilderView::ImgToGlyphThreadFun()

void CHYImageFontBuilderView::SplitImageThreadFun()
{
	SendMessage(MSG_PRS_SHOW, 0, SW_SHOW);

	size_t st = g_vtSelFiles.size();
	int iTopEdge = ::XSysproxy().m_tagScanPrm.iTopEdge;
	int iLefeEdge = ::XSysproxy().m_tagScanPrm.iLeftEdge;
	int iBottomEdge = ::XSysproxy().m_tagScanPrm.iBottomEdge;
	int iRightEdge = ::XSysproxy().m_tagScanPrm.iRightEdge;
	int iColmn = ::XSysproxy().m_tagScanPrm.iColmns;
	int iRow = ::XSysproxy().m_tagScanPrm.iRows;
	int iThres = ::XSysproxy().m_tagScanPrm.iGrayThre;
	int iDirect = ::XSysproxy().m_tagScanPrm.iScanDirection;
	int iImgeCode = ::XSysproxy().m_tagScanPrm.iGridImgNumber;

	_mkdir(::XSysproxy().m_tagScanPrm.szGridImgDir);
	for (size_t i = 0; i < st; i++)
	{
		CString szPathName = g_vtSelFiles[i];	
		::XSysproxy().SplitImage((LPSTR)(LPCTSTR)szPathName,
			::XSysproxy().m_tagScanPrm.szGridImgDir, iImgeCode,
			iTopEdge,iLefeEdge, iRightEdge, iBottomEdge,
			iColmn,	iRow, iThres, iDirect);

		::XSysproxy().UpdateAllViews(MSG_PRS_MAIN, (LPARAM)((i + 1) * 100 / st));
	}

	SendMessage(MSG_PRS_SHOW, 0, SW_HIDE);

	UpdateHYThumWnds(0);

}	// end of void CHYImageFontBuilderView::SplitImageThreadFun()

void CHYImageFontBuilderView::OnFileSave()
{
	CHYFontCodec& FntCode =  ::XSysproxy().m_hyFontCodec;
	if (FntCode.m_vtHYGlyphs.size() == 0)
	{
		AfxMessageBox("字形数据不能为空!");
		return;
	}

	TCHAR		szFilters[] = _T("字库文件(*.ttf)|*.ttf||");
	CFileDialog  openFileDlg(FALSE, _T(""), _T(""), OFN_READONLY | OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilters);
	if (openFileDlg.DoModal() != IDOK)	return;

	// unicode编码匹配字形
	for (size_t i = 0; i < ::XSysproxy().m_vtUnicode.size(); i++)
	{
		if (i < FntCode.m_vtHYGlyphs.size())
		{			
			FntCode.m_vtHYGlyphs[i].vtUnicode.push_back(::XSysproxy().m_vtUnicode[i]);
		}
	}

	//FntCode.VerifyFont();

	size_t stGlyphNum = FntCode.m_vtHYGlyphs.size();
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

	CWaitCursor wc;
	CString strFontName = openFileDlg.GetPathName();
	FntCode.MakeHYCodeMap();
	FntCode.Encode((LPSTR)(LPCTSTR)strFontName, ulTableFlag);

}	// end of void CHYImageFontBuilderView::OnFileSave()

void CHYImageFontBuilderView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CFormView::OnTimer(nIDEvent);

}	// end of void CHYImageFontBuilderView::OnTimer()

void CHYImageFontBuilderView::OnExportMonoimgMu()
{	
	CMonoImgDlg	dlg(this);
	dlg.DoModal();	

}	// end of void CHYImageFontBuilderView::OnExportMonoimgMu()

void CHYImageFontBuilderView::OnImagpocMu()
{	
	CHYImgProcDlg	dlg;
	dlg.DoModal();

}	// end of 	void CHYImageFontBuilderView::OnImagpocMu()