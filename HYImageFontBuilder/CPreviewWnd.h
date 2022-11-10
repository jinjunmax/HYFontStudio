#pragma once
#include "FontShowWnd.h"
class CPreviewWnd :	public CFontShowWnd
{
public:
	CPreviewWnd();
	CPreviewWnd(Color clrBKRnd);
	~CPreviewWnd();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnClose();
public:
	void SetGlyphInfo(HYFONTCODEC::CHYFontCodec& FontCodec,int iGid,DRAWFONTPARAM& tagDwPrm);

	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};