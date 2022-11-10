#pragma once

#include "HYFontCodecDef.h"
#include <vector>

namespace HYFONTCODEC
{
	class CHYFpgm
	{
	public:
		CHYFpgm(void);
		~CHYFpgm(void);

		void SetDefault();
	public:
		std::vector<unsigned char> vtfgpm;
	};
};
