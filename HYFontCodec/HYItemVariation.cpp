#include "stdafx.h"
#include "HYItemVariation.h"

namespace HYFONTCODEC
{ 
	RegionAxisCoordinates::RegionAxisCoordinates()
	{
		startCoord = 0;
		peakCoord = 0;
		endCoord = 0;

	}	// end of RegionAxisCoordinates::RegionAxisCoordinates()

	RegionAxisCoordinates::~RegionAxisCoordinates()
	{
		startCoord = 0;
		peakCoord = 0;
		endCoord = 0;

	}	// end of RegionAxisCoordinates::~RegionAxisCoordinates()

	VariationRegion::VariationRegion()
	{
		axisCount = 0;

	}	// end of VariationRegion::VariationRegion()

	VariationRegion::~VariationRegion()
	{
		axisCount = 0;
		vtRegionAxes.clear();

	}	// end of VariationRegion::~VariationRegion()

	VariationRegionList::VariationRegionList()
	{
		axisCount = 0; 
		regionCount = 0;	

	}	// end of VariationRegionList::VariationRegionList()

	VariationRegionList::~VariationRegionList()
	{
		SetDefault();

	}	// end of VariationRegionList::~VariationRegionList()

	void VariationRegionList::SetDefault()
	{
		axisCount = 0;
		regionCount = 0;
		vtVariationRegion.clear();

	}	// end of void VariationRegionList::SetDefault()

	CHYDeltaSet::CHYDeltaSet()
	{

	}	// end of CHYDeltaSet::CHYDeltaSet()

	CHYDeltaSet::~CHYDeltaSet()
	{
		vtDeltaData.clear();

	}	// end of CHYDeltaSet::~CHYDeltaSet()

	CHYItemVariationData::CHYItemVariationData()
	{
		ItemCount = 0;
		ShortDeltaCount = 0;
		RegionIndexCount = 0;		

	}	// end of CHYItemVariationData::CHYItemVariationData()

	CHYItemVariationData::~CHYItemVariationData()
	{
		ItemCount = 0;
		ShortDeltaCount = 0;
		RegionIndexCount = 0;
		vtRegionIndexes.clear();//uint16 	regionIndexes[regionIndexCount]
		vtdeltaSets.clear();	//DeltaSet 	deltaSets[itemCount]

	}	// end of CHYItemVariationData::~CHYItemVariationData()

	CHYItemVariationStore::CHYItemVariationStore()
	{
		format = 0;
		variationRegionListOffset = 0;
		itemVariationDataCount = 0;

	}	// end of CHYItemVariation::CHYItemVariation()

	CHYItemVariationStore::~CHYItemVariationStore()
	{
		SetDefault();

	}	// end of CHYItemVariation::~CHYItemVariation()

	void CHYItemVariationStore::SetDefault()
	{
		format = 0;
		variationRegionListOffset = 0;
		itemVariationDataCount = 0;
		vtitemVariationDataOffsets.clear();
		variationRglst.SetDefault();
		vtItemVariationData.clear();

	}	// end of void CHYItemVariationStore::SetDefault()

	DeltaSetIndexMap::DeltaSetIndexMap()
	{
		entryFormat = 0;
		mapCount = 0;

	}	// end of DeltaSetIndexMap::DeltaSetIndexMap()

	DeltaSetIndexMap::~DeltaSetIndexMap()
	{
		vtmapData.clear();

	}	// end of DeltaSetIndexMap::~DeltaSetIndexMap()

	void DeltaSetIndexMap::SetDefault()
	{
		entryFormat = 0;
		mapCount = 0;
		vtmapData.clear();

	}	// end of void DeltaSetIndexMap::SetDefault()

	void DeltaSetIndexMap::MapIndex2MapData()
	{
		vtmapData.clear();



	}	// end of void DeltaSetIndexMap::MapIndex2MapData()


}	
