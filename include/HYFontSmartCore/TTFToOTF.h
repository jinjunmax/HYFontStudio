#pragma once
#include "OTFToOTFEx1.h"

class AFX_EXT_CLASS CTTFToOTF :	 public COTFToOTFEx1
{
public:
	CTTFToOTF(void);
	~CTTFToOTF(void);

	int		TTFToOTFTest(int i);

	int		TTF2OTFregular(HY_POINT* pDstHYP,HY_POINT* pSrcHYP,int ihy);
	void	TTFAnchor2Curve(HY_POINT* pDstHYP,int ihy);
	int		OTFrebuildHorVerStart(HY_POINT* pDstHYP,int ihy);
	BOOL	onB3(float x0,float y0,float x1,float y1,float x2,float y2,float x3,float y3,float x,float y);
	BOOL	isBezier(HY_POINT* pCubicHYP, int n, float fx1, float fy1, float fx2, float fy2);
	int		locateMidPoint(HY_POINT* pCubicHYP,int n);
	int		OTFrebuildHorVerCorner(HY_POINT* pDstHYP,int ihy);

	void	TTFCurveToAnchorHorVer(HY_POINT* pDstHYP,int ihy);

};
