// CVrFontShowWnd.cpp: 实现文件
//

#include "stdafx.h"
#include "CVrFontShowWnd.h"


// CVrFontShowWnd

IMPLEMENT_DYNAMIC(CVrFontShowWnd, CFontShowWnd)

CVrFontShowWnd::CVrFontShowWnd()
{

}	// end of CVrFontShowWnd::CVrFontShowWnd()

CVrFontShowWnd::~CVrFontShowWnd()
{

}	// end of CVrFontShowWnd::~CVrFontShowWnd()

BEGIN_MESSAGE_MAP(CVrFontShowWnd, CFontShowWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()

// CVrFontShowWnd 消息处理程序
void CVrFontShowWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CFontShowWnd::OnPaint()
	PainGlyph(dc);

}	// end of void CVrFontShowWnd::OnPaint()
