#include "StdAfx.h"
#include "HYGasp.h"

namespace HYFONTCODEC
{	
	CHYGasp_RangeRecord::CHYGasp_RangeRecord()
	{

	}	// end of CHYGasp_RangeRecord::CHYGasp_RangeRecord()

	CHYGasp_RangeRecord::~CHYGasp_RangeRecord()
	{

	}	// end of CHYGasp_RangeRecord::~CHYGasp_RangeRecord()

	CHYGasp_Head::CHYGasp_Head()
	{

	}	// end of CHYGasp_Head::CHYGasp_Head()

	CHYGasp_Head::~CHYGasp_Head()
	{

	}	// end of CHYGasp_Head::~CHYGasp_Head()

	CHYGasp::CHYGasp(void)
	{

	}	// end of CHYGasp::CHYGasp(void)

	CHYGasp::~CHYGasp(void)
	{
		SetDefault();

	}	// end of CHYGasp::~CHYGasp(void)

	void CHYGasp::SetDefault()
	{
		Head.version = 0;
		Head.numRanges = 0;
		vtRangeRecord.clear();

	}	// end of void CHYGasp::SetDefault()
}