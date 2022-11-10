#include "StdAfx.h"
#include "HYCOLR.h"

namespace HYFONTCODEC
{

	CLayerRecord::CLayerRecord()
	{
		GID				 = 0; 		//Glyph ID of layer glyph (must be in z-order from bottom to top).
		paletteIndex	 = 0; //		

	}	// end of CLayerRecord::CLayerRecord()

	CLayerRecord::~CLayerRecord()
	{

	}	// end of CLayerRecord::~CLayerRecord()

	CLayerRecord& CLayerRecord::operator=(const CLayerRecord& other)
	{
		GID				 = other.GID;
		paletteIndex	 = other.paletteIndex;		

		return *this;

	}	// end of void CLayerRecord::operator=()

	CBaseGlyphRecord::CBaseGlyphRecord()
	{
		GID					= 0;				//Glyph ID of reference glyph. This glyph is for reference only and is not rendered for color.
		firstLayerIndex		= 0;	//Index (from beginning of the Layer Records) to the layer record. There will be numLayers consecutive entries for this base glyph.
		numLayers			= 0;		//Number of color layers associated with this glyph.

	}	// end of CBaseGlyphRecord::CBaseGlyphRecord()

	CBaseGlyphRecord::~CBaseGlyphRecord()
	{
		

	}	// end of CBaseGlyphRecord::~CBaseGlyphRecord()

	void CBaseGlyphRecord::operator=(const CBaseGlyphRecord& other)
	{
		GID					= other.GID;
		firstLayerIndex		= other.firstLayerIndex;
		numLayers			= other.numLayers;

	}	// end of void operator=()

	CHYCOLR::CHYCOLR(void)
	{
		version = 0; 				
		numBaseGlyphRecords = 0;	
		baseGlyphRecordsOffset = 0;	
		layerRecordsOffset = 0;		
		numLayerRecords = 0;

	}	// end of CHYCOLR::CHYCOLR(void)

	CHYCOLR::~CHYCOLR(void)
	{

	}	// end of CHYCOLR::~CHYCOLR(void)

	void CHYCOLR::SetDefault()
	{
		version = 0; 				
		numBaseGlyphRecords = 0;	
		baseGlyphRecordsOffset = 0;	
		layerRecordsOffset = 0;		
		numLayerRecords = 0;

		vtBaseGlyphRecord.clear();
		vtLayerRecord.clear();

	}	// end of void CHYCOLR::SetDefault()

	void CHYCOLR::operator=(const CHYCOLR& other)
	{
		this->version = other.version;
		this->numBaseGlyphRecords = other.numBaseGlyphRecords;
		this->numLayerRecords = other.numLayerRecords;

		this->baseGlyphRecordsOffset = other.baseGlyphRecordsOffset;
		this->layerRecordsOffset = other.layerRecordsOffset;

		this->vtBaseGlyphRecord = other.vtBaseGlyphRecord;
		this->vtLayerRecord = other.vtLayerRecord;

	}	// end of void CHYCOLR::operator=()

	bool CHYCOLR::FindBaseGlyhRecord(int iGID,CBaseGlyphRecord& GlyphRecord)
	{
		size_t st = vtBaseGlyphRecord.size();
		for (size_t i=0; i<st; i++)
		{
			if ( vtBaseGlyphRecord[i].GID == iGID)
			{
				GlyphRecord.GID = vtBaseGlyphRecord[i].GID;
				GlyphRecord.firstLayerIndex = vtBaseGlyphRecord[i].firstLayerIndex;
				GlyphRecord.numLayers = vtBaseGlyphRecord[i].numLayers;

				return true;
			}
		}

		return false;

	}	// end of bool CHYCOLR::FindBaseGlyhRecord()

	void CHYCOLR::FindLayerRecord(int iGID, std::vector<CLayerRecord>& out_vtLayerRecord)
	{
		size_t st = this->vtLayerRecord.size();
		for (size_t i=0; i<st; i++)
		{
			if ( this->vtLayerRecord[i].GID == iGID)
			{
				CLayerRecord tmpLayerRecord;

				tmpLayerRecord.GID =this->vtLayerRecord[i].GID;
				tmpLayerRecord.paletteIndex = this->vtLayerRecord[i].paletteIndex;

				out_vtLayerRecord.push_back(tmpLayerRecord);
			}
		}

	}	// end of bool CHYCOLR::FindLayerRecord()

	void CHYCOLR::FindLayerRecord(int iLayerIndex,int iLayerNum,std::vector<CLayerRecord>& out_vtLayerRecord)
	{	
		for (int i = 0; i<iLayerNum; i++)
		{
			out_vtLayerRecord.push_back(vtLayerRecord[i+ iLayerIndex]);
		}

	}	// end of void CHYCOLR::FindLayerRecord()

}