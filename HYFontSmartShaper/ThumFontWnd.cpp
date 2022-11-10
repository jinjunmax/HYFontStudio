#include "StdAfx.h"
#include "ThumFontWnd.h"

IMPLEMENT_DYNAMIC(CThumFontWnd,CFontShowWnd)
CThumFontWnd::CThumFontWnd(void)
{
	m_iThumsIndex = -1;
	m_bSelect = FALSE;
		
}	// end of CThumFontWnd::CThumFontWnd(void)

CThumFontWnd::~CThumFontWnd(void)
{

}	// end of CThumFontWnd::~CThumFontWnd(void)

void CThumFontWnd::SetGlyphInfo(HYFONTCODEC::CHYFontCodec&	 FontCodec, int iGid )
{
	std::vector<CHYGlyph>& vtGryph = FontCodec.m_vtHYGlyphs;
	DRAWFONTPARAM		tagDwPrm;
	ZeroMemory(&tagDwPrm,sizeof(DRAWFONTPARAM));

	tagDwPrm.iFill=1;
	tagDwPrm.iViewType = VW_PARAM_HEAD;
	CFontShowWnd::SetGlyphInfo(FontCodec,iGid,tagDwPrm);

}	// end of void CThumFontWnd::SetGlyphInfo()

void CThumFontWnd::SetSelect(BOOL bSelect)
{
	m_bSelect = bSelect;

	Invalidate(TRUE);

}	// end of void CThumFontWnd::SetSelect()

BEGIN_MESSAGE_MAP(CThumFontWnd, CFontShowWnd)
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CThumFontWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_pGlyph)
	{		
		if (GetParent())
		{
			if (GetParent()->m_hWnd)
			{
				GetParent()->SendMessage(MSG_PREVIEW_WND, (WPARAM)m_iThumsIndex,0L);
			}
		}
	}		

	//CFontShowWnd::OnLButtonDown(nFlags, point);

}	// end of void CThumFontWnd::OnLButtonDown()

void CThumFontWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	Gdiplus::Graphics graphics(dc.GetSafeHdc());
	
	if (m_pbmpGraphics)
	{
		if (m_pbmp)
		{
			Pen 			pn(Color(100,0,0,0),0.1f);	
			m_pbmpGraphics->DrawRectangle(&pn, m_rcBgRect);			

			/*Important! Create a CacheBitmap object for quick drawing*/
			Gdiplus::CachedBitmap cachedBmp(m_pbmp,&graphics);

			graphics.SetSmoothingMode(SmoothingModeHighQuality);
			graphics.DrawCachedBitmap(&cachedBmp,0,0); 		
		}
	}

	Gdiplus::Bitmap bmpMask((int)m_rcBgRect.Width,(int)m_rcBgRect.Height);
 	Gdiplus::Graphics gphMask(&bmpMask); 	
 	if (m_bSelect)	
 	{
 		SolidBrush	sb(Color(153,1,92,245));
 		gphMask.FillRectangle(&sb, m_rcBgRect);	
 		Gdiplus::CachedBitmap cachedBmp1(&bmpMask,&graphics);
 		graphics.DrawCachedBitmap(&cachedBmp1,0,0);		
 	}

}	// end of void CThumFontWnd::OnPaint()
