#include "StdAfx.h"
#include "HYFeatureRecord.h"

namespace HYFONTCODEC
{
	CHYFeatureTable::CHYFeatureTable()
	{

	}	// end of CHYFeatureTable::CHYFeatureTable()

	CHYFeatureTable::~CHYFeatureTable()
	{

	}	// end of CHYFeatureTable::~CHYFeatureTable()

	CHYFeatureRecord::CHYFeatureRecord()
	{

	}	// end of CHYFeatureRecord::CHYFeatureRecord()

	CHYFeatureRecord::~CHYFeatureRecord()
	{
		memset(Tag,0,4);

	}	// end of CHYFeatureRecord::~CHYFeatureRecord()
}

