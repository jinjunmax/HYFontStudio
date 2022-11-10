#include "stdafx.h"
#include "HYHVAR.h"
namespace HYFONTCODEC
{
	CHYHVAR::CHYHVAR()
	{
		majorVersion = 0;
		minorVersion = 0;
		itemVariationStoreOffset = 0;
		advanceWidthMappingOffset = 0;
		lsbMappingOffset = 0;
		rsbMappingOffset = 0;
		IsadvanceWidthMapping = false;
		IslsbMapping = false;
		IsrsbMapping = false;

	}	// end of CHYHVAR::CHYHVAR()

	CHYHVAR::~CHYHVAR()
	{
		majorVersion = 0;
		minorVersion = 0;
		itemVariationStoreOffset = 0;
		advanceWidthMappingOffset = 0;
		lsbMappingOffset = 0;
		rsbMappingOffset = 0;

	}	// end of CHYHVAR::~CHYHVAR()

	void CHYHVAR::SetDefault()
	{
		majorVersion = 0;
		minorVersion = 0;
		itemVariationStoreOffset = 0;
		advanceWidthMappingOffset = 0;
		lsbMappingOffset = 0;
		rsbMappingOffset = 0;

		itemVariationStore.SetDefault();
		advanceWidthMapping.SetDefault();
		lsbMapping.SetDefault();
		rsbMapping.SetDefault();

	}	// end of void CHYHVAR::SetDefault()

}