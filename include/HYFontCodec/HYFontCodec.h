#pragma once

#include <map>

#include "HYFontCodecDef.h"
#include "HYTableDirectory.h"
#include "HYCmap.h"
#include "HYCvt.h"
#include "HYFpgm.h"
#include "HYGlyf.h"
#include "HYHead.h"
#include "HYHhea.h"
#include "HYHmtx.h"
#include "HYLoca.h"
#include "HYMaxp.h"
#include "HYName.h"
#include "HYOS.h"
#include "HYPost.h"
#include "HYPrep.h"
#include "HYVhea.h"
#include "HYVmtx.h"
#include "HYCFFInfo.h"
#include "HYCodeMap.h"
#include "HYGsub.h"
#include "HYCBLC.h"
#include "HYsbix.h"
#include "HYDSIG.h"
#include "HYfvar.h"
#include "HYgvar.h"
#include "HYVORG.h"
#include "HYSTAT.h"
#include "HYGasp.h"
#include "HYCOLR.h"
#include "HYavar.h"
#include "HYHVAR.h"
#include "HYVVAR.h"
#include "HYSVG.h"
#include "Markup.h"

namespace HYFONTCODEC
{	
	class HY_DLL_CLASS TableData
	{
	public:
		TableData(){	
			iTableFlg = 0;
			ulDataSize = 0;
		}
		~TableData(){
			setDefault();
		}
		void setDefault(){
			vtData.clear();
			iTableFlg = 0;
			ulDataSize = 0;
		}
		unsigned long		ulDataSize;
		std::vector<char>	vtData;
		int					iTableFlg;

	};	//end of class HY_DLL_CLASS TableData
	
	class HY_DLL_CLASS TableDataS
	{
	public:
		TableDataS(){}
		~TableDataS()
		{
			setDefault();
		}

		void setDefault()
		{
			for (size_t i = 0; i < vtTableData.size(); i++){
				vtTableData[i].setDefault();
			}
			vtTableData.clear();
		}

		TableData* FindTableData(int iTableFlg)
		{
			for (size_t i = 0; i < vtTableData.size(); i++) {
				if (vtTableData[i].iTableFlg == iTableFlg) {
					return &vtTableData[i];
				}
			}
			return NULL;
		}

		void RemoveTableData(int iTableFlg)
		{
			for (size_t i = 0; i < vtTableData.size(); i++) {
				if (vtTableData[i].iTableFlg == iTableFlg) {
					vtTableData.erase(vtTableData.begin()+i);
					return;
				}
			}
		}
	public:
		std::vector<TableData> vtTableData;

	};	// end of class HY_DLL_CLASS TableDataS

	#define WARNLEVEL_NORMAL 0
	#define WARNLEVEL_WARN 1
	#define WARNLEVEL_ERR 2
	class HY_DLL_CLASS CVerifyInformation
	{
		public:
			CVerifyInformation()
			{
				iWarnLevel = WARNLEVEL_NORMAL;
				strInf = "";
			}	// end of CVerifyInformation()

			~CVerifyInformation(){}
		public:
			std::string		strInf;
			int				iWarnLevel;	// 0,正常 1，警告 2，错误

	};	// end of class HY_DLL_CLASS CVerifyInformation

	class HY_DLL_CLASS CFontVerifyResult
	{
		public:
			CFontVerifyResult()
			{
				strFontFile = "";
			}	// end of CFontVerifyResult()

			~CFontVerifyResult(){}
		public:
			//是否有警告信息	0,没有 1，警告数量
			inline int		IsWarn()
			{
				int iWarnNum = 0;
				size_t st = vtVerifyInf.size();
				for (size_t i = 0; i != st; i++){
					CVerifyInformation&  inf = vtVerifyInf[i];
					if (inf.iWarnLevel == WARNLEVEL_WARN){ 
						iWarnNum++;
					}
				}
				return iWarnNum;
			}	// end of inline int IsWarn()

			//是否有错误信息	0,没有 1，错误数量
			inline int		IsError()
			{
				int iErrorNum = 0;
				size_t st = vtVerifyInf.size();
				for (size_t i = 0; i != st; i++){
					CVerifyInformation&  inf = vtVerifyInf[i];
					if (inf.iWarnLevel == WARNLEVEL_ERR){
						iErrorNum++;
					}
				}
				return iErrorNum;
			}	//endo of inline int		IsWarn()

		public:
			std::string						strFontFile;
			std::vector<CVerifyInformation> vtVerifyInf;

	};	// end of class HY_DLL_CLASS CFontVerifyResult

	class HY_DLL_CLASS CHYFontCodec
	{
	public:
		CHYFontCodec(void);
		~CHYFontCodec(void);	

		struct GroupRecord
		{
			unsigned long		startCode;
			unsigned long		endCode;
			int 				startGID;
			bool				contiguous;
		};	

		// 兼容康熙部首
		void	InitKangxi();
		// 中日韩兼容表意文字
		void	InitVariantChar();
		//兼容unicode新旧标准
		void	InitCompatChar();
		//兼容JF转换中JF字库缺失的繁体字符（将简体字形赋入繁体和简体编码）
		void	InitJFChar();
		
		
		void	SetDefault();

		int		OpenFile(const char* pFontFile);
		int		CreateFile(const char* pFontFile);
		void	CloseFile();
		int		CollocateGlyf(int iFontType);

		int		Decode(const char* pFontFile);
		void	Dump(char* pFontFile, char* pDumpFile, std::vector<unsigned long> vtTBFlag);
		int		VerifyFont(char* pFontFile, CFontVerifyResult& vtResult);

		int		Encode(char* pFontFile, std::vector<unsigned long> vtFlag );
		int		Encode(char* pFontFile, std::vector<unsigned long> vtFlag, HY_OPTION_PRM& tagOption);

		void	RestPsName();
		void	AddControlChar();

		int		EncodeSVG(char* pSVGFile);
		int		EncodeEOT(const char* pEotFile, const char* pFontFile);
		int		EncodeEmojiOpentypeFont(char* pFontFile, char* EmojiProfile);
		int		EncodeEmojiOAppleFont(char* pFontFile, char* EmojiProfile);

		int		MakeTableDirectory(std::vector<unsigned long> vtFlag);
		int		MakeHYCodeMap(std::vector<CHYGlyph>& vtHYGlyphs);

		void	MixKangxiChars();
		void	MixVariantcharacters();
		void	MixCompatcharacters();
		void	MixJFCharacters();

		void	SetCmapEntry(std::vector<CMAP_TABLE_ENTRY>&	vtEntry);
		void	MakeCmap();		
		int		MakeBitmapGlyphs(char* pProfile);
		void	MakeVheaVmtx();
		void	MakeVerticalMetrics();
		void	CountVerticalMetrics();
		int	    SetCheckSumAdjustment(const char* pFontFile);

	public:
		int						DecodeTableDirectory();
		int						EncodeTableDirectory();
		CHYTableEntry*			FindTableEntry(unsigned long ulTableFlg);
		void					GetTableFlag(std::vector<unsigned long>& vtFlag);

		//Required Tables
		int		Decodecmap();
		void	Dumpcmap();
		int		Encodecmap(FILE* pFontFile, CHYTableDirectory& HYTbDirectory);
		int		DecodeCmapFmt0(FILE* pFontFile,CMAP_TABLE_ENTRY& entry);
		void	DumpcmapFmt0();
		int		DecodeCmapFmt2(FILE* pFontFile,CMAP_TABLE_ENTRY& entry);
		void	DumpcmapFmt2();
		int		DecodeCmapFmt4(FILE* pFontFile,CMAP_TABLE_ENTRY& entry);
		void	DumpcmapFmt4();
		int		DecodeCmapFmt6(FILE* pFontFile,CMAP_TABLE_ENTRY& entry);
		void	DumpcmapFmt6();
		int		DecodeCmapFmt8(FILE* pFontFile,CMAP_TABLE_ENTRY& entry);
		void	DumpcmapFmt8();
		int		DecodeCmapFmt10(FILE* pFontFile,CMAP_TABLE_ENTRY& entry);
		void	DumpcmapFmt10();
		int		DecodeCmapFmt12(FILE* pFontFile,CMAP_TABLE_ENTRY& entry);
		void	DumpcmapFmt12();
		int		DecodeCmapFmt13(FILE* pFontFile,CMAP_TABLE_ENTRY& entry);
		void	DumpcmapFmt13();
		int		DecodeCmapFmt14(FILE* pFontFile,CMAP_TABLE_ENTRY& entry);
		void	DumpcmapFmt14();

		int		EncodeCmapFmt0(FILE* pFontFile,CMAP_TABLE_ENTRY& entry);	
		int		EncodeCmapFmt2(FILE* pFontFile,CMAP_TABLE_ENTRY& entry);
		int		EncodeCmapFmt4(FILE* pFontFile,CMAP_TABLE_ENTRY& entry);
		int		EncodeCmapFmt4forGB_CJK(FILE* pFontFile,CMAP_TABLE_ENTRY& entry);
		int		NextGroup(int ix, int eov, GroupRecord& group);
		int		EncodeCmapFmt6(FILE* pFontFile,CMAP_TABLE_ENTRY& entry);
		int		EncodeCmapFmt8(FILE* pFontFile,CMAP_TABLE_ENTRY& entry);
		int		EncodeCmapFmt10(FILE* pFontFile,CMAP_TABLE_ENTRY& entry);
		int		EncodeCmapFmt12(FILE* pFontFile,CMAP_TABLE_ENTRY& entry);
		int		EncodeCmapFmt13(FILE* pFontFile,CMAP_TABLE_ENTRY& entry);
		int		EncodeCmapFmt14(FILE* pFontFile,CMAP_TABLE_ENTRY& entry);

	public:
		// EBLC CBLC Table
		int		DecodeEBLC();
		int		DecodeCBLC();
		void	DumpCBLC();
		void	DecodeSbitLineMetrics(SbitLineMetrics& sbit);
		void	DecodeBigGlyphMetrics(BigGlyphMetrics& bigMtrcs);
		int		DecodeIndexSubTableArray(BitmapSize& bitmapSizeTable,long  indexSubTableArrayOffset);
		int		DecodeIndexSubTable(IndexSubTableArray& IndexSubArray);
		void	DecdoeIndexSubTable1(IndexSubTableArray& IndexSubArray);
		void	DecdoeIndexSubTable2(IndexSubTableArray& IndexSubArray);
		void	DecdoeIndexSubTable3(IndexSubTableArray& IndexSubArray);
		void	DecdoeIndexSubTable4(IndexSubTableArray& IndexSubArray);
		void	DecdoeIndexSubTable5(IndexSubTableArray& IndexSubArray);

		void	DecodeBmpLC(unsigned long ulBLCPos);
		int		EncodeCBLC();
		int		EncodeEBLC();
		void	EncodeBLCInfo(CHYCBLC& HYCblc,unsigned long ulBLCPos);
		void	EncodeIndexSubTableArray(IndexSubTableArray& subtableArray, int additionalOffset);
		void	EncodeIndexSubTable1(IndexSubTableArray& IndexArray, unsigned long additionalOffset);
		void	EncodeIndexSubTable2(IndexSubTableArray& IndexArray, unsigned long additionalOffset);
		void	EncodeIndexSubTable3(IndexSubTableArray& IndexArray, unsigned long additionalOffset);
		void	EncodeIndexSubTable4(IndexSubTableArray& IndexArray, unsigned long additionalOffset);
		void	EncodeIndexSubTable5(IndexSubTableArray& IndexArray, unsigned long additionalOffset);

		// EBDT CBDT Table
		int		DecodeEBDT();
		int		DecodeCBDT();
		void	DecodeEBDTData(IndexSubTableArray& arrySubtable,unsigned long ulEBDTPos);
		void	DecodeEBDTFormat1(CBDTFormat1& format1,unsigned long dataLen);
		void	DecodeEBDTFormat2(CBDTFormat2& format2,unsigned long dataLen);
		void	DecodeEBDTFormat5(CBDTFormat5& format5,unsigned long dataLen);
		void	DecodeEBDTFormat6(CBDTFormat6& format6, unsigned long dataLen);
		void	DecodeEBDTFormat7(CBDTFormat7& format7, unsigned long dataLen);
		void	DecodeEBDTFormat8(CBDTFormat8& format8);
		void	DecodeEBDTFormat9(CBDTFormat9& format9);
		void	DecodeEBDTFormat17(CBDTFormat17& format17);
		void	DecodeEBDTFormat18(CBDTFormat18& format18);
		void	DecodeEBDTFormat19(CBDTFormat19& format19);

		void	DumpCBDT();
		int		EncodeEBDT();
		int		EncodeCBDT();
		int		EncodeEBDTData(unsigned long uloffsetEBDT);
		void	EncodeEBDTFormat1(IndexSubTableArray& SubTableArray, unsigned long uloffsetEBDT);
		void	EncodeEBDTFormat2(IndexSubTableArray& SubTableArray, unsigned long uloffsetEBDT);
		//Format 3: (obsolete) Format 4: (not supported) metrics in EBLC, compressed data
		void	EncodeEBDTFormat5(IndexSubTableArray& SubTableArray, unsigned long uloffsetEBDT);
		void	EncodeEBDTFormat6(IndexSubTableArray& SubTableArray, unsigned long uloffsetEBDT);
		void	EncodeEBDTFormat7(IndexSubTableArray& SubTableArray, unsigned long uloffsetEBDT);
		void	EncodeEBDTFormat8(IndexSubTableArray& SubTableArray, unsigned long uloffsetEBDT);
		void	EncodeEBDTFormat9(IndexSubTableArray& SubTableArray, unsigned long uloffsetEBDT);
		void	EncodeEBDTFormat17(IndexSubTableArray& SubTableArray, unsigned long uloffsetEBDT);
		void	EncodeEBDTFormat18(IndexSubTableArray& SubTableArray, unsigned long uloffsetEBDT);
		void	EncodeEBDTFormat19(IndexSubTableArray& SubTableArray, unsigned long uloffsetEBDT);		
		//head table
		int		Decodehead();
		void	Dumphead(CMarkup& MK);
		int		Encodehead();
		
		//hhea table
		int		Decodehhea();
		void	Dumphhea();
		int		Encodehhea();
		//hmtx table
		int		Decodehmtx();
		void	Dumphmtx();
		int		Encodehmtx();
		//maxp table
		int		Decodemaxp();	
		void	Dumpmaxp(CMarkup &Mk);
		int		Encodemaxp();
		//name table
		int		Decodename();
		void	Dumpname();
		int		Encodename();
		// os2 table
		int		DecodeOS2();
		void	DumpOS2();
		int		EncodeOS2();
		// post table
		int		Decodepost();
		void	Dumppost();
		int		Encodepost(HYFIXED& fxVersion);	
		//prep table
		int		Decodeprep();
		void	Dumpprep();
		int		Encodeprep();
		// Tables Related to TrueType Outlines
		int		Decodecvt();
		void	Dumpcvt();
		int		Decodefgpm();
		void	Dumpfgpm();
		// glyf table
		int		Decodeglyf();
		void	Dumpglyf();
		int		EncodeGlyph();

	protected:
		void	DecodeGLYF_SIMPLE(CHYGlyph& Glyph, unsigned short usContours);
		void	DecodeGLYF_COMPOS(CHYGlyph& Glyph);	
	public:	
		// loca table
		int		Decodeloca();
		void	Dumploca();
		int		Encodeloca();	

		// Tables Related to PostScript Outlines
		int		DecodeCFF();
		void	DecodeNameString();	
		void	DecodeTopDict(CHYCFFTopDict& TopDict);	
		
		void	DecodeGlobalSubIndex();
		void	DecodeFDArray(unsigned long ulOffset,unsigned long ulCFFBegPos);
		void	DecodePrivteDict(CHYCFFPrivteDict& PrvtDict, unsigned long ulOffset,unsigned long ulDataSize);
		void	DecodeFDSelect(unsigned long  ulOffset);		
		int		GetFDIndex(CHYCFFFDSelect& FDSelect, int iGID);
		void	DecodeCharString(unsigned long  ulOffset);		
		void	DecodeCharData(unsigned char* pData, unsigned long ulDataSize,CHYGlyph& HYGryph, int iGID, double& AdvWidth,void* pCharParam,int iDepth,CHYContour& HYContour);					
		void	SetMovePath (bool &movePath, size_t depth, size_t &injectPos, unsigned char const *opstart, unsigned char const * bufstart);
		int		HintBytes(unsigned hintCount);

		std::string DecodeCFFString(int SID, CHYCFFIndex& strIndex);
		void		DecodeCFFIndex(CHYCFFIndex& CffIndex);
		long		DecodeCFFInteger(int iDataSize);
		long		DecodeDICTInteger();
		double		DecodeDICTReal();
		int			EncodeCFF();
	protected:
		void		PrepareCFFInfo();
		int			EncodeCFFHead(std::vector<char>& vtHead);
		int			EncodeNameIndex(std::vector<char>& vtName);
		void		DecodeStringIndex();
		int			EncodeStringIndex(std::vector<char>& vtString);
		int			EncodeGlobalIndex(std::vector<char>& vtGlobal);
		int			EncodeTopDictIndex(std::vector<char>& vtTopDict);
		int			EncodeFDSelect(std::vector<char>& vtData);
		int			EncodeFDArray(std::vector<char>& vtFDArrayData,size_t stOffset);
		void		DecodeCharSets(unsigned long  ulOffset);
		int			EncodeCharSets(std::vector<char>& vtCharSetData);
		int			EncodeCharStrings(std::vector<char>& vtCharSetData);
		int			EncodeCharString(std::vector<char>& vtCharSetData,CHYCFFPrivteDict& PrivteDict, CHYGlyph& Glyph);
		int			EncodeLineToData(std::vector<char>& vtData, CHYPoint& PrevHYPoint, std::vector<CHYPoint>& vtHYPoints, size_t& iPointIndex, int& iOperatorFlag, int& iDirection,size_t& iArgumentStackSize);
		int			EncodeCurveToData(std::vector<char>& vtData, CHYPoint& PrevHYPoint, std::vector<CHYPoint>& vtHYPoints, size_t& iPointIndex, int& iOperatorFlag, int& iDirection,size_t& iArgumentStackSize);
		int			EncodeIndexData(std::vector<char>& vtOutData,std::vector<char>& vtData, std::vector<unsigned int>& vtOffset);		
		int			EncodePrivateDICTData(std::vector<char>& vtData,CHYCFFPrivteDict& PrivtDict);
		void		EncodeDICTMaxInteger(long lsrc, std::vector<char>& vtData);
		void		EncodeDICTInteger(long lsrc, std::vector<char>& vtData);
		void		EncodeDICTReal(double dbsrc, std::vector<char>& vtData);
		unsigned int	PutStringToVectorChar(std::vector<char>& vtData, std::string str);
		int				GetFDIndex(unsigned int iGID);

	public:
		//Advanced Typographic Tables
		int			DecodeBASE();
		void		DumpBASE();
		int			DecodeGDEF();
		void		DumpGDEF();
		int			DecodeGPOS();
		void		DumpGPOS();
		int			DecodeGSUB();
		void		DumpGSUB();
		bool		DecodeScriptList(unsigned long ulOffset);
		bool		DecodeFeaturetList(unsigned long ulOffset);
		bool		DecodeLookupList(unsigned long ulOffset);

		int			EncodeGSUB();
		int			BulidGSUB();
		int			EncodeScriptList();
		int			EncodeFeatureList();
		int			EncodeLookupList();

		int			EncodeGPOS();

		int			DecodeJSTF();
		void		DumpJSTF();

		//Other OpenType Tables	
		int			MakeGasp_ClearType();
		int			Decodegasp();
		void		Dumpgasp();
		int			Encodegasp();

		int			Decodehdmx();
		void		Dumphdmx();
		int			Decodekern();
		void		Dumpkern();
		int			DecodeLTSH();
		void		DumpLTSH();
		int			DecodePCLT();
		void		DumpPCLT();
		int			DecodeVDMX();
		void		DumpVDMX();

		int			Decodevhea();
		void		Dumpvhea();
		int			Encodevhea();
		int			Decodevmtx();
		void		Dumpvmtx();
		int			Encodevmtx();		

		int			Decodesbix();
		void		Dumpsbix();
		int			Encodesbix();

		int			DecodeCVT();
		void		DumpCVT();
		int			EncodeCVT();

		int			Decodefpgm();
		void		Dumpfpgm();
		int			Encodefpgm();

		int			DecodeVORG();
		void		DumpVORG();
		int			EncodeVORG();
		void		CountVORG();

		int			DecodeDSIG();
		void		DumpDSIG();
		int			EncodeDSIG();

		int			DecodeCOLR();
		void		DumpCOLR();
		int			EncodeCOLR();

		int			DecodeSVG();
		void		DumpSVG();
		int			EncodeSVG();

		//Font Variations Table
		int			Decodeavar();
		void		Dumpavar();
		int			Encodeavar();

		int			Decodefvar();
		void		Dumpfvar(CMarkup& MK);
		int			Encodefvar();

		int			Decodegvar();
		void		Dumpgvar(CMarkup& MK);
		int			DecodeGlyphVariationData(unsigned long uloffsetGlyphVartnDataAry);
		int			DecocdeGlyphVariationDataHeader(glyphVariationData&  VGlyphData,CHYGlyph& curglyph);
		int 		Decocdeserializeddata(glyphVariationData&  VGlyphData,CHYGlyph& curglyph);
		unsigned long  DecocdepointNumbers(FILE* pFile, pointNumbers& pntNumbers);
		int			Encodegvar();
		void		EndocdeSharedtuples(std::vector<unsigned char>& vtShrdtpls);
		void		EndocdeGlyphVariationData(glyphVariationData& glyphVrtDt);
		void		EncodeTupleVariationHeader(TupleVariationHeader& Header, std::vector<unsigned char>& vtGlyphVariationData, std::vector<unsigned long>& vtVrtnDtSzPos, unsigned long& ulOffset);
		void		EndocdeSerializedData(glyphVariationData& glyphVrtDt,std::vector<unsigned short>& vtVrtnDtSz);
		int			EndocdepointNumber(pointNumbers& ptNumber, std::vector<unsigned char>& vtGlyphVariationData);
		int			EndocdePackedDeltas(std::vector<short>& vtVariationPoint, std::vector<unsigned char>& vtGVData);

		int			DecodeSTAT();		
		void		DecodeDesignAxes(unsigned long offstTable,CHYSTAT& HYStat);
		void		DecodeAxisValue(unsigned long offstTable,CHYSTAT& HYStat);
		void		DumpSTAT(CMarkup& MK);
		int			EncodeSTAT();

		int			DecodeVariationRegionList(long Offset, CHYItemVariationStore* pitmVratnStr);
		int			DecodeItemVariationData(long Offset, CHYItemVariationData& itmVrtndt);
		int			DecodeDeltaSetIndexMap(long Offset, DeltaSetIndexMap&  dltstInxmp);
		int			DecodeitemVariationStore(long Offset, long tbFlag);		

		int			DecodeHVAR();
		void		DumpHVAR(CMarkup& MK);
		int			EnCodeHVAR();
		void		EnCodeItemVariationStore(FILE* pFl, CHYItemVariationStore& itemVariationStore);
		void		EncodeDeltaSetIndexMap(FILE* pFlie, DeltaSetIndexMap& DtIndxMp);

		int			DecodeVVAR();
		void		DumpVVAR(CMarkup& MK);

	public:
		// 通过Unicode编码获取Gryph Index 遍历CMAP表
		int				FindGryphIndexByUnciode(unsigned long ulUnicode);
		// 通过Unicode编码获取Gryph Index 遍历m_vtHYGlyphs
		int				FindGryphIndexByUnciodeEx(unsigned long ulUnicode);
		// 通过name Gryph Index获取
		int				FindGryphIndexByGlyName(string strGName);

		// 通过Gryph Index 获取 Unicode,一个GryphIndex 可能会对应多个Uncoide 编码	
		void			FindGryphUncidoByIndex(unsigned long  ulGlyphIndex, std::vector<unsigned long>& szUnicode);

		int				FindHeadXminGID();
		int				FindHeadXmaxGID();
		int				FindHeadYminGID();
		int				FindHeadYmaxGID();
		int				FindVertMinTop();
		int				FindVertMinBottom();
		int				FindVertTopSideBearGID();
		int				FindVertBottomSideBearGID();
		int				FindMinRightSideBearing();
		unsigned short	GetAdvancMaxWidth();
		unsigned short	GetAdvancMaxHeight();		
		void			CountFontBound();		
		bool			MulToWCharBL(char* pDest, int& iDestlen,const char* pSrc, int iSrclen);
		bool			MulToWChar(char* pDest, int& iDestlen,const char* pSrc, int iSrclen);

		int				FindGlyph(unsigned long ulUni, std::vector<CHYGlyph>& vtHYGlyphs);		
		int				FindLastCharIndex(std::vector<CHYGlyph>& vtHYGlyphs);
		int				GetGlyfAdvancWidth(int iGlyphID);		
		int				GetGlyfAdvancHeight(int iGlyphID);
		void			GetGlyfVrtmetric(int iGlyphID, VMTX_LONGVRTMETRIC& Vrtmetric);
		void			CountUnicodeRange(unsigned long& uni1, unsigned long& uni2,unsigned long& uni3,unsigned long& uni4);		
		int				fourbytesAlign(unsigned long Entrylength);
		void			CountGlyphBox(unsigned int iGid, std::vector<CHYGlyph>&	vtHYGlyphs);
		bool			FindFlag(std::vector<unsigned long>vtFlag, unsigned long  FindFlag);
		bool			FindFlag(unsigned long  FindFlag);
		void			GetFontHist(std::vector<int>& vtHist, int iDirct);
		void			RemoveRepeatpoint();
		void			TailAddRepeatpoint();
			
	public:
		std::vector<CHYGlyph>	m_vtHYGlyphs;
		CHYCmap					m_HYCmap;
		CHYMaxp					m_HYMaxp;
		CHYLoca					m_HYloca;
		CHYHead					m_HYhead;
		CHYOS					m_HYOS2;
		CHYHhea					m_HYHhea;
		CHYHmtx					m_HYHmtx;
		CHYVhea					m_HYVhea;
		CHYVmtx					m_HYVmtx;
		CHYFpgm					m_HYFpgm;
		CHYPrep					m_HYPrep;
		CHYCvt					m_HYCvt;
		CHYName					m_HYName;
		CHYPost					m_HYPost;
		CHYTableDirectory		m_HYTbDirectory;
		CHYGsub					m_HYGsub;
		CHYCBLC					m_HYCblc;
		CHYCBDT					m_HYCBDT;
		CHYsbix					m_HYsbix; 
		std::vector<char>		m_HYfpgm;
		CHYDSIG					m_HYDSIG;
		CHYCOLR					m_HYCOLR;
		CHYSVG					m_HYSVG;
		CHYVORG					m_HYVORG;

		FILE*					m_pFontFile;
		int						m_iFontType;
		int						m_iStepNo;
		CHYCFFInfo				m_HYCFFInfo;
		CHYCodeMap				m_HYCodeMap;
		
		CHYavar					m_HYavar;
		CHYfvar					m_HYfvar;
		CHYgvar					m_HYgvar;
		CHYSTAT					m_HYStat;
		CHYHVAR					m_HYHVAR;
		CHYVVAR					m_HYVVAR;
		CHYGasp					m_HYGasp;

		TableDataS				m_mulpTableData;

		std::multimap<unsigned long,unsigned long>	m_mpVariantChar;
		std::multimap<unsigned long,unsigned long>	m_mpCompatChar;
		std::multimap<unsigned long, unsigned long>	m_mpJ2FChar;
		std::multimap<unsigned long, unsigned long>	m_mpKangxiChar;


		HY_OPTION_PRM			m_tagOption;	
	};
};

	