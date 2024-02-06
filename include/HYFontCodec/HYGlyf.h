#pragma once
#include "HYFontCodecDef.h"

#include <vector>
using namespace std;
//////////////////////////////////////////////////////////////////////////////////
//
namespace HYFONTCODEC
{
	class HY_DLL_CLASS  CHYPoint
	{
	public:
		CHYPoint(void);
		~CHYPoint(void);	

		void SetDefault();
		CHYPoint& operator=(const CHYPoint& other);

	public:
		int		x;
		int		y;
		int		flag;
	};

	class HY_DLL_CLASS CHYContour
	{
	public:
		CHYContour(void);
		~CHYContour(void);	

		void SetDefault();
		CHYContour& operator=(const CHYContour& other);

		// 过滤的位置和属性相同的点。
		void Filter();

	public:
		std::vector<CHYPoint>	vtHYPoints;
	};

	class HY_DLL_CLASS CHYCmpst_Vector
	{
	public:
		CHYCmpst_Vector();
		~CHYCmpst_Vector();

		void SetDefault();
		CHYCmpst_Vector& operator=(const CHYCmpst_Vector& other);
	public:
		F2Dot14			xscale;
		F2Dot14			yscale;		
	};

	class HY_DLL_CLASS CHYCmpst_Tensor
	{
	public:
		CHYCmpst_Tensor();
		~CHYCmpst_Tensor();

		void SetDefault();
		CHYCmpst_Tensor& operator=(const CHYCmpst_Tensor& other);

	public:
		F2Dot14			xscale;
		F2Dot14			scale01;
		F2Dot14			scale10;
		F2Dot14			yscale;
	};

	class HY_DLL_CLASS CHYCmpst
	{
	public:
		CHYCmpst();
		~CHYCmpst();

		void SetDefault();
		CHYCmpst& operator=(const CHYCmpst& other);
	public:
		USHORT				flags;
		USHORT				glyphIndex;
		SHORT				args[2];       
		F2Dot14				scale;
		CHYCmpst_Vector		vctr;
		CHYCmpst_Tensor		tensor;
	};

	class HY_DLL_CLASS CHYGlyph
	{
	public:
		CHYGlyph(void);
		~CHYGlyph(void);

		void SetDefault();
		CHYGlyph& operator=(const CHYGlyph& other);

	public:
		int								GetTotalPointsNumber();
		void							CountBoundBox();
		bool							GetCountourBox(size_t iConturIndex, int& minX, int& minY, int & maxX, int& maxY);
		bool							FindUnicode(unsigned long Uni);

	public: 
		short							minX;
		short							minY;
		short							maxX;
		short							maxY;
		short							sContourNums;

		int								fontFlag;
		std::string						strPostName;
		int								advanceWidth;
		int								advanceHeight;
		short							topSB;	
		std::vector<unsigned long>		vtUnicode;	
		std::vector<CHYContour>			vtContour;
		std::vector<unsigned char>		vtInstruntions;	
		std::vector<CHYCmpst>			vtComponents;

		std::vector<CHYPoint>			vtPts;
		std::vector<unsigned short>		vtendPtsofContours;

	private:
		int							modulo(int x, size_t mod);
		double	const				Nan();
		double	const				Infinity();
		double	const				granularity();
		bool						IsNan (double x);
		double						SignX(double x);
		double						Binomial (double x, double n);
		void						Quadratic (double a, double b, double c, double &root1, double &root2);
		std::pair <double, double>	BezierInflection (double a0, double a1, double a2, double a3);
		double						BezierEval (double a0, double a1, double a2, double a3, double t);
		double						Greater (double original, double x);
		double						Less (double original, double x);
		void						Report3Bezier(RECT &rect, double ax, double ay, double bx, double by, double cx, double cy, double dx, double dy);
		void						ReportBounds(RECT &bounds, CHYPoint &pos);
	};
}