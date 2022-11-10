#pragma once

#include <vector>
#include "HYFontCodecDef.h"

namespace HYFONTCODEC
{
	class HY_DLL_CLASS CMAP_ENCODE_FORMAT_0
	{
	public:
		CMAP_ENCODE_FORMAT_0();
		~CMAP_ENCODE_FORMAT_0();

		void SetDefault();

	public:
		unsigned short	format;									//Format number is set to 0. 
		unsigned short	length;									//This is the length in bytes of the subtable.
		unsigned short	language;								//Version number (starts at 0).			
		std::vector<unsigned char> vtGlyphId;

	};	// end of class HY_DLL_CLASS CMAP_ENCODE_FORMAT_0

	class HY_DLL_CLASS CMAP_ENCODE_FORMAT_2_SUBHEAD
	{
	public:
		CMAP_ENCODE_FORMAT_2_SUBHEAD();
		~CMAP_ENCODE_FORMAT_2_SUBHEAD();

		void SetDefault();
	public:
		unsigned short	firstCode;				
		unsigned short	entryCount;				
		SHORT	idDelta;
		unsigned short	idRangeOffset;

	};	// end of class HY_DLL_CLASS CMAP_ENCODE_FORMAT_2_SUBHEAD

	class HY_DLL_CLASS CMAP_ENCODE_FORMAT_2
	{
	public:
		CMAP_ENCODE_FORMAT_2();
		~CMAP_ENCODE_FORMAT_2();

		void SetDefault();

	public:
		unsigned short											format;
		unsigned short											length;
		unsigned short											language;

		std::vector<unsigned short>								vtsubHeaderKeys;	
		std::vector<CMAP_ENCODE_FORMAT_2_SUBHEAD>				vtSubHead;
		std::vector<unsigned short>								vtglyphIndex;

	};	// end of class HY_DLL_CLASS CMAP_ENCODE_FORMAT_2

	class HY_DLL_CLASS CMAP_ENCODE_FORMAT_4
	{
	public:
		CMAP_ENCODE_FORMAT_4();
		~CMAP_ENCODE_FORMAT_4();

		void	SetDefault();
	public:
		unsigned short					format;					
		unsigned short					length;					
		unsigned short					language;				
		unsigned short					segCountX2;			 	
		unsigned short					searchRange;		 	
		unsigned short					entrySelector;	 		
		unsigned short					rangeShift;		
		std::vector<unsigned short>		vtEndCount;	
		unsigned short					reservedPad;	
		std::vector<unsigned short>		vtstartCount;	
		std::vector<short>				vtidDelta;
		std::vector<unsigned short>		vtidRangeOffset;
		std::vector<unsigned short>		vtglyphIdArray;

	};	// end of class HY_DLL_CLASS CMAP_ENCODE_FORMAT_4

	class HY_DLL_CLASS CMAP_ENCODE_FORMAT_6
	{
	public:
		CMAP_ENCODE_FORMAT_6();
		~CMAP_ENCODE_FORMAT_6();

		void	SetDefault();
	public:
		unsigned short		format;			
		unsigned short		length;
		unsigned short		language;
		unsigned short		firstCode;
		unsigned short		entryCount;
		std::vector<unsigned short> vtGlyphId;

	};	// end of class HY_DLL_CLASS CMAP_ENCODE_FORMAT_6

	class HY_DLL_CLASS CMAP_ENCODE_FORMAT_12_GROUP
	{
	public:
		CMAP_ENCODE_FORMAT_12_GROUP();
		~CMAP_ENCODE_FORMAT_12_GROUP();

		void	SetDefault();
	public:
		unsigned long		startCharCode;
		unsigned long		endCharCode;
		unsigned long		startGlyphID;

	};	// end of class HY_DLL_CLASS CMAP_ENCODE_FORMAT_12_GROUP

	class HY_DLL_CLASS CMAP_ENCODE_FORMAT_12
	{
	public:
		CMAP_ENCODE_FORMAT_12();
		~CMAP_ENCODE_FORMAT_12();

		void	SetDefault();
	public:
		unsigned short					format;
		unsigned short					reserved;
		unsigned long					length;
		unsigned long					language;
		unsigned long					nGroups;

		std::vector<CMAP_ENCODE_FORMAT_12_GROUP>	vtGroup;

	};	// end of class HY_DLL_CLASS CMAP_ENCODE_FORMAT_12	

	class HY_DLL_CLASS CUnicodeRange
	{
	public:
		CUnicodeRange();
		~CUnicodeRange();
	public:
		void SetDefault();

	public:
		unsigned char	startUnicodeValue[3];//First value in this range UINT24
		unsigned char 	additionalCount;	//Number of additional values in this range

	};	// end of class HY_DLL_CLASS CUnicodeRange

	class HY_DLL_CLASS CUVSMapping
	{
	public:
		CUVSMapping();
		~CUVSMapping();
	public:
		void SetDefault();

	public:
		unsigned char	unicodeValue[3];//Base Unicode value of the UVS
		unsigned short 	glyphID;	//Glyph ID of the UVS

	};	// end of class HY_DLL_CLASS CUVSMapping

	class HY_DLL_CLASS CDefaultUVS
	{
	public:
		CDefaultUVS();
		~CDefaultUVS();

		void SetDefault();
	public:
		unsigned long numUnicodeValueRanges;
		std::vector<CUnicodeRange> vtranges;

	};	// end of class HY_DLL_CLASS CDefaultUVS

	class HY_DLL_CLASS CNonDefaultUVS
	{
	public:
		CNonDefaultUVS();
		~CNonDefaultUVS();

		void SetDefault();

	public:
		unsigned long numUVSMappings;
		std::vector<CUVSMapping> vtuvsMappings;

	};	// end of class HY_DLL_CLASS CNonDefaultUVS

	class HY_DLL_CLASS CVariationSelector
	{
	public:
		CVariationSelector();
		~CVariationSelector();
	public:
		void SetDefault();

	public:
		unsigned char	varSelector[3]; 	//Variation selector UINT24
		unsigned long 	defaultUVSOffset; 	//Offset from the start of the format 14 subtable to Default UVS Table.May be 0.
		unsigned long 	nonDefaultUVSOffset; 	//Offset from the start of the format 14 subtable to Non - Default UVS Table.May be

		CDefaultUVS			DefaultUVS;
		CNonDefaultUVS		NonDefaultUVS;

	};	// end of class HY_DLL_CLASS CVariationSelector

	class HY_DLL_CLASS CMAP_ENCODE_FORMAT_14
	{
	public:
		CMAP_ENCODE_FORMAT_14();
		~CMAP_ENCODE_FORMAT_14();

		void	SetDefault();
	public:
		unsigned short  				format;
		unsigned long 	 				length;
		unsigned long 					numVarSelectorRecords;
		std::vector<CVariationSelector>	vtVariationSelector;

	};	//end of class HY_DLL_CLASS CMAP_ENCODE_FORMAT_14

	class HY_DLL_CLASS CMAP_TABLE_ENTRY
	{
	public:
		CMAP_TABLE_ENTRY();
		~CMAP_TABLE_ENTRY();
		void	SetDefault();
	public:
		unsigned short						plat_ID;
		unsigned short						plat_encod_ID;
		unsigned long						offset;					 //Byte offset from beginning of table to the subtable for this encoding.
		unsigned short						format;	
		CMAP_ENCODE_FORMAT_0				Format0;
		CMAP_ENCODE_FORMAT_2				Format2;
		CMAP_ENCODE_FORMAT_4				Format4;
		CMAP_ENCODE_FORMAT_6				Format6;
		CMAP_ENCODE_FORMAT_12				Format12;
		CMAP_ENCODE_FORMAT_14				Format14;

	};	// end of class HY_DLL_CLASS CMAP_TABLE_ENTRY	

	class HY_DLL_CLASS CHYCmap
	{
	public:
		CHYCmap(void);
		~CHYCmap(void);

		void	SetDefault();
		int		FindSpecificEntryIndex(unsigned short iFormat);
		int		FindSpecificEntryIndex(unsigned short  plat_ID,unsigned short plat_encod_ID,unsigned short iFormat);

	public:	
		unsigned short	version;
		unsigned short	numSubTable;
		std::vector<CMAP_TABLE_ENTRY>	vtCamp_tb_entry;
	};
}

