// CompareFontWnd.cpp : implementation file
//

#include "stdafx.h"
#include "HYFontSmartShaper.h"
#include "CompareFontWnd.h"


// CCompareFontWnd
IMPLEMENT_DYNAMIC(CCompareFontWnd, CFontShowWnd)

CCompareFontWnd::CCompareFontWnd()
{

}	// end of CCompareFontWnd::CCompareFontWnd() 

CCompareFontWnd::~CCompareFontWnd()
{

}	// end of CCompareFontWnd::~CCompareFontWnd()

void CCompareFontWnd::SetGlyphInfo(HYFONTCODEC::CHYFontCodec& srcFontEnCodec,HYFONTCODEC::CHYFontCodec& dstFontEnCodec,int iGid,DRAWFONTPARAM& tagDwPrm)
{

	std::vector<CHYGlyph>& vtSrcGryph = srcFontEnCodec.m_vtHYGlyphs;
	std::vector<CHYGlyph>& vtDstGryph = dstFontEnCodec.m_vtHYGlyphs;
	// 数据复位
	m_fEMScale		= 0.0f;
	m_fLeftSide		= 0.0f;
	m_fRightSide	= 0.0f;
	m_pGlyph		= NULL;
	m_pDstGlyph		= NULL;

	for(size_t i=0; i<m_vtCmpsPath.size();i++)
	{
		if (m_vtCmpsPath[i])
		{
			delete m_vtCmpsPath[i];
		}
	}
	m_vtSrcCmpsPath.clear();
	m_vtSrcCmpsIndex.clear();	
	m_vtDstCmpsPath.clear();
	m_vtDstCmpsIndex.clear();

	if (iGid==-1||iGid>=vtSrcGryph.size()||iGid>=vtDstGryph.size())
	{
		Invalidate(TRUE);
		return;
	}

	m_tagDrawFnotParam	= tagDwPrm;	
	float fWndEM	= m_rcBgRect.Width<m_rcBgRect.Height?m_rcBgRect.Width:m_rcBgRect.Height;
	float fBaseLine = 0.0f;
	switch(tagDwPrm.iViewType)
	{
	case VW_PARAM_HEAD:
		fBaseLine	= srcFontEnCodec.m_HYhead.yMin;
		m_fEMScale	= fWndEM/(float)(srcFontEnCodec.m_HYhead.yMax-srcFontEnCodec.m_HYhead.yMin);
		break;
	case VW_PARAM_HHEA:
		fBaseLine	= srcFontEnCodec.m_HYHhea.Descender;
		m_fEMScale	= fWndEM/(float)(srcFontEnCodec.m_HYHhea.Ascender-srcFontEnCodec.m_HYHhea.Descender);
		break;
	case VW_PARAM_OS_TYPO:
		fBaseLine	= srcFontEnCodec.m_HYOS2.sTypoDescender;
		m_fEMScale	= fWndEM/(float)(srcFontEnCodec.m_HYOS2.sTypoAscender-srcFontEnCodec.m_HYOS2.sTypoDescender);
		break;
	case VW_PARAM_OS_WIN:
		fBaseLine	= srcFontEnCodec.m_HYOS2.usWinDescent;
		m_fEMScale	= fWndEM/(float)(srcFontEnCodec.m_HYOS2.usWinAscent+srcFontEnCodec.m_HYOS2.usWinDescent);
		break;
	default:
		break;	
	}

	m_fWndBaseLine	= abs(fBaseLine)*m_fEMScale;

	m_pSrcGlyph = &vtSrcGryph[iGid];
	m_pDstGlyph = &vtDstGryph[iGid];

	CountDrawInfo(m_pSrcGlyph,m_SrcGlyphPath,vtSrcGryph,m_vtSrcDrawPts,m_vtSrcDrawPtFlag,m_vtSrcCmpsPath,m_vtSrcCmpsIndex);
	CountDrawInfo(m_pDstGlyph,m_DstGlyphPath,vtDstGryph,m_vtDstDrawPts,m_vtDstDrawPtFlag,m_vtDstCmpsPath,m_vtDstCmpsIndex);

	if (m_pbmpGraphics)
	{
		// 将整体背景填充为白色
		m_pbmpGraphics->Clear(Color(255,255,255,255));

		if (m_tagDrawFnotParam.iReferLine)
		{
			DrawReferLine(m_pbmpGraphics);
		}

		Color  clrSrc(255,125,125,125);
		Color  clrDst(255,0,0,255);

		if (m_tagDrawFnotParam.iFill)
		{			
			FillPath(m_pbmpGraphics,clrSrc,m_pSrcGlyph,m_SrcGlyphPath,m_vtSrcCmpsPath);
			FillPath(m_pbmpGraphics,clrDst,m_pDstGlyph,m_DstGlyphPath,m_vtDstCmpsPath);
		}

		if (m_tagDrawFnotParam.iLine)
		{			
			DrawPath(m_pbmpGraphics,clrSrc,m_pSrcGlyph,m_SrcGlyphPath,m_vtSrcCmpsPath);
			DrawPath(m_pbmpGraphics,clrDst,m_pDstGlyph,m_DstGlyphPath,m_vtDstCmpsPath);
		}

		if (m_tagDrawFnotParam.iPoint)
		{			
			DrawPoint(m_pbmpGraphics,clrSrc,clrSrc,m_pSrcGlyph,m_vtSrcDrawPts,m_vtSrcDrawPtFlag);
			DrawPoint(m_pbmpGraphics,clrDst,clrDst,m_pDstGlyph,m_vtDstDrawPts,m_vtDstDrawPtFlag);

		}

		if (m_tagDrawFnotParam.iPointNo)
		{			
			DrawPointNo(m_pbmpGraphics,clrSrc,m_pSrcGlyph,m_vtSrcDrawPts,m_vtSrcCmpsPath,m_vtSrcCmpsIndex);
			DrawPointNo(m_pbmpGraphics,clrDst,m_pDstGlyph,m_vtDstDrawPts,m_vtDstCmpsPath,m_vtDstCmpsIndex);
		}
	}

	Invalidate(TRUE);

}	// end of void CCompareFontWnd::SetGlyphInfo()

BEGIN_MESSAGE_MAP(CCompareFontWnd, CFontShowWnd)
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

// CCompareFontWnd message handlers
BOOL CCompareFontWnd::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class

	return CFontShowWnd::PreTranslateMessage(pMsg);

}	// end of BOOL CCompareFontWnd::PreTranslateMessage()

BOOL CCompareFontWnd::OnEraseBkgnd(CDC* pDC)
{
	return CFontShowWnd::OnEraseBkgnd(pDC);

}	// end of BOOL CCompareFontWnd::OnEraseBkgnd()

int CCompareFontWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFontShowWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	return 0;

}	// end of int CCompareFontWnd::OnCreate(t)

void CCompareFontWnd::OnDestroy()
{
	CFontShowWnd::OnDestroy();

	
}	// end of void CCompareFontWnd::OnDestroy()

void CCompareFontWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	PainGlyph(dc);

}	// end of void CCompareFontWnd::OnPaint()

void CCompareFontWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	//CFontShowWnd::OnLButtonDown(nFlags, point);

}	// end of void CCompareFontWnd::OnLButtonDown()
