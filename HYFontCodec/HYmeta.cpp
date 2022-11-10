#include "StdAfx.h"
#include "HYmeta.h"

namespace HYFONTCODEC
{
	CMetaDateMap::CMetaDateMap()
	{
		memset(tag,0,4);
		dataOffset = 0;
		dataLength = 0;
		
	}	// end of CMetaDateMap::CMetaDateMap()

	CMetaDateMap::~CMetaDateMap()
	{

	}	// end of CMetaDateMap::~CMetaDateMap()

	CHYmeta::CHYmeta(void)
	{

	}	// end of CHYmeta::CHYmeta()	

	CHYmeta::~CHYmeta(void)
	{

	}	// end of CHYmeta::~CHYmeta()

	void CHYmeta::Setdefault()
	{
		vtDatamap.clear();

	}	// end of CHYmeta::Setdefault()

}
