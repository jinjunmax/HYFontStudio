#pragma once
#include "FontShowWnd.h"

// CFontZoomWnd
class CFontZoomWnd : public CFontShowWnd
{
	DECLARE_DYNAMIC(CFontZoomWnd)

public:
	CFontZoomWnd();
	virtual ~CFontZoomWnd();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnClose();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};


