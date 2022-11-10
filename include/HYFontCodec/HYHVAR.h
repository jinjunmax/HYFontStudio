#pragma once
#include "HYItemVariation.h"

namespace HYFONTCODEC
{
	class HY_DLL_CLASS CHYHVAR
	{
		public:
			CHYHVAR();
			~CHYHVAR();
		public:
			void	SetDefault();

		public:
			unsigned short	majorVersion;			
			unsigned short	minorVersion;
			unsigned int	itemVariationStoreOffset;

			bool	IsadvanceWidthMapping;
			unsigned int	advanceWidthMappingOffset;
			bool	IslsbMapping;
			unsigned int	lsbMappingOffset;
			bool	IsrsbMapping;
			unsigned int	rsbMappingOffset;

			CHYItemVariationStore	itemVariationStore;
			DeltaSetIndexMap		advanceWidthMapping;
			DeltaSetIndexMap		lsbMapping;
			DeltaSetIndexMap		rsbMapping;
	};
}

