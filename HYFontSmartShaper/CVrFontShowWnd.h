#pragma once

#include "FontShowWnd.h"
class CVrFontShowWnd : public CFontShowWnd
{
	DECLARE_DYNAMIC(CVrFontShowWnd)

public:
	CVrFontShowWnd();
	virtual ~CVrFontShowWnd();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
};


