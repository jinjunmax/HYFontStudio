#pragma once
#include "HYFontCodecDef.h"

namespace HYFONTCODEC
{
	class HY_DLL_CLASS CHYMaxp
	{
	public:
		CHYMaxp(void);
		~CHYMaxp(void);

		void SetDefault();
	public:
		HYFIXED			version;
		unsigned short	numGlyphs;
		unsigned short	maxPoints;
		unsigned short	maxContours;
		unsigned short	maxCompositePoints;
		unsigned short	maxCompositeContours;
		unsigned short	maxZones;
		unsigned short	maxTwilightPoints;
		unsigned short	maxStorage;
		unsigned short	maxFunctionDefs;
		unsigned short	maxInstructionDefs;
		unsigned short	maxStackElements;
		unsigned short	maxSizeOfInstructions;
		unsigned short	maxComponentElements;
		unsigned short	maxComponentDepth;
	};
};

