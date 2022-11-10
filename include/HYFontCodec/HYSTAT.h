#pragma once

namespace HYFONTCODEC
{
	class  HY_DLL_CLASS CHYAxisValues
	{
		public:
			CHYAxisValues();
			~CHYAxisValues();

		public:
			unsigned short	axisIndex;
			HYFIXED			fxValue;
	};	
	 
	class HY_DLL_CLASS CHYAxisvalueFormat
	{
	public:
		CHYAxisvalueFormat();
		~CHYAxisvalueFormat();

		unsigned short					format;
		unsigned short					axisIndex;
		unsigned short					flags;
		unsigned short					valueNameID;
		HYFIXED							nominalValue;
		HYFIXED							rangeMinValue;
		HYFIXED							rangeMaxValue;
		HYFIXED							fxValue;
		HYFIXED							linkedValue;
		unsigned short					axisCount;
		std::vector<CHYAxisValues>		vtAxisValues;
	};

	class HY_DLL_CLASS CHYAxisRecord
	{
		public:
			CHYAxisRecord();
			~CHYAxisRecord();
		public: 
			unsigned long	axisTag;
			unsigned short	axisNameID;
			unsigned short	axisOrdering;
	};

	class HY_DLL_CLASS CHYSTAT
	{
		public:
			CHYSTAT(void);
			~CHYSTAT(void);

		public:
			void SetDefault();

		public:
			BOOL GetAxisValueList(int idsgAxisIndex, std::vector<CHYAxisvalueFormat>& vtAxsVulFrmat);

		public:
			unsigned short					majorVersion;
			unsigned short					minorVersion;
			unsigned short					designAxisSize;
			unsigned short					designAxisCount;
			unsigned long					designAxesOffset;
			unsigned short					axisValueCount;
			unsigned long					offsetToAxisValueOffsets;
			unsigned short					elidedFallbackNameID;
			std::vector<CHYAxisRecord>		vtdesignAxes; 
			std::vector<unsigned short>		axisValueOffsets;			
			std::vector<CHYAxisvalueFormat>	vtAxisvalueFormat;
	};
};

