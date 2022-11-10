#pragma once
#include "HYFontCodecHead.h"

namespace HYFONTCODEC
{
	class HY_DLL_CLASS CHYTableData
	{
	public:
		CHYTableData();
		~CHYTableData();
	public:
		std::vector<unsigned char>		vtData;
	};

	class HY_DLL_CLASS CHYTabelShareInfo
	{
		public:
			CHYTabelShareInfo();
			~CHYTabelShareInfo();
		public:
			int								FontID;
			unsigned long 					checkSum;
			unsigned long 					tbTag;
			unsigned long					lenth;
			unsigned long					Pos;
	};	

	class HY_DLL_CLASS CHYFontData
	{
	public:
		CHYFontData();
		~CHYFontData();
	public:
		CHYTableEntry * FindTableEntry(unsigned long& tag);
		CHYTableData* FindTableData(unsigned long& tag);
	public:
		CHYTableDirectory			TableDirectory;
		std::vector<CHYTableData>	vtTableData;		
	};

	class HY_DLL_CLASS CTTCHeader
	{
	public:
		CTTCHeader();
		~CTTCHeader();
	public:
		unsigned long 				ttcTag;			// Font Collection ID string : 'ttcf'		 
		unsigned short 				majorVersion;	// Major version of the TTC Header, = 2.
		unsigned short 				minorVersion;	// Minor version of the TTC Header, = 0.
		unsigned long 				numFonts; 		// Number of fonts in TTC
		std::vector<unsigned long>  vtOffsetTable;  //Array of offsets to the OffsetTable for each font from the beginning of the file

		unsigned long 	dsigTag; 			//Tag indicating that a DSIG table exists, 0x44534947 ('DSIG') (null if no signature)
		unsigned long 	dsigLength; 		//The length(in bytes) of the DSIG table(null if no signature)
		unsigned long 	dsigOffset; 		//The offset(in bytes) of the DSIG table from the beginning of the TTC file(null if no signature)
	};

	class HY_DLL_CLASS CHYTTC
	{
	public:
		CHYTTC();
		~CHYTTC();
	public:
		int  TTC2Font(std::string strTTC);
		int  MakeFont(CHYFontData& FontData, const std::string& strFontName);
		
		int Font2TTC(std::string strTTC, std::vector<string>& Fonts, std::vector<string>* CharsTable = NULL);

		// 简单模式
		int Font2TTCSimaple(std::string strTTC, std::vector<string>& Fonts);
		// 复杂模式
		int Font2TTCComplex(std::string strTTC, std::vector<string>& Fonts, std::vector<string>* pCharsTable);

		// 构建ttc文件中的font部分
		void MakeTTCFont(FILE* FontFile, int iFontID);

	protected:
		void GetFontData(FILE* pttcFile, CHYFontData& FontData);
		int  IsShare(unsigned long usTag, unsigned long checkSum);
	private:
		std::vector<CHYFontData>			m_vtFontData;		
		CTTCHeader							m_TTCHead;
		std::vector<CHYTabelShareInfo>		m_vttbShareInf;
	};
}
