#pragma once
#include <vector>
#include "HYFontCodec.h"
namespace HYFONTCODEC
{
	struct WOFFTableDictEntry
	{
		unsigned long		tag;			//4-byte sfnt table identifier.
		unsigned long		offset;			//Offset to the data, from beginning of WOFF file.
		unsigned long		complength;		//Length of the compressed data, excluding padding.
		unsigned long		origlength;		//Length of the uncompressed table, excluding padding.
		unsigned long		origchecksum;	//Checksum of the uncompressed table.
	};

	class HY_DLL_CLASS WoffCodec
	{
		public:
			WoffCodec(void);
			~WoffCodec(void);

		public:
			int		Woff2Sfnt(const char* pWoffFile, const char* SfntFile);
			int		Sfnt2Woff(const char* SfntFile, const char* pWoffFile, const char* pMetadataFile = 0);

		protected:
			void	SetDefault();
			int		DecodeWoff(const char* pWoffFile);
			int		DecodeWoffHeader(FILE* pWoff);
			int		DecodeWoffTableDictr(FILE* pWoff);		

			int		MakeSfntFont(const char* pSfnt);
			int		DecodeSfntFont(const char* pSfnt);
			void	CompresssnftTableBuf();
			int 	EncodeWoff(const char* pWoff);
			int		EncodeWoffHeader(FILE* pWoff);
			int		EncodeWoffTableDiecrtory(FILE* pWff);
			int		EncodeWoffTableData(FILE* pWff);

		private:
			char*							m_pSfntBuf;
			unsigned	long				m_ulSfntBufSize;
			std::vector<char*>				m_vtsfntorigTblBuf;			
			std::vector<char*>				m_vtsfntcmprsTblBuf;
			char*							m_pCmprsMetaBuf;
			unsigned	long				m_ulCmprsMetaBufSize;
			char*							m_pOrigMetaBuf;
			unsigned	long				m_ulOrigMetaBufSize;

			char*							m_pPrivteBuf;
			unsigned	long				m_ulPrivteBufSize;

			std::vector<WOFFTableDictEntry>	m_vtWoffTblDict;
			CHYFontCodec					m_FontCodec;			
	};
};


