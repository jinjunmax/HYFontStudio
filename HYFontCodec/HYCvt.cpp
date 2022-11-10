#include "StdAfx.h"
#include "HYCvt.h"

namespace HYFONTCODEC
{
	CHYCvt::CHYCvt(void)
	{
	}	// end of CHYCvt::CHYCvt()

	CHYCvt::~CHYCvt(void)
	{
		vtCvt.clear();

	}	// end of CHYCvt::~CHYCvt()

	void CHYCvt::SetDefault()
	{
		vtCvt.clear();

	}	// end of void CHYCvt::SetDefault()
}
