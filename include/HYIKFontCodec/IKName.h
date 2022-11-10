#pragma once
#include "HYIkCodecDef.h"

#define IK_FILENAME_LENG		6<<1
#define IK_FONTNAME_LENG		40<<1
#define IK_PRODUCTDATE_LENG		3
#define IK_CHANGEDATE_LENG		3
namespace HYFONTCODEC
{
	class HYIKCODEC_API CIKName
	{
	public:
		CIKName(void);
		~CIKName(void);

		void				SetDefault();

		short				sNameLength;						// 文件中Name部分的长度，不是结构体的大小
		unsigned short		usURWLstNumber;						// = 0
		char				FileName[IK_FILENAME_LENG];
		char				FontName[IK_FONTNAME_LENG];
		short				sDataFormat;						// IK	
		short				sProductDate[IK_PRODUCTDATE_LENG];	// production date	
		short				sChangeDate[IK_CHANGEDATE_LENG];	// Change date	
	};

	class HYIKCODEC_API CIKFontInf
	{
	public:
		CIKFontInf(void);
		~CIKFontInf(void);

		void		SetDefault();

		short		sLength;			// length of font information
		short		sIndicator;			// indicator for font
		short		sCharNums;			// number of characters
		short		sCapHeight;			// cap height
		short		sBodySize;			// body Size
		short		sX_Height;			// x-height
		short		sBaseLine;			// Distance base line - lower body line
		short		sTextLine;			// Text line distance for setting
		short		sStemThickness;		// stem thickness
		short		sAngle;				// Angle of Italisation
		short		sOptimum;			// optimum point size for setting
		short		sAverageWidth;		// Average character width
	};

	class HYIKCODEC_API CIKCharIndex
	{
	public:
		CIKCharIndex();
		~CIKCharIndex();

		void				SetDefault();

		short				sCharNumber;		// character number 
		short				sStartRecord;		// pointer to record containing start of character
		short				sWordPoint;			// word pointer to start of character
	};

	class HYIKCODEC_API CIKCharIndexInf
	{
	public:
		CIKCharIndexInf();
		~CIKCharIndexInf();

		void						SetDefault();

		unsigned short				usCharIndexLength;	// length of character index in words
		unsigned short				usLastReocrd;		// point to last written record (=length of font in records) (=LENGTH OF FONT IN PHYSICAL RECORDS)
		unsigned short				usLastWord;			// Last Word of last record	

		std::vector<CIKCharIndex>	vtCharIndex;	
	};

	class HYIKCODEC_API CIKHeader
	{
	public:
		CIKHeader();
		~CIKHeader();

		void						SetDefault();

		unsigned short				usLength;				// length of font header in words
		CIKName						Name;
		CIKFontInf					FontInf;
		short						sHierarchy;				// 1 for tis font
		CIKCharIndexInf				CharIndexInf;
	};

	class HYIKCODEC_API CIKCharName
	{	
	public:
		CIKCharName();
		~CIKCharName();

		void		SetDefault();

		short		sNameLength;		// length of name section in words
		short		sCharNumber;		// character number
		short		Reserved;			// number of following part
	};

	class HYIKCODEC_API CIKCharSet
	{
	public:
		CIKCharSet();
		~CIKCharSet();

		void		SetDefault();

		short		sSetLength;			// length of setting information
		short		sCharacterType;		// character type
		short		sDigitiNumber;		// number of digitizations
		short		sTotalWidth;		// Total set width (T=L+W+R)
		short		sLeftBearing;		// Left side bearing
		short		sWidth;				// width(W)
		short		sRightBearing;		// Rgiht side bearing
		short		sXMin;				// x-minimum;
		short		sXMax;				// x-maximum;
		short		sYMin;				// Y-minimum;
		short		sYMax;				// Y-maximum;
		short		sUnit;				// Unit;
	};

	class HYIKCODEC_API CIKPoint
	{
	public:
		CIKPoint();
		~CIKPoint();

		void		SetDefault();

		short	x;
		short	y;
		short	t;
	};

	class HYIKCODEC_API CIKContourIndex
	{
	public:
		CIKContourIndex();
		~CIKContourIndex();
		
		void		SetDefault();

		short		sRecordPoint;		// point of first contour relative to start of image information. The start of the
		short		sWordPoint;			// image information relative to character data record 
		short		sRotation;			// -1 clockwise 0 open contour 1 anti-clockwise
		short		sHierarchy;			// 
		short		sColor;				// 0 transparent 1 black
		short		sNumber;			// number of digitizations

		std::vector<CIKPoint> vtIKPoint;
	};

	class HYIKCODEC_API CIKContourInf
	{
	public:
		CIKContourInf();
		~CIKContourInf();

		void		SetDefault();

		short		sRecords;			//Length of Contour Index = sRecords * 2048 + Words
		short		sWords;	

		std::vector<CIKContourIndex> vtContourIndex;	
	};

	class HYIKCODEC_API CIKCharacter
	{
	public:
		CIKCharacter();
		~CIKCharacter();

		void		SetDefault();

		short						sRecords;		// Records length of character
		short						sWords;			// words;

		CIKCharName					CharNameInf;
		CIKCharSet					CharSet;
		CIKContourInf				ContourInfo;
		unsigned short				ImgLen;
		unsigned long				MapCode;		//unicode 
	};

	class HYIKCODEC_API CIKFont
	{
	public:
		CIKFont();
		~CIKFont();

		void						SetDefault();

		CIKHeader					Header;
		std::vector<CIKCharacter>	vtCharacter;	

		int							SetIkChar(size_t stIKIndex, CIKCharacter& IKChar);
		size_t						IkCharExistByMapCode(unsigned long ulUni);
		size_t						IkCharExistByIKNumber(unsigned short usIKNumber);
		void						SortCharASC();
	};
};
