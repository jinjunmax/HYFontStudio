#pragma once

namespace HYFONTCODEC
{
	class HY_DLL_CLASS CHYPrep
	{
	public:
		CHYPrep(void);
		~CHYPrep(void);

		void SetDefault();
	public:
		std::vector<unsigned char> vtPrep;
	};

};

