#pragma once
#include "xsysbaseproxy.h"

class CXSysProxy :	public CXSysBaseProxy
{
public:
	CXSysProxy(void);
	~CXSysProxy(void);

public:
	BOOL	MToWChar(WCHAR* pDest, int& iDestlen, char* pSrc);

	void	offsetGlyph(CHYGlyph& glyph, int ioffset);
	void	AdjustLsb(CHYFontCodec& FntCdc);
	int		PrepareFont(const char* strFontFile, CHYFontCodec& FontDecode);
	bool	IsContainHinting(CString strFontFile);

	int		BulidFont(CHYFontCodec& FontCodec, const char* strNewTTFFile);
	void	WriteTableEntry(FILE* pFile,CHYTableDirectory&	HYTbDirectory);
	int		Bulidhead(CHYFontCodec& FontCodec, FILE* pCreatFile);
	int		BulidLOCAL(CHYFontCodec& FontCodec,FILE* pCreatFile);
	int		Bulidmaxp(CHYFontCodec& FontCodec,FILE* pCreatFile);
	int		BulidHHEA(CHYFontCodec& FontCodec,FILE* pCreatFile);
	int 	BulidHMTX(CHYFontCodec& FontCodec,FILE* pCreatFile);
	int		BulidGlyph(CHYFontCodec& FontCodec, FILE* pCreatFile);
};

CXSysProxy&	XSysproxy(); 
CXSysProxy*	PXSysproxy();