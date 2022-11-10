#include "StdAfx.h"
#include "HYLoca.h"

namespace HYFONTCODEC
{
	CHYLoca::CHYLoca(void)
	{

	} // end of CHYLoca::CHYLoca(void)

	CHYLoca::~CHYLoca(void)
	{
		vtLoca.clear();

	}	// end of CHYLoca::~CHYLoca(void)

	void CHYLoca::SetDefault()
	{
		vtLoca.clear();

	}	// end of void CHYLoca::SetDefault()
}
