#include "StdAfx.h"
#include "HYSTAT.h"

namespace HYFONTCODEC
{
	CHYAxisValues::CHYAxisValues()
	{
		axisIndex = 0;
		fxValue.value = 0;
		fxValue.fract = 0;

	}	// end of CHYAxisValues::CHYAxisValues()

	CHYAxisValues::~CHYAxisValues()
	{

	}	// end of CHYAxisValues::CHYAxisValues()

	CHYAxisvalueFormat::CHYAxisvalueFormat()
	{
		format					= 0;
		axisIndex				= 0;
		flags					= 0;
		valueNameID				= 0;
		nominalValue.value		= 0;
		nominalValue.fract		= 0;
		rangeMinValue.value		= 0;
		rangeMinValue.fract		= 0;
		rangeMaxValue.value		= 0;
		rangeMaxValue.fract		= 0;
		fxValue.value			= 0;
		fxValue.fract			= 0;
		linkedValue.value		= 0;
		linkedValue.fract		= 0;
		axisCount				= 0;

	}	// end of CHYAxisvalueFormat::CHYAxisvalueFormat()

	CHYAxisvalueFormat::~CHYAxisvalueFormat()
	{


	}	// end of CHYAxisvalueFormat::~CHYAxisvalueFormat()
	
	/////////////////////////////////////////////////////////////
	CHYAxisRecord::CHYAxisRecord()
	{
		axisTag	 = 0;
		axisNameID = 0;
		axisOrdering = 0;

	}	// end of CHYAxisRecord::CHYAxisRecord()

	CHYAxisRecord::~CHYAxisRecord()
	{

	}	// end of CHYAxisRecord::~CHYAxisRecord()

	////////////////////////////////////////////////////////////	
	CHYSTAT::CHYSTAT()
	{
		majorVersion				= 1;
		minorVersion				= 2;
		designAxisSize				= 0;
		designAxisCount				= 0;
		designAxesOffset			= 0;
		axisValueCount				= 0;
		offsetToAxisValueOffsets	= 0;
		elidedFallbackNameID		= 0;		

	}	// end of CHYSTAT::CHYSTAT()

	CHYSTAT::~CHYSTAT()
	{

	}	// end of CHYSTAT::~CHYSTAT()

	void CHYSTAT::SetDefault()
	{
		majorVersion				= 0;
		minorVersion				= 0;
		designAxisSize				= 0;
		designAxisCount				= 0;
		designAxesOffset			= 0;
		axisValueCount				= 0;
		offsetToAxisValueOffsets	= 0;
		elidedFallbackNameID		= 0;

		vtdesignAxes.clear();
		axisValueOffsets.clear();
		vtAxisvalueFormat.clear();		

	}	// end of void CHYSTAT::SetDefault()

	BOOL CHYSTAT::GetAxisValueList(int idsgAxisIndex, std::vector<CHYAxisvalueFormat>& vtAxsVulFrmat)
	{
		vtAxsVulFrmat.clear();

		for (size_t i = 0; i < vtAxisvalueFormat.size(); i++)
		{
			CHYAxisvalueFormat AxisvalueFormat;

			if (vtAxisvalueFormat[i].format != 4)
			{
				if (vtAxisvalueFormat[i].axisIndex == idsgAxisIndex)
				{	
					AxisvalueFormat.axisIndex = vtAxisvalueFormat[i].axisIndex;
					AxisvalueFormat.flags = vtAxisvalueFormat[i].flags;
					AxisvalueFormat.format = vtAxisvalueFormat[i].format;
					AxisvalueFormat.fxValue = vtAxisvalueFormat[i].fxValue;
					AxisvalueFormat.linkedValue = vtAxisvalueFormat[i].linkedValue;
					AxisvalueFormat.nominalValue = vtAxisvalueFormat[i].nominalValue;
					AxisvalueFormat.rangeMaxValue = vtAxisvalueFormat[i].rangeMaxValue;
					AxisvalueFormat.rangeMinValue = vtAxisvalueFormat[i].rangeMinValue;
					AxisvalueFormat.valueNameID = vtAxisvalueFormat[i].valueNameID;

					vtAxsVulFrmat.push_back(AxisvalueFormat);
				}

			}
			else
			{
				AxisvalueFormat.axisCount = vtAxisvalueFormat[i].axisCount;
				for (size_t j = 0; j < AxisvalueFormat.axisCount; j++)
				{
					if (vtAxisvalueFormat[i].vtAxisValues[j].axisIndex = idsgAxisIndex)
					{
						CHYAxisValues	AxisValue;
						AxisValue.axisIndex = vtAxisvalueFormat[i].vtAxisValues[j].axisIndex;
						AxisValue.fxValue = vtAxisvalueFormat[i].vtAxisValues[j].fxValue;
						AxisvalueFormat.vtAxisValues.push_back(AxisValue);

						vtAxsVulFrmat.push_back(AxisvalueFormat);
					}
				}
			}
		}

		return TRUE;

	}	// end of BOOL CHYSTAT::GetAxisValueList()


}

