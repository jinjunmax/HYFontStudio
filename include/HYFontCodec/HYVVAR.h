#pragma once
#include "HYItemVariation.h"
namespace HYFONTCODEC
{
	class HY_DLL_CLASS CHYVVAR
	{
		public:
			CHYVVAR();
			~CHYVVAR();
		public:
			void	SetDefault();

		public:
			unsigned short	majorVersion;
			unsigned short	minorVersion;
			unsigned int	itemVariationStoreOffset;
			unsigned int	advanceHeightMappingOffset;
			unsigned int	tsbMappingOffset;
			unsigned int	bsbMappingOffset;
			unsigned int	vOrgMappingOffset;

			CHYItemVariationStore	itemVariationStore;
			DeltaSetIndexMap		advanceHeightMapping;
			DeltaSetIndexMap		tsbMapping;
			DeltaSetIndexMap		bsbMapping;
			DeltaSetIndexMap		vOrgMapping;
	};
}


