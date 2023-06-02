#include "stdafx.h"
#include "HYFontFunc.h"

CHYFontFunc::CHYFontFunc()
{


}	// end of CHYFontFunc::CHYFontFunc()

CHYFontFunc::~CHYFontFunc()
{

}	// end of CHYFontFunc::~CHYFontFunc()

void CHYFontFunc::MergeGlyphs(std::vector<CHYGlyph>& src1Glyphs, std::vector<CHYGlyph>& src2Glyphs)
{
	size_t st = src2Glyphs.size();
	
	for (size_t i = 0; i < st; i++) {
		CHYGlyph src2Gly = src2Glyphs[i];

		if (src2Gly.strPostName == ".notdef")		
			continue;

		if (src2Gly.vtUnicode.size() > 0)
		{ 
			// 不去拷贝控制符
			if (src2Gly.vtUnicode[0] == 0xffff)
				continue;
			if (src2Gly.vtUnicode[0] == 0x0020)
				continue;
			if (src2Gly.vtUnicode[0] == 0x000a)
				continue;
			if (src2Gly.vtUnicode[0] == 0x000d)
				continue;
			if (src2Gly.vtUnicode[0] == 0x0000)
				continue;

			//如果unicode重复后边覆盖前边
			unsigned long ulUni1 = src2Gly.vtUnicode[0];
			int iIndex = FindGryphIndexByUnciode(src1Glyphs, ulUni1);
			if (iIndex != -1) {
				src1Glyphs[iIndex] = src2Gly;
			}
			else {
				src1Glyphs.push_back(src2Gly);
			}
		}
		else if (src2Gly.vtUnicode.size() == 0) { // 如果没有unicode
			if (src2Gly.strPostName == "") {	// 既没有unicode编码也没有name
				src1Glyphs.push_back(src2Gly);
			}
			else {	//如果name重复后边覆盖前面
				int iGID = FindGryphIndexByGlyName(src1Glyphs, src2Gly.strPostName);
				if (iGID != -1) {
					src1Glyphs[iGID] = src2Gly;
				}
				else {
					src1Glyphs.push_back(src2Gly);
				}
			}
		}		
	}

}	// end of void CHYFontFunc::MergeGlyphs()

int	CHYFontFunc::FindGryphIndexByGlyName(std::vector<CHYGlyph>& InGlyphs, string strGName)
{
	size_t  st = InGlyphs.size();
	for (size_t i = 0; i < st; i++) {
		CHYGlyph& glyph = InGlyphs[i];
		if (strGName == glyph.strPostName) {
			return i;
		}
	}
	return -1;

}	// end of int CHYFontFunc::FindGryphIndexByGlyName()

int	CHYFontFunc::FindGryphIndexByUnciode(std::vector<CHYGlyph>& InGlyphs, unsigned long ulUnicode)
{
	size_t st = InGlyphs.size();
	for (size_t i = 0; i < st; i++) {
		CHYGlyph& glyph = InGlyphs[i];
		for (size_t j = 0; j < glyph.vtUnicode.size(); j++) {
			if (ulUnicode == glyph.vtUnicode[j])
				return i;
		}
	}
	return -1;

}	// end of int CHYFontFunc::FindGryphIndexByUnciode()

void CHYFontFunc::GetSubsetbyName(std::vector<CHYGlyph>& InGlyphs, std::vector<string>& names, std::vector<CHYGlyph>& outHYGlyphs)
{
	outHYGlyphs.clear();

	size_t stIn = InGlyphs.size();
	size_t stNm = names.size();
	for (size_t i = 0; i < stIn; i++) {
		CHYGlyph inGly = InGlyphs[i];
		string strInName = inGly.strPostName;
		for (size_t j = 0; j < stNm; j++) {
			if (strInName == names[j]) {
				outHYGlyphs.push_back(inGly);
			}
		}
	}

}	// end of void GetSubsetbyName()

// 通过unicode编码提取子集，outHYGlyphs不会被清空，bOverlap决定是否新字形覆盖旧字形
void CHYFontFunc::GetSubsetbyUni(std::vector<CHYGlyph>& InGlyphs, std::vector<unsigned long>& uni, std::vector<CHYGlyph>& outHYGlyphs, bool bOverlap/*=TRUE*/)
{
	// 不考虑是否重叠
	if (!bOverlap) {		
		size_t stIn = InGlyphs.size();
		size_t stUni = uni.size();
		for (size_t i = 0; i < stIn; i++) {
			CHYGlyph inGly = InGlyphs[i];
			for (size_t j = 0; j < stUni; j++) {
				if (inGly.FindUnicode(uni[j])) {
					outHYGlyphs.push_back(inGly);
					break;
				}
			}
		}	
	}
	else{		
		size_t stIn = InGlyphs.size();
		size_t stUni = uni.size();
		for (size_t i = 0; i < stIn; i++) {
			CHYGlyph inGly = InGlyphs[i];
			for (size_t j = 0; j < stUni; j++) {
				if (inGly.FindUnicode(uni[j])) {
					int iIndex = FindGryphIndexByUnciode(outHYGlyphs, uni[j]);
					if (iIndex == -1) {
						outHYGlyphs.push_back(inGly);
					}
					else {
						outHYGlyphs[iIndex] = inGly;
					}
					break;
				}
			}
		}
	}

}	// end of void GetSubsetbyUni()

//两点间距离函数
double CHYFontFunc::Distance(const CHYPoint& p1, const CHYPoint& p2)
{
	return(sqrt((double)((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y))));

}// end of double Distance()

double CHYFontFunc::DistanceF(const HYPOINTF& p1, const HYPOINTF& p2)
{
	return(sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y)));

}// end of double Distance()

// 求三个点之间的夹角
double CHYFontFunc::AnglePoint(const CHYPoint& StartPt, const CHYPoint& MidPt, const CHYPoint& EndPt)
{
	double	angle = 0;
	double	pi = 3.1415926;
	float	pt1x = 0, pt1y = 0;
	float	pt2x = 0, pt2y = 0;

	pt1x = (float)(StartPt.x - MidPt.x);
	pt1y = (float)(StartPt.y - MidPt.y);
	pt2x = (float)(EndPt.x - MidPt.x);
	pt2y = (float)(EndPt.y - MidPt.y);
	if ((pt1x==0&&pt1y== 0) || (pt2x==0&&pt2y==0)){
		angle = 1;
	}
	else{
		double cosc = (pt1x*pt2x+pt1y*pt2y) /
			(sqrt((double)pt1x*pt1x+pt1y*pt1y)*sqrt((double)pt2x*pt2x+pt2y*pt2y));

		if (cosc<=-1) cosc=-1;
		else if (cosc>=1) cosc=1;
		angle = acos(cosc)*180/pi;
	}

	return angle;

}	// end of double AnglePoint()

HYPOINTF CHYFontFunc::GetBezierCenter(std::vector<CHYPoint> vtBPoints, int sPoints)
{
	HYPOINTF rtpoint;
	rtpoint.x = 0;
	rtpoint.y = 0;

	for (int i = 0; i < sPoints; i++) {
		HYPOINTF nxtPnt = GetBezierPos(i / (double)(sPoints - 1), vtBPoints);

		rtpoint.x += nxtPnt.x;
		rtpoint.y += nxtPnt.y;		
	}

	rtpoint.x /= sPoints;
	rtpoint.y /= sPoints;

	return rtpoint;


}	// end of double CHYFontFunc::GetBezierCenter()

// 最高支持求三次贝塞尔曲线弧长（近似值）t采样频率
double CHYFontFunc::GetBezierLength(std::vector<CHYPoint> vtBPoints, int sPoints)
{
	double dbLength = 0;
	HYPOINTF fstPnt = GetBezierPos(0,vtBPoints);
	for (int i = 1; i < sPoints; i++){
		HYPOINTF nxtPnt = GetBezierPos(i/(double)(sPoints - 1), vtBPoints);
		dbLength += DistanceF(fstPnt, nxtPnt);
		fstPnt = nxtPnt;
	}

	return dbLength;

}	// end of double CHYFontFunc::GetBezierLength()

// 求贝塞尔曲线采样点
HYPOINTF  CHYFontFunc::GetBezierPos(double t, std::vector<CHYPoint>& vtBPoints)
{
	HYPOINTF pntF;
	pntF.x = 0;
	pntF.y = 0;

	size_t stFact = vtBPoints.size();	
	float subtrDt = 1.0f - t;
	//两次贝塞尔曲线
	if (stFact == 3) {		
		pntF.x = (float)vtBPoints[0].x*subtrDt*subtrDt + 2*t*subtrDt*(float)vtBPoints[1].x + t*t*(float)vtBPoints[2].x;
		pntF.y = (float)vtBPoints[0].y*subtrDt*subtrDt + 2*t*subtrDt*(float)vtBPoints[1].y + t*t*(float)vtBPoints[2].y;
	}

	// 三次贝塞尔曲线
	if (stFact == 4) {
		pntF.x = (float)vtBPoints[0].x * subtrDt * subtrDt * subtrDt
				+ 3 * t * subtrDt * subtrDt * (float)vtBPoints[1].x
				+ 3 * t * t * subtrDt * (float)vtBPoints[2].x
				+ t * t * t * (float)vtBPoints[3].x;

		pntF.y = (float)vtBPoints[0].y * subtrDt * subtrDt * subtrDt
				+ 3 * t * subtrDt * subtrDt * (float)vtBPoints[1].y
				+ 3 * t * t * subtrDt * (float)vtBPoints[2].y
				+ t * t * t * (float)vtBPoints[3].y;
	}

	return pntF;

}	// end of void  CHYFontFunc::GetBezierPos()

void CHYFontFunc::GetCircularF(HYPOINTF P1, HYPOINTF P2, HYPOINTF P3, float& R, HYPOINTF PCenter)
{
	double a = 2 * (P2.x - P1.x);
	double b = 2 * (P2.y - P1.y);
	double c = P2.x * P2.x + P2.y * P2.y - P1.x * P1.x - P1.y * P1.y;
	double d = 2 * (P3.x - P2.x);
	double e = 2 * (P3.y - P2.y);
	double f = P3.x * P3.x + P3.y * P3.y - P2.x * P2.x- P2.y * P2.y;

	PCenter.x = (b*f-e*c)/(b*d-e*a);
	PCenter.y = (d*c-a*f)/(b*d-e*a);
	R = sqrtf((float)((PCenter.x-P1.x)*(PCenter.x-P1.x)+(PCenter.y-P1.y)*(PCenter.y-P1.y)));

}	// end of static void CHYFontFunc::GetCircularF()

// 通过圆点和弧长的两个端点获取三次贝塞尔曲线的两个线外点。
void CHYFontFunc::GetBzrOffPnt(HYPOINTF P1, HYPOINTF P4, HYPOINTF PCenter, HYPOINTF& C2, HYPOINTF& C3)
{
	double x1 = P1.x;
	double y1 = P1.y;
	double x4 = P4.x;
	double y4 = P4.y;
	double xc = PCenter.x;
	double yc = PCenter.y;

	double ax = x1 - xc;
	double ay = y1 - yc;
	double bx = x4 - xc;
	double by = y4 - yc;
	double q1 = ax * ax + ay * ay;
	double q2 = q1 + ax * bx + ay * by;
	double k2 = (4/3) * (sqrt(2 * q1 * q2) - q2) / (ax * by - ay * bx);
	C2.x  = xc + ax - k2 * ay;
	C2.y = yc + ay + k2 * ax;
	C3.x = xc + bx + k2 * by;
	C3.y = yc + by - k2 * bx;

}	// end of  void CHYFontFunc::GetBezier()

// 获取两点间的斜率
double CHYFontFunc::GetSlope(HYPOINTF P1, HYPOINTF P2)
{
	if (P1.x == P2.x) return 0;
	return (P2.y - P1.y) / (P2.x - P1.x);

}	// end of double CHYFontFunc::GetSlope()

double CHYFontFunc::GetSlope(CHYPoint& P1, CHYPoint& P2)
{
	if (P1.x == P2.x) return 0;
	return (P2.y - P1.y) / (double)(P2.x - P1.x);

}	// end of double CHYFontFunc::GetSlope()

CHYPoint CHYFontFunc::GetCenter(std::vector<CHYPoint> vtBPoints)
{
	CHYPoint Center;
	Center.flag = POINT_FLG_ANCHOR;
	size_t st = vtBPoints.size();
	for (int i = 0; i < st; i++)	{
		Center.x += vtBPoints[i].x;
		Center.y += vtBPoints[i].y;
	}

	Center.x /= (int)st;
	Center.y /= (int)st;

	return Center;

}	// end of CHYPoint CHYFontFunc::GetCenter()

void CHYFontFunc::SmoothFitting(std::vector<CHYPoint>& InvtPnt, std::vector<CHYPoint>& OutvtPnt)
{
	int stIn = (int)InvtPnt.size();
	if (stIn < 4) {
		for each (CHYPoint var in InvtPnt){
			var.flag = POINT_FLG_ANCHOR;
			OutvtPnt.push_back(var);
		}
		return;
	}

	OutvtPnt.push_back(InvtPnt[0]);
	for (int n = 0; n < stIn; n++) {
		if (n <= stIn - 4) {			
			double a1 = 0, a2 = 0, a3 = 0, a4 = 0;
			for (double t = 0.0; t <= 1.0; t += 0.1) {
				a1 = pow((1.0 - t), 3.0) / 6.0;
				a2 = (3.0 * pow(t, 3.0) - 6.0 * pow(t, 2.0) + 4.0) / 6.0;
				a3 = (-3.0 * pow(t, 3.0) + 3.0 * pow(t, 2.0) + 3.0 * t + 1) / 6.0;
				a4 = pow(t, 3.0) / 6.0;

				double x = a1 * (double)InvtPnt[n].x + a2 * (double)InvtPnt[n + 1].x +
					a3 * (double)InvtPnt[n + 2].x + a4 * (double)InvtPnt[n + 3].x;
				double y = a1 * (double)InvtPnt[n].y + a2 * (double)InvtPnt[n + 1].y +
					a3 * (double)InvtPnt[n + 2].y + a4 * (double)InvtPnt[n + 3].y;

				CHYPoint puhPnt;
				puhPnt.flag = POINT_FLG_ANCHOR;
				puhPnt.x = ::HY_RealRount(x);
				puhPnt.y = ::HY_RealRount(y);
				OutvtPnt.push_back(puhPnt);
			}
		}
	}
	OutvtPnt.push_back(InvtPnt[stIn - 1]);

}	// void CHYFontFunc::BezierSmoothFitting()

#define N           1000
#define cosA        -0.7   //标记角度位
#define bcdis       10       //bezier拟合时判断是否加点（数值越大加点的机会越小）
void CHYFontFunc::BezierFitting(std::vector<CHYPoint>& InvtPnt, std::vector<CHYPoint>& OutvtPnt, int BezierN, double value, double Delta)
{
	int iInVtSize = InvtPnt.size();
	if (iInVtSize < 3){
		for each (CHYPoint pt in InvtPnt){
			pt.flag = POINT_FLG_ANCHOR;
			OutvtPnt.push_back(pt);
		}
		return;
	}

	std::vector<CHYPoint> vtBezierPnt;
	CHYPoint pt = InvtPnt[0];
	pt.flag = POINT_FLG_ANCHOR;
	OutvtPnt.push_back(pt);
	vtBezierPnt.push_back(pt);

	int fdflag = 0,j=1;
	double k0 = 0,k1 = 0;
	while (j < iInVtSize - 1) {
		CHYPoint& temp_Point1 = InvtPnt[j-1];
		CHYPoint& temp_Point2 = InvtPnt[j];
		CHYPoint& temp_Point3 = InvtPnt[j+1];

		double a = CHYFontFunc::Distance(temp_Point1, temp_Point2);
		double b = CHYFontFunc::Distance(temp_Point2, temp_Point3);
		double c = CHYFontFunc::Distance(temp_Point1, temp_Point3);
		double cosc = (a * a + b * b - c * c) / (2 * a * b);

		if ((temp_Point2.x - temp_Point1.x) != 0) {
			k0 = (double)(temp_Point2.y - temp_Point1.y) /
				(temp_Point2.x - temp_Point1.x); 
		}
		else {
			k0 = N + 1;
		}

		if (temp_Point3.x - temp_Point2.x != 0)	{
			k1 = (double)(temp_Point3.y - temp_Point2.y) / 
				(temp_Point3.x - temp_Point2.x);
		}
		else{
			k1 = N + 1;
		}

		std::vector<CHYPoint>	toPointlist, toPointlistOut;
		CHYPoint       toHYPoint, b_HYPoint;
		if ((cosc >= cosA) || (abs(k0) >= 1 && abs(k1) < 1) ||
			(abs(k0) < 1 && abs(k1) >= 1) || 
			temp_Point3.flag == POINT_FLG_CONTROL){
			size_t b_size = vtBezierPnt.size();
			
			if (b_size > 1)	{	
				vtBezierPnt.push_back(temp_Point2);

				if (BezierN == 2)
				{
					bezierS2_2(vtBezierPnt, toPointlist, value, BezierN, Delta);
				}
				else if (BezierN == 3)
				{
					bezierS(vtBezierPnt, toPointlist, value, BezierN);
				}

				size_t stToPntlst = toPointlist.size();
				for (size_t l = 0; l < stToPntlst; l++){
					b_HYPoint = toPointlist[l];
					OutvtPnt.push_back(b_HYPoint);
				}
				
				temp_Point2.flag = POINT_FLG_ANCHOR;				
				OutvtPnt.push_back(temp_Point2);

				vtBezierPnt.clear();			
				vtBezierPnt.push_back(temp_Point2);
				j = j + 1;
				if (j == iInVtSize - 1){
					temp_Point3.flag = POINT_FLG_ANCHOR;
					OutvtPnt.push_back(temp_Point3);
				}
			}
			else{
				temp_Point2.flag = POINT_FLG_ANCHOR;
				OutvtPnt.push_back(temp_Point2);

				vtBezierPnt.clear();
				vtBezierPnt.push_back(temp_Point2);
				j = j + 1;
				if (j == iInVtSize - 1)	{
					temp_Point3.flag = POINT_FLG_ANCHOR;
					OutvtPnt.push_back(temp_Point3);
				}
			}
		}
		else{			
			vtBezierPnt.push_back(temp_Point2);
			j = j + 1;
			if (j == iInVtSize - 1)	{
				vtBezierPnt.push_back(temp_Point3);
				int b_size = (int)vtBezierPnt.size();				
				if (BezierN == 2){
					bezierS2_2(vtBezierPnt, toPointlist, value, BezierN, Delta);
				}
				else if (BezierN == 3){
					bezierS(vtBezierPnt, toPointlist, value, BezierN);
				}

				size_t stToPntlst = toPointlist.size();
				for (int l = 0; l < (int)stToPntlst; l++){
					OutvtPnt.push_back(toPointlist[l]);					
				}

				temp_Point3.flag = POINT_FLG_ANCHOR;
				OutvtPnt.push_back(temp_Point3);
			}
		}			
	}

}	// end of void CHYFontFunc::BezierFitting()

void CHYFontFunc::bezierS2_2(std::vector<CHYPoint>& temp_PointList, std::vector<CHYPoint>& toPointlist, double value, int BezierN, double Delta)
{
	int i, j, l;
	int NN = temp_PointList.size();
	int ANN = NN + 2, addPntlst1Num=0;
	double d[N] = {0};//累加弦长
	double t[N] = {0};//对应轮廓段上离散点的估计的Bezier曲线上的点
	int    bflag = 0;
	double dk[N] = {0};
	double maxdk = 0;
	int    maxdki = 0;
	int dmmi = 0;
	double dmm = 0,dmi = 100000;
	CHYPoint m_point1,m_point2,temp_point,m_pt;

	std::vector<CHYPoint>	add_PointList,PointList,new_PointList,add_PointList1;
	bflag = beziern(temp_PointList, toPointlist, BezierN, value);
	if (bflag != -1){
		return;
	}

	for (i = 0; i < NN; i++){		
		add_PointList.push_back(temp_PointList[i]);
	}
	add_PointList1.push_back(add_PointList[0]);


	for (j = 0; j < NN - 1; j++)
	{
		if (j == 0)	{	
			m_point1 = add_PointList[j];
			m_point2 = add_PointList[j+1];
			if (Distance(m_point1, m_point2) < 100)	{
				temp_point.x = (m_point1.x * 2 + m_point2.x * 2) / 4;
				temp_point.y = (m_point1.y * 2 + m_point2.y * 2) / 4;				
				add_PointList1.push_back(temp_point);
			}			
			add_PointList1.push_back(m_point2);

		}
		else if (j == NN-2)	{	
			m_point1 = add_PointList[j];
			m_point2 = add_PointList[j + 1];

			if (Distance(m_point1, m_point2) < 100){
				temp_point.x = (m_point1.x * 2 + m_point2.x * 2) / 4;
				temp_point.y = (m_point1.y * 2 + m_point2.y * 2) / 4;				
				add_PointList1.push_back(temp_point);
			}
			add_PointList1.push_back(m_point2);
		}
		else{			
			m_point2 = add_PointList[j + 1];			
			add_PointList1.push_back(add_PointList[j + 1]);
		}
	}

	if (isSCurve(temp_PointList) == 1){
		for (j = 1; j < NN-1; j++){	
			m_point1 = temp_PointList[0];
			m_point2 = temp_PointList[NN - 1];			
			temp_point = temp_PointList[j];
			dk[j] = DispointToLine(m_point1, m_point2, temp_point);
			if (dk[j] > maxdk){
				maxdk = dk[j]; maxdki = j;
			}
		}
		if (maxdk < 5){
			MeanContour(add_PointList1, new_PointList);
		}
		else{
			addPntlst1Num = add_PointList1.size();
			for (i = 0; i < addPntlst1Num; i++){
				new_PointList.push_back(add_PointList1[i]);
			}
		}
	}
	else
	{
		addPntlst1Num = add_PointList1.size();
		for (i = 0; i < addPntlst1Num; i++)	{			
			new_PointList.push_back(m_point2);
		}
	}

	add_PointList.clear();
	toPointlist.clear();
	PointList.clear();

	//增加端点数据
	int newPntLstNum = new_PointList.size();
	m_point1 = new_PointList[0];
	m_point2 = new_PointList[1];	
	temp_point.x = 2 * m_point1.x - m_point2.x;
	temp_point.y = new_PointList[newPntLstNum - 1].y;
	add_PointList.push_back(temp_point);

	for (i = 0; i < newPntLstNum; i++){	
		add_PointList.push_back(new_PointList[i]);
	}	
	m_point1 = new_PointList[newPntLstNum - 2];	
	m_point2 = new_PointList[newPntLstNum - 1];
	temp_point.x = 2 * m_point2.x - m_point1.x;
	temp_point.y = new_PointList[0].y;
	add_PointList.push_back(temp_point);

	m_point1 = add_PointList[0];
	PointList.push_back(m_point1);
	for (i = 1; i < add_PointList.size(); i++){		
		m_point2 = add_PointList[i];
		if (m_point1.x != m_point2.x || m_point1.y != m_point2.y){
			PointList.push_back(m_point2);
			m_point1.x = m_point2.x;
			m_point1.y = m_point2.y;
		}
	}
	ANN = PointList.size();
	double* A = new double[ANN * ANN];
	double* A_ = new double[ANN * ANN];
	double* B = new double[ANN * 2 * ANN];
	CHYPoint* toPoint0 = new CHYPoint[ANN + 2];
	CHYPoint* toPoint1 = new CHYPoint[ANN + 2];
	for (i = 1; i < ANN; i++){
		m_point1 = PointList[i - 1];
		m_point2 = PointList[i];
		d[i] = Distance(m_point1, m_point2);
	}
	for (i = 1; i < ANN - 1; i++){
		m_point1 = PointList[i-1];
		m_point2 = PointList[i];
		temp_point = PointList[i+1];		
		t[i] = d[i] / (d[i] + d[i + 1]);
	}
	for (i = 0; i < ANN; i++){
		for (j = 0; j < ANN; j++){
			A[i * ANN + j] = 0;
		}
	}
	for (i = 1; i < ANN - 1; i++){
		A[i * ANN + i - 1] = (t[i] * t[i] - 2 * t[i] + 1) / 2;
		A[i * ANN + i] = (-2 * t[i] * t[i] + 2 * t[i] + 1) / 2;
		A[i * ANN + i + 1] = (t[i] * t[i]) / 2;
	}

	/*****************************************************************/
	A[0] = (-2 * t[ANN - 2] * t[ANN - 2] + 2 * t[ANN - 2] + 1) / 2;
	A[1] = (t[ANN - 2] * t[ANN - 2]) / 2;
	A[ANN - 3] = (t[ANN - 2] * t[ANN - 2] - 2 * t[ANN - 2] + 1) / 2;

	A[(ANN - 1) * ANN + 2] = (t[1] * t[1]) / 2;
	A[(ANN - 1) * ANN + ANN - 2] = (t[1] * t[1] - 2 * t[1] + 1) / 2;
	A[(ANN - 1) * ANN + ANN - 1] = (-2 * t[1] * t[1] + 2 * t[1] + 1) / 2;

	for (i = 0; i < ANN; i++){		
		temp_point = PointList[i];
		toPoint1[i].x = temp_point.x;
		toPoint1[i].y = temp_point.y;
		toPoint1[i].flag = POINT_FLG_CONTROL;
	}

	double error = 10000;
	while (abs(error) > value){
		error = 0;
		new_PointList.clear();
		for (i = 0; i < ANN; i++){
			toPoint0[i].x = toPoint1[i].x;
			toPoint0[i].y = toPoint1[i].y;
			toPoint0[i].flag = POINT_FLG_CONTROL;
		}

		temp_point = PointList[0];
		m_point1.x = A[0] * toPoint0[0].x + A[1] * toPoint0[1].x + A[ANN - 3] * toPoint0[ANN - 3].x;
		m_point1.y = A[0] * toPoint0[0].y + A[1] * toPoint0[1].y + A[ANN - 3] * toPoint0[ANN - 3].y;
		error += Distance(temp_point, m_point1);

		toPoint1[0].x = toPoint0[0].x + temp_point.x - m_point1.x;
		toPoint1[0].y = toPoint0[0].y + temp_point.y - m_point1.y;

		for (int i = 1; i < ANN - 1; i++){			
			temp_point = PointList[i];
			m_point1.x = A[i * ANN + i - 1] * toPoint0[i - 1].x + A[i * ANN + i] * toPoint0[i].x + A[i * ANN + i + 1] * toPoint0[i + 1].x;
			m_point1.y = A[i * ANN + i - 1] * toPoint0[i - 1].y + A[i * ANN + i] * toPoint0[i].y + A[i * ANN + i + 1] * toPoint0[i + 1].y;
			error += Distance(temp_point, m_point1);

			toPoint1[i].x = toPoint0[i].x + temp_point.x - m_point1.x;
			toPoint1[i].y = toPoint0[i].y + temp_point.y - m_point1.y;
		}

		temp_point = PointList[PointList.size()-1];
		m_point1.x = A[(ANN - 1) * ANN + 2] * toPoint0[2].x + A[(ANN - 1) * ANN + ANN - 2] * toPoint0[ANN - 2].x + A[(ANN - 1) * ANN + ANN - 1] * toPoint0[ANN - 1].x;
		m_point1.y = A[(ANN - 1) * ANN + 2] * toPoint0[2].y + A[(ANN - 1) * ANN + ANN - 2] * toPoint0[ANN - 2].y + A[(ANN - 1) * ANN + ANN - 1] * toPoint0[ANN - 1].y;
		error += Distance(temp_point, m_point1);

		toPoint1[ANN - 1].x = toPoint0[ANN - 1].x + temp_point.x - m_point1.x;
		toPoint1[ANN - 1].y = toPoint0[ANN - 1].y + temp_point.y - m_point1.y;
	}
	for (i = 1; i < ANN - 1; i++)	{
		toPoint0[i].flag = POINT_FLG_CONTROL;
	}

	add_PointList.clear();
	PointList.clear();
	for (i = 2; i < ANN - 2; i++){		
		PointList.push_back(toPoint0[i]);	
	}
	
	m_point1 = PointList[0];
	toPointlist.push_back(m_point1);

	int szpntLst = (int)PointList.size();
	for (int j = 1; j < szpntLst-1; j++)	{
		m_point2 = PointList[j + 1];
		temp_point = PointList[j];
		if (temp_point.x == m_point1.x && temp_point.y == m_point1.y)continue;
		if (abs(DispointToLine(m_point1, m_point2, temp_point)) < 0.5 * Delta)continue;

		m_point1 = temp_point;
		toPointlist.push_back(temp_point);		
	}
	temp_point = PointList[PointList.size() - 1];
	toPointlist.push_back(temp_point);

	add_PointList.clear();
	PointList.clear();
	new_PointList.clear();
	add_PointList1.clear();
	delete[] A;
	delete[] A_;
	delete[] B;
	delete[] toPoint0;
	delete[] toPoint1;

}// end of double  CHYFontFunc::bezierS2_2()

void CHYFontFunc::bezierS(std::vector<CHYPoint>& InPnts, std::vector<CHYPoint>& outPnts, double value, int BezierN)
{
	int i=0, j=0;
	float  DPhi;
	double dk[1000] = { 0 };
	double maxdk = 0;
	int    maxdki = 0, maxdkii = 0, NN = (int)InPnts.size();
	CHYPoint toPoint1,toPoint2,toPoint3,toPoint4,toPoint21,toPoint22;
	CHYPoint m_point1, m_point2, m_point3, m_point4, temp_point;
	std::vector<CHYPoint>	temp_PointList1;
	std::vector<CHYPoint>	temp_PointList2;
	int      bflag = 0,returnflag = 0;
	bflag = beziern(InPnts, outPnts, BezierN, value);

	int istInPnts = InPnts.size();
	if (bflag == -1){
		bflag = 0;
		outPnts.clear();		
		for (j = 1; j < istInPnts - 1; j++)	{
			m_point1 = InPnts[0];
			m_point2 = InPnts[NN-1];		
			
			temp_point = InPnts[j];
			dk[j] = DispointToLine(m_point1, m_point2, temp_point);
			if (dk[j] > maxdk){
				maxdk = dk[j]; maxdkii = j; returnflag = 1;
			}
		}
		if (returnflag == 0) return;
		returnflag = 0;
		if (istInPnts >= 4){
			//找拐点
			for (j = 1; j < istInPnts - 2; j++){
				m_point1 = InPnts[j - 1];
				m_point2 = InPnts[j];
				m_point3 = InPnts[j+1];
				m_point4 = InPnts[j+2];				
				DPhi = ((m_point2.x - m_point1.x) * 
					(m_point3.y - m_point2.y) - (m_point3.x - m_point2.x) * 
					(m_point2.y - m_point1.y)) * 
					((m_point3.x - m_point2.x) * 
					(m_point4.y - m_point3.y) - (m_point4.x - m_point3.x) * 
					(m_point3.y - m_point2.y));

				if (DPhi < 0){
					maxdki = j; returnflag = 1;
				}
			}
			if (returnflag == 0)
				maxdki = maxdkii;
		}
		else{
			maxdki = maxdkii;
		}

		for (j = 0; j <= maxdki; j++){
			m_point1 = InPnts[j];
			temp_PointList1.push_back(m_point1);
		}

		if (temp_PointList1.size() <= 2){
			temp_PointList2.clear();
		}
		else if (temp_PointList1.size() <= 4){
			beziern(temp_PointList1, temp_PointList2, BezierN, value);
		}
		else{
			bezierS(temp_PointList1, temp_PointList2, value, BezierN);
		}

		for (i = 0; i < temp_PointList2.size(); i++){
			outPnts.push_back(temp_PointList2[i]);
		}
		outPnts.push_back(InPnts[maxdki]);

		temp_PointList1.clear();
		temp_PointList2.clear();
		for (j = maxdki; j < NN; j++){
			temp_PointList1.push_back(InPnts[j]);
		}
		if (temp_PointList1.size() <= 2){
			temp_PointList2.clear();
		}
		else if (temp_PointList1.size() <= 4){
			beziern(temp_PointList1, temp_PointList2, BezierN, value);
		}
		else{
			bezierS(temp_PointList1, temp_PointList2, value, BezierN);
		}

		for (i = 0; i < temp_PointList2.size(); i++) {
			outPnts.push_back(temp_PointList2[i]);
		}

		temp_PointList1.clear();
		temp_PointList2.clear();
	}

}  // end of double  CHYFontFunc::bezierS()

int  CHYFontFunc::beziern(std::vector<CHYPoint>& temp_PointList, std::vector<CHYPoint>& toPointlist, int BN, double value)
{
	int i, j, l;
	int NN = (int)temp_PointList.size();
	int bflag = 0;
	int Guassflag = 0;
	double segma = 0;
	double tempx = 0;
	double tempy = 0;
	double dis[N] = { 0 };
	int maxdi = 0;
	int size = 0;
	double sum = 0;                 //累加弦长
	double t[N] = { 0 };              //对应轮廓段上离散点的估计的Bezier曲线上的点
	double dt = 0;                  //单位增加量
	double dk[1000] = { 0 };
	double maxdk = 0;
	int    maxdki = 0;
	double dmm = 0;
	double dmi = 100000;
	int dmmi = 0;
	double d[N - 1] = { 0 };              //弦长
	double* S = new double[(BN - 1) * (BN + 1)];
	double* T = new double[(BN - 1) * 2];
	double* A = new double[(BN - 1) * (BN - 1)];
	double* X = new double[BN - 1];
	double* B = new double[BN - 1];
	CHYPoint m_point1;
	CHYPoint m_point2;
	CHYPoint temp_point;
	
	CHYPoint* toPoint = new CHYPoint[BN - 1];
	std::vector<CHYPoint>	add_PointList,add_PointList1,new_PointList;

	add_PointList.clear();
	for (i = 0; i < (BN - 1) * (BN + 1); i++){
		S[i] = 0;
	}
	for (i = 0; i < (BN - 1) * 2; i++){
		T[i] = 0;
	}

	for (i = 1; i < NN; i++){
		m_point1 = temp_PointList[i - 1];
		m_point2 = temp_PointList[i];

		d[i - 1] = Distance(m_point1, m_point2);
		if (d[i - 1] > dmm)	{
			dmmi = i - 1; dmm = d[i - 1];
		}
		if (d[i - 1] < dmi){
			dmi = d[i - 1];
		}
	}

	if ((dmm / dmi) >= bcdis){
		for (j = 0; j <= dmmi; j++)	{
			add_PointList.push_back(temp_PointList[j]);
		}
		for (j = dmmi; j < (dmmi + 1); j++)
		{
			m_point1 = temp_PointList[j];
			m_point2 = temp_PointList[j+1];

			temp_point.x = (m_point1.x * 3 + m_point2.x) / 4;
			temp_point.y = (m_point1.y * 3 + m_point2.y) / 4;
			add_PointList.push_back(temp_point);

			temp_point.x = (m_point1.x * 2 + m_point2.x * 2) / 4;
			temp_point.y = (m_point1.y * 2 + m_point2.y * 2) / 4;			
			add_PointList.push_back(temp_point);

			temp_point.x = (m_point1.x + m_point2.x * 3) / 4;
			temp_point.y = (m_point1.y + m_point2.y * 3) / 4;
			add_PointList.push_back(temp_point);
		}

		for (j = (dmmi + 1); j < NN; j++){
			add_PointList.push_back(temp_PointList[j]);
		}
	}
	else
	{
		for (i = 0; i < NN; i++){
			add_PointList.push_back(temp_PointList[i]);
		}
	}
	
	add_PointList1.push_back(add_PointList[0]);
	for (j = 0; j < add_PointList.size() - 1; j++){	

		m_point1 = add_PointList[j];
		m_point2 = add_PointList[j + 1];
		temp_point.x = (m_point1.x + m_point2.x) / 2;
		temp_point.y = (m_point1.y + m_point2.y) / 2;

		add_PointList1.push_back(temp_point);		
		add_PointList1.push_back(m_point2);
	}

	if (isSCurve(temp_PointList) == 1){
		for (j = 1; j < temp_PointList.size() - 1; j++){

			m_point1 = temp_PointList[0];
			m_point2 = temp_PointList[temp_PointList.size() - 1];			
			temp_point = temp_PointList[j];
			dk[j] = DispointToLine(m_point1, m_point2, temp_point);
			if (dk[j] > maxdk){
				maxdk = dk[j]; maxdki = j;
			}
		}
		if (maxdk < 5){
			MeanContour(add_PointList1,new_PointList);
		}
		else
		{
			for (i = 0; i < add_PointList1.size(); i++)	{	
				m_point2 = add_PointList1[i];				
				new_PointList.push_back(add_PointList1[i]);
			}
		}
	}
	else
	{
		for (i = 0; i < add_PointList1.size(); i++)	{
			new_PointList.push_back(add_PointList1[i]);
		}
	}

	int addNN = new_PointList.size();
	double* Phi = new double[(BN+1) * addNN];           //Bezier曲线的基函数
	for (i = 1; i < addNN; i++){	
		m_point1 = new_PointList[i-1];
		m_point2 = new_PointList[i];
		d[i - 1] = Distance(m_point1, m_point2);
	}

	for (i = 0; i < addNN - 1; i++){
		sum += d[i];
	}

	dt = 1 / sum;
	for (i = 0; i < addNN; i++){
		double sum0 = 0;
		for (j = 0; j < i; j++){
			sum0 += d[j];
		}
		t[i] = sum0 * dt;
	}

	for (j = 0; j < (BN + 1); j++){
		for (i = 0; i < addNN; i++)	{
			Phi[j * addNN + i] = (float)(Jiecheng(BN) / (Jiecheng(j) * Jiecheng(BN - j))) * Chengfang(1 - t[i], BN - j) * Chengfang(t[i], j);
		}
	}

	for (i = 0; i < BN - 1; i++){
		for (j = 0; j < BN + 1; j++){
			for (l = 0; l < addNN; l++)	{
				S[i * (BN + 1) + j] = S[i * (BN + 1) + j] + Phi[(i + 1) * addNN + l] * Phi[j * addNN + l];
			}
		}
	}
	for (i = 0; i < BN - 1; i++)
	{
		for (l = 0; l < addNN; l++)	{	
			m_point1 = new_PointList[l];
			T[2 * i] = T[2 * i] + Phi[(i + 1) * addNN + l] * m_point1.x;
			T[2 * i + 1] = T[2 * i + 1] + Phi[(i + 1) * addNN + l] * m_point1.y;
		}
	}
	m_point1 = new_PointList[0];
	m_point2 = new_PointList[addNN - 1];

	for (i = 0; i < BN - 1; i++){
		for (j = 0; j < BN - 1; j++){
			A[i * (BN - 1) + j] = S[i * (BN + 1) + j + 1];
		}
	}
	for (i = 0; i < BN - 1; i++){
		B[i] = T[2 * i] - S[i * (BN + 1) + 0] * m_point1.x - S[i * (BN + 1) + BN] * m_point2.x;
	}

	Guassflag = Guass(A, X, B, BN - 1, Guassflag);

	for (i = 0; i < BN - 1; i++){
		toPoint[i].x = X[i];
	}
	for (i = 0; i < BN - 1; i++){
		B[i] = T[2 * i + 1] - S[i * (BN + 1) + 0] * m_point1.y - S[i * (BN + 1) + BN] * m_point2.y;
	}
	Guassflag = Guass(A, X, B, BN - 1, Guassflag);
	for (i = 0; i < BN - 1; i++){
		toPoint[i].y = X[i];
	}

	for (i = 0; i < BN - 1; i++){
			toPoint[i].flag = POINT_FLG_CONTROL;
	}

	for (i = 0; i < BN - 1; i++){
		toPointlist.push_back(toPoint[i]);
	}

	bflag = bezierntest(new_PointList, toPointlist, BN, bflag, value);

	if (Phi) delete[] Phi;
	Phi = NULL;
	if (toPoint) delete[] toPoint;
	toPoint = NULL;
	if (B) delete[] B;
	B = NULL;
	if (X) delete[] X;
	X = NULL;
	if (A) delete[] A;
	A = NULL;
	if (T) delete[] T;
	T = NULL;
	if (S) delete[] S;
	S = NULL;

	return bflag;

}  // end of double  CHYFontFunc::beziern()

//计算点到直线的距离
double  CHYFontFunc::DispointToLine(CHYPoint& startpoint, CHYPoint& endpoint, CHYPoint& tagpoint)
{
	double dis;
	double line;
	int A;
	int B;
	double temp;
	if (endpoint.x == startpoint.x && endpoint.y == startpoint.y){
		return  Distance(startpoint, tagpoint);		
	}
	A = endpoint.x - startpoint.x;
	B = endpoint.y - startpoint.y;
	line = abs((double)(B * tagpoint.x - A * tagpoint.y + A * startpoint.y - B * startpoint.x));
	temp = (double)(A * A + B * B);
	dis = line / (sqrt(temp));

	return dis;

}	// end of double  CHYFontFunc::Dispointtoline()

//计算点到直线的距离
CHYPoint CHYFontFunc::cubicToQuadratic(CHYPoint p1, CHYPoint p2, CHYPoint p3, CHYPoint p4)
{
	CHYPoint q1, q2, q3;
	q1.x = (p1.x + 3 * p2.x) / 4;
	q1.y = (p1.y + 3 * p2.y) / 4;
	q2.x = (3 * p3.x + p4.x) / 4;
	q2.y = (3 * p3.y + p4.y) / 4;
	q3.x = (q1.x + q2.x) / 2;
	q3.y = (q1.y + q2.y) / 2;
	q3.flag = POINT_FLG_CONTROL;
	return q3;

}	// end of CHYPoint cubicToQuadratic()


int CHYFontFunc::Jiecheng(int n)
{
	int i;
	int result = 1;
	if (n == 0){
		result = 1;
	}
	else{
		for (i = 1; i <= n; i++){
			result = result * i;
		}
	}
	return result;
}  // end of double  CHYFontFunc::Jiecheng()

float CHYFontFunc::Chengfang(double x, int n)
{
	double result = 1;
	int i = 0;
	for (i = 0; i < n; i++){
		result = result * x;
	}
	return result;

} // end of double  CHYFontFunc::Chengfang()

int CHYFontFunc::Guass(double* A, double* X, double* B, int XN, int Guassflag)
{
	int i, j, k, maxk = 0;
	double maxA = 0;
	double aa = 0;
	double* tempA = new double[(XN + 1)];
	double* AB = new double[(XN + 1) * XN];
	double tempX = 0;
	double absA = 1;
	for (i = 0; i < XN; i++)
	{
		for (j = 0; j < XN; j++)
		{
			AB[i * (XN + 1) + j] = A[i * XN + j];
		}
		AB[i * (XN + 1) + XN] = B[i];
	}
	for (i = 0; i < XN - 1; i++)
	{
		maxA = abs(AB[i * (XN + 1) + i]);
		maxk = i;
		for (j = i; j < XN; j++)
		{
			if (abs(AB[j * (XN + 1) + i]) > maxA)
			{
				maxA = AB[j * (XN + 1) + i]; maxk = j;
			}
		}
		for (j = 0; j < (XN + 1); j++)//两行互换
		{
			tempA[j] = AB[i * (XN + 1) + j];
			AB[i * (XN + 1) + j] = AB[maxk * (XN + 1) + j];
			AB[maxk * (XN + 1) + j] = tempA[j];
		}
		for (j = i + 1; j < XN; j++)
		{
			if (AB[j * (XN + 1) + i] == 0)continue;
			aa = -AB[i * (XN + 1) + i] / AB[j * (XN + 1) + i];
			for (k = 0; k < (XN + 1); k++)
			{
				AB[j * (XN + 1) + k] = AB[j * (XN + 1) + k] * aa + AB[i * (XN + 1) + k];
			}
		}
	}
	for (i = 0; i < XN; i++)
	{
		aa = AB[i * (XN + 1) + i];
		for (j = 0; j < (XN + 1); j++)
		{
			AB[i * (XN + 1) + j] = AB[i * (XN + 1) + j] / aa;
		}
	}
	for (i = 0; i < XN; i++)
	{
		absA = absA * AB[i * (XN + 1) + i];
	}
	if (absA == 0)
	{
		Guassflag = -1;

	}
	else
	{
		X[XN - 1] = AB[(XN - 1) * (XN + 1) + XN];
		for (i = XN - 2; i >= 0; i--)
		{
			tempX = 0;
			for (j = i + 1; j < XN; j++)
			{
				tempX += AB[i * (XN + 1) + j] * X[j];
			}
			X[i] = AB[i * (XN + 1) + XN] - tempX;
		}
		Guassflag = 0;
	}

	if (AB)		delete[] AB;
	AB = NULL;
	if (tempA)	delete[] tempA;
	tempA = NULL;

	return Guassflag;

}  // end of double  CHYFontFunc::Guass()

//m 五点三次平滑的阶数，阶数越高，滤波后的信号越平滑但耗时也更长
void CHYFontFunc::Mean5_3(std::vector<CHYPoint>& invtPnt, std::vector<CHYPoint>& outvtPnt, int m)
{
	outvtPnt.clear();
	int nInLen = (int)invtPnt.size();
	if (nInLen < 4) {	
		outvtPnt = invtPnt;
		return;
	}

	outvtPnt.resize(nInLen);
	for (int i = 0; i < nInLen;i++) {
		outvtPnt[i].flag = 1;
	}
	
	for (int k = 0; k < m; k++) {
		outvtPnt[0].x = (69 * invtPnt[0].x + 4 * (invtPnt[1].x + invtPnt[3].x) - 6 * invtPnt[2].x - invtPnt[4].x)/70.0;
		outvtPnt[1].x = (2 * (invtPnt[0].x + invtPnt[4].x) + 27 * invtPnt[1].x + 12 * invtPnt[2].x - 8 * invtPnt[3].x)/35.0;
		for (int j = 2; j < nInLen- 2; j++) {
			outvtPnt[j].x = (-3 * (invtPnt[j - 2].x + invtPnt[j + 2].x) + 12 * (invtPnt[j - 1].x + invtPnt[j + 1].x) + 17 * invtPnt[j].x)/35.0;
		}
		outvtPnt[nInLen - 2].x = (2 * (invtPnt[nInLen - 1].x + invtPnt[nInLen - 5].x) + 27 * invtPnt[nInLen - 2].x + 12 * invtPnt[nInLen - 3].x - 8 * invtPnt[nInLen - 4].x)/35.0;
		outvtPnt[nInLen - 1].x = (69 * invtPnt[nInLen - 1].x + 4 * (invtPnt[nInLen - 2].x + invtPnt[nInLen - 4].x) - 6 * invtPnt[nInLen - 3].x - invtPnt[nInLen - 5].x)/70.0;
	}

	for (int k = 0; k < m; k++) {
		outvtPnt[0].y = (69 * invtPnt[0].y + 4 * (invtPnt[1].y + invtPnt[3].y) 
			- 6 * invtPnt[2].y - invtPnt[4].y) / 70.0;
		outvtPnt[1].y = (2 * (invtPnt[0].y + invtPnt[4].y) + 27 * invtPnt[1].y 
			+ 12 * invtPnt[2].y - 8 * invtPnt[3].y) / 35.0;
		for (int j = 2; j < nInLen - 2; j++) {
			outvtPnt[j].y = (-3 * (invtPnt[j - 2].y + invtPnt[j + 2].y) + 
				12 * (invtPnt[j - 1].y + invtPnt[j + 1].y) + 17 * invtPnt[j].y) / 35.0;
		}
		outvtPnt[nInLen - 2].y = (2 * (invtPnt[nInLen - 1].y + invtPnt[nInLen - 5].y) 
			+ 27 * invtPnt[nInLen - 2].y + 12 * invtPnt[nInLen - 3].y
			- 8 * invtPnt[nInLen - 4].y) / 35.0;
		outvtPnt[nInLen - 1].y = (69 * invtPnt[nInLen - 1].y + 4 * 
			(invtPnt[nInLen - 2].y + invtPnt[nInLen - 4].y) - 
			6 * invtPnt[nInLen - 3].y - invtPnt[nInLen - 5].y) / 70.0;
	}
	

}	// end of static void Mean5_3()

void CHYFontFunc::MeanContour(std::vector<CHYPoint>& ContourItemLst, std::vector<CHYPoint>& pHy)
{
	int i=0, j=0, st= (int)ContourItemLst.size();
	CHYPoint   Pt;	
	pHy.push_back(ContourItemLst[0]);
	for (j = 1; j < st-1; j++){
		CHYPoint& temp_Point1 = ContourItemLst[j - 1];
		CHYPoint& temp_Point2 = ContourItemLst[j];
		CHYPoint& temp_Point3 = ContourItemLst[j + 1];
		Pt.x = (temp_Point1.x + temp_Point2.x + temp_Point3.x) / 3;
		Pt.y = (temp_Point1.y + temp_Point2.y + temp_Point3.y) / 3;		
		pHy.push_back(Pt);
	}
	pHy.push_back(ContourItemLst[st - 1]);

}// end of double  CHYFontFunc::MeanContour()

int  CHYFontFunc::isSCurve(std::vector<CHYPoint>& temp_PointList)
{	
	CHYPoint temp_HYPoint;
	CHYPoint  m_point1;
	CHYPoint  m_point2;
	CHYPoint  pt1, pt2;
	double k1, b1;
	m_point1 = temp_PointList[0];
	m_point2 = temp_PointList[temp_PointList.size() - 1];	
	
	if ((m_point2.x - m_point1.x) != 0){
		k1 = (m_point2.y - m_point1.y) / (m_point2.x - m_point1.x);
		b1 = m_point1.y - k1 * m_point1.x;
	}
	else{
		k1 = 100; b1 = 100;
	}
	
	int istTmpPnts = temp_PointList.size();
	pt1 = temp_PointList[1];
	if (istTmpPnts < 4){
		return 0;
	}
	for (int i = 2; i < istTmpPnts - 1; i++){		
		pt2 = temp_PointList[i];
		if (FindJLinetoLine(k1, b1, m_point1, pt1, pt2) == 1){
			return 1;
		}
	}
	return 0;
}// end of double  CHYFontFunc::isSCurve()

//直线与直线相交的交点
int CHYFontFunc::FindJLinetoLine(double k1, double b1, CHYPoint& Point11, CHYPoint& Point21, CHYPoint& Point22)
{
	CHYPoint        Jtemp_HYPoint;
	CHYPoint        Jtemp_Point;
	CHYPoint        jpoint;

	HYPOINTF		Pt;
	Pt.x = 0;
	Pt.y = 0;

	double          xu1, xd1, yu1, yd1, xu2, xd2, yu2, yd2;
	double          k2, b2;
	double          y0, y1;
	double          xs, xe;
	double          ys, ye;
	if (Point21.x > Point22.x){
		xu2 = Point21.x; xd2 = Point22.x;
	}
	else{
		xu2 = Point22.x; xd2 = Point21.x;
	}
	if (Point21.y > Point22.y)	{
		yu2 = Point21.y; yd2 = Point22.y;
	}
	else{
		yu2 = Point22.y; yd2 = Point21.y;
	}
	if (Point22.x - Point21.x != 0){
		k2 = (double)(Point22.y - Point21.y) / (Point22.x - Point21.x);
		b2 = (double)Point22.y - k2 * Point22.x;
	}
	else{
		k2 = 100; b2 = 100;
	}
	if (k1 != k2){
		if (k1 == 100 || abs(k1) > 50){
			Pt.x = Point11.x;
			Pt.x = k2 * Pt.x + b2;
		}
		else if (k2 == 100 || abs(k2) > 50)	{
			Pt.x = Point21.x;
			Pt.y = k1 * Pt.x + b1;
		}
		else if (abs(k1) < 0.1 && k2 != 0){
			Pt.y = Point11.y;
			Pt.x = (Pt.y - b2) / k2;
		}
		else if (abs(k2) < 0.1 && k1 != 0){
			Pt.y = Point21.y;
			Pt.x = (Pt.y - b1) / k1;
		}
		else{
			Pt.x = (b2 - b1) / (k1 - k2);
			y0 = k1 * Pt.x + b1;
			y1 = k2 * Pt.x + b2;
			if (abs(k1 - 1) <= abs(k2 - 1))	{
				Pt.y = y0;
			}
			else{
				Pt.y = y1;
			}
		}
		jpoint.x = Pt.x;
		jpoint.y = Pt.y;
		jpoint.flag = POINT_FLG_ANCHOR;
		if ((jpoint.y >= yd2 && jpoint.y <= yu2 && jpoint.x >= xd2 && jpoint.x <= xu2)){
			return 1;
		}
		else{
			return 0;
		}
	}
	else{
		return 0;
	}

}// end of double  CHYFontFunc::FindJLinetoLine()

//bezier检验函数，如果点在bezier曲线上返回true否则返回false
int  CHYFontFunc::bezierntest(std::vector<CHYPoint>& temp_PointList01, std::vector<CHYPoint>& toPointlist, int BN, int bflag, double value)
{
	int    NN = (int)temp_PointList01.size();
	float ts = 0,te = 1, d = 0, ds = 0, de = 0,t = 0, x = 0, y = 0;
	int size = 0;
	CHYPoint m_point1;
	CHYPoint m_point2;
	CHYPoint temp_point;
	CHYPoint* toPoint = new CHYPoint[BN - 1];

	m_point1 = temp_PointList01[0];
	m_point2 = temp_PointList01[NN - 1];	
	for (int i = 1; i < NN - 1; i++){
		int countj = 1;
		ts = 0;
		te = 1;
		temp_point = temp_PointList01[i];		
		t = (ts + te) / 2;
		x = Chengfang(1 - t, BN) * m_point1.x + Chengfang(t, BN) * m_point2.x;
		y = Chengfang(1 - t, BN) * m_point1.y + Chengfang(t, BN) * m_point2.y;
		for (int j = 1; j < BN; j++){			
			toPoint[j - 1] = toPointlist[j - 1];
			x = x + (float)(Jiecheng(BN) / (Jiecheng(j) * Jiecheng(BN - j))) * Chengfang(1 - t, BN - j) * Chengfang(t, j) * toPoint[j - 1].x;
			y = y + (float)(Jiecheng(BN) / (Jiecheng(j) * Jiecheng(BN - j))) * Chengfang(1 - t, BN - j) * Chengfang(t, j) * toPoint[j - 1].y;
		}
		d = sqrt((x - temp_point.x) * (x - temp_point.x) + (y - temp_point.y) * (y - temp_point.y));
		ds = Distance(m_point1, temp_point);
		de = Distance(m_point2, temp_point);
		while (countj < 20){
			if (ds <= de){
				te = t;
				de = d;
			}
			else{
				ts = t;
				ds = d;
			}
			t = (ts + te) / 2;
			x = Chengfang(1 - t, BN) * m_point1.x + Chengfang(t, BN) * m_point2.x;
			y = Chengfang(1 - t, BN) * m_point1.y + Chengfang(t, BN) * m_point2.y;
			for (int j = 1; j < BN; j++){				
				toPoint[j - 1] = toPointlist[j - 1];
				x = x + (float)(Jiecheng(BN) / (Jiecheng(j) * Jiecheng(BN - j))) * Chengfang(1 - t, BN - j) * Chengfang(t, j) * toPoint[j - 1].x;
				y = y + (float)(Jiecheng(BN) / (Jiecheng(j) * Jiecheng(BN - j))) * Chengfang(1 - t, BN - j) * Chengfang(t, j) * toPoint[j - 1].y;
			}
			d = sqrt((x - temp_point.x) * (x - temp_point.x) + (y - temp_point.y) * (y - temp_point.y));
			countj = countj + 1;
		}

		if (d > value){
			bflag = -1;
			break;
		}
	}

	if (toPoint)
		delete[] toPoint;
	toPoint = NULL;

	return bflag;

}// end of double  CHYFontFunc::bezierntest()