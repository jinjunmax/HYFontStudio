#include "StdAfx.h"
#include "HYVhea.h"

namespace HYFONTCODEC
{
	CHYVhea::CHYVhea(void)
	{
		version.value		= 0;
		version.fract		= 0;
		ascent				= 0;
		descent				= 0;
		lineGap				= 0;
		advanceHeightMax	= 0;
		minTop				= 0;
		minBottom			= 0;
		yMaxExtent			= 0;
		caretSlopeRise		= 0;
		caretSlopeRun		= 0;
		caretOffset			= 0;
		reserved1			= 0;
		reserved2			= 0;
		reserved3			= 0;
		reserved4			= 0;
		metricDataFormat	= 0;
		numOfLongVerMetrics = 0;

	}	// end of CHYVhea::CHYVhea()

	CHYVhea::~CHYVhea(void)
	{

	}	// end of CHYVhea::~CHYVhea()

	void CHYVhea::SetDefault()
	{
		version.value		= 0;
		version.fract		= 0;
		ascent				= 0;
		descent				= 0;
		lineGap				= 0;
		advanceHeightMax	= 0;
		minTop				= 0;
		minBottom			= 0;
		yMaxExtent			= 0;
		caretSlopeRise		= 0;
		caretSlopeRun		= 0;
		caretOffset			= 0;
		reserved1			= 0;
		reserved2			= 0;
		reserved3			= 0;
		reserved4			= 0;
		metricDataFormat	= 0;
		numOfLongVerMetrics = 0;

	}	// end of void CHYVhea::SetDefault()
}