// CHYEmjioDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "HYFontSmartShaper.h"
#include "CHYEmjioDlg.h"
#include "afxdialogex.h"

// CHYEmjioDlg 对话框
IMPLEMENT_DYNAMIC(CHYEmjioDlg, CDialogEx)

CHYEmjioDlg::CHYEmjioDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_EMJ_DLG, pParent)
	, m_strCnfgFile(_T(""))
	, m_strCodeFile(_T(""))
	, m_strHMtricFile(_T(""))
{

}	// end of CHYEmjioDlg::CHYEmjioDlg()

CHYEmjioDlg::~CHYEmjioDlg()
{

}	// end of CHYEmjioDlg::~CHYEmjioDlg()

void CHYEmjioDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EMJ_CNGFILE_EDIT, m_strCnfgFile);
	DDX_Text(pDX, IDC_EMJ_CDEFILE_EDIT, m_strCodeFile);
	DDX_Text(pDX, IDC_EMJ_HMFILE_EDIT, m_strHMtricFile);
	
}	// end of void CHYEmjioDlg::DoDataExchange()

BEGIN_MESSAGE_MAP(CHYEmjioDlg, CDialogEx)
	ON_BN_CLICKED(IDC_EMJ_CNGFILE_BTN, &CHYEmjioDlg::OnBnClickedEmjCngfileBtn)
	ON_BN_CLICKED(IDC_EMJ_CDEFILE_BTN, &CHYEmjioDlg::OnBnClickedEmjCdefileBtn)
	ON_BN_CLICKED(IDC_EMJ_MK_BTN, &CHYEmjioDlg::OnBnClickedEmjMkBtn)
	ON_BN_CLICKED(IDC_EMJ_HMEFILE_BTN, &CHYEmjioDlg::OnBnClickedEmjHmefileBtn)
END_MESSAGE_MAP()

void CHYEmjioDlg::XmlToHead(CMarkup& mkEmoji, CHYFontCodec& FontEnCodec)
{
	if (mkEmoji.FindElem("Head"))
	{
		FontEnCodec.m_HYhead.SetDefault();

		mkEmoji.IntoElem();
		mkEmoji.FindElem("version");
		FontEnCodec.m_HYhead.version.value = ::HY_float_to_HYFIXED(atof(mkEmoji.GetData())).value;
		FontEnCodec.m_HYhead.version.fract = ::HY_float_to_HYFIXED(atof(mkEmoji.GetData())).fract;
		mkEmoji.FindElem("fontversion");
		FontEnCodec.m_HYhead.fontRevision.value = ::HY_float_to_HYFIXED(atof(mkEmoji.GetData())).value;
		FontEnCodec.m_HYhead.fontRevision.fract = ::HY_float_to_HYFIXED(atof(mkEmoji.GetData())).fract;
		mkEmoji.FindElem("flag");
		FontEnCodec.m_HYhead.flags = ::HY_float_to_HYFIXED(atof(mkEmoji.GetData())).value;
		mkEmoji.FindElem("Em");
		FontEnCodec.m_HYhead.unitsPerEm = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("xMin");
		FontEnCodec.m_HYhead.xMin = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("yMin");
		FontEnCodec.m_HYhead.yMin = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("xMax");
		FontEnCodec.m_HYhead.xMax = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("yMax");
		FontEnCodec.m_HYhead.yMax = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("macStyle");
		FontEnCodec.m_HYhead.macStyle = atoi(mkEmoji.GetData());
		mkEmoji.OutOfElem();

		FontEnCodec.m_HYhead.magicNumber = 0x5F0F3CF5;

		__time64_t	 tspace = 2082844799;
		CTime tCurTime = CTime::GetCurrentTime();
		__time64_t  t64CurTime = tCurTime.GetTime();
		t64CurTime += tspace;
		char p[8] = { 0 };
		CopyMemory((PBYTE)p, &t64CurTime, sizeof(__time64_t));
		for (int i = 0; i < 8; i++) {
			FontEnCodec.m_HYhead.created[i] = p[7 - i];
			FontEnCodec.m_HYhead.modified[i] = p[7 - i];
		}
	}

}	// end of void CHYEmjioDlg::XmlToHead()

void CHYEmjioDlg::XmlToHhea(CMarkup& mkEmoji, CHYFontCodec& FntEncode)
{
	if (mkEmoji.FindElem("Hhea"))
	{
		FntEncode.m_HYHhea.SetDefault();

		mkEmoji.IntoElem();
		mkEmoji.FindElem("ascender");
		FntEncode.m_HYHhea.Ascender = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("descender");
		FntEncode.m_HYHhea.Descender = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("lineGap");
		FntEncode.m_HYHhea.LineGap = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("minLeftSideBearing");
		FntEncode.m_HYHhea.minLeftSideBearing = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("minRightSideBearing");
		FntEncode.m_HYHhea.minRightSideBearing = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("advanceWidthMax");
		FntEncode.m_HYHhea.advanceWidthMax = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("xMaxExtent");
		FntEncode.m_HYHhea.xMaxExtent = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("caretSlopeRise");
		FntEncode.m_HYHhea.caretSlopeRise = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("caretSlopeRun");
		FntEncode.m_HYHhea.caretSlopeRun = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("caretOffset");
		FntEncode.m_HYHhea.caretOffset = atoi(mkEmoji.GetData());
		mkEmoji.OutOfElem();

		FntEncode.m_HYHhea.version.value = 1;
		FntEncode.m_HYHhea.version.fract = 0;
		FntEncode.m_HYHhea.reserved1 = 0;
		FntEncode.m_HYHhea.reserved2 = 0;
		FntEncode.m_HYHhea.reserved3 = 0;
		FntEncode.m_HYHhea.reserved4 = 0;
		FntEncode.m_HYHhea.metricDataFormat = 0;
		FntEncode.m_HYHhea.numberOfHMetrics = 1;

	}

}	// void CHYEmjioDlg::XmlToHhea()

void CHYEmjioDlg::XmlTomaxp(CMarkup& mkEmoji, CHYFontCodec& FontEnCodec)
{
	if (mkEmoji.FindElem("maxp"))
	{
		FontEnCodec.m_vtHYGlyphs.clear();
		FontEnCodec.m_HYMaxp.SetDefault();

		mkEmoji.IntoElem();
		mkEmoji.FindElem("numgGlyphs");
		FontEnCodec.m_HYMaxp.numGlyphs = atoi(mkEmoji.GetData());
		FontEnCodec.m_vtHYGlyphs.resize(FontEnCodec.m_HYMaxp.numGlyphs);		
		mkEmoji.FindElem("maxPoints");
		FontEnCodec.m_HYMaxp.maxPoints = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("maxContours");
		FontEnCodec.m_HYMaxp.maxContours = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("maxCompositePoints");
		FontEnCodec.m_HYMaxp.maxCompositePoints = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("maxCompositeContours");
		FontEnCodec.m_HYMaxp.maxCompositeContours = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("maxZones");
		FontEnCodec.m_HYMaxp.maxZones = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("maxTwilightPoints");
		FontEnCodec.m_HYMaxp.maxTwilightPoints = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("maxStorage");
		FontEnCodec.m_HYMaxp.maxStorage = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("maxFunctionDefs");
		FontEnCodec.m_HYMaxp.maxFunctionDefs = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("maxInstructionDefs");
		FontEnCodec.m_HYMaxp.maxInstructionDefs = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("maxStackElements");
		FontEnCodec.m_HYMaxp.maxStackElements = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("maxSizeOfInstructions");
		FontEnCodec.m_HYMaxp.maxSizeOfInstructions = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("maxComponentElements");
		FontEnCodec.m_HYMaxp.maxComponentElements = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("maxComponentDepth");
		FontEnCodec.m_HYMaxp.maxComponentDepth = atoi(mkEmoji.GetData());		
		mkEmoji.OutOfElem();
	}

	FontEnCodec.m_HYMaxp.version.value = 1;
	FontEnCodec.m_HYMaxp.version.fract = 0;

}	// end of void CHYEmjioDlg::XmlTomaxp()

void CHYEmjioDlg::XmlToOS(CMarkup& mkEmoji, CHYFontCodec& FontEnCodec)
{
	if (mkEmoji.FindElem("OS2"))
	{
		FontEnCodec.m_HYOS2.SetDefault();

		mkEmoji.IntoElem();
		mkEmoji.FindElem("version");
		FontEnCodec.m_HYOS2.version = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("AvgCharWidth");
		FontEnCodec.m_HYOS2.xAvgCharWidth = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("WeightClass");
		FontEnCodec.m_HYOS2.usWeightClass = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("WidthClass");
		FontEnCodec.m_HYOS2.usWidthClass = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("fsType");		
		FontEnCodec.m_HYOS2.fsType = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("SubscriptXSize");
		FontEnCodec.m_HYOS2.ySubscriptXSize = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("SubscriptYSize");
		FontEnCodec.m_HYOS2.ySubscriptYSize = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("SubscriptXOffset");
		FontEnCodec.m_HYOS2.ySubscriptXOffset = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("SubscriptYOffset");
		FontEnCodec.m_HYOS2.ySubscriptXOffset = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("SuperscriptXSize");
		FontEnCodec.m_HYOS2.ySuperscriptXSize = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("SuperscriptYSize");
		FontEnCodec.m_HYOS2.ySuperscriptYSize = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("SuperscriptXOffset");
		FontEnCodec.m_HYOS2.ySuperscriptXOffset = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("SuperscriptYOffset");
		FontEnCodec.m_HYOS2.ySuperscriptYOffset = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("StrikeoutSize");
		FontEnCodec.m_HYOS2.yStrikeoutSize = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("StrikeoutPosition");
		FontEnCodec.m_HYOS2.yStrikeoutPosition = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("sFamilyClass");
		FontEnCodec.m_HYOS2.sFamilyClass = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("UnicodeRange1");
		FontEnCodec.m_HYOS2.ulUnicodeRange1 = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("UnicodeRange2");
		FontEnCodec.m_HYOS2.ulUnicodeRange2 = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("UnicodeRange3");
		FontEnCodec.m_HYOS2.ulUnicodeRange3 = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("UnicodeRange4");
		FontEnCodec.m_HYOS2.ulUnicodeRange4 = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("fsSelection");
		FontEnCodec.m_HYOS2.fsSelection = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("FirstCharIndex");
		FontEnCodec.m_HYOS2.usFirstCharIndex = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("LastCharIndex");
		FontEnCodec.m_HYOS2.usLastCharIndex = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("TypoAscender");
		FontEnCodec.m_HYOS2.sTypoAscender = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("TypoDescender");
		FontEnCodec.m_HYOS2.sTypoDescender = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("TypoLineGap");
		FontEnCodec.m_HYOS2.sTypoLineGap = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("WinAscent");
		FontEnCodec.m_HYOS2.usWinAscent = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("WinDescent");
		FontEnCodec.m_HYOS2.usWinDescent = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("CodePageRange1");
		FontEnCodec.m_HYOS2.ulCodePageRange1 = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("CodePageRange2");
		FontEnCodec.m_HYOS2.ulCodePageRange2 = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("Height");
		FontEnCodec.m_HYOS2.sxHeight = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("CapHeight");
		FontEnCodec.m_HYOS2.sCapHeight = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("usDefaultChar");
		FontEnCodec.m_HYOS2.usDefaultChar = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("usBreakChar");
		FontEnCodec.m_HYOS2.usBreakChar = atoi(mkEmoji.GetData());
		mkEmoji.FindElem("usMaxContext");
		FontEnCodec.m_HYOS2.usMaxContext = atoi(mkEmoji.GetData());

		FontEnCodec.m_HYOS2.vtachVendID.push_back('H');
		FontEnCodec.m_HYOS2.vtachVendID.push_back('N');
		FontEnCodec.m_HYOS2.vtachVendID.push_back('Y');
		FontEnCodec.m_HYOS2.vtachVendID.push_back('I');

		mkEmoji.OutOfElem();
	}

}	// end of void CHYEmjioDlg::XmlToOS()

void CHYEmjioDlg::XmlToNoImageChar(CMarkup& mkEmoji, CHYFontCodec& FontEnCodec)
{
	int iGid = 0;
	std::vector<CHYGlyph>& vtHYGlyphs =  FontEnCodec.m_vtHYGlyphs;
	if (mkEmoji.FindElem("NoImgChar"))
	{
		mkEmoji.IntoElem();
		while (mkEmoji.FindElem("Char"))
		{
			mkEmoji.IntoElem();

			mkEmoji.FindElem("GID");
			iGid = atoi(mkEmoji.GetData());			
			mkEmoji.FindElem("Name");
			vtHYGlyphs[iGid].strPostName = mkEmoji.GetData();
			mkEmoji.FindElem("unicode");
			if (!mkEmoji.GetData().IsEmpty())
			{				
				vtHYGlyphs[iGid].vtUnicode.push_back(strtol(mkEmoji.GetData(), NULL, 16));
			}
			
			mkEmoji.FindElem("minLeftSideBearing");
			vtHYGlyphs[iGid].minX = atoi(mkEmoji.GetData());
			mkEmoji.FindElem("advanceWidth");
			vtHYGlyphs[iGid].advanceWidth = atoi(mkEmoji.GetData());

			mkEmoji.OutOfElem();
		}		
		mkEmoji.OutOfElem();
	}

}	// end of void CHYEmjioDlg::XmlToNoImageChar()

void CHYEmjioDlg::XmlToEmoji(CMarkup& mkEmoji, CHYFontCodec& FntEncode)
{
	std::vector<CHYGlyph>& vtGlyphs = FntEncode.m_vtHYGlyphs;

	//CHYCodeMap& HYCodeMap = FntEncode.m_HYCodeMap;
	CHYCBLC& CBLC = FntEncode.m_HYCblc;
	CHYCBDT& CBDT = FntEncode.m_HYCBDT;

	CBLC.SetDefault();
	CBDT.SetDefault();

	CBLC.Header.version.value = 3;
	CBLC.Header.version.fract = 0;

	if (mkEmoji.FindElem("Emoji"))
	{
		mkEmoji.IntoElem();
		if (mkEmoji.FindElem("OpenType"))
		{
			mkEmoji.IntoElem();
			while (mkEmoji.FindElem("bitmapSizeTable"))
			{
				mkEmoji.IntoElem();
				BitmapSize bmpSz;

				if (mkEmoji.FindElem("sbitLineMetrics_hori"))
				{
					mkEmoji.IntoElem();

					mkEmoji.FindElem("ascender");
					bmpSz.Hori.ascender = (char)atoi(mkEmoji.GetData());
					mkEmoji.FindElem("descender");
					bmpSz.Hori.descender = (char)atoi(mkEmoji.GetData());
					mkEmoji.FindElem("widthMax");
					bmpSz.Hori.widthMax = (unsigned char)atoi(mkEmoji.GetData());
					mkEmoji.FindElem("caretSlopeNumerator");
					bmpSz.Hori.caretSlopeNumerator = (char)atoi(mkEmoji.GetData());
					mkEmoji.FindElem("caretSlopeDenominator");
					bmpSz.Hori.caretSlopeDenominator = (char)atoi(mkEmoji.GetData());
					mkEmoji.FindElem("caretOffset");
					bmpSz.Hori.caretOffset = (char)atoi(mkEmoji.GetData());
					mkEmoji.FindElem("minOriginSB");
					bmpSz.Hori.minOriginSB = (char)atoi(mkEmoji.GetData());
					mkEmoji.FindElem("minAdvanceSB");
					bmpSz.Hori.minAdvanceSB = (char)atoi(mkEmoji.GetData());
					mkEmoji.FindElem("maxBeforeBL");
					bmpSz.Hori.maxBeforeBL = (char)atoi(mkEmoji.GetData());
					mkEmoji.FindElem("minAfterBL");
					bmpSz.Hori.minAfterBL = (char)atoi(mkEmoji.GetData());
					mkEmoji.FindElem("pad1");
					bmpSz.Hori.pad1 = (char)atoi(mkEmoji.GetData());
					mkEmoji.FindElem("pad2");
					bmpSz.Hori.pad2 = (char)atoi(mkEmoji.GetData());
					mkEmoji.OutOfElem();
				}

				if (mkEmoji.FindElem("sbitLineMetrics_vert"))
				{
					mkEmoji.IntoElem();
					mkEmoji.FindElem("ascender");
					bmpSz.Vert.ascender = (char)atoi(mkEmoji.GetData());
					mkEmoji.FindElem("descender");
					bmpSz.Vert.descender = (char)atoi(mkEmoji.GetData());
					mkEmoji.FindElem("widthMax");
					bmpSz.Vert.widthMax = (unsigned char)atoi(mkEmoji.GetData());
					mkEmoji.FindElem("caretSlopeNumerator");
					bmpSz.Vert.caretSlopeNumerator = (char)atoi(mkEmoji.GetData());
					mkEmoji.FindElem("caretSlopeDenominator");
					bmpSz.Vert.caretSlopeDenominator = (char)atoi(mkEmoji.GetData());
					mkEmoji.FindElem("caretOffset");
					bmpSz.Vert.caretOffset = (char)atoi(mkEmoji.GetData());
					mkEmoji.FindElem("minOriginSB");
					bmpSz.Vert.minOriginSB = (char)atoi(mkEmoji.GetData());
					mkEmoji.FindElem("minAdvanceSB");
					bmpSz.Vert.minAdvanceSB = (char)atoi(mkEmoji.GetData());
					mkEmoji.FindElem("maxBeforeBL");
					bmpSz.Vert.maxBeforeBL = (char)atoi(mkEmoji.GetData());
					mkEmoji.FindElem("minAfterBL");
					bmpSz.Vert.minAfterBL = (char)atoi(mkEmoji.GetData());
					mkEmoji.FindElem("pad1");
					bmpSz.Vert.pad1 = (char)atoi(mkEmoji.GetData());
					mkEmoji.FindElem("pad2");
					bmpSz.Vert.pad2 = (char)atoi(mkEmoji.GetData());
					mkEmoji.OutOfElem();
				}

				if (mkEmoji.FindElem("startGlyphIndex"))
				{
					bmpSz.startGlyphIndex = (unsigned short)atoi(mkEmoji.GetData());
				}

				if (mkEmoji.FindElem("endGlyphIndex"))
				{
					bmpSz.endGlyphIndex = (unsigned short)atoi(mkEmoji.GetData());
				}

				if (mkEmoji.FindElem("ppemX"))
				{
					bmpSz.ppemX = (unsigned char)atoi(mkEmoji.GetData());
				}

				if (mkEmoji.FindElem("ppemY"))
				{
					bmpSz.ppemY = (unsigned char)atoi(mkEmoji.GetData());
				}

				if (mkEmoji.FindElem("bitDepth"))
				{
					bmpSz.bitDepth = (unsigned char)atoi(mkEmoji.GetData());
				}

				if (mkEmoji.FindElem("flags"))
				{
					bmpSz.Flags = (unsigned char)atoi(mkEmoji.GetData());
				}

				if (mkEmoji.FindElem("indexSubTableArray"))
				{
					IndexSubTableArray	subTableArray;
					subTableArray.firstGlyphIndex = (unsigned short)atoi(mkEmoji.GetAttrib("firstGlyphIndex"));
					subTableArray.lastGlyphIndex = (unsigned short)atoi(mkEmoji.GetAttrib("lastGlyphIndex"));
					subTableArray.IndxSubTable.Header.indexFormat = (unsigned short)atoi(mkEmoji.GetAttrib("IndexSubFormat"));
					subTableArray.IndxSubTable.Header.imageFormat = (unsigned short)atoi(mkEmoji.GetAttrib("ImageFormat"));

					mkEmoji.IntoElem();
					while (mkEmoji.FindElem("glyphLoc"))
					{
						CEBData	imgData;
						imgData.GID = atoi(mkEmoji.GetAttrib("id"));

						if (!mkEmoji.GetAttrib("unicode").IsEmpty()) {
							imgData.unicode = strtol(mkEmoji.GetAttrib("unicode"), NULL, 16);
							vtGlyphs[imgData.GID].vtUnicode.push_back(imgData.unicode);
						}
						vtGlyphs[imgData.GID].minX = 0;
						vtGlyphs[imgData.GID].advanceWidth = FntEncode.m_HYHhea.advanceWidthMax;
						vtGlyphs[imgData.GID].strPostName = mkEmoji.GetAttrib("name");

						mkEmoji.IntoElem();
						if (subTableArray.IndxSubTable.Header.imageFormat == 17)
						{
							imgData.DataFromat = 17;
							CBDTFormat17& Data17 = imgData.DataFrmt17;
							if (mkEmoji.FindElem("smallGlyphMetrics"))
							{
								mkEmoji.IntoElem();
								mkEmoji.FindElem("height");
								Data17.smallGlyphMtcs.height = (char)atoi(mkEmoji.GetData());
								mkEmoji.FindElem("width");
								Data17.smallGlyphMtcs.width = (char)atoi(mkEmoji.GetData());
								mkEmoji.FindElem("BearingX");
								Data17.smallGlyphMtcs.BearingX = (char)atoi(mkEmoji.GetData());
								mkEmoji.FindElem("BearingY");
								Data17.smallGlyphMtcs.BearingY = (char)atoi(mkEmoji.GetData());
								mkEmoji.FindElem("Advance");
								Data17.smallGlyphMtcs.Advance = (char)atoi(mkEmoji.GetData());
								mkEmoji.OutOfElem();
							}

							if (mkEmoji.FindElem("Path"))
							{
								CString str = mkEmoji.GetData();
								FILE* PngFile = fopen(str, "rb");
								if (PngFile != NULL)
								{
									unsigned long fLength = HY_FileLength(mkEmoji.GetData());
									fseek(PngFile, 0, SEEK_SET);

									unsigned char uc;
									Data17.dataLen = fLength;
									for (unsigned long z1 = 0; z1 < fLength; z1++)
									{
										fread(&uc, 1, 1, PngFile);
										Data17.data.push_back(uc);
									}
									fclose(PngFile);
								}
							}
						}
						mkEmoji.OutOfElem();

						subTableArray.IndxSubTable.vtEBData.push_back(imgData);

					}
					bmpSz.vtIndexSubTableArray.push_back(subTableArray);
					mkEmoji.OutOfElem();

				}	// end of indexSubTableArray

				mkEmoji.OutOfElem();
				bmpSz.numberofIndexSubTables = bmpSz.vtIndexSubTableArray.size();
				CBLC.vtBitmapSizeTb.push_back(bmpSz);
			}
			mkEmoji.OutOfElem();
			CBLC.Header.numsizes = CBLC.vtBitmapSizeTb.size();
		}
		mkEmoji.OutOfElem();
		//HYCodeMap.QuickSortbyUnicode();
	}

}	// end of void CHYEmjioDlg::XmlToEmoji()

void CHYEmjioDlg::SetTables(CHYFontCodec& FntEncode)
{
	CMAP_TABLE_ENTRY	entry;
	entry.plat_ID = 3;
	entry.plat_encod_ID = 10;
	entry.format = CMAP_ENCODE_FT_12;
	FntEncode.m_HYCmap.vtCamp_tb_entry.push_back(entry);
	FntEncode.m_HYCmap.version = 0;
	FntEncode.m_HYCmap.numSubTable = (unsigned short)FntEncode.m_HYCmap.vtCamp_tb_entry.size();	

	FntEncode.m_HYHhea.numberOfHMetrics = 0;
	int szGlyphNum = FntEncode.m_HYMaxp.numGlyphs;
	if (szGlyphNum > 0) {
		int iBaseadvanceWidth = FntEncode.m_vtHYGlyphs[--szGlyphNum].advanceWidth;
		while (--szGlyphNum >= 0) {
			if (FntEncode.m_vtHYGlyphs[szGlyphNum].advanceWidth == iBaseadvanceWidth)
				FntEncode.m_HYHhea.numberOfHMetrics++;
			else
				break;
		}
		FntEncode.m_HYHhea.numberOfHMetrics = (unsigned short)FntEncode.m_HYMaxp.numGlyphs - FntEncode.m_HYHhea.numberOfHMetrics;
	}
	//HMTX
	FntEncode.m_HYHmtx.SetDefault();
	for (size_t i = 0; i < FntEncode.m_HYMaxp.numGlyphs; i++)
	{
		HMTX_LONGHORMERTRIC  h_Mertric;
		h_Mertric.advanceWidth = FntEncode.m_vtHYGlyphs[i].advanceWidth;
		h_Mertric.lsb = FntEncode.m_vtHYGlyphs[i].minX;
		FntEncode.m_HYHmtx.vtLonghormetric.push_back(h_Mertric);
	}

	FntEncode.m_tagOption.bCmplVert = FALSE;
	FntEncode.m_tagOption.bKangXi = FALSE;
	FntEncode.m_tagOption.bHanyi = FALSE;
	FntEncode.m_tagOption.bCmplCMAP = TRUE;
	FntEncode.m_tagOption.bOldStandard = FALSE;
	FntEncode.m_tagOption.bRePsName = FALSE;
	FntEncode.m_tagOption.bReSortUni = FALSE;
	FntEncode.m_tagOption.bsetADW = FALSE;
	FntEncode.m_tagOption.bsetADH = FALSE;
	FntEncode.m_tagOption.bYitizi = FALSE;

	for (size_t i = 0; i < FntEncode.m_HYMaxp.numGlyphs; i++) {
		const CHYGlyph& glyph = FntEncode.m_vtHYGlyphs[i];
		FntEncode.m_HYPost.InsertName(glyph.strPostName);
	}

	FntEncode.MakeHYCodeMap();

}	// end of void CHYEmjioDlg::SetTables()

void CHYEmjioDlg::loadCodeFile(CString strFile, CHYFontCodec& Encode)
{
	unsigned long			dwOffset = 0;
	unsigned long size = ::HY_FileExist(strFile);
	if (size == 0)								return;

	Encode.m_HYCodeMap.vtHYCodeMap.clear();
	ifstream in(strFile);
	string line;
	if (in) {
		while (getline(in, line)) { // line中不包括每行的换行符	
			std::vector<string> split;
			::HY_SpliteString(line.c_str(), ";", split);

			CHYCodeMapItem Itm;
			Itm.iGlyphIndex = atoi(split[0].c_str());
			Itm.ulGlyphNo = strtol(split[1].c_str(), NULL, 16);
			Encode.m_HYCodeMap.vtHYCodeMap.push_back(Itm);
		}

		Encode.m_HYCodeMap.QuickSortbyUnicode();
	}

}	// end of void CHYEmjioDlg::loadCodeFile()

void CHYEmjioDlg::loadHMFile(CString strFile, CHYFontCodec& Encode)
{
	unsigned long			dwOffset = 0;
	unsigned long size = ::HY_FileExist(strFile);
	if (size == 0)								return;

	Encode.m_HYHmtx.vtLonghormetric.clear();
	ifstream in(strFile);
	string line;
	if (in) {
		while (getline(in, line)) { // line中不包括每行的换行符	
			std::vector<string> split;
			::HY_SpliteString(line.c_str(), ";", split);

			HMTX_LONGHORMERTRIC hmMtric;
			hmMtric.lsb = atoi(split[0].c_str());
			hmMtric.advanceWidth = atoi(split[1].c_str());			
			Encode.m_HYHmtx.vtLonghormetric.push_back(hmMtric);
		}
	}

}	// end of void CHYEmjioDlg::loadHMFile()

// CHYEmjioDlg 消息处理程序
BOOL CHYEmjioDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE

}	// end of BOOL CHYEmjioDlg::OnInitDialog()

void CHYEmjioDlg::OnBnClickedEmjCngfileBtn()
{
	TCHAR	szFilters[] = _T("XML 文件(*.xml)|*.xml||");
	CFileDialog  openFileDlg(TRUE, _T(""), _T(""), OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilters);
	if (openFileDlg.DoModal() != IDOK)	return;
	m_strCnfgFile = openFileDlg.GetPathName();

	CMarkup			mkEmoji;
	if (!mkEmoji.Load(m_strCnfgFile)) return;
	mkEmoji.ResetMainPos();
	if (mkEmoji.FindElem("Font"))
	{
		mkEmoji.IntoElem();
		XmlTomaxp(mkEmoji, m_Encode);		
		XmlToHead(mkEmoji, m_Encode);
		XmlToHhea(mkEmoji, m_Encode);
		XmlToOS(mkEmoji, m_Encode);
		XmlToNoImageChar(mkEmoji, m_Encode);
		XmlToEmoji(mkEmoji, m_Encode);
		mkEmoji.OutOfElem();
	}

	UpdateData(FALSE);

}	// end of void CHYEmjioDlg::OnBnClickedEmjCngfileBtn()

void CHYEmjioDlg::OnBnClickedEmjCdefileBtn()
{
	TCHAR	szFilters[] = _T("Txt 文件(*.txt)|*.txt||");
	CFileDialog  openFileDlg(TRUE, _T(""), _T(""), OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilters);
	if (openFileDlg.DoModal() != IDOK)	return;
	m_strCodeFile = openFileDlg.GetPathName();
	
	loadCodeFile(m_strCodeFile, m_Encode);

	UpdateData(FALSE);

}	//end of void CHYEmjioDlg::OnBnClickedEmjCdefileBtn()

void CHYEmjioDlg::OnBnClickedEmjHmefileBtn()
{
	TCHAR	szFilters[] = _T("Txt 文件(*.txt)|*.txt||");
	CFileDialog  openFileDlg(TRUE, _T(""), _T(""), OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilters);
	if (openFileDlg.DoModal() != IDOK)	return;
	m_strHMtricFile = openFileDlg.GetPathName();

	loadHMFile(m_strHMtricFile, m_Encode);

	UpdateData(FALSE);	

}	// end of void CHYEmjioDlg::OnBnClickedEmjHmefileBtn()

void CHYEmjioDlg::OnBnClickedEmjMkBtn()
{
	/*
	if (m_strCnfgFile == "")	{
		AfxMessageBox("emjio配置文件不能为空");
		return;
	}

	if (m_strCodeFile == "") {
		AfxMessageBox("编码映射文件不能为空");
		return;
	}
	*/

	::XSysproxy().MakeFontName(m_Encode);
	SetTables(m_Encode);

	std::vector<unsigned long> ulTableFlag;
	ulTableFlag.push_back(CMAP_TAG);
	ulTableFlag.push_back(HEAD_TAG);
	ulTableFlag.push_back(HHEA_TAG);
	ulTableFlag.push_back(HMTX_TAG);
	ulTableFlag.push_back(MAXP_TAG);
	ulTableFlag.push_back(NAME_TAG);
	ulTableFlag.push_back(OS2_TAG);
	ulTableFlag.push_back(POST_TAG);
	ulTableFlag.push_back(CBLC_TAG);
	ulTableFlag.push_back(CBDT_TAG);
	
	std::string strEmoji = ::HY_GetDirFromPath(string(m_strCnfgFile)) + HY_GetFileNameFromPath(string(m_strCnfgFile))
		+ ".ttf";

	::DeleteFile(strEmoji.c_str());

	if (m_Encode.Encode((char*)strEmoji.c_str(), ulTableFlag, ::XSysproxy().m_tagOpeionPrm) == HY_NOERROR)
		AfxMessageBox(_T("字库生成完成"));
	else
		AfxMessageBox(_T("字库生成失败"));

}	// end of void CHYEmjioDlg::OnBnClickedEmjMkBtn()



