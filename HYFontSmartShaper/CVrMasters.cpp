#include "stdafx.h"
#include "CVrMasters.h"

CVrGlyph::CVrGlyph()
{


}	// end of CVrGlyph::CVrGlyph() 

CVrGlyph::~CVrGlyph()
{


}	// end of CVrGlyph::~CVrGlyph()

CVrGlyphs::CVrGlyphs()
{


}	// end of CVrGlyph::CVrGlyphs() 

CVrGlyphs::~CVrGlyphs()
{


}	// end of CVrGlyph::~CVrGlyphs()

CVrMaster::CVrMaster()
{
	m_strMasterName = "";
	m_strFontName = "";
	m_strFileName = "";
	m_iAcsender = 850;
	m_iDescender = -150;
	m_iTypoAscender = 850;
	m_iTypoDescender = -150;
	m_bDefault = false;

}	// end of CVrMasters::CVrMasters()

CVrMaster::~CVrMaster()
{

}	// end of CVrMasters::~CVrMasters()

void CVrMaster::SetDefault()
{
	m_strMasterName = "";
	m_iAcsender = 850;
	m_iDescender = -150;
	m_iTypoAscender = 850;
	m_iTypoDescender = -150;
	m_bDefault = false;
	m_AxisRecords.Setdefault();
	m_tupNormCoordinates.clear();

}	//end of void CVrMaster::SetDefault()

bool CVrMaster::IsSingleAxis()
{	
	int iTmp = 0;
	for (size_t i = 0; i < m_tupNormCoordinates.size(); i++)
	{
		if (m_tupNormCoordinates[i] != 0.0)
			iTmp++;
	}

	if (iTmp == 1)
		return	true;

	return false;

}	// end of bool CVrMaster::IsSingleAxis()

CVrMasters::CVrMasters()
{

}	// end of CVrMasters::CVrMasters()

CVrMasters::~CVrMasters()
{

}	// end of CVrMaster::~CVrMaster()

CVrMaster* CVrMasters::GetDefaultMater()
{	
	CVrMaster* pMaster = NULL;
	for (size_t i = 0; i < m_vtMaster.size(); i++)
	{
		pMaster = &m_vtMaster[i];
		if (pMaster->m_bDefault)
			return pMaster;
	}

	return pMaster;

}	// end of CVrMaster* CVrMasters::GetDefaultMater()

CVrMaster* CVrMasters::GetMaster(std::vector<float>& tupNormCoordinates)
{	
	CVrMaster* pMaster = NULL;
	for (size_t i = 0; i < m_vtMaster.size(); i++)
	{
		bool b = true;
		pMaster = &m_vtMaster[i];
		if (tupNormCoordinates.size() != pMaster->m_tupNormCoordinates.size()) 
			return pMaster;
		for (size_t j = 0; j < pMaster->m_tupNormCoordinates.size(); j++){
			if (pMaster->m_tupNormCoordinates[j] != tupNormCoordinates[j]) {
				b = false;
				break;
			}
		}
		if (b)
			return pMaster;
	}

	return pMaster;

}	// end of CVrMaster* CVrMasters::GetMaster()

CVrMaster* CVrMasters::GetRangMaxMaster(CString strTag, float axsValue)
{
	CVrMaster* pOutMaster = NULL;
	float fDefault = GetAxisDefault(strTag);
	float fMax = GetAxisMax(strTag);
	
	for (size_t i = 0; i < m_vtMaster.size(); i++)
	{
		CVrMaster* pMaster = &m_vtMaster[i];
		if (!pMaster->IsSingleAxis())
			continue;

		float ftmpValue0 = fDefault;
		float ftmpValue1 = fMax;

		for (size_t j = 0; j < pMaster->m_AxisRecords.vtVrAxisInfs.size(); j++)
		{
			if (pMaster->m_AxisRecords.vtVrAxisInfs[j].m_strTag == strTag)
			{
				if (axsValue < fDefault)
				{
					float ftmpValue = pMaster->m_AxisRecords.vtVrAxisInfs[j].m_AxisValue.Value;
					if (ftmpValue >= axsValue && ftmpValue <= ftmpValue0)
					{
						pOutMaster = pMaster;
						ftmpValue0 = ftmpValue;
					}
				}
				else
				{
					float ftmpValue = pMaster->m_AxisRecords.vtVrAxisInfs[j].m_AxisValue.Value;
					if (ftmpValue >= axsValue && ftmpValue <= ftmpValue1)
					{
						pOutMaster = pMaster;
						ftmpValue1 = ftmpValue;
					}
				}
			}
		}

		if (pOutMaster != NULL)
			return pOutMaster;
	}

	return pOutMaster;

}	// end of CVrMaster* CVrMasters::GetRangMaxMaster()

CVrMaster* CVrMasters::GetRangMinMaster(CString strTag, float axsValue)
{
	CVrMaster* pOutMaster = NULL;
	float fDefault = GetAxisDefault(strTag);
	float fMin = GetAxisMin(strTag);

	for (size_t i = 0; i < m_vtMaster.size(); i++)
	{
		CVrMaster* pMaster = &m_vtMaster[i];
		float ftmpValue0 = fDefault;
		float ftmpValue1 = fMin;

		for (size_t j = 0; j < pMaster->m_AxisRecords.vtVrAxisInfs.size(); j++)
		{
			if (pMaster->m_AxisRecords.vtVrAxisInfs[j].m_strTag == strTag)
			{
				float ftmpValue = pMaster->m_AxisRecords.vtVrAxisInfs[j].m_AxisValue.Value;
				if (axsValue > fDefault)
				{					
					if ((ftmpValue<=axsValue) && (ftmpValue >= ftmpValue0))
					{
						pOutMaster = pMaster;
						ftmpValue0 = ftmpValue;
					}
				}
				else
				{					
					if ((ftmpValue <= axsValue) && (ftmpValue >= ftmpValue1))
					{
						pOutMaster = pMaster;
						ftmpValue1 = ftmpValue;
					}
				}
			}
			else
			{
				float fDefault1 = GetAxisDefault(pMaster->m_AxisRecords.vtVrAxisInfs[j].m_strTag);
				float ftmpValue1 = pMaster->m_AxisRecords.vtVrAxisInfs[j].m_AxisValue.Value;
				if (fDefault1 != ftmpValue1)
				{
					pOutMaster = NULL;
					break;
				}
			}
		}

		if (pOutMaster != NULL)
			return pOutMaster;
	}

	return pOutMaster;

}	// end of CVrMaster* CVrMasters::GetRangMinMaster()

float CVrMasters::GetAxisMax(CString strTag)
{
	float Max = ERROR_MAX;
	for (size_t i = 0; i < m_vtMaster.size(); i++)
	{
		CVrMaster& Master =  m_vtMaster[i];
		float f = Master.m_AxisRecords.FindAxisMax(strTag);
		if (f>Max){
			Max = f;
		}
	}

	return Max;

}	// end of double CVrMasters::GetAxisMax()

float CVrMasters::GetAxisMin(CString strTag)
{
	float Min = ERROR_MIN;
	for (size_t i = 0; i < m_vtMaster.size(); i++)
	{
		CVrMaster& Master = m_vtMaster[i];
		float f = Master.m_AxisRecords.FindAxisMax(strTag);
		if (f<Min) {
			Min = f;
		}
	}

	return Min;

}	// end of double CVrMasters::GetAxisMin()

float CVrMasters::GetAxisDefault(CString strTag)
{
	float fDefault = 0.0f;
	for (size_t i = 0; i < m_vtMaster.size(); i++){
		CVrMaster& Master = m_vtMaster[i];
		if (Master.m_bDefault){
			for (size_t j = 0; j < Master.m_AxisRecords.vtVrAxisInfs.size(); j++) {
				if (Master.m_AxisRecords.vtVrAxisInfs[j].m_strTag == strTag) {
					fDefault = Master.m_AxisRecords.vtVrAxisInfs[j].m_AxisValue.Value;
				}
			}
		}
	}

	return fDefault;

}	// end of float CVrMasters::GetAxisDefault()

CVrInstance::CVrInstance()
{
	m_strStyleName;
	m_iWeight = 400;
	m_iWidth = 5;
	m_AxisRecords.Setdefault();

}	// end of CVrInstance::CVrInstance()

CVrInstance::~CVrInstance()
{
	

}	// end of CVrInstance::~CVrInstance()

void CVrInstance::SetDefault()
{
	m_strStyleName = "";
	m_iWeight = 400;
	m_iWidth = 5;
	m_AxisRecords.Setdefault();
	m_iNameID = 0;

}	// end of void CVrInstance::SetDefault()

bool CVrInstance::operator==(const CVrInstance& other)
{
	if (m_strStyleName != other.m_strStyleName) return false;
	if (m_iWeight != other.m_iWeight) return false;
	if (m_iWidth != other.m_iWidth) return false;
	if (m_AxisRecords != other.m_AxisRecords) return false;
	if (m_iNameID != other.m_iNameID) return false;

	return true;

}	// end of bool CVrInstance::operator==()

bool CVrInstance::operator!=(const CVrInstance& other)
{
	return !(*this == other);

}	// end of bool CVrInstance::operator!=()

CVrInstances::CVrInstances()
{


}	// end of CVrInstances::CVrInstances()

CVrInstances::~CVrInstances()
{


}	// end of CVrInstances::~CVrInstances()

int CVrInstances::FindInstancesIdx(CString StyleName)
{
	for (size_t i = 0; i < m_vtinstances.size(); i++){

		CVrInstance& Instance =  m_vtinstances[i];
		if (Instance.m_strStyleName == StyleName)
			return (int)i;
	}

	return -1;

}	// end of int CVrInstances::FindInstancesIdx()

int CVrInstances::FindInstancesIdx(CVrAxisRecords& inAxisRecords)
{
	for (size_t i = 0; i < m_vtinstances.size(); i++) {

		CVrInstance& Instance = m_vtinstances[i];
		CVrAxisRecords& AxRcrds = Instance.m_AxisRecords;

		if (inAxisRecords == AxRcrds) 
			return (int)i;			
	}

	return -1;

}	// end of int CVrInstances::FindInstancesIdx()

int CVrInstances::IsExist(CVrInstance& inInstance)
{
	for (size_t i = 0; i < m_vtinstances.size(); i++) {

		CVrInstance& Instance = m_vtinstances[i];
		if (Instance == inInstance)
			return (int)i;
	}

	return -1;

}	// end of bool CVrInstances::IsExist()

CVrFontName::CVrFontName()
{
	m_strENGFamilyName = "";
	m_strCHSFamilyName = "";
	m_strPostScriptName = "";
	m_strENGCopyright = "";
	m_strCHSCopyright = "";
	m_strVersion = "";

}	// end of CVrFontName::CVrFontName()

CVrFontName::~CVrFontName()
{


}	// end of CVrFontName::~CVrFontName()


