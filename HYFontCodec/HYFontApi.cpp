#include "stdafx.h"

#include "HYFontApi.h"
#include "FontExtract.h"
#include "HYBaseFunction.h"
#include "WoffCodec.h"
#include "HYFontApiPrivate.h"
#include "boost/algorithm/string.hpp"
#include "HYZIPCodec.h"

using namespace  boost::algorithm;
using namespace HYFONTCODEC;
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void EmojiToXml(CMarkup& Emoji,char* pDirectory,CHYFontCodec& FontCodec)
{
	Emoji.AddElem("Emoji");
	Emoji.AddAttrib("type", 0);
	Emoji.IntoElem();

	CHYCBLC& CBLC = FontCodec.m_HYCblc;
	size_t stBitMapTB = CBLC.Header.numsizes;
	if (stBitMapTB > 0)	// opentype体系Emoji字库表项
	{
		Emoji.AddElem("OpenType");
		Emoji.IntoElem();

		//Emoji.AddElem("bitmapSize", stBitMapTB);
		std::vector<string> szbmpSaveDir;
		for (size_t i = 0; i < stBitMapTB; i++)
		{
			BitmapSize& btMapSzTable = CBLC.vtBitmapSizeTb[i];
			Emoji.AddElem("bitmapSizeTable");
			Emoji.IntoElem();

			Emoji.AddElem("sbitLineMetrics_hori");
			Emoji.IntoElem();
			Emoji.AddElem("ascender", btMapSzTable.Hori.ascender);
			Emoji.AddElem("descender", btMapSzTable.Hori.descender);
			Emoji.AddElem("widthMax", btMapSzTable.Hori.widthMax);
			Emoji.AddElem("caretSlopeNumerator", btMapSzTable.Hori.caretSlopeNumerator);
			Emoji.AddElem("caretSlopeDenominator", btMapSzTable.Hori.caretSlopeDenominator);
			Emoji.AddElem("caretOffset", btMapSzTable.Hori.caretOffset);
			Emoji.AddElem("minOriginSB", btMapSzTable.Hori.minOriginSB);
			Emoji.AddElem("minAdvanceSB", btMapSzTable.Hori.minAdvanceSB);
			Emoji.AddElem("maxBeforeBL", btMapSzTable.Hori.maxBeforeBL);
			Emoji.AddElem("minAfterBL", btMapSzTable.Hori.minAfterBL);
			Emoji.AddElem("pad1", btMapSzTable.Hori.pad1);
			Emoji.AddElem("pad2", btMapSzTable.Hori.pad2);
			Emoji.OutOfElem();

			Emoji.AddElem("sbitLineMetrics_vert");
			Emoji.IntoElem();
			Emoji.AddElem("ascender", btMapSzTable.Vert.ascender);
			Emoji.AddElem("descender", btMapSzTable.Vert.descender);
			Emoji.AddElem("widthMax", btMapSzTable.Vert.widthMax);
			Emoji.AddElem("caretSlopeNumerator", btMapSzTable.Vert.caretSlopeNumerator);
			Emoji.AddElem("caretSlopeDenominator", btMapSzTable.Vert.caretSlopeDenominator);
			Emoji.AddElem("caretOffset", btMapSzTable.Vert.caretOffset);
			Emoji.AddElem("minOriginSB", btMapSzTable.Vert.minOriginSB);
			Emoji.AddElem("minAdvanceSB", btMapSzTable.Vert.minAdvanceSB);
			Emoji.AddElem("maxBeforeBL", btMapSzTable.Vert.maxBeforeBL);
			Emoji.AddElem("minAfterBL", btMapSzTable.Vert.minAfterBL);
			Emoji.AddElem("pad1", btMapSzTable.Vert.pad1);
			Emoji.AddElem("pad2", btMapSzTable.Vert.pad2);
			Emoji.OutOfElem();

			Emoji.AddElem("startGlyphIndex", btMapSzTable.startGlyphIndex);
			Emoji.AddElem("endGlyphIndex", btMapSzTable.endGlyphIndex);
			Emoji.AddElem("ppemX", btMapSzTable.ppemX);
			Emoji.AddElem("ppemY", btMapSzTable.ppemY);
			Emoji.AddElem("bitDepth", btMapSzTable.bitDepth);
			Emoji.AddElem("flags", btMapSzTable.Flags);

			char md[MAX_PATH] = { 0 };
			char strike[MAX_PATH] = { 0 };
			sprintf_s(strike, MAX_PATH, "%sBimap_strikeindex%02d\\", pDirectory, i);
			sprintf_s(md, MAX_PATH, "md %s", strike);
			::system(md);
			szbmpSaveDir.push_back(strike);

			//IndexSubTableArray
			unsigned long ulIndexsubArryNum = btMapSzTable.numberofIndexSubTables;
			for (unsigned long y = 0; y < ulIndexsubArryNum; y++)
			{
				IndexSubTableArray& subTableArray = btMapSzTable.vtIndexSubTableArray[y];

				string strSaveDir = szbmpSaveDir[y] + "Gid" + std::to_string(subTableArray.firstGlyphIndex)
					+ "ToGid" + std::to_string(subTableArray.lastGlyphIndex)
					+ "\\";
				string strmd = "md " + strSaveDir;
				::system(strmd.c_str());

				Emoji.AddElem("indexSubTableArray");
				Emoji.AddAttrib("IndexSubFormat", subTableArray.IndxSubTable.Header.indexFormat);
				Emoji.AddAttrib("ImageFormat", subTableArray.IndxSubTable.Header.imageFormat);
				Emoji.AddAttrib("firstGlyphIndex", subTableArray.firstGlyphIndex);
				Emoji.AddAttrib("lastGlyphIndex", subTableArray.lastGlyphIndex);
				Emoji.IntoElem();

				unsigned short usoffst = 0;
				for (unsigned short x1 = subTableArray.firstGlyphIndex; x1 < subTableArray.lastGlyphIndex + 1; x1++)
				{
					char FileName[MAX_PATH] = { 0 };
					char strTmp[MAX_PATH] = { 0 }; 

					sprintf_s(FileName, MAX_PATH, "%s%04d.png", strSaveDir.c_str(), x1);
					std::vector<unsigned long> szUnicode;
					FontCodec.FindGryphUncidoByIndex(x1, szUnicode);
					if (szUnicode.size() > 0) {
						sprintf_s(strTmp, MAX_PATH, "0x%x", szUnicode[0]);
					}

					Emoji.AddElem("glyphLoc");
					Emoji.AddAttrib("id", x1);
					Emoji.AddAttrib("unicode", strTmp);
					Emoji.AddAttrib("name", FontCodec.FindNameByIndex(x1).c_str());

					Emoji.IntoElem();
					if (usoffst < subTableArray.IndxSubTable.vtEBData.size())
					{
						CBDTFormat17& format17 = subTableArray.IndxSubTable.vtEBData[usoffst++].DataFrmt17;
						Emoji.AddElem("smallGlyphMetrics");
						Emoji.IntoElem();
						Emoji.AddElem("height", format17.smallGlyphMtcs.height);
						Emoji.AddElem("width", format17.smallGlyphMtcs.width);
						Emoji.AddElem("BearingX", format17.smallGlyphMtcs.BearingX);
						Emoji.AddElem("BearingY", format17.smallGlyphMtcs.BearingY);
						Emoji.AddElem("Advance", format17.smallGlyphMtcs.Advance);
						Emoji.OutOfElem();

						FILE* PngFile = fopen(FileName, "w+b");
						if (PngFile != NULL)
						{
							size_t stdataLen = format17.data.size();
							for (size_t z = 0; z < stdataLen; z++)
							{
								fwrite(&format17.data[z], 1, 1, PngFile);
							}
							fflush(PngFile);
							fclose(PngFile);
						}
						Emoji.AddElem("Path", FileName);
					}
					Emoji.OutOfElem();
				}
				Emoji.OutOfElem();
			}
			Emoji.OutOfElem();
		}
		Emoji.OutOfElem();
	}
	else //sbix 苹果字库特有表项
	{
		char rd[MAX_PATH] = { 0 };
		sprintf_s(rd, MAX_PATH, "rd %s /s/q", pDirectory);
		::system(rd);
		char md[MAX_PATH] = { 0 };
		sprintf_s(md, MAX_PATH, "md %s", pDirectory);
		::system(md);

		CHYsbix& sbix = FontCodec.m_HYsbix;
		for (unsigned long i = 0; i < sbix.numStrikes; i++)
		{
			char strike[MAX_PATH] = { 0 };
			sprintf_s(strike, MAX_PATH, "%sstrikeindex%02d\\", pDirectory, i);
			::ZeroMemory(md, MAX_PATH);
			sprintf_s(md, MAX_PATH, "md %s", strike);
			::system(md);

			CSbixStrikes& SbixStrikes = sbix.vtSbixStrikes[i];
			size_t szDatarecord = SbixStrikes.vtStrikesDatarecord.size();
			for (unsigned long j = 0; j < szDatarecord; j++)
			{
				CStrikesDatarecord& Daterecord = SbixStrikes.vtStrikesDatarecord[j];
				std::vector<unsigned long> szUnicode;
				FontCodec.FindGryphUncidoByIndex(Daterecord.gid, szUnicode);

				char FileName[MAX_PATH] = { 0 };
				char FileExt[5] = { 0 };
				memcpy_s(FileExt, 5, Daterecord.graphicType, 4);

				if (szUnicode.size() > 0)
				{
					sprintf_s(FileName, MAX_PATH, "%s0x%x.%s", strike, szUnicode[0], FileExt);
				}
				else
				{
					sprintf_s(FileName, MAX_PATH, "%sgid%03d.%s", strike, Daterecord.gid, FileExt);
				}

				FILE* PngFile = fopen(FileName, "w+b");
				if (PngFile != NULL)
				{
					size_t stdataLen = Daterecord.vtdata.size();
					for (size_t z = 0; z < stdataLen; z++)
					{
						fwrite(&Daterecord.vtdata[z], 1, 1, PngFile);
					}
					fflush(PngFile);
					fclose(PngFile);
				}
			}
		}
	}

	Emoji.OutOfElem();

}	//end of void EmojiToXml()

void HeadToXml(CMarkup& xml, CHYFontCodec& FontCodec)
{
	xml.AddElem("Head");
	xml.IntoElem();
	xml.AddElem("version", ::HY_HYFIXED_to_float(FontCodec.m_HYhead.version));
	xml.AddElem("fontversion", ::HY_HYFIXED_to_float(FontCodec.m_HYhead.fontRevision));
	xml.AddElem("flag", FontCodec.m_HYhead.flags);
	xml.AddElem("Em", FontCodec.m_HYhead.unitsPerEm);
	xml.AddElem("xMin", FontCodec.m_HYhead.xMin);
	xml.AddElem("yMin", FontCodec.m_HYhead.yMin);
	xml.AddElem("xMax", FontCodec.m_HYhead.xMax);
	xml.AddElem("yMax", FontCodec.m_HYhead.yMax);
	xml.OutOfElem();

}	// end of void HeadToXml()

void HheaToXml(CMarkup& xml, CHYFontCodec& FontCodec)
{
	xml.AddElem("Hhea");
	xml.IntoElem();
	xml.AddElem("ascender", FontCodec.m_HYHhea.Ascender);
	xml.AddElem("descender", FontCodec.m_HYHhea.Descender);	
	xml.AddElem("lineGap", FontCodec.m_HYHhea.LineGap);
	xml.OutOfElem();

}	// end of void HheaToXml()

void maxpToXml(CMarkup& xml, CHYFontCodec& FontCodec)
{
	xml.AddElem("maxp");
	xml.IntoElem();
	xml.AddElem("numgGlyphs", FontCodec.m_HYMaxp.numGlyphs);
	xml.OutOfElem();

}	// end of void maxpToXml()

void OS2ToXml(CMarkup& xml, CHYFontCodec& FontCodec)
{
	xml.AddElem("OS2");
	xml.IntoElem();
	xml.AddElem("version", FontCodec.m_HYOS2.version);
	xml.AddElem("AvgCharWidth", FontCodec.m_HYOS2.xAvgCharWidth);
	xml.AddElem("WeightClass", FontCodec.m_HYOS2.usWeightClass);
	xml.AddElem("WidthClass", FontCodec.m_HYOS2.usWidthClass);
	xml.AddElem("fsType", FontCodec.m_HYOS2.fsType);
	xml.AddElem("SubscriptXSize", FontCodec.m_HYOS2.ySubscriptXSize);
	xml.AddElem("SubscriptYSize", FontCodec.m_HYOS2.ySubscriptYSize);
	xml.AddElem("SubscriptXOffset", FontCodec.m_HYOS2.ySubscriptXOffset);
	xml.AddElem("SubscriptYOffset", FontCodec.m_HYOS2.ySubscriptYOffset);
	xml.AddElem("SuperscriptXSize", FontCodec.m_HYOS2.ySuperscriptXSize);
	xml.AddElem("SuperscriptYSize", FontCodec.m_HYOS2.ySuperscriptYSize);
	xml.AddElem("SuperscriptXOffset", FontCodec.m_HYOS2.ySuperscriptXOffset);
	xml.AddElem("SuperscriptYOffset", FontCodec.m_HYOS2.ySuperscriptYOffset);
	xml.AddElem("StrikeoutSize", FontCodec.m_HYOS2.yStrikeoutSize);
	xml.AddElem("StrikeoutPosition", FontCodec.m_HYOS2.yStrikeoutPosition);
	xml.AddElem("FamilyClass", FontCodec.m_HYOS2.sFamilyClass);
	xml.AddElem("FamilyType", FontCodec.m_HYOS2.panose.FamilyType);
	xml.AddElem("SerifStyle", FontCodec.m_HYOS2.panose.SerifStyle);
	xml.AddElem("Weight", FontCodec.m_HYOS2.panose.Weight);
	xml.AddElem("Proportion", FontCodec.m_HYOS2.panose.Proportion);
	xml.AddElem("Contrast", FontCodec.m_HYOS2.panose.Contrast);
	xml.AddElem("StrokeVariation;", FontCodec.m_HYOS2.panose.StrokeVariation);
	xml.AddElem("ArmStyle;", FontCodec.m_HYOS2.panose.ArmStyle);
	xml.AddElem("Letterform;", FontCodec.m_HYOS2.panose.Letterform);
	xml.AddElem("Midline", FontCodec.m_HYOS2.panose.Midline);
	xml.AddElem("XHeigh", FontCodec.m_HYOS2.panose.XHeight);
	xml.AddElem("UnicodeRange1", FontCodec.m_HYOS2.ulUnicodeRange1);
	xml.AddElem("UnicodeRange2", FontCodec.m_HYOS2.ulUnicodeRange2);
	xml.AddElem("UnicodeRange3", FontCodec.m_HYOS2.ulUnicodeRange3);
	xml.AddElem("UnicodeRange4", FontCodec.m_HYOS2.ulUnicodeRange4);

	std::string VendID(FontCodec.m_HYOS2.vtachVendID.begin(), FontCodec.m_HYOS2.vtachVendID.end());
	xml.AddElem("achVendID", VendID.c_str());

	xml.AddElem("fsSelection", FontCodec.m_HYOS2.fsSelection);

	xml.AddElem("FirstCharIndex", FontCodec.m_HYOS2.usFirstCharIndex);
	xml.AddElem("LastCharIndex", FontCodec.m_HYOS2.usLastCharIndex);

	xml.AddElem("TypoAscender", FontCodec.m_HYOS2.sTypoAscender);
	xml.AddElem("TypoDescender", FontCodec.m_HYOS2.sTypoDescender);
	xml.AddElem("TypoLineGap", FontCodec.m_HYOS2.sTypoLineGap);

	xml.AddElem("WinAscent", FontCodec.m_HYOS2.usWinAscent);
	xml.AddElem("WinDescent", FontCodec.m_HYOS2.usWinDescent);
	
	xml.AddElem("CodePageRange1", FontCodec.m_HYOS2.ulCodePageRange1);
	xml.AddElem("CodePageRange2", FontCodec.m_HYOS2.ulCodePageRange2);




	



	xml.OutOfElem();

}	// end of void OS2ToXml()

int	__cdecl	EmojiToImage(char* pFontName, char* pDirectory)
{
	CHYFontCodec	FontCodec;
	int Ret = FontCodec.Decode(pFontName);
	if (Ret!=HY_NOERROR) return Ret;

	char rd[MAX_PATH]={0};
	sprintf_s(rd,MAX_PATH,"rd %s /s/q",pDirectory);
	::system(rd);
	char md[MAX_PATH]={0};
	sprintf_s(md,MAX_PATH,"md %s",pDirectory);
	::system(md);

	// Emoji xml 文件	
	CMarkup Emoji;

	Emoji.SetDoc("<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n");

	Emoji.AddElem("Head");
	Emoji.IntoElem();
	maxpToXml(Emoji, FontCodec);
	HeadToXml(Emoji,FontCodec);
	HheaToXml(Emoji,FontCodec);	
	EmojiToXml(Emoji,pDirectory, FontCodec);	
	Emoji.OutOfElem();
	// 存盘	
	std::string strXml = string(pDirectory) + "\\emojiinfo.xml";
	Emoji.Save(strXml.c_str());

	return HY_NOERROR;

}	// end of int __cdecl	EmojiToImage()

int __cdecl WoffToFONT(char* pWoffFile, char* pFontName)
{
	HYFONTCODEC::WoffCodec  wfcdc;

	return wfcdc.Woff2Sfnt(pWoffFile,pFontName);
	
}	// end of int __stdcall WoffToFONT()

int __cdecl TTF2WebFont(char* pFontName, char* pzipFile)
{
	std::string	 strDir = ::HY_GetDirFromPath(string(pzipFile));
	std::string strFileName = HY_GetFileNameFromPath(pFontName);

	std::string strWoff = strDir+ strFileName+".woff";
	HYFONTCODEC::WoffCodec				wfcdc;
	int iRt = wfcdc.Sfnt2Woff(pFontName, strWoff.c_str());
	if (iRt != HY_NOERROR) return iRt;

	std::string strEot = strDir + strFileName + ".eot";
	iRt = ::FontToEOT(pFontName,strEot.c_str());
	if (iRt != HY_NOERROR) return iRt;	

	std::vector<std::string> files;
	files.push_back(strWoff);
	files.push_back(strEot);
	files.push_back(string(pFontName));

	::HY_ZipFiles(string(pzipFile), files);

	DeleteFile(strWoff.c_str());
	DeleteFile(strEot.c_str());
	
	return HY_NOERROR;

}	// end of int __cdecl TTF2WebFont()

int __cdecl TTF2WebFont1(char* pFontName, char* pzipFile, char* strTextFile)
{	
	DeleteFile(pzipFile);

	CFontExtract	FontExtract;
	std::string		strTmpFile;
	int									irtn = 0;	

	int iFilestat = ::HY_FileExist(pFontName); 
	if (iFilestat == 0)  
		return HY_ERR_FILE_ZERO;	
	if (iFilestat == -1)  
		return HY_ERR_FILE_NOEXIST;	
	if (iFilestat == -2)  
		return HY_ERR_FILE_STAT;	
	if (iFilestat == -3)  
		return HY_ERR_FILE_OTHER;

	std::string strSubTTF = HY_GetDirFromPath(pFontName) + HY_GetFileNameFromPath(pFontName) +"Subset.ttf";
	irtn = FontExtract.GetSubset(pFontName, strSubTTF.c_str(), strTextFile);
		
	if (irtn==HY_NOERROR){
		irtn = TTF2WebFont((char*)strSubTTF.c_str(), pzipFile);
	}

	DeleteFile(strSubTTF.c_str());	
	
	return 0;

}	// end of int __cdecl TTF2WebFont1()


int	__cdecl	ExtractFONT(char* pFontName, char* pNewFontName, unsigned long* pUni, unsigned long ulUniSize,int CMAP)
{	
	HYFONTCODEC::CFontExtract	FontExtract;
	for (unsigned long i=0; i<ulUniSize; i++){
		FontExtract.m_vtUnicode.push_back(pUni[i]);
	}	

	return FontExtract.ExtractFont(pFontName,pNewFontName,CMAP);	

}	// end of int	__cdecl	ExtractFONT()

int	__cdecl	ExtractFONT1(char* pFontName, char* pNewFontName, char* pTextFile,int CMAP)
{
	HYFONTCODEC::CFontExtract	FontExtract;
	return FontExtract.ExtractFont(pFontName,pNewFontName,pTextFile,CMAP);
	
}	// end of int	__cdecl	ExtractFONT1()

int	__cdecl	FontToSVG(char* pFontName, char* pSVG)
{
	CHYFontCodec	FontCodec;
	int Ret = FontCodec.Decode(pFontName);
	if (Ret!=HY_NOERROR) return Ret;

	return FontCodec.EncodeSVG(pSVG);	

}	// end of int __cdecl	FontToSVG()

int	__cdecl	FontToEOT(const char* pFontName, const char* pEotFile)
{
	CHYFontCodec	FontCodec;
	return FontCodec.EncodeEOT(pEotFile,pFontName);

}	// end of int	__cdecl	FontToEOT()


int	__cdecl ModifyVMTX1(char* pFont, char* nwFnt, char* pVmtx)
{
	HYFONTCODEC::CFontExtract	FontExtract;
	return FontExtract.ModifyVMTX1(pFont,nwFnt,pVmtx);

}	//end of int	__cdecl ModifyVMTX()

int	__cdecl ModifyVMTX2(char* pFont, char* nwFnt, int space)
{
	HYFONTCODEC::CFontExtract	FontExtract;
	return FontExtract.ModifyVMTX2(pFont,nwFnt,space);

}	//end of int	__cdecl ModifyVMTX()

int	__cdecl ModifyVMTX3(char* pFont, char* pPSNameFile, char* nwFnt, int space)
{
	HYFONTCODEC::CFontExtract	FontExtract;
	return FontExtract.ModifyVMTX3(pFont,pPSNameFile,nwFnt,space);

}	// end of int	__cdecl ModifyVMTX3(char* pFont, char* nwFnt, int space)

int	__cdecl ChaFileToHanyiFile(char* sourcefile, char* targetfile, int iflag)
{
	HYFONTCODEC::CFontExtract	FontExtract;
	return FontExtract.ChaFileToHanyiFile(sourcefile,targetfile,iflag);

}	// end of int	__cdecl ChaFileToHanyiFile()

int	__cdecl ExtractCAMP(char* pFontName, char* sourcechar, char* targetXml)
{	
	HYFONTCODEC::CFontExtract	FontExtract;
	return FontExtract.ExtractCAMP(pFontName,sourcechar,targetXml);

}	//end of int	__cdecl ExtractCAMP()

int	__cdecl ReplaceCAMP(char* pSource, char* pTatget, char* ReplaceXml)
{	
	return -1;

}	// end of int	__cdecl ReplaceCAMP()
