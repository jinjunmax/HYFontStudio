// CImageGridSetWnd.cpp: 实现文件
//

#include "stdafx.h"
#include "HYImageFontBuilder.h"
#include "ImageGridSetWnd.h"

// CImageGridSetWnd

IMPLEMENT_DYNAMIC(CImageGridSetWnd, CWnd)

CImageGridSetWnd::CImageGridSetWnd()
{
	m_dbImgScale = 0.0;
	m_inImg.release();
	m_iImgHeight = 0;
	m_iImgWidth = 0;

	m_ptTraceLT.x = 0;
	m_ptTraceLT.y = 0;
	m_ptTraceRB.x = 0;
	m_ptTraceRB.y = 0;

}	// end of CImageGridSetWnd::CImageGridSetWnd()

CImageGridSetWnd::~CImageGridSetWnd()
{
	ReleaseMemoryDC();

}	// end of CImageGridSetWnd::~CImageGridSetWnd()

void CImageGridSetWnd::SetImageFile(CString strPath)
{	
	m_inImg = imread(cv::String(strPath));

	m_strImgPath = strPath;

	CreateMemoryDC();

	Invalidate();

}	// end of void CImageGridSetWnd::SetImageFile()

void CImageGridSetWnd::CreateMemoryDC()
{
	ReleaseMemoryDC();

	// 读取图像数据
	wchar_t strImg[1024];
	MultiByteToWideChar(CP_ACP, NULL, m_strImgPath, 1024, strImg, 1024);
	Image img(strImg);
	m_iImgWidth = img.GetWidth();
	m_iImgHeight = img.GetHeight();

	// 获取客户区大小
	CRect rect;
	GetClientRect(&rect);
	int iWndWidth = rect.Width();
	int iWndHeight = rect.Height();

	if (m_iImgWidth > m_iImgHeight)
		m_dbImgScale = (double)iWndWidth / m_iImgWidth;
	else
		m_dbImgScale = (double)iWndWidth / m_iImgHeight;

	int iNewWidth = m_dbImgScale * m_iImgWidth + 0.5f;
	int iNewHeight = m_dbImgScale * m_iImgHeight + 0.5f;	

	CClientDC dc(this);
	m_dcMemory.CreateCompatibleDC(&dc);
	m_bitmapMemory.CreateCompatibleBitmap(&dc, iNewWidth, iNewHeight);
	SelectObject(m_dcMemory, m_bitmapMemory);

	// 绘图到内存
	Gdiplus::Rect  rct;
	rct.X = 0;
	rct.Y = 0;
	rct.Width = iNewWidth;
	rct.Height = iNewHeight;

	Graphics graphics(m_dcMemory);
	graphics.DrawImage(&img, rct,0, 0, m_iImgWidth, m_iImgHeight, Gdiplus::Unit::UnitPixel);

}	// end of void CImageGridSetWnd::CreateMemoryDC()

void CImageGridSetWnd::ReleaseMemoryDC()
{
	m_bitmapMemory.DeleteObject();
	m_dcMemory.DeleteDC();

}	// end of void CImageGridSetWnd::ReleaseMemoryDC()

BEGIN_MESSAGE_MAP(CImageGridSetWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

// CImageGridSetWnd 消息处理程序
void CImageGridSetWnd::OnPaint()
{
	//CPaintDC dc(this);
	CClientDC dc(this);

	if (m_dbImgScale == 0) return;

	int iNewX = 0;
	int iNewY = 0;

	CRect rect;
	GetClientRect(&rect);
	int iWndWidth = rect.Width();
	int iWndHeight = rect.Height();

	int iNewWidth = m_dbImgScale * m_iImgWidth + 0.5f;
	int iNewHeight = m_dbImgScale * m_iImgHeight + 0.5f;

	if (m_iImgWidth > m_iImgHeight)
	{
		iNewY = (iWndHeight - iNewHeight)/2+0.5f;
	}		
	else
	{
		iNewX = (iWndWidth - iNewWidth)/2+0.5f;
	}
	

	Gdiplus::Pen  pnTrace(RGB(255, 0, 0), 2);
	dc.SetStretchBltMode(HALFTONE);
	dc.StretchBlt(iNewX, iNewY, iNewWidth, iNewHeight, &m_dcMemory,0,0, iNewWidth, iNewHeight, SRCCOPY);

	if (m_bTrace)
	{	
		::XSysproxy().m_rctTraceRect.left = m_ptTraceLT.x;
		::XSysproxy().m_rctTraceRect.top = m_ptTraceLT.y;
		::XSysproxy().m_rctTraceRect.right = m_ptTraceRB.x;
		::XSysproxy().m_rctTraceRect.bottom = m_ptTraceRB.y;
		dc.DrawFocusRect(::XSysproxy().m_rctTraceRect);
	}
					  
}	// end of void CImageGridSetWnd::OnPaint()

BOOL CImageGridSetWnd::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	return CWnd::OnEraseBkgnd(pDC);

}	// end of BOOL CImageGridSetWnd::OnEraseBkgnd()

int CImageGridSetWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;

}	// end of int CImageGridSetWnd::OnCreate()

void CImageGridSetWnd::OnLButtonDown(UINT nFlags, CPoint point)
{	
	m_bTrace = TRUE;
	m_ptTraceLT.x = point.x;
	m_ptTraceLT.y = point.y;

	CWnd::OnLButtonDown(nFlags, point);

}	// end of void CImageGridSetWnd::OnLButtonDown()

void CImageGridSetWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bTrace)
	{
		m_bTrace = FALSE;

		::XSysproxy().m_rctTraceRect.left = m_ptTraceLT.x / m_dbImgScale;
		::XSysproxy().m_rctTraceRect.top = m_ptTraceLT.y / m_dbImgScale;
		::XSysproxy().m_rctTraceRect.right = m_ptTraceRB.x / m_dbImgScale;
		::XSysproxy().m_rctTraceRect.bottom = m_ptTraceRB.y / m_dbImgScale;

		GetParent()->PostMessage(MSG_TRACERECT, 0, 0);
		Invalidate();
	}

	CWnd::OnLButtonUp(nFlags, point);

}	// end of void CImageGridSetWnd::OnLButtonUp()

void CImageGridSetWnd::OnMouseMove(UINT nFlags, CPoint point)
{	
	if (m_bTrace)
	{
		m_ptTraceRB.x = point.x;
		m_ptTraceRB.y = point.y;

		Invalidate();
	}

	CWnd::OnMouseMove(nFlags, point);

}	// end of void CImageGridSetWnd::OnMouseMove()
