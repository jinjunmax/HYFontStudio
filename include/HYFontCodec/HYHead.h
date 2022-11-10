#pragma once

#include "HYFontCodecDef.h"

namespace HYFONTCODEC
{
	class HY_DLL_CLASS CHYHead
	{
		public:
		CHYHead(void);
		~CHYHead(void);

		void SetDefault();
		CHYHead& operator=(CHYHead& other);

		public:
		HYFIXED				version;
		HYFIXED				fontRevision;
		unsigned long		checkSumAdjustment;
		unsigned long		magicNumber;
		unsigned short		flags;	
		unsigned short		unitsPerEm;
		char				created[8];	
		char				modified[8];
		short				xMin;					
		short				yMin;					
		short				xMax;					
		short				yMax;			
		unsigned short		macStyle;
		unsigned short		lowestRecPPEM;
		short				fontDirectionHint;	
		short				indexToLocFormat;
		short				glyphDataFormat;	
	};
};