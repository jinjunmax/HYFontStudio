#include "StdAfx.h"
#include "HYCmap.h"

namespace HYFONTCODEC
{
	CMAP_ENCODE_FORMAT_0::CMAP_ENCODE_FORMAT_0()
	{
		format	= 0;
		length	= 0;
		language = 0;

	}	// end of CMAP_ENCODE_FORMAT_0::CMAP_ENCODE_FORMAT_0()

	CMAP_ENCODE_FORMAT_0::~CMAP_ENCODE_FORMAT_0()
	{
		vtGlyphId.clear();

	}	// end of CMAP_ENCODE_FORMAT_0::CMAP_ENCODE_FORMAT_0()

	void CMAP_ENCODE_FORMAT_0::SetDefault()
	{
		format	= 0;
		length	= 0;
		language = 0;
		vtGlyphId.clear();

	}	// end of void CMAP_ENCODE_FORMAT_0::SetDefault()

	CMAP_ENCODE_FORMAT_2_SUBHEAD::CMAP_ENCODE_FORMAT_2_SUBHEAD()
	{	
		firstCode		= 0;				
		entryCount		= 0;				
		idDelta			= 0;
		idRangeOffset	= 0;

	}	// end of CMAP_ENCODE_FORMAT_2_SUBHEAD::CMAP_ENCODE_FORMAT_2_SUBHEAD()

	CMAP_ENCODE_FORMAT_2_SUBHEAD::~CMAP_ENCODE_FORMAT_2_SUBHEAD()
	{

	}	// end of CMAP_ENCODE_FORMAT_2_SUBHEAD::~CMAP_ENCODE_FORMAT_2_SUBHEAD()

	void CMAP_ENCODE_FORMAT_2_SUBHEAD::SetDefault()
	{
		firstCode		= 0;				
		entryCount		= 0;				
		idDelta			= 0;
		idRangeOffset	= 0;

	}	// end of void CMAP_ENCODE_FORMAT_2_SUBHEAD::SetDefault()

	CMAP_ENCODE_FORMAT_2::CMAP_ENCODE_FORMAT_2()
	{
		format		= 0;
		length		= 0;
		language	= 0;	

	}	// end of CMAP_ENCODE_FORMAT_2::CMAP_ENCODE_FORMAT_2()

	CMAP_ENCODE_FORMAT_2::~CMAP_ENCODE_FORMAT_2()
	{
		vtsubHeaderKeys.clear();	
		vtSubHead.clear();
		vtglyphIndex.clear();

	}	//end of CMAP_ENCODE_FORMAT_2::~CMAP_ENCODE_FORMAT_2()

	void CMAP_ENCODE_FORMAT_2::SetDefault()
	{
		format		= 0;
		length		= 0;
		language	= 0;

		vtsubHeaderKeys.clear();	
		vtSubHead.clear();
		vtglyphIndex.clear();

	}	// end of void CMAP_ENCODE_FORMAT_2::SetDefault()

	CMAP_ENCODE_FORMAT_4::CMAP_ENCODE_FORMAT_4()
	{
		format				= 0;					
		length				= 0;					
		language			= 0;				
		segCountX2			= 0;			 	
		searchRange			= 0;		 	
		entrySelector		= 0;	 		
		rangeShift			= 0;			
		reservedPad			= 0;

	}	// end of CMAP_ENCODE_FORMAT_4::CMAP_ENCODE_FORMAT_4()

	CMAP_ENCODE_FORMAT_4::~CMAP_ENCODE_FORMAT_4()
	{

	}	// end of CMAP_ENCODE_FORMAT_4::~CMAP_ENCODE_FORMAT_4()

	void CMAP_ENCODE_FORMAT_4::SetDefault()
	{
		format				= 0;					
		length				= 0;					
		language			= 0;				
		segCountX2			= 0;			 	
		searchRange			= 0;		 	
		entrySelector		= 0;	 		
		rangeShift			= 0;			
		reservedPad			= 0;

	}	// end of void CMAP_ENCODE_FORMAT_4::SetDefault()

	CMAP_ENCODE_FORMAT_6::CMAP_ENCODE_FORMAT_6()
	{
		format				= 0;		
		length				= 0;
		language			= 0;
		firstCode			= 0;
		entryCount			= 0;

	}	// end of CMAP_ENCODE_FORMAT_6::CMAP_ENCODE_FORMAT_6()

	CMAP_ENCODE_FORMAT_6::~CMAP_ENCODE_FORMAT_6()
	{
		vtGlyphId.clear();

	}	// end of CMAP_ENCODE_FORMAT_6::~CMAP_ENCODE_FORMAT_6()

	void CMAP_ENCODE_FORMAT_6::SetDefault()
	{
		format				= 0;		
		length				= 0;
		language				= 0;
		firstCode			= 0;
		entryCount			= 0;

		vtGlyphId.clear();

	}	// end of void CMAP_ENCODE_FORMAT_6::SetDefault()

	CMAP_ENCODE_FORMAT_12_GROUP::CMAP_ENCODE_FORMAT_12_GROUP()
	{
		startCharCode	= 0;
		endCharCode		= 0;
		startGlyphID	= 0;

	}	// end of CMAP_ENCODE_FORMAT_12_GROUP::CMAP_ENCODE_FORMAT_12_GROUP()

	CMAP_ENCODE_FORMAT_12_GROUP::~CMAP_ENCODE_FORMAT_12_GROUP()
	{

	}	// end of CMAP_ENCODE_FORMAT_12_GROUP::~CMAP_ENCODE_FORMAT_12_GROUP()

	void CMAP_ENCODE_FORMAT_12_GROUP::SetDefault()
	{
		startCharCode	= 0;
		endCharCode		= 0;
		startGlyphID	= 0;

	}	// end of void CMAP_ENCODE_FORMAT_12_GROUP::SetDefault()	

	CMAP_ENCODE_FORMAT_12::CMAP_ENCODE_FORMAT_12()
	{
		format = 0;
		reserved = 0;
		length = 0;
		language = 0;
		nGroups = 0;

	}	// end of CMAP_ENCODE_FORMAT_12::CMAP_ENCODE_FORMAT_12()

	CMAP_ENCODE_FORMAT_12::~CMAP_ENCODE_FORMAT_12()
	{
		vtGroup.clear();

	}	// end of CMAP_ENCODE_FORMAT_12::~CMAP_ENCODE_FORMAT_12()

	void CMAP_ENCODE_FORMAT_12::SetDefault()
	{
		format = 0;
		reserved = 0;
		length = 0;
		language = 0;
		nGroups = 0;

		vtGroup.clear();

	}	// end of void CMAP_ENCODE_FORMAT_12::SetDefault()

	CUnicodeRange::CUnicodeRange()
	{
		SetDefault();

	}	// end of CUnicodeRange::CUnicodeRange()

	CUnicodeRange::~CUnicodeRange()
	{


	}	// end of CUnicodeRange::~CUnicodeRange()

	void CUnicodeRange::SetDefault()
	{
		memset(&startUnicodeValue, 0, 3);
		additionalCount = 0;

	}	// end of void CUnicodeRange::SetDefault()

	CUVSMapping::CUVSMapping()
	{
		SetDefault();

	}	// end of CUVSMapping::CUVSMapping()

	CUVSMapping::~CUVSMapping()
	{


	}	// end of CUVSMapping::~CUVSMapping()

	void CUVSMapping::SetDefault()
	{
		memset(&unicodeValue,0,3);//Base Unicode value of the UVS
		glyphID = 0;

	}	// end of void CUVSMapping::SetDefault()

	CDefaultUVS::CDefaultUVS()
	{
		numUnicodeValueRanges = 0;

	}	// end of CDefaultUVS::CDefaultUVS()

	CDefaultUVS::~CDefaultUVS()
	{

	}	// end of CDefaultUVS::~CDefaultUVS()

	void CDefaultUVS::SetDefault()
	{
		numUnicodeValueRanges = 0;
		vtranges.clear();

	}	// end of void CDefaultUVS::SetDefault()

	CNonDefaultUVS::CNonDefaultUVS()
	{
		numUVSMappings = 0;

	}	// end of CNonDefaultUVS::CNonDefaultUVS()

	CNonDefaultUVS::~CNonDefaultUVS()
	{


	}	// end of CNonDefaultUVS::~CNonDefaultUVS()

	void CNonDefaultUVS::SetDefault()
	{
		numUVSMappings = 0;
		vtuvsMappings.clear();

	}	// end of void CNonDefaultUVS::SetDefault()

	CVariationSelector::CVariationSelector()
	{
		SetDefault();

	}	// end of CVariationSelector::CVariationSelector()

	CVariationSelector::~CVariationSelector()
	{

	}	// end of CVariationSelector::~CVariationSelector()

	void CVariationSelector::SetDefault()
	{
		memset(&varSelector, 0, 3);
		defaultUVSOffset = 0;
		nonDefaultUVSOffset = 0;

		DefaultUVS.SetDefault();
		NonDefaultUVS.SetDefault();

	}	// end of void CVariationSelector::SetDefault()

	CMAP_ENCODE_FORMAT_14::CMAP_ENCODE_FORMAT_14()
	{		
		format = 0;
		length = 0;
		numVarSelectorRecords = 0;

	}	// end of CMAP_ENCODE_FORMAT_14::CMAP_ENCODE_FORMAT_14()

	CMAP_ENCODE_FORMAT_14::~CMAP_ENCODE_FORMAT_14()
	{


	}	// end of CMAP_ENCODE_FORMAT_14::~CMAP_ENCODE_FORMAT_14()

	void CMAP_ENCODE_FORMAT_14::SetDefault()
	{
		format = 0;
		length = 0;
		numVarSelectorRecords  = 0;
		vtVariationSelector.clear();

	}	// end of void CMAP_ENCODE_FORMAT_14::SetDefault()

	CMAP_TABLE_ENTRY::CMAP_TABLE_ENTRY()
	{
		plat_ID				= 0;
		plat_encod_ID		= 0;
		offset				= 0;	

	}	// end of CMAP_TABLE_ENTRY::CMAP_TABLE_ENTRY()

	CMAP_TABLE_ENTRY::~CMAP_TABLE_ENTRY()
	{

	}	// end of CMAP_TABLE_ENTRY::~CMAP_TABLE_ENTRY()

	void CMAP_TABLE_ENTRY::SetDefault()
	{
		plat_ID				= 0;
		plat_encod_ID		= 0;
		offset				= 0;

		Format0.SetDefault();
		Format2.SetDefault();
		Format4.SetDefault();
		Format6.SetDefault();
		Format12.SetDefault();
		Format14.SetDefault();

	}	// end of void CMAP_TABLE_ENTRY::SetDefault()

	CHYCmap::CHYCmap(void)
	{
		version			= 0;
		numSubTable		= 0;

	}	// end of CHYCmap::CHYCmap()

	CHYCmap::~CHYCmap(void)
	{
		vtCamp_tb_entry.clear();

	}	// end of CHYCmap::~CHYCmap()

	void CHYCmap::SetDefault()
	{
		version			= 0;
		numSubTable		= 0;
		vtCamp_tb_entry.clear();

	}	// end of void CHYCmap::SetDefault()

	int CHYCmap::FindSpecificEntryIndex(unsigned short iFormat)
	{
		for (size_t i=0; i<vtCamp_tb_entry.size();i++)
		{
			if (vtCamp_tb_entry[i].format == iFormat)
				return i;
		}

		return -1;

	}	// end of int CHYCmap::FindSpecificEntry()

	int CHYCmap::FindSpecificEntryIndex(unsigned short  plat_ID,unsigned short plat_encod_ID,unsigned short iFormat)
	{		
		for (size_t i=0; i<vtCamp_tb_entry.size();i++)
		{
			if ((vtCamp_tb_entry[i].plat_ID == plat_ID) 
				&& (vtCamp_tb_entry[i].plat_encod_ID == plat_encod_ID) 
				&& (vtCamp_tb_entry[i].format==iFormat))			
				return i;
		}

		return -1;

	}	// end of int CHYCmap::FindSpecificEntry()

}