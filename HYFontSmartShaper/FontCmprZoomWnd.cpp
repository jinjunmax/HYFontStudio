// FontCmprZoomWnd.cpp : implementation file
//

#include "stdafx.h"
#include "HYFontSmartShaper.h"
#include "FontCmprZoomWnd.h"


// CFontCmprZoomWnd

IMPLEMENT_DYNAMIC(CFontCmprZoomWnd, CCompareFontWnd)

CFontCmprZoomWnd::CFontCmprZoomWnd()
{

}	//	end of CFontCmprZoomWnd::CFontCmprZoomWnd()

CFontCmprZoomWnd::~CFontCmprZoomWnd()
{

}	// end of CFontCmprZoomWnd::~CFontCmprZoomWnd()

BEGIN_MESSAGE_MAP(CFontCmprZoomWnd, CCompareFontWnd)
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_WM_LBUTTONDOWN()
	ON_WM_SHOWWINDOW()
	ON_WM_PAINT()
END_MESSAGE_MAP()

// CFontCmprZoomWnd message handlers
void CFontCmprZoomWnd::OnDestroy()
{
	CCompareFontWnd::OnDestroy();

	
}	// end of void CFontCmprZoomWnd::OnDestroy()

void CFontCmprZoomWnd::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	ShowWindow(SW_HIDE);
	//CCompareFontWnd::OnClose();

}	// end of void CFontCmprZoomWnd::OnClose()

void CFontCmprZoomWnd::OnLButtonDown(UINT nFlags, CPoint point)
{

	CCompareFontWnd::OnLButtonDown(nFlags, point);

}	// end of void CFontCmprZoomWnd::OnLButtonDown()

void CFontCmprZoomWnd::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CCompareFontWnd::OnShowWindow(bShow, nStatus);
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
	
}	// end of void CFontCmprZoomWnd::OnShowWindow()

void CFontCmprZoomWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	PainGlyph(dc);	

}	// end of void CFontCmprZoomWnd::OnPaint()
