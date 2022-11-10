#pragma once


#include "HYDataFactory.h"
#include "HY_Global.h"
class AFX_EXT_CLASS CTTFFilter
{
public:
	CTTFFilter(void);
	~CTTFFilter(void);

	BOOL	HYTOHY(HY_POINT* pSrcHYP, int iSrctPointsSize, HY_POINT* pDstHYP, int& iDsttPointsSize, HEAD_TABLE* pSrcTTFHead, TTF_CONVTER_PARAM* pTTFParam);

	void	scaleNarrowHYZS(HY_POINT* pHYP, int scale, int em, int ihy);
	void	BoldHorHWZS(HY_POINT* pHYP, int scale, int em, int ihy);

	int		convertHWKT(HY_POINT* pDstHYP, HY_POINT* pSrcHYP, int em, int ihy);
	int		convertHYHT(HY_POINT* pDstHYP, HY_POINT* pSrcHYP, int em, int ihy);
	int		convertHWZS(HY_POINT* pDstHYP, HY_POINT* pSrcHYP, int em, int ihy);
	int		convertFZYT(HY_POINT* pDstHYP, HY_POINT* pSrcHYP, int em, int ihy);
	int		convertFZYH(HY_POINT* pDstHYP, HY_POINT* pSrcHYP, int em, int ihy);

	static void	moveStartPoint2Top(HY_POINT* pDstHYP, HY_POINT* pSrcHYP, int ihy);
	static void	moveStartPoint2Bottom(HY_POINT* pDstHYP, HY_POINT* pSrcHYP, int ihy);
	void	moveStartPointContourJ(HY_POINT* pDstHYP, HY_POINT* pSrcHYP, int j, int ihy);
	int		buildUpLeftCorner(HY_POINT*  pDstHYP, HY_POINT*  pSrcHYP, int ilup, int ildown, int ihy);
	int		buildUpRightCorner(HY_POINT*  pDstHYP, HY_POINT*  pSrcHYP, int irup, int irdown, int ihy);
	int		buildDownLeftCorner(HY_POINT*  pDstHYP, HY_POINT*  pSrcHYP, int ildout, int ildin, int ihy);
	int		buildDownRightCorner(HY_POINT*  pDstHYP, HY_POINT*  pSrcHYP, int irdout, int irdin, int ihy);
	BOOL	isRightDownCorner(HY_POINT* pTmpHYP, int& irdout, int& irdin, int i, int ihy);
	BOOL	isLeftUpCorner(HY_POINT* pTmpHYP, int& iluout, int& iluin, int i, int ihy);
	BOOL	isLeftUpCornerNotVertical(HY_POINT* pTmpHYP, int& iluout, int& iluin, int i, int ihy);
	BOOL	isRightUpCornerCurveDown(HY_POINT* pTmpHYP, int& ilunvout, int& ilunvin, int i, int ihy);
	BOOL	isRightUpCorner(HY_POINT* pTmpHYP, int& iruout, int& ircin, int i, int ihy);
	BOOL	isRightUpCornerNotVertical(HY_POINT* pTmpHYP, int& iruout, int& iruin, int i, int ihy);
	BOOL	isLeftDownCorner(HY_POINT* pTmpHYP, int& ildout, int& ildin, int i, int ihy);
	BOOL	isLeftUpCornerDownCurve(HY_POINT* pTmpHYP, int& iluout, int& iluin, int i, int ihy);
	BOOL	isLeftUpDownCurve(HY_POINT* pTmpHYP, int& iluout, int& iluin, int i, int ihy);
	int		removePieEndMidPoint(HY_POINT* pDstHYP, HY_POINT* pSrcHYP, int ihy);
	int		rebuildPieTopRen(HY_POINT* pDstHYP, HY_POINT* pSrcHYP, int ihy);
	int		rebuildKOUFoot(HY_POINT* pDstHYP, HY_POINT* pSrcHYP, int ihy);
	int		rebuildGOUtoLeft(HY_POINT* pDstHYP, HY_POINT* pSrcHYP, int ihy);
	int		rebuildKOUWithoutFoot(HY_POINT* pDstHYP, HY_POINT* pSrcHYP, int ihy);
	int		rebuildRightUpZhe(HY_POINT* pDstHYP, HY_POINT* pSrcHYP, int ihy);
	void	boldVertical(HY_POINT* pDstHYP, int ihy);
	BOOL	deleteAnchorMid(HY_POINT* pSrcHYP,int i);
	int		getStrokes(HY_POINT*  pDstHYP, HY_POINT*  pSrcHYP, int ihy);
	int		locateOverStroke(HY_POINT* pSrcHYP, int k, int* CS, int ihy);
	int		locateEmbededEnd(HY_POINT* pSrcHYP, int k, int* CS, int& dx, int& dy,int& recCorner, int ihy);

	int		getStrokeFZYH(HY_POINT*  pDstHYP, HY_POINT*  pSrcHYP, int em, int iSrctPointsSize);
	static int		arcsine(int x1, int y1, int x2, int y2);
	BOOL	isEND(HY_POINT*  pSrcHYP,int i,int* CS, int& is, int& ie, int ihy);
	void	getContourSeg(int* CS, int n, int& is, int& ie, int ihy);
	static int		distanceStem(int x0,int y0,int x1,int y1,int a);
	static double	sine(int a);
	int		saperateOutline2Strokes(HY_POINT*  pDstHYP, HY_POINT*  pSrcHYP, int ihy);
	int		removeHorVerPoint(HY_POINT* pSrcHYP, HY_POINT* pDstHYP, int ihy);
	BOOL	isAcuteAngle(int a1,int a2,int a4,int a5);


	//for new algrithm
	void	SetIntersection(HY_POINT* pSrcHYP, int* CS, int ihy);
	int		goIntersection(HY_POINT* pSrcHYP, int k, int* CS, int ihy);
	int		findEmbedEnd(HY_POINT* pSrcHYP, int k, int* CS, int& dx, int& dy, int ihy);
	int		findIntersection(HY_POINT* pSrcHYP, int k, int* CS, int ihy);
	int		getIntersectionPoint(HY_POINT* pDstHYP, HY_POINT* pSrcHYP, int ihy);
	int		getOutStrokes(HY_POINT* pDstHYP, HY_POINT* pSrcHYP, int j, int* CS, int ihy);
	int		getEmbedStrokes(HY_POINT* pDstHYP, HY_POINT* pSrcHYP, int j, int* CS, int ihy);
	void	setDELETEmark(HY_POINT* pSrcHYP,int* CS, int i,int ihy);
	BOOL	isNotNearby(int i, int k, int* CS,int ihy);
	int		getOtherContour(HY_POINT* pDstHYP, HY_POINT* pSrcHYP, int j, int* CS,  int ihy);
	BOOL	isIntersection(HY_POINT* pSrcHYP, int k, int* CS, int ihy);
	int		goOverStem(HY_POINT* pSrcHYP, int k, int* CS, int ihy);
	BOOL	lineSegmentIntersection(int x1,int y1,int x2,int y2,int x3,int y3,int x4,int y4);
	BOOL	intersectionCheck(HY_POINT* pSrcHYP,int i1,int i2,int* CS,int ihy);


};
