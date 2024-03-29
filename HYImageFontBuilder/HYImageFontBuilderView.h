
// HYImageFontBuilderView.h: CHYImageFontBuilderView 类的接口
//
#pragma once

#include"ThumFontWnd.h"
#include "MakePrgrssDlg.h"


//class CHYGridParamDlg;
class CPreviewWnd;

class CHYImageFontBuilderView : public CFormView
{
protected: // 仅从序列化创建 
	CHYImageFontBuilderView();
	DECLARE_DYNCREATE(CHYImageFontBuilderView)

public:
#ifdef AFX_DESIGN_TIME
	enum{ IDD = IDD_HYIMAGEFONTBUILDER_FORM };
#endif

// 特性
public:
	CHYImageFontBuilderDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void OnInitialUpdate(); // 构造后第一次调用
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CHYImageFontBuilderView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	HYFONTCODEC::CHYFontCodec		m_FontEnCodec;

public:
	void								ImgToGlyphThreadFun();
	HANDLE								m_hdImgToGlyphThread;

	void								SplitImageThreadFun();
	HANDLE								m_hdSplitImgThread;
	
protected:
	void	InitCtrl();
	void	UnInitCtrl();
	void	InitHYShowListWnd();
	void	UninitHYShowListWnd();
	void	UpdateHYThumWnds(int iStart);
	void	FlashHYThumWnds();
	void	SetSelectHYThumWnds(int iSltIndex);

	int		m_iThumClumnNums;
	int		m_iThumRowNums;

	std::vector<CThumFontWnd*>			m_vtThumFnontWnd;
	std::vector<CRect>					m_vtThumRect;
	Gdiplus::Bitmap*					m_pbmp;
	Gdiplus::Graphics*					m_pbmpGraphics;
	Gdiplus::RectF						m_rcBgRect;
	CPreviewWnd*						m_pGlyphView;
	CMakePrgrssDlg*						m_pPrqressDlg;

// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnDraw(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg LRESULT OnUpdateThum(WPARAM waram, LPARAM lparam);
	afx_msg LRESULT OnGlyphPreView(WPARAM waram, LPARAM lparam);
	afx_msg LRESULT	OnShowPRSWnd(WPARAM waram, LPARAM lparam);
	afx_msg	LRESULT	OnUpdateAllViews(WPARAM waram, LPARAM lparam);
	afx_msg void OnScanParamSetup();
	afx_msg void OnGridParam();
	afx_msg void OnFileOpen();
	afx_msg void OnFileSave();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnExportMonoimgMu();
	afx_msg void OnImagpocMu();
	// 图像切割
	afx_msg void OnSplitimgMu();
};

#ifndef _DEBUG  // HYImageFontBuilderView.cpp 中的调试版本
inline CHYImageFontBuilderDoc* CHYImageFontBuilderView::GetDocument() const
   { return reinterpret_cast<CHYImageFontBuilderDoc*>(m_pDocument); }
#endif

