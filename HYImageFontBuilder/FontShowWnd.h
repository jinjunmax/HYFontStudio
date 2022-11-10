#pragma once

// CFontShowWnd
class CFontShowWnd : public CWnd
{
	DECLARE_DYNAMIC(CFontShowWnd)

public:
	CFontShowWnd();
	virtual ~CFontShowWnd();

protected:
	DECLARE_MESSAGE_MAP()

public:
	void			SetGlyphInfo(HYFONTCODEC::CHYFontCodec&	 FontCodec,int iGid,DRAWFONTPARAM& tagDwPrm);
	void			TraceVariationInf();
	void			Flush();

protected:
	void			InitParam();
	void			UinitParam();
	void			SetDefault();

	void			CountDrawInfo(	CHYGlyph*								pGlyph,								 
									Gdiplus::GraphicsPath&					GlyphPath, 
									std::vector<CHYGlyph>&					vtGryph,
									std::vector<PointF>&					vtDrawPts,
									std::vector<int>&						vtDrawPtFlag,
									std::vector<Gdiplus::GraphicsPath*>&	vtCmpsPath,
									std::vector<int>&						vtCmpsIndex							
									);

	void			CountTTFInfo(	CHYGlyph*								pGlyph,
									Gdiplus::GraphicsPath&					GlyphPath, 
									std::vector<CHYGlyph>&					vtGryph,
									std::vector<PointF>&					vtDrawPts,
									std::vector<int>&						vtDrawPtFlag,
									std::vector<Gdiplus::GraphicsPath*>&	vtCmpsPath,
									std::vector<int>&						vtCmpsIndex
									);

	void			CountCCFInfo(	CHYGlyph*								pGlyph,
									Gdiplus::GraphicsPath&					GlyphPath, 
									std::vector<PointF>&					vtDrawPts,
									std::vector<int>&						vtDrawPtFlag
									);

	void			TrimPointlist(std::vector<PointF>& vtPoints, std::vector<int>& vtPointsFlg);
	void			MakeConturPath(GraphicsPath& grphPath,CHYGlyph* pGlyph,std::vector<PointF>& vtPoints, std::vector<int>& vtPointsFlg);
	void			CountBezierPath(PointF& pt0, PointF& pt1, PointF& pt2, GraphicsPath&  path);
	void			CountBezierCurvePos(PointF& inP0, PointF& inP1, PointF& inP2, PointF* otP, int& iioPtNum);	

	// ²Î¿¼Ïß
	void			DrawReferLine(Graphics* pbmpGraphics);
	void			DrawCrossLine(Graphics* pbmpGraphics);
	BOOL			DrawBaseLine(Graphics* pbmpGraphics);
	BOOL			DrawSideLine(Graphics* pbmpGraphics);
	
	BOOL			DrawPoint(	Graphics*				pbmpGraphics,
								Color&					clrOnline,
								Color&					clrOffline,
								CHYGlyph*				pGlyph,
								std::vector<PointF>		&vtDrawPts,
								std::vector<int>		&vtDrawPtFlag
								);

	BOOL			DrawTTFPoint(	Graphics*				pbmpGraphics,
									Color&					clrOnline,
									Color&					clrOffline,
									CHYGlyph*				pGlyph,
									std::vector<PointF>		&vtDrawPts,
									std::vector<int>		&vtDrawPtFlag
									);

	BOOL			DrawCFFPoint(	Graphics*				pbmpGraphics,
									Color&					clrOnline,
									Color&					clrOffline,
									CHYGlyph*				pGlyph,
									std::vector<PointF>		&vtDrawPts,
									std::vector<int>		&vtDrawPtFlag
									);

	BOOL			DrawPointNo(	Graphics*							pbmpGraphics,
									Color&								clrPtNo,
									CHYGlyph*							pGlyph,
									std::vector<PointF>					&vtDrawPts,							   
									std::vector<Gdiplus::GraphicsPath*>	&vtCmpsPath,
									std::vector<int>					&vtCmpsIndex
									);

	BOOL			DrawTTFPointNo(	Graphics*							pbmpGraphics,
									Color&								clrPtNo,
									CHYGlyph*							pGlyph,
									std::vector<PointF>					&vtDrawPts,							   
									std::vector<Gdiplus::GraphicsPath*>	&vtCmpsPath,
									std::vector<int>					&vtCmpsIndex
									);

	
	BOOL			DrawCFFPointNo( Graphics* pbmpGraphics,
									Color&								clrPtNo,
									CHYGlyph*							pGlyph,
									std::vector<PointF>					&vtDrawPts							   
									);

	void			GrawGlyph();
	void			DrawPointCoordinate(int iPtIndex,Graphics* pbmpGraphics, Color& clrPtNo,CPoint ptMu);
	BOOL			DrawPath(Graphics* pbmpGraphics,Color& clrPth, CHYGlyph*	pGlyph,Gdiplus::GraphicsPath& GlyphPath,std::vector<Gdiplus::GraphicsPath*>&  vtCmpsPath);
	BOOL			DrawTTFPath(Graphics* pbmpGraphics,Color& clrPth, CHYGlyph*	pGlyph,Gdiplus::GraphicsPath& GlyphPath,std::vector<Gdiplus::GraphicsPath*>&  vtCmpsPath);
	BOOL			DrawCFFPath(Graphics* pbmpGraphics,Color& clrPth,Gdiplus::GraphicsPath& GlyphPath);
	void			PainGlyph(CDC& dc);

	BOOL			FillPath(Graphics* pbmpGraphics,Color& clrFll, CHYGlyph*	pGlyph,Gdiplus::GraphicsPath& GlyphPath,std::vector<Gdiplus::GraphicsPath*>&  vtCmpsPath);
	BOOL			FillTTFPath(Graphics* pbmpGraphics,Color& clrFll, CHYGlyph*	pGlyph,Gdiplus::GraphicsPath& GlyphPath,std::vector<Gdiplus::GraphicsPath*>&  vtCmpsPat);
	BOOL			FillCFFPath(Graphics* pbmpGraphics,Color& clrFll,Gdiplus::GraphicsPath& GlyphPath);
	int				HitPoints(CPoint point);

	void			SelectVariationData(int iInstancID);

	virtual			BOOL PreTranslateMessage(MSG* pMsg);
	
public:
	afx_msg BOOL	OnEraseBkgnd(CDC* pDC);	
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void	OnDestroy();
	afx_msg void	OnPaint();
	afx_msg void	OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void	OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg LRESULT	OnSetVaration(WPARAM waram, LPARAM lparam);

public:	
	Gdiplus::RectF						m_rcBgRect;	
	float								m_fWndBaseLine;
	float								m_fLeftSide;
	float								m_fRightSide;
	float								m_fEMScale;	
	CHYFontCodec*						m_pHYViewCodec;
	
	DRAWFONTPARAM						m_tagDrawFnotParam;
	Gdiplus::Bitmap*					m_pbmp;
	Gdiplus::Graphics*					m_pbmpGraphics;
	Color								m_clrBKRnd;
	std::vector<CHYPoint>				m_vtOrginPts;
	CHYGlyph*								m_pGlyph;
	Gdiplus::GraphicsPath					m_GlyphPath;
	std::vector<Gdiplus::GraphicsPath*>		m_vtCmpsPath;
	std::vector<int>						m_vtCmpsIndex;
	std::vector<PointF>						m_vtDrawPts;
	std::vector<int>						m_vtDrawPtFlag;
	int										m_iWndID;
	int										m_iGlyphID;


	std::vector<PointF>						m_vtVarOffsetPts;

};


