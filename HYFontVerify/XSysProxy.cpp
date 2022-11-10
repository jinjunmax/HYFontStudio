#include "stdafx.h"
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

void CXSysProxy::offsetGlyph(CHYGlyph& glyph, int ioffset)
{

	glyph.minX += ioffset;
	glyph.maxX += ioffset;

	size_t stContours = glyph.vtContour.size();
	for (size_t i = 0; i < stContours; i++)
	{
		CHYContour&  Cntur = glyph.vtContour[i];
		size_t stPnt = Cntur.vtHYPoints.size();
		for (size_t j = 0; j < stPnt; j++){
			 CHYPoint& pt = Cntur.vtHYPoints[j];
			 pt.x += ioffset;
		}
	}

}	// end of void CXSysProxy::offsetGlyph()

void CXSysProxy::AdjustLsb(CHYFontCodec& FntCdc)
{
	int iTmpAdw = 0;
	std::vector<HMTX_LONGHORMERTRIC>	vtTmpLonghormetric;

	int GID = 0;	

	std::vector<unsigned long> vtUnicode;
	for (int i = 0; i < FntCdc.m_HYHhea.numberOfHMetrics; i++)
	{	
		HMTX_LONGHORMERTRIC tmpMERTRIC = FntCdc.m_HYHmtx.vtLonghormetric[i];
		iTmpAdw = tmpMERTRIC.advanceWidth;
		if (FntCdc.m_HYHmtx.vtLonghormetric[i].lsb < 0) {
			FntCdc.FindGryphUncidoByIndex(GID, vtUnicode);
			if (vtUnicode.size() > 0)
			{
				tmpMERTRIC.lsb = 0;
				tmpMERTRIC.advanceWidth = FntCdc.m_HYHmtx.vtLonghormetric[i].advanceWidth - FntCdc.m_HYHmtx.vtLonghormetric[i].lsb;
				offsetGlyph(FntCdc.m_vtHYGlyphs[GID], abs(FntCdc.m_HYHmtx.vtLonghormetric[i].lsb));
			}				
		}
		vtTmpLonghormetric.push_back(tmpMERTRIC);
		GID++;
	}

	for (size_t i = 0; i < FntCdc.m_HYHmtx.vtLeftsidebearing.size(); i++)
	{
		HMTX_LONGHORMERTRIC tmpMERTRIC;
		short slsb = FntCdc.m_HYHmtx.vtLeftsidebearing[i];
		
		if (slsb < 0){
			FntCdc.FindGryphUncidoByIndex(GID, vtUnicode);
			if (vtUnicode.size() > 0)
			{ 
				tmpMERTRIC.advanceWidth = iTmpAdw - slsb;
				tmpMERTRIC.lsb = 0;

				offsetGlyph(FntCdc.m_vtHYGlyphs[GID], abs(FntCdc.m_HYHmtx.vtLeftsidebearing[i]));
			}
		}
		else{
			tmpMERTRIC.advanceWidth = iTmpAdw;
			tmpMERTRIC.lsb = slsb;
		}

		vtTmpLonghormetric.push_back(tmpMERTRIC);
		GID++;
	}

	// 重新构建HHEA和HMTX表
	//numberOfHMetrics	
	FntCdc.m_HYHmtx.SetDefault();
	
	int szGlyphNum = (int)vtTmpLonghormetric.size();
	FntCdc.m_HYHhea.numberOfHMetrics = 0;

	int iMaxadw = 0;	
	int iMinlsb = 0;
	int iMinRsb = 0;
	int iMaxExtent = 0;
	if (szGlyphNum>0)
	{
		int				iBaseadvanceWidth = vtTmpLonghormetric[--szGlyphNum].advanceWidth;
		iMaxadw = iBaseadvanceWidth;
		iMinlsb = vtTmpLonghormetric[szGlyphNum].lsb;
		iMinRsb = vtTmpLonghormetric[szGlyphNum].advanceWidth - vtTmpLonghormetric[szGlyphNum].lsb -(FntCdc.m_vtHYGlyphs[szGlyphNum].maxX - FntCdc.m_vtHYGlyphs[szGlyphNum].minX);
		iMaxExtent = vtTmpLonghormetric[szGlyphNum].lsb + (FntCdc.m_vtHYGlyphs[szGlyphNum].maxX - FntCdc.m_vtHYGlyphs[szGlyphNum].minX);

		while (--szGlyphNum >= 0)
		{
			if (vtTmpLonghormetric[szGlyphNum].advanceWidth == iBaseadvanceWidth)
				FntCdc.m_HYHhea.numberOfHMetrics++;
			else
				break;

			if (vtTmpLonghormetric[szGlyphNum].advanceWidth > iMaxadw)
				iMaxadw = vtTmpLonghormetric[szGlyphNum].advanceWidth;

			if (vtTmpLonghormetric[szGlyphNum].lsb < iMinlsb)
				iMinlsb = vtTmpLonghormetric[szGlyphNum].lsb;		

			int itmp = vtTmpLonghormetric[szGlyphNum].advanceWidth - vtTmpLonghormetric[szGlyphNum].lsb - (FntCdc.m_vtHYGlyphs[szGlyphNum].maxX - FntCdc.m_vtHYGlyphs[szGlyphNum].minX);
			if (itmp < iMinRsb) iMinRsb = itmp;

			itmp = vtTmpLonghormetric[szGlyphNum].lsb + (FntCdc.m_vtHYGlyphs[szGlyphNum].maxX - FntCdc.m_vtHYGlyphs[szGlyphNum].minX);
			if (itmp > iMaxExtent) iMaxExtent  = itmp;

		}

		FntCdc.m_HYHhea.numberOfHMetrics = (unsigned short)vtTmpLonghormetric.size() - FntCdc.m_HYHhea.numberOfHMetrics;

		FntCdc.m_HYHhea.advanceWidthMax = iMaxadw;
		FntCdc.m_HYHhea.minLeftSideBearing = iMinlsb;
		FntCdc.m_HYHhea.minRightSideBearing = iMinRsb;
		FntCdc.m_HYHhea.xMaxExtent = iMaxExtent;

		for (int i = 0; i < FntCdc.m_HYHhea.numberOfHMetrics; i++){
			FntCdc.m_HYHmtx.vtLonghormetric.push_back(vtTmpLonghormetric[i]);
		}

		for (int i = FntCdc.m_HYHhea.numberOfHMetrics; i < FntCdc.m_HYMaxp.numGlyphs; i++){
			FntCdc.m_HYHmtx.vtLeftsidebearing.push_back(vtTmpLonghormetric[i].lsb);
		}
	}

}	// end of void	CXSysProxy::AdjustAdw()


int  CXSysProxy::PrepareFont(const char* strFontFile, CHYFontCodec& FntCdc)
{
	int err = 0;
	int iFilestat = ::HY_FileExist(strFontFile);
	if (iFilestat == 0)
		return HY_ERR_FILE_ZERO;
	if (iFilestat == -1)
		return HY_ERR_FILE_NOEXIST;
	if (iFilestat == -2)
		return HY_ERR_FILE_STAT;
	if (iFilestat == -3)
		return HY_ERR_FILE_OTHER;

	if (FntCdc.OpenFile(strFontFile) == HY_NOERROR)
	{
		FntCdc.DecodeTableDirectory();
		if (FntCdc.m_HYTbDirectory.version.value != 1 && FntCdc.m_HYTbDirectory.version.fract != 0)
			return FONT_ERR_NO_TTF;	// 不是truetype		
		
		if (FntCdc.m_HYTbDirectory.FindTableEntry(HEAD_TAG) == -1)		return FONT_ERR_HEAD_DECODE;
		FntCdc.Decodehead();
		if (FntCdc.m_HYTbDirectory.FindTableEntry(MAXP_TAG) == -1)		return FONT_ERR_MAXP_DECODE;
		FntCdc.Decodemaxp();
		if (FntCdc.m_HYTbDirectory.FindTableEntry(LOCA_TAG) == -1)		return FONT_ERR_LOCA_DECODE;
		FntCdc.Decodeloca();
		if (FntCdc.m_HYTbDirectory.FindTableEntry(HHEA_TAG) == -1)		return FONT_ERR_HHEA_DECODE;
		FntCdc.Decodehhea();
		if (FntCdc.m_HYTbDirectory.FindTableEntry(HMTX_TAG) == -1)		return FONT_ERR_HMTX_DECODE;
		FntCdc.Decodehmtx();
		if (FntCdc.m_HYTbDirectory.FindTableEntry(CMAP_TAG) == -1)		return FONT_ERR_CMAP_DECODE;
		FntCdc.Decodecmap();
		if (FntCdc.m_HYTbDirectory.FindTableEntry(GLYF_TAG) == -1 )		return FONT_ERR_GLYF_DECODE;
		else 
		{ 
			int iEntryIndex = FntCdc.m_HYTbDirectory.FindTableEntry(GLYF_TAG);
			CHYTableEntry& tbEntry = FntCdc.m_HYTbDirectory.vtTableEntry[iEntryIndex];			
			fseek(FntCdc.m_pFontFile, tbEntry.offset, SEEK_SET);
			FntCdc.m_vtHYGlyphs.resize(FntCdc.m_HYMaxp.numGlyphs);
			FntCdc.Decodeglyf();
		}
		// remove hinting
		if (FntCdc.m_HYTbDirectory.FindTableEntry(FPGM_TAG) != -1) {
			int iIndex = FntCdc.m_HYTbDirectory.FindTableEntry(FPGM_TAG);
			FntCdc.m_HYTbDirectory.vtTableEntry.erase(FntCdc.m_HYTbDirectory.vtTableEntry.begin() + iIndex);
			FntCdc.m_HYTbDirectory.numTables--;
		}
		if (FntCdc.m_HYTbDirectory.FindTableEntry(CVT_TAG) != -1) {
			int iIndex = FntCdc.m_HYTbDirectory.FindTableEntry(CVT_TAG);
			FntCdc.m_HYTbDirectory.vtTableEntry.erase(FntCdc.m_HYTbDirectory.vtTableEntry.begin() + iIndex);
			FntCdc.m_HYTbDirectory.numTables--;
		}		
		if (FntCdc.m_HYTbDirectory.FindTableEntry(PREP_TAG) != -1) {
			int iIndex = FntCdc.m_HYTbDirectory.FindTableEntry(PREP_TAG);
			FntCdc.m_HYTbDirectory.vtTableEntry.erase(FntCdc.m_HYTbDirectory.vtTableEntry.begin() + iIndex);
			FntCdc.m_HYTbDirectory.numTables--;
		}		
	}

	return HY_NOERROR;

}	// end of void  CXSysProxy::PrepareFont()

bool CXSysProxy::IsContainHinting(CString strFontFile)
{
	CHYFontCodec FntDecode;

	if (FntDecode.OpenFile(strFontFile) == HY_NOERROR)
	{
		FntDecode.DecodeTableDirectory();
		if (FntDecode.m_HYTbDirectory.FindTableEntry(CVT_TAG) != -1)		return true;
		if (FntDecode.m_HYTbDirectory.FindTableEntry(FPGM_TAG) != -1)		return true;
		if (FntDecode.m_HYTbDirectory.FindTableEntry(PREP_TAG) != -1)		return true;
	}	
	FntDecode.CloseFile();

	return false;	

}	// end of bool CXSysProxy::IsContainHinting()

int CXSysProxy::BulidFont(CHYFontCodec& FontCodec, const char* strNewTTFFile)
{
	FILE* pCreatFile = NULL;
	int err;
	pCreatFile = fopen(strNewTTFFile, "wb");
	if (pCreatFile == 0)
	{
		_get_errno(&err);
		_set_errno(0);
		return err;
	}

	WriteTableEntry(pCreatFile, FontCodec.m_HYTbDirectory);
	for (size_t i = 0; i != FontCodec.m_HYTbDirectory.vtTableEntry.size(); i++)
	{
		CHYTableEntry&	HYEntry = FontCodec.m_HYTbDirectory.vtTableEntry[i];
		HYEntry.offset = 0;
		HYEntry.length = 0;

		if (HYEntry.tag == LOCA_TAG){
			BulidLOCAL(FontCodec, pCreatFile);
		}
		else if (HYEntry.tag == GLYF_TAG){
			BulidGlyph(FontCodec, pCreatFile);

		}
		else if (HYEntry.tag == HEAD_TAG){
			Bulidhead(FontCodec, pCreatFile);
		}
		else if (HYEntry.tag == MAXP_TAG){
			Bulidmaxp(FontCodec, pCreatFile);
		}		
		else if (HYEntry.tag == HHEA_TAG){
			BulidHHEA(FontCodec, pCreatFile);
		}
		else if (HYEntry.tag == HMTX_TAG){
			BulidHMTX(FontCodec, pCreatFile);
		}		
		else{
			TableData* pTbData = FontCodec.m_mulpTableData.FindTableData(HYEntry.tag);
			if (pTbData){
				// 偏移
				HYEntry.offset = ftell(pCreatFile);
				HYEntry.length = pTbData->ulDataSize;

				for (int j = 0; j < HYEntry.length; j++)
				{
					fwrite(&pTbData->vtData[j], 1, 1, pCreatFile);
				}				

				int iTail = 4 - HYEntry.length % 4;
				if (HYEntry.length % 4>0){
					char c = 0;
					for (int j = 0; j<iTail; j++) {
						fwrite(&c, 1, 1, pCreatFile);
					}
				}
			}
		}
	}

	// 关闭源文件
	FontCodec.CloseFile();
	WriteTableEntry(pCreatFile, FontCodec.m_HYTbDirectory);
	// 关闭目标文件
	fflush(pCreatFile);
	fclose(pCreatFile);

	return FontCodec.SetCheckSumAdjustment(strNewTTFFile);

}	// end of int CXSysProxy::BulidFont()

bool CompareForSort(CHYTableEntry Entry1, CHYTableEntry Entry2)
{	
	int param1 = (Entry1.tag>>24);
	int param2 = (Entry2.tag>>24);	

	if (param2 > param1) return true;
	return false;
}

void CXSysProxy::WriteTableEntry(FILE* pFile, CHYTableDirectory&	HYTbDirectory)
{
	unsigned short	searchRange = 0;
	searchRange = (unsigned short)(log((double)HYTbDirectory.numTables) / log(2.0));
	searchRange = (unsigned short)(pow(2.0, searchRange));
	searchRange = searchRange * 16;

	HYTbDirectory.searchRange = searchRange;
	HYTbDirectory.entrySelector = (unsigned short)(log((float)(HYTbDirectory.searchRange / 16)) / log(2.0));
	HYTbDirectory.rangeShift = HYTbDirectory.numTables * 16 - HYTbDirectory.searchRange;

	fseek(pFile, 0, SEEK_SET);

	unsigned short	usTmp;
	unsigned long	ulTmp;
	// sfnt version
	usTmp = hy_cdr_int16_to(HYTbDirectory.version.value);
	fwrite(&usTmp, 2, 1, pFile);
	usTmp = hy_cdr_int16_to(HYTbDirectory.version.fract);
	fwrite(&usTmp, 2, 1, pFile);
	//numTables
	usTmp = hy_cdr_int16_to(HYTbDirectory.numTables);
	fwrite(&usTmp, 2, 1, pFile);
	// searchRange
	usTmp = hy_cdr_int16_to(HYTbDirectory.searchRange);
	fwrite(&usTmp, 2, 1, pFile);
	// entrySelector
	usTmp = hy_cdr_int16_to(HYTbDirectory.entrySelector);
	fwrite(&usTmp, 2, 1, pFile);
	//rangeShift
	usTmp = hy_cdr_int16_to(HYTbDirectory.rangeShift);
	fwrite(&usTmp, 2, 1, pFile);

	
	std::sort(HYTbDirectory.vtTableEntry.begin(), HYTbDirectory.vtTableEntry.end(), CompareForSort);
	for (int i = 0; i<HYTbDirectory.numTables; i++)
	{
		CHYTableEntry&	HYEntry = HYTbDirectory.vtTableEntry[i];

		//tag		
		ulTmp = hy_cdr_int32_to(HYEntry.tag);
		fwrite(&ulTmp, 4, 1, pFile);
		// checkSum
		ulTmp = hy_cdr_int32_to(HYEntry.checkSum);
		fwrite(&ulTmp, 4, 1, pFile);
		//offset
		ulTmp = hy_cdr_int32_to(HYEntry.offset);
		fwrite(&ulTmp, 4, 1, pFile);
		//length
		ulTmp = hy_cdr_int32_to(HYEntry.length);
		fwrite(&ulTmp, 4, 1, pFile);
	}

}	// end of void CXSysProxy::WriteTableEntry()

int CXSysProxy::Bulidhead(CHYFontCodec& FontCodec, FILE* pCreatFile)
{	
	unsigned short	usTmp = 0;
	unsigned long	ulTmp = 0;

	int iEntryIndex = FontCodec.m_HYTbDirectory.FindTableEntry(HEAD_TAG);
	if (iEntryIndex == -1) return FONT_ERR_HEAD_ENCODE;

	CHYTableEntry& tbEntry = FontCodec.m_HYTbDirectory.vtTableEntry[iEntryIndex];
	tbEntry.offset = ftell(pCreatFile);

	//Table version number
	usTmp = hy_cdr_int16_to(FontCodec.m_HYhead.version.value);
	fwrite(&usTmp, 2, 1, pCreatFile);
	usTmp = hy_cdr_int16_to(FontCodec.m_HYhead.version.fract);
	fwrite(&usTmp, 2, 1, pCreatFile);
	//fontRevision			
	usTmp = hy_cdr_int16_to(FontCodec.m_HYhead.fontRevision.value);
	fwrite(&usTmp, 2, 1, pCreatFile);
	usTmp = hy_cdr_int16_to(FontCodec.m_HYhead.fontRevision.fract);
	fwrite(&usTmp, 2, 1, pCreatFile);

	// checkSumAdjustment			
	ulTmp = 0;
	fwrite(&ulTmp, 4, 1, pCreatFile);

	// magicNumber
	FontCodec.m_HYhead.magicNumber = 0x5F0F3CF5;
	ulTmp = hy_cdr_int32_to(FontCodec.m_HYhead.magicNumber);
	fwrite(&ulTmp, 4, 1, pCreatFile);
	//flags
	usTmp = hy_cdr_int16_to(FontCodec.m_HYhead.flags);
	fwrite(&usTmp, 2, 1, pCreatFile);
	//unitsPerEm
	usTmp = hy_cdr_int16_to(FontCodec.m_HYhead.unitsPerEm);
	fwrite(&usTmp, 2, 1, pCreatFile);

	__time64_t	 tspace = 2082844799;
	CTime tCurTime = CTime::GetCurrentTime();
	__time64_t  t64CurTime = tCurTime.GetTime();
	t64CurTime += tspace;
	char p[8] = { 0 };
	CopyMemory((PBYTE)p, &t64CurTime, sizeof(__time64_t));

	for (int i = 0; i<8; i++){	
		FontCodec.m_HYhead.modified[i] = p[7 - i];
	}
	//created			
	fwrite(FontCodec.m_HYhead.created, 8, 1, pCreatFile);
	//modified
	fwrite(FontCodec.m_HYhead.modified, 8, 1, pCreatFile);

	//xMin
	usTmp = hy_cdr_int16_to(FontCodec.m_HYhead.xMin);
	fwrite(&usTmp, 2, 1, pCreatFile);
	// yMin
	usTmp = hy_cdr_int16_to(FontCodec.m_HYhead.yMin);
	fwrite(&usTmp, 2, 1, pCreatFile);
	//xMax
	usTmp = hy_cdr_int16_to(FontCodec.m_HYhead.xMax);
	fwrite(&usTmp, 2, 1, pCreatFile);
	// yMax
	usTmp = hy_cdr_int16_to(FontCodec.m_HYhead.yMax);
	fwrite(&usTmp, 2, 1, pCreatFile);

	// macStyle
	usTmp = hy_cdr_int16_to(FontCodec.m_HYhead.macStyle);
	fwrite(&usTmp, 2, 1, pCreatFile);
	//lowestRecPPEM
	usTmp = hy_cdr_int16_to(FontCodec.m_HYhead.lowestRecPPEM);
	fwrite(&usTmp, 2, 1, pCreatFile);
	//fontDirectionHint
	usTmp = hy_cdr_int16_to(FontCodec.m_HYhead.fontDirectionHint);
	fwrite(&usTmp, 2, 1, pCreatFile);
	//indexToLocFormat	
	usTmp = hy_cdr_int16_to(FontCodec.m_HYhead.indexToLocFormat);
	fwrite(&usTmp, 2, 1, pCreatFile);
	//glyphDataFormat	
	usTmp = hy_cdr_int16_to(FontCodec.m_HYhead.glyphDataFormat);
	fwrite(&usTmp, 2, 1, pCreatFile);

	tbEntry.length = ftell(pCreatFile) - tbEntry.offset;
	int iTail = 4 - tbEntry.length % 4;
	if (tbEntry.length % 4>0){
		char c = 0;
		for (int i = 0; i<iTail; i++){
			fwrite(&c, 1, 1, pCreatFile);
		}
	}	

	return HY_NOERROR;

}	// end of int CHYFontCodec::Encodehead()

int  CXSysProxy::BulidLOCAL(CHYFontCodec& FontCodec, FILE* pCreatFile)
{
	int iEntryIndex = FontCodec.m_HYTbDirectory.FindTableEntry(LOCA_TAG);
	if (iEntryIndex == -1) return FONT_ERR_LOCA_ENCODE;

	char*			pLocal = NULL;
	unsigned long	ulLocalIndex = 0;
	int				BuffLength = 0;

	size_t stNums = FontCodec.m_HYloca.vtLoca.size();
	if (FontCodec.m_HYhead.indexToLocFormat == 0)
	{
		BuffLength = (stNums * 2 + 3) / 4 * 4;
		pLocal = new char[BuffLength];
		memset(pLocal, 0, BuffLength);
		for (size_t i = 0; i != stNums; i++)
		{
			unsigned short soffset = hy_cdr_int16_to(FontCodec.m_HYloca.vtLoca[i] >> 1);
			memcpy(pLocal + ulLocalIndex, &soffset, 2);
			ulLocalIndex += 2;
		}
	}
	if (FontCodec.m_HYhead.indexToLocFormat == 1)
	{
		BuffLength = stNums * 4;
		pLocal = new char[BuffLength];
		memset(pLocal, 0, BuffLength);
		for (size_t i = 0; i != stNums; i++)
		{
			long loffset = hy_cdr_int32_to(FontCodec.m_HYloca.vtLoca[i]);
			memcpy(pLocal + ulLocalIndex, &loffset, 4);
			ulLocalIndex += 4;
		}
	}

	// 偏移
	CHYTableEntry& HYEntry = FontCodec.m_HYTbDirectory.vtTableEntry[iEntryIndex];
	HYEntry.offset = ftell(pCreatFile);		
	HYEntry.length = ulLocalIndex;
	HYEntry.checkSum = HY_CalcFontTableChecksum((unsigned long*)pLocal, ulLocalIndex);

	fwrite(pLocal, BuffLength, 1, pCreatFile);

	if (pLocal)
		delete[] pLocal;
	pLocal = NULL;

	return HY_NOERROR;

}	// end of void CXSysProxy::BulidLOCAL()

int	CXSysProxy::Bulidmaxp(CHYFontCodec& FontCodec, FILE* pCreatFile)
{
	if (pCreatFile)
	{
		unsigned short	usTmp = 0;
		unsigned long	ulTmp = 0;

		int iEntryIndex = FontCodec.m_HYTbDirectory.FindTableEntry(MAXP_TAG);
		if (iEntryIndex == -1) return FONT_ERR_MAXP_ENCODE;

		CHYTableEntry& tbEntry = FontCodec.m_HYTbDirectory.vtTableEntry[iEntryIndex];
		tbEntry.offset = ftell(pCreatFile);

		//Table version number		
		usTmp = hy_cdr_int16_to(FontCodec.m_HYMaxp.version.value);
		fwrite(&usTmp, 2, 1, pCreatFile);
		usTmp = hy_cdr_int16_to(FontCodec.m_HYMaxp.version.fract);
		fwrite(&usTmp, 2, 1, pCreatFile);

		//numGlyphs		
		usTmp = hy_cdr_int16_to(FontCodec.m_HYMaxp.numGlyphs);
		fwrite(&usTmp, 2, 1, pCreatFile);

		size_t szMaxPoints = 0, szMaxTContours = 0, szCntursNum = 0, szTmpPoint = 0;
		size_t maxCompositePoints = 0, maxCompositeContours = 0, maxComponentElements = 0, maxComponentDepth = 0;
		for (unsigned short i = 0; i<FontCodec.m_HYMaxp.numGlyphs; i++)
		{
			szTmpPoint = 0;
			CHYGlyph& glyph = FontCodec.m_vtHYGlyphs[i];
			szCntursNum = glyph.vtContour.size();
			if (szCntursNum == 0 && glyph.vtComponents.size() == 0) continue;

			if (szCntursNum > 0)
			{
				if (szMaxTContours<szCntursNum)
					szMaxTContours = szCntursNum;

				for (unsigned short j = 0; j<szCntursNum; j++)
				{
					szTmpPoint += glyph.vtContour[j].vtHYPoints.size();
				}

				if (szMaxPoints<szTmpPoint)
					szMaxPoints = szTmpPoint;
			}
			else
			{
				size_t  maxCmpnntElmTmp = glyph.vtComponents.size();
				size_t  maxCompositePointTmp = 0, maxCompositeContourTmp = 0;

				maxComponentDepth = 1;
				for (unsigned int j = 0; j<maxCmpnntElmTmp; j++)
				{
					CHYCmpst& cmpst = glyph.vtComponents[j];
					CHYGlyph& cmpGlyph = FontCodec.m_vtHYGlyphs[cmpst.glyphIndex];
					maxCompositePointTmp += cmpGlyph.GetTotalPointsNumber();
					maxCompositeContourTmp += cmpGlyph.vtContour.size();
				}

				if (maxCmpnntElmTmp>maxComponentElements)
					maxComponentElements = maxCmpnntElmTmp;
				if (maxCompositePointTmp > maxCompositePoints)
					maxCompositePoints = maxCompositePointTmp;
				if (maxCompositeContourTmp > maxCompositeContours)
					maxCompositeContours = maxCompositeContourTmp;
			}
		}
		//maxPoints
		FontCodec.m_HYMaxp.maxPoints = (unsigned short)szMaxPoints;
		usTmp = hy_cdr_int16_to(FontCodec.m_HYMaxp.maxPoints);
		fwrite(&usTmp, 2, 1, pCreatFile);
		//maxContours
		FontCodec.m_HYMaxp.maxContours = (unsigned short)szMaxTContours;
		usTmp = hy_cdr_int16_to(FontCodec.m_HYMaxp.maxContours);
		fwrite(&usTmp, 2, 1, pCreatFile);
		//maxCompositePoints				
		usTmp = hy_cdr_int16_to((unsigned short)maxCompositePoints);
		fwrite(&usTmp, 2, 1, pCreatFile);
		//maxCompositeContours
		usTmp = hy_cdr_int16_to((unsigned short)maxCompositeContours);
		fwrite(&usTmp, 2, 1, pCreatFile);
		//maxZones
		usTmp = hy_cdr_int16_to(2);
		fwrite(&usTmp, 2, 1, pCreatFile);
		//maxTwilightPoints
		usTmp = hy_cdr_int16_to(4);
		fwrite(&usTmp, 2, 1, pCreatFile);
		//maxStorage
		usTmp = hy_cdr_int16_to(32);
		fwrite(&usTmp, 2, 1, pCreatFile);
		//maxFunctionDefs
		usTmp = hy_cdr_int16_to(96);
		fwrite(&usTmp, 2, 1, pCreatFile);
		//maxInstructionDefs
		usTmp = hy_cdr_int16_to(96);
		fwrite(&usTmp, 2, 1, pCreatFile);
		//maxStackElements
		usTmp = hy_cdr_int16_to(256);
		fwrite(&usTmp, 2, 1, pCreatFile);
		//maxSizeOfInstructions
		usTmp = hy_cdr_int16_to(8);
		fwrite(&usTmp, 2, 1, pCreatFile);
		//maxComponentElements				
		usTmp = hy_cdr_int16_to(unsigned short(maxComponentElements));
		fwrite(&usTmp, 2, 1, pCreatFile);
		//maxComponentDepth
		usTmp = hy_cdr_int16_to(unsigned short(maxComponentDepth));
		fwrite(&usTmp, 2, 1, pCreatFile);
		

		tbEntry.length = ftell(pCreatFile) - tbEntry.offset;
		int iTail = 4 - tbEntry.length % 4;
		if (tbEntry.length % 4>0)
		{
			char c = 0;
			for (int i = 0; i<iTail; i++){
				fwrite(&c, 1, 1, pCreatFile);
			}
		}
	}

	return HY_NOERROR;

}	// end of int	CXSysProxy::Bulidmaxp()

int CXSysProxy::BulidHHEA(CHYFontCodec& FontCodec, FILE* pCreatFile)
{	
	unsigned short	usTmp = 0;
	unsigned long	ulTmp = 0;

	int iEntryIndex = FontCodec.m_HYTbDirectory.FindTableEntry(HHEA_TAG);
	if (iEntryIndex == -1) return FONT_ERR_HHEA_ENCODE;

	CHYTableEntry& tbEntry = FontCodec.m_HYTbDirectory.vtTableEntry[iEntryIndex];
	tbEntry.offset = ftell(pCreatFile);

	// version 	
	usTmp = hy_cdr_int16_to(FontCodec.m_HYHhea.version.value);
	fwrite(&usTmp, 2, 1, pCreatFile);	
	usTmp = hy_cdr_int16_to(FontCodec.m_HYHhea.version.fract);
	fwrite(&usTmp, 2, 1, pCreatFile);
	//Ascender	
	usTmp = hy_cdr_int16_to(FontCodec.m_HYHhea.Ascender);
	fwrite(&usTmp, 2, 1, pCreatFile);
	//Descender	
	usTmp = hy_cdr_int16_to(FontCodec.m_HYHhea.Descender);
	fwrite(&usTmp, 2, 1, pCreatFile);
	//LineGap			
	usTmp = hy_cdr_int16_to(FontCodec.m_HYHhea.LineGap);
	fwrite(&usTmp, 2, 1, pCreatFile);
	//advanceWidthMax			
	usTmp = hy_cdr_int16_to(FontCodec.m_HYHhea.advanceWidthMax);
	fwrite(&usTmp, 2, 1, pCreatFile);
	//minLeftSideBearing	
	usTmp = hy_cdr_int16_to(FontCodec.m_HYHhea.minLeftSideBearing);
	fwrite(&usTmp, 2, 1, pCreatFile);
	//minRightSideBearing
	usTmp = hy_cdr_int16_to(FontCodec.m_HYHhea.minRightSideBearing);
	fwrite(&usTmp, 2, 1, pCreatFile);
	//xMaxExtent	
	usTmp = hy_cdr_int16_to(FontCodec.m_HYHhea.xMaxExtent);
	fwrite(&usTmp, 2, 1, pCreatFile);
	//caretSlopeRise		
	usTmp = hy_cdr_int16_to(FontCodec.m_HYHhea.caretSlopeRise);
	fwrite(&usTmp, 2, 1, pCreatFile);
	//caretSlopeRun
	usTmp = hy_cdr_int16_to(FontCodec.m_HYHhea.caretSlopeRun);
	fwrite(&usTmp, 2, 1, pCreatFile);
	//caretOffset
	usTmp = hy_cdr_int16_to(FontCodec.m_HYHhea.caretOffset);
	fwrite(&usTmp, 2, 1, pCreatFile);

	usTmp = 0;
	//reserved1			
	fwrite(&usTmp, 2, 1, pCreatFile);
	//reserved2			
	fwrite(&usTmp, 2, 1, pCreatFile);
	//reserved3			
	fwrite(&usTmp, 2, 1, pCreatFile);
	//reserved4			
	fwrite(&usTmp, 2, 1, pCreatFile);
	//metricDataFormat
	usTmp = hy_cdr_int16_to(FontCodec.m_HYHhea.metricDataFormat);
	fwrite(&usTmp, 2, 1, pCreatFile);
	//numberOfHMetrics	
	usTmp = hy_cdr_int16_to(FontCodec.m_HYHhea.numberOfHMetrics);
	fwrite(&usTmp, 2, 1, pCreatFile);

	tbEntry.length = ftell(pCreatFile) - tbEntry.offset;
	int iTail = 4 - tbEntry.length % 4;
	if (tbEntry.length % 4>0)
	{
		char c = 0;
		for (int i = 0; i<iTail; i++){
			fwrite(&c, 1, 1, pCreatFile);
		}
	}	

	return FONT_ERR_HHEA_ENCODE;

}	// end of void CXSysProxy::BulidHHEA()

int CXSysProxy::BulidHMTX(CHYFontCodec& FontCodec, FILE* pCreatFile)
{
	unsigned short	usTmp = 0;
	unsigned long	ulTmp = 0;

	int iEntryIndex = FontCodec.m_HYTbDirectory.FindTableEntry(HMTX_TAG);
	if (iEntryIndex == -1) return FONT_ERR_HMTX_ENCODE;

	CHYTableEntry& tbEntry = FontCodec.m_HYTbDirectory.vtTableEntry[iEntryIndex];
	tbEntry.offset = ftell(pCreatFile);
	
	unsigned short longhormetricNums = FontCodec.m_HYHhea.numberOfHMetrics;
	unsigned short lefsidebearNums = FontCodec.m_HYMaxp.numGlyphs - FontCodec.m_HYHhea.numberOfHMetrics;

	for (size_t i = 0; i<longhormetricNums; i++)
	{	
		usTmp = hy_cdr_int16_to(FontCodec.m_HYHmtx.vtLonghormetric[i].advanceWidth);
		fwrite(&usTmp, 2, 1, pCreatFile);

		usTmp = hy_cdr_int16_to(FontCodec.m_HYHmtx.vtLonghormetric[i].lsb);
		fwrite(&usTmp, 2, 1, pCreatFile);
	}

	for (size_t i = 0; i < lefsidebearNums; i++)
	{
		usTmp = hy_cdr_int16_to(FontCodec.m_HYHmtx.vtLeftsidebearing[i]);
		fwrite(&usTmp, 2, 1, pCreatFile);
	}

	tbEntry.length = ftell(pCreatFile) - tbEntry.offset;
	int iTail = 4 - tbEntry.length % 4;
	if (tbEntry.length % 4>0){
		char c = 0;
		for (int i = 0; i<iTail; i++){
			fwrite(&c, 1, 1, pCreatFile);
		}
	}	

	return HY_NOERROR;

}	// end of void CXSysProxy::BulidHMTX()

int CXSysProxy::BulidGlyph(CHYFontCodec& FontCodec, FILE* pCreatFile)
{
	unsigned long	ulTmp = 0;
	unsigned short  usTmp = 0;
	
	FontCodec.m_HYloca.SetDefault();

	unsigned long ulTableBegin = ftell(pCreatFile);
	unsigned long ulGlyphOffset = 0;

	int iEntryIndex = FontCodec.m_HYTbDirectory.FindTableEntry(GLYF_TAG);
	if (iEntryIndex == -1)  return FONT_ERR_GLYF_ENCODE;
	CHYTableEntry& tbEntry = FontCodec.m_HYTbDirectory.vtTableEntry[iEntryIndex];
	tbEntry.offset = ftell(pCreatFile);

	for (size_t i = 0; i<FontCodec.m_HYMaxp.numGlyphs; i++)
	{
		FontCodec.m_HYloca.vtLoca.push_back(ulGlyphOffset);

		CHYGlyph& glyph = FontCodec.m_vtHYGlyphs[i];
		if (glyph.vtComponents.size() == 0 && glyph.vtContour.size() == 0){
			continue;
		}

		size_t szCnturSize = glyph.vtContour.size();
		if (glyph.vtContour.size()>0)	// 轮廓字形
		{
			//numberOfContours
			usTmp = szCnturSize;
			usTmp = hy_cdr_int16_to(usTmp);
			fwrite(&usTmp, 2, 1, pCreatFile);
			//xMin
			usTmp = hy_cdr_int16_to(glyph.minX);
			fwrite(&usTmp, 2, 1, pCreatFile);
			//yMin
			usTmp = hy_cdr_int16_to(glyph.minY);
			fwrite(&usTmp, 2, 1, pCreatFile);
			//xMax
			usTmp = hy_cdr_int16_to(glyph.maxX);
			fwrite(&usTmp, 2, 1, pCreatFile);
			//yMax
			usTmp = hy_cdr_int16_to(glyph.maxY);
			fwrite(&usTmp, 2, 1, pCreatFile);

			bool			bXIsBYTE = true, bYIsBYTE = true;
			CHYPoint		prePoint;
			std::vector<short> vtXCoordinat;
			std::vector<short> vtYCoordinat;
			std::vector<unsigned char>	vtflage;
			std::vector<unsigned short> vtendPtsOfContour;
			short	XOffset = 0, YOffset = 0;

			size_t  TatlPntNum = 0;
			for (size_t j = 0; j<glyph.vtContour.size(); j++){
				const CHYContour& cntur = glyph.vtContour[j];
				size_t stCutPntnum = cntur.vtHYPoints.size();
				for (size_t x = 0; x<stCutPntnum; x++)	{
					const CHYPoint& pt = cntur.vtHYPoints[x];
					unsigned char	flage = 0;

					if (pt.flag == POINT_FLG_ANCHOR){
						flage |= GLYF_SMPL_ONCURVE;
					}

					// 计算X轴坐标点偏移量
					XOffset = pt.x - prePoint.x;
					if (XOffset == 0)	{
						// 当前点与上一点x位置相同
						flage |= GLYF_SMPL_X_SAME_FLG;
					}
					else {
						// 如果偏移量的绝对值小于255，按单字节保存
						if (abs(XOffset) <= 255){
							// x-Short Vector位设置为1
							flage |= GLYF_SMPL_X_VECTOR;
							// 如果offset为正,This x is same表示偏移方向 1为正， 0为负
							if (XOffset>0) flage |= GLYF_SMPL_X_SAME_FLG;

							vtXCoordinat.push_back((short)abs(XOffset));
						}
						else {	// 按双字节保存
							vtXCoordinat.push_back(XOffset);
						}
					}

					// 计算Y轴坐标点偏移量				
					YOffset = pt.y - prePoint.y;
					if (YOffset == 0){
						// 当前点与上一点偏移量相同
						flage |= GLYF_SMPL_Y_SAME_FLG;
					}
					else{
						// 如果偏移量的绝对值小于255，按单字节保存
						if (abs(YOffset) <= 255){
							flage |= GLYF_SMPL_Y_VECTOR;

							// 如果offset为正,This x is same表示偏移方向 1为正， 0为负
							if (YOffset>0) flage |= GLYF_SMPL_Y_SAME_FLG;
							vtYCoordinat.push_back((short)abs(YOffset));
						}
						else {	// 按双字节保存
							vtYCoordinat.push_back(YOffset);
						}
					}
					prePoint = pt;
					vtflage.push_back(flage);
				}
				TatlPntNum += stCutPntnum;
				vtendPtsOfContour.push_back(TatlPntNum - 1);
			}

			//endPtsOfContours
			size_t stEndPtsCnturNum = vtendPtsOfContour.size();
			for (size_t y = 0; y<stEndPtsCnturNum; y++)	{
				usTmp = hy_cdr_int16_to(vtendPtsOfContour[y]);
				fwrite(&usTmp, 2, 1, pCreatFile);
			}

			//instructionLength					
			usTmp = hy_cdr_int16_to((unsigned short)glyph.vtInstruntions.size());
			fwrite(&usTmp, 2, 1, pCreatFile);

			//instructions[n]
			size_t stInstrSize = glyph.vtInstruntions.size();
			for (size_t y = 0; y<stInstrSize; y++)	{
				fwrite(&glyph.vtInstruntions[y], 1, 1, pCreatFile);
			}

			// 对Flag做行程压缩
			int				iRepeat = 0;
			unsigned char	btFlag = 0, btNextFlag = 0;
			std::vector<unsigned char>	vtRle;
			size_t	stflagsize = vtflage.size();

			size_t y = 0, z = 0;
			for (y = 0; y<stflagsize; y++){
				btFlag = vtflage[y];
				iRepeat = 0;
				for (z = y + 1; z<stflagsize; z++){
					if (iRepeat>255) break;
					btNextFlag = vtflage[z];

					if (btNextFlag != btFlag) break;
					iRepeat++;
				}

				if (iRepeat>0){
					y = z - 1;
					btFlag |= GLYF_SMPL_REPEAT;
					vtRle.push_back(btFlag);
					vtRle.push_back((unsigned char)iRepeat);
				}
				else{
					vtRle.push_back(btFlag);
				}
			}

			//flags
			size_t stRlesz = vtRle.size();
			for (size_t y = 0; y<stRlesz; y++){
				fwrite(&vtRle[y], 1, 1, pCreatFile);
			}

			unsigned char ucTmp = 0;
			//xCoordinates
			size_t stcrdts = vtflage.size();
			size_t stAryIndex = 0;
			for (size_t y = 0; y<stcrdts; y++)	{
				if (vtflage[y] & GLYF_SMPL_X_VECTOR)	{
					ucTmp = (unsigned char)vtXCoordinat[stAryIndex];
					fwrite(&ucTmp, 1, 1, pCreatFile);
				}
				else {
					if (vtflage[y] & GLYF_SMPL_X_SAME_FLG)
						continue;
					usTmp = hy_cdr_int16_to(vtXCoordinat[stAryIndex]);
					fwrite(&usTmp, 2, 1, pCreatFile);
				}
				stAryIndex++;
			}

			stAryIndex = 0;
			//yCoordinates
			stcrdts = vtflage.size();
			for (size_t y = 0; y<stcrdts; y++)	{
				if (vtflage[y] & GLYF_SMPL_Y_VECTOR){
					ucTmp = (unsigned char)vtYCoordinat[stAryIndex];
					fwrite(&ucTmp, 1, 1, pCreatFile);
				}
				else{
					if (vtflage[y] & GLYF_SMPL_Y_SAME_FLG)
						continue;

					usTmp = hy_cdr_int16_to(vtYCoordinat[stAryIndex]);
					fwrite(&usTmp, 2, 1, pCreatFile);
				}
				stAryIndex++;
			}
		}
		else {// 组件字形	
			//numberOfContours
			usTmp = 0xffff;
			fwrite(&usTmp, 2, 1, pCreatFile);
			//xMin
			usTmp = hy_cdr_int16_to(glyph.minX);
			fwrite(&usTmp, 2, 1, pCreatFile);
			//yMin
			usTmp = hy_cdr_int16_to(glyph.minY);
			fwrite(&usTmp, 2, 1, pCreatFile);
			//xMax
			usTmp = hy_cdr_int16_to(glyph.maxX);
			fwrite(&usTmp, 2, 1, pCreatFile);
			//yMax
			usTmp = hy_cdr_int16_to(glyph.maxY);
			fwrite(&usTmp, 2, 1, pCreatFile);

			size_t stCmpsz = glyph.vtComponents.size();
			for (size_t x = 0; x<stCmpsz; x++){
				//flags
				const CHYCmpst&  Cmps = glyph.vtComponents[x];
				usTmp = hy_cdr_int16_to(Cmps.flags);
				fwrite(&usTmp, 2, 1, pCreatFile);

				//glyphIndex
				usTmp = hy_cdr_int16_to(Cmps.glyphIndex);
				fwrite(&usTmp, 2, 1, pCreatFile);
				//
				if (Cmps.flags&GLYF_CMPST_ARG_1_AND_2_ARE_WORDS)	{
					usTmp = hy_cdr_int16_to(Cmps.args[0]);
					fwrite(&usTmp, 2, 1, pCreatFile);

					usTmp = hy_cdr_int16_to(Cmps.args[1]);
					fwrite(&usTmp, 2, 1, pCreatFile);
				}
				else{
					usTmp = Cmps.args[0] << 8 | (Cmps.args[1] & 0xFF);
					usTmp = hy_cdr_int16_to(usTmp);
					fwrite(&usTmp, 2, 1, pCreatFile);
				}

				if (Cmps.flags&GLYF_CMPST_WE_HAVE_A_SCALE)	{
					usTmp = hy_cdr_int16_to(Cmps.scale);
					fwrite(&usTmp, 2, 1, pCreatFile);
				}
				else if (Cmps.flags & GLYF_CMPST_WE_HAVE_AN_X_AND_Y_SCALE){
					usTmp = hy_cdr_int16_to(Cmps.vctr.xscale);
					fwrite(&usTmp, 2, 1, pCreatFile);

					usTmp = hy_cdr_int16_to(Cmps.vctr.yscale);
					fwrite(&usTmp, 2, 1, pCreatFile);
				}
				else if (Cmps.flags & GLYF_CMPST_WE_HAVE_A_TWO_BY_TWO){
					usTmp = hy_cdr_int16_to(Cmps.tensor.xscale);
					fwrite(&usTmp, 2, 1, pCreatFile);

					usTmp = hy_cdr_int16_to(Cmps.tensor.scale01);
					fwrite(&usTmp, 2, 1, pCreatFile);

					usTmp = hy_cdr_int16_to(Cmps.tensor.scale10);
					fwrite(&usTmp, 2, 1, pCreatFile);

					usTmp = hy_cdr_int16_to(Cmps.tensor.yscale);
					fwrite(&usTmp, 2, 1, pCreatFile);
				}
			}

			if (glyph.vtInstruntions.size() > 0){
				size_t stInstSize = glyph.vtInstruntions.size();
				for (size_t x = 0; x<stInstSize; x++){
					fwrite(&glyph.vtInstruntions[x], 1, 1, pCreatFile);
				}
			}
		}
		ulGlyphOffset = ftell(pCreatFile) - ulTableBegin;

		int iTail = 4 - ulGlyphOffset % 4;
		if (ulGlyphOffset % 4>0){
			char c = 0;
			for (int i = 0; i<iTail; i++){
				fwrite(&c, 1, 1, pCreatFile);
			}
			ulGlyphOffset += iTail;
		}
	}

	// loca number = glyph+1;
	FontCodec.m_HYloca.vtLoca.push_back(ulGlyphOffset);

	tbEntry.length = ftell(pCreatFile) - tbEntry.offset;
	int iTail = 4 - tbEntry.length % 4;
	if (tbEntry.length % 4>0){
		char c = 0;
		for (int i = 0; i<iTail; i++){
			fwrite(&c, 1, 1, pCreatFile);
		}
	}

	return 0;

}	// end of int CHYFontCodec::EncodeGlyph()