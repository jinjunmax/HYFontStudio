#pragma once

namespace HYFONTCODEC
{
	class HY_DLL_CLASS CHYFeatureTable
	{
	public:
		CHYFeatureTable();
		~CHYFeatureTable();
	public:
		std::vector<unsigned short>				vtLookupListIndex;
	};

	class HY_DLL_CLASS CHYFeatureRecord 
	{
	public:
		CHYFeatureRecord();
		~CHYFeatureRecord();
	public:
		char									Tag[4];
		CHYFeatureTable							FeatureTable;
	};
};
