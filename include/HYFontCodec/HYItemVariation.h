#pragma once

namespace HYFONTCODEC
{
	class HY_DLL_CLASS RegionAxisCoordinates
	{
		public:
			RegionAxisCoordinates();
			~RegionAxisCoordinates();

		public:
			F2Dot14 startCoord;
			F2Dot14 peakCoord;
			F2Dot14 endCoord;
	};	

	class HY_DLL_CLASS VariationRegion
	{
		public:
			VariationRegion();
			~VariationRegion();
		public:
			unsigned short axisCount;
			std::vector<RegionAxisCoordinates>	vtRegionAxes;
	};

	class HY_DLL_CLASS VariationRegionList
	{
	public:
		VariationRegionList();
		~VariationRegionList();
	public:
		void SetDefault();
		
	public:
		unsigned short					axisCount;
		unsigned short					regionCount;
		std::vector<VariationRegion>	vtVariationRegion;
	};	

	class HY_DLL_CLASS CHYDeltaSet
	{
		public:
			CHYDeltaSet();
			~CHYDeltaSet();
		public:
			//Logically, each DeltaSet record has regionIndexCount number of elements.
			//The first shortDeltaCount elements are represented as signed 16 - bit values(int16),
			//and the remaining regionIndexCount - shortDeltaCount elements are 
			//represented as signed 8 - bit values(int8).
			//The length of the data for each row is shortDeltaCount + regionIndexCount.
			std::vector<short> vtDeltaData;
	};

	class HY_DLL_CLASS CHYItemVariationData
	{
		public:
			CHYItemVariationData();
			~CHYItemVariationData();

		public:
			unsigned short				ItemCount;
			unsigned short				ShortDeltaCount;
			unsigned short				RegionIndexCount;
			std::vector<unsigned short>	vtRegionIndexes;//uint16 	regionIndexes[regionIndexCount]
			std::vector<CHYDeltaSet>	vtdeltaSets;	//DeltaSet 	deltaSets[itemCount]
	};

	class HY_DLL_CLASS CHYItemVariationStore
	{
		public:
			CHYItemVariationStore();
			~CHYItemVariationStore();
		public:
			void SetDefault();
		public:
			unsigned short						format;
			unsigned int						variationRegionListOffset;
			unsigned short						itemVariationDataCount;
			std::vector<unsigned long>			vtitemVariationDataOffsets;	//itemVariationDataOffsets[itemVariationDataCount]

			VariationRegionList					variationRglst;
			std::vector<CHYItemVariationData>	vtItemVariationData;
	};	

	class HY_DLL_CLASS DeltaSetIndexMap
	{
		public:
			DeltaSetIndexMap();
			~DeltaSetIndexMap();

		public:
			void SetDefault();
			void MapIndex2MapData();

		public:
			unsigned short 					entryFormat;
			unsigned short 					mapCount;
			std::vector<unsigned char>		vtmapData;
			unsigned short					oneEntrySize;
			std::vector<unsigned short>		vtOuterIndex;
			std::vector<unsigned short>		vtInnerIndex;
	};

}

