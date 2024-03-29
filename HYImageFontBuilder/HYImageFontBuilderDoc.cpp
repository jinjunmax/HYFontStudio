
// HYImageFontBuilderDoc.cpp: CHYImageFontBuilderDoc 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "HYImageFontBuilder.h"
#endif

#include "HYImageFontBuilderDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static CHYImageFontBuilderDoc	g_Doc;
CHYImageFontBuilderDoc&		XDoc() { return g_Doc; }
CHYImageFontBuilderDoc*		PDoc() { return &g_Doc; }

// CHYImageFontBuilderDoc

IMPLEMENT_DYNCREATE(CHYImageFontBuilderDoc, CDocument)

BEGIN_MESSAGE_MAP(CHYImageFontBuilderDoc, CDocument)	
	ON_COMMAND(ID_IMGTEST, &CHYImageFontBuilderDoc::OnImgTest)	
	ON_COMMAND(ID_IMPRT_CHRTB_MU, &CHYImageFontBuilderDoc::OnImprtChrtbMu)
	ON_COMMAND(ID_FILE_SAVE, &CHYImageFontBuilderDoc::OnFileSave)
END_MESSAGE_MAP()


// CHYImageFontBuilderDoc 构造/析构

CHYImageFontBuilderDoc::CHYImageFontBuilderDoc()
{
	CString strFeatureFile;
	strFeatureFile.Format(_T("%sdata\\Profile.xml"), ::XSysproxy().m_strAppDir);
	::XSysproxy().LoadFontFeature(strFeatureFile, m_FontEnCodec);

}	// end of CHYImageFontBuilderDoc::CHYImageFontBuilderDoc()

CHYImageFontBuilderDoc::~CHYImageFontBuilderDoc()
{

}	// end of CHYImageFontBuilderDoc::~CHYImageFontBuilderDoc()

BOOL CHYImageFontBuilderDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	return TRUE;

}	// end of BOOL CHYImageFontBuilderDoc::OnNewDocument()

BOOL CHYImageFontBuilderDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	// TODO:  在此添加您专用的创建代码

	return TRUE;

}	// end of BOOL CHYImageFontBuilderDoc::OnOpenDocument()


// CHYImageFontBuilderDoc 序列化

void CHYImageFontBuilderDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}

#ifdef SHARED_HANDLERS

// 缩略图的支持
void CHYImageFontBuilderDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 修改此代码以绘制文档数据
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 搜索处理程序的支持
void CHYImageFontBuilderDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:     strSearchContent = _T("point;rectangle;circle;ole object;")；
	SetSearchContent(strSearchContent);
}

void CHYImageFontBuilderDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CHYImageFontBuilderDoc 诊断

#ifdef _DEBUG
void CHYImageFontBuilderDoc::AssertValid() const
{
	CDocument::AssertValid();

}	// end of void CHYImageFontBuilderDoc::AssertValid()

void CHYImageFontBuilderDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);

}	// end of void CHYImageFontBuilderDoc::Dump()
#endif //_DEBUG

// CHYImageFontBuilderDoc 命令

int  SortHeight = 0;
bool SortByY(const CvRect &v1, const CvRect &v2)
{
	int iH1 = v1.height;
	int iH2 = v2.height;

	if (abs(v1.y - v2.y) <= SortHeight)
	{
		return v1.x < v2.x;
	}
	else
	{
		return v1.y < v2.y;		 
	}	

}	// end of bool SortByY()

void CHYImageFontBuilderDoc::SortFontBound(std::vector<CvRect>& vtRct)
{
	std::sort(vtRct.begin(), vtRct.end(), SortByY);	

}	// end of void CHYImageFontBuilderDoc::QuickSortFontBound()

void CHYImageFontBuilderDoc::PointToRect(CvPoint* pt, CvRect &rt)
{
	// 找到左上角
	int iMinX = pt[0].x;
	int iMinY = pt[0].y;
	int iMaxX = pt[0].x;
	int iMaxY = pt[0].y;

	for (int i = 1; i < 4; i++)
	{
		iMinX = pt[i].x < iMinX ? pt[i].x : iMinX;
		iMinY = pt[i].y < iMinY ? pt[i].y : iMinY;
		iMaxX = pt[i].x > iMaxX ? pt[i].x : iMaxX;
		iMaxY = pt[i].y > iMaxY ? pt[i].y : iMaxY;
	}

	rt.x = iMinX;
	rt.y = iMinY;
	rt.width = iMaxX - iMinX;
	rt.height = iMaxY - iMinY;

}	// end of void CHYImageFontBuilderDoc::PointToRect()

//////////////////////////////////////////////////////////////////
//函数功能：用向量来做COSα=两向量之积/两向量模的乘积求两条线段夹角
//输入：   线段3个点坐标pt1,pt2,pt0,最后一个参数为公共点
//输出：   线段夹角，单位为角度
//////////////////////////////////////////////////////////////////
double CHYImageFontBuilderDoc::angle(CvPoint* pt1, CvPoint* pt2, CvPoint* pt0)
{
	double dx1 = pt1->x - pt0->x;
	double dy1 = pt1->y - pt0->y;
	double dx2 = pt2->x - pt0->x;
	double dy2 = pt2->y - pt0->y;
	double angle_line = (dx1*dx2 + dy1 * dy2) / sqrt((dx1*dx1 + dy1 * dy1)*(dx2*dx2 + dy2 * dy2) + 1e-10);//余弦值
	return acos(angle_line) * 180 / 3.141592653;

}	// end of double CHYImageFontBuilderDoc::angle()

//////////////////////////////////////////////////////////////////
//函数功能：采用多边形检测，通过约束条件寻找矩形
//输入：   img					原图像
//        storage				存储
//        minarea，maxarea		检测矩形的最小/最大面积
//         minangle,maxangle	检测矩形边夹角范围，单位为角度
//输出：   矩形序列
//////////////////////////////////////////////////////////////////
CvSeq* CHYImageFontBuilderDoc::findSquares4(IplImage* img, CvMemStorage* storage, int minarea, int maxarea, int minangle, int maxangle)
{
	CvSeq* contours;//边缘	
	CvSize sz = cvSize(img->width & -2, img->height & -2);
	IplImage* timg = cvCloneImage(img);//拷贝一次img
	IplImage* gray = cvCreateImage(sz, 8, 1); //img灰度图
	IplImage* pyr = cvCreateImage(cvSize(sz.width / 2, sz.height / 2), img->depth, img->nChannels);  //金字塔滤波3通道图像中间变量
	IplImage* tgray = cvCreateImage(sz, 8, 1); ;
	CvSeq* result;
	double s, t;
	CvSeq* squares = cvCreateSeq(0, sizeof(CvSeq), sizeof(CvPoint), storage);

	cvSetImageROI(timg, cvRect(0, 0, sz.width, sz.height));

	//金字塔滤波 
	cvPyrDown(timg, pyr, 7);
	cvPyrUp(pyr, timg, 7);
	
	cvSetImageCOI(timg, 1);
	cvCopy(timg, tgray, 0);
		
	//二值化
	cvThreshold(tgray, gray, 125, 255, CV_THRESH_BINARY);
	cvFindContours(gray, storage, &contours, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0));

	while (contours)
	{ 
		//多边形逼近             
		result = cvApproxPoly(contours, sizeof(CvContour), storage, CV_POLY_APPROX_DP, cvContourPerimeter(contours)*0.01, 0);
		//如果是凸四边形并且面积在范围内
		int iMin = fabs(cvContourArea(result, CV_WHOLE_SEQ));

		if (result->total == 4 && iMin > minarea  && iMin < maxarea &&  cvCheckContourConvexity(result))
		{
			s = 0;
			//判断每一条边
			for (int i = 0; i < 5; i++)
			{
				if (i >= 2)
				{   //角度            
					t = fabs(angle((CvPoint*)cvGetSeqElem(result, i), (CvPoint*)cvGetSeqElem(result, i - 2), (CvPoint*)cvGetSeqElem(result, i - 1)));
					s = s > t ? s : t;
				}
			}
			//这里的S为直角判定条件 单位为角度
			if (s > minangle && s < maxangle)
			{
				for (int i = 0; i < 4; i++)
					cvSeqPush(squares, (CvPoint*)cvGetSeqElem(result, i));
			}
			else
			{
				int iJinjun = 0;
			}
		}
		contours = contours->h_next;
	}

	cvReleaseImage(&gray);
	cvReleaseImage(&pyr);
	cvReleaseImage(&tgray);
	cvReleaseImage(&timg);

	return squares;

}	// end of CvSeq* findSquares4()

//////////////////////////////////////////////////////////////////
//函数功能：画出所有矩形
//输入：   img 原图像
//          squares 矩形序列
//输出：   图像中标记矩形
//////////////////////////////////////////////////////////////////
void CHYImageFontBuilderDoc::drawSquares(IplImage* img, CvSeq* squares)
{
	CvSeqReader reader;
	IplImage* cpy = img;
	CvPoint pt[4];
	int i;
	cvStartReadSeq(squares, &reader, 0);
	for (i = 0; i < squares->total; i += 4)
	{
		CvPoint* rect = pt;
		int count = 4;
		memcpy(pt, reader.ptr, squares->elem_size);
		CV_NEXT_SEQ_ELEM(squares->elem_size, reader);
		memcpy(pt + 1, reader.ptr, squares->elem_size);
		CV_NEXT_SEQ_ELEM(squares->elem_size, reader);
		memcpy(pt + 2, reader.ptr, squares->elem_size);
		CV_NEXT_SEQ_ELEM(squares->elem_size, reader);
		memcpy(pt + 3, reader.ptr, squares->elem_size);
		CV_NEXT_SEQ_ELEM(squares->elem_size, reader);

		cvPolyLine(cpy, &rect, &count, 1, 1, CV_RGB(255,0, 0), 15 );
	}

}	// end of void CHYImageFontBuilderDoc::drawSquares()

void CHYImageFontBuilderDoc::OnImgTest()
{
	TCHAR	szFilters[] = _T("jpg 文件(*.jpg)|*.jpg|png 文件(*.png)|*.png|BMP 文件(*.bmp)|(*.bmp)|图像文件(*.bmp,*png,*jpg)|(*.bmp,*png,*jpg)||");
	CFileDialog  openFileDlg(TRUE, _T(""), _T(""), OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilters);
	if (openFileDlg.DoModal() != IDOK)	return;

	CString strImagePath = openFileDlg.GetPathName();
	OpcvImgTest((LPTSTR)(LPCTSTR)strImagePath, 570000, 750000, 80, 100);

}	// end of void CHYImageFontBuilderDoc::OnImgTest()

Mat src, src_gray;
Mat dst, detected_edges;
int edgeThresh = 1;
int lowThreshold;
int const max_lowThreshold = 100;
int ratio = 3;
int kernel_size = 3;
char* window_name = "Edge Map";

void CannyThreshold(int, void*)
{
	/// Reduce noise with a kernel 3x3
	blur(src_gray, detected_edges, cv::Size(3, 3));

	/// Canny detector
	cv::Canny(detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size);

	/// Using Canny's output as a mask, we display our result
	dst = Scalar::all(0);

	src.copyTo(dst, detected_edges);
	imshow(window_name, dst);

}	// end of void CannyThreshold(int, void*)

void CHYImageFontBuilderDoc::OpcvTempTest(char* SrcImg,int MinArea, int MaxArea, int MinAngle, int MaxAngle)
{
#if 1
	IplImage* clrImage = cvLoadImage(SrcImg, CV_LOAD_IMAGE_GRAYSCALE);
	if (NULL == clrImage)
	{
		AfxMessageBox("图像读取失败！");
		return;
	}

	IplImage * image_blur = cvCloneImage(clrImage);
	cvZero(image_blur);

	cvSmooth(clrImage, image_blur, CV_GAUSSIAN, 5, 5, 3, 3);
	cvSaveImage("d:\\高斯模糊.jpg", image_blur);

	IplImage* BinaryImage = cvCreateImage(cvGetSize(image_blur), IPL_DEPTH_8U, 1);

	cvThreshold(image_blur, BinaryImage, 128, 255, CV_THRESH_BINARY);

	IplImage* Canny = cvCreateImage(cvGetSize(image_blur), IPL_DEPTH_8U, 1);
	cvCanny(BinaryImage,Canny,100,100,3);
	cvSaveImage("d:\\FindContours-before.jpg", Canny);
	
	double s = 0.0, t = 0.0;
	CvSeq* result;
	CvSeq* contours;//边缘	
	CvMemStorage* storage = cvCreateMemStorage(0);	
	CvSeq* squares = cvCreateSeq(0, sizeof(CvSeq), sizeof(CvPoint), storage);

	CvSize sz = cvSize(clrImage->width, clrImage->height);
	
	cvFindContours(Canny, storage, &contours, sizeof(CvContour), CV_RETR_EXTERNAL, CV_RETR_CCOMP, cvPoint(0, 0));
	
	IplImage* dst = cvCreateImage(cvGetSize(Canny), 8, 3);

	std::vector<int> lstCntAre;

	//CvMemStorage* storage1 = cvCreateMemStorage(0);
	//CvSeq* lstRect = cvCreateSeq(0, sizeof(CvSeq), sizeof(CvPoint), storage1);
	std::vector<CvRect> vtRect;
	int iContouNum = 0;
	SortHeight = 0;
	for (; contours != 0; contours = contours->h_next)
	{	
		int iMin = fabs(cvContourArea(contours, CV_WHOLE_SEQ));
		if (iMin > 100)
		{
			lstCntAre.push_back(iMin);

			CvScalar color = CV_RGB(255, 0, 0);				
			cvDrawContours(dst, contours, color, color, 0, 2, 8);
			
			CvRect rctCntrs = cvBoundingRect(contours);
			CvPoint pt1, pt2;
			pt1.x = rctCntrs.x;
			pt1.y = rctCntrs.y;

			pt2.x = rctCntrs.x + rctCntrs.width;
			pt2.y = rctCntrs.y + rctCntrs.height;

			color = CV_RGB(0, 255, 0);
			cvDrawRect(dst, pt1, pt2, color,2);
			vtRect.push_back(rctCntrs);

			SortHeight = SortHeight < rctCntrs.height ? rctCntrs.height: SortHeight;
			iContouNum++;
		}
	}

	SortFontBound(vtRect);
#if 1	// 小框
	for (size_t i = 0; i < vtRect.size(); i++)
	{
		CvRect& rct = vtRect[i];
		{
			char str[10] = { 0 };
			sprintf_s(str, 10, "%d", i);

			CvPoint pt1;
			pt1.x = rct.x;
			pt1.y = rct.y;
			double Scale = 1;
			int Thickness = 1;
			CvFont Font1 = cvFont(Scale, Thickness);
			CvScalar color = CV_RGB(255, 255, 255);
			cvPutText(dst, str, pt1, &Font1, color);
		}
	}
#endif 

	std::vector<CvRect> vtBigRect;
	std::vector<int> vtdis;
	int TDis = 0;
	double dbAvgDis = 0;
	if (vtRect.size() > 0)	{
		if (vtRect.size() == 1) vtBigRect.push_back(vtRect[0]);

		for (size_t i = 0; i < vtRect.size()-1; i++)
		{
			const CvRect& rct1 = vtRect[i];
			const CvRect& rct2 = vtRect[i + 1];

			int iDis = ::XSysproxy().RectDistance(rct1, rct2);
			vtdis.push_back(iDis);
			TDis += iDis;
		}
		// 矩形平均间距
		dbAvgDis = TDis / (int)vtRect.size();

		// 获取需要的矩形
		std::vector<int> vtHit;
		for (size_t i = 0; i < vtRect.size(); i++)
		{
			if (::XSysproxy().Hit(vtHit, i))
				continue;

			CvRect rct1 = vtRect[i];
			CvRect rctMerge = rct1;
			vtHit.push_back(i);

			//bool bTMP = false;
			for (size_t j = i+1; j < vtRect.size();j++)
			{
				if (::XSysproxy().Hit(vtHit, j))
					continue;

				CvRect rct2 = vtRect[j];
				int iDis = ::XSysproxy().RectDistance(rct1, rct2);
				if (iDis > dbAvgDis)
					continue;

				::XSysproxy().RectMerge(rct1, rct2, rctMerge);
				rct1 = rctMerge;
				vtHit.push_back(j);
				//bTMP = true;
			}			
			vtBigRect.push_back(rctMerge);
		}

	}
	
#if 1 // 大框
	// 画出合适的矩形
	for (size_t i = 0; i < vtBigRect.size(); i++)
	{
		CvRect& rct = vtBigRect[i];
		{
			CvScalar color = CV_RGB(155, 30, 56);			
			cvRectangleR(dst, rct, color, 2);

			char str[10] = { 0 };
			sprintf_s(str, 10, "%d", i);
			CvPoint pt1;
			pt1.x = rct.x+5;
			pt1.y = rct.y+10;
			double Scale = 1;
			int Thickness = 1;
			CvFont Font1 = cvFont(Scale, Thickness);
			color = CV_RGB(255, 255, 255);
			cvPutText(dst, str, pt1, &Font1, color);
		}
	}
#endif 

	cvNamedWindow("Components", 1);
	cvShowImage("Components", dst);

	//cvNamedWindow("FindContours", CV_WINDOW_NORMAL);
	//cvShowImage("FindContours", Canny);
	cvSaveImage("d:\\FindContours-after.jpg", dst);
	/*
	while (contours)
	{
		//多边形逼近             
		result = cvApproxPoly(contours, sizeof(CvContour), storage, CV_POLY_APPROX_DP, cvContourPerimeter(contours)*0.01, 0);
		//如果是凸四边形并且面积在范围内
		int iMin = fabs(cvContourArea(result, CV_WHOLE_SEQ));
	
		//if (iMin > MinArea  && iMin < MaxArea &&  cvCheckContourConvexity(result))
		//{
			int  rcLeft = 0, rcRight = 0, rcTop = 0, rcBottom = 0;
			CvPoint temp;
			CvSeqReader reader; 

			cvStartReadSeq(result, &reader, 0);
			CV_READ_SEQ_ELEM(temp, reader);

			rcLeft = temp.x;
			rcRight = temp.x;
			rcTop = temp.y;
			rcBottom = temp.y;
			//cvMinAreaRect2
			for (int i = 1; i<result->total; i++)
			{
				CV_READ_SEQ_ELEM(temp, reader);

				if (temp.x < rcLeft) rcLeft = temp.x;
				if (temp.y < rcTop) rcTop = temp.y;
				if (temp.x > rcRight) rcRight = temp.x;
				if (temp.y > rcBottom) rcBottom = temp.y;
			}

			CvRect Rect;
			Rect.x = rcLeft;
			Rect.y = rcTop;
			Rect.width = rcRight - rcLeft;
			Rect.height = rcBottom - rcTop;

			cvSeqPush(squares, &Rect);
		//}
		contours = contours->h_next;
	}
	*/
	
#else 
	src = imread(SrcImg);

	if (!src.data)
	{
		return;
	}
	/// Create a matrix of the same type and size as src (for dst)
	dst.create(src.size(), src.type());
	/// Convert the image to grayscale
	cv::cvtColor(src, src_gray, CV_BGR2GRAY);
	/// Create a window
	cv::namedWindow(window_name, CV_WINDOW_AUTOSIZE);
	/// Create a Trackbar for user to enter threshold
	cv::createTrackbar("Min Threshold:", window_name, &lowThreshold, max_lowThreshold, CannyThreshold);

	/// Show the image
	CannyThreshold(0, 0);
#endif 
	
}	// end of void CHYImageFontBuilderDoc::OpcvTempTest()

// 水平 垂直 直方图投射
void CHYImageFontBuilderDoc::OpcvTempTest1(char* SrcImg)
{
	IplImage* clrImage = cvLoadImage(SrcImg, CV_LOAD_IMAGE_GRAYSCALE);
	if (NULL == clrImage)
	{
		AfxMessageBox("图像读取失败！");
		return;
	}

	IplImage* BinaryImage = cvCreateImage(cvGetSize(clrImage), IPL_DEPTH_8U, 1);
	cvThreshold(clrImage, BinaryImage, 120, 255, CV_THRESH_BINARY);

	//cvSaveImage("d:\\黑白图.jpg", BinaryImage);

	IplImage* dst = cvCreateImage(cvGetSize(BinaryImage), BinaryImage->depth, BinaryImage->nChannels); //创建目的图像
	int i = 0, j = 0;
	uchar* srcData = (uchar*)BinaryImage->imageData;
	uchar* dstData = (uchar*)dst->imageData;
	uchar srcpix = 0;

	int linebytes = BinaryImage->widthStep;
	for (i = 0; i < BinaryImage->height; i++)
	{	
		for (j = 0; j < BinaryImage->width; j++)
		{
			srcpix = 0;
			srcpix = (uchar)srcData[i*linebytes + j];

			if (i<10 || j<10 || i> BinaryImage->height-10 || j > BinaryImage->width-10)
				dstData[i*linebytes + j] = 0;
			else
				dstData[i*linebytes + j] = 255 - srcpix;
		}
	}

//	cvSaveImage("d:\\白黑图.jpg", dst);

	int width_src = BinaryImage->width;
	int Height_src = BinaryImage->height;	
	try {

		CvSize sz = cvGetSize(BinaryImage);
		sz.height = 1;
		CvMat* MatV = cvCreateMat(sz.height, sz.width, CV_32S);
		cvZero(MatV);
		cvReduce(dst, MatV, 0, CV_REDUCE_SUM);

		std::vector<int> vtV;
		for (int i = 0; i < MatV->width; i++)
		{
			int iSum  = MatV->data.i[i];
			vtV.push_back(iSum);
		}
		std::vector<int> vtV1(vtV.size(),0);
		::XSysproxy().LinearSmooth7(vtV, vtV1, vtV.size());
		std::vector<int> vtMargin;
		::XSysproxy().FindMargin(vtV1,vtMargin,10);

#if 1
		// 显示列直方图 列
		sz = cvGetSize(BinaryImage);
		//新建一个Mat用于储存投影直方图并将背景置为白色 
		Mat verticalProjectionMat(sz.height,sz.width, CV_8UC1);
		for (int i = 0; i < sz.height; i++) {
			for (int j = 0; j < sz.width; j++) {
				uchar perPixelValue = 255; //背景设置为白色。   
				verticalProjectionMat.at<uchar>(i, j) = perPixelValue; 
			} 
		}
		//将直方图的曲线设为黑色 
		for (int i = 0; i < sz.width; i++) {
			int height = vtV[i] > sz.height ? sz.height : vtV[i];
			for (int j = 0; j < height; j++) {
				uchar perPixelValue = 0; //直方图设置为黑色  
				verticalProjectionMat.at<uchar>(sz.height - 1 - j, i) = perPixelValue;
			}
		}
		cvNamedWindow("垂直投影", CV_WINDOW_FREERATIO);		
		imshow("垂直投影",verticalProjectionMat);		

		Mat verticalProjectionMat1(sz.height, sz.width, CV_8UC1);
		for (int i = 0; i < sz.height; i++) {
			for (int j = 0; j < sz.width; j++) {
				uchar perPixelValue = 255; //背景设置为白色。   
				verticalProjectionMat1.at<uchar>(i, j) = perPixelValue;
			}
		}

		for (int i = 0; i < sz.width; i++) {
			int height = vtV1[i] > sz.height ? sz.height : vtV1[i];
			for (int j = 0; j < height; j++) {
				uchar perPixelValue = 0; //直方图设置为黑色  
				verticalProjectionMat1.at<uchar>(sz.height - 1 - j, i) = perPixelValue;
			}
		}
		cvNamedWindow("垂直投影1", CV_WINDOW_FREERATIO);
		imshow("垂直投影1", verticalProjectionMat1);
#endif	
#if 0
		//////////////////////////////////////////////////////////////////
		sz = cvGetSize(BinaryImage);
		sz.width = 1;

		CvMat* MatH = cvCreateMat(sz.height, sz.width, CV_32S);
		cvZero(MatH);
		cvReduce(dst, MatH, 1, CV_REDUCE_SUM);
		
		std::vector<int> vtH;
		for (int i = 0; i < MatH->height; i++)
		{
			int iSum = MatH->data.i[i];
			vtH.push_back(iSum);
		}

		std::vector<int> vtH1(vtV.size(), 0);
		::XSysproxy().LinearSmooth7(vtH, vtH1, vtH.size());

		std::vector<int> vtMargin1;
		::XSysproxy().FindMargin(vtH1, vtMargin1, 10);

		// 显示水平直方图
		sz = cvGetSize(BinaryImage);
		//新建一个Mat用于储存投影直方图并将背景置为白色
		Mat horizontalProjectionMat(sz.height, sz.width, CV_8UC1);
		for (int i = 0; i < sz.height; i++) {			
			for (int j = 0; j < sz.width; j++) {
				uchar perPixelValue = 255; //背景设置为白色。   
				horizontalProjectionMat.at<uchar>(i, j) = perPixelValue;
			}
		}

		//将直方图的曲线设为黑色		
		for (int i = 0; i < sz.height; i++) {
			int width = vtH[i] > sz.width ? sz.width : vtH[i];
			for (int j = 0; j < width; j++) {
				uchar perPixelValue = 0; //直方图设置为黑色  
				horizontalProjectionMat.at<uchar>(i,j) = perPixelValue;
			}
		}
		
		cvNamedWindow("水平投影", CV_WINDOW_FREERATIO);
		imshow("水平投影", horizontalProjectionMat);
#endif 

	}	

	catch (cv::Exception ext)
	{
		const char* p = ext.msg.c_str();
	}

}	//end of void CHYImageFontBuilderDoc::OpcvTempTest1()

void CHYImageFontBuilderDoc::OpcvDrawGrid(char* SrcImg,int TopEdge,int LeftEdge,int RightEdge,int BottomEdge,int Colum, int Row )
{
	IplImage* clrImage = cvLoadImage(SrcImg, CV_LOAD_IMAGE_GRAYSCALE);
	if (NULL == clrImage)
	{
		AfxMessageBox("图像读取失败！");
		return;
	}
	IplImage* BinaryImage = cvCreateImage(cvGetSize(clrImage), IPL_DEPTH_8U, 1);
	cvThreshold(clrImage, BinaryImage, 120, 255, CV_THRESH_BINARY);

	IplImage* dst = cvCreateImage(cvGetSize(BinaryImage), IPL_DEPTH_8U,1); //创建目的图像
	int i = 0, j = 0;
	uchar* srcData = (uchar*)BinaryImage->imageData;
	uchar* dstData = (uchar*)dst->imageData;
	uchar srcpix = 0;

	int linebytes = BinaryImage->widthStep;	
	for (i = 0; i < BinaryImage->height; i++)
	{
		for (j = 0; j < BinaryImage->width; j++)
		{
			srcpix = 0;
			srcpix = (uchar)srcData[i*linebytes + j];

			if (i<TopEdge || j<LeftEdge || i> BinaryImage->height - BottomEdge || j > BinaryImage->width - RightEdge)
				dstData[i*linebytes + j] = 0;
			else
				dstData[i*linebytes + j] = 255 - srcpix;
		}
	}
	cvSaveImage("d:\\白黑图.jpg", dst);

	CvSize sz = cvGetSize(BinaryImage);	
	sz.width = 1;
	CvMat* MatH = cvCreateMat(sz.height, sz.width, CV_32S);
	cvZero(MatH);
	cvReduce(dst, MatH, 1, CV_REDUCE_AVG);
	std::vector<int> vtH;
	for (int i = 0; i < MatH->height; i++){
		int iSum = MatH->data.i[i];
		vtH.push_back(iSum);
	}
	std::vector<int> vtH1(vtH.size(), 0);
	::XSysproxy().LinearSmooth7(vtH, vtH1, vtH.size());
	std::vector<int> vtRowMargin;
	::XSysproxy().FindMargin(vtH1, vtRowMargin, Row);	

	// 按列处理
	for (size_t Row = 0; Row<vtRowMargin.size();)
	{
		int p1 = vtRowMargin[Row++];
		int p2 = vtRowMargin[Row++];

		CvSize SubImgSize;
		SubImgSize.width = BinaryImage->width;
		SubImgSize.height = p2 - p1;
		IplImage* SubImg = cvCreateImage(SubImgSize, IPL_DEPTH_8U, 1);
		uchar* srcData = (uchar*)BinaryImage->imageData;
		uchar* SubImgData = (uchar*)SubImg->imageData;
		ZeroMemory(SubImgData, SubImg->imageSize);
		int linebytes = SubImg->widthStep;
		int SubRow = 0;
		for (int y = p1; y < p2; y++)	{	
			for (int x = 0; x < BinaryImage->width; x++)	{				
				srcpix = (uchar)srcData[y*linebytes + x];
				if (y == p1 || x == 0 || y == p2 - 1 || x == SubImg->width - 1)
					SubImgData[SubRow*linebytes + x] = 0;
				else
					SubImgData[SubRow*linebytes + x] = 255 - srcpix;
			}
			SubRow++;
		}

		CvSize sz = cvGetSize(SubImg);
		sz.height = 1;
		CvMat* MatCol = cvCreateMat(sz.height, sz.width, CV_32S);
		cvZero(MatCol);
		cvReduce(SubImg, MatCol, 0, CV_REDUCE_AVG);
		std::vector<int> vtCol;
		for (int i = 0; i < MatCol->width; i++) {
			int iSum = MatCol->data.i[i];
			vtCol.push_back(iSum);
		}
		std::vector<int> vtCol1(vtCol.size(), 0);
		::XSysproxy().LinearSmooth7(vtCol, vtCol1, vtCol.size());
		std::vector<int> vtColMargin;
		::XSysproxy().FindMargin(vtCol1, vtColMargin, Colum);
		
		size_t stColNums = vtColMargin.size();
		for (size_t i = 0; i < stColNums; i++)
		{
			CvPoint pt1(vtColMargin[i], p1);
			CvPoint pt2(vtColMargin[i], p2);
			cvLine(BinaryImage, pt1, pt2, cv::Scalar(0, 0, 255), 2);
		}		
		CString strFileName;
		strFileName.Format("d:\\SubImg%02d.jpg", Row);
		cvSaveImage(strFileName, SubImg);
		int iJinJunTest = 0;
	}
	
	//画线存图
	sz = cvGetSize(dst);
	IplImage* Grid = cvCreateImage(sz, 8, 3);
	size_t stColRows = vtRowMargin.size();
	for (size_t i = 0; i < stColRows; i++)
	{
	CvPoint pt1(0, vtRowMargin[i]);
	CvPoint pt2(sz.height, vtRowMargin[i]);
	cvLine(BinaryImage, pt1, pt2, cv::Scalar(0, 0, 255), 2);
	}
	cvSaveImage("d:\\Grid.jpg", BinaryImage);
	cvReleaseImage(&dst);
	cvReleaseImage(&BinaryImage);
	cvReleaseImage(&clrImage);	

}	// end of void CHYImageFontBuilderDoc::OpcvDrawGrid()

void CHYImageFontBuilderDoc::OpcvImgTest(char* SrcImg, int MinArea, int MaxArea, int MinAngle, int MaxAngle)
{
	IplImage* clrImage = cvLoadImage(SrcImg, CV_LOAD_IMAGE_GRAYSCALE);
	if (NULL == clrImage)
	{
		AfxMessageBox("图像读取失败！");
		return;
	}

	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* squares = findSquares4(clrImage, storage, MinArea, MaxArea, MinAngle, MaxAngle);
	cvReleaseImage(&clrImage);
	cvClearMemStorage(storage);  //清空存储

	IplImage* gryImage = cvLoadImage(SrcImg, CV_LOAD_IMAGE_COLOR);
	if (NULL == gryImage)
	{
		AfxMessageBox("图像读取失败！");
		return;
	}
	cvNamedWindow("Square Detection Demo", CV_WINDOW_NORMAL);
	drawSquares(gryImage, squares);

	cvResizeWindow("Square Detection Demo", 800, 800);
	cvShowImage("Square Detection Demo", gryImage);
	cvReleaseImage(&gryImage);

}	// end of void CHYImageFontBuilderDoc::OpcvTest()

void CHYImageFontBuilderDoc::OpcvImgTest1(CStringArray& szFileName, int MinArea, int MaxArea, int MinAngle, int MaxAngle)
{
	int iszFileNameNums = szFileName.GetCount();
	for (int i = 0; i < iszFileNameNums; i++)
	{
		char* filename = (LPTSTR)(LPCTSTR)szFileName[i];
		IplImage* clrImage = cvLoadImage(filename, CV_LOAD_IMAGE_GRAYSCALE);
		CString strMsg;
		if (NULL == clrImage)
		{	
			strMsg.Format(_T("%s 读取失败!"), filename);
			::XSysproxy().m_strBulidInf = strMsg;
			::XSysproxy().UpdateAllViews(MSG_TRACE_BUILD_ADD_INF, 0);
			continue;
		}

		CvMemStorage* storage = cvCreateMemStorage(0);
		CvSeq* squares = findSquares4(clrImage, storage, MinArea, MaxArea, MinAngle, MaxAngle);		
		
		CvSeqReader reader;
		cvStartReadSeq(squares, &reader, 0);

		int iRectNums = squares->total / 4;
		strMsg.Format(_T("文件%s 矩形数量 %d "), filename, iRectNums);
		::XSysproxy().m_strBulidInf = strMsg;
		::XSysproxy().UpdateAllViews(MSG_TRACE_BUILD_ADD_INF, 0);

		cvReleaseImage(&clrImage);
		cvClearMemStorage(storage);  //清空存储
	}

}	// end of void CHYImageFontBuilderDoc::OpcvImgTest1()

void CHYImageFontBuilderDoc::makeMatirxCode(std::vector<CvRect>& vtGlyphBound, CString& strMatirxInf)
{
	CvRect  cvMaxRct;
	size_t	stGlyphNum = vtGlyphBound.size();	
	if (stGlyphNum == 0) return;

	int iMaxWD = vtGlyphBound[0].width;

	int iMinX = vtGlyphBound[0].x;
	int iMinXIndex = 0;
	for (size_t i = 1; i < stGlyphNum; i++)
	{	
		if (vtGlyphBound[i].width > iMaxWD)
			iMaxWD = vtGlyphBound[i].width;
		if (vtGlyphBound[i].x < iMinX)
		{
			iMinX = vtGlyphBound[i].x;
			iMinXIndex = i;
		}
	}

	CString strTmp;
	if (iMaxWD % 2 != 0)
		iMaxWD += 1;
	strTmp.Format("iiWidth = %d;\r\n", iMaxWD);	
	strMatirxInf += strTmp;

	std::vector<int> vtY;
	int y1 = 0;
	for (size_t i = 0; i < stGlyphNum; i++)
	{
		if (i == 0)
		{
			vtY.push_back(vtGlyphBound[i].y);
			y1 = vtGlyphBound[i].y;
		}
		else
		{
			if (vtGlyphBound[i].y - y1 > iMaxWD / 2)
			{
				vtY.push_back(vtGlyphBound[i].y);
				y1 = vtGlyphBound[i].y;
			}
		}
	}

	std::vector<int> vtX;
	y1 = 0;
	for (size_t i = iMinXIndex; i < stGlyphNum; i++)
	{
		if (i == iMinXIndex)
		{
			vtX.push_back(vtGlyphBound[i].x);
			y1 = vtGlyphBound[i].y;
		}
		else
		{
			if (vtGlyphBound[i].y - y1 > iMaxWD / 2)			
				break;
			vtX.push_back(vtGlyphBound[i].x);
		}
	}

	int iIndex=0;
	size_t stYNum = vtY.size();
	for (size_t i = 0; i<stYNum; i++)
	{
		strTmp.Format("Vline[%d]=%d;\r\n", iIndex, vtY[i]);
		strMatirxInf += strTmp;
		strTmp.Format("Vline[%d]= Vline[%d]+iiWidth;\r\n", iIndex+1, iIndex);
		strMatirxInf += strTmp;

		iIndex+=2;
	}

	strTmp.Format("for(int i=0; i<%d;i++)\r\n", stYNum);
	strMatirxInf += strTmp;

	strTmp.Format("{\r\n", stYNum);
	strMatirxInf += strTmp;

	iIndex = 0;
	size_t stXNum = vtX.size();
	for (size_t i = 0; i < stXNum; i++)
	{
		strTmp.Format("	Hline[%d]=%d;\r\n", iIndex, vtX[i]);
		strMatirxInf += strTmp;
		strTmp.Format("	Hline[%d]= Hline[%d]+iiWidth;\r\n", iIndex+1, iIndex);		
		strMatirxInf += strTmp;
		iIndex+=2;
	}
	strTmp.Format("}\r\n", stYNum);
	strMatirxInf += strTmp;

}	// end of void CHYImageFontBuilderDoc::makeMatirxCode()

void CHYImageFontBuilderDoc::OpcvMatirxCodeTest(char* SrcImg, int MinArea, int MaxArea, int MinAngle, int MaxAngle, CString& strMatirxInf)
{	
	IplImage* src = cvLoadImage(SrcImg, CV_LOAD_IMAGE_GRAYSCALE);
	if (NULL == src)
	{
		AfxMessageBox(_T("图像读取失败！"));

		::XSysproxy().m_strBulidInf = _T("图像读取失败！");
		::XSysproxy().UpdateAllViews(MSG_TRACE_BUILD_ADD_INF, 0);
		return;
	}

	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* squares = findSquares4(src, storage, MinArea, MaxArea, MinAngle, MaxAngle);

	CvSeqReader reader;
	cvStartReadSeq(squares, &reader, 0);

	std::vector<CvRect> vtGlyphBound;
	for (int i = 0; i < squares->total; i += 4)
	{
		CvPoint pt[4];
		int count = 4;
		memcpy(pt, reader.ptr, squares->elem_size);
		CV_NEXT_SEQ_ELEM(squares->elem_size, reader);
		memcpy(pt + 1, reader.ptr, squares->elem_size);
		CV_NEXT_SEQ_ELEM(squares->elem_size, reader);
		memcpy(pt + 2, reader.ptr, squares->elem_size);
		CV_NEXT_SEQ_ELEM(squares->elem_size, reader);
		memcpy(pt + 3, reader.ptr, squares->elem_size);
		CV_NEXT_SEQ_ELEM(squares->elem_size, reader);

		CvRect	rt;
		PointToRect(pt, rt);
		vtGlyphBound.push_back(rt);
	}

	SortFontBound(vtGlyphBound);	
	makeMatirxCode(vtGlyphBound, strMatirxInf);

	cvClearMemStorage(squares->storage);
	cvReleaseMemStorage(&storage);
	cvReleaseImage(&src);
	vtGlyphBound.clear();

}	// end of void CHYImageFontBuilderDoc::OpcvMatirxCodeTest()

void CHYImageFontBuilderDoc::OnImprtChrtbMu()
{
	TCHAR		szFilters[] = _T("字表文件(*.txt)|*.txt||");
	CFileDialog  openFileDlg(TRUE, _T(""), _T(""), OFN_READONLY | OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilters);
	if (openFileDlg.DoModal() != IDOK)	return;

	::XSysproxy().m_vtUnicode.clear();	
	CString strTxtPath = openFileDlg.GetPathName();
	::HY_TXTFileToUniArray((LPTSTR)(LPCTSTR)strTxtPath, ::XSysproxy().m_vtUnicode);

}	// end of void CHYImageFontBuilderDoc::OnImprtChrtbMu()

void CHYImageFontBuilderDoc::OnFileSave()
{
	if (m_FontEnCodec.m_vtHYGlyphs.size() == 0)
	{
		AfxMessageBox("字形数据不能为空!");
		return;
	}

	TCHAR		szFilters[] = _T("字库文件(*.ttf)|*.ttf||");
	CFileDialog  openFileDlg(FALSE, _T(""), _T(""), OFN_READONLY | OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilters);
	if (openFileDlg.DoModal() != IDOK)	return;

	// unicode编码匹配字形
	for (size_t i = 0; i < ::XSysproxy().m_vtUnicode.size(); i++)
	{
		if (i < m_FontEnCodec.m_vtHYGlyphs.size())
		{
			m_FontEnCodec.m_vtHYGlyphs[i].vtUnicode.push_back(::XSysproxy().m_vtUnicode[i]);
		}
	}

	//m_FontEnCodec.VerifyFont();
	
	size_t stGlyphNum = m_FontEnCodec.m_vtHYGlyphs.size();
	std::vector<unsigned long> ulTableFlag;
	ulTableFlag.push_back(CMAP_TAG);
	ulTableFlag.push_back(DSIG_TAG);	
	ulTableFlag.push_back(HEAD_TAG);
	ulTableFlag.push_back(HHEA_TAG);
	ulTableFlag.push_back(HMTX_TAG);
	ulTableFlag.push_back(MAXP_TAG);
	ulTableFlag.push_back(NAME_TAG);
	ulTableFlag.push_back(OS2_TAG);
	ulTableFlag.push_back(POST_TAG);
	ulTableFlag.push_back(GASP_TAG);
	ulTableFlag.push_back(GLYF_TAG);
	ulTableFlag.push_back(LOCA_TAG);
	ulTableFlag.push_back(PREP_TAG);

	CWaitCursor wc;
	CString strFontName = openFileDlg.GetPathName();
	m_FontEnCodec.MakeHYCodeMap();
	m_FontEnCodec.Encode((LPSTR)(LPCTSTR)strFontName, ulTableFlag);

}	// end of void CHYImageFontBuilderDoc::OnFileSave()
