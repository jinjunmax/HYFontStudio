#include "StdAfx.h"
#include "HYOS.h"

namespace HYFONTCODEC
{
	HYPANOSE::HYPANOSE()
	{
		FamilyType			= 0;
		SerifStyle			= 0;
		Weight				= 0;
		Proportion			= 0;
		Contrast			= 0;
		StrokeVariation		= 0;
		ArmStyle			= 0;
		Letterform			= 0;
		Midline				= 0;
		XHeight				= 0;

	}	// end of HYPANOSE::HYPANOSE()

	HYPANOSE::~HYPANOSE()
	{

	}	// end of HYPANOSE::~HYPANOSE()

	void HYPANOSE::SetDefault()
	{
		FamilyType			= 0;
		SerifStyle			= 0;
		Weight				= 0;
		Proportion			= 0;
		Contrast			= 0;
		StrokeVariation		= 0;
		ArmStyle			= 0;
		Letterform			= 0;
		Midline				= 0;
		XHeight				= 0;

	}	// end of void HYPANOSE::SetDefault()

	CHYOS::CHYOS(void)
	{
		version					= 0;
		xAvgCharWidth			= 0;
		usWeightClass			= 0;
		usWidthClass			= 0;
		fsType					= 0;
		ySubscriptXSize			= 0;
		ySubscriptYSize			= 0;
		ySubscriptXOffset		= 0;
		ySubscriptYOffset		= 0;
		ySuperscriptXSize		= 0;
		ySuperscriptYSize		= 0;
		ySuperscriptXOffset		= 0;
		ySuperscriptYOffset		= 0;
		yStrikeoutSize			= 0;
		yStrikeoutPosition		= 0;
		sFamilyClass			= 0;	
		ulUnicodeRange1			= 0;
		ulUnicodeRange2			= 0;
		ulUnicodeRange3			= 0;
		ulUnicodeRange4			= 0;
		vtachVendID.clear();
		fsSelection				= 0;
		usFirstCharIndex		= 0;
		usLastCharIndex			= 0;
		sTypoAscender			= 0;
		sTypoDescender			= 0;
		sTypoLineGap			= 0;
		usWinAscent				= 0;
		usWinDescent			= 0;
		ulCodePageRange1		= 0;
		ulCodePageRange2		= 0;
		sxHeight				= 0;
		sCapHeight				= 0;
		usDefaultChar			= 0;
		usBreakChar				= 0;
		usMaxContext			= 0;

	}	// end of CHYOS::CHYOS(void)

	CHYOS::~CHYOS(void)
	{

	}	// end of CHYOS::~CHYOS(void)

	void CHYOS::SetDefault()
	{
		version					= 0;
		xAvgCharWidth			= 0;
		usWeightClass			= 0;
		usWidthClass			= 0;
		fsType					= 0;
		ySubscriptXSize			= 0;
		ySubscriptYSize			= 0;
		ySubscriptXOffset		= 0;
		ySubscriptYOffset		= 0;
		ySuperscriptXSize		= 0;
		ySuperscriptYSize		= 0;
		ySuperscriptXOffset		= 0;
		ySuperscriptYOffset		= 0;
		yStrikeoutSize			= 0;
		yStrikeoutPosition		= 0;
		sFamilyClass			= 0;	
		ulUnicodeRange1			= 0;
		ulUnicodeRange2			= 0;
		ulUnicodeRange3			= 0;
		ulUnicodeRange4			= 0;
		vtachVendID.clear();
		fsSelection				= 0;
		usFirstCharIndex		= 0;
		usLastCharIndex			= 0;
		sTypoAscender			= 0;
		sTypoDescender			= 0;
		sTypoLineGap			= 0;
		usWinAscent				= 0;
		usWinDescent			= 0;
		ulCodePageRange1		= 0;
		ulCodePageRange2		= 0;
		sxHeight				= 0;
		sCapHeight				= 0;
		usDefaultChar			= 0;
		usBreakChar				= 0;
		usMaxContext			= 0;

	}	// end of void CHYOS::SetDefault()

	CHYOS& CHYOS::operator=(const CHYOS& other)
	{
		if (this == &other)
		{
			return *this;
		}
		else 
		{
			version					= other.version;
			xAvgCharWidth			= other.xAvgCharWidth;
			usWeightClass			= other.usWeightClass;
			usWidthClass			= other.usWidthClass;
			fsType					= other.fsType;
			ySubscriptXSize			= other.ySubscriptXSize;
			ySubscriptYSize			= other.ySubscriptYSize;
			ySubscriptXOffset		= other.ySubscriptXOffset;
			ySubscriptYOffset		= other.ySubscriptYOffset;
			ySuperscriptXSize		= other.ySuperscriptXSize;
			ySuperscriptYSize		= other.ySuperscriptYSize;
			ySuperscriptXOffset		= other.ySuperscriptXOffset;
			ySuperscriptYOffset		= other.ySuperscriptYOffset;
			yStrikeoutSize			= other.yStrikeoutSize;
			yStrikeoutPosition		= other.yStrikeoutPosition;
			sFamilyClass			= other.sFamilyClass;	
			ulUnicodeRange1			= other.ulUnicodeRange1;
			ulUnicodeRange2			= other.ulUnicodeRange2;
			ulUnicodeRange3			= other.ulUnicodeRange3;
			ulUnicodeRange4			= other.ulUnicodeRange4;
			vtachVendID.clear();
			vtachVendID				= other.vtachVendID;
			fsSelection				= other.fsSelection;
			usFirstCharIndex		= other.usFirstCharIndex;
			usLastCharIndex			= other.usLastCharIndex;
			sTypoAscender			= other.sTypoAscender;
			sTypoDescender			= other.sTypoDescender;
			sTypoLineGap			= other.sTypoLineGap;
			usWinAscent				= other.usWinAscent;
			usWinDescent			= other.usWinDescent;
			ulCodePageRange1		= other.ulCodePageRange1;
			ulCodePageRange2		= other.ulCodePageRange2;
			sxHeight				= other.sxHeight;
			sCapHeight				= other.sCapHeight;
			usDefaultChar			= other.usDefaultChar;
			usBreakChar				= other.usBreakChar;
			usMaxContext			= other.usMaxContext;			
		}

		return *this;

	}	// end of CHYOS& CHYOS::operator=()

}


