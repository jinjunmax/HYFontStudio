#pragma once
#include <vector>

namespace HYFONTCODEC
{
	class HY_DLL_CLASS CHYCodeMapItem
	{
	public:
		CHYCodeMapItem(void);
		~CHYCodeMapItem(void);

		int				iGlyphIndex;
		unsigned long  	ulGlyphNo;
		std::string			psName;
	};

	class HY_DLL_CLASS CHYCodeMap
	{
	public:
		CHYCodeMap();
		~CHYCodeMap();

		void	FindIndexByUnicode(unsigned long ulUnicode, std::vector<int>& vtGid);
		int		FindIndexByUnicode(unsigned long ulUnicode);
		void	FilterByUnicode(unsigned long ulUnicode);

		void	QuickSortbyUnicode();

		std::vector<CHYCodeMapItem>		vtHYCodeMap;
	};
};
