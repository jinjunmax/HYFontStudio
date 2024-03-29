
// HYImageFontBuilderDoc.h: CHYImageFontBuilderDoc 类的接口
//
#pragma once

typedef struct
{
	CvPoint cP;
	int height;
	int width;

} RecP;

class CHYImageFontBuilderDoc : public CDocument
{
public: // 仅从序列化创建
	CHYImageFontBuilderDoc();
	DECLARE_DYNCREATE(CHYImageFontBuilderDoc)

// 特性
public:

// 操作
public:

// 重写
public:
	virtual BOOL OnNewDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);

	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 实现
public:
	virtual ~CHYImageFontBuilderDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 用于为搜索处理程序设置搜索内容的 Helper 函数
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS

public:	
	void	OpcvImgTest(char* SrcImg, int MinArea, int MaxArea, int MinAngle, int MaxAngle);
	void	OpcvTempTest(char* SrcImg, int MinArea, int MaxArea, int MinAngle, int MaxAngle);
	void	OpcvTempTest1(char* SrcImg);
	void	OpcvDrawGrid(char* SrcImgint,int TopEdge, int LeftEdge, int RightEdge, int BottomEdge, int Colum, int Row);

	void	OpcvImgTest1(CStringArray& szFileName, int MinArea, int MaxArea, int MinAngle, int MaxAngle);
	void	makeMatirxCode(std::vector<CvRect>& vtGlyphBound, CString& strMatirxInf);
	void	OpcvMatirxCodeTest(char* SrcImg, int MinArea, int MaxArea, int MinAngle, int MaxAngle, CString& strMatirxInf);

	double	angle(CvPoint* pt1, CvPoint* pt2, CvPoint* pt0);
	CvSeq*	findSquares4(IplImage* img, CvMemStorage* storage, int minarea, int maxarea, int minangle, int maxangle);
	void	drawSquares(IplImage* img, CvSeq* squares);

public:
	afx_msg void OnImgTest();

public:	
	void	PointToRect(CvPoint* pt, CvRect &rt);
	void	SortFontBound(std::vector<CvRect>& vtRct);

public:
	CHYFontCodec				m_FontEnCodec;
	CHYImageToFont				m_ImageToFont;

	afx_msg void OnImprtCdtbMu();
	afx_msg void OnImprtChrtbMu();
	afx_msg void OnFileSave();
};
CHYImageFontBuilderDoc&	XDoc();
CHYImageFontBuilderDoc*	PDoc();