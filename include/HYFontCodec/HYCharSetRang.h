#pragma once
#include "HYFontCodecDef.h"

namespace HYFONTCODEC
{	

	class HY_DLL_CLASS CHYCharSetRang
	{
		public:
			CHYCharSetRang(void)
			{
				PrepareUnicodeRang();
			}
		private:
			class CHYSetRang
			{
				public:
					CHYSetRang()
					{
						FirstCode = 0;
						LastCode = 0;
					}

					CHYSetRang(int F, int L)
					{
						FirstCode = F;
						LastCode = L;	
					}									
				
					unsigned int  FirstCode;
					unsigned int  LastCode;
			};

			std::vector<CHYSetRang> vtUnicodeRange;
			void  PrepareUnicodeRang()
			{
				//CHYSetRang Rang(0000, 007F);
				//vtUnicodeRange.push_back();

			}
			
			
	};


};

