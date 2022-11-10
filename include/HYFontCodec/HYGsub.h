#pragma once
#include <vector>
#include "HYScriptRecord.h"
#include "HYFeatureRecord.h"

namespace HYFONTCODEC
{
	class HY_DLL_CLASS CRangeRecord
	{
		public:
			CRangeRecord();
			~CRangeRecord();
		public:
			unsigned short Start;
			unsigned short End;
			unsigned short StartCoverageIndex;
	};

	class HY_DLL_CLASS CCoverage
	{
		public:
			CCoverage();
			~CCoverage();
		public:
			unsigned short					CoverageFormat;
			unsigned short					GlyphCount;		// format1
			std::vector<unsigned short>		vtGlyphID;		// format1
			unsigned short					RangeCount;		// format2
			std::vector<CRangeRecord>		vtRangeRecord;	// format2
	};

	class HY_DLL_CLASS CLookUpSingleSubstitution1
	{
		public:
			CLookUpSingleSubstitution1();
			~CLookUpSingleSubstitution1();

			CCoverage					Coverage;
			short						DeltaGlyphID;
	};

	class HY_DLL_CLASS CLookUpSingleSubstitution2
	{
		public:
			CLookUpSingleSubstitution2();
			~CLookUpSingleSubstitution2();
		
			unsigned short				GlyphCount;
			CCoverage					Coverage;
			std::vector<unsigned short>	vtGlyphID;
	};

	class HY_DLL_CLASS CLookUpType1
	{
		public:
			CLookUpType1();	
			~CLookUpType1();
		public:
			unsigned short					SubFormat;	
			CLookUpSingleSubstitution1		Substitution1;
			CLookUpSingleSubstitution2		Substitution2;
	};

	class HY_DLL_CLASS CLookUp
	{
		public:
			CLookUp();
			~CLookUp();

		public:
			unsigned short				LookUpType;
			unsigned short				LookUpFlag;
			unsigned short				SubTableCount;
			std::vector<CLookUpType1>	vtLookupType1;
	};	

	class HY_DLL_CLASS CHYGsub
	{
		public:
			CHYGsub();
			~CHYGsub();
		public:
			void Setdefault();
		
		public:
			HYFIXED								version;
			std::vector<CLookUp>				vtLookupList;
			std::vector<CHYScriptRecord>		vtScriptsList;
			std::vector<CHYFeatureRecord>		vtFeaturesList;
		public:
			int FindFeature(char* pTag);

	};

};