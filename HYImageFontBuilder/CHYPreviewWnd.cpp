// CHYPreviewWnd.cpp: 实现文件
//

#include "stdafx.h"
#include "HYImageFontBuilder.h"
#include "CHYPreviewWnd.h"


// CHYPreviewWnd

IMPLEMENT_DYNAMIC(CHYPreviewWnd, CWnd)
CHYPreviewWnd::CHYPreviewWnd()
{
	m_iWndHT = 0;
	m_iWndWD = 0;

}	// end of CHYPreviewWnd::CHYPreviewWnd()

CHYPreviewWnd::~CHYPreviewWnd()
{

}	// end of CHYPreviewWnd::~CHYPreviewWnd()

BEGIN_MESSAGE_MAP(CHYPreviewWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CHYPreviewWnd 消息处理程序
BOOL CHYPreviewWnd::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{		

	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);

}	// end of BOOL CHYPreviewWnd::Create()

void CHYPreviewWnd::SetMat(cv::Mat& in_img)
{
	// 因为窗口为正方形，输入图像也是正方形，所以这里只处理正方形的情况
	if (!in_img.data) return;

	int iImgEM = in_img.rows > in_img.cols ? in_img.rows : in_img.cols;	
	float ratio = static_cast<float>(m_iWndEM)/static_cast<float>(iImgEM);
	
	int showWidth = static_cast<unsigned int>(ratio * in_img.cols);
	int showHeight= static_cast<unsigned int>(ratio * in_img.rows);

	m_cvimg.release();
	if (showWidth > 0 && showHeight > 0){
		cv::resize(in_img, m_cvimg, cv::Size(showWidth, showHeight), 0, 0, cv::INTER_LINEAR);
	}

	Invalidate();

}	// end of void CHYPreviewWnd::SetMat()

void CHYPreviewWnd::OnPaint()
{	
	CClientDC cntDc(this);
	ShowImage(m_cvimg, cntDc.m_hDC);

}	//end of void CHYPreviewWnd::OnPaint()

void CHYPreviewWnd::ConvertMat2CImage(cv::Mat& src_img, CImage& dst_img)
{
	if (!src_img.data) return;
	
	dst_img.Destroy();
	int width = src_img.cols;				//获取输入图像的宽度  
	int height = src_img.rows;				//获取输入图像的高度	

 
	dst_img.Create(width, height, 24);
	unsigned char* dst_data = static_cast<unsigned char*>(dst_img.GetBits());
	int step_size = dst_img.GetPitch();     //获取位图行与行之间相差的字节数  
	unsigned char* src_data = nullptr;	
	
	for (int i = 0; i < height; i++)
	{
		src_data = const_cast<unsigned char*>(src_img.ptr<unsigned char>(i));   //获取行指针  
		for (int j = 0; j < width; j++)
		{	
			*(dst_data + step_size * i + j*3) = *src_data++;
			*(dst_data + step_size * i + j * 3 + 1) = *src_data;
			*(dst_data + step_size * i + j * 3 + 2) = *src_data;
		}
	}
		
}	//end of void CHYPreviewWnd::ConvertMat2CImage()

void CHYPreviewWnd::ShowImage(cv::Mat& img, HDC dst_hdc)
{
	if (!img.data)	return;

	CImage dst_img;	
	ConvertMat2CImage(img, dst_img);					//图像转换  
	int offsetx = (m_iWndWD - img.cols) / 2;			//调整偏移量  
	int offsety = (m_iWndHT - img.rows) / 2;

	BOOL b = dst_img.Draw(dst_hdc, offsetx, offsety, dst_img.GetWidth(), dst_img.GetHeight());  //图像显示  
	
}	// end of void CHYPreviewWnd::ShowImage()

BOOL CHYPreviewWnd::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值、
	CRect rect;
	GetClientRect(rect);
	m_iWndHT = rect.bottom - rect.top;
	m_iWndWD = rect.right - rect.left;
	m_iWndEM = m_iWndHT < m_iWndWD ? m_iWndHT : m_iWndWD;

	/*
	CBrush brush(RGB(255, 255, 255));
	CBrush *pOldBrush = pDC->SelectObject(&brush);
	pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY);
	pDC->SelectObject(pOldBrush);
	*/
	return TRUE;

} // end of BOOL CHYPreviewWnd::OnEraseBkgnd()
