#include "StdAfx.h"
#include "HYDSIG.h"

namespace HYFONTCODEC
{
	CHYDSIG::CHYDSIG(void)
	{
		ulVersion = 1;
		usNumSigs = 0;
		usFlag  = 0;

	}	// end of CHYDSIG::CHYDSIG()

	CHYDSIG::~CHYDSIG(void)
	{

	}	// end of CHYDSIG::~CHYDSIG()


	void CHYDSIG::SetDefault()
	{
		ulVersion = 0x00000001;
		usNumSigs = 0;
		usFlag = 0;

	}	// end of void CHYDSIG::SetDefault()

	CHYDSIG& CHYDSIG::operator =(const CHYDSIG& other)
	{
		if (this == &other)
		{
			return *this;
		}
		else 
		{
			ulVersion = other.ulVersion;
			usNumSigs = other.usNumSigs;
			usFlag = other.usFlag;
		}

		return *this;

	}	// end of CHYDSIG& CHYDSIG::operator =()
}
