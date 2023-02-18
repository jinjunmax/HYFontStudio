#pragma once

namespace HYFONTCODEC
{
	class HY_DLL_CLASS BigGlyphMetrics
	{
		public:
			BigGlyphMetrics();
			~BigGlyphMetrics();

			void SetDefault();

		public:
			unsigned char 	height;
			unsigned char 	width;
			char 			horiBearingX;
			char			horiBearingY;
			unsigned char 	horiAdvance;
			char			vertBearingX;
			char			vertBearingY;
			unsigned char	vertAdvance;
	};

	class HY_DLL_CLASS SmallGlyphMetrics
	{
	public:
		SmallGlyphMetrics();
		~SmallGlyphMetrics();
		void SetDefault();

	public:
		unsigned char 	height;
		unsigned char 	width;		
		char			BearingX;
		char			BearingY;
		unsigned char	Advance;
	};	

	class HY_DLL_CLASS SbitLineMetrics 
	{
	public:
		SbitLineMetrics ();
		~SbitLineMetrics ();
		void Setdefault();
	public:
		char			ascender;
		char			descender;
		unsigned char	widthMax;
		char			caretSlopeNumerator;
		char			caretSlopeDenominator;
		char			caretOffset;
		char			minOriginSB;
		char			minAdvanceSB;
		char			maxBeforeBL;
		char			minAfterBL;
		char			pad1;
		char			pad2;

	};	// end of class HY_DLL_CLASS SbitLineMetrics 

	class HY_DLL_CLASS IndexSubHeader
	{
	public:
		IndexSubHeader();
		~IndexSubHeader();

		void Setdefault();
	public:
		unsigned short	indexFormat;		//format of this indexSubTable
		unsigned short 	imageFormat;		//format of 'EBDT' image data
		unsigned long	imageDataOffset;	//offset to image data in 'EBDT' table

	};	// end of class HY_DLL_CLASS IndexSubHeader

	class HY_DLL_CLASS IndexSubTable1
	{
		public:
			IndexSubTable1();
			~IndexSubTable1();
			void Setdefault();
		public:			
			std::vector<unsigned long> vtsbitOffsets;			

	};	//end of class HY_DLL_CLASS IndexSubTable1

	class HY_DLL_CLASS IndexSubTable2
	{
		public:
			IndexSubTable2();
			~IndexSubTable2();

			void Setdefault();
		public:			
			unsigned long				ulImageSize;
			BigGlyphMetrics				bigMetrics;

	};	//end of class HY_DLL_CLASS IndexSubTable2

	class HY_DLL_CLASS IndexSubTable3
	{
		public:
			IndexSubTable3();
			~IndexSubTable3();

			void Setdefault();
		public:		
			std::vector<unsigned short> vtOffsetArray;			

	};	// end of class HY_DLL_CLASS IndexSubTable3	

	class HY_DLL_CLASS GlyphIdOffsetPair
	{
	public:
		GlyphIdOffsetPair();
		~GlyphIdOffsetPair();

		void SetDefault();
	public:
		unsigned short	glyphID;	// Glyph ID of glyph present.
		unsigned short 	sbitOffset;	//Location in EBDT.

	};	// end of class HY_DLL_CLASS GlyphIdOffsetPair

	class HY_DLL_CLASS IndexSubTable4
	{
		public:
			IndexSubTable4();
			~IndexSubTable4();
			void Setdefault();
		public:
			unsigned long 					numGlyphs;	//	Array length.			
			std::vector<GlyphIdOffsetPair>	vtGlyphArray;

	};	//end of class HY_DLL_CLASS IndexSubTable4	

	class HY_DLL_CLASS IndexSubTable5
	{
		public:
			IndexSubTable5();
			~IndexSubTable5();

			void Setdefault();
		public:		
			unsigned long				imageSize;			//all glyphs have the same data size
			BigGlyphMetrics				bigMetrics;			//all glyphs have the same metrics
			unsigned long				numGlyphs;			//array length
			std::vector<unsigned short>	vtGlyphIDArray;	//one per glyph, sorted by glyph code; sizeOfArray=numGlyphs

	};	// end of class HY_DLL_CLASS IndexSubTable5

	class HY_DLL_CLASS CBDTFormat1
	{
	public:
		CBDTFormat1();
		~CBDTFormat1();

		void SetDefult();

	public:
		SmallGlyphMetrics SmlGlyphMtcs;
		std::vector<unsigned char> imagedata;

	};	// end of class HY_DLL_CLASS CBDTFormat1

	class HY_DLL_CLASS CBDTFormat2
	{
	public:
		CBDTFormat2();
		~CBDTFormat2();

		void SetDefult();

	public:
		SmallGlyphMetrics SmlGlyphMtcs;
		std::vector<unsigned char> imagedata;

	};	// end of class HY_DLL_CLASS CBDTFormat2

	class HY_DLL_CLASS CBDTFormat5
	{
	public:
		CBDTFormat5();
		~CBDTFormat5();

		void SetDefult();

	public:
		std::vector<unsigned char> imagedata;

	};	// end of class HY_DLL_CLASS CBDTFormat5

	class HY_DLL_CLASS CBDTFormat6
	{
	public:
		CBDTFormat6();
		~CBDTFormat6();
		void SetDefult();
	public:
		BigGlyphMetrics bigGlyphMtcs;
		std::vector<unsigned char> imagedata;

	};	// end of class HY_DLL_CLASS CBDTFormat6

	class HY_DLL_CLASS CBDTFormat7
	{
	public:
		CBDTFormat7();
		~CBDTFormat7();
		void SetDefault();
	public:
		BigGlyphMetrics bigGlyphMtcs;
		std::vector<unsigned char> imagedata;

	};	// end of class HY_DLL_CLASS CBDTFormat7

	class HY_DLL_CLASS CEbdtComponent
	{
	public:
		CEbdtComponent();
		~CEbdtComponent();
		void SetDefault();
	public:
		unsigned short glyphID;//Component glyph ID
		char 	xOffset;	//Position of component left
		char 	yOffset;	//Position of component top

	};	// end of class HY_DLL_CLASS CEbdtComponent

	class HY_DLL_CLASS CBDTFormat8
	{
	public:
		CBDTFormat8();
		~CBDTFormat8();
		void SetDefault();
	public:
		SmallGlyphMetrics smallGlyphMtcs;
		unsigned char pad;
		unsigned short numComponents;
		std::vector<CEbdtComponent> vtEbdtComponent;

	};	// end of class HY_DLL_CLASS CBDTFormat8

	class HY_DLL_CLASS CBDTFormat9
	{
	public:
		CBDTFormat9();
		~CBDTFormat9();
		void SetDefault();
	public:
		BigGlyphMetrics bigGlyphMtcs;
		unsigned char pad;
		unsigned short numComponents;
		std::vector<CEbdtComponent> vtEbdtComponent;

	};	// end of class HY_DLL_CLASS CBDTFormat9

	class HY_DLL_CLASS CBDTFormat17
	{
	public:
		CBDTFormat17();
		~CBDTFormat17();

		void SetDefault();
	public:
		SmallGlyphMetrics			smallGlyphMtcs;
		unsigned long				dataLen;
		std::vector<unsigned char>	data;

	};	// end of class HY_DLL_CLASS CBDTFormat17

	class HY_DLL_CLASS CBDTFormat18
	{
	public:
		CBDTFormat18();
		~CBDTFormat18();

		void SetDefault();
	public:
		BigGlyphMetrics				bigGlyphMtcs;
		unsigned long				dataLen;
		std::vector<unsigned char>	data;

	};	// end of class HY_DLL_CLASS CBDTFormat18

	class HY_DLL_CLASS CBDTFormat19
	{
	public:
		CBDTFormat19();
		~CBDTFormat19();

		void SetDefault();
	public:
		unsigned long				dataLen;
		std::vector<unsigned char>	data;

	};	// end of class HY_DLL_CLASS CBDTFormat19


	class HY_DLL_CLASS IndexSUBTable
	{
		public:
			IndexSUBTable();
			~IndexSUBTable();

			void Setdefault();
		public:
			IndexSubHeader	Header;

			IndexSubTable1	SubTable1;
			IndexSubTable2	SubTable2;
			IndexSubTable3	SubTable3;
			IndexSubTable4	SubTable4;
			IndexSubTable5	SubTable5;

			// ’‚¿Ô¥Ê∑≈BitmapData
			CBDTFormat1		dataFrmt1;
			CBDTFormat2		dataFrmt2;
			CBDTFormat5		dataFrmt5;
			CBDTFormat6		dataFrmt6;
			CBDTFormat7		dataFrmt7;
			CBDTFormat8		dataFrmt8;
			CBDTFormat9		dataFrmt9;
			CBDTFormat17	dataFrmt17;
			CBDTFormat18	dataFrmt18;
			CBDTFormat19	dataFrmt19;

	};	//end of class HY_DLL_CLASS IndexSUBTable	

	class HY_DLL_CLASS IndexSubTableArray
	{
	public:
		IndexSubTableArray();
		~IndexSubTableArray();

		void SetDefault();
	public:
		unsigned short 	firstGlyphIndex;		//first glyph code of this range
		unsigned short 	lastGlyphIndex;			//last glyph code of this range (inclusive)
		unsigned long	Offset;					//add to indexSubTableArrayOffset to get offset from beginning of 'EBLC'
	public:
		std::vector<IndexSUBTable>	vtIndxSubTable;

	};	// end of class HY_DLL_CLASS IndexSubTableArray

	class HY_DLL_CLASS BitmapSize
	{
		public:
			BitmapSize();
			~BitmapSize();

			void SetDefault();
		public:
			unsigned long		indexSubTableArrayOffset;	//Offset to index subtable from beginning of CBLC.
			unsigned long		indexTablesSize;			//number of bytes in corresponding index subtables and array
			unsigned long 		numberofIndexSubTables;		//an index subtable for each range or format change
			unsigned long 		colorRef;					//not used; set to 0.
			SbitLineMetrics 	Hori;						//line metrics for text rendered horizontally
			SbitLineMetrics 	Vert;						//line metrics for text rendered vertically
			unsigned short		startGlyphIndex;			//lowest glyph index for this size
			unsigned short 		endGlyphIndex;				//highest glyph index for this size
			unsigned char 		ppemX;						//horizontal pixels per Em
			unsigned char		ppemY;						//vertical pixels per Em
			unsigned char		bitDepth;					//In addtition to already defined bitDepth values 1, 2, 4, and 8 supported by existing implementations, the value of 32 is used to identify color bitmaps with 8 bit per pixel RGBA channels
			char				Flags;						//vertical or horizontal (see bitmapFlags)
			
			std::vector<IndexSubTableArray>  vtIndexSubTableArray;

	};	//end of class HY_DLL_CLASS BitmapSize

	class HY_DLL_CLASS CBLCHEADER
	{
	public:
		CBLCHEADER();
		~CBLCHEADER();

		void SetDefault();
	public:
		HYFIXED			version;
		unsigned long	numsizes;

	};	// end of class HY_DLL_CLASS CBLCHEADER

	class HY_DLL_CLASS CHYCBLC
	{
		public: 
			CHYCBLC(void);
			~CHYCBLC(void);

			void SetDefault();
		public:
			CBLCHEADER						Header;
			std::vector<BitmapSize>			vtBitmapSizeTb;			

	};	// end of class HY_DLL_CLASS CHYCBLC
	
	class HY_DLL_CLASS CHYCBDT
	{
	public:
		CHYCBDT(void);
		~CHYCBDT(void);

		void SetDefault();

	public:
		unsigned short majorVersion;	// Major version of the CBDT table, = 3. EBDT table, = 2.
		unsigned short minorVersion;	// Minor version of the CBDT table, = 0. EBDT table, = 0.		

	};	// end of class HY_DLL_CLASS CHYCBDT

}
