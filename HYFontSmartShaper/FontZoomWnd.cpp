// FontZoomWnd.cpp : implementation file
//

#include "stdafx.h"
#include "HYFontSmartShaper.h"
#include "FontZoomWnd.h"


// CFontZoomWnd

IMPLEMENT_DYNAMIC(CFontZoomWnd, CFontShowWnd)

CFontZoomWnd::CFontZoomWnd()
{

}	// end of CFontZoomWnd::CFontZoomWnd()

CFontZoomWnd::~CFontZoomWnd()
{

}	// end of CFontZoomWnd::~CFontZoomWnd()

BEGIN_MESSAGE_MAP(CFontZoomWnd, CFontShowWnd)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_CLOSE()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

// CFontZoomWnd message handlers
void CFontZoomWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	PainGlyph(dc);

}	// end of void CFontZoomWnd::OnPaint()

void CFontZoomWnd::OnDestroy()
{
	CFontShowWnd::OnDestroy();	

}	// end of void CFontZoomWnd::OnDestroy()

void CFontZoomWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CFontShowWnd::OnLButtonDown(nFlags, point);

}	// end of void CFontZoomWnd::OnLButtonDown()

void CFontZoomWnd::OnClose()
{
	ShowWindow(SW_HIDE);
	//CFontShowWnd::OnClose();

}	// end of void CFontZoomWnd::OnClose()

void CFontZoomWnd::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CFontShowWnd::OnShowWindow(bShow, nStatus);
	if (bShow)
	{
		CRect rcClient;
		GetClientRect(rcClient);
		m_rcBgRect.X		= 	static_cast<float>(rcClient.left);
		m_rcBgRect.Y		= 	static_cast<float>(rcClient.top);
		m_rcBgRect.Width	=	static_cast<float>(rcClient.Width());
		m_rcBgRect.Height	=	static_cast<float>(rcClient.Height());

		m_pbmp	= new Gdiplus::Bitmap((int)m_rcBgRect.Width,(int)m_rcBgRect.Height);
		m_pbmpGraphics = new Gdiplus::Graphics(m_pbmp);
		m_pbmpGraphics->SetSmoothingMode(SmoothingModeHighQuality);
	}

}	// end of void CFontZoomWnd::OnShowWindow()
