#pragma once

using namespace HYFONTCODEC;
#include "HYFontCodecHead.h"

class HY_DLL_CLASS CHYFontFunc
{
public:
	CHYFontFunc();
	~CHYFontFunc();

	// �������������ݺϲ�Ϊһ��,��������psname�ظ���߸���ǰ��
	static void MergeGlyphs(std::vector<CHYGlyph>& src1Glyphs, std::vector<CHYGlyph>& src2Glyphs);
	// ͨ���ַ�nameȷ��GID
	static int FindGryphIndexByGlyName(std::vector<CHYGlyph>& InGlyphs, string strGName);
	// ͨ���ַ�unicode����ȷ��GID
	static int FindGryphIndexByUnciode(std::vector<CHYGlyph>& InGlyphs, unsigned long ulUnicode);
	// ͨ��postcript�ļ���ȡ�Ӽ���outHYGlyphs���ȱ���գ����Բ���Ҫ����outHYGlyphs���Ƿ�����ظ��ַ�
	static void GetSubsetbyName(std::vector<CHYGlyph>& InGlyphs, std::vector<string>& names, std::vector<CHYGlyph>& outHYGlyphs);
	// ͨ��unicode������ȡ�Ӽ���outHYGlyphs���ᱻ��գ�bOverlap�����Ƿ������θ��Ǿ�����
	static void GetSubsetbyUni(std::vector<CHYGlyph>& InGlyphs, std::vector<unsigned long>& uni, std::vector<CHYGlyph>& outHYGlyphs, bool bOverlap=TRUE);
	// ��������֮���ֱ�߾�������
	static double Distance(const CHYPoint& p1, const CHYPoint& p2);
	// ��������֮���ֱ�߾��븡����
	static double DistanceF(const HYPOINTF& p1, const HYPOINTF& p2);
	// ��������֮��ļн�
	static double AnglePoint(const CHYPoint& StartPt, const CHYPoint& MidPt, const CHYPoint& EndPt);
	// ��ȡ�������������ĵ�
	static HYPOINTF GetBezierCenter(std::vector<CHYPoint> vtBPoints, int sPoints);
	// ���������߻���������ֵ), t����Ƶ�ʣ�(���֧�ֵ�����)��
	static double GetBezierLength(std::vector<CHYPoint> vtBPoints, int sPoints=50);
	// ���������߲�����
	static HYPOINTF GetBezierPos(double t, std::vector<CHYPoint>& vtBPoints);
	// ��֪Բ��������뾶��Բ������
	static void GetCircularF(HYPOINTF P1, HYPOINTF P2, HYPOINTF P3, float& R, HYPOINTF PCenter);
	// ͨ��Բ������ߵ������˵��ȡ���α��������ߵ���������㡣
	static void GetBzrOffPnt(HYPOINTF P1, HYPOINTF P2, HYPOINTF PCenter, HYPOINTF& C1, HYPOINTF& C2);
	// ��ȡ������б��
	static double GetSlope(HYPOINTF P1, HYPOINTF P2);
	// ��ȡ������б��
	static double GetSlope(CHYPoint& P1, CHYPoint& P2);
	// ��ȡ���ĵ�
	static CHYPoint GetCenter(std::vector<CHYPoint> vtBPoints);
	// ���������
	static void BezierFitting(std::vector<CHYPoint>& InvtPnt, std::vector<CHYPoint>& OutvtPnt, int BzrTyp, double value, double Delta);
	// ���߶ν���ƽ�����
	static void SmoothFitting(std::vector<CHYPoint>& InvtPnt, std::vector<CHYPoint>& OutvtPnt);

	static void		bezierS2_2(std::vector<CHYPoint>& temp_PointList, std::vector<CHYPoint>& toPointlist, double value, int BezierN, double Delta);
	static void		bezierS(std::vector<CHYPoint>& InPnts, std::vector<CHYPoint>& outPnts, double value, int BezierN);
	static int		beziern(std::vector<CHYPoint>& temp_PointList, std::vector<CHYPoint>& toPointlist, int BN, double value);
	static int		Guass(double* A, double* X, double* B, int XN, int Guassflag);
	
	static float	Chengfang(double x, int n);
	static int		Jiecheng(int n);

	// 5��3��ƽ��
	static void		Mean5_3(std::vector<CHYPoint>& invtPnt, std::vector<CHYPoint>& outvtPnt, int m);
	static void		MeanContour(std::vector<CHYPoint>& ContourItemLst, std::vector<CHYPoint>& pHy);
	static int		isSCurve(std::vector<CHYPoint>& temp_PointList);
	static int		FindJLinetoLine(double k1, double b1, CHYPoint& Point11, CHYPoint& Point21, CHYPoint& Point22);
	static int		bezierntest(std::vector<CHYPoint>& temp_PointList01, std::vector<CHYPoint>& toPointlist, int BN, int bflag, double m_value);
	//����㵽ֱ�ߵľ���
	static double	DispointToLine(CHYPoint& startpoint, CHYPoint& endpoint, CHYPoint& tagpoint);

	// ���α�����to���α�����
	static CHYPoint cubicToQuadratic(CHYPoint p1, CHYPoint p2, CHYPoint p3, CHYPoint p4);
};

