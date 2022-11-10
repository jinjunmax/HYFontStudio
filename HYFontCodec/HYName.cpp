#include "StdAfx.h"
#include "HYName.h"
#include <algorithm>

namespace HYFONTCODEC
{
	LANGTAGRECORD::LANGTAGRECORD()
	{
		length					= 0;
		offset					= 0;
		strContent.empty();

	}	// end of NAMERECORD::LANGTAGRECORD()

	LANGTAGRECORD::~LANGTAGRECORD()
	{

	}	// end of NAMERECORD::~LANGTAGRECORD()

	void LANGTAGRECORD::SetDefault()
	{
		length					= 0;
		offset					= 0;
		strContent.empty();

	}	// end of void NAMERECORD::SetDefault()

	NAMERECORD::NAMERECORD()
	{
		platformID				= 0;			
		encodingID				= 0;	
		languageID				= 0;			
		nameID					= 0;				
		length					= 0;			
		offset					= 0;
		strContent="";

	}	// end of NAMERECORD::NAMERECORD()

	NAMERECORD::~NAMERECORD()
	{
		strContent = "";
	}	// end of NAMERECORD::NAMERECORD()

	void NAMERECORD::SetDefault()
	{
		platformID				= 0;			
		encodingID				= 0;	
		languageID				= 0;			
		nameID					= 0;				
		length					= 0;			
		offset					= 0;
		strContent.empty();

	}	// end of void NAMERECORD::SetDefault()

	CHYName::CHYName(void)
	{	
		Format			= 0;
		count			= 0;
		offset			= 0;

	}	// end of CHYName::CHYName()

	CHYName::~CHYName(void)
	{
		vtNameRecord.clear();
		vtLangTargeRecord.clear();

	}	// end of CHYName::~CHYName()

	void CHYName::SetDefault()
	{
		Format			= 0;
		count			= 0;
		offset			= 0;
		vtNameRecord.clear();
		vtLangTargeRecord.clear();
		strData.empty();

	}	// end of void NAMERECORD::SetDefault()

	CHYName& CHYName::operator=(const CHYName& other)
	{
		if (this == &other)
		{
			return *this;
		}
		else
		{
			Format			= other.Format;
			count			= other.count;
			offset			= other.offset;

			vtNameRecord.clear();			
			for (size_t i=0; i<other.vtNameRecord.size();i++)
			{
				vtNameRecord.push_back(other.vtNameRecord[i]);
			}
			
			vtLangTargeRecord.clear();
			for (size_t i=0; i<other.vtLangTargeRecord.size();i++)
			{
				vtLangTargeRecord.push_back(other.vtLangTargeRecord[i]);
			}

			strData = other.strData;
		}

		return *this;

	}	// end of CHYName& operator=(const CHYName& other)

	std::string CHYName::GetFamilyNameEN()
	{		
		size_t loop = vtNameRecord.size();
		for (size_t i=0; i<loop;i++)
		{
			NAMERECORD& record = vtNameRecord[i];			
			if (record.platformID==1){				
				if (record.encodingID==0){		
					if (record.languageID == 0){
						if (record.nameID == 1){
							return record.strContent;
						}
					}
				}
			}

			if (record.platformID == 3) {
				if (record.encodingID == 1) {
					if (record.languageID == 1033) {
						if (record.nameID == 1) {
							return record.strContent;
						}
					}
				}
			}
		}

		return "";

	}	// end of void CHYName::GetFamilyNameEN()

	std::string CHYName::GetSubFamilyEN()
	{
		size_t loop = vtNameRecord.size();
		for (size_t i = 0; i<loop; i++)
		{
			NAMERECORD& record = vtNameRecord[i];
			if (record.platformID == 1){
				if (record.encodingID == 0){
					if (record.languageID == 0){
						if (record.nameID == 2){
							return record.strContent;
						}
					}
				}
			}

			if (record.platformID == 3) {
				if (record.encodingID == 1) {
					if (record.languageID == 1033) {
						if (record.nameID == 2) {
							return record.strContent;
						}
					}
				}
			}
		}

		return "";

	}	// end of std::string CHYName::GetSubFamilyEN()

	std::string CHYName::GetFamilyNameCN()
	{
		size_t loop = vtNameRecord.size();
		for (size_t i = 0; i<loop; i++)
		{
			NAMERECORD& record = vtNameRecord[i];
			if (record.platformID == 1) {
				if (record.encodingID == 25) {
					if (record.languageID == 33) {
						if (record.nameID == 1) {
							return record.strContent;
						}
					}
				}
			}

			if (record.platformID == 3) {
				if (record.encodingID == 1) {
					if (record.languageID == 2052) {
						if (record.nameID == 1) {
							return record.strContent;
						}
					}
				}
			}
		}

		return "";

	}	// end of void CHYName::GetFontNameCN()

	std::string CHYName::GetSubFamilyCN()
	{
		size_t loop = vtNameRecord.size();
		for (size_t i = 0; i<loop; i++)
		{
			NAMERECORD& record = vtNameRecord[i];
			if (record.platformID == 1) {
				if (record.encodingID == 25) {
					if (record.languageID == 33) {
						if (record.nameID == 2) {
							return record.strContent;
						}
					}
				}
			}

			if (record.platformID == 3) {
				if (record.encodingID == 1) {
					if (record.languageID == 2052) {
						if (record.nameID == 2) {
							return record.strContent;
						}
					}
				}
			}
		}

		return "";

	}	// end of std::string CHYName::GetSubFamilyCN()

	std::string CHYName::GetNamestring(unsigned short nameID)
	{
		size_t loop = vtNameRecord.size();
		for (size_t i=0; i<loop;i++)
		{
			NAMERECORD& record = vtNameRecord[i];
			if (record.nameID==nameID)
			{
				return record.strContent;
			}
		}

		return "";

	}	// end of std::string CHYName::GetNamestring()

	std::string CHYName::GetNamestring(unsigned short	platformID, unsigned short	encodingID, unsigned short	languageID, unsigned short	nameID)
	{
		size_t loop = vtNameRecord.size();
		for (size_t i=0; i<loop;i++)
		{
			NAMERECORD& record = vtNameRecord[i];
			if (record.platformID==platformID&&record.encodingID==encodingID&&record.languageID==languageID&& record.nameID==nameID)
			{
				return record.strContent;
			}
		}

		return "";

	}	// end of std::string CHYName::GetNamestring()

	bool HYNameSortPredicate( NAMERECORD& d1,  NAMERECORD& d2)
	{
		return ( d1.platformID < d2.platformID);

	}	// end of BOOL HYNameSortPredicate()

	void CHYName::Sort()
	{	
		std::sort(vtNameRecord.begin(), vtNameRecord.end(), HYNameSortPredicate);

	}	// end of void CHYName::Sort()

}
