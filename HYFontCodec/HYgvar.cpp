#include "StdAfx.h"
#include "HYgvar.h"

namespace HYFONTCODEC
{
	CHYTuple::CHYTuple()
	{
	}	// end of Tuple::Tuple()

	CHYTuple::~CHYTuple()
	{

	}	// end of Tuple::~Tuple()

	TupleVariationHeader::TupleVariationHeader()
	{
		variationDataSize = 0;
		tupleIndex		  = 0;		

		iSharedTupleInx	  = -1;
		vtVariationPoint_X.clear();
		vtVariationPoint_Y.clear();		

	}	// end of TupleVariationHeader::TupleVariationHeader()

	TupleVariationHeader::~TupleVariationHeader()
	{

	}	// end of TupleVariationHeader::~TupleVariationHeader()

	unsigned short TupleVariationHeader::SetTupleindex()
	{
		return 0;

	}	// end of unsigned short TupleVariationHeader::SetTupleindex()

	pointNumbers::pointNumbers()
	{
		usPointNumer = 0;

	}	//end of pointNumbers::pointNumbers()

	pointNumbers::~pointNumbers()
	{
		usPointNumer = 0;
		vtPointIndex.clear();

	}	// end of pointNumbers::~pointNumbers();

	glyphVariationData::glyphVariationData()
	{
		tupleVariationCount = 0;
		offsetToData		= 0;

	}	// end of glyphVariationData::glyphVariationData()

	glyphVariationData::~glyphVariationData()
	{
		vtGlyphVariationData.clear();

	}	// end of glyphVariationData::~glyphVariationData()

	BOOL  glyphVariationData::IsVariate()
	{
		for (size_t i = 0; i < vttupleVariationHeaders.size(); i++){
			TupleVariationHeader& Header = vttupleVariationHeaders[i];
			size_t stSize = Header.vtVariationPoint_X.size();
			for (size_t j = 0; j < stSize; j++){
				if (Header.vtVariationPoint_X[j] != 0)
					return TRUE;
				if (Header.vtVariationPoint_Y[j] != 0)
					return TRUE;
			}
		}

		return FALSE;

	}	// end of BOOL  glyphVariationData::IsVariate()

	CHYgvar::CHYgvar(void)
	{
		majorVersion = 0;
		minorVersion = 0;
		axisCount = 0;
		sharedTupleCount = 0;
		offsetToSharedTuples = 0;
		glyphCount = 0;
		flags = 0;
		offsetToGlyphVariationData = 0;
		vtOffsets.clear();
		sharedTuples.clear();
		vtGlyphVariationData.clear();

	}	// end of HYgvar::HYgvar()

	CHYgvar::~CHYgvar(void)
	{

	}	// end of HYgvar::~HYgvar()

	void CHYgvar::setDefault()
	{		
		majorVersion			= 0;
		minorVersion			= 0;
		axisCount				= 0;
		sharedTupleCount		= 0;
		offsetToSharedTuples	= 0;
		glyphCount				= 0;
		flags					= 0;
		offsetToGlyphVariationData = 0;//Offset from the start of this table to the array of glyphVariationData tables.
		vtOffsets.clear();

	}	// end of void CHYgvar::setDefault()

}

