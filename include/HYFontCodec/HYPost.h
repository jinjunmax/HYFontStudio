#pragma once

#include "HYFontCodecDef.h"


namespace HYFONTCODEC
{
	class HY_DLL_CLASS CPostFormat2
	{
		public:		
			CPostFormat2();
			~CPostFormat2();

			void SetDefault();
			CPostFormat2& operator=(const CPostFormat2& other);

			unsigned short					usNumberOfGlyphs;
			std::vector<unsigned short>		vtGlyphNameIndex;
			std::vector<char>				vtNameLength;
			std::vector<char>				vtNameStrings;
	};

	class HY_DLL_CLASS CPostFormat25
	{
		public:
			CPostFormat25();
			~CPostFormat25();

			void SetDefault();
			CPostFormat25& operator=(const CPostFormat25& other);

			unsigned short					usNumberOfGlyphs;
			std::vector<char>				vtNameOffset;
			std::vector<char>				vtNameStrings;			
	};

	// 兼容V2.0,V2.5,V3.0
	class HY_DLL_CLASS CHYPost
	{
		public:
			CHYPost(void);
			~CHYPost(void);

			void SetDefault();
			CHYPost& operator=(const CHYPost& other);

			std::string			FindNameByGID(unsigned long ulID);
			unsigned short		FindNameIDByGID(unsigned long ulID);
			int					FindGIDbyName(std::string  strPSName);

			// 返回NameID 如果存在同名Name返回同名Name的ID，否则返回新加的ID
			int					InsertName(std::string strName);

		public:	
			HYFIXED				Format;
			HYFIXED				italicAngle;	
			short				underlinePosition;
			short				underlineThickness;
			unsigned long		isFixedPitch;
			unsigned long		minMemType42;
			unsigned long		maxMemType42;
			unsigned long		minMemType1;
			unsigned long		maxMemType1;

			//The names for these 258 glyphs are, in order:
			std::vector<std::string> m_vtStandString;
			CPostFormat2		m_Format2;
			CPostFormat25		m_Format25;


	};
};