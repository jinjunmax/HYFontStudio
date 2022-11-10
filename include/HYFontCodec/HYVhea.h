#pragma once

#include "HYFontCodecDef.h"

namespace HYFONTCODEC
{
	class HY_DLL_CLASS CHYVhea
	{
	public:
		CHYVhea(void);
		~CHYVhea(void);

		void SetDefault();

	public:
		HYFIXED			version;
		short			ascent;
		short			descent;
		short			lineGap;
		short			advanceHeightMax;
		short			minTop;
		short			minBottom;
		short			yMaxExtent;
		short			caretSlopeRise;
		short			caretSlopeRun;
		short			caretOffset;
		short			reserved1;
		short			reserved2;
		short			reserved3;
		short			reserved4;
		short			metricDataFormat;
		unsigned short	numOfLongVerMetrics;
	};
};