#include "StdAfx.h"
#include "HYHmtx.h"

namespace HYFONTCODEC
{
	HMTX_LONGHORMERTRIC::HMTX_LONGHORMERTRIC()
	{
		advanceWidth	= 0;
		lsb				= 0;

	}	// end of HMTX_LONGHORMERTRIC::HMTX_LONGHORMERTRIC()

	HMTX_LONGHORMERTRIC::~HMTX_LONGHORMERTRIC()
	{

	}	// end of HMTX_LONGHORMERTRIC::~HMTX_LONGHORMERTRIC()

	void HMTX_LONGHORMERTRIC::SetDefault()
	{
		advanceWidth	= 0;
		lsb				= 0;

	}	// end of void HMTX_LONGHORMERTRIC::SetDefault()

	CHYHmtx::CHYHmtx(void)
	{

	}	// end of CHYHmtx::CHYHmtx(void)

	CHYHmtx::~CHYHmtx(void)
	{
		vtLonghormetric.clear();
		vtLeftsidebearing.clear();

	}	// end of CHYHmtx::~CHYHmtx(void)

	void CHYHmtx::SetDefault()
	{
		vtLonghormetric.clear();
		vtLeftsidebearing.clear();

	}	// end of void CHYHmtx::SetDefault()

	int CHYHmtx::GetHMTX(int iGlyphID, HMTX_LONGHORMERTRIC& HMTX)
	{
		int iLonghormetricSize = vtLonghormetric.size();
		if (iLonghormetricSize== 0) return -1;

		if (iGlyphID<iLonghormetricSize)
		{
			HMTX.advanceWidth = vtLonghormetric[iGlyphID].advanceWidth;
			HMTX.lsb = vtLonghormetric[iGlyphID].lsb;
		}
		else 
		{
			HMTX.advanceWidth=vtLonghormetric[iLonghormetricSize-1].advanceWidth;
			HMTX.lsb = vtLeftsidebearing[iGlyphID-iLonghormetricSize];
		}

		return HY_NOERROR;

	}	// end of int CHYHmtx::GetHMTX()

	int CHYHmtx::GetAvgWidth()
	{
		int iLonghormetricSize = vtLonghormetric.size();
		if (iLonghormetricSize == 0) return -1;

		int iAvgWidth = 0;
		for (int i = 0; i < iLonghormetricSize; i++)
		{
			iAvgWidth += vtLonghormetric[i].advanceWidth;
		}

		int iLeftsidebearing = vtLeftsidebearing.size();
		for (int i = 0; i < iLeftsidebearing; i++)
		{
			iAvgWidth += vtLonghormetric[iLonghormetricSize - 1].advanceWidth;
		}

		if (iAvgWidth == 0) return 0;
		return iAvgWidth / (iLonghormetricSize + iLeftsidebearing) + 0.5f;
	
	}	// end of int CHYHmtx::GetAvgWidth()


}


