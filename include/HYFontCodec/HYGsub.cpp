#include "StdAfx.h"
#include "HYGsub.h"

namespace HYFONTCODEC
{
	CRangeRecord::CRangeRecord()
	{
		Start				= 0;
		End					= 0;
		StartCoverageIndex	= 0;

	}	// end of CRangeRecord::CRangeRecord()

	CRangeRecord::~CRangeRecord()
	{

	}	// end of CRangeRecord::~CRangeRecord()

	CCoverage::CCoverage()
	{
		CoverageFormat	= 0;
		GlyphCount		= 0;
		RangeCount		= 0;

	}	// end of CCoverage::CCoverage()

	CCoverage::~CCoverage()
	{

	}	// end of CCoverage::~CCoverage()

	CLookUpSingleSubstitution1::CLookUpSingleSubstitution1()
	{
		DeltaGlyphID = 0;

	}	// end of CLookUpSingleSubstitution1::CLookUpSingleSubstitution1()

	CLookUpSingleSubstitution1::~CLookUpSingleSubstitution1()
	{

	}	// end of CLookUpSingleSubstitution1::~CLookUpSingleSubstitution1()

	CLookUpSingleSubstitution2::CLookUpSingleSubstitution2()
	{
		GlyphCount	= 0;

	}	// end of CLookUpSingleSubstitution2::CLookUpSingleSubstitution2()

	CLookUpSingleSubstitution2::~CLookUpSingleSubstitution2()
	{

	}	// end of CLookUpSingleSubstitution2::~CLookUpSingleSubstitution2()

	CLookUpType1::CLookUpType1()
	{
		SubFormat = 0;

	}	// end of CLookUpType1::CLookUpType1()

	CLookUpType1::~CLookUpType1()
	{

	}	// end of CLookUpType1::~CLookUpType1()

	CLookUp::CLookUp()
	{
		LookUpType = 0;
		LookUpFlag = 0;
		SubTableCount = 0;

	}	// end of CLookUp::CLookUp()

	CLookUp::~CLookUp()
	{

	}	// end of CLookUp::~CLookUp()

	CHYGsub::CHYGsub()
	{
		version.value = 0;
		version.fract = 0;

	}	// end of CHYGsub::CHYGsub()

	CHYGsub::~CHYGsub()
	{

	}	// end of CHYGsub::~CHYGsub()

	void CHYGsub::Setdefault()
	{
		version.value = 0;
		version.fract = 0;

		vtLookupList.clear();
		vtScriptsList.clear();
		vtFeaturesList.clear();		

	}	// end of void CHYGsub::Setdefault()

	int CHYGsub::FindFeature(char* pTag)
	{
		char p[5] = { 0 };

		for (int i = 0; i < (int)vtFeaturesList.size(); i++)
		{
			memcpy(p, vtFeaturesList[i].Tag,4);

			if (strcmp(pTag, p) == 0) return i;
		}
		return -1;
	}
}
