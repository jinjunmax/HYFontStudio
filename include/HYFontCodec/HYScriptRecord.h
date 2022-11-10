#pragma once

namespace HYFONTCODEC
{
	class HY_DLL_CLASS CHYLangSys
	{
		public:
			CHYLangSys();
			~CHYLangSys();

		public:
			unsigned short				ReqFeatureIndex;
			unsigned short				FeatureCount;
			std::vector<unsigned short>	vtFeatureIndex;	
	};

	class HY_DLL_CLASS CHYLangSysRecord
	{
		public:
			CHYLangSysRecord();
			~CHYLangSysRecord();
		public:
			char			SysTag[4];
			CHYLangSys		LangSys;
	};	

	class HY_DLL_CLASS CHYScript
	{
		public: 
			CHYScript();
			~CHYScript();
		public:
			CHYLangSys							DefaultLangSys;
			std::vector<CHYLangSysRecord>		vtLangSysRecord;
	};

	class HY_DLL_CLASS CHYScriptRecord
	{
		public:
			CHYScriptRecord();
			~CHYScriptRecord();
		public:
			char								Tag[4];						//  4-byte feature identification tag
			CHYScript							Script;		
	};

};
