#ifndef  HY_GLOBAL_HEAD
#define  HY_GLOBAL_HEAD
//////////////////////////////////////////////////////////////////////////////////////////////
#define OTF_POINT_FLG_START		0x0101
#define OTF_POINT_FLG_ANCHOR	0x0102
#define OTF_POINT_FLG_CONTROL	0x0103
#define OTF_POINT_FLG_END		0x0104

///////////////////////////////////////////////////////////////////////////////////////////////////
// HY Font Define 
#define HY_CRV_FLG_UNKONW			0xFF
#define HY_CRV_FLG_ANCHOR			0x00
#define HY_CRV_FLG_ANCHOR_C			0x01
#define HY_CRV_FLG_CURVE			0x02
#define HY_CRV_FLG_ANCHOR_START		0x03	
#define HY_CRV_FLG_ANCHOR_END		0x04

#define HY_CRV_FLG_CURVE_START		0x05	
#define HY_CRV_FLG_CURVE_END		0x06

#define HY_CRV_FLG_START_MOD_X		0x07	
#define HY_CRV_FLG_START_MOD_Y		0x08
#define HY_CRV_FLG_MOD_X			0x09	
#define HY_CRV_FLG_MOD_Y			0x0a

#define HY_CRV_FLG_DELETE			0x0b
#define HY_CRV_FLG_MARKUP			0x0C

#define HY_CNTR_LINE_MAX			256
#define HY_CNTR_CURVE_MAX			256
#define HY_BZR_PT_MAX				10

#define FONT_EM						512
#define FONT_STEM					36
#define STARTPOINT					256
#define HY_POINT_MAXLEN 			4096

#define STROKE_HORIZENTAL 			0
#define STROKE_VERTICAL 			90

#define	SET_NONE					0x0
#define	SET_START					0x1
#define	SET_END						0x2
#define	SET_BOTH					0x3
#define	SET_M2						0x4
#define	SET_M2_START				0x5
#define	SET_M2_END					0x6
#define	SET_M2_BOTH					0x7

#define	SIN00						0
#define	COS00						100
#define	SIN05						9
#define	COS05						100
#define	SIN10						17
#define	COS10						98
#define	SIN15						26
#define	COS15						97
#define	SIN20						34
#define	COS20						94
#define	SIN25						42
#define	COS25						91
#define	SIN30						50
#define	COS30						87
#define	SIN35						57
#define	COS35						82
#define	SIN40						64
#define	COS40						77
#define	SIN45						71
#define	COS45						71
#define	TG00						0	
#define	TG05						9	
#define	TG10						17
#define	TG15						27
#define	TG20						36
#define	TG25						46
#define	TG30						57
#define	TG35						70
#define	TG40						83
#define	TG45						100

#define HY_COMNPANY					0x594E4148 //0x48414E59
#define HY_MISS_CHAR				0xFFFF	// 如果一个字符没有有效的unicode码，即将此值赋给字符


typedef struct	HEAD_TABLE
{
	unsigned short		unitsPerEm;
	char				created[8];	
	char				modified[8];
	short				xMin;					
	short				yMin;					
	short				xMax;					
	short				yMax;					
	unsigned short		macStyle;		
	unsigned short		lowestRecPPEM;
	short				fontDirectionHint;	
	short				indexToLocFormat;		
	short				glyphDataFormat;

} HEAD_TB;

typedef struct HYPOINT
{
	short x;
	short y;
	short flag;
	short reserve;

}HY_POINT, *LPHY_POINT;

typedef struct CPOINT
{
	short xc;
	short yc;
	short dx;
	short dy;
	short flag;

}C_POINT, *LPC_POINT;

typedef struct HY_DRAWFONT_PARAM
{
	int		iFilter;
	int		code;
	int		Param1;
	int		Param2;
	int		Param3;
	int		Param4;
	int		Param5;
	int		Param6;
	int		Param7;
	int		Param8;
	int		Param9;
	int		Param10;
}HY_DF_PARAM;

#define HY_DP_PARAM_P1		0x0001
#define HY_DP_PARAM_P2		0x0002
#define HY_DP_PARAM_P3		0x0004
#define HY_DP_PARAM_P4		0x0008
#define HY_DP_PARAM_P5		0x0010
#define HY_DP_PARAM_P6		0x0020
#define HY_DP_PARAM_P7		0x0040
#define HY_DP_PARAM_P8		0x0080
#define HY_DP_PARAM_P9		0x0100
#define HY_DP_PARAM_P10		0x0200

const int		g_szHYDFParamFilter[10] = {	HY_DP_PARAM_P1,
											HY_DP_PARAM_P2,
											HY_DP_PARAM_P3,
											HY_DP_PARAM_P4,
											HY_DP_PARAM_P5,
											HY_DP_PARAM_P6,
											HY_DP_PARAM_P7,
											HY_DP_PARAM_P8,
											HY_DP_PARAM_P9,
											HY_DP_PARAM_P10
										};


///////////////////////////////////////////////////////////////////////////////////////////////
//	File Define
#ifndef HY_FILENAME_INFO
#define HY_FILENAME_INFO
typedef struct _FILENAME_INFO
{
	int		iFileType;
	char	szDrive[20], szDir[256], szFName[256], szExt[256];
	CString	strTitle, strRawFileName, strPath;

}	FILENAME_INFO;

#endif

#define FILETYPE_UNKNOWN		-1
#define FILETYPE_BASE			0
#define FILETYPE_TTF			(FILETYPE_BASE+1)
#define FILETYPE_OTF			(FILETYPE_BASE+2)
#define FILETYPE_HY				(FILETYPE_BASE+3)
#define FILETYPE_IK				(FILETYPE_BASE+4)
#define FILETYPE_HYM			(FILETYPE_BASE+5)

////////////////////////////////////////////////////////////////////////////////////
// HY File 
#define		HYF_FONTNAME_LEN					12
#define		HYF_HEAD_RESERV_LEN					32

#define HY_RADICAL_NUMS			128
#define HY_MAP_UNICODE_NUMS		256

typedef struct _IK_CONVTER_PPARAM
{
	int		iParam0;
	int		iParam1;
	int		iParam2;
	int		iParam3;
	int		iParam4;
	int		iParam5;
	int		iParam6;
	int		iParam7;
	int		iParam8;
	int		iParam9;

} IK_CONVTER_PARAM;

typedef struct _TTF_CONVTER_PPARAM
{
	int		iParam0;
	int		iParam1;
	int		iParam2;
	int		iParam3;
	int		iParam4;
	int		iParam5;
	int		iParam6;
	int		iParam7;
	int		iParam8;
	int		iParam9;

} TTF_CONVTER_PARAM;

//#define	INST_DEBUG	1

#endif