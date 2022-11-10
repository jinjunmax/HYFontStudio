#pragma once

namespace HYFONTCODEC
{
	
	class CHYSVGHeader
	{
	public:
		CHYSVGHeader();
		~CHYSVGHeader();
		void SetDefault();

		unsigned short version;	//Table version(starting at 0).Set to 0.
		unsigned long svgDocumentListOffset; // Offset to the SVG Document List, from the start of the SVG table.Must be non - zero.
		unsigned long reserved; // Set to 0.
	};	
	
	class SVGDocumentRecord
	{
	public:
		SVGDocumentRecord();
		~SVGDocumentRecord();
		void SetDefault();
		SVGDocumentRecord& operator=(const SVGDocumentRecord& other);

		void Copy(SVGDocumentRecord& other);
		
	public:
		unsigned short startGlyphID; //The first glyph ID for the range covered by this record.
		unsigned short endGlyphID;//The last glyph ID for the range covered by this record.
		unsigned long svgDocOffset; //Offset from the beginning of the SVGDocumentList to an SVG document.Must be non - zero.
		unsigned long svgDocLength; //Length of the SVG document data.Must be non - zero.
		std::vector<unsigned char> vtDocData;
	};

	class CSVGDocumentList
	{
	public:
		CSVGDocumentList();
		~CSVGDocumentList();
		void SetDefault();

	public:
		unsigned short  	numEntries;// 	Number of SVG document records.Must be non - zero.
		std::vector<SVGDocumentRecord>	vtDocumentRecord;			
	};
	
	class CHYSVG
	{
		public:
			CHYSVG();
			~CHYSVG();
			void SetDefault();

		public:
			CHYSVGHeader Header;
			CSVGDocumentList  DocumentList;
	}; 

}


