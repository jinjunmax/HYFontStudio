#pragma once

// CImageGridSetWnd
class CImageGridSetWnd : public CWnd
{
	DECLARE_DYNAMIC(CImageGridSetWnd)

public:
	CImageGridSetWnd();
	virtual ~CImageGridSetWnd();

	void	SetImageFile(CString strPath);

protected:
	Mat		m_inImg;
	RECT	m_rtWnd;

	CString m_strImgPath;

protected:
	CBitmap m_bitmapMemory;		// 内存位图
	CDC     m_dcMemory;			// 内存dc

	int		m_iImgHeight;		// 图片高
	int		m_iImgWidth;		// 图片宽
	double	m_dbImgScale;

	CPoint	m_ptTraceLT;
	CPoint	m_ptTraceRB;
	BOOL	m_bTrace;
					
public:							// 操作
	void	CreateMemoryDC();	// 创建位图和内存dc
	void	ReleaseMemoryDC();	// 释放位图和内存dc


protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};


