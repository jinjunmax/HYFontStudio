#ifndef HY_FONT_CODEC_DEF
#define HY_FONT_CODEC_DEF

//////////////////////////////////////////////////////////////
//
#ifdef HYFONTCODEC_EXPORTS
#define HY_DLL_CLASS __declspec(dllexport)
#else 
#define HY_DLL_CLASS __declspec(dllimport)
#endif

//////////////////////////////////////////////////////////////////////////////////
// Data Type
typedef short			FWORD;
typedef unsigned short 	UFWORD;
typedef short			F2Dot14;        /* 2.14 fixed point number used for unit vectors */
typedef long			F26Dot6;		/* 26.6 fixed point number to specify pixel positions */

#ifndef HY_FIXED_STRUCT
#define HY_FIXED_STRUCT
typedef struct HY_FIXED 
{
	short			value;
	unsigned short	fract;	
} HYFIXED;
#endif

#ifndef HY_POINTF
#define HY_POINTF
typedef struct tagHYPOINTF
{
	double  x;
	double  y;
} HYPOINTF;
#endif


//If set, this axis value table provides axis value information that is applicable to other fonts within the same font family. 
//This is used if the other fonts were released earlier and did not include information about values for some axis. 
//If newer versions of the other fonts include the information themselves and are present, 
//then this record is ignored.
#ifndef OLDER_SIBLING_FONT_ATTRIBUTE
#define OLDER_SIBLING_FONT_ATTRIBUTE	0x0001
#endif

//If set, it indicates that the axis value represents 
//the ��normal�� value for the axis and may be omitted 
//when composing name strings.
#ifndef ELIDABLE_AXIS_VALUE_NAME
#define ELIDABLE_AXIS_VALUE_NAME		0x0002
#endif

typedef int BOOL;

#ifndef HY_OPTION_STRUCT
#define HY_OPTION_STRUCT
typedef struct _HY_OPTION_PRM
{
	BOOL 			bCmplVert;		// �Ƿ����±������ű�	
	BOOL			bsetADW;		// �������ĺ��ſ��
	unsigned short	usSetADW;		// ���ֵ	
	BOOL			bRePsName;		// ���������������ε�name
	BOOL			bReSortUni;		// �ֿⰴunicode��������
	BOOL			bCmplCMAP;		// �Ƿ����±���CMAP
	BOOL			bYitizi;		// �Ƿ����CJK�����ַ�
	BOOL			bOldStandard;	// �Ƿ���ݾɱ�׼
	BOOL			bDelOld;		// �Ƿ�ɾ���ɱ�׼��bOldStandard����
	BOOL			bKangXi;		// �Ƿ���ݿ�������
	BOOL			bCnturCorrect;	// �Ƿ�����������	
	BOOL			bFontname;		// �Ƿ���Ҫ���������ֿ�����	
	BOOL			bCmplLayout;	// �Ƿ���Ҫ���±���layout���Ŀǰ���빦����δʵ�֣�
	BOOL			bHanyi;			// �Ƿ�ִ��Hanyi�ڲ�������׼
	unsigned short	usUnitsPerEM;	// �趨�ֿ��EM
	short			sTypo_Ascender;	
	short			sTypo_Descender;
	unsigned short	usWeight;
	unsigned short	usWidth;
	unsigned long	ulCodePageRang1;
	unsigned long	ulCodePageRang2;
	BOOL			bCIDFnt;
	char			Registry[MAX_PATH];
	char			Ordering[MAX_PATH];
	char			Supplement[MAX_PATH];
	char			CHSCpyright[MAX_PATH];
	char			ENGCpyright[MAX_PATH];
	char			CHSTrademark[MAX_PATH];
	char			ENGTrademark[MAX_PATH];
	char			CHSFaimlyName[MAX_PATH];
	char			CHSSubName[MAX_PATH];
	char			ENGFaimlyName[MAX_PATH];
	char			ENGSubName[MAX_PATH];
	char			Version[MAX_PATH];

} HY_OPTION_PRM;
#endif


#ifndef HY_MAKE_GLYF_STRUCT
#define HY_MAKE_GLYF_STRUCT
typedef struct _HY_MAKE_GLYF_PRM
{
	int		FontType;
	double  dbPrecision;
	int		iAvrdis;
	int		iDisWidth;

} HY_MAKE_GLYF_PRM;
#endif

#ifndef _HY_SCAN_PRM
#define _HY_SCAN_PRM
typedef struct _HY_SCAN_PRM
{
	int iTopEdge;
	int iLeftEdge;
	int iBottomEdge;
	int iRightEdge;
	int iGrayThre;
	int iScanDirection;		// 0 ����ɨ�� 1 ����ɨ��
	int iRows;
	int iColmns;
	int iAvrdis;
	int iDisWidth;
	int iSingleBorder;
	int iGridThres;			// ��ֱ��ͼ���������ж���ֵ
	int iGridLenThres;		// �ж�����ֵ֮�µ���ֵ������������ֵ
	int iGridImgNumber;
	int	iSmoothParam;		// ƽ�������趨
	char szGridImgDir[MAX_PATH];
} HY_SCAN_PRM;
#endif

#ifndef _HY_IMG_OPTIM
#define _HY_IMG_OPTIM
typedef struct _HY_IMG_OPTIM
{
	int		Light;				//����
	int		Contrast;			//�Աȶ�
	bool	InVert;				//�Ƿ�ɫ
	bool	bMono;				//�Ƿ�ת��Ϊ�ڰ�ͼ
	int		MonoThres;			//�ڰ�ͼ����ֵ
	int		BKType;				//��������	0 ��ɫ���� 1 ��ɫ����
	
} HY_IMG_OPTIM;
#endif

//EntryFormat Field Masks
#define INNER_INDEX_BIT_COUNT_MASK	0x000F
#define MAP_ENTRY_SIZE_MASK			0x0030

/////////////////////////////////////////////////////////////////////////////////
// Font Type
#define FONTTYPE_UNKNOWN		-1
#define FONTTYPE_BASE			0
#define FONTTYPE_TTF			(FONTTYPE_BASE+1)
#define FONTTYPE_OTF			(FONTTYPE_BASE+2)
#define FONTTYPE_XML			(FONTTYPE_BASE+3)

////////////////////////////////////////////////////////////////////////////////
// POINT Type
#define POINT_FLG_UNKNOW		0xFF
#define POINT_FLG_ANCHOR		0x01
#define POINT_FLG_CONTROL		0x00

/////////////////////////////////////////////////////////////////////////////// 
// TABLE Type  version	1.0

#define TTC_TAG			0x74746366
#define AVAR_TAG		0x61766172
#define BASE_TAG		0x42415345
#define CBDT_TAG		0x43424454
#define CBLC_TAG		0x43424C43
#define CFF_TAG			0x43464620
#define CFF2_TAG		0x43464632
#define CMAP_TAG		0x636D6170
#define COLR_TAG		0x434F4C52
#define CPAL_TAG		0x4350414C
#define	CVAR_TAG		0x63766172
#define CVT_TAG			0x63767420
#define DSIG_TAG 		0x44534947
#define EBDT_TAG		0x45424454
#define EBLC_TAG		0x45424C43
#define EBSC_TAG		0x45425343
#define FPGM_TAG		0x6670676D
#define FVAR_TAG		0x66766172
#define GASP_TAG		0x67617370
#define GDEF_TAG		0x47444546
#define GLYF_TAG		0x676C7966
#define GPOS_TAG		0x47504F53
#define GSUB_TAG		0x47535542
#define GVAR_TAG		0x67766172
#define HDMX_TAG		0x68646D78
#define HEAD_TAG		0x68656164
#define HHEA_TAG		0x68686561
#define	HMTX_TAG		0x686D7478
#define HVAR_TAG		0x48564152
#define JSTF_TAG		0x4A535446
#define KERN_TAG		0x6B65726E
#define LOCA_TAG		0x6C6F6361
#define LTSH_TAG		0x4C545348
#define MATH_TAG		0x4D415448
#define MAXP_TAG		0x6D617870
#define MERG_TAG		0x4D455247
#define META_TAG		0x6D657461
#define MVAR_TAG		0x4D564152
#define NAME_TAG		0x6E616D65
#define OS2_TAG 		0x4F532F32
#define PCLT_TAG		0x50434C54
#define POST_TAG		0x706F7374
#define PREP_TAG		0x70726570
#define SBIX_TAG		0x73626978
#define STAT_TAG		0x53544154
#define SVG_TAG			0x53564720
#define VDMX_TAG		0x56444D58
#define VHEA_TAG		0x76686561
#define VMTX_TAG		0x766D7478
#define VORG_TAG		0x564F5247
#define VVAR_TAG		0x56564152
#define FEAT_TAG		0x66656174
#define MORX_TAG		0x6D6F7278

/////////////////////////////////////////////////
// miss char
#define MISS_CHARA		0xFFFF

////////////////////////////////////////////////
//CMAP FLAG
#define CMAP_PLATFORMID_UNICODE						0
#define CMAP_PLATFORMID_MACINTOSH					1
#define CMAP_PLATFORMID_ISO							2
#define CMAP_PLATFORMID_WINDOWS						3
#define CMAP_PLATFORMID_CUSTOM						4

#define CMAP_ENCODEINGID_UNICODE_1D0				0
#define CMAP_ENCODEINGID_UNICODE_1D1				1
#define CMAP_ENCODEINGID_UNICODE_10646				2
#define CMAP_ENCODEINGID_UNICODE_BMP20				3
#define CMAP_ENCODEINGID_UNICODE_FULL20				4
#define CMAP_ENCODEINGID_UNICODE_VARIATION			5
#define CMAP_ENCODEINGID_UNICODE_FULL				6

#define CMAP_ENCODEINGID_WINDOWS_SYMBOL				0
#define CMAP_ENCODEINGID_WINDOWS_BMP				1
#define CMAP_ENCODEINGID_WINDOWS_SHIFTJIS			2
#define CMAP_ENCODEINGID_WINDOWS_PRC				3
#define CMAP_ENCODEINGID_WINDOWS_BIG5				4
#define CMAP_ENCODEINGID_WINDOWS_WANSUNG			5
#define CMAP_ENCODEINGID_WINDOWS_JOHAB				6
#define CMAP_ENCODEINGID_WINDOWS_RESERVED1			7
#define CMAP_ENCODEINGID_WINDOWS_RESERVED2			8
#define CMAP_ENCODEINGID_WINDOWS_RESERVED3			9
#define CMAP_ENCODEINGID_WINDOWS_UCS4				10

#define CMAP_ENCODEINGID_MAC_ROMAN					0
#define CMAP_ENCODEINGID_MAC_JAPANESE				1
#define CMAP_ENCODEINGID_MAC_CHINESE_TRADITIONAL	2
#define CMAP_ENCODEINGID_MAC_KOREAN					3
#define CMAP_ENCODEINGID_MAC_ARABIC					4
#define CMAP_ENCODEINGID_MAC_HEBREW					5
#define CMAP_ENCODEINGID_MAC_GREEK					6
#define CMAP_ENCODEINGID_MAC_RUSSIAN				7
#define CMAP_ENCODEINGID_MAC_RSYMBOL				8
#define CMAP_ENCODEINGID_MAC_DEVANAGARI				9
#define CMAP_ENCODEINGID_MAC_GURMUKRI				10
#define CMAP_ENCODEINGID_MAC_GUJARATI				11
#define CMAP_ENCODEINGID_MAC_ORIYA					12
#define CMAP_ENCODEINGID_MAC_BENGALI				13
#define CMAP_ENCODEINGID_MAC_TAMIL					14
#define CMAP_ENCODEINGID_MAC_TELUGU					15
#define CMAP_ENCODEINGID_MAC_KANNADA				16
#define CMAP_ENCODEINGID_MAC_MALAYALAM				17
#define CMAP_ENCODEINGID_MAC_SINHALESE				18
#define CMAP_ENCODEINGID_MAC_BURMESE				19
#define CMAP_ENCODEINGID_MAC_KHMER					20
#define CMAP_ENCODEINGID_MAC_THAI					21
#define CMAP_ENCODEINGID_MAC_LAOTIAN				22
#define CMAP_ENCODEINGID_MAC_GEORGIAN				23
#define CMAP_ENCODEINGID_MAC_ARMENIAN				24
#define CMAP_ENCODEINGID_MAC_CHINESE_SIMPLIFIED 	25
#define CMAP_ENCODEINGID_MAC_TIBETAN 				26
#define CMAP_ENCODEINGID_MAC_MONGOLIAN 				27
#define CMAP_ENCODEINGID_MAC_GEEZ 					28
#define CMAP_ENCODEINGID_MAC_SLAVIC 				29
#define CMAP_ENCODEINGID_MAC_VIETNAMESE 			30
#define CMAP_ENCODEINGID_MAC_SINDHI 				31
#define CMAP_ENCODEINGID_MAC_UNINTERPRETED			32

#define CMAP_ENCODE_FT_0							0
#define CMAP_ENCODE_FT_2							2
#define CMAP_ENCODE_FT_4							4
#define CMAP_ENCODE_FT_6							6
#define CMAP_ENCODE_FT_8							8
#define CMAP_ENCODE_FT_10							10
#define CMAP_ENCODE_FT_12							12
#define CMAP_ENCODE_FT_13							13
#define CMAP_ENCODE_FT_14							14

//////////////////////////////////////////////////
//GLYF TABLE FLAG
#define		GLYF_COORD_BYTE								0
#define		GLYF_COORD_SHORT							1
#define		GLYF_COORD_ZERO								2

#define		GLYF_SMPL_ONCURVE							0x0001
#define		GLYF_SMPL_X_VECTOR							0x0002
#define		GLYF_SMPL_Y_VECTOR							0x0004
#define		GLYF_SMPL_REPEAT							0x0008
#define		GLYF_SMPL_X_SAME_FLG						0x0010
#define		GLYF_SMPL_Y_SAME_FLG						0x0020

#define		GLYF_CMPST_ARG_1_AND_2_ARE_WORDS			0x0001
#define		GLYF_CMPST_ARGS_ARE_XY_VALUES				0x0002
#define		GLYF_CMPST_ROUND_XY_TO_GRID					0x0004
#define		GLYF_CMPST_WE_HAVE_A_SCALE					0x0008
#define		GLYF_CMPST_RESERVE							0x0010
#define		GLYF_CMPST_MORE_COMPONENT					0x0020
#define		GLYF_CMPST_WE_HAVE_AN_X_AND_Y_SCALE			0x0040
#define		GLYF_CMPST_WE_HAVE_A_TWO_BY_TWO				0x0080
#define		GLYF_CMPST_WE_HAVE_INSTRUCTIONS				0x0100
#define		GLYF_CMPST_USE_MY_METRICS					0x0200
#define		GLYF_CMPST_OVERLAP_COMPOUND					0x0400
#define		GLYF_CMPST_SCALED_COMPONENT_OFFSET			0x0800
#define		GLYF_CMPST_UNSCALED_COMPONENT_OFFSET		0x1000

////////////////////////////////////////////////////////
// LOCA TABLE
#define		LOCA_INDEXTOLOCFORMAT_SHORT					0
#define		LOCA_INDEXTOLOCFORMAT_LONG					1

/////////////////////////////////////////////////////////////////////
// GASP TABLE
#define GASP_GRIDFIT				0x0001
#define GASP_DOGRAY					0x0002
#define GASP_SYMMETRIC_GRIDFIT		0x0004	//Only supported in version 1 gasp
#define GASP_SYMMETRIC_SMOOTHING	0x0008	//Only supported in version 1 gasp 

////////////////////////////////////////////////////////////
//Registered design-variation axis tag
#define		AXIS_TAG_WGHT	0x77676874
#define		AXIS_TAG_WDTH	0x77647468
#define		AXIS_TAG_ITAL	0x6974616C
#define		AXIS_TAG_OPSZ	0x6F70737A
#define		AXIS_TAG_SLNT	0x736C6E74

#define		EMBEDDED_PEAK_TUPLE		0x8000
#define		INTERMEDIATE_REGION		0x4000
#define		PRIVATE_POINT_NUMBERS	0x2000
#define		TUPLE_INDEX_MASK		0x0FFF

#define		SHARED_POINT_NUMBERS	0x8000	
#define		COUNT_MASK				0x0FFF

#define		POINTS_ARE_WORDS		0x80
#define		POINT_RUN_COUNT_MASK	0x7F

#define		DELTAS_ARE_ZERO			0x80
#define		DELTAS_ARE_WORDS		0x40
#define		DELTA_RUN_COUNT_MASK	0x3F

#define		AXES_WIDTH				"wdth"
#define		AXES_WEIGHT				"wght"

#define		HY_CHARARRY_MAX			2048

// 
/*
#define		BASICLATIN	0 //0000-007F


#define	 	LATIN1SUPPLEMENT	1 //0080-00FF
#define	 	LATINEXTENDEDA		2//0100-017F
#define 3	LATIN EXTENDED-B	0180-024F
#define 4	IPA EXTENSIONS	0250-02AF
#define PHONETIC EXTENSIONS	1D00-1D7F
#define PHONETIC EXTENSIONS SUPPLEMENT	1D80-1DBF
#define 5	SPACING MODIFIER LETTERS	02B0-02FF
#define MODIFIER TONE LETTERS	A700-A71F
#define 6	COMBINING DIACRITICAL MARKS	0300-036F
#define COMBINING DIACRITICAL MARKS SUPPLEMENT	1DC0-1DFF
#define 7	GREEK AND COPTIC	0370-03FF
#define 8	COPTIC	2C80-2CFF
#define 9	CYRILLIC	0400-04FF
#define CYRILLIC SUPPLEMENT	0500-052F
#define CYRILLIC EXTENDED-A	2DE0-2DFF
#define CYRILLIC EXTENDED-B	A640-A69F
#define 10	ARMENIAN	0530-058F
#define 11	HEBREW	0590-05FF
#define 12	VAI	A500-A63F
#define 13	ARABIC	0600-06FF
#define ARABIC SUPPLEMENT	0750-077F
#define 14	NKO	07C0-07FF
#define 15	DEVANAGARI	0900-097F
#define 16	BENGALI	0980-09FF
#define 17	GURMUKHI	0A00-0A7F
#define 18	GUJARATI	0A80-0AFF
#define 19	ORIYA	0B00-0B7F
#define 20	TAMIL	0B80-0BFF
#define 21	TELUGU	0C00-0C7F
#define 22	KANNADA	0C80-0CFF
#define 23	MALAYALAM	0D00-0D7F
#define 24	THAI	0E00-0E7F
#define 25	LAO	0E80-0EFF
#define 26	GEORGIAN	10A0-10FF
#define GEORGIAN SUPPLEMENT	2D00-2D2F
#define 27	BALINESE	1B00-1B7F
#define 28	HANGUL JAMO	1100-11FF
#define 29	LATIN EXTENDED ADDITIONAL	1E00-1EFF
#define LATIN EXTENDED-C	2C60-2C7F
#define LATIN EXTENDED-D	A720-A7FF
#define 30	GREEK EXTENDED	1F00-1FFF
#define 31	GENERAL PUNCTUATION	2000-206F
#define SUPPLEMENTAL PUNCTUATION	2E00-2E7F
#define 32	SUPERSCRIPTS AND SUBSCRIPTS	2070-209F
#define 33	CURRENCY SYMBOLS	20A0-20CF
#define 34	COMBINING DIACRITICAL MARKS FOR SYMBOLS	20D0-20FF
#define 35	LETTERLIKE SYMBOLS	2100-214F
#define 36	NUMBER FORMS	2150-218F
#define 37	ARROWS	2190-21FF
#define SUPPLEMENTAL ARROWS-A	27F0-27FF
#define SUPPLEMENTAL ARROWS-B	2900-297F
#define MISCELLANEOUS SYMBOLS AND ARROWS	2B00-2BFF
#define 38	MATHEMATICAL OPERATORS	2200-22FF
#define SUPPLEMENTAL MATHEMATICAL OPERATORS	2A00-2AFF
#define MISCELLANEOUS MATHEMATICAL SYMBOLS-A	27C0-27EF
#define MISCELLANEOUS MATHEMATICAL SYMBOLS-B	2980-29FF
#define 39	MISCELLANEOUS TECHNICAL	2300-23FF
#define 40	CONTROL PICTURES	2400-243F
#define 41	OPTICAL CHARACTER RECOGNITION	2440-245F
#define 42	ENCLOSED ALPHANUMERICS	2460-24FF
#define 43	BOX DRAWING	2500-257F
#define 44	BLOCK ELEMENTS	2580-259F
#define 45	GEOMETRIC SHAPES	25A0-25FF
#define 46	MISCELLANEOUS SYMBOLS	2600-26FF
#define 47	DINGBATS	2700-27BF
#define 48	CJK SYMBOLS AND PUNCTUATION	3000-303F
#define 49	HIRAGANA	3040-309F
#define 50	KATAKANA	30A0-30FF
#define KATAKANA PHONETIC EXTENSIONS	31F0-31FF
#define 51	BOPOMOFO	3100-312F
#define BOPOMOFO EXTENDED	31A0-31BF
#define 52	HANGUL COMPATIBILITY JAMO	3130-318F
#define 53	PHAGS-PA	A840-A87F
#define 54	ENCLOSED CJK LETTERS AND MONTHS	3200-32FF
#define 55	CJK COMPATIBILITY	3300-33FF
#define 56	HANGUL SYLLABLES	AC00-D7AF
#define 57	NON-PLANE 0 *	D800-DFFF
#define 58	PHOENICIAN	10900-1091F
#define 59	CJK UNIFIED IDEOGRAPHS	4E00-9FFF
#define CJK RADICALS SUPPLEMENT	2E80-2EFF
#define KANGXI RADICALS	2F00-2FDF
#define IDEOGRAPHIC DESCRIPTION CHARACTERS	2FF0-2FFF
#define CJK UNIFIED IDEOGRAPHS EXTENSION A	3400-4DBF
#define CJK UNIFIED IDEOGRAPHS EXTENSION B	20000-2A6DF
#define KANBUN	3190-319F
#define 60	PRIVATE USE AREA (PLANE 0)	E000-F8FF
#define 61	CJK STROKES	31C0-31EF
#define CJK COMPATIBILITY IDEOGRAPHS	F900-FAFF
#define CJK COMPATIBILITY IDEOGRAPHS SUPPLEMENT	2F800-2FA1F
#define 62	ALPHABETIC PRESENTATION FORMS	FB00-FB4F
#define 63	ARABIC PRESENTATION FORMS-A	FB50-FDFF
#define 64	COMBINING HALF MARKS	FE20-FE2F
#define 65	VERTICAL FORMS	FE10-FE1F
#define CJK COMPATIBILITY FORMS	FE30-FE4F
#define 66	SMALL FORM VARIANTS	FE50-FE6F
#define 67	ARABIC PRESENTATION FORMS-B	FE70-FEFF
#define 68	HALFWIDTH AND FULLWIDTH FORMS	FF00-FFEF
#define 69	SPECIALS	FFF0-FFFF
#define 70	TIBETAN	0F00-0FFF
#define 71	SYRIAC	0700-074F
#define 72	THAANA	0780-07BF
#define 73	SINHALA	0D80-0DFF
#define 74	MYANMAR	1000-109F
#define 75	ETHIOPIC	1200-137F
#define ETHIOPIC SUPPLEMENT	1380-139F
#define ETHIOPIC EXTENDED	2D80-2DDF
#define 76	CHEROKEE	13A0-13FF
#define 77	UNIFIED CANADIAN ABORIGINAL SYLLABICS	1400-167F
#define 78	OGHAM	1680-169F
#define 79	RUNIC	16A0-16FF
#define 80	KHMER	1780-17FF
#define KHMER SYMBOLS	19E0-19FF
#define 81	MONGOLIAN	1800-18AF
#define 82	BRAILLE PATTERNS	2800-28FF
#define 83	YI SYLLABLES	A000-A48F
#define YI RADICALS	A490-A4CF
#define 84	TAGALOG	1700-171F
#define HANUNOO	1720-173F
#define BUHID	1740-175F
#define TAGBANWA	1760-177F
#define 85	OLD ITALIC	10300-1032F
#define 86	GOTHIC	10330-1034F
#define 87	DESERET	10400-1044F
#define 88	BYZANTINE MUSICAL SYMBOLS	1D000-1D0FF
#define MUSICAL SYMBOLS	1D100-1D1FF
#define ANCIENT GREEK MUSICAL NOTATION	1D200-1D24F
#define 89	MATHEMATICAL ALPHANUMERIC SYMBOLS	1D400-1D7FF
#define 90	PRIVATE USE (PLANE 15)	FF000-FFFFD
#define PRIVATE USE (PLANE 16)	100000-10FFFD
#define 91	VARIATION SELECTORS	FE00-FE0F
#define VARIATION SELECTORS SUPPLEMENT	E0100-E01EF
#define 92	TAGS	E0000-E007F
#define 93	LIMBU	1900-194F
#define 94	TAI LE	1950-197F
#define 95	NEW TAI LUE	1980-19DF
#define 96	BUGINESE	1A00-1A1F
#define 97	GLAGOLITIC	2C00-2C5F
#define 98	TIFINAGH	2D30-2D7F
#define 99	YIJING HEXAGRAM SYMBOLS	4DC0-4DFF
#define 100	SYLOTI NAGRI	A800-A82F
#define 101	LINEAR B SYLLABARY	10000-1007F
#define LINEAR B IDEOGRAMS	10080-100FF
#define AEGEAN NUMBERS	10100-1013F
#define 102	ANCIENT GREEK NUMBERS	10140-1018F
#define 103	UGARITIC	10380-1039F
#define 104	OLD PERSIAN	103A0-103DF
#define 105	SHAVIAN	10450-1047F
#define 106	OSMANYA	10480-104AF
#define 107	CYPRIOT SYLLABARY	10800-1083F
#define 108	KHAROSHTHI	10A00-10A5F
#define 109	TAI XUAN JING SYMBOLS	1D300-1D35F
#define 110	CUNEIFORM	12000-123FF
#define CUNEIFORM NUMBERS AND PUNCTUATION	12400-1247F
#define 111	COUNTING ROD NUMERALS	1D360-1D37F
#define 112	SUNDANESE	1B80-1BBF
#define 113	LEPCHA	1C00-1C4F
#define 114	OL CHIKI	1C50-1C7F
#define 115	SAURASHTRA	A880-A8DF
#define 116	KAYAH LI	A900-A92F
#define 117	REJANG	A930-A95F
#define 118	CHAM	AA00-AA5F
#define 119	ANCIENT SYMBOLS	10190-101CF
#define 120	PHAISTOS DISC	101D0-101FF
#define 121	CARIAN	102A0-102DF
#define LYCIAN	10280-1029F
#define LYDIAN	10920-1093F
#define 122	DOMINO TILES	1F030-1F09F
#define MAHJONG TILES	1F000-1F02F
#define 123-127	RESERVED FOR PROCESS-INTERNAL USAGE
*/

#endif