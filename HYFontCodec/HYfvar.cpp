#include "StdAfx.h"
#include "HYfvar.h"

namespace HYFONTCODEC
{
	CHYFvarAxisRecord::CHYFvarAxisRecord()
	{
		Tag = 0;
		minValue.value=0;	
		minValue.fract=0;
		defaultValue.value=0;
		defaultValue.fract=0;
		maxValue.value=0;
		maxValue.fract=0;
		flags = 0;
		axisNameID=0;

	}	// end of CHYFvarAxisRecord::CHYFvarAxisRecord()

	CHYFvarAxisRecord::~CHYFvarAxisRecord()
	{

	}	// end of CHYFvarAxisRecord::~CHYFvarAxisRecord()

	CHYFvarInstncRecord::CHYFvarInstncRecord()
	{
		subfamilyNameID		= 0;
		flags				= 0;		
		postScriptNameID	= 0xFFFF;

	}	// end of CHYFvarInstncRecord::CHYFvarInstncRecord()

	CHYFvarInstncRecord::~CHYFvarInstncRecord()
	{

	}	// end of CHYFvarInstncRecord::~CHYFvarInstncRecord()

	CHYfvar::CHYfvar(void)
	{
		majorVersion = 1;
		minorVersion = 0;
		offsetToAxesArray = 0;
		reserved = 2;
		axisCount = 0;
		axisSize = 20;
		instanceCount = 0;
		instanceSize = 0;	

	}	// end of CHYfvar::CHYfvar(void)

	CHYfvar::~CHYfvar(void)
	{

	}	// end of CHYfvar::~CHYfvar(void)

	void CHYfvar::setDefault()
	{
		majorVersion = 1;
		minorVersion = 0;
		offsetToAxesArray = 0;
		reserved = 2;
		axisCount = 0;
		axisSize = 20;
		instanceCount = 0;
		instanceSize = 0;	

		vtaxes.clear();		
		vtinstances.clear();

	}	// end of void CHYfvar::setDefault()

	CHYFvarAxisRecord* CHYfvar::GetAxisRecord(unsigned long Tag)
	{
		for (size_t i=0; i<vtaxes.size(); i++)
		{
			CHYFvarAxisRecord& Record = vtaxes[i];
			if (Record.Tag == Tag)
			{
				return &Record;
			}
		}
		return 0L;

	}	// end of CHYFvarAxisRecord& CHYfvar::GetAxisRecord()

		// 通过NameID获取	指定AxisRecord
	BOOL CHYfvar::FindAxisRecord(int iNameID, CHYFvarAxisRecord& AxisRcrd)
	{
		size_t stSize = vtaxes.size();	
		for (size_t i = 0; i < stSize; i++)
		{
			if (vtaxes[i].axisNameID == iNameID)
			{
				AxisRcrd.Tag = vtaxes[i].Tag;
				AxisRcrd.minValue = vtaxes[i].minValue;
				AxisRcrd.defaultValue = vtaxes[i].defaultValue;
				AxisRcrd.maxValue = vtaxes[i].maxValue;
				AxisRcrd.flags = vtaxes[i].flags;
				AxisRcrd.axisNameID = vtaxes[i].axisNameID;
				return TRUE;
			}
		}

		return FALSE;	

	}	// end of void CHYfvar::FindAxisRecord()

	BOOL CHYfvar::FindInstanceRecord(int iNameID, CHYFvarInstncRecord& InStanceRcrd)
	{
		size_t stSize = vtinstances.size();
		for (size_t i = 0; i < stSize; i++)
		{
			if (vtinstances[i].subfamilyNameID == iNameID)
			{
				InStanceRcrd.subfamilyNameID = vtinstances[i].subfamilyNameID;
				InStanceRcrd.flags = vtinstances[i].flags;

				for (size_t j = 0; j < vtinstances[i].coordinates.size(); j++)
				{
					HYFIXED fixed;
					fixed.value = vtinstances[i].coordinates[j].value;
					fixed.fract = vtinstances[i].coordinates[j].fract;

					InStanceRcrd.coordinates.push_back(fixed);
				}
				InStanceRcrd.postScriptNameID = vtinstances[i].postScriptNameID;

				return TRUE;
			}
		}

		return FALSE;

	}	// end of void CHYfvar::FindInstanceRecord()

}
