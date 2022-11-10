#include "StdAfx.h"
#include "HYVORG.h"

namespace HYFONTCODEC
{
	
	CHYVertOriginYMetrics::CHYVertOriginYMetrics()
	{
		glyphIndex = 0;
		vertOriginY = 0;

	}	// end of CHYVertOriginYMetrics::CHYVertOriginYMetrics()

	CHYVertOriginYMetrics::~CHYVertOriginYMetrics()
	{

	}	// end of CHYVertOriginYMetrics::~CHYVertOriginYMetrics()

	CHYVORG::CHYVORG(void)
	{
		majorVersion = 0;
		minorVersion = 0;
		defaultVertOriginY = 0;
		numVertOriginYMetrics = 0; 		

	}	// end of CHYVORG::CHYVORG()

	CHYVORG::~CHYVORG(void)
	{

	}	// end of CHYVORG::~CHYVORG()

	void CHYVORG::SetDefault()
	{
		majorVersion = 0;
		minorVersion = 0;
		defaultVertOriginY = 0;
		numVertOriginYMetrics = 0; 	
		vtVertOriginYMetrics.clear();

	}	// end of void CHYVORG::SetDefault()

}
