#pragma once

#include "HYFontCodecDef.h"
#include<vector>

namespace HYFONTCODEC
{
	class HY_DLL_CLASS HYPANOSE
	{
	public:
		HYPANOSE();
		~HYPANOSE();

		void SetDefault();

		unsigned char			FamilyType;
		unsigned char			SerifStyle;
		unsigned char			Weight;
		unsigned char			Proportion;
		unsigned char			Contrast;
		unsigned char			StrokeVariation;
		unsigned char			ArmStyle;
		unsigned char			Letterform;
		unsigned char			Midline;
		unsigned char			XHeight;
	}; 

	class HY_DLL_CLASS CHYOS
	{
	public:
		CHYOS(void);
		~CHYOS(void);

		void SetDefault();
		CHYOS& operator=(const CHYOS& other);
	public:
		unsigned short					version;
		short							xAvgCharWidth;
		unsigned short					usWeightClass;
		unsigned short					usWidthClass;
		short							fsType;
		short							ySubscriptXSize;
		short							ySubscriptYSize;
		short							ySubscriptXOffset;
		short							ySubscriptYOffset;
		short							ySuperscriptXSize;
		short							ySuperscriptYSize;
		short							ySuperscriptXOffset;
		short							ySuperscriptYOffset;
		short							yStrikeoutSize;
		short							yStrikeoutPosition;
		short							sFamilyClass;
		HYPANOSE						panose;
		unsigned long					ulUnicodeRange1;
		unsigned long					ulUnicodeRange2;
		unsigned long					ulUnicodeRange3;
		unsigned long					ulUnicodeRange4;
		std::vector<char>				vtachVendID;
		unsigned short					fsSelection;
		unsigned short					usFirstCharIndex;
		unsigned short					usLastCharIndex;
		short							sTypoAscender;
		short							sTypoDescender;
		short							sTypoLineGap;
		unsigned short					usWinAscent;
		unsigned short					usWinDescent;
		unsigned long					ulCodePageRange1;
		unsigned long					ulCodePageRange2;
		short							sxHeight;
		short							sCapHeight;
		unsigned short					usDefaultChar;
		unsigned short					usBreakChar;
		unsigned short					usMaxContext;
	};
};