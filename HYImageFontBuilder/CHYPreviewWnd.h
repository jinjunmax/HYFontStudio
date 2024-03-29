#pragma once


// CHYPreviewWnd

class CHYPreviewWnd : public CWnd
{
	DECLARE_DYNAMIC(CHYPreviewWnd)

public:
	CHYPreviewWnd();
	virtual ~CHYPreviewWnd();

	void SetMat(cv::Mat& in_img);
protected:
	cv::Mat			m_cvimg;
	int				m_iWndEM;	
	int				m_iWndHT;
	int				m_iWndWD;
	
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	afx_msg void OnPaint();
	void ConvertMat2CImage(cv::Mat& src_img, CImage& dst_img);
	void ShowImage(cv::Mat& img, HDC dst_hdc);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


