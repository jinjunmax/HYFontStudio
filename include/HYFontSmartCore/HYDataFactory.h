#pragma once

class AFX_EXT_CLASS CHYDataFactory
{
public:
	CHYDataFactory(void);
	~CHYDataFactory(void);

	// Ik 格式转换到HY格式	 
	static	BOOL	IKToHY(PIK_HEAD pIKHead_In, PIK_CHARACTER pIKCharacter_In, IK_CONVTER_PARAM* pIKParam, HY_POINT* pHYP, int& iSrctPointsSize);
	static	BOOL	IKToIK(PIK_HEAD pIKHead_In, PIK_CHARACTER pIKCharacter_In, IK_CONVTER_PARAM* pIKParam, PIK_POINT pIKP, int& iPointsSize);

protected:
	static void		IKPointTangentCalculation(PIK_CHARACTER pIKCharacter_In);
	static void		getIntersectionPoint(float x0, float y0, int v0, float k0, float x1, float y1, int v1, float k1, int& x, int& y);
	static void		segmentAnalysis(PIK_CHARACTER pIKCharacter_In, int iss, int ise, HY_POINT* pHYP, int& ihy);
	static void		putVerSide(PIK_CHARACTER pIKCharacter_In, int iss, int ise, HY_POINT* pHYP, int& ihy);
	static void		putHorSide(PIK_CHARACTER pIKCharacter_In, int iss, int ise, HY_POINT* pHYP, int& ihy);
	static void		putRegularConvert(PIK_CHARACTER pIKCharacter_In, int iss, int ise, HY_POINT* pHYP, int& ihy);
	static void		putNormalEnd(PIK_CHARACTER pIKCharacter_In, int iss, int ise, HY_POINT* pHYP, int& ihy);
	static BOOL		pointInRec(int x1,int y1,int x2,int y2,int x,int y);
	static BOOL		isEnd(PIK_CHARACTER pIKCharacter_In, int iss, int ise);
	static BOOL		isFlatCurve(PIK_CHARACTER pIKCharacter_In, int iss, int ise, HY_POINT* pHYP, int& ihy);
	static void		getMaxDist(PIK_CHARACTER pIKCharacter_In, int iss, int ise, int& n, float& dmax);
	static void		putAllCurve(PIK_CHARACTER pIKCharacter_In, int ics, int ice, HY_POINT* pHYP, int& ihy);
	static void		putOtherHalfSide(PIK_CHARACTER pIKCharacter_In, int iss, int ise, HY_POINT* pHYP, int& ihy);
	static BOOL		isOtherHalfSide(PIK_CHARACTER pIKCharacter_In, int iss, int ise);
	static void		getSegmentCornerTangent(PIK_CHARACTER pIKCharacter_In, int iss, int ise,int&v1,int&v2,float&k1,float&k2);
	static BOOL		isVerSide(PIK_CHARACTER pIKCharacter_In, int iss, int ise);
	static BOOL		isHorSide(PIK_CHARACTER pIKCharacter_In, int iss, int ise);
	static BOOL		curveCalcAndPutHY(PIK_CHARACTER pIKCharacter_In, int iss, int ise, int ism, HY_POINT* pHYP, int& ihy);
	static BOOL		isHorVerHalfSide(PIK_CHARACTER pIKCharacter_In, int iss, int ise);
	static void		putVerHorHalfSide(PIK_CHARACTER pIKCharacter_In, int iss, int ise, HY_POINT* pHYP, int& ihy);
	static BOOL		PointOnLine(int x1,int y1, int x2, int y2, int x, int y);
	static BOOL		curveSegProcess(PIK_CHARACTER pIKCharacter_In, int iss, int ise, HY_POINT* pHYP, int& ihy);
	static BOOL		put1PtCv(PIK_CHARACTER pIKCharacter_In, int iss, int ise, HY_POINT* pHYP, int& ihy);
	static BOOL		onBspline2(int xs,int ys,int xm1,int ym1,int xm2, int ym2, int xe,int ye,int x,int y);
	static BOOL		put2PtCv(PIK_CHARACTER pIKCharacter_In, int iss, int ise, HY_POINT* pHYP, int& ihy);
	static BOOL		onBspline4(float xs,float ys,float xm1,float ym1,float xm2, float ym2,float xm3,float ym3,float xm4, float ym4, float xe,float ye,float x,float y);
	static BOOL		put4PtCv(PIK_CHARACTER pIKCharacter_In, int iss, int ise, HY_POINT* pHYP, int& ihy);
	static BOOL		onB2(float xs,float ys,float xm,float ym,float xe,float ye,float x,float y);
	static void		get2CvPt(PIK_CHARACTER pIKCharacter_In, int iss, int ise, float& x1, float& y1, float& x2, float& y2);
	static void		get1CvPt(PIK_CHARACTER pIKCharacter_In, int iss, int ise, int& x, int& y);
	static void		get4CvPt(PIK_CHARACTER pIKCharacter_In, int iss, int ise, float& x1, float& y1, float& x2, float& y2,float& x3, float& y3, float& x4, float& y4);
	static void		get5CvPt(PIK_CHARACTER pIKCharacter_In, int iss, int ise, float& x1, float& y1, float& x2, float& y2,float& x3, float& y3, float& x4, float& y4);
	static BOOL		put8PtCv(PIK_CHARACTER pIKCharacter_In, int iss, int ise, HY_POINT* pHYP, int& ihy);
	static int		removeRedundancyPtHY(HY_POINT* pHYP, int ihy);
	static BOOL		isOffDirect(float xs,float ys,float x1,float y1,float x2,float y2,float x3,float y3,float xe,float ye);
	static BOOL		isOffDirect2(float xs,float ys,float x1,float y1,float x2,float y2,float xe,float ye);

	static void		convertHorizentalEndZsIK(PIK_POINT pIKP, int iPointsSize);
	static int		scaleNarrowHYZS(HY_POINT* pHYP, int scale, int ihy);

	static int		removeRedundancyPtHYZS(HY_POINT* pHYP, int ihy);

//dengxianti sans process function as following
	static void		segAnalysisDX(PIK_CHARACTER pIKCharacter_In, int iss, int ise, HY_POINT* pHYP, int& ihy);
	static BOOL		isHorSideDX(PIK_CHARACTER pIKCharacter_In, int iss, int ise, HY_POINT* pHYP, int& ihy);
	static BOOL		isVerLeftSideDX(PIK_CHARACTER pIKCharacter_In, int iss, int ise, HY_POINT* pHYP, int& ihy);
	static BOOL		isVerRightSideDX(PIK_CHARACTER pIKCharacter_In, int iss, int ise, HY_POINT* pHYP, int& ihy);
	static BOOL		isHorHalfSideDX(PIK_CHARACTER pIKCharacter_In, int iss, int ise, HY_POINT* pHYP, int& ihy);
	static BOOL		isVerHalfLeftSideDX(PIK_CHARACTER pIKCharacter_In, int iss, int ise, HY_POINT* pHYP, int& ihy);
	static BOOL		isVerHalfRightSideDX(PIK_CHARACTER pIKCharacter_In, int iss, int ise, HY_POINT* pHYP, int& ihy);
	static BOOL		isVerHalfLeftSide1DX(PIK_CHARACTER pIKCharacter_In, int iss, int ise, HY_POINT* pHYP, int& ihy);
	static BOOL		isVerHalfSideDowntoCurveDX(PIK_CHARACTER pIKCharacter_In, int iss, int ise, HY_POINT* pHYP, int& ihy);
	static BOOL		isVerHalfSideUptoCurveDX(PIK_CHARACTER pIKCharacter_In, int iss, int ise, HY_POINT* pHYP, int& ihy);
	static BOOL		isVerHalfRightSideDowntoCurveDX(PIK_CHARACTER pIKCharacter_In, int iss, int ise, HY_POINT* pHYP, int& ihy);
	static BOOL		isPieDX(PIK_CHARACTER pIKCharacter_In, int iss, int ise, HY_POINT* pHYP, int& ihy);
	static BOOL		isHorHalfSide1DX(PIK_CHARACTER pIKCharacter_In, int iss, int ise, HY_POINT* pHYP, int& ihy);
	static BOOL		isVerHalfSide2DX(PIK_CHARACTER pIKCharacter_In, int iss, int ise, HY_POINT* pHYP, int& ihy);

//IK to IK process
	static void		moveTangentStartToCornerIK(IK_IMAGE_INF& tagImageInfTmp, PIK_POINT pIKP, int& iPointsSize);
	static void		buffCopyBackIK(IK_IMAGE_INF& tagImageInfTmp, PIK_POINT pIKP);
	static void		moveHorVerOneContourIK(IK_IMAGE_INF& tagImageInfTmp, PIK_POINT pIKP, int threshold, int& iPointsSize);
	static void		getContourIK(IK_IMAGE_INF& tagImageInfTmp, int& ics, int& ice);
	static void		removeRedundancyPtIK(IK_IMAGE_INF& tagImageInfTmp, PIK_POINT pIKP, int& iSrctPointsSize);
	static void		moveHorVerContoursIK(IK_IMAGE_INF& tagImageInfTmp, PIK_POINT pIKP, int threshold, int& iSrctPointsSize);
	static BOOL		contourIntersectIK(IK_IMAGE_INF& tagImageInfTmp, int ics, int ice, int ics1, int ice1);
	static void		scanContoursIK(IK_IMAGE_INF& tagImageInfTmp, int threshold, int ics, int ice, int ics1, int ice1);
	static void		buffCopyIK(IK_IMAGE_INF& tagImageInfTmp, PIK_POINT pIKP);
	static void		dataBufferIK(PIK_CHARACTER pIKCharacter_In, IK_IMAGE_INF& tagImageInfTmp);
	static BOOL		isGouIK(IK_IMAGE_INF tagImageInfTmp, int i, int i0);
	static BOOL		isOverlop(int y1,int y2,int y3,int y4);
	static BOOL		isEndForSZ(IK_IMAGE_INF tagImageInfTmp, int i, int ics, int ice);
	static BOOL		isContourContain(IK_IMAGE_INF& tagImageInfTmp, int ics, int ice, int ics1, int ice1);
	static void		markSegmentIK(PIK_CHARACTER pIKCharacter_In);

	static void		convertTangent2CurveIK(IK_IMAGE_INF& pIKCharacter_In, PIK_POINT pIKP, int iPointsSize);
	static void		convertTangent2CornerIK(IK_IMAGE_INF& tagImageInfTmp, PIK_POINT pIKP, int iPointsSize);
	static	BOOL	checkLineIK(IK_IMAGE_INF tagImageInfTmp, int i);

	static	void	segmentAnalysisST(PIK_CHARACTER pIKCharacter_In, int iss, int ise, HY_POINT* pHYP, int& ihy);
};
