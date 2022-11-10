#pragma once

#include <vector>
#include <string>

namespace HYFONTCODEC
{
	class HY_DLL_CLASS LANGTAGRECORD
	{
		public:
			LANGTAGRECORD();
			~LANGTAGRECORD();

			void SetDefault();
		public:
			unsigned short	length;			
			unsigned short	offset;
			std::string		strContent;
	};

	class HY_DLL_CLASS NAMERECORD
	{
		public:
			NAMERECORD();
			~NAMERECORD();

			void SetDefault();
		public:
			unsigned short	platformID;			
			unsigned short	encodingID;	
			unsigned short	languageID;			
			unsigned short	nameID;				
			unsigned short	length;			
			unsigned short	offset;
			std::string		strContent;
	};

	class HY_DLL_CLASS CHYName
	{
		public:
			CHYName(void);
			~CHYName(void);

			void SetDefault();
			CHYName& operator=(const CHYName& other);

			void Sort();
			std::string GetFamilyNameEN();
			std::string GetSubFamilyEN();
			std::string GetFamilyNameCN();
			std::string GetSubFamilyCN();
			std::string GetNamestring(unsigned short nameID);
			std::string GetNamestring(unsigned short	platformID, unsigned short	encodingID, unsigned short	languageID, unsigned short	nameID);

		public:	
			unsigned short					Format;
			unsigned short					count;
			unsigned short					offset;
			std::vector<NAMERECORD>			vtNameRecord;
			std::vector<LANGTAGRECORD>		vtLangTargeRecord;
			std::string						strData;
	};
};

