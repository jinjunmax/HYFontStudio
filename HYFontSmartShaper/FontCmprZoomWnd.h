#pragma once


// CFontCmprZoomWnd

class CFontCmprZoomWnd : public CCompareFontWnd
{
	DECLARE_DYNAMIC(CFontCmprZoomWnd)

public:
	CFontCmprZoomWnd();
	virtual ~CFontCmprZoomWnd();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnPaint();
};


