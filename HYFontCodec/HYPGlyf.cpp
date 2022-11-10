#include "StdAfx.h"
#include "HYGlyf.h"
#include "math.h"

namespace HYFONTCODEC
{
	CHYPoint::CHYPoint(void)
	{
		x		= 0;
		y		= 0;
		flag	= POINT_FLG_UNKNOW;

	}	// end of CHYPoint::CHYPoint()

	CHYPoint::~CHYPoint(void)
	{

	}	// end of CHYPoint::~CHYPoint()

	void CHYPoint::SetDefault()
	{
		x		= 0;
		y		= 0;
		flag	= POINT_FLG_UNKNOW;

	}	// end of void CHYPoint::SetDefault()

	CHYPoint& CHYPoint::operator=(const CHYPoint& other)
	{
		if (this != &other)
		{
			x		= other.x;
			y		= other.y;
			flag	= other.flag;
		}

		return *this;

	}	// end of CHYPoint& CHYPoint::operator=()

	CHYContour::CHYContour(void)
	{	
		

	}	// end of CHYContour::CHYContour()

	CHYContour::~CHYContour(void)
	{
		vtHYPoints.clear();

	}	// end of CHYContour::~CHYContour()

	void CHYContour::SetDefault()
	{
		vtHYPoints.clear();	

	}	// end of void CHYContour::SetDefault()		

	CHYContour& CHYContour::operator=(const CHYContour& other)
	{
		if (this != &other)
		{
			vtHYPoints = other.vtHYPoints;
		}

		return *this;

	}	// end of CHYContour& CHYContour::operator=()

	void CHYContour::Filter()
	{
		std::vector<CHYPoint>	vtTmpPoints;
		size_t st=0;
		while (st<vtHYPoints.size())
		{
			CHYPoint ptTmp1 = vtHYPoints[st];			
			size_t st1=st+1;
			while (st1<vtHYPoints.size())
			{
				CHYPoint ptTmp2 = vtHYPoints[st1];
				if (ptTmp1.x==ptTmp2.x && ptTmp1.y==ptTmp2.y && ptTmp1.flag==ptTmp2.flag)				
				{
					vtHYPoints.erase(vtHYPoints.begin()+st1);					
					break;
				}
				st1++;
			}

			vtTmpPoints.push_back(ptTmp1);
			st++;
		}

		vtHYPoints.clear();
		vtHYPoints = vtTmpPoints;

	}	// end of void CHYContour::Filter()

	CHYCmpst_Vector::CHYCmpst_Vector()
	{
		xscale = 0;
		yscale = 0;

	}	// end of CHYCmpst_Vector::CHYCmpst_Vector()

	CHYCmpst_Vector::~CHYCmpst_Vector()
	{

	}	// end of CHYCmpst_Vector::CHYCmpst_Vector()

	void CHYCmpst_Vector::SetDefault()
	{
		xscale = 0;
		yscale = 0;

	}	// end of void CHYCmpst_Vector::SetDefault()

	CHYCmpst_Vector& CHYCmpst_Vector::operator=(const CHYCmpst_Vector& other)
	{

		if (this != &other)
		{
			xscale = other.xscale;
			yscale = other.yscale;
		}

		return *this;

	}	// end of CHYCmpst_Vector& CHYCmpst_Vector::operator=()

	CHYCmpst_Tensor::CHYCmpst_Tensor()
	{
		xscale		= 0;
		scale01		= 0;
		scale10		= 0;
		yscale		= 0;

	}	// end of CHYCmpst_Tensor::CHYCmpst_Tensor()

	CHYCmpst_Tensor::~CHYCmpst_Tensor()
	{

	}	// end of CHYCmpst_Tensor::~CHYCmpst_Tensor()

	void CHYCmpst_Tensor::SetDefault()
	{
		xscale		= 0.0;
		scale01		= 0;
		scale10		= 0;
		yscale		= 0.0;

	}	// end of void CHYCmpst_Tensor::SetDefault()

	CHYCmpst_Tensor& CHYCmpst_Tensor::operator=(const CHYCmpst_Tensor& other)
	{
		if (this != &other)
		{
			xscale		= other.xscale;
			scale01		= other.scale01;
			scale10		= other.scale10;
			yscale		= other.yscale;
		}

		return *this;

	}	// end of CHYCmpst_Tensor& CHYCmpst_Tensor::operator=()

	CHYCmpst::CHYCmpst()
	{
		flags		= 0;
		glyphIndex	= 0;
		args[0]		= 0;       
		args[1]		= 0;
		scale		= 1.0;

	}	// end of CHYCmpst::CHYCmpst()

	CHYCmpst::~CHYCmpst()
	{

	}	// end of CHYCmpst::~CHYCmpst()

	void CHYCmpst::SetDefault()
	{
		flags		= 0;
		glyphIndex	= 0;
		args[0]		= 0;       
		args[1]		= 0;
		scale		= 1.0;

	}	// end of void CHYCmpst::SetDefault()

	CHYCmpst& CHYCmpst::operator=(const CHYCmpst& other)
	{
		if (this != &other)
		{
			flags		= other.flags;
			glyphIndex	= other.glyphIndex;
			args[0]		= other.args[0];       
			args[1]		= other.args[1];
			scale		= other.scale;
		}

		return *this;

	}	// end of CHYCmpst& CHYCmpst::operator=()

	CHYGlyph::CHYGlyph(void)
	{
		minX			= 0;
		minY			= 0;
		maxX			= 0;
		maxY			= 0;
		fontFlag		= FONTTYPE_UNKNOWN;
		glyfType		= GLYF_TYPE_UNKNOW;
		advanceWidth	= 0;
		advanceHeight	= 0;
		topSB			= 0;	
		strPostName     = "";

	}	// end of CHYGlyph::~CHYGlyph()

	CHYGlyph::~CHYGlyph(void)
	{
		vtUnicode.clear();
		vtContour.clear();
		vtInstruntions.clear();
		vtComponents.clear();

	}	// end of CHYGlyph::~CHYGlyph()

	void CHYGlyph::SetDefault()
	{
		minX			= 0;
		minY			= 0;
		maxX			= 0;
		maxY			= 0;
		fontFlag		= FONTTYPE_UNKNOWN;
		glyfType		= GLYF_TYPE_UNKNOW;
		advanceWidth	= 0;
		advanceHeight	= 0;
		topSB			= 0;
		strPostName		= "";	

		vtUnicode.clear();
		vtContour.clear();
		vtInstruntions.clear();
		vtComponents.clear();
		vtPts.clear();
		vtendPtsofContours.clear();

	}	// end of void CHYGlyph::SetDefault()

	CHYGlyph& CHYGlyph::operator=(const CHYGlyph& other)
	{
		if (this != &other) 
		{ 

			minX			= other.minX;
			minY			= other.minY;
			maxX			= other.maxX;
			maxY			= other.maxY;
			fontFlag		= other.fontFlag;
			glyfType		= other.glyfType;
			advanceWidth	= other.advanceWidth;
			advanceHeight	= other.advanceHeight;
			topSB			= other.topSB;
			strPostName		= other.strPostName;
			vtUnicode		= other.vtUnicode;
			vtContour		= other.vtContour;
			vtInstruntions  = other.vtInstruntions;
			vtComponents	= other.vtComponents;	
			vtPts			= other.vtPts;
			vtendPtsofContours = other.vtendPtsofContours;
		}

		return *this;

	}	// end of CHYGlyph& operator=()


	int	CHYGlyph::GetTotalPointsNumber()
	{
		int iTotalPoints = 0;

		for (size_t i=0; i<vtContour.size(); i++)
		{
			CHYContour&		ContourItem = vtContour[i];		
			iTotalPoints+=(int)ContourItem.vtHYPoints.size();		
		}

		return iTotalPoints;

	}	// end of int CHYGlyph::GetTotalPointsNumber()

	void CHYGlyph::CountBoundBox()
	{
		if(vtContour.size() == 0){
			minX = 0;
			minY = 0;
			maxX = 0;
			maxY = 0;
			return;
		}

		int i		= 0;
		int j		= 0;

		minX = 32767;
		minY = 32767;
		maxX = -32768;
		maxY = -32768;

		if (fontFlag == FONTTYPE_OTF){
			RECT		rc;
			rc.left		= minX;
			rc.right	= maxX;
			rc.top		= maxY;
			rc.bottom	= minY;			

			while (i<vtContour.size()){
				j = 0;
				double ax=0.0, ay=0.0, bx=0.0, by=0.0, cx=0.0, cy=0.0, dx=0.0, dy=0.0;

				CHYContour&		ContourItem = vtContour[i];				
				while (j<ContourItem.vtHYPoints.size()){
					CHYPoint& HYPoint = ContourItem.vtHYPoints[j];
					if	(HYPoint.flag == POINT_FLG_ANCHOR){
						ReportBounds(rc,HYPoint);
					}
					else{
						if (j-1<0 ) break;
						if (j+2>=ContourItem.vtHYPoints.size()) break;

						ax = ContourItem.vtHYPoints[j-1].x;
						ay = ContourItem.vtHYPoints[j-1].y;
						bx = ContourItem.vtHYPoints[j].x;
						by = ContourItem.vtHYPoints[j].y;
						cx = ContourItem.vtHYPoints[++j].x;
						cy = ContourItem.vtHYPoints[j].y;
						dx = ContourItem.vtHYPoints[++j].x;
						dy = ContourItem.vtHYPoints[j].y;

						Report3Bezier(rc,ax,ay,bx,by,cx,cy,dx,dy);
						ReportBounds(rc,ContourItem.vtHYPoints[j]);
					}
					j++;
				}
				i++;
			}
			minX	= (short)rc.left;
			maxX	= (short)rc.right;
			maxY	= (short)rc.top;
			minY	= (short)rc.bottom;
		}	

		if (fontFlag == FONTTYPE_TTF){
			if (glyfType == GLYF_TYPE_SIMPLE){
				size_t CntuNums = vtContour.size();
				for (size_t i=0; i<CntuNums; i++){
					CHYContour& cntur = vtContour[i];
					std::vector<CHYPoint>& vtPoints = cntur.vtHYPoints;
					size_t pointNums = vtPoints.size();

					for (size_t j=0; j<pointNums; j++){
						CHYPoint&	HYPoint = vtPoints[j];
						if (HYPoint.x>maxX){
							maxX = (short)::HY_RealRount(HYPoint.x);			
						}
						if (HYPoint.x< minX){				
							minX = (short)::HY_RealRount(HYPoint.x);				
						}
						if (HYPoint.y > maxY){
							maxY = (short)::HY_RealRount(HYPoint.y);				
						}
						if (HYPoint.y < minY){
							minY = (short)::HY_RealRount(HYPoint.y);					
						}
					}
				}
			}			
		}

	}	// end of void	CHYGlyph::CountBoundBox()

	bool CHYGlyph::GetCountourBox(size_t iConturIndex, int& minX, int& minY, int & maxX, int& maxY)
	{			
		if (this->glyfType == GLYF_TYPE_SIMPLE){
			if (iConturIndex>=vtContour.size()) return false;

			CHYContour& cntur = vtContour[iConturIndex];
			std::vector<CHYPoint>& vtPoints = cntur.vtHYPoints;
			size_t pointNums = vtPoints.size();

			for (size_t j=0; j<pointNums; j++){
				CHYPoint&	HYPoint = vtPoints[j];
				if (HYPoint.x>maxX)	{
					maxX = (short)::HY_RealRount(HYPoint.x);			
				}
				if (HYPoint.x< minX){				
					minX = (short)::HY_RealRount(HYPoint.x);				
				}
				if (HYPoint.y > maxY){
					maxY = (short)::HY_RealRount(HYPoint.y);				
				}
				if (HYPoint.y < minY){
					minY = (short)::HY_RealRount(HYPoint.y);					
				}
			}
		}

		if (fontFlag == FONTTYPE_OTF){
			RECT		rc;
			rc.left		= minX;
			rc.right	= maxX;
			rc.top		= maxY;
			rc.bottom	= minY;			

			if (iConturIndex>=vtContour.size()) return false;			
			
			int j = 0;
			double ax=0.0, ay=0.0, bx=0.0, by=0.0, cx=0.0, cy=0.0, dx=0.0, dy=0.0;

			CHYContour&		ContourItem = vtContour[iConturIndex];				
			while ( j<ContourItem.vtHYPoints.size() ){
				CHYPoint& HYPoint = ContourItem.vtHYPoints[j];
				if	(HYPoint.flag == POINT_FLG_ANCHOR){
					ReportBounds(rc,HYPoint);
				}
				else{
					if (j-1<0 ) break;
					if (j+2>=ContourItem.vtHYPoints.size()) break;

					ax = ContourItem.vtHYPoints[j-1].x;
					ay = ContourItem.vtHYPoints[j-1].y;
					bx = ContourItem.vtHYPoints[j].x;
					by = ContourItem.vtHYPoints[j].y;
					cx = ContourItem.vtHYPoints[++j].x;
					cy = ContourItem.vtHYPoints[j].y;
					dx = ContourItem.vtHYPoints[++j].x;
					dy = ContourItem.vtHYPoints[j].y;

					Report3Bezier(rc,ax,ay,bx,by,cx,cy,dx,dy);
					ReportBounds(rc,ContourItem.vtHYPoints[j]);
				}
				j++;
			}
			minX	= (short)rc.left;
			maxX	= (short)rc.right;
			maxY	= (short)rc.top;
			minY	= (short)rc.bottom;
		}	

		return true;

	}	// end of void CHYGlyph::GetCountourBox()

	bool CHYGlyph::FindUnicode(unsigned long Uni)
	{
		size_t st = vtUnicode.size();
		for (size_t i = 0; i < st; i++){
			if (Uni == vtUnicode[i])
				return true;
		}

		return false;

	}	// end of bool	CHYGlyph::FindUnicode()

	int	CHYGlyph::modulo(int x, size_t mod)
	{
		while (x < 0)
			x += static_cast <int> (mod);

		while (x >= static_cast <int> (mod))
			x -= static_cast <int> (mod);

		return x;

	}	// end of int	CHYGlyph::modulo()

	double const CHYGlyph::Nan()
	{
		return sqrt(-1.0);

	}	// end of double const CHYGlyph::Nan()

	double const CHYGlyph::Infinity()
	{
		return tan(3.1415926535897932384626/2.0);

	}	// end of double const CHYGlyph::Infinity()

	double const CHYGlyph::granularity()
	{
		return 2.2204460493e-16;

	}	// end of double const CHYGlyph::granularity()

	bool CHYGlyph::IsNan (double x)
	{
		return !(x == x);

	}	// end of bool CHYGlyph::IsNan()

	double	CHYGlyph::SignX(double x)
	{
		return x < 0.0? -1.0: 1.0;

	}	// end of double	CHYGlyph::SignX()

	double	CHYGlyph::Binomial (double x, double n)
	{
		/* Compute (1+x)^n - 1 for x near 0 by binomial theorem */

		double		iterator,
			factorial,
			nfact,
			niterator,
			xpower,
			sum,
			lastsum,
			sign,
			nsign,
			toggle,
			delta,
			lastDelta;

		if (IsNan (x + n) || x < -1.0)
			sum = Nan();
		else if (x*x > 0.025)
			sum = exp (n * log (1.0 + x)) - 1.0;
		else
		{
			nsign = SignX (n);
			sign = nsign;
			toggle = sign;
			factorial = 1.0;
			iterator = 1.0;
			lastsum = 0.0;
			xpower = x;
			nfact = n;
			niterator = n;
			sum = toggle * x * n;
			lastDelta = Infinity();

			while (lastsum != (sum += (delta = ((toggle *= sign) * (xpower *= x) * (nfact *= (niterator -= nsign)) /
				(factorial *= (iterator += 1.0))))))
			{
				lastsum = sum;

				if (delta * delta >= lastDelta * lastDelta)
					break;

				lastDelta = delta;
			}
		}

		return sum;

	}	// end of   double	CHYGlyph::Binomial ()

	void	CHYGlyph::Quadratic (double a, double b, double c, double &root1, double &root2)
	{
		double		determinant, y,	factor;

		if (a == 0.0)
		{
			if (b == 0.0)
			{
				root1 = Nan();
				root2 = Nan();
			} 
			else
			{
				root1 = -c/b;
				root2 = root1;
			}
		} 
		else if (b == 0.0)
		{
			root1 = sqrt (-a * c) / a;
			root2 = -root1;
		} else 
		{
			factor = 0.5 * b / a;
			y = -4.0 * a * c / (b * b);

			if (abs(y) < 0.05)
			{
				determinant = Binomial(y, 0.5);
				root1 = determinant;
				root2 = -2.0 - determinant;
			}
			else
			{
				determinant = 1.0 + y;

				if (determinant < -100.0 * granularity())
					determinant = Nan();
				else if (determinant < 0.0)
					determinant = 0.0;
				else
					determinant = sqrt (determinant);

				root1 = -1.0 + determinant;
				root2 = -1.0 - determinant;
			}

			root1 *= factor;
			root2 *= factor;
		}

	}	// end of void	CHYGlyph::Quadratic ()

	//	Return the parameter values of t at which the bezier curve reaches a minimum or maximum.
	std::pair <double, double> CHYGlyph::BezierInflection (double a0, double a1, double a2, double a3)
	{
		double		a = -a0 + 3.0*a1 - 3.0*a2 + a3;
		double		b = 2.0*a0 -4.0*a1 + 2.0*a2;
		double		c = -a0 + a1;
		double		root1;
		double		root2;

		Quadratic (a, b, c, root1, root2);
		return std::make_pair (root1, root2);

	}	// end of std::pair <double, double> CHYGlyph::BezierInflection ()

	double	CHYGlyph::BezierEval (double a0, double a1, double a2, double a3, double t)
	{
		if (IsNan (t))
			return Nan();

		if (t < 0.0 || t > 1.0) return -1.0;

		double		rt = 1.0 - t;
		return rt * ((rt * (rt * a0 + 3.0*a1*t)) + 3.0*a2*t*t) + t*t*t*a3;

	}	// end of double CHYGlyph::BezierEval ()

	double	CHYGlyph::Greater (double original, double x)
	{
		if (IsNan (x))
			return original;

		if (x > original)
			return x;
		else
			return original;

	}	// end of double	CHYGlyph::Greater()

	double	CHYGlyph::Less (double original, double x)
	{
		if (IsNan (x))
			return original;

		if (x < original)
			return x;
		else
			return original;

	}	// end of double	CHYGlyph::Less()

	void CHYGlyph::Report3Bezier (RECT &rect, double ax, double ay, double bx, double by, double cx, double cy, double dx, double dy)
	{
		std::pair <double, double>		xs = BezierInflection (ax, bx, cx, dx);
		std::pair <double, double>		ys = BezierInflection (ay, by, cy, dy);

		if (xs.first < 0.0 || xs.first > 1.0)
			xs.first = Nan();

		if (xs.second < 0.0 || xs.second > 1.0)
			xs.second = Nan();

		if (ys.first < 0.0 || ys.first > 1.0)
			ys.first = Nan();

		if (ys.second < 0.0 || ys.second > 1.0)
			ys.second = Nan();

		double		a, b;
		a = BezierEval (ax, bx, cx, dx, xs.first);
		b = BezierEval (ax, bx, cx, dx, xs.second);
		rect.left = ::HY_RealRount(Less (rect.left, a));
		rect.left = ::HY_RealRount(Less (rect.left, b));
		rect.right =::HY_RealRount(Greater(rect.right, a));
		rect.right = ::HY_RealRount(Greater (rect.right, b));

		a = BezierEval (ay, by, cy, dy, ys.first);
		b = BezierEval (ay, by, cy, dy, ys.second);

		rect.bottom = ::HY_RealRount(Less(rect.bottom, a));
		rect.bottom = ::HY_RealRount(Less(rect.bottom, b));
		rect.top = ::HY_RealRount(Greater(rect.top, a));
		rect.top = ::HY_RealRount(Greater(rect.top, b));

	}	// end of void CHYGlyph::ReportBezier()

	void CHYGlyph::ReportBounds (RECT &bounds, CHYPoint &pos)
	{
		if (pos.x < bounds.left)
			bounds.left = pos.x;

		if (pos.x > bounds.right)
			bounds.right = pos.x;

		if (pos.y < bounds.bottom)
			bounds.bottom = pos.y;

		if (pos.y > bounds.top)
			bounds.top = pos.y;

	}	// end of void CHYGlyph::ReportBounds ()
}






