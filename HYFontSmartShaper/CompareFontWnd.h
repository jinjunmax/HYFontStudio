#pragma once


// CCompareFontWnd
#include "FontShowWnd.h"
class CCompareFontWnd : public CFontShowWnd
{
	DECLARE_DYNAMIC(CCompareFontWnd)

public:
	CCompareFontWnd();
	virtual ~CCompareFontWnd();

	void			SetGlyphInfo(HYFONTCODEC::CHYFontCodec& srcFontEnCodec,HYFONTCODEC::CHYFontCodec& dstFontEnCodec,int iGid,DRAWFONTPARAM& tagDwPrm);

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL	PreTranslateMessage(MSG* pMsg);
	afx_msg BOOL	OnEraseBkgnd(CDC* pDC);
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void	OnDestroy();
	afx_msg void	OnPaint();
	afx_msg void	OnLButtonDown(UINT nFlags, CPoint point);

public:
	CHYGlyph*								m_pSrcGlyph;
	Gdiplus::GraphicsPath					m_SrcGlyphPath;
	std::vector<Gdiplus::GraphicsPath*>		m_vtSrcCmpsPath;
	std::vector<int>						m_vtSrcCmpsIndex;
	std::vector<PointF>						m_vtSrcDrawPts;
	std::vector<int>						m_vtSrcDrawPtFlag;

	CHYGlyph*								m_pDstGlyph;
	Gdiplus::GraphicsPath					m_DstGlyphPath;
	std::vector<Gdiplus::GraphicsPath*>		m_vtDstCmpsPath;
	std::vector<int>						m_vtDstCmpsIndex;
	std::vector<PointF>						m_vtDstDrawPts;
	std::vector<int>						m_vtDstDrawPtFlag;
	
};


