#pragma once

namespace HYFONTCODEC
{
	
	class HY_DLL_CLASS CLayerRecord
	{
		public:
			CLayerRecord();
			~CLayerRecord();
		public:
			unsigned short 	GID; 		//Glyph ID of layer glyph (must be in z-order from bottom to top).
			unsigned short  paletteIndex; //	
			CLayerRecord&	operator=(const CLayerRecord& other);

	};

	class HY_DLL_CLASS CBaseGlyphRecord
	{
		public:
			CBaseGlyphRecord();
			~CBaseGlyphRecord();

		public:
			unsigned short GID;				//Glyph ID of reference glyph. This glyph is for reference only and is not rendered for color.
			unsigned short firstLayerIndex;	//Index (from beginning of the Layer Records) to the layer record. There will be numLayers consecutive entries for this base glyph.
			unsigned short numLayers;		//Number of color layers associated with this glyph.
		public:
			void operator=(const CBaseGlyphRecord& other);
	};
	
	class HY_DLL_CLASS CHYCOLR
	{
		public:
			CHYCOLR(void);
			~CHYCOLR(void);

			void SetDefault();

			unsigned short 	version; 				//	Table version number (starts at 0).
			unsigned short	numBaseGlyphRecords;	//	Number of Base Glyph Records.
			unsigned int	baseGlyphRecordsOffset;	// 	Offset (from beginning of COLR table) to Base Glyph records.
			unsigned int	layerRecordsOffset;		// 	Offset (from beginning of COLR table) to Layer Records.
			unsigned short  numLayerRecords;		// 	Number of Layer Records.

		public:
			std::vector<CBaseGlyphRecord>  vtBaseGlyphRecord;
			std::vector<CLayerRecord>  vtLayerRecord;

		public:

			void	operator=(const CHYCOLR& other);
			bool	FindBaseGlyhRecord(int iGID,CBaseGlyphRecord& GlyphRecord);
			void 	FindLayerRecord(int iGID, std::vector<CLayerRecord>& vtLayerRecord);
			void	FindLayerRecord(int iLayerIndex, int iLayerNum, std::vector<CLayerRecord>& out_vtLayerRecord);

	};
}


