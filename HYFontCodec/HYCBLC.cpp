#include "StdAfx.h"
#include "HYCBLC.h"

namespace HYFONTCODEC
{
	BigGlyphMetrics::BigGlyphMetrics()
	{
		height			= 0;
		width			= 0;
		horiBearingX	= 0;
		horiBearingY	= 0;
		horiAdvance		= 0;
		vertBearingX	= 0;
		vertBearingY	= 0;
		vertAdvance		= 0;
	
	}	// end of BigGlyphMetrics::BigGlyphMetrics()

	BigGlyphMetrics::~BigGlyphMetrics()
	{

	}	// end of BigGlyphMetrics::BigGlyphMetrics()

	void BigGlyphMetrics::SetDefault()
	{
		height			= 0;
		width			= 0;
		horiBearingX	= 0;
		horiBearingY	= 0;
		horiAdvance		= 0;
		vertBearingX	= 0;
		vertBearingY	= 0;
		vertAdvance		= 0;

	}	// end of void BigGlyphMetrics::SetDefault()

	SmallGlyphMetrics::SmallGlyphMetrics()
	{
		height			= 0;
		width			= 0;
		BearingX		= 0;
		BearingY		= 0;
		Advance			= 0;

	}	// end of SmallGlyphMetrics::SmallGlyphMetrics()

	SmallGlyphMetrics::~SmallGlyphMetrics()
	{

	}	// end of SmallGlyphMetrics::~SmallGlyphMetrics()

	void SmallGlyphMetrics::SetDefault()
	{
		height			= 0;
		width			= 0;
		BearingX		= 0;
		BearingY		= 0;
		Advance			= 0;

	}	// end of void SmallGlyphMetrics::SetDefault()

	SbitLineMetrics ::SbitLineMetrics ()
	{
		Setdefault();

	}	// end of SbitLineMetrics ::SbitLineMetrics ()

	SbitLineMetrics::~SbitLineMetrics ()
	{

	}	// end of SbitLineMetrics ::~SbitLineMetrics ()

	void SbitLineMetrics::Setdefault()
	{
		ascender = 0;
		descender = 0;
		widthMax = 0;
		caretSlopeNumerator = 0;
		caretSlopeDenominator = 0;
		caretOffset = 0;
		minOriginSB = 0;
		minAdvanceSB = 0;
		maxBeforeBL = 0;
		minAfterBL = 0;
		pad1 = 0;
		pad2 = 0;

	}	// end of void SbitLineMetrics::Setdefault()
	
	IndexSubTable1::IndexSubTable1()
	{

	}	// end of IndexSubTable1::IndexSubTable1()

	IndexSubTable1::~IndexSubTable1()
	{

	}	// end of IndexSubTable1::~IndexSubTable1()

	void IndexSubTable1::Setdefault()
	{		
		vtsbitOffsets.clear();		

	}	// end of void IndexSubTable1::Setdefault()

	IndexSubTable2::IndexSubTable2()
	{
		ulImageSize = 0;	

	}	// end of IndexSubTable2::IndexSubTable2()

	IndexSubTable2::~IndexSubTable2()
	{

	}	// end of IndexSubTable2::~IndexSubTable2()

	void IndexSubTable2::Setdefault()
	{
		ulImageSize = 0;
		bigMetrics.SetDefault();

	}	// end of void IndexSubTable2::Setdefault()
	
	IndexSubTable3::IndexSubTable3()
	{

	}	// end of IndexSubTable3::IndexSubTable3()

	IndexSubTable3::~IndexSubTable3()
	{

	}	// end of IndexSubTable3::~IndexSubTable3()

	void IndexSubTable3::Setdefault()
	{
		vtOffsetArray.clear();

	}	// end of void IndexSubTable3::Setdefault()

	GlyphIdOffsetPair::GlyphIdOffsetPair()
	{
		SetDefault();

	}	// end of GlyphIdOffsetPair::GlyphIdOffsetPair()

	GlyphIdOffsetPair::~GlyphIdOffsetPair()
	{
		SetDefault();

	}	// end of GlyphIdOffsetPair::~GlyphIdOffsetPair()

	void GlyphIdOffsetPair::SetDefault()
	{
		glyphID = 0;	// Glyph ID of glyph present.
		sbitOffset = 0;	//Location in EBDT.

	}	// end of void GlyphIdOffsetPair::SetDefault()

	IndexSubTable4::IndexSubTable4()
	{

	}	// end of IndexSubTable4::IndexSubTable4()

	IndexSubTable4::~IndexSubTable4()
	{

	}	// end of IndexSubTable4::~IndexSubTable4()

	void IndexSubTable4::Setdefault()
	{
		vtGlyphArray.clear();

	}	// end of void IndexSubTable4::Setdefault()

	IndexSubTable5::IndexSubTable5()
	{
		imageSize = 0;			//all glyphs have the same data size		
		numGlyphs = 0;			//array length

	}	// end of IndexSubTable5::IndexSubTable5()

	IndexSubTable5::~IndexSubTable5()
	{

	}	// end of IndexSubTable5::~IndexSubTable5()

	void IndexSubTable5::Setdefault()
	{
		imageSize			= 0;			//all glyphs have the same data size
		bigMetrics.SetDefault();			//all glyphs have the same metrics
		numGlyphs			= 0;			//array length
		vtGlyphIDArray.clear();	//one per glyph, sorted by glyph code; sizeOfArray=numGlyphs

	}	// end of void IndexSubTable5::Setdefault()

	IndexSubHeader::IndexSubHeader()
	{
		indexFormat		= 0;		//format of this indexSubTable
		imageFormat		= 0;		//format of 'EBDT' image data
		imageDataOffset = 0;		//offset to image data in 'EBDT' table

	}	// end of IndexSubHeader::IndexSubHeader()

	IndexSubHeader::~IndexSubHeader()
	{

	}	// end of IndexSubHeader::~IndexSubHeader()

	void IndexSubHeader::Setdefault()
	{
		indexFormat = 0;
		imageFormat = 0;
		imageDataOffset = 0;

	}	// end of void IndexSUBTable::Setdefault()

	CEBData::CEBData()
	{

	}	//end of CEBData::CEBData()

	CEBData::~CEBData()
	{

	}	//end of CEBData::~CEBData()

	IndexSUBTable::IndexSUBTable()
	{

	}	// end of IndexSubTable1::IndexSUBTable()

	IndexSUBTable::~IndexSUBTable()
	{

	}	// end of IndexSubTable1::~IndexSUBTable()

	void IndexSUBTable::Setdefault()
	{
		Header.Setdefault();
		SubTable1.Setdefault();
		SubTable2.Setdefault();
		SubTable3.Setdefault();
		SubTable4.Setdefault();
		SubTable5.Setdefault();

	}	// end of void IndexSUBTable::Setdefault()	

	IndexSubTableArray::IndexSubTableArray()
	{
		firstGlyphIndex	 = 0;		//first glyph code of this range
		lastGlyphIndex	 = 0;		//last glyph code of this range (inclusive)
		Offset			 = 0;		//add to indexSubTableArrayOffset to get offset from beginning of 'EBLC'

	}	// end of IndexSubTableArray::IndexSubTableArray()

	IndexSubTableArray::~IndexSubTableArray()
	{

	}	// end of IndexSubTableArray::~IndexSubTableArray()

	void IndexSubTableArray::SetDefault()
	{
		firstGlyphIndex	 = 0;
		lastGlyphIndex	 = 0;
		Offset			 = 0;

	}	// end of void IndexSubTableArray::SetDefault()

	BitmapSize::BitmapSize()
	{
		indexSubTableArrayOffset	= 0;
		indexTablesSize				= 0;
		numberofIndexSubTables		= 0;
		colorRef					= 0;
		startGlyphIndex				= 0;
		endGlyphIndex				= 0;
		ppemX						= 0;
		ppemY						= 0;
		bitDepth					= 0;
		Flags						= 0;

	}	// end of BitmapSize::BitmapSize()

	BitmapSize::~BitmapSize()
	{

	}	// end of BitmapSize::~BitmapSize()

	void BitmapSize::SetDefault()
	{
		indexSubTableArrayOffset	= 0;
		indexTablesSize				= 0;
		numberofIndexSubTables		= 0;
		colorRef					= 0;
		startGlyphIndex				= 0;
		endGlyphIndex				= 0;
		ppemX						= 0;
		ppemY						= 0;
		bitDepth					= 0;
		Flags						= 0;

	}	// end of void BitmapSize::SetDefault()

	CBLCHEADER::CBLCHEADER()
	{
		numsizes = 0;

	}	// end of CBLCHEADER::CBLCHEADER()

	CBLCHEADER::~CBLCHEADER()
	{

	}	// end of CBLCHEADER::~CBLCHEADER()

	void CBLCHEADER::SetDefault()
	{
		version.fract = 0;
		version.value = 0;
		numsizes = 0;

	}	// end of void CBLCHEADER::SetDefault()

	CHYCBLC::CHYCBLC(void)
	{

	}	// end of CHYCBLC::CHYCBLC()

	CHYCBLC::~CHYCBLC(void)
	{

	}	// end of CHYCBLC::~CHYCBLC(void)

	void CHYCBLC::SetDefault()
	{
		Header.SetDefault();
		vtBitmapSizeTb.clear();

	}	// end of void CHYCBLC::SetDefault()

	CBDTFormat1::CBDTFormat1()
	{
		SetDefult();

	}	// end of CBDTFormat1::CBDTFormat1()

	CBDTFormat1::~CBDTFormat1()
	{
		SetDefult();

	}	// end of CBDTFormat1::~CBDTFormat1()

	void CBDTFormat1::SetDefult()
	{
		SmlGlyphMtcs.SetDefault();
		
		imagedata.clear();

	}	// end of void CBDTFormat1::Setdefult()

	CBDTFormat2::CBDTFormat2()
	{
		SetDefult();

	}	// end of CBDTFormat2::CBDTFormat2()

	CBDTFormat2::~CBDTFormat2()
	{
		SetDefult();

	}	// end of CBDTFormat1::~CBDTFormat1()

	void CBDTFormat2::SetDefult()
	{
		SmlGlyphMtcs.SetDefault();
		imagedata.clear();

	}	// end of void CBDTFormat2::Setdefult()

	CBDTFormat5::CBDTFormat5()
	{
		SetDefult();

	}	// end of CBDTFormat5::CBDTFormat5()

	CBDTFormat5::~CBDTFormat5()
	{
		SetDefult();

	}	// end of CBDTFormat5::~CBDTFormat5()

	void CBDTFormat5::SetDefult()
	{
		imagedata.clear();

	}	// end of void CBDTFormat5::SetDefult()

	CBDTFormat6::CBDTFormat6()
	{
		SetDefult();

	}	// end of CBDTFormat6::CBDTFormat6()

	CBDTFormat6::~CBDTFormat6()
	{
		SetDefult();

	}	// end of CBDTFormat6::~CBDTFormat6()

	void CBDTFormat6::SetDefult()
	{
		bigGlyphMtcs.SetDefault();
		imagedata.clear();

	}	// end of void CBDTFormat6::Setdefult()

	CBDTFormat7::CBDTFormat7()
	{
		SetDefault();

	}	// end of CBDTFormat7::CBDTFormat7()

	CBDTFormat7::~CBDTFormat7()
	{
		SetDefault();

	}	// end of CBDTFormat7::~CBDTFormat7()

	void CBDTFormat7::SetDefault()
	{
		bigGlyphMtcs.SetDefault();
		imagedata.clear();

	}	// end of void CBDTFormat7::SetDefault()

	CEbdtComponent::CEbdtComponent()
	{
		SetDefault();

	}	// end of CEbdtComponent::CEbdtComponent()

	CEbdtComponent::~CEbdtComponent()
	{
		SetDefault();

	}	// end of CEbdtComponent::~CEbdtComponent()

	void CEbdtComponent::SetDefault()
	{
		glyphID = 0;
		xOffset = 0;
		yOffset = 0;

	}	// end of void CEbdtComponent::SetDefault()

	CBDTFormat8::CBDTFormat8()
	{
		SetDefault();

	}	// end of CBDTFormat8::CBDTFormat8()

	CBDTFormat8::~CBDTFormat8()
	{
		SetDefault();

	}	// end of CBDTFormat8::~CBDTFormat8()

	void CBDTFormat8::SetDefault()
	{
		smallGlyphMtcs.SetDefault();
		pad = 0;
		numComponents = 0;
		vtEbdtComponent.clear();

	}	// end of void CBDTFormat8::SetDefault()

	CBDTFormat9::CBDTFormat9()
	{
		SetDefault();

	}	// end of CBDTFormat9::CBDTFormat9()

	CBDTFormat9::~CBDTFormat9()
	{
		SetDefault();

	}	// end of CBDTFormat9::~CBDTFormat9()

	void CBDTFormat9::SetDefault()
	{
		bigGlyphMtcs.SetDefault();
		pad = 0;
		numComponents = 0;
		vtEbdtComponent.clear();

	}	// end of void CBDTFormat9::SetDefault()

	CBDTFormat17::CBDTFormat17()
	{		
		dataLen = 0;

	}	// end of CBDTFormat17::CBDTFormat17()
		
	CBDTFormat17::~CBDTFormat17()
	{

	}	// end of CBDTFormat17::~CBDTFormat17()

	void CBDTFormat17::SetDefault()
	{		
		data.clear();
		dataLen = 0;
		smallGlyphMtcs.SetDefault();

	}	// end of void CBDTFormat17::SetDefault()

	CBDTFormat18::CBDTFormat18()
	{		
		dataLen = 0;

	}	// end of CBDTFormat18::CBDTFormat18()

	CBDTFormat18::~CBDTFormat18()
	{

	}	// end of CBDTFormat18::~CBDTFormat18()

	void CBDTFormat18::SetDefault()
	{		
		data.clear();
		dataLen = 0;
		bigGlyphMtcs.SetDefault();

	}	// end of void CBDTFormat18::SetDefault()

	CBDTFormat19::CBDTFormat19()
	{		
		dataLen = 0;

	}	// end of CBDTFormat19::CBDTFormat19()

	CBDTFormat19::~CBDTFormat19()
	{

	}	// end of CBDTFormat19::~CBDTFormat19()

	void CBDTFormat19::SetDefault()
	{		
		data.clear();
		dataLen = 0;	

	}	// end of void CBDTFormat19::SetDefault()

	CHYCBDT::CHYCBDT(void)
	{
		SetDefault();

	}	// end of HYCBDT::HYCBDT(void)

	CHYCBDT::~CHYCBDT(void)
	{
		SetDefault();

	}	// end of HYCBDT::~HYCBDT(void)

	void CHYCBDT::SetDefault()
	{		
		majorVersion = 0;
		minorVersion = 0;

	}	// end of void CHYCBDT::SetDefault()

}