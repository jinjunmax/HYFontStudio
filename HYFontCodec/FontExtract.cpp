#include "StdAfx.h"
#include "FontExtract.h"
#include <algorithm>
#include <functional>
#include <atltime.h>
#include <math.h>
#include "Markup.h"

using namespace std;

namespace HYFONTCODEC
{
	bool HYCodeMapIDSort(CHYCodeMapItem& d1, CHYCodeMapItem& d2)
	{
		return (d1.iGlyphIndex < d2.iGlyphIndex);

	}	// end of BOOL HYCodeMapIDSortPredicate()

	CFontExtract::CFontExtract(void)
	{

	}	// end of CFontExtract::CFontExtract()

	CFontExtract::~CFontExtract(void)
	{

	}	// end of CFontExtract::~CFontExtract()

	int	 CFontExtract::CheckNumberRepeat(unsigned long	ulUnicod, std::vector<unsigned long>& vtUnicode)
	{
		for (size_t i=0; i<vtUnicode.size(); i++){
			if (vtUnicode[i] == ulUnicod) return i;
		}

		return 0;

	}	// end of int CheckNumberRepeat()

	int CFontExtract::ExtractFont(const char* strTTFFile, const char* strNewTTFFile, const char* strTxtFile,int flag)
	{
		int iRn = HY_NOERROR;
		if (flag == 0 || flag == 1){
			m_vtUnicode.clear();
			iRn = ::HY_TXTFileToUniArray((char*)strTxtFile, m_vtUnicode);
			if (iRn != HY_NOERROR) {
				return iRn;
			}
		}
		if(flag == 2){
			iRn = ::HY_LoadPsNameFile((char*)strTxtFile,m_vtPsName);
			if (iRn != HY_NOERROR) {
				return iRn;
			}
		}

		return ExtractFont(strTTFFile, strNewTTFFile, flag);

	}	// end of int CFontExtract::ExtractFont()

	int CFontExtract::PrepareExtractFont(const char* strTTFFile, HYFONTCODEC::CHYFontCodec& FontDecode)
	{
		int err = 0;
		int iFilestat = ::HY_FileExist(strTTFFile);
		if (iFilestat == 0)
			return HY_ERR_FILE_ZERO;
		if (iFilestat == -1)
			return HY_ERR_FILE_NOEXIST;
		if (iFilestat == -2)
			return HY_ERR_FILE_STAT;
		if (iFilestat == -3)
			return HY_ERR_FILE_OTHER;

		if (FontDecode.OpenFile(strTTFFile) == HY_NOERROR)
		{
			FontDecode.DecodeTableDirectory();
			if (FontDecode.m_HYTbDirectory.version.value != 1 && FontDecode.m_HYTbDirectory.version.fract != 0)
				return FONT_ERR_NO_TTF;	// 不是truetype
			if (FontDecode.m_HYTbDirectory.FindTableEntry(CMAP_TAG) == -1)		return FONT_ERR_CMAP_DECODE;
				FontDecode.Decodecmap();
			if (FontDecode.m_HYTbDirectory.FindTableEntry(MAXP_TAG) == -1)		return FONT_ERR_MAXP_DECODE;
				FontDecode.Decodemaxp();
			if (FontDecode.m_HYTbDirectory.FindTableEntry(HEAD_TAG) == -1)		return FONT_ERR_HEAD_DECODE;
				FontDecode.Decodehead();
			if (FontDecode.m_HYTbDirectory.FindTableEntry(LOCA_TAG) == -1)		return FONT_ERR_LOCA_DECODE;
				FontDecode.Decodeloca();

			/*
			if (FontDecode.m_HYTbDirectory.FindTableEntry(HHEA_TAG) == -1)		return FONT_ERR_HHEA_DECODE;			
				FontDecode.Decodehhea();
			if (FontDecode.m_HYTbDirectory.FindTableEntry(HMTX_TAG) == -1)		return FONT_ERR_HMTX_DECODE;
				FontDecode.Decodehmtx();			
			if (FontDecode.m_HYTbDirectory.FindTableEntry(POST_TAG) == -1)		return FONT_ERR_POST_DECODE;
				FontDecode.Decodepost();			
			if (FontDecode.m_HYTbDirectory.FindTableEntry(VHEA_TAG) != -1)
				FontDecode.Decodevhea();
			if (FontDecode.m_HYTbDirectory.FindTableEntry(VMTX_TAG) != -1)
				FontDecode.Decodevmtx();
			*/
			if (FontDecode.m_HYTbDirectory.FindTableEntry(COLR_TAG) != -1)
				FontDecode.DecodeCOLR();
			if (FontDecode.m_HYTbDirectory.FindTableEntry(SVG_TAG) != -1)
				FontDecode.DecodeSVG();
			if (FontDecode.m_HYTbDirectory.FindTableEntry(EBLC_TAG) != -1)
				FontDecode.DecodeEBLC();
			if (FontDecode.m_HYTbDirectory.FindTableEntry(CBLC_TAG) != -1)
				FontDecode.DecodeCBLC();
			if (FontDecode.m_HYTbDirectory.FindTableEntry(EBDT_TAG) != -1)
				FontDecode.DecodeEBDT();
			if (FontDecode.m_HYTbDirectory.FindTableEntry(CBDT_TAG) != -1)
				FontDecode.DecodeCBDT();

			int iTableEntryIndx = FontDecode.m_HYTbDirectory.FindTableEntry(GLYF_TAG);
			if (iTableEntryIndx == -1)											return FONT_ERR_GLYF_DECODE;

			// 抽取子字库后会出现字符缺失或字序打乱，导致GSUB映射错误，所以这个需要删除
			if (FontDecode.m_HYTbDirectory.FindTableEntry(GSUB_TAG) != -1){
				int iIndex = FontDecode.m_HYTbDirectory.FindTableEntry(GSUB_TAG);
				FontDecode.m_HYTbDirectory.vtTableEntry.erase(FontDecode.m_HYTbDirectory.vtTableEntry.begin() + iIndex);
				FontDecode.m_HYTbDirectory.numTables--;
			}

			//这个在抽取字库时，如果不删掉会无法在webfont上显示（可能）
			if (FontDecode.m_HYTbDirectory.FindTableEntry(VDMX_TAG) != -1){
				int iIndex = FontDecode.m_HYTbDirectory.FindTableEntry(VDMX_TAG);
				FontDecode.m_HYTbDirectory.vtTableEntry.erase(FontDecode.m_HYTbDirectory.vtTableEntry.begin() + iIndex);
				FontDecode.m_HYTbDirectory.numTables--;
			}

			//这个在抽取字库时，如果不删掉会无法在webfont上显示（必现）
			if (FontDecode.m_HYTbDirectory.FindTableEntry(HDMX_TAG) != -1){
				int iIndex = FontDecode.m_HYTbDirectory.FindTableEntry(HDMX_TAG);
				FontDecode.m_HYTbDirectory.vtTableEntry.erase(FontDecode.m_HYTbDirectory.vtTableEntry.begin() + iIndex);
				FontDecode.m_HYTbDirectory.numTables--;
			}

			//在子字库中没有意义
			if (FontDecode.m_HYTbDirectory.FindTableEntry(LTSH_TAG) != -1){
				int iIndex = FontDecode.m_HYTbDirectory.FindTableEntry(LTSH_TAG);
				FontDecode.m_HYTbDirectory.vtTableEntry.erase(FontDecode.m_HYTbDirectory.vtTableEntry.begin() + iIndex);
				FontDecode.m_HYTbDirectory.numTables--;
			}

			// subset多为web应用，所以vmtx和vhea也可以删除掉
			if (FontDecode.m_HYTbDirectory.FindTableEntry(VHEA_TAG) != -1) {
				int iIndex = FontDecode.m_HYTbDirectory.FindTableEntry(VHEA_TAG);
				FontDecode.m_HYTbDirectory.vtTableEntry.erase(FontDecode.m_HYTbDirectory.vtTableEntry.begin() + iIndex);
				FontDecode.m_HYTbDirectory.numTables--;
			}
			if (FontDecode.m_HYTbDirectory.FindTableEntry(VMTX_TAG) != -1) {
				int iIndex = FontDecode.m_HYTbDirectory.FindTableEntry(VMTX_TAG);
				FontDecode.m_HYTbDirectory.vtTableEntry.erase(FontDecode.m_HYTbDirectory.vtTableEntry.begin() + iIndex);
				FontDecode.m_HYTbDirectory.numTables--;
			}
		}

		return HY_NOERROR;

	}	// end of int CFontExtract::PrepareExtractFont()

	int CFontExtract::GetSubset(const char* strTTFFile, const char* strNewTTFFile, const char* strTxtFile,bool bRovlp)
	{
		int iRn = HY_NOERROR;		
		iRn = ::HY_TXTFileToUniArray((char*)strTxtFile,m_vtUnicode);
		if (iRn != HY_NOERROR) return iRn;

		if (bRovlp) {
			RemoveUniRepeat(m_vtUnicode);
		}

		CHYFontCodec		FontDecode;
		iRn = PrepareExtractFont(strTTFFile, FontDecode);
		if (iRn != HY_NOERROR) return iRn;

		CHYCodeMap	 HYCodeMap;
		unsigned long ulGlyphsLen = 0;
		// 必须在FilterGlyphs前调用FilterColors，否则描述layer的glyph会被删掉。
		if (FontDecode.m_HYTbDirectory.FindTableEntry(COLR_TAG) != -1)
			FilterColors(FontDecode, HYCodeMap.vtHYCodeMap);
		
		char* pOutGlyphs = FilterGlyphs(FontDecode, m_vtUnicode, HYCodeMap.vtHYCodeMap, ulGlyphsLen);
		if (FontDecode.m_HYTbDirectory.FindTableEntry(SVG_TAG) != -1)
			FilterSVG(FontDecode, HYCodeMap.vtHYCodeMap);

		if (FontDecode.m_HYTbDirectory.FindTableEntry(EBDT_TAG) != -1 ||
			FontDecode.m_HYTbDirectory.FindTableEntry(CBDT_TAG) != -1){
			FilterEmbeddedBitmap(FontDecode, HYCodeMap.vtHYCodeMap);
		}		
		iRn = BulidFont(FontDecode, pOutGlyphs, ulGlyphsLen, strNewTTFFile,HYCodeMap);
		if (pOutGlyphs) delete[] pOutGlyphs;
		pOutGlyphs = NULL;

		FontDecode.CloseFile();

		return iRn;

	}	// end of int CFontExtract::GetSubset()

	char* CFontExtract::FilterGlyphs(CHYFontCodec& Decode, std::vector<unsigned long>& vtInUni, std::vector<CHYCodeMapItem>& vtCodeMapItem ,unsigned long& ulGlyphsLen)
	{
		size_t stUni = vtInUni.size();
		int iEntryIndex = Decode.m_HYTbDirectory.FindTableEntry(GLYF_TAG);
		CHYTableEntry& entry = Decode.m_HYTbDirectory.vtTableEntry[iEntryIndex];
		fseek(Decode.m_pFontFile, entry.offset, SEEK_SET);

		// 第一步确定miss char
		CHYCodeMapItem  mapItm;
		mapItm.ulGlyphNo = 0xffff;
		mapItm.iGlyphIndex = 0;
		vtCodeMapItem.push_back(mapItm);

		ulGlyphsLen = Decode.m_HYloca.vtLoca[1];
		for (size_t i = 0; i < stUni; i++) {			
			int iIndex = Decode.FindGryphIndexByUnciode(vtInUni[i]);
			if (CheckGid(vtCodeMapItem, iIndex))	{

				CHYCodeMapItem  cdMapItem;
				cdMapItem.iGlyphIndex = iIndex;
				cdMapItem.ulGlyphNo = vtInUni[i];
				vtCodeMapItem.push_back(cdMapItem);

				unsigned long lca0 = Decode.m_HYloca.vtLoca[iIndex];
				unsigned long lca1 = Decode.m_HYloca.vtLoca[iIndex+1];
				if (lca1>lca0){
					fseek(Decode.m_pFontFile, entry.offset+lca0, SEEK_SET);
					std::vector<int> vtCmpGID;
					IsCompositeGlyph(Decode.m_pFontFile, vtCmpGID);

					size_t stCmpGID =  vtCmpGID.size();
					for (size_t j = 0; j < stCmpGID; j++) {
						if (CheckGid(vtCodeMapItem, vtCmpGID[j])) {
							if (Decode.m_HYloca.vtLoca[vtCmpGID[j]+1] > Decode.m_HYloca.vtLoca[vtCmpGID[j]]){
								ulGlyphsLen += Decode.m_HYloca.vtLoca[vtCmpGID[j]+1] - Decode.m_HYloca.vtLoca[vtCmpGID[j]];

								CHYCodeMapItem  cdMapItem;
								cdMapItem.iGlyphIndex = vtCmpGID[j];
								cdMapItem.ulGlyphNo = 0xffffffff;
								vtCodeMapItem.push_back(cdMapItem);
							}							
						}
					}
					ulGlyphsLen += Decode.m_HYloca.vtLoca[iIndex+1] - Decode.m_HYloca.vtLoca[iIndex];
				}
			}
		}

		//std::sort(vtCodeMapItem.begin(), vtCodeMapItem.end(), HYCodeMapIDSort);

		// 分配好子集字形内存
		int Real = (ulGlyphsLen+3)/4*4;		 
		char* pOutGlyphs = new char[Real];
		ZeroMemory(pOutGlyphs, Real);

		CHYLoca local;
		// 第一步抽取miss char数据		
		local.vtLoca.push_back(0);
		int glyphlenth = Decode.m_HYloca.vtLoca[1] - Decode.m_HYloca.vtLoca[0];
		if (glyphlenth > 0)	{
			fseek(Decode.m_pFontFile, entry.offset + Decode.m_HYloca.vtLoca[0], SEEK_SET);
			fread(pOutGlyphs, sizeof(char), glyphlenth, Decode.m_pFontFile);
		}		

		int iBuffOffset = glyphlenth;
		for (int i = 1; i < Decode.m_HYMaxp.numGlyphs; i++){
			if (CheckGid(vtCodeMapItem, i))	{
				local.vtLoca.push_back(iBuffOffset);
			}
			else{
				local.vtLoca.push_back(iBuffOffset);
				// 获取GLYH的长度
				int iGlyhLen = Decode.m_HYloca.vtLoca[i + 1] - Decode.m_HYloca.vtLoca[i];

				fseek(Decode.m_pFontFile, entry.offset + Decode.m_HYloca.vtLoca[i], SEEK_SET);
				fread(pOutGlyphs + iBuffOffset, sizeof(char), iGlyhLen, Decode.m_pFontFile);
				iBuffOffset += iGlyhLen;
			}
		}
		local.vtLoca.push_back(iBuffOffset);
		Decode.m_HYloca = local;		

		return pOutGlyphs;

	}	// end of int CFontExtract::FilterGlyphs()

	int CFontExtract::IsCompositeGlyph(FILE* f, std::vector<int>& vtGID)
	{
		unsigned short usTmp = 0;
		unsigned long ulTmp = 0;
		unsigned short  flags = 0;

		fread(&usTmp, 2, 1, f);
		short sCmp = hy_cdr_int16_to(usTmp);
		//numberOfContours
		if (sCmp == -1){
			do {
				// box
				fseek(f,8,SEEK_CUR);
				//flags				
				fread(&usTmp, 2, 1, f);
				flags = hy_cdr_int16_to(usTmp);

				//glyphIndex
				fread(&usTmp, 2, 1, f);
				usTmp = hy_cdr_int16_to(usTmp);
				if (CheckGid(vtGID, usTmp))
					vtGID.push_back(usTmp);

				if (flags & GLYF_CMPST_ARG_1_AND_2_ARE_WORDS){
					fseek(f,2,SEEK_CUR);
					fseek(f,2,SEEK_CUR);
				}
				else{
					fseek(f, 2, SEEK_CUR);
				}

				if (flags & GLYF_CMPST_WE_HAVE_A_SCALE)	{
					fseek(f, 2, SEEK_CUR);
				}
				else if (flags & GLYF_CMPST_WE_HAVE_AN_X_AND_Y_SCALE){
					fseek(f, 2, SEEK_CUR);
					fseek(f, 2, SEEK_CUR);
				}
				else if (flags & GLYF_CMPST_WE_HAVE_A_TWO_BY_TWO){
					fseek(f, 2, SEEK_CUR);				
					fseek(f, 2, SEEK_CUR);					
					fseek(f, 2, SEEK_CUR);					
					fseek(f, 2, SEEK_CUR);
				}

			} while (flags & GLYF_CMPST_MORE_COMPONENT);
			return true;
		}

		return false;
		
	}	// end of int CFontExtract::IsCompositeGlyph()

	int	CFontExtract::FilterSVG(CHYFontCodec& Decode, std::vector<CHYCodeMapItem>& vtCodeMapItem)
	{	
		for (int i = 0; i < Decode.m_HYSVG.DocumentList.numEntries;i++)	{
			SVGDocumentRecord& svgDoc = Decode.m_HYSVG.DocumentList.vtDocumentRecord[i];
			bool b = false;
			for (int j = svgDoc.startGlyphID; j <= svgDoc.endGlyphID; j++) {
				// doc在subset的范围内
				if (!CheckGid(vtCodeMapItem,j))	{
					b = true;
				}
			}

			// doc 不在subset范围内
			if (!b) {
				svgDoc.svgDocOffset = 0;
				svgDoc.svgDocLength = 0;
				svgDoc.vtDocData.clear();
			}
		}

		return true;

	}	// end of int CFontExtract::FilterSVG()
	
	int	CFontExtract::FilterColors(CHYFontCodec& Decode, std::vector<CHYCodeMapItem>& vtCodeMapItem)
	{		
		std::vector<CBaseGlyphRecord>  vtNewBaseGlyph;
		unsigned short 	numNewBaseGlyph = 0;
		size_t st = vtCodeMapItem.size();
		for (size_t i = 0; i < st; i++) {
			CBaseGlyphRecord	newBaseGlyph;
			if (Decode.m_HYCOLR.FindBaseGlyhRecord(vtCodeMapItem[i].iGlyphIndex, newBaseGlyph)) {
				vtNewBaseGlyph.push_back(newBaseGlyph);
				numNewBaseGlyph++;
			}
		}

		Decode.m_HYCOLR.vtBaseGlyphRecord = vtNewBaseGlyph;
		Decode.m_HYCOLR.baseGlyphRecordsOffset = 0;
		Decode.m_HYCOLR.layerRecordsOffset = 0;

		// 找到layer glyph 并且保留下来。
		for (int i = 0; i < Decode.m_HYCOLR.numLayerRecords; i++){
			CLayerRecord& layer = Decode.m_HYCOLR.vtLayerRecord[i];
			if(CheckGid(vtCodeMapItem,layer.GID)){
				CodeMapItem CodeItem;
				CodeItem.GID = layer.GID;
				CodeItem.vtUnicode.push_back(0xffffffff);
			}
		}

		return true;

	}	// end of int CFontExtract::FilterColors()

	int	CFontExtract::FilterEmbeddedBitmap(CHYFontCodec& Decode, std::vector<CHYCodeMapItem>& vtCodeMapItem)
	{
		size_t stLoop = vtCodeMapItem.size();
		for (int i = 0; i < stLoop; i++){
			CHYCodeMapItem&	MapItm = vtCodeMapItem[i];
			RemoveEmbeddedBitmap(Decode.m_HYCblc, MapItm.iGlyphIndex);
		}

		return true;

	}	// end of int CFontExtract::FilterEmbeddedBitmap()

	void CFontExtract::RemoveEmbeddedBitmap(CHYCBLC& BLC,int GID)
	{
		for (int i = 0; i < BLC.Header.numsizes; i++){
			BitmapSize& bmpSz = BLC.vtBitmapSizeTb[i];
			size_t szSubTable = bmpSz.vtIndexSubTableArray.size();
			for (size_t j = 0; j < szSubTable; j++) {
				IndexSubTableArray& SubTableArray =  bmpSz.vtIndexSubTableArray[j];				
				for (int x = SubTableArray.firstGlyphIndex; 
					x <= SubTableArray.lastGlyphIndex; x++)	{
					if (x != GID) {
						int iIndex = SubTableArray.lastGlyphIndex - GID;
						if (SubTableArray.IndexSubTb.Header.imageFormat == 17) {
							CBDTFormat17& Format17 = SubTableArray.IndexSubTb.vtDdataForm17[iIndex];
							Format17.dataLen = 0;
							Format17.data.clear();
						}

						if (SubTableArray.IndexSubTb.Header.imageFormat == 18) {
							CBDTFormat18& Format18 = SubTableArray.IndexSubTb.vtDdataForm18[iIndex];
							Format18.dataLen = 0;
							Format18.data.clear();
						}

						if (SubTableArray.IndexSubTb.Header.imageFormat == 19) {
							CBDTFormat19& Format19 = SubTableArray.IndexSubTb.vtDdataForm19[iIndex];
							Format19.dataLen = 0;
							Format19.data.clear();
						}
					}					
				}				
			}			
		}

	}	// end of void CFontExtract::RemoveEmbeddedBitmap()		

	int CFontExtract::ExtractFont(const char* strTTFFile, const char* strNewTTFFile,int CMAP)
	{
		int err = 0;
		HYFONTCODEC::CHYFontCodec		FontDecode;
		err = PrepareExtractFont(strTTFFile, FontDecode);
		if (err != HY_NOERROR) return err;

		int GlyphDataLen = 0;
		char* pGlpyData = NULL;

		std::vector<unsigned long> vtTableFlag;
		CHYCodeMap	 HYCodeMap;
		if (CMAP==0)
		{
			pGlpyData = Extrace(FontDecode,m_vtUnicode,GlyphDataLen);
			if (pGlpyData== NULL)  return -1;
			//LOCA、GLYF、HEAD、MAXP				
			vtTableFlag.push_back(LOCA_TAG);
			vtTableFlag.push_back(GLYF_TAG);
			vtTableFlag.push_back(HEAD_TAG);
			vtTableFlag.push_back(MAXP_TAG);
			vtTableFlag.push_back(POST_TAG);

			FontDecode.m_HYPost.Format.value = 3;
			FontDecode.m_HYPost.Format.fract = 0;
			err = BulidFont(FontDecode,pGlpyData,GlyphDataLen,strNewTTFFile,vtTableFlag,HYCodeMap);				
		}
		if (CMAP==1)
		{				
			pGlpyData = Extrace1(FontDecode,m_vtUnicode,GlyphDataLen,HYCodeMap);
			if (pGlpyData== NULL)  return -1;			

			//LOCA、GLYPH、CMAP、HEAD、MAXP、HHEA、HMTX、VHEA、VMTX
			vtTableFlag.push_back(LOCA_TAG);
			vtTableFlag.push_back(GLYF_TAG);
			vtTableFlag.push_back(CMAP_TAG);
			vtTableFlag.push_back(HEAD_TAG);
			vtTableFlag.push_back(MAXP_TAG);
			vtTableFlag.push_back(HHEA_TAG);
			vtTableFlag.push_back(HMTX_TAG);
			vtTableFlag.push_back(VHEA_TAG);
			vtTableFlag.push_back(VMTX_TAG);
			vtTableFlag.push_back(POST_TAG);

			if (FontDecode.m_HYTbDirectory.FindTableEntry(COLR_TAG) != -1)
			vtTableFlag.push_back(COLR_TAG);

			FontDecode.m_HYPost.Format.value = 3;
			FontDecode.m_HYPost.Format.fract = 0;
			err = BulidFont(FontDecode,pGlpyData,GlyphDataLen,strNewTTFFile,vtTableFlag,HYCodeMap);
		}
		if (CMAP == 2){
			pGlpyData = Extrace2(FontDecode, m_vtPsName, GlyphDataLen, HYCodeMap);
			if (pGlpyData == NULL)  return -1;

			//LOCA、GLYPH、CMAP、HEAD、MAXP、HHEA、HMTX、VHEA、VMTX
			vtTableFlag.push_back(LOCA_TAG);
			vtTableFlag.push_back(GLYF_TAG);
			vtTableFlag.push_back(CMAP_TAG);
			vtTableFlag.push_back(HEAD_TAG);
			vtTableFlag.push_back(MAXP_TAG);
			vtTableFlag.push_back(HHEA_TAG);
			vtTableFlag.push_back(HMTX_TAG);
			vtTableFlag.push_back(VHEA_TAG);
			vtTableFlag.push_back(VMTX_TAG);
			vtTableFlag.push_back(POST_TAG);

			if (FontDecode.m_HYTbDirectory.FindTableEntry(COLR_TAG) != -1)
				vtTableFlag.push_back(COLR_TAG);

			FontDecode.m_HYPost.SetDefault();
			FontDecode.m_HYPost.Format.value = 2;
			FontDecode.m_HYPost.Format.fract = 0;
			err = BulidFont(FontDecode, pGlpyData, GlyphDataLen, strNewTTFFile, vtTableFlag, HYCodeMap);
		}

		FontDecode.CloseFile();

		if (pGlpyData) delete[] pGlpyData;
		pGlpyData = NULL;	

		return err;

	}	// end of int CFontExtract::ExtractFont()

	int	CFontExtract::ChaFileToHanyiFile(char* sourcefile,char* targetfile, int iflag)
	{
		if (iflag==0)
		{
			return ChaFileToHanyiCodeTxt(sourcefile,targetfile);
		}
		if (iflag ==1 ) 
		{
			return ChaFileToHanyiCharTxt(sourcefile,targetfile);
		}

		return -1;

	}	// end of int	CFontExtract::ChaFileToHanyiFile()

	int CFontExtract::ReadChaFile(char* filename,chaRecord& Records)
	{
		return readChaFile(filename, Records);

	}	// end of int CFontExtract::ReadChaFile()

	int	CFontExtract::WriteChaFile(char* filename,chaRecord& Records)
	{
		FILE* pTargetFile = NULL;
		errno_t errt = fopen_s(&pTargetFile,filename,"wt");
		if (errt!=0)
		{			
			return HY_ERR_FILE_WRITE;
		}

		string strWrite;
		strWrite = "Version 002.000\n";
		fwrite(strWrite.c_str(),strWrite.length(),1,pTargetFile);
		strWrite = "Starttable\n";
		fwrite(strWrite.c_str(),strWrite.length(),1,pTargetFile);
		strWrite = "GlyInd;URWNum;UNINum;Width;PSName\n";
		fwrite(strWrite.c_str(),strWrite.length(),1,pTargetFile);

		for (size_t i=0; i<Records.m_vtchaList.size();i++)
		{
			chaRecordItem& item = Records.m_vtchaList[i];

			if(item.UNINum == -1)
			{
				strWrite = HY_StringFormat("%d;%d;;%d;%s\n",item.iGID,item.URWNum,item.AdvanceWith,item.PSName.c_str());
			}
			else 
			{
				strWrite = HY_StringFormat("%d;%d;x%x;%d;%s\n",item.iGID,item.URWNum,item.UNINum,item.AdvanceWith,item.PSName.c_str());
				//strWrite = HY_StringFormat("%d;%d;x%x;%d;x%x.vert\n",item.iGID,item.URWNum,item.UNINum,item.AdvanceWith,item.UNINum);
			}

			fwrite(strWrite.c_str(),strWrite.length(),1,pTargetFile);
		}

		strWrite = "Endtable\n";
		fwrite(strWrite.c_str(),strWrite.length(),1,pTargetFile);
		

		if (pTargetFile)
		{
			fflush(pTargetFile);
			fclose(pTargetFile);
		}	

		return HY_NOERROR;

	}	//end of int CFontExtract::WriteChaFile()

	int	CFontExtract::ChaFileToHanyiCodeTxt(char* sourcefile,char* targetfile)
	{		
		FILE* pTargetFile = NULL;
		errno_t errt = fopen_s(&pTargetFile,targetfile,"wt");
		if (errt!=0)
		{			
			return HY_ERR_FILE_WRITE;
		}

		std::vector<std::string> filebuf;
		::HY_ReadTextLine(sourcefile,filebuf);

		std::vector<std::string> vtOutputString;
		for (size_t i=0; i<filebuf.size(); i++)
		{
			std::string					strTmp = filebuf[i];
			std::vector<std::string>	vtSplitItem;
			char splitFlg[]	= _T(";");

			::HY_SpliteString(strTmp.data(),splitFlg,vtSplitItem);
			for (size_t j=0; j<vtSplitItem.size(); j++)
			{
				std::string strItem = vtSplitItem[j];
				strItem = ::HY_trim(strItem);

				if (!strItem.empty())
				{
					if (strItem[0] == 'x')
					{
						//strItem = strItem.erase();
						strItem = strItem.erase(0,1);
						transform (strItem.begin(),strItem.end(), strItem.begin(), toupper);
						fputs(strItem.data(),pTargetFile);
						fputs("\n",pTargetFile);
					}
				}
			}
		}

		fflush(pTargetFile);
		fclose(pTargetFile);

		return HY_NOERROR;

	}	// end of int	CFontExtract::ChaFileToHanyiCodeTxt()	

	int	CFontExtract::ChaFileToHanyiCharTxt(char* sourcefile,char* targetfile)
	{		

		return -1;

	}	// end of int	CFontExtract::ChaFileToHanyiCharTxt()

	int	CFontExtract::MakeHYCodeMap(CHYFontCodec& FontEncode,chaRecord& chaRecorder)
	{
		FontEncode.m_HYCodeMap.vtHYCodeMap.clear();

		size_t	stRecordNum = chaRecorder.m_vtchaList.size();		
		for (size_t i=0; i<stRecordNum; i++)
		{		
			chaRecordItem& recdItem = chaRecorder.m_vtchaList[i];

			CHYCodeMapItem  mapItm;
			mapItm.iGlyphIndex = recdItem.iGID;
			mapItm.ulGlyphNo = recdItem.UNINum;

			FontEncode.m_HYCodeMap.vtHYCodeMap.push_back(mapItm);
		}
		FontEncode.m_HYCodeMap.QuickSortbyUnicode();

		return HY_NOERROR;

	}	// end of int CFontExtract::MakeHYCodeMap()

	/************************************************************************/
	//构建内部码位映射机制
	//CHYFontCodec		FontEncode				字库编码器
	//chaRecord			vtUnicode				unicode链表
	/************************************************************************/
	int	CFontExtract::MakeHYCodeMap(CHYFontCodec& FontEncode, std::vector<unsigned long>& vtUnicode)
	{
		FontEncode.m_HYCodeMap.vtHYCodeMap.clear();

		// 字库默认第一字符必须是misscha
		CHYCodeMapItem  mapItm;
		mapItm.iGlyphIndex = 0;
		mapItm.ulGlyphNo = 0xffff;
		FontEncode.m_HYCodeMap.vtHYCodeMap.push_back(mapItm);

		size_t	stRecordNum = vtUnicode.size();		
		for (size_t i=0; i<stRecordNum; i++)
		{	
			mapItm.iGlyphIndex = i+1;
			mapItm.ulGlyphNo = vtUnicode[i];

			FontEncode.m_HYCodeMap.vtHYCodeMap.push_back(mapItm);
		}
		FontEncode.m_HYCodeMap.QuickSortbyUnicode();

		return HY_NOERROR;

	}	// end of int	CFontExtract::MakeHYCodeMap()


	/************************************************************************/
	//读取Cha文件到chaList
	//char*				chafile					cha文件
	//chaRecord&								vtchaList
	/************************************************************************/
	int	CFontExtract::readChaFile(char* chafile, chaRecord&	cha)
	{
		cha.m_vtchaList.clear();
		cha.vesion =0;

		std::vector<std::string> filebuf;
		int iRet = ::HY_ReadTextLine(chafile,filebuf);
		if (iRet!=HY_NOERROR) return iRet;

		size_t stTxtLineNum = filebuf.size();
		if (stTxtLineNum<3) return HY_ERR_FILE_CHA_READ;

		unsigned int stLineIndex = 0;		
		while (stLineIndex < stTxtLineNum)
		{
			string strRecord;
			std::vector<std::string> vtSplitItem;
			if (stLineIndex==0)
			{
				strRecord = filebuf[stLineIndex++];
				strRecord = ::HY_trim(strRecord);
				//strRecord = ::HY_StringToUppercase(strRecord);

				HY_SpliteString(strRecord.c_str()," ",vtSplitItem);

				if (vtSplitItem.size() <2) return HY_ERR_FILE_CHA_READ;
				if (vtSplitItem[0] != "Version") return HY_ERR_FILE_CHA_READ;
				cha.vesion = atof(vtSplitItem[1].c_str());

				strRecord = filebuf[stLineIndex++];
				strRecord = ::HY_trim(strRecord);
				//strRecord = ::HY_StringToUppercase(strRecord);
				if (strRecord != "Starttable" ) return HY_ERR_FILE_CHA_READ;

				strRecord = filebuf[stLineIndex];
				strRecord = ::HY_trim(strRecord);
				//strRecord = ::HY_StringToUppercase(strRecord);

				vtSplitItem.clear();
				HY_SpliteString(strRecord.c_str(),";",vtSplitItem);

				if (vtSplitItem[0]!="GlyInd") return HY_ERR_FILE_CHA_READ;
				if (vtSplitItem[1]!="URWNum") return HY_ERR_FILE_CHA_READ;
				if (vtSplitItem[2]!="UNINum") return HY_ERR_FILE_CHA_READ;
				if (vtSplitItem[3]!="PSName" && vtSplitItem[3]!="Width") return HY_ERR_FILE_CHA_READ;

				if(vtSplitItem[3]=="Width")
				{
					cha.vesion += 1.0f;
				}
			}
			else 
			{
				strRecord = filebuf[stLineIndex];
				strRecord = ::HY_trim(strRecord);				
				//strRecord = ::HY_StringToUppercase(strRecord);	

				if (strRecord=="Endtable") break;

				vtSplitItem.clear();
				std::size_t found = strRecord.find_first_of(";");
				std::size_t prefound = 0;
				while (found!=std::string::npos)
				{					
					vtSplitItem.push_back(strRecord.substr(prefound,found-prefound));
					prefound = found+1;
					found=strRecord.find_first_of(";",found+1);
				}
				vtSplitItem.push_back(strRecord.substr(prefound,found-prefound));


				chaRecordItem recordItem;
				//memset(&recordItem,0,sizeof(chaRecordItem));
				recordItem.iGID = atoi(vtSplitItem[0].c_str());
				recordItem.URWNum = atoi(vtSplitItem[1].c_str());
				if (vtSplitItem[2].c_str() == NULL)
				{
					recordItem.UNINum = -1;
				}
				else 
				{					
					string struni = vtSplitItem[2];

					if (struni.length() == 0)
					{
						recordItem.UNINum = -1;
					}
					else if (struni[0]=='x' ||struni[0]=='X')
					{						
						recordItem.UNINum = strtol(struni.substr(1,std::string::npos).c_str(),NULL,16);
					}
					else // 假设不是16进制的就是10进制的。
					{
						recordItem.UNINum = strtol(struni.c_str(),NULL,10);
					}
				}

				if(cha.vesion ==2.0)
				{
					recordItem.PSName = vtSplitItem[3];
				}
				else if(cha.vesion ==3.0)
				{
					recordItem.AdvanceWith = strtol(vtSplitItem[3].c_str(),NULL, 10);
					recordItem.PSName = vtSplitItem[4];
				}

				cha.m_vtchaList.push_back(recordItem);
			}

			stLineIndex++;
		}

		return iRet;

	}	// end of int	CFontExtract::readChaFile()
	
	char* CFontExtract::Extrace(HYFONTCODEC::CHYFontCodec&  HYFontCode, std::vector<unsigned long>& vtUnicode, int& GlyphDataLen)
	{
		int iEntryIndex = HYFontCode.m_HYTbDirectory.FindTableEntry(GLYF_TAG);
		CHYTableEntry& entry = HYFontCode.m_HYTbDirectory.vtTableEntry[iEntryIndex];
		char* pSrcData = new char[entry.length];
		fseek(HYFontCode.m_pFontFile,entry.offset,SEEK_SET);
		fread(pSrcData,entry.length,1,HYFontCode.m_pFontFile);
		std::vector<int> vtExtraceGid;
		GlyphDataLen = 0;

		// miss char 必须存在
		vtExtraceGid.push_back(0);	
		GlyphDataLen = HYFontCode.m_HYloca.vtLoca[1] - HYFontCode.m_HYloca.vtLoca[0];
		for (size_t i = 0; i < vtUnicode.size(); i++)
		{
			int iIndex = HYFontCode.FindGryphIndexByUnciode(vtUnicode[i]);
			if (CheckGid(vtExtraceGid, iIndex))	{
				vtExtraceGid.push_back(iIndex);
				GlyphDataLen+= HYFontCode.m_HYloca.vtLoca[iIndex+1] - HYFontCode.m_HYloca.vtLoca[iIndex];
			}
		}

		int Real = (GlyphDataLen+3)/4*4;
		char* pGlyphData = new char[Real];
		ZeroMemory(pGlyphData,Real);
		CHYLoca local;
		CopyMemory(pGlyphData,pSrcData+HYFontCode.m_HYloca.vtLoca[0], HYFontCode.m_HYloca.vtLoca[1]-HYFontCode.m_HYloca.vtLoca[0]);
		int iBuffOffset = HYFontCode.m_HYloca.vtLoca[1]-HYFontCode.m_HYloca.vtLoca[0];	
		local.vtLoca.push_back(0);

		for (int i=1; i<HYFontCode.m_HYMaxp.numGlyphs; i++)
		{
			if (CheckGid(vtExtraceGid,i))
			{
				local.vtLoca.push_back(iBuffOffset);
			}
			else 
			{
				local.vtLoca.push_back(iBuffOffset);
				// 获取GLYH的长度
				int iGlyhLen = HYFontCode.m_HYloca.vtLoca[i+1]- HYFontCode.m_HYloca.vtLoca[i];
				CopyMemory(pGlyphData+iBuffOffset,pSrcData+HYFontCode.m_HYloca.vtLoca[i], iGlyhLen);
				iBuffOffset += iGlyhLen;			
			}
		}
		local.vtLoca.push_back(iBuffOffset);
		HYFontCode.m_HYloca = local;
		if(pSrcData) delete[] pSrcData;

		return pGlyphData;

	}	// end of void CFontExtract::Extrace()

	char* CFontExtract::Extrace1(HYFONTCODEC::CHYFontCodec&  HYFontCode, std::vector<unsigned long>& vtUnicode, int& GlyphDataLen, CHYCodeMap& HYCodeMap)
	{
		int iEntryIndex = HYFontCode.m_HYTbDirectory.FindTableEntry(GLYF_TAG);
		CHYTableEntry& entry = HYFontCode.m_HYTbDirectory.vtTableEntry[iEntryIndex];
		char* pSrcData = new char[entry.length];
		fseek(HYFontCode.m_pFontFile,entry.offset,SEEK_SET);
		fread(pSrcData,entry.length,1,HYFontCode.m_pFontFile);
	
		std::vector<HMTX_LONGHORMERTRIC>		vtExtraceHMTX;
		std::vector<VMTX_LONGVRTMETRIC>			vtExtraceVMTX;
		GlyphDataLen = 0;

		std::vector<unsigned long> vtNewGID;
		std::vector<unsigned long> vtExtraceGid;

		// miss char 必须存在
		VMTX_LONGVRTMETRIC	VMTX;
		HMTX_LONGHORMERTRIC HMTX;
		vtExtraceGid.push_back(0);
		vtNewGID.push_back(0);
		HYFontCode.m_HYHmtx.GetHMTX(0,HMTX);
		vtExtraceHMTX.push_back(HMTX);		
		if (HYFontCode.m_HYVmtx.vtLongVrtmetric.size()>0){
			HYFontCode.m_HYVmtx.GetVrtMetric(0,VMTX);
			vtExtraceVMTX.push_back(VMTX);
		}
		GlyphDataLen = HYFontCode.m_HYloca.vtLoca[1] - HYFontCode.m_HYloca.vtLoca[0];

		// 一字多码将在这里作为单独的字形分别拷贝	
		int iNewGID = 1;	//miss char 0
		for (size_t i = 0; i < vtUnicode.size(); i++)
		{
			int iIndex = HYFontCode.FindGryphIndexByUnciode(vtUnicode[i]);
			if (iIndex == -1) continue;
			
			vtNewGID.push_back(iNewGID++);			
			vtExtraceGid.push_back(iIndex);

			HYFontCode.m_HYHmtx.GetHMTX(iIndex, HMTX);
			vtExtraceHMTX.push_back(HMTX);

			if (HYFontCode.m_HYVmtx.vtLongVrtmetric.size()>0){
				HYFontCode.m_HYVmtx.GetVrtMetric(iIndex, VMTX);
				vtExtraceVMTX.push_back(VMTX);
			}
			GlyphDataLen += HYFontCode.m_HYloca.vtLoca[iIndex + 1] - HYFontCode.m_HYloca.vtLoca[iIndex];			
		}	
		
		// 加入color层glyph
		if (HYFontCode.m_HYCOLR.numBaseGlyphRecords > 0)
		{
			std::vector<CLayerRecord>  vtnewLayerRecord;
			for (int i = 0; i < HYFontCode.m_HYCOLR.numLayerRecords; i++)
			{
				CLayerRecord&  layer = HYFontCode.m_HYCOLR.vtLayerRecord[i];

				CLayerRecord newlayer;
				int iRepeatIndx = CheckNumberRepeat(layer.GID, vtExtraceGid);
				if (iRepeatIndx == 0)
				{
					newlayer.GID = iNewGID;
					newlayer.paletteIndex = layer.paletteIndex;

					vtNewGID.push_back(iNewGID++);
					vtExtraceGid.push_back(layer.GID);

					HYFontCode.m_HYHmtx.GetHMTX(layer.GID, HMTX);
					vtExtraceHMTX.push_back(HMTX);
					if (HYFontCode.m_HYVmtx.vtLongVrtmetric.size()>0) {
						HYFontCode.m_HYVmtx.GetVrtMetric(layer.GID, VMTX);
						vtExtraceVMTX.push_back(VMTX);
					}
					GlyphDataLen += HYFontCode.m_HYloca.vtLoca[layer.GID + 1] - HYFontCode.m_HYloca.vtLoca[layer.GID];					
				}
				else
				{

					newlayer.GID = vtNewGID[iRepeatIndx];
					newlayer.paletteIndex = layer.paletteIndex;
				}

				vtnewLayerRecord.push_back(newlayer);
			}

			ReCountCOLOR(HYFontCode, vtExtraceGid, vtNewGID, vtnewLayerRecord);
		}

		int Real = (GlyphDataLen+3)/4*4;
		char* pGlyphData = new char[Real];
		ZeroMemory(pGlyphData,Real);
		CHYLoca local;		
		int iBuffOffset = 0;

		HYFontCode.m_HYMaxp.numGlyphs = (unsigned short)vtExtraceGid.size();
		for (int i=0; i<HYFontCode.m_HYMaxp.numGlyphs; i++)
		{			
			local.vtLoca.push_back(iBuffOffset);

			// 获取GLYH的长度
			int iGlyhLen = HYFontCode.m_HYloca.vtLoca[vtExtraceGid[i]+1]- HYFontCode.m_HYloca.vtLoca[vtExtraceGid[i]];
			CopyMemory(pGlyphData+iBuffOffset,pSrcData+HYFontCode.m_HYloca.vtLoca[vtExtraceGid[i]], iGlyhLen);
			iBuffOffset += iGlyhLen;				
		}
		local.vtLoca.push_back(iBuffOffset);
		HYFontCode.m_HYloca = local;
		if(pSrcData) delete[] pSrcData;

		ReCountHVMTX(HYFontCode, vtExtraceHMTX, vtExtraceVMTX);	
		
		HYCodeMap.vtHYCodeMap.clear();
		CHYCodeMapItem			mapItm;
		mapItm.ulGlyphNo	=	0xffff;
		mapItm.iGlyphIndex	= 0;
		HYCodeMap.vtHYCodeMap.push_back(mapItm);			

		size_t	stUniNum = m_vtUnicode.size();
		for (size_t i=0; i<stUniNum; i++)
		{				
			CHYCodeMapItem  mapItm;
			mapItm.ulGlyphNo = m_vtUnicode[i];
			mapItm.iGlyphIndex = i+1;
			HYCodeMap.vtHYCodeMap.push_back(mapItm);										
		}	
		HYCodeMap.QuickSortbyUnicode();

		return pGlyphData;

	}	// end of char* CFontExtract::Extrace1()

	void CFontExtract::ReCountHVMTX(HYFONTCODEC::CHYFontCodec&  HYFontCode, std::vector<HMTX_LONGHORMERTRIC>& vtExtraceHMTX, std::vector<VMTX_LONGVRTMETRIC>&	vtExtraceVMTX)
	{
		HYFontCode.m_HYHmtx.vtLonghormetric.clear();
		HYFontCode.m_HYVmtx.vtLongVrtmetric.clear();

		//numberOfHMetrics		
		int iHMTXNum = vtExtraceHMTX.size();
		if (iHMTXNum>0)
		{
			HYFontCode.m_HYHhea.numberOfHMetrics = 0;
			int				iBaseadvanceWidth = vtExtraceHMTX[--iHMTXNum].advanceWidth;
			while (--iHMTXNum >= 0)
			{
				if (vtExtraceHMTX[iHMTXNum].advanceWidth == iBaseadvanceWidth)
					HYFontCode.m_HYHhea.numberOfHMetrics++;
				else
					break;
			}
			HYFontCode.m_HYHhea.numberOfHMetrics = (unsigned short)vtExtraceHMTX.size() - HYFontCode.m_HYHhea.numberOfHMetrics;
			HYFontCode.m_HYHmtx.vtLonghormetric = vtExtraceHMTX;
		}

		int szVMTXNum = vtExtraceVMTX.size();
		if (szVMTXNum>0)
		{
			HYFontCode.m_HYVhea.numOfLongVerMetrics = 0;
			int				iBaseadvanceHeight = vtExtraceVMTX[--szVMTXNum].advanceHeight;
			while (--szVMTXNum >= 0)
			{
				if (vtExtraceVMTX[szVMTXNum].advanceHeight == iBaseadvanceHeight)
					HYFontCode.m_HYVhea.numOfLongVerMetrics++;
				else
					break;
			}
			HYFontCode.m_HYVhea.numOfLongVerMetrics = (unsigned short)vtExtraceVMTX.size() - HYFontCode.m_HYVhea.numOfLongVerMetrics;
			HYFontCode.m_HYVmtx.vtLongVrtmetric = vtExtraceVMTX;
		}

	}	// end of void CFontExtract::ReCountHVMTX()

	void CFontExtract::ReCountCOLOR(HYFONTCODEC::CHYFontCodec&  HYFontCode, std::vector<unsigned long>& vtExtraceGid, std::vector<unsigned long>& vtNewGID, std::vector<CLayerRecord>&  vtnewLayerRecord)
	{
		std::vector<unsigned long> vtTmpGid = vtExtraceGid;
		
		std::vector<CBaseGlyphRecord>  vtNewBaseGlyphRecord;

		for (size_t i = 0; i < vtTmpGid.size(); i++)
		{
			// make新的Base
			CBaseGlyphRecord	tmpBaseGlyphRecord;			

			if (HYFontCode.m_HYCOLR.FindBaseGlyhRecord(vtTmpGid[i], tmpBaseGlyphRecord)) {
				tmpBaseGlyphRecord.GID = vtNewGID[i];				
				vtNewBaseGlyphRecord.push_back(tmpBaseGlyphRecord);
			}
		}
		
		HYFontCode.m_HYCOLR.numBaseGlyphRecords = (unsigned short)vtNewBaseGlyphRecord.size();
		HYFontCode.m_HYCOLR.numLayerRecords = (unsigned short)vtnewLayerRecord.size();
		HYFontCode.m_HYCOLR.vtBaseGlyphRecord = vtNewBaseGlyphRecord;
		HYFontCode.m_HYCOLR.vtLayerRecord = vtnewLayerRecord;

		
	}	// end of void CFontExtract::ReCountCOLOR()
	
	char* CFontExtract::Extrace2(HYFONTCODEC::CHYFontCodec&  HYFontCode, std::vector<string>& vtPSName, int& GlyphDataLen, CHYCodeMap& HYCodeMap)
	{
		int iEntryIndex = HYFontCode.m_HYTbDirectory.FindTableEntry(GLYF_TAG);
		CHYTableEntry& entry = HYFontCode.m_HYTbDirectory.vtTableEntry[iEntryIndex];
		char* pSrcData = new char[entry.length];
		fseek(HYFontCode.m_pFontFile, entry.offset, SEEK_SET);
		fread(pSrcData, entry.length, 1, HYFontCode.m_pFontFile);

		std::vector<int>						vtExtraceGid;
		std::vector<HMTX_LONGHORMERTRIC>		vtExtraceHMTX;
		std::vector<VMTX_LONGVRTMETRIC>			vtExtraceVMTX;
		CHYCOLR									ExtraceCOLR;

		GlyphDataLen = 0;

		// miss char 必须存在
		VMTX_LONGVRTMETRIC	VMTX;
		HMTX_LONGHORMERTRIC HMTX;

		vtExtraceGid.push_back(0);
		HYFontCode.m_HYHmtx.GetHMTX(0, HMTX);
		vtExtraceHMTX.push_back(HMTX);

		if (HYFontCode.m_HYVmtx.vtLongVrtmetric.size()>0){
			HYFontCode.m_HYVmtx.GetVrtMetric(0, VMTX);
			vtExtraceVMTX.push_back(VMTX);
		}
		GlyphDataLen = HYFontCode.m_HYloca.vtLoca[1] - HYFontCode.m_HYloca.vtLoca[0];

		std::vector<unsigned long> vtExtractUnicode;
		size_t stPSName = vtPSName.size();
		for (size_t i=0; i<stPSName; i++){
			int iIndex = HYFontCode.m_HYPost.FindGIDbyName(vtPSName[i]);
			if (iIndex == -1) continue;			

			vtExtraceGid.push_back(iIndex);
			HYFontCode.m_HYHmtx.GetHMTX(iIndex, HMTX);
			vtExtraceHMTX.push_back(HMTX);

			if (HYFontCode.m_HYVmtx.vtLongVrtmetric.size()>0){
				HYFontCode.m_HYVmtx.GetVrtMetric(iIndex, VMTX);
				vtExtraceVMTX.push_back(VMTX);
			}
			GlyphDataLen += HYFontCode.m_HYloca.vtLoca[iIndex + 1] - HYFontCode.m_HYloca.vtLoca[iIndex];
			
		}	

		//numberOfHMetrics		
		int iHMTXNum = vtExtraceHMTX.size();
		if (iHMTXNum>0){
			HYFontCode.m_HYHhea.numberOfHMetrics = 0;
			int				iBaseadvanceWidth = vtExtraceHMTX[--iHMTXNum].advanceWidth;
			while (--iHMTXNum >= 0)
			{
				if (vtExtraceHMTX[iHMTXNum].advanceWidth == iBaseadvanceWidth)
					HYFontCode.m_HYHhea.numberOfHMetrics++;
				else
					break;
			}
			HYFontCode.m_HYHhea.numberOfHMetrics = (unsigned short)vtExtraceHMTX.size() - HYFontCode.m_HYHhea.numberOfHMetrics;
			HYFontCode.m_HYHmtx.vtLonghormetric = vtExtraceHMTX;
		}

		int szVMTXNum = vtExtraceVMTX.size();
		if (szVMTXNum>0){
			HYFontCode.m_HYVhea.numOfLongVerMetrics = 0;
			int				iBaseadvanceHeight = vtExtraceVMTX[--szVMTXNum].advanceHeight;
			while (--szVMTXNum >= 0)
			{
				if (vtExtraceVMTX[szVMTXNum].advanceHeight == iBaseadvanceHeight)
					HYFontCode.m_HYVhea.numOfLongVerMetrics++;
				else
					break;
			}
			HYFontCode.m_HYVhea.numOfLongVerMetrics = (unsigned short)vtExtraceVMTX.size() - HYFontCode.m_HYVhea.numOfLongVerMetrics;
			HYFontCode.m_HYVmtx.vtLongVrtmetric = vtExtraceVMTX;
		}

		int Real = (GlyphDataLen + 3) / 4 * 4;
		char* pGlyphData = new char[Real];
		ZeroMemory(pGlyphData, Real);
		CHYLoca local;		
		int iBuffOffset = 0;
		HYFontCode.m_HYHmtx.vtLonghormetric.clear();
		HYFontCode.m_HYVmtx.vtLongVrtmetric.clear();
		HYFontCode.m_HYMaxp.numGlyphs = (unsigned short)vtExtraceGid.size();

		HYCodeMap.vtHYCodeMap.clear();
		CHYCodeMapItem			mapItm;
		for (int i = 0; i<HYFontCode.m_HYMaxp.numGlyphs; i++)
		{
			local.vtLoca.push_back(iBuffOffset);

			if (i == 0){
				mapItm.ulGlyphNo = 0xffff;
				mapItm.iGlyphIndex = 0;
				HYCodeMap.vtHYCodeMap.push_back(mapItm);
			}
			else {
				std::vector<unsigned long> vtUnicodeTmp;
				HYFontCode.FindGryphUncidoByIndex(vtExtraceGid[i], vtUnicodeTmp);
				for (size_t j = 0; j<vtUnicodeTmp.size(); j++) {
					CHYCodeMapItem  mapItm;
					mapItm.ulGlyphNo = vtUnicodeTmp[j];
					mapItm.iGlyphIndex = i;
					HYCodeMap.vtHYCodeMap.push_back(mapItm);
				}
			}
			HYCodeMap.QuickSortbyUnicode();

			// 获取GLYH的长度
			int iGlyhLen = HYFontCode.m_HYloca.vtLoca[vtExtraceGid[i] + 1] - HYFontCode.m_HYloca.vtLoca[vtExtraceGid[i]];
			CopyMemory(pGlyphData + iBuffOffset, pSrcData + HYFontCode.m_HYloca.vtLoca[vtExtraceGid[i]], iGlyhLen);
			iBuffOffset += iGlyhLen;

			HYFontCode.m_HYHmtx.vtLonghormetric.push_back(vtExtraceHMTX[i]);
			if (vtExtraceVMTX.size()>0){
				HYFontCode.m_HYVmtx.vtLongVrtmetric.push_back(vtExtraceVMTX[i]);
			}

			// make新的COLR
			CBaseGlyphRecord	tmpBaseGlyphRecord;
			std::vector<CLayerRecord> tmpvtlayerReord;
			if (HYFontCode.m_HYCOLR.FindBaseGlyhRecord(vtExtraceGid[i], tmpBaseGlyphRecord)){
				HYFontCode.m_HYCOLR.FindLayerRecord(vtExtraceGid[i], tmpvtlayerReord);

				tmpBaseGlyphRecord.GID = i;
				tmpBaseGlyphRecord.firstLayerIndex = (unsigned short)ExtraceCOLR.vtLayerRecord.size();
				for (size_t j = 0; j<tmpvtlayerReord.size(); j++){
					CLayerRecord tmpLayerRecord = tmpvtlayerReord[j];
					tmpLayerRecord.GID = i;
					ExtraceCOLR.vtLayerRecord.push_back(tmpLayerRecord);
				}
				ExtraceCOLR.vtBaseGlyphRecord.push_back(tmpBaseGlyphRecord);
			}
		}

		ExtraceCOLR.numBaseGlyphRecords = (unsigned short)ExtraceCOLR.vtBaseGlyphRecord.size();
		ExtraceCOLR.numLayerRecords = (unsigned short)ExtraceCOLR.vtLayerRecord.size();
		HYFontCode.m_HYCOLR = ExtraceCOLR;

		local.vtLoca.push_back(iBuffOffset);
		HYFontCode.m_HYloca = local;
		if (pSrcData) delete[] pSrcData;		
		
		return pGlyphData;

	}	// end of char* CFontExtract::Extrace2()
	
	BOOL CFontExtract::CheckGid(std::vector<int>& vtgid, int iGid)
	{
		if (iGid==-1) return FALSE;
		size_t stGid = vtgid.size();
		for (size_t i = 0; i < stGid; i++){
			if(vtgid[i] == iGid) return FALSE;
		}

		return TRUE;

	}	// end of BOOL CFontExtract::CheckGidRepeat()

	BOOL CFontExtract::CheckGid(std::vector<CHYCodeMapItem>& vtMpItem, int iGid)
	{
		if (iGid == -1) return FALSE;

		size_t stMpItem = vtMpItem.size();
		for (size_t i = 0; i < stMpItem; i++) {
			if (vtMpItem[i].iGlyphIndex == iGid) return FALSE;
		}

		return TRUE;

	}	// end of BOOL CFontExtract::CheckGid()

	BOOL CFontExtract::CheckUnicode(std::vector<unsigned long>& m_vtunicode, unsigned long unicode)
	{		
		for (size_t i = 0; i < m_vtunicode.size(); i++)
		{
			if(m_vtunicode[i] == unicode) return TRUE;
		}

		return FALSE;

	}	// end of BOOL CFontExtract::CheckUnicode()

	void CFontExtract::WriteTableEntry(FILE* pFile, CHYTableDirectory&	HYTbDirectory)
	{
		unsigned short	searchRange = 0; 
		searchRange = (unsigned short)(log((double)HYTbDirectory.numTables)/log(2.0));
		searchRange = (unsigned short)(pow(2.0, searchRange));
		searchRange = searchRange * 16;

		HYTbDirectory.searchRange = searchRange;
		HYTbDirectory.entrySelector = (unsigned short)(log((float)(HYTbDirectory.searchRange/16))/log(2.0));
		HYTbDirectory.rangeShift = HYTbDirectory.numTables*16 - HYTbDirectory.searchRange;

		fseek(pFile,0,SEEK_SET);

		unsigned short	usTmp;
		unsigned long	ulTmp;
		// sfnt version
		usTmp = hy_cdr_int16_to(HYTbDirectory.version.value);
		fwrite(&usTmp,2,1,pFile);
		usTmp = hy_cdr_int16_to(HYTbDirectory.version.fract);
		fwrite(&usTmp,2,1,pFile);
		//numTables
		usTmp = hy_cdr_int16_to(HYTbDirectory.numTables);
		fwrite(&usTmp,2,1,pFile);
		// searchRange
		usTmp = hy_cdr_int16_to(HYTbDirectory.searchRange);
		fwrite(&usTmp,2,1,pFile);
		// entrySelector
		usTmp = hy_cdr_int16_to(HYTbDirectory.entrySelector);
		fwrite(&usTmp,2,1,pFile);
		//rangeShift
		usTmp = hy_cdr_int16_to(HYTbDirectory.rangeShift);
		fwrite(&usTmp, 2,1,pFile);
		for (int i=0; i<HYTbDirectory.numTables; i++){
			CHYTableEntry&	HYEntry = HYTbDirectory.vtTableEntry[i];

			//tag		
			ulTmp = hy_cdr_int32_to(HYEntry.tag);
			fwrite(&ulTmp,4,1,pFile);
			// checkSum
			ulTmp = hy_cdr_int32_to(HYEntry.checkSum);			
			fwrite(&ulTmp,4,1,pFile);		
			//offset
			ulTmp = hy_cdr_int32_to(HYEntry.offset);
			fwrite(&ulTmp,4,1,pFile);		
			//length
			ulTmp = hy_cdr_int32_to(HYEntry.length);
			fwrite(&ulTmp,4,1,pFile);
		}

	}	// end of void CFontExtract::WriteTableEntry()

	BOOL CFontExtract::FindItem(unsigned long ulItem, std::vector<unsigned long> vtUL)
	{
		for (size_t i=0; i<vtUL.size(); i++){
			if (vtUL[i] == ulItem){
				return TRUE;
			}
		}

		return FALSE;

	}	// end of void CFontExtract::FindTableFlag()

	int CFontExtract::ExtractCAMP(char* pFontName, char* sourcechar, char* targetXml)
	{
		int iRet = HY_NOERROR;

		std::vector<CodeMapItem>		CMAPCodeMap;
		HYFONTCODEC::CHYFontCodec		FntDecode;
		iRet = FntDecode.Decode(pFontName);
		if (iRet!=HY_NOERROR) return iRet;

		iRet = ::HY_TXTFileToUniArray((char*)sourcechar, m_vtUnicode);
		if (iRet!=HY_NOERROR) return iRet;

		size_t stTxtGlyphSize = m_vtUnicode.size();
		for (int i=0; i<stTxtGlyphSize; i++)
		{
			unsigned long ulTxtUnicode = m_vtUnicode[i];			
			int GID = FntDecode.FindGryphIndexByUnciodeEx(ulTxtUnicode);
			if (GID!=-1)
			{
				std::vector<unsigned long>		vtUnicode;
				FntDecode.FindGryphUncidoByIndex(GID,vtUnicode);

				CodeMapItem itm;
				itm.GID = GID;
				itm.vtUnicode = vtUnicode;
				CMAPCodeMap.push_back(itm);			
			}
		}

		CMapTOXml(CMAPCodeMap,targetXml);

		return HY_NOERROR;

	}	// end of int CFontExtract::ExtractCAMP()

	void CFontExtract::CMapTOXml(std::vector<CodeMapItem>& CMAPCodeMap, char* targetXml)
	{
		FILE* pTxtFile = fopen(targetXml,"wt");
		if (pTxtFile)
		{
			char pTxt[MAX_PATH] = {0}; 
			sprintf_s(pTxt,MAX_PATH,"%s", "<?xml version=\"1.0\" encoding=\"gb2312\" standalone=\"yes\"?>\n");
			fwrite(pTxt,strlen(pTxt),1,pTxtFile);
			memset(pTxt,0,MAX_PATH);		
			
			sprintf_s(pTxt,MAX_PATH,"%s", "<CMAPInfo>\n");			
			fwrite(pTxt,strlen(pTxt),1,pTxtFile);
			memset(pTxt,0,MAX_PATH);

			size_t stCodeMap = CMAPCodeMap.size();
			for (size_t i=0; i<stCodeMap; i++)
			{
				CodeMapItem& mpItem = CMAPCodeMap[i];
				sprintf_s(pTxt,MAX_PATH,"<CMAP GID=\"%d\">\n", i);
				fwrite(pTxt,strlen(pTxt),1,pTxtFile);
				memset(pTxt,0,MAX_PATH);

				for (size_t j=0; j<mpItem.vtUnicode.size();j++)
				{
					sprintf_s(pTxt,MAX_PATH, "<uni>%X</uni>\n", mpItem.vtUnicode[j]);
					fwrite(pTxt,strlen(pTxt),1,pTxtFile);
					memset(pTxt,0,MAX_PATH);
				}

				sprintf_s(pTxt,MAX_PATH,"</CMAP>\n");
			}
			
			sprintf_s(pTxt,MAX_PATH,"</CMAPInfo>\n");
			fwrite(pTxt,strlen(pTxt),1,pTxtFile);
			fclose(pTxtFile);
		}

	}	// end of void CFontExtract::CMAPTOXML()

	int CFontExtract::UpdateCMAP(std::vector<CodeMapItem>& CMAPCodeMap, char* ReplaceXml)
	{


		return HY_NOERROR;

	}	// end of int CFontExtract::UpdateCMAP()

	int CFontExtract::PrepareMergeData(char* fontfile,HYFONTCODEC::CHYFontCodec& FntDecode,std::vector<GlyfData>& vtMrg1GlyfData)
	{		
		if (FntDecode.OpenFile(fontfile) == HY_NOERROR)		
		{	
			vtMrg1GlyfData.clear();

			FntDecode.DecodeTableDirectory();		
			if(FntDecode.m_HYTbDirectory.version.value != 1 && FntDecode.m_HYTbDirectory.version.fract != 0) 	
				return FONT_ERR_NO_TTF;	// 不是truetype
			if (FntDecode.m_HYTbDirectory.FindTableEntry(CMAP_TAG) == -1)		return FONT_ERR_CMAP_DECODE;		
			FntDecode.Decodecmap();
			if (FntDecode.m_HYTbDirectory.FindTableEntry(MAXP_TAG) == -1)		return FONT_ERR_MAXP_DECODE;		
			FntDecode.Decodemaxp();
			if (FntDecode.m_HYTbDirectory.FindTableEntry(HEAD_TAG) == -1)		return FONT_ERR_HEAD_DECODE;		
			FntDecode.Decodehead();
			if (FntDecode.m_HYTbDirectory.FindTableEntry(LOCA_TAG) == -1)		return FONT_ERR_LOCA_DECODE;		
			FntDecode.Decodeloca();
			if (FntDecode.m_HYTbDirectory.FindTableEntry(HHEA_TAG) == -1)		return FONT_ERR_HHEA_DECODE;		
			FntDecode.Decodehhea();
			if (FntDecode.m_HYTbDirectory.FindTableEntry(HMTX_TAG) == -1)		return FONT_ERR_HMTX_DECODE;		
			FntDecode.Decodehmtx();
			if (FntDecode.m_HYTbDirectory.FindTableEntry(POST_TAG) == -1)		return FONT_ERR_POST_DECODE;		
			FntDecode.Decodepost();
			if (FntDecode.m_HYTbDirectory.FindTableEntry(OS2_TAG) == -1)		return FONT_ERR_OS2_DECODE;		
			FntDecode.DecodeOS2();

			
			if (FntDecode.m_HYTbDirectory.FindTableEntry(VHEA_TAG) != -1)
				FntDecode.Decodevhea();
			if (FntDecode.m_HYTbDirectory.FindTableEntry(VMTX_TAG) != -1)
				FntDecode.Decodevmtx();		

			int iTableEntryIndx = FntDecode.m_HYTbDirectory.FindTableEntry(GLYF_TAG);
			if (iTableEntryIndx ==  -1)		return FONT_ERR_GLYF_DECODE;	

			CHYTableEntry& entry = FntDecode.m_HYTbDirectory.vtTableEntry[iTableEntryIndx];
			fseek(FntDecode.m_pFontFile,entry.offset,SEEK_SET);			
			for (int i=0; i<FntDecode.m_HYMaxp.numGlyphs;i++)
			{
				GlyfData glyfData;
				glyfData.pDataLen = FntDecode.m_HYloca.vtLoca[i+1] - FntDecode.m_HYloca.vtLoca[i];
				if (glyfData.pDataLen>0)				
				{
					//glyfData.pData = new char[glyfData.pDataLen];
					for (int j=0; j<glyfData.pDataLen; j++)
					{
						byte bData = 0;
						fread(&bData,1,1,FntDecode.m_pFontFile);
						glyfData.vtData.push_back(bData);
					}					
				}

				FntDecode.FindGryphUncidoByIndex(i,glyfData.vtUnicode);
				glyfData.strName = FntDecode.m_HYPost.FindNameByGID(i);
				if (i==0)
				{
					if (glyfData.vtUnicode.size()==0)
					{
						glyfData.vtUnicode.push_back(65535);
					}
				}

				if (i<FntDecode.m_HYHhea.numberOfHMetrics)
				{
					glyfData.usAdvWidth = FntDecode.m_HYHmtx.vtLonghormetric[i].advanceWidth;
					glyfData.sLsb = FntDecode.m_HYHmtx.vtLonghormetric[i].lsb;
				}
				else
				{
					glyfData.usAdvWidth = FntDecode.m_HYHmtx.vtLonghormetric[FntDecode.m_HYHmtx.vtLonghormetric.size()-1].advanceWidth;	
					glyfData.sLsb = FntDecode.m_HYHmtx.vtLeftsidebearing[i-FntDecode.m_HYHmtx.vtLonghormetric.size()];
				}

				if (FntDecode.m_HYVhea.numOfLongVerMetrics > 0)
				{
					if (i<FntDecode.m_HYVhea.numOfLongVerMetrics)
					{
						glyfData.usAdvHeight = FntDecode.m_HYVmtx.vtLongVrtmetric[i].advanceHeight;
						glyfData.sTsb = FntDecode.m_HYVmtx.vtLongVrtmetric[i].tsb;
					}
					else 
					{
						glyfData.usAdvHeight= FntDecode.m_HYVmtx.vtLongVrtmetric[FntDecode.m_HYVmtx.vtLongVrtmetric.size()-1].advanceHeight;	
						glyfData.sTsb = FntDecode.m_HYVmtx.vtTopsidebearing[i-FntDecode.m_HYVmtx.vtLongVrtmetric.size()];
					}
				}

				vtMrg1GlyfData.push_back(glyfData);
			}	

		//	FntDecode.CloseFile();
		}
	
		return HY_NOERROR;

	}	// end of int CFontExtract::PrepareMergeData()

	void CFontExtract::Merge(HYFONTCODEC::CHYFontCodec& Fnt1Decode,std::vector<GlyfData>& vtMrg1GlyfData, HYFONTCODEC::CHYFontCodec& Fnt2Decode,std::vector<GlyfData>& vtMrg2GlyfData,char* GlyfTableData,int& iGlyfTableDataLen,int getFlag)
	{
		if (getFlag == 0) // unicode 
		{
			for (size_t i=0; i<m_vtUnicode.size(); i++)
			{	
				if (m_vtUnicode[i] == 0xffff)
					continue;

				int iMgr1Index = FindMergeGlyhDataIndex(vtMrg1GlyfData,m_vtUnicode[i]);	
				if (iMgr1Index!=-1)
				{
					//如果字库1中存在字符，字库2中也存在字符，字库2中的字符覆盖字库1中的字符
					int iMgr2Index = FindMergeGlyhDataIndex(vtMrg2GlyfData,m_vtUnicode[i]);
					if (iMgr2Index!=-1)
					{
						GlyfData& Data1 = vtMrg1GlyfData[iMgr1Index];
						GlyfData& Data2 = vtMrg2GlyfData[iMgr2Index];					
						Data1.vtData.clear();					
						Data1.pDataLen = Data2.pDataLen;				
						for (int j=0; j<Data1.pDataLen; j++)
						{
							Data1.vtData.push_back(Data2.vtData[j]);
						}
						Data1.usAdvWidth = Data2.usAdvWidth;
						Data1.sLsb = Data2.sLsb;
					}
				}
				else //如果字库1中没有字符但是字库2中有，字库2中的字库附加到字库1中的尾部
				{
					// 临时关闭
					int iMgr2Index = FindMergeGlyhDataIndex(vtMrg2GlyfData,m_vtUnicode[i]);
					if (iMgr2Index!=-1)
					{					
						GlyfData& Data2 = vtMrg2GlyfData[iMgr2Index];
						vtMrg1GlyfData.push_back(Data2);
					}
				}
			}
		}
		else if (getFlag ==1)	//psname
		{
			size_t stMrg2GlyfData = vtMrg2GlyfData.size();
			for (size_t iMgr2Index=0; iMgr2Index<stMrg2GlyfData; iMgr2Index++)
			{
				GlyfData& glyfData2 = vtMrg2GlyfData[iMgr2Index];

				int iMgr1Index = FindMergeGlyhDataIndex(vtMrg1GlyfData,glyfData2.strName);
				if (iMgr1Index != -1 )
				{	
					GlyfData& Data1 = vtMrg1GlyfData[iMgr1Index];
					GlyfData& Data2 = vtMrg2GlyfData[iMgr2Index];

					Data1.vtData.clear();					
					Data1.pDataLen = Data2.pDataLen;				
					for (int j=0; j<Data1.pDataLen; j++)
					{
						Data1.vtData.push_back(Data2.vtData[j]);
					}
					Data1.usAdvWidth = Data2.usAdvWidth;
					Data1.sLsb = Data2.sLsb;
					Data1.usAdvHeight = Data2.usAdvHeight;
					Data1.sTsb = Data2.sTsb;
					Data1.strName = Data2.strName;
				}
				else 
				{					
					GlyfData& Data2 = vtMrg2GlyfData[iMgr2Index];
					vtMrg1GlyfData.push_back(Data2);
				}
			}
		}

		Fnt1Decode.m_HYMaxp.numGlyphs = (unsigned short)vtMrg1GlyfData.size();
		// loca、VTMX、HTMX
		iGlyfTableDataLen	= 0;		
		Fnt1Decode.m_HYloca.vtLoca.clear();
		Fnt1Decode.m_HYHmtx.vtLonghormetric.clear();
		//Fnt1Decode.m_HYVmtx.vtLongVrtmetric.clear();

		m_vtPsName.clear();
		for(int i=0; i<Fnt1Decode.m_HYMaxp.numGlyphs; i++)		
		{
			Fnt1Decode.m_HYloca.vtLoca.push_back(iGlyfTableDataLen);
			GlyfData& Data1 = vtMrg1GlyfData[i];

			for (int j=0; j<Data1.pDataLen; j++)
			{
				memcpy(GlyfTableData++,&Data1.vtData[j],1);
			}
			//memcpy(GlyfTableData+iGlyfTableDataLen,Data1.pData,Data1.pDataLen);
			iGlyfTableDataLen+=Data1.pDataLen;

			HMTX_LONGHORMERTRIC tagHMTXMetric;
			tagHMTXMetric.advanceWidth = Data1.usAdvWidth;
			tagHMTXMetric.lsb = Data1.sLsb;
			Fnt1Decode.m_HYHmtx.vtLonghormetric.push_back(tagHMTXMetric);
			/*
			VMTX_LONGVRTMETRIC tagVMTXMetric;
			if (Fnt1Decode.m_HYVhea.numOfLongVerMetrics>0)
			{
				tagVMTXMetric.advanceHeight = Data1.usAdvHeight;
				tagVMTXMetric.tsb = Data1.sTsb;
				Fnt1Decode.m_HYVmtx.vtLongVrtmetric.push_back(tagVMTXMetric);
			}
			else 
			{
				Fnt1Decode.m_HYVhea.numOfLongVerMetrics = 0;
				Fnt2Decode.m_HYVhea.numOfLongVerMetrics = 0;
			}
			*/
			//m_vtPsName.push_back(Data1.strName);
		}

		Fnt1Decode.m_HYloca.vtLoca.push_back(iGlyfTableDataLen);

		//Hhea
		int  szGlyphNum = (int)(vtMrg1GlyfData.size());
		Fnt1Decode.m_HYHhea.numberOfHMetrics = 0;
		if (szGlyphNum>0)
		{
			int				iBaseadvanceWidth = vtMrg1GlyfData[--szGlyphNum].usAdvWidth;		
			while(--szGlyphNum>=0)
			{				
				if(vtMrg1GlyfData[szGlyphNum].usAdvWidth == iBaseadvanceWidth)
					Fnt1Decode.m_HYHhea.numberOfHMetrics++;
				else 
					break;
			}

			Fnt1Decode.m_HYHhea.numberOfHMetrics = (unsigned short)vtMrg1GlyfData.size()-Fnt1Decode.m_HYHhea.numberOfHMetrics;
		}		

		/*
		//Vhea
		if (Fnt1Decode.m_HYVhea.numOfLongVerMetrics>0)//&&Fnt2Decode.m_HYVhea.numOfLongVerMetrics>0
		{
			szGlyphNum = (int)(vtMrg1GlyfData.size());
			Fnt1Decode.m_HYVhea.numOfLongVerMetrics = 0;
			if (szGlyphNum>0)
			{
				int				iBaseadvanceHeight = vtMrg1GlyfData[--szGlyphNum].usAdvHeight;		
				while(--szGlyphNum>=0)
				{				
					if(vtMrg1GlyfData[szGlyphNum].usAdvHeight == iBaseadvanceHeight)
						Fnt1Decode.m_HYVhea.numOfLongVerMetrics++;
					else 
						break;
				}
				Fnt1Decode.m_HYVhea.numOfLongVerMetrics = (unsigned short)vtMrg1GlyfData.size()-Fnt1Decode.m_HYVhea.numOfLongVerMetrics;
			}
		}	
		*/

	}	// end of int CFontExtract::Merge()

	int CFontExtract::FindMergeGlyhDataIndex(std::vector<GlyfData>& vtMrgGlyfData, unsigned long Unicode)
	{
		size_t st = vtMrgGlyfData.size();
		for(size_t i=0; i<st; i++)
		{		
			GlyfData& glyhData = vtMrgGlyfData[i];
			if (CheckUnicode(glyhData.vtUnicode,Unicode))
			{
				return  i;
			}
		}

		return -1;

	}	// end of int CFontExtract::FindMergeGlyhDataIndex()

	int CFontExtract::FindMergeGlyhDataIndex(std::vector<GlyfData>& vtMrgGlyfData, std::string& glyphName)
	{
		size_t st = vtMrgGlyfData.size();
		for(size_t i=0; i<st; i++)
		{		
			GlyfData& glyhData = vtMrgGlyfData[i];
			if (glyhData.strName == glyphName)
			{
				return  i;
			}
		}

		return -1;

	}	// end of int CFontExtract::FindMergeGlyhDataIndex()

	int CFontExtract::LoadModifyVMTXFile(char* pFile)
	{
		int iFilestat = ::HY_FileExist(pFile); 
		if (iFilestat == 0)  
			return HY_ERR_FILE_ZERO;	
		if (iFilestat == -1)  
			return HY_ERR_FILE_NOEXIST;	
		if (iFilestat == -2)  
			return HY_ERR_FILE_STAT;	
		if (iFilestat == -3)  
			return HY_ERR_FILE_OTHER;	

		m_mpModifiedVMTX.clear();
		std::vector<std::string> szlineText;
		HY_ReadTextLine(pFile, szlineText);

		for (size_t i=0; i<szlineText.size(); i++)
		{
			std::vector<std::string> szVMTX;
			char splitFlg[]  = ",";		
			int inumber = ::HY_SpliteString(szlineText[i].c_str(),splitFlg,szVMTX);
			if (inumber == 2)
			{
				m_mpModifiedVMTX.insert(map<int, int>::value_type(atoi(szVMTX[0].c_str()),atoi(szVMTX[1].c_str())));
			}
		}

		return HY_NOERROR;

	}	// end of int CFontExtract::LoadModifyVMTXFile()

	// 根据导入的VMTX映射文件， 修改字库中的VMTX表项
	int CFontExtract::ModifyVMTX1(char* pFont, char* nwFnt, char* pVmtx)
	{
		int err = HY_NOERROR;
		err = LoadModifyVMTXFile(pVmtx);
		if (err != HY_NOERROR)
			return err;

		int iFilestat = ::HY_FileExist(pFont); 
		if (iFilestat == 0)  
			return HY_ERR_FILE_ZERO;	
		if (iFilestat == -1)  
			return HY_ERR_FILE_NOEXIST;	
		if (iFilestat == -2)  
			return HY_ERR_FILE_STAT;	
		if (iFilestat == -3)  
			return HY_ERR_FILE_OTHER;

		HYFONTCODEC::CHYFontCodec		HYFontCode;
		if (HYFontCode.OpenFile(pFont) == HY_NOERROR)		
		{		
			HYFontCode.DecodeTableDirectory();			
			if (HYFontCode.m_HYTbDirectory.FindTableEntry(MAXP_TAG) == -1) return FONT_ERR_VMTX_DECODE;
				HYFontCode.Decodemaxp();
			if (HYFontCode.m_HYTbDirectory.FindTableEntry(VHEA_TAG) == -1) return FONT_ERR_VHEA_DECODE;
				HYFontCode.Decodevhea();
			if (HYFontCode.m_HYTbDirectory.FindTableEntry(VMTX_TAG) == -1) return FONT_ERR_VMTX_DECODE;
				HYFontCode.Decodevmtx();		


			std::vector<VMTX_LONGVRTMETRIC> vtVMTX;
			for (int i=0; i<HYFontCode.m_HYMaxp.numGlyphs;i++)
			{
				VMTX_LONGVRTMETRIC  vmtxLongMetric;
				if (i<HYFontCode.m_HYVhea.numOfLongVerMetrics)
				{
					vmtxLongMetric.advanceHeight = HYFontCode.m_HYVmtx.vtLongVrtmetric[i].advanceHeight;
					vmtxLongMetric.tsb = HYFontCode.m_HYVmtx.vtLongVrtmetric[i].tsb;					
				}
				else 
				{
					vmtxLongMetric.advanceHeight = HYFontCode.m_HYVmtx.vtLongVrtmetric[HYFontCode.m_HYVhea.numOfLongVerMetrics-1].advanceHeight;
					vmtxLongMetric.tsb = HYFontCode.m_HYVmtx.vtTopsidebearing[i-HYFontCode.m_HYVhea.numOfLongVerMetrics];
				}	

				vtVMTX.push_back(vmtxLongMetric);
			}

			for (int i=0; i<HYFontCode.m_HYMaxp.numGlyphs;i++)
			{
				map<int,int>::iterator iter;
				iter = m_mpModifiedVMTX.find(i);
				if (iter!=m_mpModifiedVMTX.end())
				{
					vtVMTX[i].advanceHeight = (unsigned short)iter->second;
				}
			}

			HYFontCode.m_HYVmtx.SetDefault();
			HYFontCode.m_HYVmtx.vtLongVrtmetric = vtVMTX;

			int szVMTXNum = (int)HYFontCode.m_HYVmtx.vtLongVrtmetric.size();
			HYFontCode.m_HYVhea.numOfLongVerMetrics = 0;
			int				iBaseadvanceHeight = HYFontCode.m_HYVmtx.vtLongVrtmetric[--szVMTXNum].advanceHeight;
			while(--szVMTXNum>=0)
			{				
				if(HYFontCode.m_HYVmtx.vtLongVrtmetric[szVMTXNum].advanceHeight == iBaseadvanceHeight)
					HYFontCode.m_HYVhea.numOfLongVerMetrics++;
				else 
					break;
			}
			HYFontCode.m_HYVhea.numOfLongVerMetrics = (unsigned short)HYFontCode.m_HYVmtx.vtLongVrtmetric.size()-HYFontCode.m_HYVhea.numOfLongVerMetrics;		

			int GlyphDataLen = 0;			
			char* pGlpyData = NULL;			

			CHYCodeMap HYCodeMap;
			std::vector<unsigned long> vtTableFlag;				
			//VMTX,VHEA				
			vtTableFlag.push_back(VHEA_TAG);
			vtTableFlag.push_back(VMTX_TAG);
			BulidFont(HYFontCode,NULL,0,nwFnt,vtTableFlag,HYCodeMap);
		}

		return err;

	}	// end of int CFontExtract::ModifyVMTX1()

	int	 CFontExtract::ModifyVMTX2(char* pFont, char* nwFnt, int space)
	{
		int err = HY_NOERROR;		

		int iFilestat = ::HY_FileExist(pFont); 
		if (iFilestat == 0)  
			return HY_ERR_FILE_ZERO;	
		if (iFilestat == -1)  
			return HY_ERR_FILE_NOEXIST;	
		if (iFilestat == -2)  
			return HY_ERR_FILE_STAT;	
		if (iFilestat == -3)  
			return HY_ERR_FILE_OTHER;

		HYFONTCODEC::CHYFontCodec		HYFontCode;				
			
		if (HYFontCode.OpenFile(pFont) == HY_NOERROR)		
		{		
			HYFontCode.DecodeTableDirectory();			
			if (HYFontCode.m_HYTbDirectory.FindTableEntry(MAXP_TAG) == -1) return FONT_ERR_VMTX_DECODE;
			HYFontCode.Decodemaxp();

			if (HYFontCode.m_HYTbDirectory.FindTableEntry(HEAD_TAG) == -1) return FONT_ERR_HEAD_DECODE;
			HYFontCode.Decodehead();

			if (HYFontCode.m_HYTbDirectory.FindTableEntry(VHEA_TAG) != -1)
				HYFontCode.Decodevhea();
			if (HYFontCode.m_HYTbDirectory.FindTableEntry(VMTX_TAG) != -1)
				HYFontCode.Decodevmtx();		

			if (HYFontCode.m_HYTbDirectory.FindTableEntry(LOCA_TAG) == -1) return FONT_ERR_LOCA_DECODE;
			HYFontCode.Decodeloca();
			if (HYFontCode.m_HYTbDirectory.FindTableEntry(GLYF_TAG) == -1) return FONT_ERR_GLYF_DECODE;
			HYFontCode.m_vtHYGlyphs.resize(HYFontCode.m_HYMaxp.numGlyphs);

			int iEntryIndex = HYFontCode.m_HYTbDirectory.FindTableEntry(GLYF_TAG);				
			if (iEntryIndex != -1)
			{
				CHYTableEntry& tbEntry = HYFontCode.m_HYTbDirectory.vtTableEntry[iEntryIndex];
				fseek(HYFontCode.m_pFontFile,tbEntry.offset,SEEK_SET);
				HYFontCode.Decodeglyf();
			}

		}

 		std::vector<VMTX_LONGVRTMETRIC> vtVMTX;		
		if (HYFontCode.FindTableEntry(VMTX_TAG) != NULL && HYFontCode.FindTableEntry(VHEA_TAG) != NULL)
		{			
			for (int i=0; i<HYFontCode.m_HYMaxp.numGlyphs;i++)
			{
				VMTX_LONGVRTMETRIC  vmtxLongMetric;
				if (i<HYFontCode.m_HYVhea.numOfLongVerMetrics)
				{
					vmtxLongMetric.advanceHeight = HYFontCode.m_HYVmtx.vtLongVrtmetric[i].advanceHeight;
					vmtxLongMetric.tsb = HYFontCode.m_HYVmtx.vtLongVrtmetric[i].tsb;
				}
				else 
				{
					vmtxLongMetric.advanceHeight = HYFontCode.m_HYVmtx.vtLongVrtmetric[HYFontCode.m_HYVhea.numOfLongVerMetrics-1].advanceHeight;
					vmtxLongMetric.tsb = HYFontCode.m_HYVmtx.vtTopsidebearing[i-HYFontCode.m_HYVhea.numOfLongVerMetrics];
				}	

				vtVMTX.push_back(vmtxLongMetric);
			}		

			for (int i=0; i<HYFontCode.m_HYMaxp.numGlyphs;i++)
			{
				CHYGlyph& glyh =  HYFontCode.m_vtHYGlyphs[i];

				if (glyh.vtContour.size()==0&&glyh.vtComponents.size()==0)
					continue;			

				VMTX_LONGVRTMETRIC&  vmtxLongMetric = vtVMTX[i];
				vmtxLongMetric.tsb = space;
				vmtxLongMetric.advanceHeight = (glyh.maxY-glyh.minY)+(space<<1);
			}
		}
		else 
		{
			int EntryNum = (int)HYFontCode.m_HYTbDirectory.vtTableEntry.size();
			bool bIsInsertLast = TRUE;

			CHYTableEntry VHEAEntry;
			VHEAEntry.tag = VHEA_TAG;
			for (int x=0; x<EntryNum; x++)
			{					
				if (HYFontCode.m_HYTbDirectory.vtTableEntry[x].tag > VHEA_TAG)
				{					
					HYFontCode.m_HYTbDirectory.vtTableEntry.insert(HYFontCode.m_HYTbDirectory.vtTableEntry.begin()+x,VHEAEntry);
					bIsInsertLast = FALSE;
				}				
			}
			if (bIsInsertLast)
			{
				HYFontCode.m_HYTbDirectory.vtTableEntry.push_back(VHEAEntry);				
			}	

			bIsInsertLast = TRUE;
			VHEAEntry.tag = VMTX_TAG;
			for (int x=0; x<EntryNum; x++)
			{
				if (HYFontCode.m_HYTbDirectory.vtTableEntry[x].tag > VMTX_TAG)
				{	
					HYFontCode.m_HYTbDirectory.vtTableEntry.insert(HYFontCode.m_HYTbDirectory.vtTableEntry.begin()+x,VHEAEntry);
				}
			}
			if (bIsInsertLast)
			{
				HYFontCode.m_HYTbDirectory.vtTableEntry.push_back(VHEAEntry);
			}	

			// 重新计算有多少表
			HYFontCode.m_HYTbDirectory.numTables = (unsigned short)HYFontCode.m_HYTbDirectory.vtTableEntry.size();

			for (int i=0; i<HYFontCode.m_HYMaxp.numGlyphs;i++)
			{				
				CHYGlyph& glyh =  HYFontCode.m_vtHYGlyphs[i];
				VMTX_LONGVRTMETRIC  vmtxLongMetric;
				if (glyh.vtContour.size()==0&&glyh.vtComponents.size()==0)
				{
					vmtxLongMetric.advanceHeight = HYFontCode.m_HYhead.unitsPerEm;
					vmtxLongMetric.tsb = 0;
				}
				else 
				{
					vmtxLongMetric.tsb = space;
					vmtxLongMetric.advanceHeight = (glyh.maxY-glyh.minY)+(space<<1);
				}

				vtVMTX.push_back(vmtxLongMetric);				
			}
		}

		HYFontCode.m_HYVmtx.SetDefault();
		HYFontCode.m_HYVmtx.vtLongVrtmetric = vtVMTX;

		int szVMTXNum = (int)HYFontCode.m_HYVmtx.vtLongVrtmetric.size();
		HYFontCode.m_HYVhea.numOfLongVerMetrics = 0;
		int				iBaseadvanceHeight = HYFontCode.m_HYVmtx.vtLongVrtmetric[--szVMTXNum].advanceHeight;
		while(--szVMTXNum>=0)
		{				
			if(HYFontCode.m_HYVmtx.vtLongVrtmetric[szVMTXNum].advanceHeight == iBaseadvanceHeight)
				HYFontCode.m_HYVhea.numOfLongVerMetrics++;
			else 
				break;
		}

		HYFontCode.m_HYVhea.numOfLongVerMetrics = (unsigned short)HYFontCode.m_HYVmtx.vtLongVrtmetric.size()-HYFontCode.m_HYVhea.numOfLongVerMetrics;	

		int GlyphDataLen = 0;			
		char* pGlpyData = NULL;			

		CHYCodeMap HYCodeMap;
		std::vector<unsigned long> vtTableFlag;				
		//VMTX,VHEA				
		vtTableFlag.push_back(VHEA_TAG);
		vtTableFlag.push_back(VMTX_TAG);
		BulidFont(HYFontCode,NULL,0,nwFnt,vtTableFlag,HYCodeMap);	

		return err;

	}	// end of int	 CFontExtract::ModifyVMTX2()

	int	 CFontExtract::ReadPsNameFile(char* pPsName, std::vector<std::string>& vtPsName)
	{	
		int iFilestat = ::HY_FileExist(pPsName); 
		if (iFilestat == 0)  
			return HY_ERR_FILE_ZERO;	
		if (iFilestat == -1)  
			return HY_ERR_FILE_NOEXIST;	
		if (iFilestat == -2)  
			return HY_ERR_FILE_STAT;	
		if (iFilestat == -3) 
			return HY_ERR_FILE_OTHER;

		vtPsName.clear();
		char* pFbuf = new char[iFilestat];
		memset(pFbuf,0,iFilestat);

		FILE* pFile = fopen(pPsName,"rt");
		fread(pFbuf,iFilestat,1,pFile);		
		::HY_SpliteString(pFbuf,"\n",vtPsName);

		if (pFbuf)
			delete[] pFbuf;

		return HY_NOERROR;

	}	//int	 CfontExtract::ReadPsNameFile()

	int	 CFontExtract::ModifyVMTX3(char* pFont, char* pPsNameFile,char* nwFnt, int space)
	{
		std::vector<std::string> vtPsName;
		int Result = ReadPsNameFile(pPsNameFile, vtPsName);
		if (Result != HY_NOERROR) return Result;

		HYFONTCODEC::CHYFontCodec		HYFontCode;
		Result = HYFontCode.OpenFile(pFont);
		if (Result != HY_NOERROR) return Result;		
				
		HYFontCode.DecodeTableDirectory();			
		if (HYFontCode.m_HYTbDirectory.FindTableEntry(VMTX_TAG) == -1)
		{
			HYFontCode.CloseFile();
			return FONT_ERR_VMTX_DECODE;
		}
		HYFontCode.Decodemaxp();

		if (HYFontCode.m_HYTbDirectory.FindTableEntry(HEAD_TAG) == -1) 
		{
			HYFontCode.CloseFile();
			return FONT_ERR_HEAD_DECODE;
		}
		HYFontCode.Decodehead();

		if (HYFontCode.m_HYTbDirectory.FindTableEntry(VHEA_TAG) != -1)		
			HYFontCode.Decodevhea();
		
		if (HYFontCode.m_HYTbDirectory.FindTableEntry(VMTX_TAG) != -1)
			HYFontCode.Decodevmtx();

		if (HYFontCode.m_HYTbDirectory.FindTableEntry(POST_TAG) != -1)
			HYFontCode.Decodepost();

		if (HYFontCode.m_HYTbDirectory.FindTableEntry(LOCA_TAG) == -1) 
		{
			HYFontCode.CloseFile();
			return FONT_ERR_LOCA_DECODE;
		}
		HYFontCode.Decodeloca();

		if (HYFontCode.m_HYTbDirectory.FindTableEntry(GLYF_TAG) == -1) 
		{
			HYFontCode.CloseFile();
			return FONT_ERR_GLYF_DECODE;
		}
		HYFontCode.m_vtHYGlyphs.resize(HYFontCode.m_HYMaxp.numGlyphs);

		int iEntryIndex = HYFontCode.m_HYTbDirectory.FindTableEntry(GLYF_TAG);
		CHYTableEntry& tbEntry = HYFontCode.m_HYTbDirectory.vtTableEntry[iEntryIndex];
		fseek(HYFontCode.m_pFontFile,tbEntry.offset,SEEK_SET);
		HYFontCode.Decodeglyf();

		if (HYFontCode.FindTableEntry(VHEA_TAG) == NULL) 
		{
			HYFontCode.CloseFile();
			return FONT_ERR_VHEA_DECODE;
		}

		if (HYFontCode.FindTableEntry(VMTX_TAG) == NULL) 
		{
			HYFontCode.CloseFile();
			return FONT_ERR_VMTX_DECODE;
		}	
			
		std::vector<VMTX_LONGVRTMETRIC> vtVMTX;
		for (int i=0; i<HYFontCode.m_HYMaxp.numGlyphs;i++)
		{
			VMTX_LONGVRTMETRIC  vmtxLongMetric;
			if (i<HYFontCode.m_HYVhea.numOfLongVerMetrics)
			{
				vmtxLongMetric.advanceHeight = HYFontCode.m_HYVmtx.vtLongVrtmetric[i].advanceHeight;
				vmtxLongMetric.tsb = HYFontCode.m_HYVmtx.vtLongVrtmetric[i].tsb;
			}
			else 
			{
				vmtxLongMetric.advanceHeight = HYFontCode.m_HYVmtx.vtLongVrtmetric[HYFontCode.m_HYVhea.numOfLongVerMetrics-1].advanceHeight;
				vmtxLongMetric.tsb = HYFontCode.m_HYVmtx.vtTopsidebearing[i-HYFontCode.m_HYVhea.numOfLongVerMetrics];
			}	

			vtVMTX.push_back(vmtxLongMetric);
		}

		for (size_t i=0; i<vtPsName.size();i++)
		{
			int iGID = HYFontCode.m_HYPost.FindGIDbyName(vtPsName[i]);
			if (iGID == -1) continue;			
			
			CHYGlyph& glyh =  HYFontCode.m_vtHYGlyphs[iGID];
			VMTX_LONGVRTMETRIC&  vmtxLongMetric = vtVMTX[iGID];
			vmtxLongMetric.tsb = space;
			vmtxLongMetric.advanceHeight = (glyh.maxY-glyh.minY)+(space<<1);
		}

		HYFontCode.m_HYVmtx.SetDefault();
		HYFontCode.m_HYVmtx.vtLongVrtmetric = vtVMTX;

		int szVMTXNum = (int)HYFontCode.m_HYVmtx.vtLongVrtmetric.size();
		HYFontCode.m_HYVhea.numOfLongVerMetrics = 0;
		int				iBaseadvanceHeight = HYFontCode.m_HYVmtx.vtLongVrtmetric[--szVMTXNum].advanceHeight;
		while(--szVMTXNum>=0)
		{				
			if(HYFontCode.m_HYVmtx.vtLongVrtmetric[szVMTXNum].advanceHeight == iBaseadvanceHeight)
				HYFontCode.m_HYVhea.numOfLongVerMetrics++;
			else 
				break;
		}

		HYFontCode.m_HYVhea.numOfLongVerMetrics = (unsigned short)HYFontCode.m_HYVmtx.vtLongVrtmetric.size()-HYFontCode.m_HYVhea.numOfLongVerMetrics;		

		int GlyphDataLen = 0;			
		char* pGlpyData = NULL;			

		CHYCodeMap HYCodeMap;
		std::vector<unsigned long> vtTableFlag;				
		//VMTX,VHEA				
		vtTableFlag.push_back(VHEA_TAG);
		vtTableFlag.push_back(VMTX_TAG);
		return BulidFont(HYFontCode,NULL,0,nwFnt,vtTableFlag,HYCodeMap);		

	}	// end of int	 CFontExtract::ModifyVMTX3()

	int CFontExtract::NextGroup(int ix, int eov, GroupRecord& group,CHYCodeMap& CodeMap) 
	{
		int				entryCount = 0;
		bool			haveContiguous = false;		// Neither contiguous nor discontiguous yet
		bool			contiguous = true;			// Default to true for single glyph range.

		std::vector<CHYCodeMapItem>& vtMapItm = CodeMap.vtHYCodeMap;
		if (ix != eov)
		{
			unsigned long 		startCharCode = vtMapItm[ix].ulGlyphNo;
			int					startGID = vtMapItm[ix].iGlyphIndex;
			int					hold = ix;
			++ix;
			++entryCount;

			group.startCode = startCharCode;
			group.startGID = startGID;
			group.endCode = startCharCode;

			while (ix!=eov && vtMapItm[ix].ulGlyphNo == startCharCode+1)
			{
				int			gid  = vtMapItm[ix].iGlyphIndex;
				bool		successive = gid == startGID+1;

				startCharCode = vtMapItm[ix].ulGlyphNo;
				++group.endCode;

				if (haveContiguous)
				{
					if (successive != contiguous)
					{
						ix = hold;
						entryCount -= 1;
						group.endCode -= 1;
						break;
					}
				}
				else
					contiguous = successive;

				haveContiguous = true;
				startGID = gid;

				hold = ++ix;
				++entryCount;
			}
		}

		group.contiguous = contiguous;

		return ix;

	}	// end of CFontExtract::NextGroup () 

	int	CFontExtract::EncodeCmapFmt4(CMAP_TABLE_ENTRY&	entry, CHYCodeMap& CodeMap)
	{	
		std::vector<char> vtCmap;

		if (vtCmap.size()==0)
		{
			std::list<GroupRecord>		groupList;		
			int		eov = CodeMap.vtHYCodeMap.size();
			int		ix = 0;

			while (ix != eov && CodeMap.vtHYCodeMap[ix].ulGlyphNo <= 0xffff)
			{
				GroupRecord				group;
				int						nextIx = NextGroup(ix, eov, group,CodeMap);		

				if (group.startCode < 0xffff)
				{	// This format does not code above 0xfffe
					if (group.endCode > 0xfffe)
						group.endCode = 0xfffe;

					groupList.push_back (group);
				}
				ix = nextIx;
			}

			std::list <GroupRecord>::iterator		gix = groupList.begin();
			std::list <GroupRecord>::iterator		glast = groupList.begin();
			if (gix != groupList.end())
				++gix;

			std::list <GroupRecord>::iterator		gend = groupList.end();
			ULONG									lastCode = 0;
			int										entryCount = 0;
			int										groupCount = 0;

			gix = groupList.begin();
			while (gix != gend)
			{
				if (!gix->contiguous)
					entryCount += gix->endCode - gix->startCode+1;

				lastCode = gix->endCode;
				++gix;
				groupCount += 1;
			}

			if (lastCode != 0xffff)		// We are required to terminate the table with a group containing 0xffff
				groupCount += 1;

			size_t					length = 2 * (8 + groupCount * 4 + entryCount);
			if (length > 65535)		return -1;

			std::vector <unsigned short>	startTable (groupCount);
			std::vector <unsigned short>	endTable (groupCount);
			std::vector <short>				idDelta (groupCount);
			std::vector <unsigned short>	idRangeOffset (groupCount);
			std::vector <unsigned short>	glyphIdArray;
			size_t							groupIx = 0;
			entryCount = 0;

			gix = groupList.begin();

			while (gix != gend)
			{
				size_t				entries = gix->endCode - gix->startCode + 1;

				startTable [groupIx] = (unsigned short)gix->startCode;
				endTable [groupIx] = (unsigned short)gix->endCode;

				if (gix->contiguous)
				{	// If the glyphIDs in the range are consecutive then we don't need entries in the GlyphIdArray.
					idDelta [groupIx] = (short)gix->startGID - (short)gix->startCode;
					idRangeOffset [groupIx] = 0xffff;
				}
				else
				{	//Theoretically the table can be compressed by finding multiple ranges with the same differences between each
					// code point in the range and the corresponding glyphID, and then adjusting the bias via idDelta. We're not going to
					// bother. It's computationally expensive and probably doesn't yield much savings for most circumstances.
					idDelta [groupIx] = 0;
					idRangeOffset [groupIx] = (unsigned short)entryCount;

					unsigned long		code = gix->startCode;
					for (size_t i=0; i<entries; ++i)
					{
						int iGID = CodeMap.FindIndexByUnicode(code++);
						if (iGID!=-1)
							glyphIdArray.push_back((unsigned short)iGID);
						else
							glyphIdArray.push_back(0);
					}

					if (entryCount+entries > 0xffff) 
						return -1;

					entryCount += static_cast <unsigned short> (entries);
				}

				++groupIx;
				++gix;
			}

			if (lastCode != 0xffff)
			{
				startTable [groupIx]	= 0xffff;
				endTable [groupIx]		= 0xffff;
				idDelta [groupIx]		= 1;
				idRangeOffset [groupIx] = 0xffff;
			}

			//format			
			entry.Format4.format = CMAP_ENCODE_FT_4;
			vtCmap.push_back(entry.Format4.format>>8);
			vtCmap.push_back(entry.Format4.format&0x00ff);

			//length
			entry.Format4.length = (unsigned short)length;
			vtCmap.push_back(entry.Format4.length>>8);
			vtCmap.push_back(entry.Format4.length&0x00ff);

			//language
			entry.Format4.language = 0;
			vtCmap.push_back(0);
			vtCmap.push_back(0);

			//segCountX2
			entry.Format4.segCountX2	= (unsigned short)groupCount*2;
			vtCmap.push_back(entry.Format4.segCountX2>>8);
			vtCmap.push_back(entry.Format4.segCountX2&0x00ff);

			//searchRange
			entry.Format4.searchRange	= (unsigned short)(2* pow(2,floor((log((double)groupCount)/log(2.0))))); 
			vtCmap.push_back(entry.Format4.searchRange>>8);
			vtCmap.push_back(entry.Format4.searchRange&0x00ff);

			//entrySelector
			entry.Format4.entrySelector		=	(unsigned short) (log(entry.Format4.searchRange / 2.0)/log(2.0));
			vtCmap.push_back(entry.Format4.entrySelector>>8);
			vtCmap.push_back(entry.Format4.entrySelector&0x00ff);

			//rangeShift
			entry.Format4.rangeShift		=	entry.Format4.segCountX2-entry.Format4.searchRange;
			vtCmap.push_back(entry.Format4.rangeShift>>8);
			vtCmap.push_back(entry.Format4.rangeShift&0x00ff);

			//endCount
			unsigned short usTmp;
			for (int i=0; i<groupCount; i++)
			{
				usTmp = endTable[i];

				vtCmap.push_back(usTmp>>8);
				vtCmap.push_back(usTmp&0x00ff);				
			}

			//reservedPad
			entry.Format4.reservedPad		=	0;
			vtCmap.push_back(0);
			vtCmap.push_back(0);

			//startCount
			for (int i=0; i<groupCount; i++)
			{
				usTmp = startTable[i];

				vtCmap.push_back(usTmp>>8);
				vtCmap.push_back(usTmp&0x00ff);				
			}	

			//idDelta
			for (int i=0; i<groupCount; i++)
			{
				usTmp = idDelta[i];

				vtCmap.push_back(usTmp>>8);
				vtCmap.push_back(usTmp&0x00ff);				
			}					

			//idRangeOffset
			for (int i=0; i<groupCount; i++)
			{
				if (idRangeOffset[i] == 0xffff)
				{
					usTmp = 0;
				}
				else 
				{
					usTmp = static_cast <unsigned short> (2*(idRangeOffset[i]+groupCount-i));						
				}					

				vtCmap.push_back(usTmp>>8);
				vtCmap.push_back(usTmp&0x00ff);				
			}			

			//glyphIdArray
			for (size_t i = 0; i < glyphIdArray.size(); i++)
			{		
				usTmp = glyphIdArray[i];
				vtCmap.push_back(usTmp>>8);
				vtCmap.push_back(usTmp&0x00ff);					
			}
		}

		return HY_NOERROR;

	}	// end of int	CFontExtract::EncodeCmapFmt4()

	int	CFontExtract::EncodeCmapFmt12(CMAP_TABLE_ENTRY& entry,CHYCodeMap& CodeMap)
	{		
		std::vector<char> vtCmap;
		if (vtCmap.size() == 0)
		{			
			int iGlyphIndex	= -1, iExpectGlyphIndex = -1;
			ULONG ulUnicode	= 0, ulExpectUnicode = 0;
			size_t i = 0, j = 0;

			size_t iCharNumber = CodeMap.vtHYCodeMap.size(); 
			while (i<iCharNumber)
			{
				CMAP_ENCODE_FORMAT_12_GROUP	 tagF12Group;
				CHYCodeMapItem& HYMapItem	= CodeMap.vtHYCodeMap[i];

				iGlyphIndex					= HYMapItem.iGlyphIndex;
				ulUnicode					= HYMapItem.ulGlyphNo;
				if (iGlyphIndex == 0) 	
				{
					i++;
					continue;		
				}

				tagF12Group.startCharCode = ulUnicode;
				tagF12Group.startGlyphID = iGlyphIndex;		

				for ( j=i+1; j<iCharNumber; j++ )
				{
					CHYCodeMapItem& HYMapItem	= CodeMap.vtHYCodeMap[j];
					iExpectGlyphIndex	= HYMapItem.iGlyphIndex;
					ulExpectUnicode		= HYMapItem.ulGlyphNo;

					if (iExpectGlyphIndex == 0) continue;
					iGlyphIndex++;
					ulUnicode++;

					// Index和unicode都必须是连续的,否则段结束
					if ( iExpectGlyphIndex != iGlyphIndex || ulExpectUnicode != ulUnicode)
					{
						tagF12Group.endCharCode = ulUnicode - 1;
						entry.Format12.vtGroup.push_back(tagF12Group);							
						i = j;
						break;
					}
				}

				// 数组末尾
				if (j==iCharNumber)			
				{
					if (tagF12Group.endCharCode==0)
					{
						if (CodeMap.vtHYCodeMap[j-1].iGlyphIndex == 0)
							tagF12Group.endCharCode = CodeMap.vtHYCodeMap[j-2].ulGlyphNo;
						else 
							tagF12Group.endCharCode = CodeMap.vtHYCodeMap[j-1].ulGlyphNo;

						entry.Format12.vtGroup.push_back(tagF12Group);
					}
					break;
				}
			}

			// 向CMAP_ENCODE_F12赋值	
			entry.Format12.format =CMAP_ENCODE_FT_12;
			vtCmap.push_back((entry.Format12.format&0xff00)>>8);
			vtCmap.push_back(entry.Format12.format&0x00ff);

			entry.Format12.reserved				= 0;
			vtCmap.push_back(0);
			vtCmap.push_back(0);

			// 16 == format + reserved + length + language + nGroups
			// 12 == startCharCode + endCharCode + startGlyphID
			entry.Format12.length				= 16+entry.Format12.vtGroup.size()*12;
			vtCmap.push_back((entry.Format12.length&0xff000000)>>24);
			vtCmap.push_back((entry.Format12.length&0x00ff0000)>>16);
			vtCmap.push_back((entry.Format12.length&0x0000ff00)>>8);
			vtCmap.push_back(entry.Format12.length&0x000000ff);

			entry.Format12.language				= 0;
			vtCmap.push_back(0);
			vtCmap.push_back(0);
			vtCmap.push_back(0);
			vtCmap.push_back(0);

			entry.Format12.nGroups				= (unsigned long)entry.Format12.vtGroup.size();
			vtCmap.push_back((entry.Format12.nGroups&0xff000000)>>24);
			vtCmap.push_back((entry.Format12.nGroups&0x00ff0000)>>16);
			vtCmap.push_back((entry.Format12.nGroups&0x0000ff00)>>8);
			vtCmap.push_back(entry.Format12.nGroups&0x000000ff);

			unsigned long ulTmp = 0;
			for (size_t i=0; i<entry.Format12.vtGroup.size(); i++)
			{
				std::vector<CMAP_ENCODE_FORMAT_12_GROUP>& vtGroups = entry.Format12.vtGroup;
				ulTmp = vtGroups[i].startCharCode;
				vtCmap.push_back((ulTmp&0xff000000)>>24);
				vtCmap.push_back((ulTmp&0x00ff0000)>>16);
				vtCmap.push_back((ulTmp&0x0000ff00)>>8);
				vtCmap.push_back(ulTmp&0x000000ff);				

				ulTmp = vtGroups[i].endCharCode;
				vtCmap.push_back((ulTmp&0xff000000)>>24);
				vtCmap.push_back((ulTmp&0x00ff0000)>>16);
				vtCmap.push_back((ulTmp&0x0000ff00)>>8);
				vtCmap.push_back(ulTmp&0x000000ff);

				ulTmp = vtGroups[i].startGlyphID;
				vtCmap.push_back((ulTmp&0xff000000)>>24);
				vtCmap.push_back((ulTmp&0x00ff0000)>>16);
				vtCmap.push_back((ulTmp&0x0000ff00)>>8);
				vtCmap.push_back(ulTmp&0x000000ff);			
			}
		}

		return HY_NOERROR;

	}	// end of int CFontExtract::EncodeCmapFmt12()

	int CFontExtract::EncodeCmapFmt14(CMAP_TABLE_ENTRY& entry, CHYCodeMap& CodeMap)
	{

		return HY_NOERROR;

	}	// end of int CFontExtract::EncodeCmapFmt14()

	BOOL CFontExtract::HitTable(std::vector<ULONG>& vtTableFlag, ULONG ulTableFlag)
	{
		for (size_t i=0; i<vtTableFlag.size(); i++)
		{
			if (vtTableFlag[i] == ulTableFlag) 
				return TRUE;
		}

		return FALSE;

	}	// end of void CFontExtract::HitTable()

	void CFontExtract::CountVertTable(HYFONTCODEC::CHYFontCodec& FontCodec, std::vector<GlyfData>& MrgGlyfData)
	{		
		//VTMX
		FontCodec.m_HYVmtx.vtLongVrtmetric.clear();

		short advHeight=0,xMin=0, yMin=0,xMax=0,yMax=0;
		advHeight = FontCodec.m_HYOS2.sTypoAscender - FontCodec.m_HYOS2.sTypoDescender;
		for(unsigned long i=0; i<FontCodec.m_HYMaxp.numGlyphs; i++)				
		{
			GlyfData& gData = MrgGlyfData[i];
			
			VMTX_LONGVRTMETRIC tagVMTXMetric;			
			if (gData.vtData.size()>0)
			{
				yMax = gData.vtData[8]<<8|(byte)gData.vtData[9];
				tagVMTXMetric.advanceHeight = advHeight;
				tagVMTXMetric.tsb = FontCodec.m_HYOS2.sTypoAscender - yMax;
			}
			else 
			{

				tagVMTXMetric.advanceHeight = advHeight;
				tagVMTXMetric.tsb = 0;
			}
			
			FontCodec.m_HYVmtx.vtLongVrtmetric.push_back(tagVMTXMetric);			
		}

		//Vhea
		int iGlyphNum = (int)FontCodec.m_HYMaxp.numGlyphs;
		FontCodec.m_HYVhea.numOfLongVerMetrics = 0;
		if (iGlyphNum>0)
		{
			int				iBaseadvanceHeight = MrgGlyfData[--iGlyphNum].usAdvHeight;		
			while(--iGlyphNum>=0)
			{				
				if(MrgGlyfData[iGlyphNum].usAdvHeight == iBaseadvanceHeight)
					FontCodec.m_HYVhea.numOfLongVerMetrics++;
				else 
					break;
			}

			FontCodec.m_HYVhea.numOfLongVerMetrics = (unsigned short)FontCodec.m_HYMaxp.numGlyphs-FontCodec.m_HYVhea.numOfLongVerMetrics;
		}

	}	// end of void CFontExtract::CountVertTable()

	int CFontExtract::BulidFont(HYFONTCODEC::CHYFontCodec& FontCodec, char* pGlpyData, int GlyphDataLen, const char* strNewTTFFile, CHYCodeMap& HYCodeMap)
	{
		FILE* pCreatFile = NULL;
		int err;
		pCreatFile = fopen(strNewTTFFile, "wb");
		if (pCreatFile == 0) {
			_get_errno(&err);
			_set_errno(0);
			return err;
		}

		WriteTableEntry(pCreatFile, FontCodec.m_HYTbDirectory);
		for (size_t i = 0; i != FontCodec.m_HYTbDirectory.vtTableEntry.size(); i++) {
			CHYTableEntry& HYEntry = FontCodec.m_HYTbDirectory.vtTableEntry[i];
			if (HYEntry.tag == LOCA_TAG){
				BulidLOCAL(FontCodec, HYEntry, pCreatFile);
			}
			else if (HYEntry.tag == GLYF_TAG){
				BulidGlyph(FontCodec, HYEntry, pCreatFile, pGlpyData, GlyphDataLen);
			}
			else if (HYEntry.tag == HEAD_TAG){
				BulidHead(FontCodec, HYEntry, pCreatFile);
			}			
			else if (HYEntry.tag == CMAP_TAG){
				MakeCMAP(FontCodec, HYCodeMap);
				FontCodec.Encodecmap(pCreatFile, FontCodec.m_HYTbDirectory);

				unsigned long ulcurrentPos = ftell(pCreatFile);
				int CheckBufSz = (HYEntry.length + 3) / 4 * 4;
				char* pCheckBuf = new char[CheckBufSz];
				fseek(pCreatFile, HYEntry.offset, SEEK_SET);
				fread(pCheckBuf, sizeof(char), CheckBufSz, pCreatFile);
				HYEntry.checkSum = HY_CalcFontTableChecksum((unsigned long*)pCheckBuf, HYEntry.length);
				if (pCheckBuf) delete[] pCheckBuf;
				fseek(pCreatFile, ulcurrentPos, SEEK_SET);
			}
			else if (HYEntry.tag == COLR_TAG){
				BulidCOLR(FontCodec, HYEntry, pCreatFile);
			}
			else if (HYEntry.tag == SVG_TAG) {
				BulidSVG(FontCodec, HYEntry, pCreatFile);
			}
			else{
				int iLength = (HYEntry.length + 3) / 4 * 4;
				char* pTabelData = new char[iLength];
				memset(pTabelData, 0, iLength);
				fseek(FontCodec.m_pFontFile, HYEntry.offset, SEEK_SET);
				fread(pTabelData, sizeof(char),HYEntry.length, FontCodec.m_pFontFile);
				// 偏移
				HYEntry.offset = ftell(pCreatFile);
				std::fwrite(pTabelData, sizeof(char), iLength, pCreatFile);
				if (pTabelData)	delete[] pTabelData;
			}
		}

		// 关闭源文件
		FontCodec.CloseFile();
		WriteTableEntry(pCreatFile, FontCodec.m_HYTbDirectory);
		// 关闭目标文件
		fflush(pCreatFile);
		fclose(pCreatFile);

		return FontCodec.SetCheckSumAdjustment(strNewTTFFile);

	}	// end of int CFontExtract::BulidFont()

	int CFontExtract::BulidFont(HYFONTCODEC::CHYFontCodec& FontCodec,char* pGlpyData,int GlyphDataLen,const char* strNewTTFFile, std::vector<ULONG>& vtTableFlag, CHYCodeMap& HYCodeMap)
	{	
		FILE* pCreatFile = NULL;
		int err;
		pCreatFile = fopen(strNewTTFFile,"wb");
		if (pCreatFile == 0){
			_get_errno(&err);
			_set_errno(0);
			return err;	
		}

		WriteTableEntry(pCreatFile, FontCodec.m_HYTbDirectory);
		for (size_t i=0; i!=FontCodec.m_HYTbDirectory.vtTableEntry.size(); i++) {
			CHYTableEntry&	HYEntry = FontCodec.m_HYTbDirectory.vtTableEntry[i];
			char* pTabelData = NULL;

			if (HitTable(vtTableFlag, HYEntry.tag))
			{
				if(HYEntry.tag == LOCA_TAG)
				{	
					BulidLOCAL(FontCodec,HYEntry,pCreatFile);
				}
				else if (HYEntry.tag == GLYF_TAG)
				{	
					BulidGlyph(FontCodec,HYEntry,pCreatFile,pGlpyData,GlyphDataLen);					
				}
				else if (HYEntry.tag == HEAD_TAG)
				{
					BulidHead(FontCodec,HYEntry,pCreatFile);
				}
				else if (HYEntry.tag == MAXP_TAG)
				{					
					BulidMAXP(FontCodec,HYEntry,pCreatFile);
				}
				else if (HYEntry.tag == CMAP_TAG)
				{
					BulidCMAP(FontCodec,HYEntry,pCreatFile,HYCodeMap);
				}
				else if (HYEntry.tag == HHEA_TAG)
				{
					BulidHHEA(FontCodec,HYEntry,pCreatFile);
				}
				else if (HYEntry.tag == HMTX_TAG)
				{
					BulidHMTX(FontCodec,HYEntry,pCreatFile);
				}
				else if (HYEntry.tag == VHEA_TAG)
				{
					BulidVHEA(FontCodec,HYEntry,pCreatFile);
				}
				else if (HYEntry.tag == VMTX_TAG)
				{
					BulidVMTX(FontCodec,HYEntry,pCreatFile);
				}
				else if (HYEntry.tag == POST_TAG)
				{
					BulidPOST(FontCodec,HYEntry,pCreatFile);
				}
				else if (HYEntry.tag  == COLR_TAG)
				{
					BulidCOLR(FontCodec,HYEntry,pCreatFile);
				}
			}				
			else 
			{
				int iLength = (HYEntry.length+3)/4*4;
				char* pTabelData = new char[iLength];
				memset(pTabelData, 0,iLength);
				fseek(FontCodec.m_pFontFile,HYEntry.offset, SEEK_SET);					
				fread(pTabelData,HYEntry.length,1,FontCodec.m_pFontFile);
				// 偏移
				HYEntry.offset = ftell(pCreatFile);
				fwrite(pTabelData,iLength,1,pCreatFile);
				if (pTabelData)
					delete[] pTabelData;
			}			
		}	

		// 关闭源文件
		FontCodec.CloseFile();
		WriteTableEntry(pCreatFile, FontCodec.m_HYTbDirectory);				
		// 关闭目标文件
		fflush(pCreatFile);
		fclose(pCreatFile);

		return FontCodec.SetCheckSumAdjustment(strNewTTFFile);

	}	// end of void CFontExtract::BulidFont()

	void CFontExtract::BulidHead(HYFONTCODEC::CHYFontCodec& FontCodec,CHYTableEntry&	HYEntry, FILE* pCreatFile)
	{
		int iLength = (HYEntry.length+3)/4*4;
		char* pTabelData = new char[iLength];
		memset(pTabelData, 0,iLength);
		fseek(FontCodec.m_pFontFile,HYEntry.offset, SEEK_SET);					
		fread(pTabelData,HYEntry.length,1,FontCodec.m_pFontFile);												
		memset(pTabelData+8,0,4);
		// 偏移
		HYEntry.offset = ftell(pCreatFile);
		fwrite(pTabelData,iLength,1,pCreatFile);
		if (pTabelData)
			delete [] pTabelData;
		pTabelData = NULL;

	}	// end of void CFontExtract::BulidHead()

	void CFontExtract::BulidGlyph(HYFONTCODEC::CHYFontCodec& FontCodec,CHYTableEntry&	HYEntry, FILE* pCreatFile,char* pGlpyData,int GlyphDataLen)
	{
		int iRealLen = (GlyphDataLen+3)/4*4;
		// 偏移
		HYEntry.offset = ftell(pCreatFile);
		fwrite(pGlpyData,sizeof(char), iRealLen,pCreatFile);
		HYEntry.length = GlyphDataLen;
		HYEntry.checkSum = HY_CalcFontTableChecksum((unsigned long*)pGlpyData,GlyphDataLen);

	}	// end of void CFontExtract::BulidGlyph()

	void CFontExtract::BulidMAXP(HYFONTCODEC::CHYFontCodec& FontCodec,CHYTableEntry&	HYEntry,FILE* pCreatFile)
	{
		int iLength = (HYEntry.length+3)/4*4;
		char* pTabelData = new char[iLength];
		memset(pTabelData, 0,iLength);
		fseek(FontCodec.m_pFontFile,HYEntry.offset, SEEK_SET);					
		fread(pTabelData,HYEntry.length,1,FontCodec.m_pFontFile);
		unsigned short tmp = hy_cdr_int16_to(FontCodec.m_HYMaxp.numGlyphs);
		memcpy(pTabelData+4,&tmp,2);

		// 偏移
		HYEntry.offset = ftell(pCreatFile);
		fwrite(pTabelData,iLength,1,pCreatFile);
		HYEntry.checkSum = HY_CalcFontTableChecksum((unsigned long*)pTabelData,iLength);
		if (pTabelData)
			delete [] pTabelData;
		pTabelData = NULL;

	}	// end of void CFontExtract::BulidMAXP()

	void CFontExtract::BulidLOCAL(HYFONTCODEC::CHYFontCodec& FontCodec,CHYTableEntry& HYEntry,FILE* pCreatFile)
	{
		char*			pLocal				= NULL;
		unsigned long	ulLocalIndex		= 0;
		int				BuffLength			= 0;

		size_t stNums = FontCodec.m_HYloca.vtLoca.size();
		if (FontCodec.m_HYhead.indexToLocFormat==0)	{		
			BuffLength = (stNums*2+3)/4*4;
			pLocal = new char[BuffLength];
			memset(pLocal,0,BuffLength);					
			for(size_t i=0; i!=stNums; i++)	{
				unsigned short soffset = hy_cdr_int16_to(FontCodec.m_HYloca.vtLoca[i]>>1);
				memcpy(pLocal+ulLocalIndex,&soffset,2);
				ulLocalIndex+=2;
			}
		}
		if (FontCodec.m_HYhead.indexToLocFormat==1)	{
			BuffLength = stNums*4;
			pLocal = new char[BuffLength];
			memset(pLocal,0,BuffLength);
			for(size_t i=0; i!=stNums; i++)	{
				long loffset = hy_cdr_int32_to(FontCodec.m_HYloca.vtLoca[i]);
				memcpy(pLocal+ulLocalIndex,&loffset,4);
				ulLocalIndex += 4;
			}
		}
		// 偏移
		HYEntry.offset = ftell(pCreatFile);
		fwrite(pLocal,BuffLength,1,pCreatFile);
		HYEntry.length = ulLocalIndex;
		HYEntry.checkSum = HY_CalcFontTableChecksum((unsigned long*)pLocal,ulLocalIndex);

		if (pLocal) 
			delete[] pLocal;
		pLocal = NULL;

	}	// end of void CFontExtract::BulidLOCAL()

	void CFontExtract::MakeCMAP(HYFONTCODEC::CHYFontCodec& FontCodec, CHYCodeMap& HYCodeMap)
	{
		FontCodec.m_HYCodeMap.vtHYCodeMap.clear();

		CHYCmap cmap;
		CMAP_TABLE_ENTRY	entry;
		entry.plat_ID = 3;
		entry.plat_encod_ID = 1;
		entry.format = CMAP_ENCODE_FT_4;
		cmap.vtCamp_tb_entry.push_back(entry);

		HYCodeMap.FilterByUnicode(0xffffffff);
		HYCodeMap.QuickSortbyUnicode();
		if (HYCodeMap.vtHYCodeMap[HYCodeMap.vtHYCodeMap.size() - 1].ulGlyphNo > 0xFFFF) {
			entry.plat_ID = 3;
			entry.plat_encod_ID = 10;
			entry.format = CMAP_ENCODE_FT_12;
			cmap.vtCamp_tb_entry.push_back(entry);
		}

		int iIndx = FontCodec.m_HYCmap.FindSpecificEntryIndex(CMAP_ENCODE_FT_14);
		if (iIndx != -1) {
			CMAP_TABLE_ENTRY Entry =  FontCodec.m_HYCmap.vtCamp_tb_entry[iIndx];
			cmap.vtCamp_tb_entry.push_back(Entry);
		}
		cmap.numSubTable = (unsigned short)cmap.vtCamp_tb_entry.size();

		FontCodec.m_HYCodeMap = HYCodeMap;
#if 0
		
		FontCodec.m_HYCmap.SetDefault();
		FontCodec.m_HYCmap.version = 0;
		FontCodec.m_HYCmap.numSubTable = (unsigned short)cmap.vtCamp_tb_entry.size();
		for (int i = 0; i < FontCodec.m_HYCmap.numSubTable; i++){
			FontCodec.m_HYCmap.vtCamp_tb_entry.push_back(cmap.vtCamp_tb_entry[i]);
		}
#else
		FontCodec.m_HYCmap = cmap;
#endif 	
	}	// end of void CFontExtract::MakeCMAP()

	void CFontExtract::BulidCMAP(HYFONTCODEC::CHYFontCodec& FontCodec,CHYTableEntry& HYEntry,FILE* pCreatFile, CHYCodeMap& HYCodeMap)
	{
#if 0
		CHYCmap cmap;
		CMAP_TABLE_ENTRY	entry;		

		entry.plat_ID		= 3;
		entry.plat_encod_ID = 1;
		entry.format		= CMAP_ENCODE_FT_4;
		cmap.vtCamp_tb_entry.push_back(entry);
		
		HYCodeMap.FilterByUnicode(0xffffffff);
		HYCodeMap.QuickSortbyUnicode();

		if (HYCodeMap.vtHYCodeMap[HYCodeMap.vtHYCodeMap.size()-1].ulGlyphNo > 0xFFFF){
			entry.plat_ID		= 3;
			entry.plat_encod_ID = 10;
			entry.format		= CMAP_ENCODE_FT_12;
			cmap.vtCamp_tb_entry.push_back(entry);
		}

		cmap.version = 0;
		cmap.numSubTable = (unsigned short)cmap.vtCamp_tb_entry.size();

		unsigned long iLength = 0;
		char* pTabelData = new char[65535*cmap.numSubTable * 2];
		memset(pTabelData,0,65535*cmap.numSubTable*2);

		unsigned short  usTmp = 0;
		unsigned long	ulTmp = 0;
		usTmp = hy_cdr_int16_to(cmap.version);
		memcpy(pTabelData,&usTmp,2);
		iLength+=2;
		usTmp = hy_cdr_int16_to(cmap.numSubTable);
		memcpy(pTabelData+iLength,&usTmp,2);
		iLength+=2;

		int i = 0;
		std::vector<int> vtoffset;
		for (i=0; i<cmap.numSubTable; i++){
			CMAP_TABLE_ENTRY&	tbEntry = cmap.vtCamp_tb_entry[i];
			// platformID
			usTmp = hy_cdr_int16_to(tbEntry.plat_ID);
			memcpy(pTabelData+iLength,&usTmp,2);
			iLength+=2;
			// encodingID
			usTmp = hy_cdr_int16_to(tbEntry.plat_encod_ID);
			memcpy(pTabelData+iLength,&usTmp,2);
			iLength+=2;						
			// offset
			vtoffset.push_back(iLength);
			tbEntry.offset = 0;
			memcpy(pTabelData+iLength,&tbEntry.offset,4);
			iLength+=4;		
		}

		for (i=0; i<cmap.numSubTable; i++){
			std::vector<char> vtCampData;
			CMAP_TABLE_ENTRY&	tbEntry = cmap.vtCamp_tb_entry[i];
			switch(tbEntry.format){					
			case CMAP_ENCODE_FT_4:{								
					EncodeCmapFmt4(tbEntry,vtCampData,HYCodeMap);									
					ulTmp = hy_cdr_int32_to(iLength);
					memcpy(pTabelData+vtoffset[i],&ulTmp,4);
					for (size_t i=0; i<vtCampData.size();i++){
						memcpy(pTabelData+iLength,&vtCampData[i],1);
						iLength++;
					}								
				}								
				break;
			case CMAP_ENCODE_FT_12:	{
					EncodeCmapFmt12(tbEntry,vtCampData,HYCodeMap);
					ulTmp = hy_cdr_int32_to(iLength);
					memcpy(pTabelData+vtoffset[i],&ulTmp,4);
					for (size_t i=0; i<vtCampData.size();i++){
						memcpy(pTabelData+iLength,&vtCampData[i],1);
						iLength++;
					}									
				}								
				break;
			default:
				break;
			}
		}

		// 偏移
		HYEntry.length = iLength;					
		HYEntry.offset = ftell(pCreatFile);
		iLength = (iLength+3)/4*4;					
		fwrite(pTabelData,iLength,1,pCreatFile);
		HYEntry.checkSum = HY_CalcFontTableChecksum((unsigned long*)pTabelData,iLength);	
		if (pTabelData)
			delete[] pTabelData;
		pTabelData = NULL;
#endif 
	}	// end of void CFontExtract::BulidCMAP()

	void CFontExtract::BulidHHEA(HYFONTCODEC::CHYFontCodec& FontCodec,CHYTableEntry& HYEntry,FILE* pCreatFile)
	{
		int iLength = (HYEntry.length+3)/4*4;
		char* pTabelData = new char[iLength];
		memset(pTabelData, 0,iLength);
		fseek(FontCodec.m_pFontFile,HYEntry.offset, SEEK_SET);					
		fread(pTabelData,HYEntry.length,1,FontCodec.m_pFontFile);
		unsigned short tmp = hy_cdr_int16_to(FontCodec.m_HYHhea.numberOfHMetrics);
		memcpy(pTabelData+34,&tmp,2);

		// 偏移
		HYEntry.offset = ftell(pCreatFile);
		fwrite(pTabelData,iLength,1,pCreatFile);
		HYEntry.checkSum = HY_CalcFontTableChecksum((unsigned long*)pTabelData,iLength);

		if (pTabelData)
			delete[] pTabelData;
		pTabelData = NULL;

	}	// end of void CFontExtract::BulidHHEA()

	void CFontExtract::BulidHMTX(HYFONTCODEC::CHYFontCodec& FontCodec,CHYTableEntry& HYEntry,FILE* pCreatFile)
	{
		unsigned short	usTmp = 0;

		size_t			szGlyphNum			= FontCodec.m_HYMaxp.numGlyphs;
		unsigned short longhormetricNums	= FontCodec.m_HYHhea.numberOfHMetrics;

		//int iLength = (HYEntry.length+3)/4*4;
		int iLength = sizeof(HMTX_LONGHORMERTRIC)*szGlyphNum;
		char* pTabelData = new char[iLength];
		memset(pTabelData,0,iLength);
		iLength = 0;		
		for (size_t i=0; i<szGlyphNum; i++)
		{		
			if (i<longhormetricNums)
			{	
				usTmp = hy_cdr_int16_to(FontCodec.m_HYHmtx.vtLonghormetric[i].advanceWidth);
				memcpy(pTabelData+iLength,&usTmp,2);
				iLength+=2;
				usTmp = hy_cdr_int16_to(FontCodec.m_HYHmtx.vtLonghormetric[i].lsb);
				memcpy(pTabelData+iLength,&usTmp,2);
				iLength+=2;
			}
			else 
			{
				usTmp = hy_cdr_int16_to(FontCodec.m_HYHmtx.vtLonghormetric[i].lsb);
				memcpy(pTabelData+iLength,&usTmp,2);
				iLength+=2;							
			}
		}

		HYEntry.length = iLength;					
		HYEntry.offset = ftell(pCreatFile);
		iLength = (iLength+3)/4*4;					
		fwrite(pTabelData,iLength,1,pCreatFile);
		HYEntry.checkSum = HY_CalcFontTableChecksum((unsigned long*)pTabelData,iLength);

		if (pTabelData)
			delete[] pTabelData;
		pTabelData = NULL;

	}	// end of void CFontExtract::BulidHMTX()

	void CFontExtract::BulidVHEA(HYFONTCODEC::CHYFontCodec& FontCodec, CHYTableEntry& HYEntry, FILE* pCreatFile)
	{
		// 如果原始字库有VMTX或VHEA
		if (HYEntry.offset!=0 && HYEntry.length!=0 && HYEntry.checkSum!=0)
		{			
			int iLength = (HYEntry.length+3)/4*4;
			char* pTabelData = new char[iLength];
			memset(pTabelData, 0,iLength);
			fseek(FontCodec.m_pFontFile,HYEntry.offset, SEEK_SET);					
			fread(pTabelData,HYEntry.length,1,FontCodec.m_pFontFile);
			// 在这里赋值新的VHEA表中
			unsigned short tmp = hy_cdr_int16_to(FontCodec.m_HYVhea.numOfLongVerMetrics);
			memcpy(pTabelData+34,&tmp,2);

			// 偏移
			HYEntry.offset = ftell(pCreatFile);
			fwrite(pTabelData,iLength,1,pCreatFile);
			HYEntry.checkSum = HY_CalcFontTableChecksum((unsigned long*)pTabelData,iLength);

			if (pTabelData)
				delete[] pTabelData;
			pTabelData = NULL;
		}
		else 
		{
			int itblLen = 36;
			unsigned short usTmp =0;
			char* pTabelData = new char[itblLen];
			memset(pTabelData,0,itblLen);			
					
			// version
			usTmp = hy_cdr_int16_to(1);
			memcpy(pTabelData,&usTmp,2);		
			usTmp = 0;
			memcpy(pTabelData+2,&usTmp,2);
			// ascent

			usTmp =(unsigned short)FontCodec.m_HYhead.unitsPerEm/2;
			usTmp = hy_cdr_int16_to(usTmp);
			memcpy(pTabelData+4,&usTmp,2);			
			// descent
			usTmp =(unsigned short)FontCodec.m_HYhead.unitsPerEm/2;
			usTmp = hy_cdr_int16_to(usTmp);
			memcpy(pTabelData+6,&usTmp,2);
			// lineGap
			usTmp = hy_cdr_int16_to(0);
			memcpy(pTabelData+8,&usTmp,2);
			// advanceHeightMax
			usTmp = hy_cdr_int16_to(GetAdvancMaxHeight(FontCodec));
			memcpy(pTabelData+10,&usTmp,2);
			// minTopSideBearing
			//usTmp = hy_cdr_int16_to((unsigned short)FindVertMinTop(FontCodec));
			usTmp = hy_cdr_int16_to(0);
			memcpy(pTabelData+12,&usTmp,2);
			//minBottomSideBearing
			//usTmp = hy_cdr_int16_to((unsigned short)FindVertMinBottom(FontCodec));
			usTmp = hy_cdr_int16_to(0);
			memcpy(pTabelData+14,&usTmp,2);
			//yMaxExtent
			short MaxExtent = FontCodec.m_HYhead.yMax - FontCodec.m_HYhead.yMin;
			usTmp = hy_cdr_int16_to(MaxExtent);
			memcpy(pTabelData+16,&usTmp,2);
			//caretSlopeRise
			usTmp = 0;
			memcpy(pTabelData+18,&usTmp,2);
			// caretSlopeRun
			usTmp = hy_cdr_int16_to(1);
			memcpy(pTabelData+20,&usTmp,2);
			//caretOffset
			usTmp = 0;
			memcpy(pTabelData+22,&usTmp,2);
			//reserved			
			memcpy(pTabelData+24,&usTmp,2);
			//reserved			
			memcpy(pTabelData+26,&usTmp,2);
			//reserved			
			memcpy(pTabelData+28,&usTmp,2);
			//reserved
			memcpy(pTabelData+30,&usTmp,2);			
			//metricDataFormat
			memcpy(pTabelData+32,&usTmp,2);
			//numOfLongVerMetrics	
			usTmp = hy_cdr_int16_to(FontCodec.m_HYVhea.numOfLongVerMetrics);
			memcpy(pTabelData+34,&usTmp,2);

			HYEntry.length = 36;					
			HYEntry.offset = ftell(pCreatFile);			
			fwrite(pTabelData,HYEntry.length,1,pCreatFile);
			HYEntry.checkSum = HY_CalcFontTableChecksum((unsigned long*)pTabelData,36);

			if (pTabelData)
				delete[] pTabelData;
			pTabelData = NULL;
		}

	}	// end of void CFontExtract::BulidVHEA()

	void CFontExtract::BulidVMTX(HYFONTCODEC::CHYFontCodec& FontCodec, CHYTableEntry& HYEntry,FILE* pCreatFile)
	{
		unsigned short	usTmp = 0;

		size_t			szGlyphNum			= FontCodec.m_HYMaxp.numGlyphs;
		unsigned short longvermetricNums	= FontCodec.m_HYVhea.numOfLongVerMetrics;

		int iLength = sizeof(VMTX_LONGVRTMETRIC)*szGlyphNum;
		char* pTabelData = new char[iLength];
		memset(pTabelData,0,iLength);
		iLength = 0;

		for (size_t i=0; i<szGlyphNum; i++)	{		
			if (i<longvermetricNums){	
				usTmp = hy_cdr_int16_to(FontCodec.m_HYVmtx.vtLongVrtmetric[i].advanceHeight);
				memcpy(pTabelData+iLength,&usTmp,2);
				iLength+=2;
				usTmp = hy_cdr_int16_to(FontCodec.m_HYVmtx.vtLongVrtmetric[i].tsb);
				memcpy(pTabelData+iLength,&usTmp,2);
				iLength+=2;
			}
			else {
				usTmp = hy_cdr_int16_to(FontCodec.m_HYVmtx.vtLongVrtmetric[i].tsb);
				memcpy(pTabelData+iLength,&usTmp,2);
				iLength+=2;							
			}
		}

		HYEntry.length = iLength;
		HYEntry.offset = ftell(pCreatFile);
		iLength = (iLength+3)/4*4;			
		fwrite(pTabelData,iLength,1,pCreatFile);
		HYEntry.checkSum = HY_CalcFontTableChecksum((unsigned long*)pTabelData,iLength);

		if (pTabelData)
			delete[] pTabelData;
		pTabelData = NULL;

	}	// end of void CFontExtract::BulidVMTX()

	void CFontExtract::BulidPOST(HYFONTCODEC::CHYFontCodec& FontCodec, CHYTableEntry& HYEntry,FILE* pCreatFile)
	{
		unsigned short	usTmp = 0;
		unsigned long	ulTmp = 0;

		int iLength = 32+FontCodec.m_HYMaxp.numGlyphs*64;
		char* pTabelData = new char[iLength];
		memset(pTabelData,0,iLength);
		iLength = 0;
			
		usTmp = hy_cdr_int16_to(FontCodec.m_HYPost.Format.value);
		memcpy(pTabelData+iLength,&usTmp,2);
		iLength+=2;
		usTmp = hy_cdr_int16_to(FontCodec.m_HYPost.Format.fract);
		memcpy(pTabelData+iLength,&usTmp,2);	
		iLength+=2;

		// italicAngle
		usTmp = hy_cdr_int16_to(FontCodec.m_HYPost.italicAngle.value);
		memcpy(pTabelData+iLength,&usTmp,2);	
		iLength+=2;
		usTmp = hy_cdr_int16_to(FontCodec.m_HYPost.italicAngle.fract);
		memcpy(pTabelData+iLength,&usTmp,2);	
		iLength+=2;

		//underlinePosition
		if (FontCodec.m_HYPost.underlinePosition==0){
			FontCodec.m_HYPost.underlinePosition = FontCodec.m_HYhead.yMin;
		}
		usTmp = hy_cdr_int16_to(FontCodec.m_HYPost.underlinePosition);
		memcpy(pTabelData+iLength,&usTmp,2);	
		iLength+=2;

		//UnderlineThickness
		if (FontCodec.m_HYPost.underlineThickness==0){
			FontCodec.m_HYPost.underlineThickness = FontCodec.m_HYhead.unitsPerEm/20;
		}

		usTmp = hy_cdr_int16_to(FontCodec.m_HYPost.underlineThickness);
		memcpy(pTabelData+iLength,&usTmp,2);	
		iLength+=2;

		//isFixedPitch
		ulTmp = hy_cdr_int32_to(FontCodec.m_HYPost.isFixedPitch);
		memcpy(pTabelData+iLength,&ulTmp,4);	
		iLength+=4;

		ulTmp = 0;
		//minMemType42
		memcpy(pTabelData+iLength,&ulTmp,4);	
		iLength+=4;
		//maxMemType42
		memcpy(pTabelData+iLength,&ulTmp,4);	
		iLength+=4;
		//minMemType1
		memcpy(pTabelData+iLength,&ulTmp,4);	
		iLength+=4;
		//maxMemType1
		memcpy(pTabelData+iLength,&ulTmp,4);	
		iLength+=4;
	
		if (FontCodec.m_HYPost.Format.value == 2 && FontCodec.m_HYPost.Format.fract == 0){
			// numberOfGlyphs
			usTmp = hy_cdr_int16_to(FontCodec.m_HYMaxp.numGlyphs);
			memcpy(pTabelData + iLength, &usTmp, 2);
			iLength += 2;

			//glyphNameIndex
			size_t  stTmp = m_vtPsName.size();
			if (stTmp != FontCodec.m_HYMaxp.numGlyphs){
				for (int i = 0; i <FontCodec.m_HYMaxp.numGlyphs; i++){
					if (i == 0) {
						m_vtPsName.insert(m_vtPsName.begin(), ".notdef");
					}

					if (stTmp == 0) {
						char  buffer[200] = { 0 };
						_stprintf(buffer, "GID%d", i);
						m_vtPsName.push_back(buffer);
					}
					unsigned short usPostNameID = FontCodec.m_HYPost.InsertName(m_vtPsName[i]);
					usTmp = hy_cdr_int16_to(usPostNameID);
					memcpy(pTabelData + iLength, &usTmp, 2);
					iLength += 2;
				}
			}
			else{
				for (int i = 0; i < FontCodec.m_HYMaxp.numGlyphs; i++) {
					unsigned short usPostNameID = FontCodec.m_HYPost.InsertName(m_vtPsName[i]);
					usTmp = hy_cdr_int16_to(usPostNameID);
					memcpy(pTabelData + iLength, &usTmp, 2);
					iLength += 2;
				}
			}
			
			//names
			size_t st = FontCodec.m_HYPost.m_vtStandString.size();
			for (size_t i = 258; i <st; i++){
				const char* strData = FontCodec.m_HYPost.m_vtStandString[i].data();
				size_t t = strlen(strData);
				memcpy(pTabelData + iLength++, (byte*)&t, 1);
				memcpy(pTabelData + iLength, strData, t);
				iLength += t;
			}
		}
		
		HYEntry.length = iLength;
		HYEntry.offset = ftell(pCreatFile);
		iLength = (iLength+3)/4*4;			
		fwrite(pTabelData,iLength,1,pCreatFile);
		HYEntry.checkSum = HY_CalcFontTableChecksum((unsigned long*)pTabelData,iLength);
		if(pTabelData)
			delete[] pTabelData;
		pTabelData = NULL;

	}	// end of void CFontExtract::BulidPOST()

	void CFontExtract::BulidCOLR(HYFONTCODEC::CHYFontCodec& FontCodec, CHYTableEntry& HYEntry,FILE* pCreatFile)
	{
		unsigned short usTmp=0;
		unsigned long ulTmp=0;

		HYEntry.offset = ftell(pCreatFile);
		
		// head
		usTmp = hy_cdr_int16_to(FontCodec.m_HYCOLR.version);
		fwrite(&usTmp,2,1,pCreatFile);
		usTmp = hy_cdr_int16_to(FontCodec.m_HYCOLR.numBaseGlyphRecords);
		fwrite(&usTmp,2,1,pCreatFile);
		long bGROffset = ftell(pCreatFile);
		ulTmp = hy_cdr_int32_to(FontCodec.m_HYCOLR.baseGlyphRecordsOffset);
		fwrite(&ulTmp,4,1,pCreatFile);
		long lROffset = ftell(pCreatFile);
		ulTmp = hy_cdr_int32_to(FontCodec.m_HYCOLR.layerRecordsOffset);
		fwrite(&ulTmp,4,1,pCreatFile);
		usTmp = hy_cdr_int16_to(FontCodec.m_HYCOLR.numLayerRecords);
		fwrite(&usTmp,2,1,pCreatFile);

		long BaseGlyphlth = 0;
		for (int i=0; i<FontCodec.m_HYCOLR.numBaseGlyphRecords; i++){
			const CBaseGlyphRecord& BaseGlyphRecord = FontCodec.m_HYCOLR.vtBaseGlyphRecord[i];

			usTmp = hy_cdr_int16_to(BaseGlyphRecord.GID);
			fwrite(&usTmp,2,1,pCreatFile);
			usTmp = hy_cdr_int16_to(BaseGlyphRecord.firstLayerIndex);
			fwrite(&usTmp,2,1,pCreatFile);
			usTmp = hy_cdr_int16_to(BaseGlyphRecord.numLayers );
			fwrite(&usTmp,2,1,pCreatFile);

			BaseGlyphlth+=6;
		}

		for (int i=0; i<FontCodec.m_HYCOLR.numLayerRecords; i++){
			const CLayerRecord& LayerRecord = FontCodec.m_HYCOLR.vtLayerRecord[i];

			usTmp = hy_cdr_int16_to(LayerRecord.GID);
			fwrite(&usTmp,2,1,pCreatFile);
			usTmp = hy_cdr_int16_to(LayerRecord.paletteIndex);
			fwrite(&usTmp,2,1,pCreatFile);				
		}

		int iTMP = ftell(pCreatFile);
		fseek(pCreatFile, bGROffset,SEEK_SET);
		FontCodec.m_HYCOLR.baseGlyphRecordsOffset = 14;
		ulTmp = hy_cdr_int32_to(FontCodec.m_HYCOLR.baseGlyphRecordsOffset);
		fwrite(&ulTmp,4,1,pCreatFile);
		FontCodec.m_HYCOLR.layerRecordsOffset = 14 + BaseGlyphlth;
		ulTmp = hy_cdr_int32_to(FontCodec.m_HYCOLR.layerRecordsOffset);
		fwrite(&ulTmp,4,1,pCreatFile);

		fseek(pCreatFile, iTMP,SEEK_SET);

		HYEntry.length = ftell(pCreatFile) - HYEntry.offset;
		int iTail = 4-HYEntry.length%4;
		if (HYEntry.length%4>0)	{
			char c = 0;
			for (int i=0; i<iTail; i++)	{
				fwrite(&c,1,1,pCreatFile);
			}				
		}

		fflush(pCreatFile);

		unsigned long ulcurrentPos = ftell(pCreatFile);
		int CheckBufSz = (HYEntry.length + 3) / 4 * 4;
		char* pCheckBuf = new char[CheckBufSz];
		fseek(pCreatFile, HYEntry.offset, SEEK_SET);
		fread(pCheckBuf, sizeof(char),1, pCreatFile);
		HYEntry.checkSum = HY_CalcFontTableChecksum((unsigned long*)pCheckBuf, HYEntry.length);
		if (pCheckBuf) delete[] pCheckBuf;
		fseek(pCreatFile, ulcurrentPos, SEEK_SET);

	}	// end of void CFontExtract::BulidCOLR()

	void CFontExtract::BulidSVG(CHYFontCodec& FontCodec, CHYTableEntry& HYEntry, FILE* pCreatFile)
	{		
		HYEntry.offset = ftell(pCreatFile);

		unsigned short usTmp = 0;
		unsigned long ulTmp = 0;

		// head
		usTmp = hy_cdr_int16_to(FontCodec.m_HYSVG.Header.version);
		fwrite(&usTmp, 2, 1, pCreatFile);
		FontCodec.m_HYSVG.Header.svgDocumentListOffset = 10;//version+svgDocumentListOffset+reserved
		ulTmp = hy_cdr_int32_to(FontCodec.m_HYSVG.Header.svgDocumentListOffset);
		fwrite(&ulTmp, 4, 1, pCreatFile);
		ulTmp = hy_cdr_int32_to(FontCodec.m_HYSVG.Header.reserved);
		fwrite(&ulTmp, 4, 1, pCreatFile);

		//SVG Document List
		long lDocLstLength = 2+FontCodec.m_HYSVG.DocumentList.numEntries * 12;
		usTmp = hy_cdr_int16_to(FontCodec.m_HYSVG.DocumentList.numEntries);
		fwrite(&usTmp, 2, 1, pCreatFile);
		unsigned long ulOffset = lDocLstLength;
		for (int i = 0; i < FontCodec.m_HYSVG.DocumentList.numEntries; i++)	{
			SVGDocumentRecord& Record = FontCodec.m_HYSVG.DocumentList.vtDocumentRecord[i];

			usTmp = hy_cdr_int16_to(Record.startGlyphID);
			fwrite(&usTmp, 2, 1, pCreatFile);
			usTmp = hy_cdr_int16_to(Record.endGlyphID);
			fwrite(&usTmp, 2, 1, pCreatFile);

			Record.svgDocOffset = lDocLstLength;
			ulTmp = hy_cdr_int32_to(Record.svgDocOffset);
			fwrite(&ulTmp, 4, 1, pCreatFile);
			ulTmp = hy_cdr_int32_to(Record.svgDocLength);
			fwrite(&ulTmp, 4, 1, pCreatFile);

			lDocLstLength += Record.svgDocLength;
		}

		for (int i = 0; i < FontCodec.m_HYSVG.DocumentList.numEntries; i++)	{
			SVGDocumentRecord& Record = FontCodec.m_HYSVG.DocumentList.vtDocumentRecord[i];
			for (int j = 0; j < Record.svgDocLength; j++) {
				fwrite(&Record.vtDocData[j], 1, 1, pCreatFile);
			}
		}

		HYEntry.length = ftell(pCreatFile) - HYEntry.offset;
		int iTail = 4 - HYEntry.length % 4;
		if (HYEntry.length % 4 > 0)	{
			char c = 0;
			for (int i = 0; i < iTail; i++)	{
				fwrite(&c, 1, 1, pCreatFile);
			}
		}

		fflush(pCreatFile);

		unsigned long ulcurrentPos  = ftell(pCreatFile);
		int CheckBufSz = (HYEntry.length + 3) / 4 * 4;
		char* pCheckBuf = new char[CheckBufSz];
		fseek(pCreatFile, HYEntry.offset, SEEK_SET);
		fread(pCheckBuf, sizeof(char),CheckBufSz, pCreatFile);
		HYEntry.checkSum = HY_CalcFontTableChecksum((unsigned long*)pCheckBuf, HYEntry.length);
		if (pCheckBuf) delete[] pCheckBuf;
		fseek(pCreatFile, ulcurrentPos, SEEK_SET);

	}	// void CFontExtract::BulidSVG()

	int	CFontExtract::FindVertMinTop(HYFONTCODEC::CHYFontCodec& FontCodec)
	{	
		int minTop = 0;
		size_t stVerMetricsNum = FontCodec.m_HYVhea.numOfLongVerMetrics;	
		if (stVerMetricsNum>0)
		{
			short minTop = FontCodec.m_HYVmtx.vtLongVrtmetric[0].tsb;
			for (size_t i=1; i<stVerMetricsNum; i++)
			{					
				if (FontCodec.m_HYVmtx.vtLongVrtmetric[i].tsb<minTop)  
				{
					minTop = FontCodec.m_HYVmtx.vtLongVrtmetric[i].tsb;	
				}	
			}

			size_t stTopsidebearNum = FontCodec.m_HYVmtx.vtTopsidebearing.size();		
			for (size_t i=0; i<stTopsidebearNum; i++)
			{					
				if (FontCodec.m_HYVmtx.vtTopsidebearing[i]<minTop)  
				{
					minTop = FontCodec.m_HYVmtx.vtTopsidebearing[i];
				}	
			}
		}

		return minTop;

	}	// end of int	CFontExtract::FindVertMinTop()

	int	CFontExtract::FindVertMinBottom(HYFONTCODEC::CHYFontCodec& FontCodec)
	{
		short minBottom = 0;
		size_t stGlyphNum = FontCodec.m_vtHYGlyphs.size();
		if (stGlyphNum>0)
		{			
			//minBottom = m_vtHYGlyphs[0].minY-m_HYhead.yMin;
			minBottom = FontCodec.m_vtHYGlyphs[0].minY-FontCodec.m_HYOS2.sTypoDescender;
			for (size_t i=0; i<stGlyphNum; i++)
			{			
				CHYGlyph& Gryph = FontCodec.m_vtHYGlyphs[i];				
				if (Gryph.minY-FontCodec.m_HYhead.yMin<minBottom)  
				{					
					minBottom = Gryph.minY-FontCodec.m_HYhead.yMin;
				}
			}
		}

		return minBottom;

	}	// end of int CFontExtract::FindVertMinBottom()

	unsigned short	CFontExtract::GetAdvancMaxHeight(HYFONTCODEC::CHYFontCodec& FontCodec)
	{
		short  maxAdh = 0;
		size_t stVerMetricsNum = FontCodec.m_HYVhea.numOfLongVerMetrics;

		if (stVerMetricsNum>0)
		{
			maxAdh = FontCodec.m_HYVmtx.vtLongVrtmetric[0].advanceHeight;
			for (size_t i=1; i<stVerMetricsNum; i++)
			{					
				if (FontCodec.m_HYVmtx.vtLongVrtmetric[i].advanceHeight>maxAdh)  
				{
					maxAdh = FontCodec.m_HYVmtx.vtLongVrtmetric[i].advanceHeight;
				}	
			}			
		}

		return maxAdh;

	}	// end of unsigned short CFontExtract::GetAdvancMaxHeight()


}	
