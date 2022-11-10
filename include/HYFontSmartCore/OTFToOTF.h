#pragma once


class AFX_EXT_CLASS COTFToOTF
{
public:
	COTFToOTF(void);
	virtual ~COTFToOTF(void);


	//for stroke control
	void	adjuestContourWithEndStart(HY_POINT* pSrcHYP,int ihy);
	void	moveStartPointToTopOTF(HY_POINT* pDstHYP, int ihy);
	int		removeFlatCurve(HY_POINT* pDstHYP, HY_POINT* pSrcHYP, int ihy);

	void	movePoint(C_POINT* pDstCP, HY_POINT* pSrcHYP,int ip,int im);
	void	scaleStemWidth(HY_POINT* pDstHYP,C_POINT* pDstCP,int xsalce,int yscale, int ihy);
	void	closeBarControl(C_POINT* pDstCP,HY_POINT* pSrcHYP,int iSrctPointsSize);
	int		aDiff(int a1,int a2);
	int		strokeControlProcess(C_POINT*  pDstCP,HY_POINT* pSrcHYP,int iSrctPointsSize) ;
	BOOL	isStrokeEnd(HY_POINT* pSrcHYP,int i,int is,int ie);
	BOOL	findAnotherEnd(HY_POINT*  pSrcHYP,int& i,int is, int ie);
	void	putAnotherEND(C_POINT* pDstCP, HY_POINT* pSrcHYP,int ip1,int im1);
	void	move3PointAvarage(C_POINT* pDstCP, HY_POINT* pSrcHYP,int i1,int i2,int i3);
	BOOL	segmentInContour(HY_POINT* pSrcHYP, int i1, int i2, int ihy);
	int		removeEmbededSegment(HY_POINT* pDstHYP, int ihy);

	void	strokeControlBufferProcess(C_POINT*  pDstCP,HY_POINT* pSrcHYP,int iSrctPointsSize) ;
	BOOL	findTransferEnd(HY_POINT* pSrcHYP,int ip1,int im1,int ip2,int im2,int& it1,int& it2);
	void	scaleGlyphFaceSize(HY_POINT* pDstHYP,int xscale,int yscale, int em, int ihy);
	int		removeUnsymmetricPoint(HY_POINT* pDstHYP, int ihy);
	BOOL	specialSegmentControlBuffer(C_POINT* pDstCP, HY_POINT* pSrcHYP, int is, int ie);

	int		rebuildShugou(HY_POINT* pDstHYP,int ihy);
	int		removeShortLine(HY_POINT* pDstHYP,HY_POINT* pSrcHYP,int ihy);
	int		rebuildDonqingSansSerif(HY_POINT* pDstHYP,HY_POINT* pSrcHYP,int ihy);
	int		removeHorVerMorePoint(HY_POINT* pDstHYP,int ihy);
	void	makeCubicBezier(HY_POINT* pDstHYP, int is, int ie, float& x1, float& y1, float& x2, float& y2);
	int		rebuildMergeCubic(HY_POINT* pDstHYP, HY_POINT* pSrcHYP, int ihy);
	void	moveStartPointToBottomOTF(HY_POINT* pDstHYP, int ihy);
	int		rebuildVerStart(HY_POINT* pDstHYP,int ihy);
	int		smoothPieWithMoreBezier(HY_POINT* pDstHYP,HY_POINT* pSrcHYP,int ihy);
	double	angle(int a);
	void	rebuildHorTune(HY_POINT* pDstHYP,int ihy);
	int		removePieEndControl(HY_POINT* pDstHYP,int ihy);
	int		removePieEndNarrow(HY_POINT* pDstHYP,int ihy);
	void	closeContuor(HY_POINT* pDstHYP,int em, int ihy);
	int		removeSamePoint(HY_POINT* pDstHYP,int ihy);
	void	moveStartToAnchorCornerOTF(HY_POINT* pDstHYP, int ihy);

	//int		rebuildHorStroke(HY_POINT* pDstHYP,int ihy);
	void	makeHorVerLineWithCubic(HY_POINT* pDstHYP, int ihy);
	int		rebuildEnd(HY_POINT* pDstHYP,int ihy);
	int		rebuildEnd1(HY_POINT* pDstHYP,int ihy);
	int		reformSmoothCorner(HY_POINT* pDstHYP,int ihy);
	int		DistanceOfLineToPoint(int x1, int y1, int x2, int y2, int x, int y);
	int		rebuildEnd2(HY_POINT* pDstHYP,int ihy);
	int		removeUTurnEnd(HY_POINT* pDstHYP,int ihy);
	void	SmoothUTurnEnd(HY_POINT* pSrcHYP,int ihy);
	int		AddExtremePointUTurnEnd(HY_POINT* pDstHYP,HY_POINT* pSrcHYP,int ihy);
	int		DeleteKOUFoot(HY_POINT* pDstHYP,int ihy);

	void	convertDirection(HY_POINT* pDstHYP,int ihy);

	
	//for overlop to outline functions
	int OTFOverlop2Trueoutline(HY_POINT* pDstHYP, int ihy);
BOOL COTFToOTF::LineLineIntersection(HY_POINT* pSrcHYP,int i,int k,float& x,float& y);
BOOL COTFToOTF::LineIntersection(float x11,float y11,float x12,float y12,float x21,float y21,float x22,float y22,float& x,float& y);
BOOL COTFToOTF::LineCurveIntersection(HY_POINT* pSrcHYP,int i,int k,float& x,float& y);
BOOL COTFToOTF::CurveLineIntersection(HY_POINT* pSrcHYP,int i,int k,float& x,float& y);
BOOL COTFToOTF::LineIntersectionCurve(float x10,float y10,float x11,float y11,float x20,float y20,float x21,float y21,float x22,float y22,float x23,float y23,float& x,float& y);
BOOL COTFToOTF::CurveCurveIntersection(HY_POINT* pSrcHYP,int i,int k,float& x,float& y);
BOOL COTFToOTF::llCross(float x11,float y11,float x12,float y12,float x21,float y21,float x22,float y22);
void COTFToOTF::Saperate2Beziers(HY_POINT* pSrcHYP,int i,float x,float y,float& xb1,float& yb1,float& xb2,float& yb2,float& xa1,float& ya1,float& xa2,float& ya2);
void COTFToOTF::getBezierT(HY_POINT* pSrcHYP,int i,float x,float y,float& t);
void COTFToOTF::StartPointInsideNextContoureCheckAndChange(HY_POINT* pDstHYP, int ihy);

int COTFToOTF::TwoContourMerge(HY_POINT* pDstHYP, int ihy);

BOOL COTFToOTF::lcCross(float x10,float y10,float x11,float y11,float x20,float y20,float x21,float y21,float x22,float y22,float x23,float y23);
BOOL COTFToOTF::clCross(float x10,float y10,float x11,float y11,float x12,float y12,float x13,float y13,float x20,float y20,float x21,float y21);
BOOL COTFToOTF::ccCross(float x10,float y10,float x11,float y11,float x12,float y12,float x13,float y13,float x20,float y20,float x21,float y21,float x22,float y22,float x23,float y23);
BOOL COTFToOTF::overlop(HY_POINT* pSrcHYP,int is0,int ie0,int is1,int ie1);
void COTFToOTF::SetReserveMark(HY_POINT* pDstHYP,int is,int ie, int mark);
int COTFToOTF::GetPairContours(HY_POINT* pSrcHYP,HY_POINT* pDstHYP,int& ie0,int& ie1,int ihy);


void COTFToOTF::get2Points(HY_POINT* pSrcHYP,int is0,int ie0,int is1,int ie1,int& k,float& x0,float& y0,float& x1,float& y1);
BOOL COTFToOTF::PointInOtherContour(HY_POINT* pSrcHYP,int is0,int ie0,int is1,int ie1,int k);
void COTFToOTF::putInsideContour(HY_POINT* pDstHYP,HY_POINT* pSrcHYP,int& j,int inside,int ihy);

BOOL COTFToOTF::CurveToIntersect(HY_POINT* pDstHYP,HY_POINT* pSrcHYP,int istart,int iend,float& t0,int& i,int& j,int& is0,int& ie0,int& is1,int& ie1);
BOOL COTFToOTF::LineToIntersect(HY_POINT* pDstHYP,HY_POINT* pSrcHYP,int istart,int iend,int& inside,float& t0,int& i,int& j,int& is0,int& ie0,int& is1,int& ie1);

int COTFToOTF::TwoContourMerge1(HY_POINT* pDstHYP, int ihy);
BOOL COTFToOTF::IsTwoContourOverlop(HY_POINT* pSrcHYP, int ihy);
int COTFToOTF::getInsidePtFrmSrc(HY_POINT* pDstHYP,HY_POINT* pSrcHYP,int iDst,int iSrc);


/////////////////////////////////////////////////////////////////////////////////////////for boyahei redesign by program
int COTFToOTF::removeEndCurve(HY_POINT* pDstHYP,HY_POINT* pSrcHYP,int ihy);
void COTFToOTF::PIEEndProc(HY_POINT* pDstHYP,int ihy);
void COTFToOTF::getPIEEndXY(HY_POINT* pDstHYP,int i,int& x,int& y);
void COTFToOTF::getDIANEndXY(HY_POINT* pDstHYP,int i,int& x,int& y);
void COTFToOTF::DIANEndProc(HY_POINT* pDstHYP,int ihy);
//void COTFToOTF::getGOUleftEndXY(HY_POINT* pDstHYP,int i,int& x,int& y);
void COTFToOTF::GOUleftEndProc(HY_POINT* pDstHYP,int ihy);
//void COTFToOTF::getGOUupEndXY(HY_POINT* pDstHYP,int i,int& x,int& y);
void COTFToOTF::GOUupEndProc(HY_POINT* pDstHYP,int ihy);
int COTFToOTF::removeRepeatPT(HY_POINT* pDstHYP,int ihy);
int COTFToOTF::GOUrightEndProc(HY_POINT* pDstHYP,int ihy);
void COTFToOTF::removeOutofEmPT(HY_POINT* pDstHYP,int ihy);
void COTFToOTF::yMapping(HY_POINT* pDstHYP,int is,int ie,int d);
void COTFToOTF::PIEDIANProc(HY_POINT* pDstHYP,int ihy);
void COTFToOTF::PIEhPIEvProc(HY_POINT* pDstHYP,int ihy);

void COTFToOTF::OTFcheck(HY_POINT* pDstHYP,int ihy);

//////////////////////////////////////采集中心线
int COTFToOTF::GetStrokeLine(HY_POINT* pDstHYP,HY_POINT* pSrcHYP,int ihy);
int COTFToOTF::reorderContour(HY_POINT* pSrcHYP,int is,int ie);
int COTFToOTF::getMiddlePt(HY_POINT* pSrcHYP,int is,int ie,int stroke_end);
void COTFToOTF::HENGZHEProc(HY_POINT* pDstHYP,int ihy);
void COTFToOTF::startPtMove(HY_POINT* pSrcHYP,int is,int ie,int i);

/////////////////////////////////////////////////////////重心移动
void COTFToOTF::yDirectionMapping(HY_POINT* pDstHYP,int ihy,int dx,int dy);
void COTFToOTF::xyMapping(C_POINT* pDstCP,int dx,int dy,int ihy);

};
