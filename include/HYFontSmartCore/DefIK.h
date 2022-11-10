#ifndef	IK_DEFINE_HEAD
#define IK_DEFINE_HEAD

#ifndef IK_FILENAME_LENG
	#define IK_FILENAME_LENG	7 << 1
#endif 

#ifndef IK_FONTNAME_LENG
#define IK_FONTNAME_LENG	39 << 1
#endif 

#ifndef IK_PT_FLG_START
#define IK_PT_FLG_START		12
#endif 

#ifndef IK_PT_FLG_CORNER
#define IK_PT_FLG_CORNER	13
#endif 

#ifndef IK_PT_FLG_CURVE
#define IK_PT_FLG_CURVE		14
#endif 

#ifndef IK_PT_FLG_TANGENT
#define IK_PT_FLG_TANGENT	15
#endif 


#define VALUE				15
#define VALUEHT				25
#define NORMAL_WIDTH		3000
#define	VALUE25				30
#define	VALUE20				25
#define	VALUE30				35

#define IK_DTFMT_UNKOWN			0
#define IK_DTFMT_BIG_ENDIAN		1
#define IK_DTFMT_LITTLE_ENDIAN	2

typedef struct _IK_NAME_INF
{
	unsigned short		usNameLength;		// 文件中Name部分的长度，不是结构体的大小
	unsigned short		usURWLstNum; 
	unsigned char		FileName[IK_FILENAME_LENG];			
	unsigned char		FontName[IK_FONTNAME_LENG];
	unsigned short		usDataFormat;		// IK	
	unsigned short		usProductDate[3];	// production date	
	unsigned short		usChangeDate[3];	// Change date	

} IK_NAME_INF;

typedef struct _IK_FONT_INF
{
	unsigned short		usFontLength;		// length of font information, 不是结构体的大小
	unsigned short		usIndicator;		// indicator for font
	unsigned short		usCharNum;			// number of characters
	unsigned short		usCapHeight;		// cap height
	unsigned short		usBodySize;			// body Size
	unsigned short		usX_Height;			// x-height
	unsigned short		usBaseLine;			// Distance base line - lower body line
	unsigned short		usTextLine;			// Text line distance for setting
	unsigned short		usStemThickness;	// stem thickness
	unsigned short		usAngle;			// Angle of Italisation
	unsigned short		usOptimum;			// optimum point size for setting
	unsigned short		usAverageWidth;		// Average character width

} IK_FONT_INF;

typedef struct _IK_CHARACTER_INDEX
{
	unsigned short		usCharNum;			// character number 
	unsigned short		usStartRecord;		// pointer to record containing start of character
	unsigned short		usWordPoint;		// word pointer to start of character	
	unsigned long		ulCharOffset;		// 从文件头开始的偏移量

} IK_CHARACTER_INDEX, *PIK_CHARACTER_INDEX;

typedef struct _IK_CHARACTER_INF
{
	unsigned short		usCharInfLength;	// length of character Index
	unsigned short		usPhysicalLength;	// length of font in Pyhsical records
	unsigned short		usLastWord;			// last word of last record
	unsigned short		usIndexNum;
	PIK_CHARACTER_INDEX	pCharaterIndex;	

} IK_CHARACTER_INF, *PIK_CHARACTER_INF;

typedef struct  _IK_HEAD
{
	unsigned short		usIKLength;
	IK_NAME_INF			tagName;
	IK_FONT_INF			tagIKFontInf;
	unsigned short		usHierarchy;
	IK_CHARACTER_INF	tagCharacterInf;

} IK_HEAD, *PIK_HEAD;

typedef struct _IK_CHAR_NAME_INF
{	
	unsigned short		usNameLength;	// length of name section
	unsigned short		usCharacterNo;	// character number
	unsigned short		usfollowNumber;	// number of following part

} IK_CHAR_NAME_INF, *PIK_CHAR_NAME_INF;

typedef struct _IK_CHAR_SETTING_INF
{	
	 short		usSetLength;		// length of setting information
	 short		usCharacterType;	// character type
	 short		usDigitiNumber;		// number of digitizations
	 short		usTotalWidth;		// Total set width (T=L+W+R)
	 short		usLeftBearing;		// Left side bearing
	 short		usWidth;			// width(W)
	 short		usRightBearing;		// Rgiht side bearing
	 short		usXMin;				// x-minimum;
	 short		usXMax;				// x-maximum;
	 short		usYMin;				// Y-minimum;
	 short		usYMax;				// Y-maximum;
	 short		usUnit;				// Unit;
	
} IK_CHAR_SETTING_INF;

typedef struct _IK_CHAR_CONTOUR_INDEX
{	
	unsigned short		usRecordsPointer;	 
	unsigned short		usWordPointer;		// words	
	unsigned short		usRotation;			// sense of rotation of contour
	unsigned short		usNest;				// Nesting
	unsigned short		usColor;			// colour inside
	unsigned short		usDigitiNumber;		// number of digitization
	unsigned short		usStartPointOffset;

} IK_CHAR_CONTOUR_INDEX, *PIK_CHAR_CONTOUR_INDEX;

typedef struct _IK_CHAR_CONTOUR_INF
{
	unsigned short			usRecords;
	unsigned short			usWords;			// words;
	unsigned short			usContourNum;
	PIK_CHAR_CONTOUR_INDEX	pContourIndex;

} IK_CHAR_CONTOUR_INF, *pIK_CHAR_CONTOUR_INF;

typedef struct _IK_POINT
{
	 short usSrcX;		// 文件中的原始x点
	 short usSrcY;		// 文件中的原始y点

	 short usRealX;		// 计算后的x点	
	 short usRealY;		// 计算后的Y点
	 short usK;			// 计算后的点的属性

	 short vFlag;		// 1 for vertical flag, 0 for not vertical, for tangent direction, (angle1+angle2)/2
	 float tan;			// tangent direction = Ysin/Xcos

}	IK_POINT, *PIK_POINT;

typedef struct _IK_IMAGE_INF
{
	unsigned short	usRecord;
	unsigned short	usWords;
	unsigned short  usPointNum;
	PIK_POINT		pIKPoint;

} IK_IMAGE_INF, *PIK_IMAGE_INF;

#define IK_UNICODE_LEN			2
typedef struct _IK_SEGMENT_PROBLEM
{
	short			sBegin;
	short			sEnd;

} IK_SEGMENT_PROBLEM;

#define IK_CHAR_SEGMENT_SIZE 8
typedef struct _IK_CHARACTER
{
	unsigned short				usRecords;		// Records length of character
	unsigned short				usWords;		// words;

	unsigned long				szUnicode[IK_UNICODE_LEN];
	unsigned long				RealUnicodeLen;

	IK_CHAR_NAME_INF			tagCharNameinf;
	IK_CHAR_SETTING_INF			tagCharSetting;
	IK_CHAR_CONTOUR_INF			tagContourInfo;
	IK_IMAGE_INF				tagImageInfo;
	IK_SEGMENT_PROBLEM			szSegment[IK_CHAR_SEGMENT_SIZE];
	int							iRealSegmentSize;

} IK_CHARACTER, *PIK_CHARACTER;

typedef struct _IK_FONT
{
	IK_HEAD			tagHead;	
	PIK_CHARACTER	pCharacter;

} IK_FONT, *PIK_FONT;

#define IK_RECORD	2048

#endif 