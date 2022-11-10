#include "stdafx.h"
#include "HYavar.h"

namespace HYFONTCODEC
{
	CHYAxisValueMap::CHYAxisValueMap()
	{
		fromCoordinate = 0;
		toCoordinate = 0;

	}	// end of CHYAxisValueMap::CHYAxisValueMap()

	CHYAxisValueMap::~CHYAxisValueMap()
	{
		fromCoordinate = 0;
		toCoordinate = 0;

	}	// end of CHYAxisValueMap::~CHYAxisValueMap()	

	CHYSegmentMaps::CHYSegmentMaps()
	{
		positionMapCount = 0;

	}	// end of CHYSegmentMaps::CHYSegmentMaps()

	CHYSegmentMaps::~CHYSegmentMaps()
	{
		positionMapCount = 0;
		vtAxisValueMaps.clear();

	}	// end of CHYSegmentMaps::~CHYSegmentMaps()

	CHYavar::CHYavar()
	{
		majorVersion = 0;
		minorVersion = 0;
		reserved = 0;
		axisCount = 0;

	}	// end of CHYavar::CHYavar()

	CHYavar::~CHYavar()
	{
		majorVersion = 0;
		minorVersion = 0;
		reserved = 0;
		axisCount = 0;
		vtSegmenMaps.clear();

	}	// end of CHYavar::~CHYavar()

	void CHYavar::SetDefault()
	{
		majorVersion = 0;
		minorVersion = 0;
		reserved = 0;
		axisCount = 0;
		vtSegmenMaps.clear();

	}	// end of void CHYavar::SetDefault()

}