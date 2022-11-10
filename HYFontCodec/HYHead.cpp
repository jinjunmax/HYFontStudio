#include "StdAfx.h"
#include "HYHead.h"

namespace HYFONTCODEC
{
	CHYHead::CHYHead(void)
	{
		version.fract		= 0;
		version.value		= 0;
		fontRevision.fract	= 0;
		fontRevision.value	= 0;
		checkSumAdjustment	= 0;
		magicNumber			= 0;
		flags				= 0;	
		unitsPerEm			= 0;	
		xMin				= 0;					
		yMin				= 0;					
		xMax				= 0;					
		yMax				= 0;					
		macStyle			= 0;		
		lowestRecPPEM		= 0;
		fontDirectionHint	= 0;	
		indexToLocFormat	= 1;		
		glyphDataFormat		= 0;		

		memset(created,0,8);
		memset(modified,0,8);

	}	// end of 	CHYHead::CHYHead()

	CHYHead::~CHYHead(void)
	{

	}	// end of CHYHead::~CHYHead(void)

	void CHYHead::SetDefault()
	{
		version.fract		= 0;
		version.value		= 0;
		fontRevision.fract	= 0;
		fontRevision.value	= 0;
		checkSumAdjustment	= 0;
		magicNumber			= 0;
		flags				= 0;	
		unitsPerEm			= 0;	
		xMin				= 0;					
		yMin				= 0;					
		xMax				= 0;					
		yMax				= 0;					
		macStyle			= 0;		
		lowestRecPPEM		= 0;
		fontDirectionHint	= 0;	
		indexToLocFormat	= 1;		
		glyphDataFormat		= 0;		

		memset(created,0,8);
		memset(modified,0,8);

	}	// end of void CHYHead::SetDefault()

	CHYHead& CHYHead::operator=(CHYHead& other)
	{
		if (this == &other)
		{
			return *this;
		}
		else 
		{
			version.fract		= other.version.fract;
			version.value		= other.version.value;
			fontRevision.fract	= other.fontRevision.fract;
			fontRevision.value	= other.fontRevision.value;
			checkSumAdjustment	= other.checkSumAdjustment;
			magicNumber			= other.magicNumber;
			flags				= other.flags;
			unitsPerEm			= other.unitsPerEm;
			xMin				= other.xMin;
			yMin				= other.yMin;
			xMax				= other.xMax;
			yMax				= other.yMax;
			macStyle			= other.macStyle;
			lowestRecPPEM		= other.lowestRecPPEM;
			fontDirectionHint	= other.fontDirectionHint;	
			indexToLocFormat	= other.indexToLocFormat;
			glyphDataFormat		= other.glyphDataFormat;

			memcpy(created,other.created,8);
			memcpy(modified,other.modified,8);
		}

		return *this;

	}	// end of CHYHead& CHYHead::operator=()

}


