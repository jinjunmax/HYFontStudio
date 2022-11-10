#include "stdafx.h"
#include "CPreviewWnd.h"

CPreviewWnd::CPreviewWnd()
{
	m_clrBKRnd = Color(255,255,255,255);	

}	//end of CPreviewWnd::CPreviewWnd()

CPreviewWnd::CPreviewWnd(Color clrBKRnd)
{
	m_clrBKRnd = clrBKRnd;

}	// end of CPreviewWnd::CPreviewWnd()

CPreviewWnd::~CPreviewWnd()
{

}	//end of CPreviewWnd::~CPreviewWnd()
BEGIN_MESSAGE_MAP(CPreviewWnd, CFontShowWnd)
	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

void CPreviewWnd::OnClose()
{	
	ShowWindow(SW_HIDE);
	
}	// end of void CPreviewWnd::OnClose()

void CPreviewWnd::SetGlyphInfo(HYFONTCODEC::CHYFontCodec& FontCodec, int iGid,DRAWFONTPARAM& tagDwPrm)
{
	std::vector<CHYGlyph>& vtGryph = FontCodec.m_vtHYGlyphs;
	
	tagDwPrm.iViewType = VW_PARAM_OS_TYPO;
	CFontShowWnd::SetGlyphInfo(FontCodec, iGid, tagDwPrm);
		
}	// end of void CPreviewWnd::SetGlyphInfo()

void CPreviewWnd::OnPaint()
{
	//CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CFontShowWnd::OnPaint()
	CClientDC dc(this);
	Gdiplus::Graphics graphics(dc.GetSafeHdc());
	if (m_pbmpGraphics)
	{
		if (m_pbmp)
		{	
			//Important! Create a CacheBitmap object for quick drawing
			Gdiplus::CachedBitmap cachedBmp(m_pbmp, &graphics);
			graphics.SetSmoothingMode(SmoothingModeHighQuality);
			graphics.DrawCachedBitmap(&cachedBmp, 0, 0);
		}
	}

}	// end of void CPreviewWnd::OnPaint()

BOOL CPreviewWnd::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

#if 0
	Gdiplus::Graphics graphics(pDC->GetSafeHdc());

	Gdiplus::Bitmap bmpMask((int)m_rcBgRect.Width, (int)m_rcBgRect.Height);
	Gdiplus::Graphics gphMask(&bmpMask);
	
	SolidBrush	sb(Color(153, 1, 92, 245));
	gphMask.FillRectangle(&sb, m_rcBgRect);
	Gdiplus::CachedBitmap cachedBmp1(&bmpMask, &graphics);
	graphics.DrawCachedBitmap(&cachedBmp1, 0, 0);

	return TRUE;
#endif 

	return CFontShowWnd::OnEraseBkgnd(pDC);

}	// end of BOOL CPreviewWnd::OnEraseBkgnd()
