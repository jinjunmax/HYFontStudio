#include "StdAfx.h"
#include "HYFpgm.h"

namespace HYFONTCODEC
{
	CHYFpgm::CHYFpgm(void)
	{

	}	// end of CHYFpgm::CHYFpgm()

	CHYFpgm::~CHYFpgm(void)
	{
		vtfgpm.clear();

	}	// end of CHYFpgm::~CHYFpgm()

	void CHYFpgm::SetDefault()
	{
		vtfgpm.clear();

	}	// end of void CHYFpgm::SetDefault()
}