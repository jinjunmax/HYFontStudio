#pragma once

namespace HYFONTCODEC
{
	class HY_DLL_CLASS CHYDSIG
	{
	public:
		CHYDSIG();
		~CHYDSIG();

		void SetDefault();
		CHYDSIG& operator=(const CHYDSIG& other);


	public:
		unsigned long ulVersion;
		unsigned short usNumSigs;
		unsigned short usFlag;
	};
}