#include "stdafx.h"
#include "VrAxisRecords.h"

CRegistryAxis::CRegistryAxis()
{
	m_strName = "";
	m_strTag = "";

}	// end of CRegistryAxis::CRegistryAxis()

CRegistryAxis::~CRegistryAxis()
{


}	// end of CRegistryAxis::~CRegistryAxis()

CRegistryAxises::CRegistryAxises()
{
	CRegistryAxis	Axis;
	Axis.m_strName = "Weight";
	Axis.m_strTag = "wght";
	vtRegistryAxise.push_back(Axis);

	Axis.m_strName = "Width";
	Axis.m_strTag = "wdth";
	vtRegistryAxise.push_back(Axis);

	Axis.m_strName = "Italic";
	Axis.m_strTag = "ital";
	vtRegistryAxise.push_back(Axis);

	Axis.m_strName = "Optical size";
	Axis.m_strTag = "opsz";
	vtRegistryAxise.push_back(Axis);

	Axis.m_strName = "Slant";
	Axis.m_strTag = "slnt";
	vtRegistryAxise.push_back(Axis);

}	// end of CRegistryAxises::CRegistryAxises()

CRegistryAxises::~CRegistryAxises()
{

}	// end of CRegistryAxises::~CRegistryAxises()

int	 CRegistryAxises::FindAxis(CString strName)
{
	for (int i = 0; i < vtRegistryAxise.size(); i++) {
		if (strName == vtRegistryAxise[i].m_strName) {
			return i;
		}
	}

	return -1;

}	// end of int CVrAxisRecords::FindAxis()

int	 CRegistryAxises::FindAxisEx(CString strTag)
{
	for (int i = 0; i < vtRegistryAxise.size(); i++) {
		if (strTag == vtRegistryAxise[i].m_strTag) {
			return i;
		}
	}

	return -1;

}	// end of int CVrAxisRecords::FindAxisEx()

CVrAxisValue::CVrAxisValue()
{
	Format			= 0;
	Value			= 0;
	nominalValue	= 0;
	rangeMinValue	= 0;
	rangeMaxValue	= 0;
	linkedValue		= 0;
	valueNameID		= 0;
	valueName		= "";
	axisIndex		= -1;

}	//end of CVrAxisValue::CVrAxisValue()

CVrAxisValue::~CVrAxisValue()
{

}	// end of CVrAxisValue::~CVrAxisValue()

bool CVrAxisValue::operator==(const CVrAxisValue& other)
{
	if (Format != other.Format) return false;
	if( Value != other.Value) return false;
	if (nominalValue != other.nominalValue) return false;
	if (rangeMinValue != other.rangeMinValue) return false;
	if (rangeMaxValue != other.rangeMaxValue) return false;
	if ( linkedValue != other.linkedValue) return false;
	if ( axisIndex != other.axisIndex) return false;

	return true;

}	// end of bool CVrAxisValue::operator==()

bool CVrAxisValue::operator!=(const CVrAxisValue& other)
{
	return !(*this == other);

}	// end of bool CVrAxisValue::operator!=()

CVrAxisRecord::CVrAxisRecord()
{
	m_iNameID = 0;
	m_strName = "";
	m_strTag = "";
	m_bHidden = false;

}	// end of CVrAxisRecord::CVrAxisRecord()

CVrAxisRecord::~CVrAxisRecord()
{

}	// end of CVrAxisRecord::~CVrAxisRecord()

bool CVrAxisRecord::operator==(const CVrAxisRecord& other)
{
	if (m_iNameID != other.m_iNameID)		return false;
	if (m_strName != other.m_strName)		return false;
	if (m_strTag != other.m_strTag)			return false;
	if (m_bHidden != other.m_bHidden)		return false;
	if (m_AxisValue != other.m_AxisValue)	return false;

	return true;

}	// end of bool CVrAxisRecord::Operate == ()

bool CVrAxisRecord::operator!=(const CVrAxisRecord& other)
{
	return !(*this == other);

}	// end of bool CVrAxisRecord::operator!=()

CVrAxisRecords::CVrAxisRecords()
{

}	// end of CVrAxisRecords::CVrAxisRecords()

CVrAxisRecords::~CVrAxisRecords()
{

}	// end of CVrAxisRecords::~CVrAxisRecords()

bool CVrAxisRecords::operator==(const CVrAxisRecords& other)
{
	if (vtVrAxisInfs.size() != other.vtVrAxisInfs.size()) return false;

	for (size_t i = 0; i < vtVrAxisInfs.size(); i++){
		if (vtVrAxisInfs[i] != other.vtVrAxisInfs[i])
			return false;
	}

	return true;

}	// end of bool CVrAxisRecords::operator==()

bool CVrAxisRecords::operator!=(const CVrAxisRecords& other)
{
	return !(*this == other);

}	// end of bool CVrAxisRecords::operator!=()


void CVrAxisRecords::Setdefault()
{
	vtVrAxisInfs.clear();

}	// end of void CVrAxisRecords::Setdefault()

int	 CVrAxisRecords::FindAxisIndex(CString strName)
{
	for (int i = 0; i < vtVrAxisInfs.size(); i++){
		if (strName == vtVrAxisInfs[i].m_strName){
			return i;
		}
	}

	return -1;

}	// end of int CVrAxisRecords::FindAxis()

int	 CVrAxisRecords::FindAxisIndexEx(CString strTag)
{
	for (int i = 0; i < vtVrAxisInfs.size(); i++) {
		if (strTag == vtVrAxisInfs[i].m_strTag) {
			return i;
		}
	}

	return -1;

}	// end of int CVrAxisRecords::FindAxisEx()

float CVrAxisRecords::FindAxisMax(CString strTag)
{
	float Max = ERROR_MAX;
	for (int i = 0; i < vtVrAxisInfs.size(); i++) {
		if (strTag == vtVrAxisInfs[i].m_strTag) {			
			if (vtVrAxisInfs[i].m_AxisValue.Value>Max){
				Max = vtVrAxisInfs[i].m_AxisValue.Value;
			}
		}
	}

	return Max;

}	// end of float CVrAxisRecords::FindAxisMax()

float CVrAxisRecords::FindAxisMin(CString strTag)
{
	float Min = ERROR_MIN;
	for (int i = 0; i < vtVrAxisInfs.size(); i++) {
		if (strTag == vtVrAxisInfs[i].m_strTag) {
			if (vtVrAxisInfs[i].m_AxisValue.Value<Min) {
				Min = vtVrAxisInfs[i].m_AxisValue.Value;
			}
		}
	}

	return Min;

}	// end of float CVrAxisRecords::FindAxisMin() 

