#include "StdAfx.h"
#include "HYMaxp.h"

namespace HYFONTCODEC
{
	CHYMaxp::CHYMaxp(void)
	{
		version.value			= 0;
		version.fract			= 0;
		numGlyphs				= 0;
		maxPoints				= 0;
		maxContours				= 0;
		maxCompositePoints		= 0;
		maxCompositeContours	= 0;
		maxZones				= 0;
		maxTwilightPoints		= 0;
		maxStorage				= 0;
		maxFunctionDefs			= 0;
		maxInstructionDefs		= 0;
		maxStackElements		= 0;
		maxSizeOfInstructions	= 0;
		maxComponentElements	= 0;
		maxComponentDepth		= 0;

	}	// end of CHYMaxp::CHYMaxp(void) 

	CHYMaxp::~CHYMaxp(void)
	{

	}	// end of CHYMaxp::~CHYMaxp(void)

	void CHYMaxp::SetDefault()
	{
		version.value			= 0;
		version.fract			= 0;
		numGlyphs				= 0;
		maxPoints				= 0;
		maxContours				= 0;
		maxCompositePoints		= 0;
		maxCompositeContours	= 0;
		maxZones				= 0;
		maxTwilightPoints		= 0;
		maxStorage				= 0;
		maxFunctionDefs			= 0;
		maxInstructionDefs		= 0;
		maxStackElements		= 0;
		maxSizeOfInstructions	= 0;
		maxComponentElements	= 0;
		maxComponentDepth		= 0;

	}	// end of void CHYMaxp::SetDefault()
}

