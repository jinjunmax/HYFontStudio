#pragma once

using namespace HYFONTCODEC;
#include "HYFontCodecHead.h"

class HY_DLL_CLASS CHYFontFunc
{
public:
	CHYFontFunc();
	~CHYFontFunc();

	// 将两套字形数据合并为一套,如果编码或psname重复后边覆盖前边
	static void MergeGlyphs(std::vector<CHYGlyph>& src1Glyphs, std::vector<CHYGlyph>& src2Glyphs);
	// 通过字符name确定GID
	static int FindGryphIndexByGlyName(std::vector<CHYGlyph>& InGlyphs, string strGName);
	// 通过字符unicode编码确定GID
	static int FindGryphIndexByUnciode(std::vector<CHYGlyph>& InGlyphs, unsigned long ulUnicode);
	// 通过postcript文件提取子集，outHYGlyphs会先被清空，所以不需要考虑outHYGlyphs中是否会有重复字符
	static void GetSubsetbyName(std::vector<CHYGlyph>& InGlyphs, std::vector<string>& names, std::vector<CHYGlyph>& outHYGlyphs);
	// 通过unicode编码提取子集，outHYGlyphs不会被清空，bOverlap决定是否新字形覆盖旧字形
	static void GetSubsetbyUni(std::vector<CHYGlyph>& InGlyphs, std::vector<unsigned long>& uni, std::vector<CHYGlyph>& outHYGlyphs, bool bOverlap=TRUE);
	// 计算两点之间的直线距离整形
	static double Distance(const CHYPoint& p1, const CHYPoint& p2);
	// 计算两点之间的直线距离浮点型
	static double DistanceF(const HYPOINTF& p1, const HYPOINTF& p2);
	// 计算三点之间的夹角
	static double AnglePoint(const CHYPoint& StartPt, const CHYPoint& MidPt, const CHYPoint& EndPt);
	// 获取贝塞尔曲线中心点
	static HYPOINTF GetBezierCenter(std::vector<CHYPoint> vtBPoints, int sPoints);
	// 求贝塞尔曲线弧长（近似值), t采样频率，(最高支持到三次)。
	static double GetBezierLength(std::vector<CHYPoint> vtBPoints, int sPoints=50);
	// 求贝塞尔曲线采样点
	static HYPOINTF GetBezierPos(double t, std::vector<CHYPoint>& vtBPoints);
	// 已知圆弧三点求半径和圆点坐标
	static void GetCircularF(HYPOINTF P1, HYPOINTF P2, HYPOINTF P3, float& R, HYPOINTF PCenter);
	// 通过圆点和曲线的两个端点获取三次贝塞尔曲线的两个线外点。
	static void GetBzrOffPnt(HYPOINTF P1, HYPOINTF P2, HYPOINTF PCenter, HYPOINTF& C1, HYPOINTF& C2);
	// 获取两点间的斜率
	static double GetSlope(HYPOINTF P1, HYPOINTF P2);
	// 获取两点间的斜率
	static double GetSlope(CHYPoint& P1, CHYPoint& P2);
	// 获取中心点
	static CHYPoint GetCenter(std::vector<CHYPoint> vtBPoints);
	// 贝塞尔拟合
	static void BezierFitting(std::vector<CHYPoint>& InvtPnt, std::vector<CHYPoint>& OutvtPnt, int BzrTyp, double value, double Delta);
	// 对线段进行平滑拟合
	static void SmoothFitting(std::vector<CHYPoint>& InvtPnt, std::vector<CHYPoint>& OutvtPnt);

	static void		bezierS2_2(std::vector<CHYPoint>& temp_PointList, std::vector<CHYPoint>& toPointlist, double value, int BezierN, double Delta);
	static void		bezierS(std::vector<CHYPoint>& InPnts, std::vector<CHYPoint>& outPnts, double value, int BezierN);
	static int		beziern(std::vector<CHYPoint>& temp_PointList, std::vector<CHYPoint>& toPointlist, int BN, double value);
	static int		Guass(double* A, double* X, double* B, int XN, int Guassflag);
	
	static float	Chengfang(double x, int n);
	static int		Jiecheng(int n);

	// 5点3次平滑
	static void		Mean5_3(std::vector<CHYPoint>& invtPnt, std::vector<CHYPoint>& outvtPnt, int m);
	static void		MeanContour(std::vector<CHYPoint>& ContourItemLst, std::vector<CHYPoint>& pHy);
	static int		isSCurve(std::vector<CHYPoint>& temp_PointList);
	static int		FindJLinetoLine(double k1, double b1, CHYPoint& Point11, CHYPoint& Point21, CHYPoint& Point22);
	static int		bezierntest(std::vector<CHYPoint>& temp_PointList01, std::vector<CHYPoint>& toPointlist, int BN, int bflag, double m_value);
	//计算点到直线的距离
	static double	DispointToLine(CHYPoint& startpoint, CHYPoint& endpoint, CHYPoint& tagpoint);

	// 两次贝塞尔to三次贝塞尔
	static CHYPoint cubicToQuadratic(CHYPoint p1, CHYPoint p2, CHYPoint p3, CHYPoint p4);
};

