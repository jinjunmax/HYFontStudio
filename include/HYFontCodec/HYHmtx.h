#pragma once
#include "HYFontCodecDef.h"
#include <vector>

namespace HYFONTCODEC
{
	class HY_DLL_CLASS HMTX_LONGHORMERTRIC
	{
		public:
			HMTX_LONGHORMERTRIC();
			~HMTX_LONGHORMERTRIC();

			void SetDefault();
		public:	
			unsigned short	advanceWidth;
			short			lsb;
	};

	class HY_DLL_CLASS CHYHmtx
	{
		public:
			CHYHmtx(void);
			~CHYHmtx(void);

			void	SetDefault();
			int		GetHMTX(int iGlyphID, HMTX_LONGHORMERTRIC& HMTX);
			int		GetAvgWidth();
		public:
			std::vector<HMTX_LONGHORMERTRIC>	vtLonghormetric;
			std::vector<short>					vtLeftsidebearing;
	};
};