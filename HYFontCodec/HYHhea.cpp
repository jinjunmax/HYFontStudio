#include "StdAfx.h"
#include "HYHhea.h"

namespace HYFONTCODEC
{
	CHYHhea::CHYHhea(void)
	{
		version.fract		= 0;
		version.value		= 0;
		Ascender			= 0;
		Descender			= 0;
		LineGap				= 0;
		advanceWidthMax		= 0;
		minLeftSideBearing	= 0;
		minRightSideBearing	= 0;
		xMaxExtent			= 0;
		caretSlopeRise		= 0;
		caretSlopeRun		= 0;
		caretOffset			= 0;		
		reserved1			= 0;
		reserved2			= 0;
		reserved3			= 0;
		reserved4			= 0;
		metricDataFormat	= 0;
		numberOfHMetrics	= 0;

	}	// end of CHYHhea::CHYHhea()

	CHYHhea::~CHYHhea(void)
	{

	}	// end of CHYHhea::~CHYHhea()

	void CHYHhea::SetDefault()
	{
		version.fract		= 0;
		version.value		= 0;
		Ascender			= 0;
		Descender			= 0;
		LineGap				= 0;
		advanceWidthMax		= 0;
		minLeftSideBearing	= 0;
		minRightSideBearing	= 0;
		xMaxExtent			= 0;
		caretSlopeRise		= 0;
		caretSlopeRun		= 0;
		caretOffset			= 0;		
		reserved1			= 0;
		reserved2			= 0;
		reserved3			= 0;
		reserved4			= 0;
		metricDataFormat	= 0;
		numberOfHMetrics	= 0;

	}	// end of void CHYHhea::SetDefault()

	CHYHhea& CHYHhea::operator=(CHYHhea& other)
	{
		if (this==&other)
		{
			return *this;
		}
		else 
		{
			version.fract		= other.version.fract;
			version.value		= other.version.value;
			Ascender			= other.Ascender;
			Descender			= other.Descender;
			LineGap				= other.LineGap;
			advanceWidthMax		= other.advanceWidthMax;
			minLeftSideBearing	= other.minLeftSideBearing;
			minRightSideBearing	= other.minRightSideBearing;
			xMaxExtent			= other.xMaxExtent;
			caretSlopeRise		= other.caretSlopeRise;
			caretSlopeRun		= other.caretSlopeRun;
			caretOffset			= other.caretOffset;
			reserved1			= other.reserved1;
			reserved2			= other.reserved2;
			reserved3			= other.reserved3;
			reserved4			= other.reserved4;
			metricDataFormat	= other.metricDataFormat;
			numberOfHMetrics	= other.numberOfHMetrics;		
		}

		return  *this;

	}	// end of CHYHhea& operator=()

}
