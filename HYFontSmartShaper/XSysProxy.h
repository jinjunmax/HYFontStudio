#pragma once
#include "xsysbaseproxy.h"

class FntCheckResult
{
public:
	FntCheckResult();
	~FntCheckResult();
public:
	int					iGlyphIndex;
	int					iFlag;	// 0 点数量不匹配， 1 轮廓数量不匹配， 2点数量匹配但是点属性对应不上。
	std::vector<int>	vtPointIndex;

}; // end of class FntCheckResult

class CXSysProxy :	public CXSysBaseProxy
{
public:
	CXSysProxy(void);
	~CXSysProxy(void);

public:
	void MakeFontName(CHYFontCodec& FnCdc);
	BOOL MToWChar(WCHAR* pDest, int& iDestlen, char* pSrc);
	void TrimTTFContour(const CHYContour& inCntur, CHYContour& OutCntur);
	void CheckFont(HYFONTCODEC::CHYFontCodec& baseFntCodec, HYFONTCODEC::CHYFontCodec& inFntCodec, std::vector<FntCheckResult>& vtResult);	
	void SumVrGlyphs(std::vector<CVrGlyphs>& invtVrGlyphs, CVrGlyphs& outVrGlyph);
	void SubVrGlyphs(CVrGlyphs& inVrGlyph1, CVrGlyphs& inVrGlyph2, CVrGlyphs& outVrGlyph);

	void InitEncodeOption(CHYFontCodec& Encode);
	void SetEncodeOption(CHYFontCodec& Encode, CHYFontCodec& Original);

public:
	CVrAxisRecords		AxisRecords;
	CVrMasters			Masters;
	CVrInstances		Instances;
	CVrFontName			VrFntName;
};

CXSysProxy&	XSysproxy(); 
CXSysProxy*	PXSysproxy();