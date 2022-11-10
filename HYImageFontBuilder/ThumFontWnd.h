#pragma once
#include "fontshowwnd.h"

class CThumFontWnd : public CFontShowWnd
{
	DECLARE_DYNAMIC(CThumFontWnd)

public:
	CThumFontWnd(void);
	~CThumFontWnd(void);


	void			SetGlyphInfo(HYFONTCODEC::CHYFontCodec&	 FontcoCodec, int iGid);
	int				m_iThumsIndex; 
	void			SetSelect(BOOL bSelect);

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();

protected:
	BOOL			m_bSelect;
};
