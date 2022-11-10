#pragma once

namespace HYFONTCODEC
{	
	class HY_DLL_CLASS pointNumbers
	{
	public:
		pointNumbers();
		~pointNumbers();

	public:
		unsigned short					usPointNumer;
		//Each subsequent point number in that run is stored 
		//as the difference between it and the previous point number
		std::vector<unsigned short>		vtPointIndex;
	};

	class HY_DLL_CLASS CHYTuple
	{
		public:
			CHYTuple(void);
			~CHYTuple(void);

		public:
			std::vector<short> coordinates;
	};

	class HY_DLL_CLASS TupleVariationHeader
	{
		public:
			TupleVariationHeader();
			~TupleVariationHeader();
		public:
			unsigned short SetTupleindex();

		public:
			unsigned short			variationDataSize;
			unsigned short 			tupleIndex;
			CHYTuple				peakTuple;
			CHYTuple				intermediateStartTuple;
			CHYTuple				intermediateEndTuple;

			int						iSharedTupleInx;
			std::vector<short>		vtVariationPoint_X;
			std::vector<short>		vtVariationPoint_Y;
			pointNumbers			prvtpointNumbers;
	};	
	
	class HY_DLL_CLASS glyphVariationData
	{
		public:
			glyphVariationData();
			~glyphVariationData();

			BOOL  IsVariate();
		public:
			unsigned short						tupleVariationCount;
			unsigned short						offsetToData;
			std::vector<TupleVariationHeader>	vttupleVariationHeaders;
			pointNumbers						SharepointNumers;

			std::vector<unsigned char>			vtGlyphVariationData;// 在底层封装的时候会用到。
	};

	class HY_DLL_CLASS CHYgvar
	{
		public:
			CHYgvar(void);
			~CHYgvar(void);

			void setDefault();

		public:
			unsigned short						majorVersion;
			unsigned short						minorVersion;
			unsigned short						axisCount;
			unsigned short						sharedTupleCount;
			unsigned long						offsetToSharedTuples;
			unsigned short						glyphCount;
			unsigned short						flags;
			unsigned long 						offsetToGlyphVariationData;
			std::vector<unsigned long>			vtOffsets;
			std::vector<CHYTuple>				sharedTuples;
			std::vector<glyphVariationData>		vtGlyphVariationData;
	};
};
