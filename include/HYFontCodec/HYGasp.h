#pragma once

namespace HYFONTCODEC
{	
	class HY_DLL_CLASS CHYGasp_RangeRecord
	{
	public:
		CHYGasp_RangeRecord();
		~CHYGasp_RangeRecord();

		unsigned short rangeMaxPPEM;
		unsigned short rangeGaspBehavior;
	};

	class HY_DLL_CLASS CHYGasp_Head
	{
	public:
		CHYGasp_Head();
		~CHYGasp_Head();

		unsigned short version;
		unsigned short numRanges;
	};

	class HY_DLL_CLASS CHYGasp
	{
	public:
		CHYGasp(void);
		~CHYGasp(void);

		void	SetDefault();
			
		CHYGasp_Head							Head;
		std::vector<CHYGasp_RangeRecord>		vtRangeRecord;
	};
};