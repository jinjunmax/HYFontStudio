#pragma once
#include "xsysbaseproxy.h"

class CXSysProxy :	public CXSysBaseProxy
{
public:
	CXSysProxy(void);
	~CXSysProxy(void);

public:
	BOOL	MToWChar(WCHAR* pDest, int& iDestlen, char* pSrc);

	float	RectOverlap(const CvRect& rct1, const CvRect& rct2);
	double	PointDistance(int x1, int x2, int y1, int y2);
	int		RectDistance(CvRect rect1, CvRect rect2);
	void	RectMerge(const CvRect& rct1, const CvRect& rct2, CvRect& rctMerge);
	bool	Hit(std::vector<int> vtValue, int Value);

	void	SplitColRow(IplImage* bWImg, std::vector<Rect2d>& vtGrid, int RowNum, int ColNum);
	void	SplitRowCol(IplImage* bWImg, std::vector<Rect2d>& vtGrid, int RowNum, int ColNum);
	
	void	Filter(std::vector<int>& vtR);
	BOOL	ImageToGlyph(char* SrcImg, int TopEdge, int LeftEdge, int RightEdge, int BottomEdge, int Colum, int Row, int BinaryThre, int iDirect);
	BOOL	SplitImage(char* SrcImg, char* SaveDir, int& code, int TopEdge, int LeftEdge, int RightEdge, int BottomEdge, int Colum, int Row, int BinaryThre, int iDirect);
	void	SaveGridImg(IplImage* bWImg, std::vector<Rect2d>& vtGrid, CString strDir, int& code);

	BOOL	SquareImage(const char* SrcImg, int BinaryThre, const char* MonoImg, int iMonoWidth, int SmthPrm=9);
	void	UnifyRectSize(std::vector<Rect2d>& vtGrid);
	
	void	MakeGlyph(IplImage* BinaryImg, std::vector<Rect2d>& vtGlyphBound, int iBaseLine);
	int		FindMaxBorder(std::vector<Rect2d> vtRect);
	void	ImageDataBorder(char* GrayData, int iWD, int iHT, char* MonoImgData, int MaxBorder);
	void	AdapterEM();

public:	
	//CHYImageToFont				m_ImageToFont;
	std::vector<unsigned long>	m_vtUnicode;

};

CXSysProxy&	XSysproxy(); 
CXSysProxy*	PXSysproxy();