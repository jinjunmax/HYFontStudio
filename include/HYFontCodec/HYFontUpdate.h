#pragma once
#include "HYFontCodec.h"

namespace HYFONTCODEC
{
	class HY_DLL_CLASS CHYFontUpdate : public CHYFontCodec
	{
	public:
		CHYFontUpdate();
		~CHYFontUpdate();
		
		//����glyf�����ݣ�Ϊ������G3���ɵ�����
		int Update_glyf(char* pFontFile, std::vector<unsigned long> vtFlag);
	};
}
