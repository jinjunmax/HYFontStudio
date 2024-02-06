// FontShowWnd.cpp : implementation file
//

#include "stdafx.h"
#include "FontShowWnd.h"
#include "XSysproxy.h"


// CFontShowWnd
IMPLEMENT_DYNAMIC(CFontShowWnd, CWnd)

CFontShowWnd::CFontShowWnd()
{
	InitParam();

}	// end of CFontShowWnd::CFontShowWnd()

CFontShowWnd::~CFontShowWnd()
{
	UinitParam();

}	// end of CFontShowWnd::~CFontShowWnd()

BEGIN_MESSAGE_MAP(CFontShowWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()	
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()	
END_MESSAGE_MAP()

// CFontShowWnd message handlers
int CFontShowWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)		return -1;

	m_iWndID = (int)lpCreateStruct->hMenu;

	CRect rcClient;
	GetClientRect(rcClient);
	m_rcBgRect.X		= 	static_cast<float>(rcClient.left);
	m_rcBgRect.Y		= 	static_cast<float>(rcClient.top);
	m_rcBgRect.Width	=	static_cast<float>(rcClient.Width());
	m_rcBgRect.Height	=	static_cast<float>(rcClient.Height());

	m_pbmp	= new Gdiplus::Bitmap((int)m_rcBgRect.Width,(int)m_rcBgRect.Height);
	m_pbmpGraphics = new Gdiplus::Graphics(m_pbmp);
	m_pbmpGraphics->SetSmoothingMode(SmoothingModeHighQuality);

	return 0;

}	// end of int CFontShowWnd::OnCreate()

void CFontShowWnd::OnDestroy()
{
	CWnd::OnDestroy();

	// TODO: Add your message handler code here

}	// end of void CFontShowWnd::OnDestroy()

void CFontShowWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting	

	PainGlyph(dc);
#if 0	
 	if (m_pbmpGraphics)
 	{
 		if (m_pbmp)
 		{	
			Gdiplus::Graphics graphics(dc.GetSafeHdc());		
			
			// 将整体背景填充为白色		
			if (m_vtDrawPts.size()==0&&m_vtCmpsIndex.size()==0)
			{
				SolidBrush 		sbrus(m_clrBKRnd);
				graphics.FillRectangle(&sbrus,m_rcBgRect);
			}	

			/*Important! Create a CacheBitmap object for quick drawing*/
			Gdiplus::CachedBitmap cachedBmp(m_pbmp,&graphics);
			graphics.DrawCachedBitmap(&cachedBmp,0,0); 
 		}
 	}
#endif 

}	// end of void CFontShowWnd::OnPaint()

BOOL CFontShowWnd::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class

	return CWnd::PreTranslateMessage(pMsg);

}	// end of BOOL CFontShowWnd::PreTranslateMessage()

BOOL CFontShowWnd::OnEraseBkgnd(CDC* pDC)
{
	//return CWnd::OnEraseBkgnd(pDC);
	return TRUE;

}	// end of BOOL CFontShowWnd::OnEraseBkgnd()

/////////////////////////////////////////////////////////////////////////////////////
//
void CFontShowWnd::SetGlyphInfo(HYFONTCODEC::CHYFontCodec&	 FontCodec,int iGid,DRAWFONTPARAM& tagDwPrm)
{
	std::vector<CHYGlyph>& vtGryph = FontCodec.m_vtHYGlyphs;
	m_pHYViewCodec = &FontCodec;
	m_iGlyphID = iGid;

	// 数据复位
	SetDefault();
	if (iGid==-1 || iGid>=(int)vtGryph.size())		
	{	
		Invalidate(TRUE); 
		return;
	}

	m_tagDrawFnotParam	= tagDwPrm;	
	float fWndEM	= m_rcBgRect.Width<m_rcBgRect.Height?m_rcBgRect.Width:m_rcBgRect.Height;
	float fBaseLine = 0.0f;
	switch(tagDwPrm.iViewType)
	{
		case VW_PARAM_HEAD:
			fBaseLine	= FontCodec.m_HYhead.yMin;
			m_fEMScale	= fWndEM/(float)(FontCodec.m_HYhead.yMax-FontCodec.m_HYhead.yMin);
			break;
		case VW_PARAM_HHEA:
			fBaseLine	= FontCodec.m_HYHhea.Descender;
			m_fEMScale	= fWndEM/(float)(FontCodec.m_HYHhea.Ascender-FontCodec.m_HYHhea.Descender);
			break;
		case VW_PARAM_OS_TYPO:
			fBaseLine	= FontCodec.m_HYOS2.sTypoDescender;
			m_fEMScale	= fWndEM/(float)(FontCodec.m_HYOS2.sTypoAscender-FontCodec.m_HYOS2.sTypoDescender);
			break;
		case VW_PARAM_OS_WIN:
			fBaseLine	= FontCodec.m_HYOS2.usWinDescent;
			m_fEMScale	= fWndEM/(float)(FontCodec.m_HYOS2.usWinAscent+FontCodec.m_HYOS2.usWinDescent);
			break;
		default:
			break;
	}

	m_fWndBaseLine	= abs(fBaseLine)*m_fEMScale;
	m_pGlyph = &vtGryph[iGid];	
	size_t		szCntunNum = m_pGlyph->vtContour.size();		
	for (size_t i=0; i<szCntunNum; i++)
	{
		CHYContour&				hyCntur = m_pGlyph->vtContour[i];
		size_t					szPtNum = hyCntur.vtHYPoints.size();
		for(size_t j=0;j<szPtNum; j++)
		{			
			CHYPoint& htPt = hyCntur.vtHYPoints[j];
			m_vtOrginPts.push_back(htPt);
		}
	}

	CountDrawInfo(m_pGlyph,m_GlyphPath,vtGryph,m_vtDrawPts,m_vtDrawPtFlag,m_vtCmpsPath,m_vtCmpsIndex);	
	GrawGlyph();	

	Invalidate(TRUE);

}	// end of void CFontShowWnd::SetGlyphInfo()

void CFontShowWnd::InitParam()
{
	m_rcBgRect.X		= 0;
	m_rcBgRect.Y		= 0;
	m_rcBgRect.Width	= 0;
	m_rcBgRect.Height	= 0;	
	m_fWndBaseLine		= 0;
	m_fLeftSide			= 0;
	m_fRightSide		= 0;
	m_pGlyph			= NULL;
	m_pbmp				= NULL;
	m_pbmpGraphics		= NULL;
	m_iWndID			= 0;
	m_pHYViewCodec		= NULL;
	m_iGlyphID			= -1;

	m_clrBKRnd.SetFromCOLORREF(RGB(255,255,255));
	
}	// end of void CFontShowWnd::InitParam()

void CFontShowWnd::UinitParam()
{
	for(size_t i=0; i<m_vtCmpsPath.size();i++)
	{
		if (m_vtCmpsPath[i])
		{
			delete m_vtCmpsPath[i];
		}
	}
	m_vtCmpsPath.clear();

}	// end of void CFontShowWnd::UinitParam()

void CFontShowWnd::SetDefault()
{
	// 数据复位
	m_fEMScale = 0.0f;
	m_fLeftSide = 0.0f;
	m_fRightSide = 0.0f;
	m_pGlyph = NULL;
	memset(&m_tagDrawFnotParam, 0, sizeof(DRAWFONTPARAM));

	for (size_t i = 0; i<m_vtCmpsPath.size(); i++)
	{
		if (m_vtCmpsPath[i])
		{
			delete m_vtCmpsPath[i];
		}
	}
	m_vtCmpsPath.clear();
	m_vtCmpsIndex.clear();
	m_vtOrginPts.clear();

	m_pbmpGraphics->Clear(Color(m_clrBKRnd));	

}	// end of void CFontShowWnd::SetDefault()

void CFontShowWnd::Flush()
{
	SetDefault();
	Invalidate(TRUE);

}	// end of void CFontShowWnd::Flush()

void CFontShowWnd::CountDrawInfo(
								 CHYGlyph*								pGlyph,								 
								 Gdiplus::GraphicsPath&					GlyphPath, 
								 std::vector<CHYGlyph>&					vtGryph,
								 std::vector<PointF>&					vtDrawPts,
								 std::vector<int>&						vtDrawPtFlag,
								 std::vector<Gdiplus::GraphicsPath*>&	vtCmpsPath,
								 std::vector<int>&						vtCmpsIndex								 
								 )
{
	vtDrawPts.clear();
	vtDrawPtFlag.clear();	
	GlyphPath.Reset();
	

	// 得到了点的信息。
	if (pGlyph)
	{
		if (pGlyph->advanceWidth)
		{
			m_fLeftSide		= (m_rcBgRect.Width-pGlyph->advanceWidth*m_fEMScale)/2.0f;
			m_fRightSide	= m_fLeftSide+pGlyph->advanceWidth*m_fEMScale;		
		}

		if (pGlyph->fontFlag == FONTTYPE_OTF)
		{
			CountCCFInfo(pGlyph,GlyphPath,vtDrawPts,vtDrawPtFlag);
		}
		else 
		{
			CountTTFInfo(pGlyph, GlyphPath, vtGryph,vtDrawPts,vtDrawPtFlag,vtCmpsPath,vtCmpsIndex);
		}				
	}

}	// end of void	CFontShowWnd::CountDrawInfo()

void CFontShowWnd::CountCCFInfo(CHYGlyph*								pGlyph,
								Gdiplus::GraphicsPath&					GlyphPath, 
								std::vector<PointF>&					vtDrawPts,
								std::vector<int>&						vtDrawPtFlag)
{
	float		fIndent=(m_rcBgRect.Width-pGlyph->advanceWidth*m_fEMScale)/2.0f;
	PointF		pfAdapter;
	size_t		szCntunNum = pGlyph->vtContour.size();		
	for (size_t i=0; i<szCntunNum; i++)
	{
		CHYContour&				hyCntur = pGlyph->vtContour[i];
		size_t					szPtNum = hyCntur.vtHYPoints.size();

		for(size_t j=0; j<szPtNum; j++)
		{
			// 转换到windows坐标系左上角
			CHYPoint& htPt = hyCntur.vtHYPoints[j];
			
			pfAdapter.X	= htPt.x*m_fEMScale+fIndent;
			pfAdapter.Y	= m_rcBgRect.Height-(htPt.y*m_fEMScale+m_fWndBaseLine);
			vtDrawPts.push_back(pfAdapter);
			vtDrawPtFlag.push_back(htPt.flag);			
		}
	}

	// 路径
	int iPtIndex = 0;
	for (size_t i=0; i<szCntunNum; i++)
	{
		CHYContour&				hyCntur = pGlyph->vtContour[i];
		size_t					szPtNum = hyCntur.vtHYPoints.size();
		Gdiplus::GraphicsPath	cnturPath;
		std::vector<PointF>		vtTmpPT;
		std::vector<int>		vtTmpFlag;

		for(size_t j=0; j<szPtNum; j++)
		{	
			vtTmpPT.push_back(vtDrawPts[iPtIndex]);
			vtTmpFlag.push_back(vtDrawPtFlag[iPtIndex]);

			iPtIndex++;
		}

		MakeConturPath(cnturPath,pGlyph,vtTmpPT,vtTmpFlag);
		GlyphPath.AddPath(&cnturPath,FALSE);
	}

}	// end of void CFontShowWnd::CountCCFInfo()

void CFontShowWnd::CountTTFInfo(CHYGlyph*								pGlyph,
								Gdiplus::GraphicsPath&					GlyphPath, 
								std::vector<CHYGlyph>&					vtGryph,
								std::vector<PointF>&					vtDrawPts,
								std::vector<int>&						vtDrawPtFlag,
								std::vector<Gdiplus::GraphicsPath*>&	vtCmpsPath,
								std::vector<int>&						vtCmpsIndex
								)
{
	float		fIndent=(m_rcBgRect.Width-pGlyph->advanceWidth*m_fEMScale)/2.0f;

	// 点
	if (pGlyph->sContourNums>0)
	{
		PointF		pfAdapter;
		size_t		szCntunNum = pGlyph->vtContour.size();		
		for (size_t i=0; i<szCntunNum; i++)	{
			CHYContour&				hyCntur = pGlyph->vtContour[i];
			size_t					szPtNum = hyCntur.vtHYPoints.size();

			for(size_t j=0; j<szPtNum; j++)	{
				// 转换到windows坐标系左上角
				CHYPoint& htPt = hyCntur.vtHYPoints[j];

				pfAdapter.X	= htPt.x*m_fEMScale+fIndent;
				pfAdapter.Y	= m_rcBgRect.Height-(htPt.y*m_fEMScale+m_fWndBaseLine);
				vtDrawPts.push_back(pfAdapter);
				vtDrawPtFlag.push_back(htPt.flag);		
			}
		}

		// 路径
		int iPtIndex = 0;
		for (size_t i=0; i<szCntunNum; i++)	{
			CHYContour&				hyCntur = pGlyph->vtContour[i];
			size_t					szPtNum = hyCntur.vtHYPoints.size();
			Gdiplus::GraphicsPath	cnturPath;
			std::vector<PointF>		vtTmpPT;
			std::vector<int>		vtTmpFlag;

			for(size_t j=0; j<szPtNum; j++)	{	
				vtTmpPT.push_back(vtDrawPts[iPtIndex]);
				vtTmpFlag.push_back(vtDrawPtFlag[iPtIndex]);

				iPtIndex++;
			}

			if (vtTmpFlag.size()>0)	{
				if (vtTmpFlag[0] == POINT_FLG_CONTROL)
					TrimPointlist(vtTmpPT, vtTmpFlag);
			}

			MakeConturPath(cnturPath,pGlyph, vtTmpPT,vtTmpFlag);
			GlyphPath.AddPath(&cnturPath,FALSE);
		}
	}

	if (pGlyph->sContourNums<0)	{
		// 针对组合轮廓转换
		SHORT sValue = 0;
		float fXScale = 1.0f, fYScale = 1.0f, fValue = 0.0f, fFraction = 0.0f;			
		int iCmpnnts = pGlyph->vtComponents.size();

		for (int j=0; j<iCmpnnts; j++){
			fXScale = 1.0f; 
			fYScale = 1.0f;				
			CHYCmpst& hyCmpst = pGlyph->vtComponents[j];			
			Gdiplus::GraphicsPath*	pGlyphPath = new Gdiplus::GraphicsPath();

			USHORT sGlyphIndex  = hyCmpst.glyphIndex;
			if (hyCmpst.flags&GLYF_CMPST_ARGS_ARE_XY_VALUES){
				if (hyCmpst.flags&GLYF_CMPST_WE_HAVE_A_SCALE){
					fYScale = fXScale = HY_F2DOT14_to_float(hyCmpst.scale);
				}

				if (hyCmpst.flags&GLYF_CMPST_WE_HAVE_AN_X_AND_Y_SCALE){					
					fXScale = HY_F2DOT14_to_float(hyCmpst.vctr.xscale);
					fYScale = HY_F2DOT14_to_float(hyCmpst.vctr.yscale);
				}

				// 计算路径
				PointF		pfAdapter;
				if (sGlyphIndex>vtGryph.size()) continue;
				if (sGlyphIndex<0)	continue;

				CHYGlyph&	cmpGlyph = vtGryph[sGlyphIndex];					

				size_t		szCntunNum = cmpGlyph.vtContour.size();						
				for (size_t i=0; i<szCntunNum; i++){
					CHYContour&				hyCntur = cmpGlyph.vtContour[i];
					size_t					szPtNum = hyCntur.vtHYPoints.size();
					std::vector<PointF>		vtTmpPT;
					std::vector<int>		vtTmpFlag;

					Gdiplus::GraphicsPath	cnturPath;
					for(size_t x=0; x<szPtNum; x++)	{							
						CHYPoint& htPt = hyCntur.vtHYPoints[x];

						if (hyCmpst.flags&GLYF_CMPST_ARGS_ARE_XY_VALUES){							
							pfAdapter.X = htPt.x*fXScale;
							pfAdapter.Y = htPt.y*fYScale;
							
							// 后偏移
							pfAdapter.X = (float)pfAdapter.X+hyCmpst.args[0];
							pfAdapter.Y = (float)pfAdapter.Y+hyCmpst.args[1];														
						}
						else {
							// 先缩放
							pfAdapter.X = htPt.x*fXScale;
							pfAdapter.Y = htPt.y*fYScale;

							// 后偏移
							pfAdapter.X = (float)htPt.x+hyCmpst.args[0];
							pfAdapter.Y = (float)htPt.y+hyCmpst.args[1];
						}

						// 适配到windows坐标系左上角
						PointF wndPt;
						wndPt.X	= pfAdapter.X*m_fEMScale+fIndent;
						wndPt.Y	= m_rcBgRect.Height-(pfAdapter.Y*m_fEMScale+m_fWndBaseLine);

						vtTmpPT.push_back(wndPt);
						vtTmpFlag.push_back(htPt.flag);
					}

					if (vtTmpFlag.size()>0)
					{
						if (vtTmpFlag[0] == POINT_FLG_CONTROL)
							TrimPointlist(vtTmpPT, vtTmpFlag);

						MakeConturPath(cnturPath,pGlyph,vtTmpPT,vtTmpFlag);
						pGlyphPath->AddPath(&cnturPath,FALSE);
					}
				}
				vtCmpsPath.push_back(pGlyphPath);
				vtCmpsIndex.push_back(sGlyphIndex);
			}				
		}		
	}	

}	// end of void CFontShowWnd::CountTTFInfo()

void CFontShowWnd::TrimPointlist(std::vector<PointF>& vtPoints, std::vector<int>& vtPointsFlg)
{
	std::vector<PointF>		vtTmpPT = vtPoints;
	std::vector<int>		vtTmpFlag = vtPointsFlg;

	vtPoints.clear();
	vtPointsFlg.clear();
	
	size_t vtSize = vtTmpPT.size();
	size_t vtTmpIndex = -1;
	for (size_t i=0; i<vtSize; i++)
	{
		if (vtTmpFlag[i] == POINT_FLG_ANCHOR)
		{
			vtTmpIndex = i;
			break;
		}		
	}
	
	if (vtTmpIndex!=-1)
	{
		for (size_t i=vtTmpIndex; i<vtSize; i++)
		{
			vtPoints.push_back(vtTmpPT[i]);
			vtPointsFlg.push_back(vtTmpFlag[i]);
		}

		for (size_t i=0; i<vtTmpIndex; i++)
		{
			vtPoints.push_back(vtTmpPT[i]);
			vtPointsFlg.push_back(vtTmpFlag[i]);
		}
	}
	else // 如果整个轮廓没有ONLine点，将起点设为Online点
	{
		PointF&		Pt1 = vtTmpPT[0];
		PointF&		Pt2 = vtTmpPT[vtSize-1];		
		PointF		PtStart;
		PtStart.X = Pt1.X+((Pt2.X-Pt1.X)/2);
		PtStart.Y = Pt1.Y+((Pt2.Y-Pt1.Y)/2);	

		vtPoints.push_back(PtStart);
		vtPointsFlg.push_back(POINT_FLG_ANCHOR);

		for (size_t i=0; i<vtSize; i++)
		{			
			vtPoints.push_back(vtTmpPT[i]);			 
			vtPointsFlg.push_back(vtTmpFlag[i]);
		}		
	}	

}	// end of void CFontShowWnd::TrimPointlist()

void CFontShowWnd::MakeConturPath(GraphicsPath& grphPath,CHYGlyph* pGlyph,std::vector<PointF>& vtPoints, std::vector<int>& vtPointsFlg)
{
	size_t  sPtNum=vtPoints.size();
	PointF f1,f2;
	size_t i=0;
	if (pGlyph->fontFlag == FONTTYPE_TTF)
	{		
		while(i<sPtNum)
		{
			if (i==0)
			{
				f1 = vtPoints[i];
				i++;
			}
			else 
			{
				if (vtPointsFlg[i]==POINT_FLG_ANCHOR)
				{
					grphPath.AddLine(f1,vtPoints[i]);
					f1 = vtPoints[i];
					i++;
					
				}
				else if (vtPointsFlg[i]==POINT_FLG_CONTROL)
				{
					if (i+1<sPtNum)
					{
						if(vtPointsFlg[i+1]==POINT_FLG_ANCHOR)
						{
							CountBezierPath(f1, vtPoints[i], vtPoints[i+1], grphPath);

							f1 = vtPoints[i+1];
							i+=2;
						}
						else if(vtPointsFlg[i+1]==POINT_FLG_CONTROL)
						{
							f2.X = vtPoints[i].X+(vtPoints[i+1].X - vtPoints[i].X)/2.0f;
							f2.Y = vtPoints[i].Y+(vtPoints[i+1].Y - vtPoints[i].Y)/2.0f;

							CountBezierPath(f1, vtPoints[i], f2, grphPath);
							f1=f2;
							i++;
						}
					}
					else if (i+1==sPtNum)
					{
						CountBezierPath(f1, vtPoints[i], vtPoints[0], grphPath);
						break;
					}
				}
			}		
		}
	}	

	if (pGlyph->fontFlag == FONTTYPE_OTF)
	{
		while(i<sPtNum)
		{
			if (i==0)
			{
				f1 = vtPoints[i];
				i++;
			}
			else
			{
				if (vtPointsFlg[i]==POINT_FLG_ANCHOR)
				{
					grphPath.AddLine(f1,vtPoints[i]);
					f1 = vtPoints[i];
					i++;
				}
				else if (vtPointsFlg[i]==POINT_FLG_CONTROL)
				{
					if (i+1==sPtNum)
					{
						grphPath.Reset();
						break;
					}

					if (i+2<sPtNum)
					{
						grphPath.AddBezier(f1,vtPoints[i],vtPoints[i+1],vtPoints[i+2]);
						f1 = vtPoints[i+2];
						i+=3;
					}
					else if(i+2==sPtNum) 
					{
						grphPath.AddBezier(f1,vtPoints[i],vtPoints[i+1],vtPoints[0]);
						break;
					}					
				}
			}
		}
	}

	grphPath.CloseFigure();

}	// end of void CFontShowWnd::MakeConturPath();

void CFontShowWnd::CountBezierPath(PointF& pt0, PointF& pt1, PointF& pt2, GraphicsPath& path)
{
	int		iCurvePSize = 10;
	PointF	Curve[10];

	CountBezierCurvePos(pt0, pt1, pt2,Curve,iCurvePSize);

	path.AddCurve(Curve, iCurvePSize);

}	// end of void CFontShowWnd::CountBezierPath()

void  CFontShowWnd::CountBezierCurvePos(PointF& inP0, PointF& inP1, PointF& inP2, PointF* otP, int& iioPtNum)
{
	float dt = 0.0; 
	float X = 0.0f, Y = 0.0f;
	int Z = 0;
	for ( int i = 0; i < iioPtNum ; i++ )
	{	
		dt =  i / (float)(iioPtNum - 1);

		if (i == 0 )
		{
			otP[i].X = (float)inP0.X * ( 1.0f - dt ) * ( 1.0f - dt ) + 2 * dt * ( 1.0f - dt) * (float)inP1.X  + dt * dt * (float)inP2.X;	
			otP[i].Y = (float)inP0.Y * ( 1.0f - dt ) * ( 1.0f - dt ) + 2 * dt * ( 1.0f - dt) * (float)inP1.Y  + dt * dt * (float)inP2.Y;				
		}
		else
		{
			X = (float)inP0.X * ( 1.0f - dt ) * ( 1.0f - dt ) + 2 * dt * ( 1.0f - dt) * (float)inP1.X  + dt * dt * (float)inP2.X;
			Y =(float) inP0.Y * ( 1.0f - dt ) * ( 1.0f - dt ) + 2 * dt * ( 1.0f - dt) * (float)inP1.Y  + dt * dt *(float) inP2.Y;

			if ((otP[i - 1].X == X) && (otP[i - 1].Y == Y) ) 
			{
				continue;
			}
			else 
			{
				otP[Z].X = X;
				otP[Z].Y = Y;
			}
		}
		Z++;
	}		

	iioPtNum = Z;

}	// end of void  CFontShowWnd::CountBezierCurvePos()

void CFontShowWnd::DrawReferLine(Graphics* pbmpGraphics)
{	
	DrawCrossLine(pbmpGraphics);
	DrawBaseLine(pbmpGraphics);
	DrawSideLine(pbmpGraphics);	

}	// end of void CFontShowWnd::DrawReferLine()

void CFontShowWnd::DrawCrossLine(Graphics* pbmpGraphics)
{		
	if (pbmpGraphics)
	{		
		Pen pen(Color(100,125,125,125), 1.0f);
		pen.SetDashStyle(DashStyleDashDot);		

		PointF ptBKgndCenter;
		ptBKgndCenter.X = m_rcBgRect.Width/2.0f;
		ptBKgndCenter.Y = m_rcBgRect.Height/2.0f;	

		float x1 = (float)m_rcBgRect.X;
		float y1 = (float)ptBKgndCenter.Y;
		float x2 = (float)m_rcBgRect.X+m_rcBgRect.Width;
		float y2 = (float)ptBKgndCenter.Y;		
		pbmpGraphics->DrawLine(&pen, x1,y1,x2,y2);

		x1 = ptBKgndCenter.X;
		y1 = (float)m_rcBgRect.Y;
		x2 = ptBKgndCenter.X;
		y2 = (float)(m_rcBgRect.Y+m_rcBgRect.Height);
		pbmpGraphics->DrawLine(&pen,x1,y1,x2,y2);		
	}	

}	// end of void CFontShowWnd::DrawCrossLine()

BOOL CFontShowWnd::DrawBaseLine(Graphics* pbmpGraphics)
{	
	if (pbmpGraphics)
	{
		Pen pen(Color(255,0,0,0), 1);
		pen.SetDashStyle(DashStyleDashDot);

		Gdiplus::PointF	p1, p2;
		p1.X = m_rcBgRect.X;
		p1.Y = m_rcBgRect.Height-m_fWndBaseLine;

		p2.X = m_rcBgRect.X+m_rcBgRect.Width;
		p2.Y = m_rcBgRect.Height-m_fWndBaseLine;

		pbmpGraphics->DrawLine(&pen, p1, p2);
	}	
	return TRUE;

}	// end of BOOL CFontShowWnd::OnDrawBaseLine()

BOOL CFontShowWnd::DrawSideLine(Graphics* pbmpGraphics)
{	
	if (pbmpGraphics)
	{
		Pen pen(Color(255,0,0,0), 1);
		pen.SetDashStyle(DashStyleDashDot);	

		Gdiplus::PointF	Lp1, Lp2;
		Lp1.X = m_fLeftSide;
		Lp1.Y = 0;

		Lp2.X = m_fLeftSide;
		Lp2.Y = m_rcBgRect.Height;
		pbmpGraphics->DrawLine(&pen, Lp1, Lp2);

		Gdiplus::PointF	Rp1, Rp2;
		Rp1.X = m_fRightSide;
		Rp1.Y = 0;

		Rp2.X = m_fRightSide;
		Rp2.Y = m_rcBgRect.Height;
		pbmpGraphics->DrawLine(&pen, Rp1, Rp2);	
	}

	return TRUE;

}	// end of BOOL CFontShowWnd::OnDrawBaseLine()

BOOL CFontShowWnd::DrawPoint(Graphics*				pbmpGraphics,
							 Color&					clrOnline,
							 Color&					clrOffline,
							 CHYGlyph*				pGlyph,
							 std::vector<PointF>	&vtDrawPts,
							 std::vector<int>		&vtDrawPtFlag
							 )
{	
	if (pbmpGraphics)
	{
		if(pGlyph->fontFlag == FONTTYPE_OTF)
		{
			DrawCFFPoint(pbmpGraphics,clrOnline,clrOffline,pGlyph,vtDrawPts,vtDrawPtFlag);
		}
		
		if (pGlyph->fontFlag == FONTTYPE_TTF)
		{
			DrawTTFPoint(pbmpGraphics,clrOnline,clrOffline,pGlyph,vtDrawPts,vtDrawPtFlag);
		}
	}

	return TRUE;

}	// end of BOOL CFontShowWnd::DrawPoint()

BOOL CFontShowWnd::DrawTTFPoint(Graphics*				pbmpGraphics,
								Color&					clrOnline,
								Color&					clrOffline,
								CHYGlyph*				pGlyph,
								std::vector<PointF>		&vtDrawPts,
								std::vector<int>		&vtDrawPtFlag
								)
{
	if(pGlyph)
	{
		Pen			penOnLinePt(clrOnline,2.0);
		Pen			penOffLinePt(clrOffline,2.0);

		RectF		rcPt;

		if (pGlyph->sContourNums>0){
			size_t ptNums = vtDrawPts.size();
			for (size_t i=0; i<ptNums; i++){
				rcPt.X = vtDrawPts[i].X-2.0f>0 ? vtDrawPts[i].X-2.0f:0;
				rcPt.Y = vtDrawPts[i].Y-2.0f>0 ? vtDrawPts[i].Y-2.0f:0;
				rcPt.Width = 4.0f;
				rcPt.Height =4.0f;

				if (vtDrawPtFlag[i]==POINT_FLG_ANCHOR){					
					pbmpGraphics->DrawRectangle(&penOnLinePt, rcPt);
				}

				if (vtDrawPtFlag[i]==POINT_FLG_CONTROL){
					pbmpGraphics->DrawLine(&penOffLinePt,rcPt.X,rcPt.Y,rcPt.X+rcPt.Width,rcPt.Y+rcPt.Height);
					pbmpGraphics->DrawLine(&penOffLinePt,rcPt.X,rcPt.Y+rcPt.Height,rcPt.X+rcPt.Width,rcPt.Y);
				}	
			}
		}

		if (pGlyph->sContourNums<0){
			Pen			penBound(Color(100,125,125,125),1.0f);

			for (size_t i=0; i<m_vtCmpsPath.size();i++){
				RectF rcBountd;
				m_vtCmpsPath[i]->GetBounds(&rcBountd);
				pbmpGraphics->DrawRectangle(&penBound,rcBountd);
			}
		}
	}

	return TRUE;

}	// end of BOOL CFontShowWnd::DrawTTFPoint()

BOOL CFontShowWnd::DrawCFFPoint(Graphics*				pbmpGraphics,
								Color&					clrOnline, 
								Color&					clrOffline,
								CHYGlyph*				pGlyph,
								std::vector<PointF>		&vtDrawPts,
								std::vector<int>		&vtDrawPtFlag)
{
	if(pGlyph)
	{
		Pen			penOnLinePt(clrOnline,2.0);
		Pen			penOffLinePt(clrOffline,2.0);
		RectF		rcPt;		
		
		size_t ptNums = vtDrawPts.size();
		for (size_t i=0; i<ptNums; i++)
		{
			rcPt.X = vtDrawPts[i].X-2.0f>0 ? vtDrawPts[i].X-2.0f:0;
			rcPt.Y = vtDrawPts[i].Y-2.0f>0 ? vtDrawPts[i].Y-2.0f:0;
			rcPt.Width = 4.0f;
			rcPt.Height =4.0f;

			if (vtDrawPtFlag[i]==POINT_FLG_ANCHOR)
			{					
				pbmpGraphics->DrawRectangle(&penOnLinePt, rcPt);
			}

			if (vtDrawPtFlag[i]==POINT_FLG_CONTROL)
			{
				pbmpGraphics->DrawLine(&penOffLinePt,rcPt.X,rcPt.Y,rcPt.X+rcPt.Width,rcPt.Y+rcPt.Height);
				pbmpGraphics->DrawLine(&penOffLinePt,rcPt.X,rcPt.Y+rcPt.Height,rcPt.X+rcPt.Width,rcPt.Y);
			}	
		}		
	}

	return TRUE;

}	// end of BOOL CFontShowWnd::DrawCFFPoint()

BOOL CFontShowWnd::DrawPointNo(Graphics*							pbmpGraphics, 
							   Color&								clrPtNo,
							   CHYGlyph*							pGlyph,
							   std::vector<PointF>					&vtDrawPts,							   
							   std::vector<Gdiplus::GraphicsPath*>	&vtCmpsPath,
							   std::vector<int>						&vtCmpsIndex
							   )
{
	
	if(pbmpGraphics)
	{
		if (pGlyph->fontFlag == FONTTYPE_OTF)
		{
			DrawCFFPointNo(pbmpGraphics,clrPtNo,pGlyph,vtDrawPts);
		}	

		if (pGlyph->fontFlag == FONTTYPE_TTF)
		{
			DrawTTFPointNo(pbmpGraphics,clrPtNo,pGlyph,vtDrawPts,vtCmpsPath,vtCmpsIndex);
		}
	}

	return TRUE;

}	// end of BOOL CFontShowWnd::DrawPointNo()

BOOL CFontShowWnd::DrawTTFPointNo(Graphics*								pbmpGraphics,
								  Color&								clrPtNo,
								  CHYGlyph*								pGlyph,
								  std::vector<PointF>					&vtDrawPts,								  
								  std::vector<Gdiplus::GraphicsPath*>   &vtCmpsPath,
								  std::vector<int>						&vtCmpsIndex
								  )
{
	CStringW		strCodeNumder;		
	SolidBrush		sBrush(clrPtNo);
	StringFormat	tpStringformat;
	tpStringformat.SetFormatFlags(StringFormatFlagsNoClip);
	tpStringformat.SetLineAlignment(StringAlignmentCenter);
	tpStringformat.SetAlignment(StringAlignmentCenter);
	CStringW		StrFontName = L"Arial";
	Gdiplus::Font	fFont(StrFontName,10.0f, FontStyleRegular, UnitPoint,NULL);

	if(pbmpGraphics)
	{
		if (pGlyph)
		{
			float			fIndent=(m_rcBgRect.Width-pGlyph->advanceWidth*m_fEMScale)/2.0f;
			PointF			pfAdapter;
			if (pGlyph->sContourNums>0)
			{			
				size_t		ptNums = vtDrawPts.size();
				int			iPtIndex = 0;
				for (size_t i=0; i<ptNums; i++){						
					pfAdapter = vtDrawPts[i];
					pfAdapter.X += 5;
					pfAdapter.Y -= 5;

					strCodeNumder.Format(L"%d", iPtIndex);
					pbmpGraphics->DrawString(strCodeNumder, strCodeNumder.GetLength(), &fFont, pfAdapter, &tpStringformat, &sBrush);
					iPtIndex++;			
				}
			}

			if (pGlyph->sContourNums<0)
			{
				for (size_t i=0; i<vtCmpsPath.size();i++){
					RectF rcBountd;
					vtCmpsPath[i]->GetBounds(&rcBountd);
					pfAdapter.X = rcBountd.X+20;
					pfAdapter.Y = rcBountd.Y+10;

					if (vtCmpsIndex.size()==0) 
						continue;

					strCodeNumder.Format(L"gid %d", vtCmpsIndex[i]);
					pbmpGraphics->DrawString(strCodeNumder, strCodeNumder.GetLength(), &fFont, pfAdapter, &tpStringformat, &sBrush);
				}
			}
		}	
	}

	return TRUE;

}	// end of BOOL CFontShowWnd::DrawTTFPointNo()

void CFontShowWnd::GrawGlyph()
{
	if (m_pbmpGraphics)
	{
		// 将整体背景填充为白色
		m_pbmpGraphics->Clear(m_clrBKRnd);

		if (m_tagDrawFnotParam.iReferLine)
		{
			DrawReferLine(m_pbmpGraphics);
		}
		if (m_tagDrawFnotParam.iFill)
		{
			Color  clrFll(255,0,0,0);
			FillPath(m_pbmpGraphics,clrFll,m_pGlyph,m_GlyphPath,m_vtCmpsPath);
		}
		if (m_tagDrawFnotParam.iLine)
		{
			Color  clrPth(255,0,0,0);
			DrawPath(m_pbmpGraphics,clrPth,m_pGlyph,m_GlyphPath,m_vtCmpsPath);
		}
		if (m_tagDrawFnotParam.iPoint)
		{
			Color  clrOnline(255,0,0,255);
			Color  clrOffline(255,155,78,0);
			DrawPoint(m_pbmpGraphics,clrOnline,clrOffline,m_pGlyph,m_vtDrawPts,m_vtDrawPtFlag);
		}
		if (m_tagDrawFnotParam.iPointNo)
		{
			Color  clrPtNo(255,0,0,0);
			DrawPointNo(m_pbmpGraphics,clrPtNo,m_pGlyph,m_vtDrawPts,m_vtCmpsPath,m_vtCmpsIndex);
		}
	}

}	// end of void CFontShowWnd::GrawGlyph()

void  CFontShowWnd::DrawPointCoordinate(int iPtIndex,Graphics* pbmpGraphics, Color& clrPtNo, CPoint ptMu)
{
	if (iPtIndex == -1) return;
	if (pbmpGraphics==NULL) return;

	CHYPoint& ptOrgin = m_vtOrginPts[iPtIndex];
	CStringW		strCodeNumder;
	strCodeNumder.Format(L"%d,%d",ptOrgin.x,ptOrgin.y);

	SolidBrush		sBrush(clrPtNo);
	StringFormat	tpStringformat;
	tpStringformat.SetFormatFlags(StringFormatFlagsNoClip);	
	tpStringformat.SetAlignment(StringAlignmentNear);

	CStringW		StrFontName = L"Arial";
	Gdiplus::Font	fFont(StrFontName,10.0f, FontStyleRegular, UnitPoint,NULL);
	Gdiplus::PointF ptCoordinate;
	ptCoordinate.X = ptMu.x+10;		
	ptCoordinate.Y = ptMu.y-5;
	// 绘制坐标字符串
	RectF			rcString;
	SolidBrush		TxtBrush(Color::WhiteSmoke);
	Status sttus = pbmpGraphics->MeasureString(strCodeNumder,wcslen(strCodeNumder), &fFont, ptCoordinate,&tpStringformat,&rcString);	
	pbmpGraphics->FillRectangle(&TxtBrush,rcString);
	pbmpGraphics->DrawString(strCodeNumder, strCodeNumder.GetLength(), &fFont, ptCoordinate, &tpStringformat, &sBrush);	

	// 绘制焦点矩形
	PointF& ptDraw = m_vtDrawPts[iPtIndex];
	RectF			rcFocus;
	rcFocus.X		= ptDraw.X-3;
	rcFocus.Y		= ptDraw.Y-3;
	rcFocus.Width	= 6;
	rcFocus.Height	= 6;

	Pen				FocusPen(Color::Red,1.0f);
	pbmpGraphics->DrawRectangle(&FocusPen,rcFocus);

}	// end of BOOL CFontShowWnd::DrawPointCoordinate()

BOOL CFontShowWnd::DrawCFFPointNo(Graphics*								pbmpGraphics,
								  Color&								clrPtNo,
								  CHYGlyph*								pGlyph,
								  std::vector<PointF>					&vtDrawPts	
								  )
{
	CStringW		strCodeNumder;		
	SolidBrush		sBrush(clrPtNo);
	StringFormat	tpStringformat;
	tpStringformat.SetFormatFlags(StringFormatFlagsNoClip);
	tpStringformat.SetLineAlignment(StringAlignmentCenter);
	tpStringformat.SetAlignment(StringAlignmentCenter);
	CStringW		StrFontName = L"Times New Roman";
	Gdiplus::Font	fFont(StrFontName,10.0f, FontStyleRegular, UnitPoint,NULL);

	if(pbmpGraphics)
	{
		if (pGlyph)
		{
			float			fIndent=(m_rcBgRect.Width-pGlyph->advanceWidth*m_fEMScale)/2.0f;
			PointF			pfAdapter;		
						
			size_t		ptNums = vtDrawPts.size();
			int			iPtIndex = 0;
			for (size_t i=0; i<ptNums; i++)
			{		
				// 转换到windows坐标系左上角
				pfAdapter = vtDrawPts[i];
				pfAdapter.X += 5;
				pfAdapter.Y -= 5;

				strCodeNumder.Format(L"%d", iPtIndex);		
				pbmpGraphics->DrawString(strCodeNumder, strCodeNumder.GetLength(), &fFont, pfAdapter, &tpStringformat, &sBrush);
				iPtIndex++;			
			}		
		}	
	}

	return TRUE;

}	// end of BOOL CFontShowWnd::DrawCFFPointNo()

BOOL CFontShowWnd::DrawPath(Graphics* pbmpGraphics,Color& clrPth, CHYGlyph*	pGlyph,Gdiplus::GraphicsPath& GlyphPath,std::vector<Gdiplus::GraphicsPath*>&  vtCmpsPath)
{
	if (pbmpGraphics)
	{
		if (pGlyph)
		{
			if (pGlyph->fontFlag == FONTTYPE_OTF)
			{
				DrawCFFPath(pbmpGraphics,clrPth,GlyphPath);
			}				

			if (pGlyph->fontFlag == FONTTYPE_TTF)
			{
				DrawTTFPath(pbmpGraphics,clrPth,pGlyph,GlyphPath,vtCmpsPath);
			}
		}		
	}

	return TRUE;

}	// end of BOOL CFontShowWnd::DrawPath()

BOOL CFontShowWnd::DrawTTFPath(Graphics* pbmpGraphics,Color& clrPth, CHYGlyph*	pGlyph,Gdiplus::GraphicsPath& GlyphPath,std::vector<Gdiplus::GraphicsPath*>&  vtCmpsPath)
{
	Pen		pnBk(clrPth,1);
	if (pGlyph->sContourNums>0){
		pbmpGraphics->DrawPath(&pnBk,&GlyphPath);
	}

	if (pGlyph->sContourNums<0)	{
		for (size_t i=0; i<vtCmpsPath.size();i++)		{
			pbmpGraphics->DrawPath(&pnBk,vtCmpsPath[i]);
		}
	}

	return TRUE;

}	// end of BOOL CFontShowWnd::DrawTTFPath()

BOOL CFontShowWnd::DrawCFFPath(Graphics* pbmpGraphics,Color& clrPth,Gdiplus::GraphicsPath& GlyphPath)
{
	Pen		pnBk(clrPth,1);
	pbmpGraphics->DrawPath(&pnBk,&GlyphPath);

	return TRUE;

}	// end of BOOL CFontShowWnd::DrawCFFPath()

void	CFontShowWnd::PainGlyph(CDC& dc)
{
	if (m_pbmpGraphics)
	{
		if (m_pbmp)
		{	
			Gdiplus::Graphics graphics(dc.GetSafeHdc());		

			// 将整体背景填充为白色		
			if (m_vtDrawPts.size()==0&&m_vtCmpsIndex.size()==0)
			{
				SolidBrush 		sbrus(m_clrBKRnd);
				graphics.FillRectangle(&sbrus,m_rcBgRect);
			}	

			/*Important! Create a CacheBitmap object for quick drawing*/
			Gdiplus::CachedBitmap cachedBmp(m_pbmp,&graphics);
			graphics.DrawCachedBitmap(&cachedBmp,0,0); 
		}
	}

}	// end of BOOL	CFontShowWnd::PainGlyph();

BOOL CFontShowWnd::FillPath(Graphics* pbmpGraphics,Color& clrFll, CHYGlyph*	pGlyph,Gdiplus::GraphicsPath& GlyphPath,std::vector<Gdiplus::GraphicsPath*>&  vtCmpsPath)
{
	if (pbmpGraphics)
	{	
		if (pGlyph)
		{
			if (pGlyph->fontFlag == FONTTYPE_OTF)
			{
				FillCFFPath(pbmpGraphics,clrFll,GlyphPath);
			}

			if (pGlyph->fontFlag == FONTTYPE_TTF)
			{
				FillTTFPath(pbmpGraphics,clrFll,pGlyph,GlyphPath,vtCmpsPath);
			}
		}		
	}

	return TRUE;

}	// end of BOOL CFontShowWnd::FillPath()

BOOL CFontShowWnd::FillTTFPath(Graphics* pbmpGraphics,Color& clrFll, CHYGlyph*	pGlyph,Gdiplus::GraphicsPath& GlyphPath,std::vector<Gdiplus::GraphicsPath*>&  vtCmpsPath)
{
	SolidBrush			SBrsh(clrFll);
	if (pGlyph->sContourNums>0)	{			
		pbmpGraphics->FillPath(&SBrsh,&GlyphPath);
	}

	if (pGlyph->sContourNums<0)	{
		for (size_t i=0; i<vtCmpsPath.size();i++){
			pbmpGraphics->FillPath(&SBrsh,vtCmpsPath[i]);
		}
	}

	return TRUE;

}	// end of BOOL CFontShowWnd::FillTTFPath()

BOOL CFontShowWnd::FillCFFPath(Graphics* pbmpGraphics,Color& clrFll,Gdiplus::GraphicsPath& GlyphPath)
{
	SolidBrush			SBrsh(clrFll);				
	pbmpGraphics->FillPath(&SBrsh,&GlyphPath);

	return TRUE;

}	// end of BOOL CFontShowWnd::FillCFFPath()

int CFontShowWnd::HitPoints(CPoint point)
{	
	Gdiplus::Rect rcHit;
	size_t stPts = m_vtDrawPts.size();
	for(size_t i=0; i<stPts; i++)
	{
		rcHit.X = m_vtDrawPts[i].X-4;
		rcHit.Y = m_vtDrawPts[i].Y-4;
		rcHit.Width = 8;
		rcHit.Height= 8;

		if (rcHit.Contains(point.x, point.y))
		{
			return static_cast<int>(i);
		}
	}

	return -1;

}	// end of int CFontShowWnd::HitPoints()

void CFontShowWnd::OnLButtonDown(UINT nFlags, CPoint point)
{	
	int iIndex = HitPoints(point);
	if (iIndex!=-1)
	{
		GrawGlyph();
		Color  clrPt(255,0,0,0);
		DrawPointCoordinate(iIndex,m_pbmpGraphics,clrPt,point);
		Invalidate();
	}

	CWnd::OnLButtonDown(nFlags, point);

}	//end of void CFontShowWnd::OnLButtonDown()

void CFontShowWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CWnd* pParentWnd = GetParent();
	if (pParentWnd)
	{
		if (pParentWnd->m_hWnd)
		{		
			pParentWnd->SendMessage(MSG_POPVIEW_WND, MAKEWPARAM(m_iWndID,0), 0);
		}
	}

	CWnd::OnLButtonDblClk(nFlags, point);

}	// end of void CFontShowWnd::OnLButtonDblClk()
