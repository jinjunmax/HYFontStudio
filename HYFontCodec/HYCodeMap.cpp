#include "StdAfx.h"
#include "HYCodeMap.h"
#include <algorithm>


namespace HYFONTCODEC
{
	bool HYCodeMapSortPredicate( CHYCodeMapItem& d1,  CHYCodeMapItem& d2)
	{
		return ( d1.ulGlyphNo < d2.ulGlyphNo );

	}	// end of BOOL HYCodeMapSortPredicate()

	CHYCodeMapItem::CHYCodeMapItem(void)
	{
		iGlyphIndex = 0;
		ulGlyphNo = 0;

	}	// end of CHYCodeMapItem::CHYCodeMapItem()

	CHYCodeMapItem::~CHYCodeMapItem(void)
	{

	}	// end of CHYCodeMapItem::~CHYCodeMapItem()

	CHYCodeMap::CHYCodeMap()
	{	

	}	// end of CHYCodeMap::CHYCodeMap()

	CHYCodeMap::~CHYCodeMap()
	{

	}	// end of CHYCodeMap::~CHYCodeMap()

	void CHYCodeMap::FindIndexByUnicode(unsigned long ulUnicode, std::vector<int>& vtGid)
	{
		vtGid.clear();

		for(size_t i=0; i<vtHYCodeMap.size(); i++)
		{
			if(vtHYCodeMap[i].ulGlyphNo == ulUnicode)
				vtGid.push_back(vtHYCodeMap[i].iGlyphIndex);
		}

	}	// end of void CHYCodeMap::FindIndexByUnicode()

	int	CHYCodeMap::FindIndexByUnicode(unsigned long ulUnicode)
	{
		for(size_t i=0; i<vtHYCodeMap.size(); i++)	{
			if(vtHYCodeMap[i].ulGlyphNo == ulUnicode)
				return vtHYCodeMap[i].iGlyphIndex;
		}

		return -1;

	}	// end of int	CHYCodeMap::FindIndexByUnicode()

	void CHYCodeMap::FilterByUnicode(unsigned long ulUnicode)
	{	
		std::vector<CHYCodeMapItem>		vtTmpHYCodeMap = vtHYCodeMap;

		vtHYCodeMap.clear();
		size_t st = vtTmpHYCodeMap.size();
		for (size_t i = 0; i < st; i++) {
			if (vtTmpHYCodeMap[i].ulGlyphNo != ulUnicode)	{
				vtHYCodeMap.push_back(vtTmpHYCodeMap[i]);
			}	
		}

	}	// end of void CHYCodeMap::FilterByUnicode()


	void CHYCodeMap::QuickSortbyUnicode()
	{
		std::sort( vtHYCodeMap.begin(), vtHYCodeMap.end(), HYCodeMapSortPredicate);

	}	// end of void CHYCodeMap::QuickSortbyUnicode()
}