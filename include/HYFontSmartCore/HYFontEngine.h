#pragma once

class AFX_EXT_CLASS CHYFontEngine
{
public:
	CHYFontEngine(void);
	virtual ~CHYFontEngine(void);

protected:
	void		InitParam();
	void		UninitParam();

public:
	void		fHengHT(int xmin, int xmax, int ymax, char stem, char flag, HY_POINT* pOutHYP, int& iOutPointsSize);
	void		fShuHT(int xmin, int ymin, int xmax, char stem, char flag, HY_POINT* pOutHYP, int& iOutPointsSize);
	void		fErDuoHT(int xmin, int ymin, int xmax, int ymax, char stem, char flag, char dpy1, char dpx2, HY_POINT* pOutHYP, int& iOutPointsSize);
	void		fPieHT(int xmin, int ymin, int xmax, int ymax, char stem, char flag, char cdx1, char cdy1, char cdx2, char cdy2, HY_POINT* pOutHYP, int& iOutPointsSize);
	void		fDianHT(int xmin, int ymin, int xmax, int ymax, char stem, char flag, char cdx, char cdy, HY_POINT* pOutHYP, int& iOutPointsSize);
	void		fNaHT(int xmin, int ymin, int xmax, int ymax, char stem, char flag, char cdx1, char cdy1, char cdx2, char cdy2,HY_POINT* pOutHYP, int& iOutPointsSize);
	void		fFengGaiHT(int xmin, int ymin, int xmax, int ymax, char stem, char delta1, char delta2, HY_POINT* pOutHYP, int& iOutPointsSize);
	void		fShuGouHT(int xmin, int ymin, int xmax, int ymax, char stem, char flag, HY_POINT* pOutHYP, int& iOutPointsSize);
	void		fWanGouHT(int xmin, int ymin, int xmax, int ymax, char stem, char flag, HY_POINT* pOutHYP, int& iOutPointsSize);
	void		fTiHT(int xmin, int ymin, int xmax, int ymax, char stem, char flag, HY_POINT* pOutHYP, int& iOutPointsSize);
	void		fZheHT(int xmin, int ymin, int xmax, int ymax, char stem, char flag, char dpx1, char dpy2, char dpx3, HY_POINT* pOutHYP, int& iOutPointsSize);
	void		fShuHengZheHT(int xmin, int ymin, int xmax, int ymax, char stem, char flag, char dpx1, char dpy2, HY_POINT* pOutHYP, int& iOutPointsSize);
	void		fHengShuZheHT(int xmin, int ymin, int xmax, int ymax, char stem, char flag, char dpx1, HY_POINT* pOutHYP, int& iOutPointsSize);
	void		fHengPieZheHT(int xmin, int ymin, int xmax, int ymax, char stem, char flag, char dpx, char cdx1, char cdy1, char cdx2, char cdy2,HY_POINT* pOutHYP, int& iOutPointsSize);
	void		fPieDianZheHT(int xmin, int ymin, int xmax, int ymax, char stem, char flag, char dpx1, char dpx2, char dpy2, char dpy3, HY_POINT* pOutHYP, int& iOutPointsSize);
	void		fPiePieZheHT(int xmin, int ymin, int xmax, int ymax, char stem, char flag, char dpx, char dpy, HY_POINT* pOutHYP, int& iOutPointsSize);
	void		fJiZheHT(int xmin, int ymin, int xmax, int ymax, char stem, char flag, char dx1, char dx2, HY_POINT* pOutHYP, int& iOutPointsSize);
	void		fHengShuGouHT(int xmin, int ymin, int xmax, int ymax, char stem, char flag, char dpx1, char dpx2, HY_POINT* pOutHYP, int& iOutPointsSize);
	void		fTiShuGouHT(int xmin, int ymin, int xmax, int ymax, char stem, char flag, char dpx1, char dpx2, char dpy, HY_POINT* pOutHYP, int& iOutPointsSize);
	void		fHengShuTiHT(int xmin, int ymin, int xmax, int ymax, char stem, char flag, char dpy1, char dpx2, HY_POINT* pOutHYP, int& iOutPointsSize);
	void		fFengGaiGouHT(int xmin, int ymin, int xmax, int ymax, char stem, char flag, char dpx1,char dpx2, char dpy3, HY_POINT* pOutHYP, int& iOutPointsSize);
	void		fFeiGouHT(int xmin, int ymin, int xmax, int ymax, char stem, char flag, char dpx, char dpy, HY_POINT* pOutHYP, int& iOutPointsSize);
	void		fFangGaiGouHT(int xmin, int ymin, int xmax, int ymax, char stem, char flag, char dpx1,char dpx2, char dpy3, char dpy4, HY_POINT* pOutHYP, int& iOutPointsSize);
	void		fNaGouHT(int xmin, int ymin, int xmax, int ymax, char stem, char flag, char dpy1, HY_POINT* pOutHYP, int& iOutPointsSize);
	void		fZhouzipanHT(int xmin, int ymin, int xmax, int ymax, char stem, char flag, HY_POINT* pOutHYP, int& iOutPointsSize);
	void		fBaogaiHT(int xmin, int ymin, int xmax, int ymax, char stem, char flag, char dpy1, HY_POINT* pOutHYP, int& iOutPointsSize);
	void		fYuegaiHT(int xmin, int ymin, int xmax, int ymax, char stem, char flag, char dpx1, char dpx2, char dpy2, HY_POINT* pOutHYP, int& iOutPointsSize);
	void		fNeikuanHT(int xmin, int ymin, int xmax, int ymax, char stem, char flag, char dpy1, char dpx2, HY_POINT* pOutHYP, int& iOutPointsSize);
	void		fKouHT(int xmin, int ymin, int xmax, int ymax, char stem, char flag, HY_POINT* pOutHYP, int& iOutPointsSize);
	void		fChangHT(int xmin, int ymin, int xmax, int ymax, char stem, char flag, char dpx1, HY_POINT* pOutHYP, int& iOutPointsSize);
	void		fYuHT(int xmin, int ymin, int xmax, int ymax, char stem, char flag, char dpx1, char dpx2, char dpy2, HY_POINT* pOutHYP, int& iOutPointsSize);
	void		fZhizheHT(int xmin, int ymin, int xmax, int ymax, char stem, char flag, char dpx1, HY_POINT* pOutHYP, int& iOutPointsSize);
	void		fZhinaHT(int xmin, int ymin, int xmax, int ymax, char stem, char flag, HY_POINT* pOutHYP, int& iOutPointsSize);
	void		fPieshutiHT(int xmin, int ymin, int xmax, int ymax, char stem, char flag, char dpx1, char dpy1, HY_POINT* pOutHYP, int& iOutPointsSize);
	void		fErduoyouHT(int xmin, int ymin, int xmax, int ymax, char stem, char flag, char dpx1, char dpy1, HY_POINT* pOutHYP, int& iOutPointsSize);
	void		fShutiHT(int xmin, int ymin, int xmax, int ymax, char stem, char flag, char dpy, HY_POINT* pOutHYP, int& iOutPointsSize);
	void		fXunzheHT(int xmin, int ymin, int xmax, int ymax, char stem, char flag, HY_POINT* pOutHYP, int& iOutPointsSize);
	void		fRunaHT(int xmin, int ymin, int xmax, int ymax, char stem, char flag, HY_POINT* pOutHYP, int& iOutPointsSize);
	void		fYiHT(int xmin, int ymin, int xmax, int ymax, char stem, char flag, char dpx1,char dpx2, char dpx3, HY_POINT* pOutHYP, int& iOutPointsSize);
	void		fJiuzheHT(int xmin, int ymin, int xmax, int ymax, char stem, char flag, char dpy1, char dpx2, char dpx3, char dpy4, HY_POINT* pOutHYP, int& iOutPointsSize);
	void		fShuxieHT(int xmin, int ymin, int ymax, char stem, char flag, char dpx1, HY_POINT* pOutHYP, int& iOutPointsSize);
	void		fPieduanHT(int xmin, int ymin, int xmax, int ymax, char stem, char flag, char cdx, char cdy, HY_POINT* pOutHYP, int& iOutPointsSize);
	void		fHuizheHT(int xmin, int ymin, int xmax, int ymax, char stem, char flag, char dpx1, HY_POINT* pOutHYP, int& iOutPointsSize);
	void		fLvzheHT(int xmin, int ymin, int xmax, int ymax, char stem, char flag, char dpx, HY_POINT* pOutHYP, int& iOutPointsSize);
	void		fShantuoHT(int xmin, int ymin, int xmax, int ymax, char stem, char flag, char dpy, HY_POINT* pOutHYP, int& iOutPointsSize);
	void		fShuwangouHT(int xmin, int ymin, int xmax, int ymax, char stem, char flag, char dpy, HY_POINT* pOutHYP, int& iOutPointsSize);
	void		fHengxieHT(int xmin, int ymin, int xmax, char stem, char flag, char dpy1, HY_POINT* pOutHYP, int& iOutPointsSize);

	int 		FilterParam(char code);
	void		showStroke(char code, int xmin, int ymin, int xmax, int ymax,char stem, char flag, char d1, char d2, char d3, char d4, char d5, char d6, char d7, char d8, HY_POINT* pOutHYP, int& iOutPointsSize);
	void		getHYFmtData(char* buffer, int& bufSize, HY_DF_PARAM* pDFParam, CRect* pFontRc);
	int			RecToChar(char cfcode, char mflag, int xmin, int ymin, int xmax, int ymax, char* buff);
	int			KeepinRectangle(int v, int min,int max);

	char		CHYFontEngine::GetAngle(int xmin, int ymin, int xmax, int ymax, int stem);
	int			CHYFontEngine::MySin(char angle);
	int			CHYFontEngine::MyCos(char angle);
};
