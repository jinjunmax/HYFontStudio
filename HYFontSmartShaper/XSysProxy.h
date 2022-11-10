#pragma once
#include "xsysbaseproxy.h"

class FntCheckResult
{
public:
	FntCheckResult();
	~FntCheckResult();
public:
	int					iGlyphIndex;
	int					iFlag;	// 0 ��������ƥ�䣬 1 ����������ƥ�䣬 2������ƥ�䵫�ǵ����Զ�Ӧ���ϡ�
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