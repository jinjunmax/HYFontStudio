#pragma once

#include "HYFontCodecDef.h"
#include <vector>

namespace HYFONTCODEC
{
	class HY_DLL_CLASS CHYLoca
	{
		public:
			CHYLoca(void);
			~CHYLoca(void);

			void SetDefault();
		public:
			std::vector<unsigned long> vtLoca;
	};
};

