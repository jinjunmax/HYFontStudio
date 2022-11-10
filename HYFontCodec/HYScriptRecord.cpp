#include "StdAfx.h"
#include "HYScriptRecord.h"

namespace HYFONTCODEC
{
	CHYLangSys::CHYLangSys()
	{
		ReqFeatureIndex = 0;
		FeatureCount	= 0;

	}	// end of CHYLangSys::CHYLangSys()

	CHYLangSys::~CHYLangSys()
	{

	}	// end of CHYLangSys::~CHYLangSys()

	CHYLangSysRecord::CHYLangSysRecord()
	{
		memset(SysTag,0,4);

	}	// end of CHYLangSysRecord::CHYLangSysRecord()

	CHYLangSysRecord::~CHYLangSysRecord()
	{

	}	// end of CHYLangSysRecord::~CHYLangSysRecord()

	CHYScript::CHYScript()
	{

	}	// end of CHYScript::CHYScript()

	CHYScript::~CHYScript()
	{

	}	// end of CHYScript::~CHYScript()

	CHYScriptRecord::CHYScriptRecord()
	{
		memset(Tag,0,4);			

	}	// end of CHYScriptRecord::CHYScriptRecord()

	CHYScriptRecord::~CHYScriptRecord()
	{

	}	// end of CHYScriptRecord::~CHYScriptRecord()
}

