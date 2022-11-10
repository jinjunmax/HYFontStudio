#include "StdAfx.h"
#include "XSysProxy.h"

static CXSysProxy	g_XSysproxy;
CXSysProxy&		XSysproxy()			{ return g_XSysproxy;} 
CXSysProxy*		PXSysproxy()		{ return &g_XSysproxy;}

CXSysProxy::CXSysProxy(void)
{

}	// end of CXSysProxy::CXSysProxy(void)

CXSysProxy::~CXSysProxy(void)
{

}	// end of CXSysProxy::~CXSysProxy(void)

BOOL CXSysProxy::MToWChar(WCHAR* pDest, int& iDestlen, char* pSrc )
{
	if ( pDest == NULL )			return FALSE;
	if ( pSrc == NULL )				return FALSE;

	WCHAR*	wpString			= NULL;
	int		iWStringLen			= 0;

	iWStringLen = MultiByteToWideChar(CP_ACP,0, pSrc, -1, NULL, 0 );
	if ( iWStringLen == 0 )			return FALSE;
	if ( iWStringLen > iDestlen )	return FALSE;

	wpString = new WCHAR[iWStringLen];
	ZeroMemory(wpString, sizeof(WCHAR) * iWStringLen);

	MultiByteToWideChar(CP_ACP, 0, pSrc, strlen(pSrc), wpString, iWStringLen);

	iDestlen = iWStringLen;
	CopyMemory(pDest,wpString, sizeof(WCHAR) * iDestlen );

	if ( wpString ) delete[] wpString;
	return TRUE;

}	// end of BOOL	CXSysProxy::MToWChar()

float CXSysProxy::RectOverlap(const CvRect& rct1, const CvRect& rct2)
{
	if (rct1.x > rct2.x + rct2.width)	{ return 0.0; }
	if (rct1.y > rct2.y + rct2.height)	{ return 0.0; }
	if (rct1.x + rct1.width < rct2.x)	{ return 0.0; }
	if (rct1.y + rct1.height < rct2.y)	{ return 0.0; }

	float colInt = min(rct1.x + rct1.width, rct2.x + rct2.width) - max(rct1.x, rct2.x);
	float rowInt = min(rct1.y + rct1.height, rct2.y + rct2.height) - max(rct1.y, rct2.y);

	float intersection = colInt * rowInt;
	float area1 = rct1.width*rct1.height;
	float area2 = rct2.width*rct2.height;

	return intersection / (area1 + area2 - intersection);

}	// end of float CXSysProxy::RectOverlap()

double CXSysProxy::PointDistance(int x1,int x2,int y1,int y2)
{
	return sqrt((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2));

}	// end of double CXSysProxy::Pointdistance()

int CXSysProxy::RectDistance(CvRect rect1, CvRect rect2)
{
	CRect rct1, rct2;

	// 相交
	if (RectOverlap(rect1, rect2) > 0.0) return -1;

	// windows 坐标系
	// 1或4象限
	if (rect2.x > rect1.x)
	{
		// 1象限
		if (rect2.y > rect1.y)
		{
			if (rect1.x + rect1.width > rect2.x)
			{
				return  rect2.y - (rect1.y+ rect1.height);
			}
			else 
			{
				if (rect2.y > rect1.y + rect1.height)
				{
					return (int)(PointDistance(rect1.x + rect1.width, rect2.x, rect1.y + rect1.height, rect2.y)+0.5);
				}
				else
				{
					return rect2.x - (rect1.x + rect1.width);
				}
			}
		}
		else  //第四象限
		{
			if (rect1.x + rect1.width > rect2.x)
			{
				return  rect1.y - rect2.y + rect2.height;
			}
			else
			{
				if (rect2.y + rect2.height < rect1.y)
				{
					return (int)(PointDistance(rect1.x + rect1.width, rect2.x, rect1.y, rect2.y+rect2.height) + 0.5);
				}
				else
				{

					return rect2.x - (rect1.x + rect1.width);
				}
			}
		}
	}
	else // 2或3象限
	{
		// 2象限
		if (rect2.y < rect1.y)
		{
			if ((rect2.x + rect2.width) > rect1.x)
			{
				return rect1.y - (rect2.y + rect2.height);
			}
			else
			{
				if ((rect2.y+rect2.height)< rect1.y)
				{
					return (int)(PointDistance(rect1.x, rect2.x+rect2.width, rect1.y, rect2.y + rect2.height) + 0.5);
				}
				else
				{
					return rect1.x - (rect2.x + rect2.width);
				}
			}
		}
		else// 3象限
		{
			if ((rect2.x + rect2.width) > rect1.x)
			{
				return rect2.y - (rect1.y + rect1.height);
			}
			else
			{
				if ((rect1.y + rect1.height) < rect2.y)
				{

					return (int)(PointDistance(rect1.x, rect2.x+ rect2.width, rect1.y+rect1.height, rect2.y) + 0.5);
				}
				else
				{
					return rect1.x - (rect2.x + rect2.width);
				}
			}
		}
	}
		
}	// end of int CXSysProxy::RectDistance()

void CXSysProxy::RectMerge(const CvRect& rct1, const CvRect& rct2, CvRect& rctMerge)
{
	rctMerge.x = rct1.x < rct2.x ? rct1.x : rct2.x;
	rctMerge.y = rct1.y < rct2.y ? rct1.y : rct2.y;

	if (rct1.x + rct1.width > rct2.x + rct2.width)	{
		rctMerge.width = rct1.x + rct1.width - rctMerge.x;
	}
	else	{
		rctMerge.width = rct2.x + rct2.width - rctMerge.x;
	}

	if (rct1.y + rct1.height> rct2.y + rct2.height) {
		rctMerge.height = rct1.y + rct1.height- rctMerge.y;
	}
	else {
		rctMerge.height= rct2.y + rct2.height- rctMerge.y;
	}

}	// end of void CXSysProxy::RectMerge()

bool CXSysProxy::Hit(std::vector<int> vtValue, int Value)
{
	for (size_t i = 0; i < vtValue.size(); i++)
	{
		if (vtValue[i] == Value) return true;
	}

	return false;

}	// end of bool CXSysProxy::Hit()

void CXSysProxy::SplitColRow(IplImage* bWImg, std::vector<Rect2d>& vtGrid, int RowNum, int ColNum)
{
	CvSize sz = cvGetSize(bWImg);
	sz.height = 1;
	CvMat* MatC = cvCreateMat(sz.height, sz.width, CV_32S);
	cvZero(MatC);

	cvReduce(bWImg, MatC, 0, CV_REDUCE_AVG);

	std::vector<int> vtC;
	for (int i = 0; i < MatC->width; i++) {
		int iAvg = MatC->data.i[i];
		vtC.push_back(iAvg);
	}

	std::vector<int> vtC1(vtC.size(), 0);
	::XSysproxy().LinearSmooth7(vtC, vtC1, vtC.size());

	std::vector<int> vtCol;
	::XSysproxy().FindMargin(vtC1, vtCol, ColNum);

	// 按列处理
	for (size_t Col = 0; Col < vtCol.size();)
	{
		int p1 = vtCol[Col++];
		int p2 = vtCol[Col++];

		CvSize SubImgSize;
		SubImgSize.width = p2 - p1;
		SubImgSize.height = bWImg->height;

		IplImage* SubImg = cvCreateImage(SubImgSize, IPL_DEPTH_8U, 1);
		uchar* srcData = (uchar*)bWImg->imageData;
		uchar* SubImgData = (uchar*)SubImg->imageData;
		ZeroMemory(SubImgData, SubImg->imageSize);
		int Srclinebytes = bWImg->widthStep;
		int Sublinebytes = SubImg->widthStep;
		int SubRow = 0;
		int SubOffset = 0;
		uchar srcpix = 0;
		for (int y = 0; y < bWImg->height; y++) {
			SubOffset = 0;
			for (int x = p1; x < p2; x++) {
				srcpix = (uchar)srcData[y*Srclinebytes + x];
				if (y == 0 || x == p1 || y == SubImg->height - 1 || x == p2 - 1)
					SubImgData[SubRow*Sublinebytes + SubOffset++] = 0;
				else
					SubImgData[SubRow*Sublinebytes + SubOffset++] = srcpix;
			}
			SubRow++;
		}

		CvSize sz = cvGetSize(SubImg);
		sz.width = 1;
		CvMat* MatRow = cvCreateMat(sz.height, sz.width, CV_32S);
		cvZero(MatRow);
		cvReduce(SubImg, MatRow, 1, CV_REDUCE_AVG);
		std::vector<int> vtR;
		for (int i = 0; i < MatRow->height; i++) {
			int iAvg = MatRow->data.i[i];
			vtR.push_back(iAvg);
		}
		std::vector<int> vtR1(vtR.size(), 0);
		::XSysproxy().LinearSmooth7(vtR, vtR1, vtR.size());
		std::vector<int> vtRow;
		::XSysproxy().FindMargin(vtR1, vtRow, RowNum);

		Rect2d	rct2d;
		rct2d.x = p1;
		rct2d.width = p2 - p1;

		for (size_t r = 0; r < vtRow.size();)
		{
			rct2d.y = vtRow[r++];
			rct2d.height = vtRow[r++] - rct2d.y;
			vtGrid.push_back(rct2d);
		}
		cvReleaseMat(&MatRow);
		cvReleaseImage(&SubImg);
	}

	cvReleaseMat(&MatC);

}	// end of void CXSysProxy::SplitColRow()

void CXSysProxy::SplitRowCol(IplImage* bWImg, std::vector<Rect2d>& vtGrid,int RowNum, int ColNum)
{
	CvSize sz = cvGetSize(bWImg);
	sz.width = 1;
	CvMat* MatH = cvCreateMat(sz.height, sz.width, CV_32S);
	cvZero(MatH);

	cvReduce(bWImg, MatH, 1, CV_REDUCE_AVG);
	std::vector<int> vtR;
	for (int i = 0; i < MatH->height; i++) {
		int iAvg = MatH->data.i[i];
		vtR.push_back(iAvg);
	}
	Filter(vtR);

	std::vector<int> vtR1(vtR.size(), 0);
	::XSysproxy().LinearSmooth7(vtR, vtR1, vtR.size());

	std::vector<int> vtRow;
	::XSysproxy().FindMargin(vtR1, vtRow, RowNum);
	int iTestIndx = 0;
	// 按列处理
	for (size_t Row = 0; Row<vtRow.size();)
	{
		int p1 = vtRow[Row++];
		int p2 = vtRow[Row++];

		CvSize SubImgSize;
		SubImgSize.width = bWImg->width;
		SubImgSize.height = p2 - p1;
		IplImage* SubImg = cvCreateImage(SubImgSize, IPL_DEPTH_8U, 1);
		uchar* srcData = (uchar*)bWImg->imageData;
		uchar* SubImgData = (uchar*)SubImg->imageData;
		ZeroMemory(SubImgData, SubImg->imageSize);
		int linebytes = SubImg->widthStep;
		int SubRow = 0;
		uchar srcpix = 0;
		for (int y = p1; y < p2; y++) {
			for (int x = 0; x < bWImg->width; x++) {
				srcpix = (uchar)srcData[y*linebytes + x];
				if (y == p1 || x == 0 || y == p2 - 1 || x == SubImg->width - 1)
					SubImgData[SubRow*linebytes + x] = 0;
				else
					SubImgData[SubRow*linebytes + x] = srcpix;
			}
			SubRow++;
		}

		CvSize sz = cvGetSize(SubImg);
		sz.height = 1;
		CvMat* MatCol = cvCreateMat(sz.height, sz.width, CV_32S);
		cvZero(MatCol);
		cvReduce(SubImg, MatCol, 0, CV_REDUCE_AVG);
		std::vector<int> vtC;
		for (int i = 0; i < MatCol->width; i++) {
			int iAvg = MatCol->data.i[i];
			vtC.push_back(iAvg);
		}
		Filter(vtC);

		std::vector<int> vtC1(vtC.size(), 0);
		::XSysproxy().LinearSmooth7(vtC, vtC1, vtC.size());
		std::vector<int> vtCol;
		::XSysproxy().FindMargin(vtC1, vtCol, ColNum);

		Rect2d	rct2d;
		rct2d.y = p1;
		rct2d.height = p2 - p1;

		int iGrid = 0;
		for (size_t c = 0; c < vtCol.size();)
		{
			rct2d.x = vtCol[c++];
			rct2d.width = vtCol[c++]- rct2d.x;
			vtGrid.push_back(rct2d);
		}

		if (MatCol != NULL)
			cvReleaseMat(&MatCol);
		if (SubImg != NULL)
			cvReleaseImage(&SubImg);
	}

	if (MatH!=NULL)
		cvReleaseMat(&MatH);
	
}	// end of void CXSysProxy::SplitRowCol()

void CXSysProxy::SaveGridImg(IplImage* bWImg,std::vector<Rect2d>& vtGrid,CString strDir,int& code)
{
	int iGrid = 0;
	for (size_t i = 0; i < vtGrid.size(); i++)
	{
		Rect2d& rc2d = vtGrid[i];

		CvSize GridImgSize;
		GridImgSize.width = rc2d.width;
		GridImgSize.height = rc2d.height;
		cvSetImageROI(bWImg, rc2d);//设置源图像ROI
		IplImage* GridImg = cvCreateImage(GridImgSize, IPL_DEPTH_8U, 1);
		ZeroMemory(GridImg->imageData, GridImg->imageSize);
		cvCopy(bWImg, GridImg);

		for (int i = 0; i < GridImg->imageSize; i++){
			GridImg->imageData[i] = 255 - GridImg->imageData[i];
		}

		CString strFile;
		strFile.Format("%s%05d.jpg", strDir, code);
		cvSaveImage((LPSTR)(LPCTSTR)strFile, GridImg);
		cvResetImageROI(bWImg);
		code++;
	}

}	// end of void CXSysProxy::SaveGridImg()

void CXSysProxy::Filter(std::vector<int>& vtR)
{
	std::vector<int> vtTmp = vtR;
	vtR.clear();
	size_t stSize = vtTmp.size();
	int iLen = 0;
	std::vector<int> vtTmpStegment;
	for (size_t i = 0; i < stSize;i++)
	{
		int& iTmp = vtTmp[i];
		if (iTmp==0){
			// 降噪点
			if (iLen > 20){	// 20 经验暂定

				vtR.insert(vtR.end(), vtTmpStegment.begin(), vtTmpStegment.end());
				vtTmpStegment.clear();
				iLen = 0;
			}

			vtR.push_back(iTmp);			
		}
		else{
			vtTmpStegment.push_back(iTmp);
			iLen++;
			
		}
	}

}	// end of void CXSysProxy::Filter()

// iDirect 0 从左到右 1 从上到下
BOOL CXSysProxy::ImageToGlyph(char* SrcImg, int TopEdge, int LeftEdge, int RightEdge, int BottomEdge, int Colum, int Row, int BinaryThre,int iDirect)
{
	IplImage* clrImage = cvLoadImage(SrcImg, CV_LOAD_IMAGE_GRAYSCALE);
	if (NULL == clrImage)	return FALSE;

	IplImage* BinaryImage = cvCreateImage(cvGetSize(clrImage), IPL_DEPTH_8U, 1);
	cvThreshold(clrImage, BinaryImage, BinaryThre, 255, CV_THRESH_BINARY);	 
	IplImage* bwImg = cvCreateImage(cvGetSize(BinaryImage), IPL_DEPTH_8U, 1);
	uchar* BinaryData = (uchar*)BinaryImage->imageData;
	uchar* bwData = (uchar*)bwImg->imageData;
	uchar srcpix = 0;

	//黑底白字为了切分图像
	int linebytes = BinaryImage->widthStep;
	for (int i = 0; i < BinaryImage->height; i++){
		for (int j = 0; j < BinaryImage->width; j++){	
			srcpix = (uchar)BinaryData[i*linebytes + j];
			if (i<TopEdge || j<LeftEdge || i> BinaryImage->height - BottomEdge || j > BinaryImage->width - RightEdge)
				bwData[i*linebytes + j] = 0;
			else
				bwData[i*linebytes + j] = 255 - srcpix;
		}
	}

	// 从左到右
	std::vector<Rect2d> vtGrid;
	if (iDirect == 0)	{
		SplitRowCol(bwImg, vtGrid, Row, Colum);
	}	// 从上到下
	else if (iDirect == 1)	{
		SplitColRow(bwImg, vtGrid, Row, Colum);
	}
	UnifyRectSize(vtGrid);

	int iBaseLine = m_fBaseLine * m_hyFontCodec.m_HYhead.unitsPerEm;
	MakeGlyph(BinaryImage, vtGrid, iBaseLine);
	m_hyFontCodec.CountFontBound();	

	cvReleaseImage(&bwImg);
	cvReleaseImage(&BinaryImage);
	cvReleaseImage(&clrImage);

	return TRUE;

}	// end of void CXSysProxy::ImageToGlyph()

BOOL CXSysProxy::SplitImage(char* SrcImg,char* SaveDir,int& code, int TopEdge, int LeftEdge, int RightEdge, int BottomEdge, int Colum, int Row, int BinaryThre, int iDirect)
{
	IplImage* clrImage = cvLoadImage(SrcImg, CV_LOAD_IMAGE_GRAYSCALE);
	if (NULL == clrImage)	return FALSE;

	IplImage* BinaryImage = cvCreateImage(cvGetSize(clrImage), IPL_DEPTH_8U, 1);
	cvThreshold(clrImage, BinaryImage, BinaryThre, 255, CV_THRESH_BINARY);
	IplImage* bwImg = cvCreateImage(cvGetSize(BinaryImage), IPL_DEPTH_8U, 1);
	
	uchar* BinaryData = (uchar*)BinaryImage->imageData;
	uchar* bwData = (uchar*)bwImg->imageData;
	uchar srcpix = 0;
	//黑底白字为了切分图像
	int linebytes = BinaryImage->widthStep;
	for (int i = 0; i < BinaryImage->height; i++) {
		for (int j = 0; j < BinaryImage->width; j++) {
			srcpix = (uchar)BinaryData[i * linebytes + j];
			if (i<TopEdge || j<LeftEdge || i> BinaryImage->height - BottomEdge || j > BinaryImage->width - RightEdge)
				bwData[i * linebytes + j] = 0;
			else
				bwData[i * linebytes + j] = 255 - srcpix;
		}
	}

	// 从左到右
	std::vector<Rect2d> vtGrid;
	if (iDirect == 0) {
		SplitRowCol(bwImg, vtGrid, Row, Colum);
	}	// 从上到下
	else if (iDirect == 1) {
		SplitColRow(bwImg, vtGrid, Row, Colum);
	}

	CString strSaveDir;

	string strName = HY_GetFileNameFromPath(string(SrcImg));	
	strSaveDir.Format("%s%s\\", SaveDir, strName.data());
	_mkdir(strSaveDir);

	SaveGridImg(bwImg, vtGrid, strSaveDir, code);

	cvReleaseImage(&bwImg);
	cvReleaseImage(&BinaryImage);
	cvReleaseImage(&clrImage);

	return TRUE;

}	// end of BOOL CXSysProxy::SplitImage()

BOOL CXSysProxy::SquareImage(const char* SrcImg, int TopEdge, int LeftEdge, int RightEdge, int BottomEdge, int Colum, int Row, int BinaryThre, int iDirect, const char* MonoImg, int iMonoWidth)
{
	IplImage* clrImage = cvLoadImage(SrcImg, CV_LOAD_IMAGE_GRAYSCALE);
	if (NULL == clrImage)	FALSE;

	IplImage* BinaryImage = cvCreateImage(cvGetSize(clrImage), IPL_DEPTH_8U, 1);
	cvThreshold(clrImage, BinaryImage, BinaryThre, 255, CV_THRESH_BINARY);

	IplImage* bwImg = cvCreateImage(cvGetSize(BinaryImage), IPL_DEPTH_8U, 1);
	uchar* BinaryData = (uchar*)BinaryImage->imageData;
	uchar* bwData = (uchar*)bwImg->imageData;
	uchar srcpix = 0;
	CString strFile;

	// 生成等宽图像
	int DataSize = iMonoWidth * iMonoWidth;
	char* pMonoData = new char[DataSize];
	::memset(pMonoData, 0xff, DataSize);

	// 缩放到合适的大小
	float fScale = BinaryImage->width > BinaryImage->height ? (float)iMonoWidth / BinaryImage->width : (float)iMonoWidth / BinaryImage->height;
	CvSize Scalesize;
	Scalesize.width = BinaryImage->width* fScale;
	Scalesize.height = BinaryImage->height* fScale;
	IplImage* ScaleImg = cvCreateImage(Scalesize, IPL_DEPTH_8U, 1);
	cvResize(BinaryImage, ScaleImg);

	ImageDataBorder(ScaleImg->imageData, ScaleImg->widthStep, ScaleImg->height, pMonoData, iMonoWidth);
	strFile.Format("%s.jpg", MonoImg);

	DeleteFile(strFile);
	CvSize sizeTmp;
	sizeTmp.height = iMonoWidth;
	sizeTmp.width = iMonoWidth;
	IplImage* RctImgBorder = cvCreateImage(sizeTmp, IPL_DEPTH_8U, 1);
	RctImgBorder->imageData = pMonoData;
	cvSaveImage((LPSTR)(LPCSTR)strFile, RctImgBorder);

	cvReleaseImage(&RctImgBorder);
	cvReleaseImage(&ScaleImg);
	
	if (pMonoData) delete[] pMonoData;
	pMonoData = NULL;

	cvReleaseImage(&bwImg);
	cvReleaseImage(&BinaryImage);
	cvReleaseImage(&clrImage);

	return true;

}	//end of void CXSysProxy::SquareImage()

void  CXSysProxy::UnifyRectSize(std::vector<Rect2d>& vtGrid)
{
	size_t stLength = vtGrid.size();
	if (stLength == 0) return;
	
	for (size_t i = 0; i != stLength; i++)
	{
		Rect2d& rc2d = vtGrid[i];

		rc2d.x -= m_tagScanPrm.iSingleBorder;
		rc2d.y -= m_tagScanPrm.iSingleBorder;
		rc2d.width += m_tagScanPrm.iSingleBorder<<1;
		rc2d.height += m_tagScanPrm.iSingleBorder<<1;
	}

}	// end of BOOL CXSysProxy::UnifyRectSize()

void CXSysProxy::MakeGlyph(IplImage* BinaryImg, std::vector<Rect2d>& vtGlyphBound,int iBaseLine)
{	
	size_t  stGlyph = vtGlyphBound.size();
	int		iMaxBorder = FindMaxBorder(vtGlyphBound);

	CString strFile;
	for (size_t i = 0; i < stGlyph; i++)	
	{	
		Rect2d rct2d = vtGlyphBound[i];
		CvSize size;
		size.width = rct2d.width;
		size.height = rct2d.height;

		//从图像中提取子图像
		IplImage* RctImg = cvCreateImage(size, IPL_DEPTH_8U,1);
		cvSetImageROI(BinaryImg, rct2d);
		cvCopy(BinaryImg, RctImg);
		cvResetImageROI(BinaryImg);

#if 0
		strFile.Format("d:\\Img%d.jpg",i);
		DeleteFile(strFile);
		cvSaveImage((LPSTR)(LPCSTR)strFile, RctImg);
#endif	

		int DataSize = iMaxBorder*iMaxBorder;
		char* pMonoData = new char[DataSize];		
		::memset(pMonoData,0xff, DataSize);

		ImageDataBorder(RctImg->imageData, RctImg->widthStep, RctImg->height, pMonoData, iMaxBorder);

#if 0
		strFile.Format("d:\\BorderImg%d.jpg", i);
		DeleteFile(strFile);
		CvSize sizeTmp;
		sizeTmp.height = iMaxBorder;
		sizeTmp.width = iMaxBorder;
		IplImage* RctImgBorder = cvCreateImage(sizeTmp, IPL_DEPTH_8U, 1);
		RctImgBorder->imageData = pMonoData;
		cvSaveImage((LPSTR)(LPCSTR)strFile, RctImgBorder);
#endif	

		int		BaziderN = 2;
		double	avrdis = ::XSysproxy().m_tagScanPrm.iAvrdis;
		double	DisWidth = ::XSysproxy().m_tagScanPrm.iDisWidth;
		int		ErrorItem = 1;
		
		CHYGlyph glyph;
		glyph.fontFlag = FONTTYPE_TTF;
		glyph.glyfType = GLYF_TYPE_SIMPLE;
		glyph.advanceWidth = iMaxBorder;

		CHYImageToFont				ImageToFont;
		ImageToFont.DotMatrixToHanYi(pMonoData, iMaxBorder, iMaxBorder, glyph, BaziderN, ErrorItem, avrdis);

		float fEMScale = (float)m_hyFontCodec.m_HYhead.unitsPerEm / iMaxBorder;
		ImageToFont.AdjustMetrics(iBaseLine, fEMScale, glyph);

		glyph.advanceWidth = m_hyFontCodec.m_HYhead.unitsPerEm;
		glyph.CountBoundBox();

		m_hyFontCodec.m_vtHYGlyphs.push_back(glyph);

		if (pMonoData) delete[] pMonoData;
		pMonoData = NULL;

		cvReleaseImage(&RctImg);

		UpdateAllViews(MSG_PRS_SUB, (LPARAM)((i + 1) * 100 / stGlyph));
	}

}	// end of void	CXSysProxy::MakeGlyph()

int CXSysProxy::FindMaxBorder(std::vector<Rect2d> vtRect)
{
	size_t st = vtRect.size();
	int iMaxBorder = 0;
	for (size_t i = 0; i < st; i++)
	{
		Rect2d& rc2D = vtRect[i];
		if (rc2D.width > iMaxBorder)
			iMaxBorder = rc2D.width;
		if (rc2D.height > iMaxBorder)
			iMaxBorder = rc2D.height;
	}

	return  HY_NumerAlign(iMaxBorder, 4);

}	// end of void CXSysProxy::FindMaxBorder()

void CXSysProxy::ImageDataBorder(char* GrayData, int iWD, int iHT, char* MonoImgData, int MaxBorder)
{
	int iBOffsetY = (MaxBorder - iHT) / 2;
	int iBOffsetX = (MaxBorder - iWD) / 2;

	for (int y = 0; y<iHT; y++)
	{
		for (int x = 0; x<iWD; x++)
		{
			int iGrayOffset = y * iWD + x;
			int iMonoImgOffset = iBOffsetY * MaxBorder+x+iBOffsetX;

			if (x == 0 || y == 0 || y == (iHT - 1) || x == (iWD - 1))
			{
				MonoImgData[iMonoImgOffset] = (char)255;
			}
			else
			{	
				MonoImgData[iMonoImgOffset] = (BYTE)GrayData[iGrayOffset];
			}
		}
		iBOffsetY++;
	}

}	// end of void CXSysProxy::ImageDataBorder

void CXSysProxy::AdapterEM()
{



}	// end of void CXSysProxy::AdapterEM()



