#include "HYFontCodecHead.h"
#include "HYFontCodec.h"
//#include "ximage.h"

using namespace  HYFONTCODEC;
namespace HYIMGTOFONT
{	
	class AFX_EXT_CLASS CHYImageToFont
	{
		public:
			CHYImageToFont(void);
			virtual ~CHYImageToFont(void);

			void		findfile(char* lpPath, std::vector<string>& vtFileNames);			
			BOOL		ImageToGlyph(string strImage, HYFONTCODEC::CHYFontCodec& FntEncode);
			void		MonoImgData(char* GrayData,char* MonoImgData, int iGrayThres ,int iWD, int iHT);
			void		DotMatrixToHanYi(char*	pMtrxData, int iWidth, int iHeight,HYFONTCODEC::CHYGlyph& HyGrph, int iBezierN, double dbErrorItem,double avrdis);
			void		FilterPointList(std::vector<CHYPoint>& vtPntLst);
			void		FilterContourList(std::vector<CHYContour>& vtPntLst);
			void		BezierContour(std::vector<CHYContour>& N_ContourItemLst,std::vector<CHYContour>& pHy,double dbvalue,int iBezierN,double dbDelta);
			void		bezierS(std::vector<CHYPoint>& temp_PointList,std::vector<CHYPoint>&	toPointlist,double dbvalue,int iBezierN);
			void		bezierS2_2(std::vector<CHYPoint>& temp_PointList,std::vector<CHYPoint>& toPointlist,double dbvalue,int dbBezierN,double dbDelta);
			float		dis(const CHYPoint& p1,const CHYPoint& p2);
			int			beziern(std::vector<CHYPoint>&	temp_PointList,std::vector<CHYPoint>& toPointlist,int BN,double dbvalue);
			int			bezierntest(std::vector<CHYPoint>&	temp_PointList01,std::vector<CHYPoint>&	toPointlist,int BN,int bflag,double m_value);
			int			Factorial(int n);
			int			Guass(double *A,double *X,double *B,int XN,int Guassflag);
			void		MeanContour(std::vector<CHYPoint>& ContourItemLst,std::vector<CHYPoint>& pHy);

			int			isSCurve(std::vector<CHYPoint>& temp_PointList);
			int			FindJLinetoLine(double k1,double b1,CHYPoint& Point11,CHYPoint& Point21,CHYPoint& Point22);
			void		Clearnoise(std::vector<CHYPoint>& VectorPointList,std::vector<CHYPoint>& ClearPointList,double alphad2,double alphad3);
			double		Calc_ANGLEThreePoint(const CHYPoint& StartPt,const CHYPoint& MidPt,const CHYPoint& EndPt);
			bool		ContourOnContour(std::vector<CHYPoint>& contourA,std::vector<CHYPoint>& contourB);
			double		AVRDisofPoint(std::vector<CHYPoint>& contour);
			int			ImageCut(std::vector<CHYContour>& ContourItemLst,std::vector<CHYContour>& ImageCutContourItemLst,int iWidth, int iHeight,double dbDelta,bool& Leftcut,bool& Rightcut,bool& Upcut,bool& Belowcut,double avrdis);
			bool		OrientationOfPolygon(std::vector<CHYPoint>& contour);
			void		BubbleSort(int *R, int n);
			void		Vectorize(std::vector<CHYPoint>& APointList,std::vector<CHYPoint>& VectorPointList,int iWidth,double delta);
			double		dispointtoline(CHYPoint& startpoint,CHYPoint& endpoint,CHYPoint& tagpoint);
			void		GetContour(char*	pMtrxData, int iWidth, int iHeight, std::vector<CHYContour>& ContourItemLst, std::vector<CHYContour>& ImageCutContourItemLst,int m_BezierN);
			void		FindImgFile(char* strFoldername,char* extName,std::vector<CString>& vtFiles);
			//char*		GetImageMtrxData(CxImage* pImg);
			//CxImage*	SquareImage(const char* ImgFile);			

			void		AdjustMetrics(int BsLine, float fEMScale, HYFONTCODEC::CHYGlyph & Glyph);
			void		AdjustMetrics(int BsLine, float fEMScale, HYFONTCODEC::CHYFontCodec& FntEncode);

	};
}
