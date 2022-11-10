#pragma once

#include "HYFontCodecDef.h"
#include <vector>

namespace HYFONTCODEC
{
	class HY_DLL_CLASS CHYCvt
	{
	public:
		CHYCvt(void);
		~CHYCvt(void);

		void	SetDefault();

	public:
		std::vector<short> vtCvt;
	};
};

