#include "stdafx.h"
#include "HYVVAR.h"

namespace HYFONTCODEC
{
	CHYVVAR::CHYVVAR()
	{
		majorVersion = 0;
		minorVersion = 0;
		itemVariationStoreOffset = 0;
		advanceHeightMappingOffset = 0;
		tsbMappingOffset = 0;
		bsbMappingOffset = 0;
		vOrgMappingOffset = 0;

	}	// end of CHYVVAR::CHYVVAR()

	CHYVVAR::~CHYVVAR()
	{
		majorVersion = 0;
		minorVersion = 0;
		itemVariationStoreOffset = 0;
		advanceHeightMappingOffset = 0;
		tsbMappingOffset = 0;
		bsbMappingOffset = 0;
		vOrgMappingOffset = 0;

	}	// end of CHYVVAR::~CHYVVAR()

	void CHYVVAR::SetDefault()
	{
		majorVersion = 0;
		minorVersion = 0;
		itemVariationStoreOffset = 0;
		advanceHeightMappingOffset = 0;
		tsbMappingOffset = 0;
		bsbMappingOffset = 0;
		vOrgMappingOffset = 0;

		itemVariationStore.SetDefault();
		advanceHeightMapping.SetDefault();
		tsbMapping.SetDefault();
		bsbMapping.SetDefault();
		vOrgMapping.SetDefault();

	}	// end of void CHYVVAR::SetDefault()

}
