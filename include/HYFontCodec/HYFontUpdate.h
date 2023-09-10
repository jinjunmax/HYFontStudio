#pragma once
#include "HYFontCodec.h"

namespace HYFONTCODEC
{
	class HY_DLL_CLASS CHYFontUpdate : public CHYFontCodec
	{
	public:
		CHYFontUpdate();
		~CHYFontUpdate();
		
		//更新glyf表数据，为了修正G3生成的数据
		int Update_glyf(char* pFontFile, std::vector<unsigned long> vtFlag);
	};
}
