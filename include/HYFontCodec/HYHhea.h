#pragma once

#include "HYFontCodecDef.h"

namespace HYFONTCODEC
{
	class HY_DLL_CLASS CHYHhea
	{
		public:
			CHYHhea(void);
			~CHYHhea(void);

			void SetDefault();
			CHYHhea& operator=(CHYHhea& other);

		public:	
			HYFIXED			version;
			short			Ascender;
			short			Descender;
			short			LineGap;
			unsigned short	advanceWidthMax;
			short			minLeftSideBearing;
			short			minRightSideBearing;
			short			xMaxExtent;
			short			caretSlopeRise;
			short			caretSlopeRun;
			short			caretOffset;			
			short			reserved1;
			short			reserved2;
			short			reserved3;
			short			reserved4;
			short			metricDataFormat;
			unsigned short	numberOfHMetrics;
	};
};

