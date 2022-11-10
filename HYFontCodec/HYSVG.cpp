#include "stdafx.h"
#include "HYSVG.h"
namespace HYFONTCODEC
{
	CHYSVGHeader::CHYSVGHeader()
	{
		SetDefault();

	}	// end of CHYSVGHeader::CHYSVGHeader()

	CHYSVGHeader::~CHYSVGHeader()
	{
		SetDefault();

	}	// end of CHYSVGHeader::~CHYSVGHeader()

	void CHYSVGHeader::SetDefault()
	{
		version = 0;
		svgDocumentListOffset = 0; // Offset to the SVG Document List, from the start of the SVG table.Must be non - zero.
		reserved = 0; // Set to 0.

	}	// end of void CHYSVGHeader::SetDefault()

	SVGDocumentRecord::SVGDocumentRecord()
	{
		SetDefault();

	}	// end of SVGDocumentRecord::SVGDocumentRecord()

	SVGDocumentRecord::~SVGDocumentRecord()
	{
		SetDefault();

	}	// end of SVGDocumentRecord::~SVGDocumentRecord()

	void SVGDocumentRecord::SetDefault()
	{
		startGlyphID  = 0;
		endGlyphID = 0;
		svgDocOffset = 0;
		svgDocLength = 0;
		vtDocData.clear();

	}	// end of void SVGDocumentRecord::SetDefault()

	SVGDocumentRecord& SVGDocumentRecord::operator=(const SVGDocumentRecord& other)
	{
		startGlyphID = other.startGlyphID;
		endGlyphID = other.endGlyphID;
		svgDocOffset = other.svgDocOffset; 
		svgDocLength = other.svgDocLength;

		vtDocData.clear();
		size_t st = other.vtDocData.size();
		vtDocData.resize(st);
		for (size_t i = 0; i < st; i++)	{			
			vtDocData[i] = other.vtDocData[i];
		}

		return *this;

	}	// end of void SVGDocumentRecord::operator=()

	void SVGDocumentRecord::Copy(SVGDocumentRecord& other)
	{
		startGlyphID = other.startGlyphID;
		endGlyphID = other.endGlyphID;
		svgDocOffset = other.svgDocOffset;
		svgDocLength = other.svgDocLength;

		vtDocData.clear();
		size_t st = other.vtDocData.size();
		vtDocData.resize(st);
		for (size_t i = 0; i < st; i++) {
			vtDocData[i] = other.vtDocData[i];
		}

	}	// end of void SVGDocumentRecord::Copy()

	
	CSVGDocumentList::CSVGDocumentList()
	{
		SetDefault();

	}	// end of CSVGDocumentList::CSVGDocumentList()
		
	CSVGDocumentList::~CSVGDocumentList()
	{
		SetDefault();

	}	// end of CSVGDocumentList::~CSVGDocumentList()

	void CSVGDocumentList::SetDefault()
	{
		numEntries = 0;
		vtDocumentRecord.clear();

	}	// end of void CSVGDocumentList::SetDefault()
	
	CHYSVG::CHYSVG()
	{
		SetDefault();

	}	// end of CHYSVG::CHYSVG()

	CHYSVG::~CHYSVG()
	{
		SetDefault();

	}	// end of CHYSVG::~CHYSVG()

	void CHYSVG::SetDefault()
	{
		Header.SetDefault();
		DocumentList.SetDefault();

	}	// end of void CHYSVG::SetDefault()
}