#include "StdAfx.h"
#include "WoffCodec.h"
#include "HYFontCodecHead.h"
#include "zlib.h"

namespace HYFONTCODEC
{
	#define WOFF_FLAG	0x774F4646

	WoffCodec::WoffCodec(void)
	{
		m_pSfntBuf				= NULL;
		m_ulSfntBufSize			= 0;
		m_pCmprsMetaBuf			= NULL;
		m_ulCmprsMetaBufSize	= 0;
		m_pOrigMetaBuf			= NULL;
		m_ulOrigMetaBufSize		= 0;
		m_pPrivteBuf			= NULL;
		m_ulPrivteBufSize		= 0;

	}	// end of WoffCodec::WoffCodec()

	WoffCodec::~WoffCodec(void)
	{
		SetDefault();

	}	// end of WoffCodec::~WoffCodec()

	int WoffCodec::Woff2Sfnt(const char* pWoffFile, const char* SfntFile)
	{
		int iRtn = HY_NOERROR;
		SetDefault();

		iRtn = DecodeWoff(pWoffFile);
		if (iRtn==HY_NOERROR)
		{
			iRtn = MakeSfntFont(SfntFile);
		}

		return iRtn;

	}	// end of int WoffCodec::Woff2Sfnt()

	int WoffCodec::Sfnt2Woff(const char* SfntFile,const char* pWoffFile,const char* pMetadataFile)
	{
		int Ret = HY_NOERROR;

		SetDefault();
		if (pMetadataFile)
		{
			m_ulOrigMetaBufSize = HY_FileLength(pMetadataFile);
			if (m_ulOrigMetaBufSize>0)
			{				
				m_pOrigMetaBuf = new char[m_ulOrigMetaBufSize];
				FILE* pMetaFile = fopen(pMetadataFile,"rb");
				if (pMetaFile)
				{
					fread(m_pOrigMetaBuf,m_ulOrigMetaBufSize,1,pMetaFile);
					fflush(pMetaFile);
					fclose(pMetaFile);

					m_ulCmprsMetaBufSize = compressBound(m_ulOrigMetaBufSize);
					m_pCmprsMetaBuf = new char[m_ulCmprsMetaBufSize];
					memset(m_pCmprsMetaBuf, 0, m_ulCmprsMetaBufSize);

					compress2((Bytef*)m_pCmprsMetaBuf,&m_ulCmprsMetaBufSize,(Bytef*)m_pOrigMetaBuf,m_ulOrigMetaBufSize,Z_DEFAULT_COMPRESSION);
				}
			}
		}

		Ret = DecodeSfntFont(SfntFile);
		if (Ret==HY_NOERROR)
		{
			CompresssnftTableBuf();
			EncodeWoff(pWoffFile);
		}

		return Ret;

	}	// end of int WoffCodec::Sfnt2Woff()

	void	WoffCodec::SetDefault()
	{
		if (m_pSfntBuf)
			delete[] m_pSfntBuf;
		m_pSfntBuf		= NULL;
		m_ulSfntBufSize = 0;

		if (m_pCmprsMetaBuf)
			delete[] m_pCmprsMetaBuf;
		m_pCmprsMetaBuf			= NULL;
		m_ulCmprsMetaBufSize	= 0;

		if (m_pOrigMetaBuf)
			delete[] m_pOrigMetaBuf;
		m_pOrigMetaBuf		= NULL;
		m_ulOrigMetaBufSize	= 0;		

		if (m_pPrivteBuf)
			delete[] m_pPrivteBuf;
		m_pPrivteBuf		= NULL;
		m_ulPrivteBufSize	= 0;

		for (size_t i=0; i<m_vtsfntorigTblBuf.size(); i++)
		{
			if (m_vtsfntorigTblBuf[i]) delete[] m_vtsfntorigTblBuf[i];
		}
		m_vtsfntorigTblBuf.clear();			


		for (size_t i=0; i<m_vtsfntcmprsTblBuf.size(); i++)
		{

			if(m_vtsfntcmprsTblBuf[i]) delete[] m_vtsfntcmprsTblBuf[i];
		}
		m_vtsfntcmprsTblBuf.clear();

		m_vtWoffTblDict.clear();	
		m_FontCodec.SetDefault();

	}	// end of int	WoffCodec::SetDefault()

	int WoffCodec::DecodeWoff(const char* pWoffFile)
	{
		SetDefault();

		FILE* pWoff = fopen(pWoffFile, "rb");
		if (pWoff==0) return HY_ERR_FILE_OPEN;

		if (DecodeWoffHeader(pWoff) != 0)
		{
			SetDefault();
			return  WOFF_ERR_DECODE;
		}		

		if (DecodeWoffTableDictr(pWoff) != 0)
		{
			SetDefault();
			return WOFF_ERR_DECODE;
		}

		return HY_NOERROR;

	}	// end of int WoffCodec::DecodeWoff()

	int WoffCodec::DecodeWoffHeader(FILE* pWoff)
	{
		unsigned long	ulTmp = 0;
		unsigned short	usTmp = 0;

		// decode WOFF Header
		fread(&ulTmp, 4,1,pWoff);
		ulTmp = hy_cdr_int32_to(ulTmp);

		if (ulTmp != WOFF_FLAG)			return -1;
		//sfnt version
		fread(&ulTmp,4,1,pWoff);
		ulTmp = hy_cdr_int32_to(ulTmp);
		m_FontCodec.m_HYTbDirectory.version.value = ulTmp>>16;
		m_FontCodec.m_HYTbDirectory.version.fract = ulTmp&0x0000ffff;

		//wofflength
		fread(&ulTmp,4,1,pWoff);
		ulTmp = hy_cdr_int32_to(ulTmp);
		//numTables
		fread(&usTmp,2,1,pWoff);
		usTmp = hy_cdr_int16_to(usTmp);
		m_FontCodec.m_HYTbDirectory.numTables = usTmp;

		m_vtsfntorigTblBuf.resize(ulTmp);

		//reserved
		fread(&usTmp,2,1,pWoff);
		//totalSfntSize
		fread(&ulTmp,4,1,pWoff);
		m_ulSfntBufSize = hy_cdr_int32_to(ulTmp);
		if (m_ulSfntBufSize>0)
		{
			m_pSfntBuf = new char[m_ulSfntBufSize];
		}
		//majorVersion
		fread(&usTmp,2,1,pWoff);
		//minorVersion
		fread(&usTmp,2,1,pWoff);

		//metaOffset
		fread(&ulTmp,4,1,pWoff);
		unsigned long ulmetaOffset = hy_cdr_int32_to(ulTmp);
		//metaLength
		fread(&ulTmp,4,1,pWoff);		
		m_ulCmprsMetaBufSize = hy_cdr_int32_to(ulTmp);
		if (m_ulCmprsMetaBufSize>0)
		{
			m_pCmprsMetaBuf = new char[m_ulCmprsMetaBufSize];
		}
		//metaOrigLength
		fread(&ulTmp,4,1,pWoff);		
		m_ulOrigMetaBufSize = hy_cdr_int32_to(ulTmp);
		if (m_ulOrigMetaBufSize>0)
		{
			m_pOrigMetaBuf = new char[m_ulOrigMetaBufSize];
		}

		//privOffset
		fread(&ulTmp,4,1,pWoff);
		unsigned long ulprivOffset = hy_cdr_int32_to(ulTmp);
		//privLength
		fread(&ulTmp,4,1,pWoff);
		m_ulPrivteBufSize = hy_cdr_int32_to(ulTmp);
		if(m_ulPrivteBufSize>0)
		{
			m_pPrivteBuf = new char[m_ulPrivteBufSize];
		}

		unsigned long ulCurPos = ftell(pWoff);

		if (m_ulCmprsMetaBufSize)
		{
			fseek(pWoff,ulmetaOffset,SEEK_SET);
			fread(m_pCmprsMetaBuf,m_ulCmprsMetaBufSize,1,pWoff);

			uncompress((Bytef*)m_pOrigMetaBuf,&m_ulOrigMetaBufSize,(Bytef*)m_pCmprsMetaBuf,m_ulCmprsMetaBufSize);
		}

		if (m_ulPrivteBufSize)
		{
			fseek(pWoff,ulprivOffset,SEEK_SET);
			fread(m_pPrivteBuf,m_ulPrivteBufSize,1,pWoff);			
		}

		fseek(pWoff,ulCurPos,SEEK_SET);

		return 0;

	}	// end of int WoffCodec::DecodeWoffHeader()

	int WoffCodec::DecodeWoffTableDictr(FILE* pWoff)
	{
		unsigned long	ulTmp = 0;
		unsigned short	usTmp = 0;

		unsigned short  numTables = m_vtsfntorigTblBuf.size();

		for (unsigned short i=0; i<numTables; i++)
		{
			WOFFTableDictEntry		Woffentry;
			CHYTableEntry			HYentry;

			//tag
			fread(&Woffentry.tag,4,1,pWoff);
			Woffentry.tag = hy_cdr_int32_to(Woffentry.tag);
			HYentry.tag = Woffentry.tag;

			//offset
			fread(&Woffentry.offset,4,1,pWoff);
			Woffentry.offset = hy_cdr_int32_to(Woffentry.offset);

			//compLength
			fread(&Woffentry.complength,4,1,pWoff);
			Woffentry.complength = hy_cdr_int32_to(Woffentry.complength);

			//origLength
			fread(&Woffentry.origlength,4,1,pWoff);
			Woffentry.origlength = hy_cdr_int32_to(Woffentry.origlength);
			HYentry.length = Woffentry.origlength;

			//origChecksum
			fread(&Woffentry.origchecksum,4,1,pWoff);
			Woffentry.origchecksum = hy_cdr_int32_to(Woffentry.origchecksum);
			HYentry.checkSum = Woffentry.origchecksum;

			m_vtWoffTblDict.push_back(Woffentry);
			m_FontCodec.m_HYTbDirectory.vtTableEntry.push_back(HYentry);
		}

		unsigned long ulCurPos = ftell(pWoff);

		for (unsigned short i=0; i<numTables; i++)
		{
			WOFFTableDictEntry&		Woffentry = m_vtWoffTblDict[i];
			if (Woffentry.complength > 0)
			{
				char* pcompTable = new char[Woffentry.complength];
				fseek(pWoff,Woffentry.offset,SEEK_SET);
				fread(pcompTable,Woffentry.complength,1,pWoff);

				unsigned long ulRealOrigTableLen = (Woffentry.origlength+3)/4*4;
				char* porigTable = new char[ulRealOrigTableLen];
				ZeroMemory(porigTable, ulRealOrigTableLen);

				if(Woffentry.complength == Woffentry.origlength)
				{
					CopyMemory(porigTable,pcompTable,Woffentry.complength);
				}
				else 
				{
					uncompress((Bytef*)porigTable,&ulRealOrigTableLen,(Bytef*)pcompTable,Woffentry.complength);
				}

				unsigned long ulChNum = HY_CalcFontTableChecksum((unsigned long *)porigTable,Woffentry.origlength);
				m_vtsfntorigTblBuf[i] = porigTable;

				delete [] pcompTable;
			}
		}

		return 0;

	}	// end of int WoffCodec::DecodeWoffTableDictr()

	int WoffCodec::MakeSfntFont(const char* pSfnt)
	{
		FILE* pSfntFont = fopen(pSfnt, "wb");
		if (pSfntFont==0) return HY_ERR_FILE_OPEN;

		m_FontCodec.m_pFontFile = pSfntFont;

		m_FontCodec.EncodeTableDirectory();
		for(size_t i=0; i<m_FontCodec.m_HYTbDirectory.vtTableEntry.size(); i++)
		{
			char*					psnftTablebuf = m_vtsfntorigTblBuf[i];
			CHYTableEntry&			TableEntry = m_FontCodec.m_HYTbDirectory.vtTableEntry[i];

			TableEntry.offset		= ftell(pSfntFont);
			unsigned long ulRealBufLen =  (TableEntry.length+3)/4*4;
			char*  psnftbuf = m_vtsfntorigTblBuf[i];

			if (TableEntry.tag == HEAD_TAG)
			{	
				memset(psnftbuf+8,0,4);
			}
			
			fwrite(psnftbuf,ulRealBufLen,1,pSfntFont);					
		}

		fflush(pSfntFont);
		m_FontCodec.EncodeTableDirectory();
		fflush(pSfntFont);
		fclose(pSfntFont);
		
		m_FontCodec.CloseFile(); 
		m_FontCodec.SetCheckSumAdjustment(pSfnt);

		return HY_NOERROR;

	}	// end of void WoffCodec::MakeSfntFont()

	int WoffCodec::DecodeSfntFont(const char* pSfnt)
	{
		FILE* pSfntFont = fopen(pSfnt, "rb");
		if (pSfntFont==0) return HY_ERR_FILE_OPEN;

		m_vtsfntorigTblBuf.clear();

		m_FontCodec.m_pFontFile = pSfntFont;
		m_FontCodec.DecodeTableDirectory();

		for (unsigned short i = 0; i < m_FontCodec.m_HYTbDirectory.numTables; i++)
		{
			CHYTableEntry& tableEntry = m_FontCodec.m_HYTbDirectory.vtTableEntry[i];

			char* pTableBUf = new char[tableEntry.length];
			fseek(pSfntFont,tableEntry.offset,SEEK_SET);
			fread(pTableBUf,tableEntry.length,1,pSfntFont);

			m_vtsfntorigTblBuf.push_back(pTableBUf);
		}

		fflush(pSfntFont);
		fclose(pSfntFont);

		return HY_NOERROR;

	}	// end of void WoffCodec::DecodeSfntFont()

	void WoffCodec::CompresssnftTableBuf()
	{
		for (unsigned short i = 0; i < m_FontCodec.m_HYTbDirectory.numTables; i++)
		{
			CHYTableEntry&		tableEntry = m_FontCodec.m_HYTbDirectory.vtTableEntry[i];
			WOFFTableDictEntry	woffEntry;

			woffEntry.tag			= tableEntry.tag;
			woffEntry.origchecksum	= tableEntry.checkSum;
			woffEntry.origlength	= tableEntry.length;

			unsigned long ulcomplngth = compressBound(tableEntry.length);
			char* comprsBuf = new char[ulcomplngth];
			memset(comprsBuf, 0, ulcomplngth);

			compress2((Bytef*)comprsBuf,&ulcomplngth,(Bytef *)m_vtsfntorigTblBuf[i],tableEntry.length,Z_DEFAULT_COMPRESSION);

			if (ulcomplngth>=tableEntry.length)
			{
				memset(comprsBuf, 0, compressBound(tableEntry.length));
				memcpy(comprsBuf,m_vtsfntorigTblBuf[i],tableEntry.length);
				woffEntry.complength = tableEntry.length;
			}
			else 
			{
				woffEntry.complength = ulcomplngth;
			}

			m_vtsfntcmprsTblBuf.push_back(comprsBuf);
			m_vtWoffTblDict.push_back(woffEntry);
		}

	}	// end of void WoffCodec::CompressTableBuf()

	int WoffCodec::EncodeWoff(const char* pWoff)
	{
		FILE* pWff = fopen(pWoff,"wb");
		if (pWff == NULL) return HY_ERR_FILE_OPEN;

		EncodeWoffHeader(pWff);
		EncodeWoffTableDiecrtory(pWff);
		EncodeWoffTableData(pWff);

		if (m_ulCmprsMetaBufSize>0)
		{
			unsigned long szRealCmprMetaBufSize = (m_ulCmprsMetaBufSize+3)&0xFFFFFFFC;
			fwrite(m_pCmprsMetaBuf,szRealCmprMetaBufSize,1,pWff);
		}

		if (m_ulPrivteBufSize>0)
		{
			unsigned long szRealCmprMetaBufSize = (m_ulPrivteBufSize+3)&0xFFFFFFFC;
			fwrite(m_pPrivteBuf,m_ulPrivteBufSize,1,pWff);
		}

		fseek(pWff,44,SEEK_SET);
		EncodeWoffTableDiecrtory(pWff);

		fflush(pWff);
		fclose(pWff);

		return HY_NOERROR;

	}	// end of void WoffCodec::EncodeWoff()

	int WoffCodec::EncodeWoffHeader(FILE* pWff)
	{
		unsigned long ulTmp = 0;
		unsigned short usTmp = 0;

		fseek(pWff,0,SEEK_SET);

		//signature 0x774F4646 
		ulTmp = hy_cdr_int32_to(WOFF_FLAG);
		fwrite(&ulTmp, 4, 1,pWff);

		//flavor
		usTmp = hy_cdr_int16_to(m_FontCodec.m_HYTbDirectory.version.value);
		fwrite(&usTmp,2,1,pWff);
		usTmp = hy_cdr_int16_to(m_FontCodec.m_HYTbDirectory.version.fract);
		fwrite(&usTmp,2,1,pWff);

		unsigned short usnumTable = (unsigned short)m_FontCodec.m_HYTbDirectory.vtTableEntry.size();
		unsigned long  ulTotalSfntSize = 12;
		ulTotalSfntSize += 16 * usnumTable;
		for (size_t i=0; i<usnumTable; i++)
		{
			CHYTableEntry& tblEntry = m_FontCodec.m_HYTbDirectory.vtTableEntry[i];
			ulTotalSfntSize += (tblEntry.length+3)&0xFFFFFFFC;
		}

		unsigned long  ulWoffsize = 44;
		ulWoffsize+= 20 * usnumTable;
		for (size_t i=0; i<usnumTable; i++)
		{
			WOFFTableDictEntry& entry = m_vtWoffTblDict[i];
			ulWoffsize += (entry.complength+3)&0xFFFFFFFC;
		}

		unsigned long ulMetaOffset = 0;
		if (m_ulCmprsMetaBufSize>0)
		{
			ulMetaOffset = ulWoffsize;
			ulWoffsize += (m_ulCmprsMetaBufSize+3)&0xFFFFFFFC;
		}

		unsigned long ulPrivteOffset = 0;
		if (m_ulPrivteBufSize>0)
		{
			ulPrivteOffset = ulWoffsize;
			ulWoffsize += (m_ulPrivteBufSize+3)&0xFFFFFFFC;
		}

		//length
		ulWoffsize = hy_cdr_int32_to(ulWoffsize);
		fwrite(&ulWoffsize,4,1,pWff);

		//numTables
		usnumTable = hy_cdr_int16_to(usnumTable);
		fwrite(&usnumTable,2,1,pWff);

		//reserved
		usTmp = 0;
		fwrite(&usTmp,2,1,pWff);

		//totalSfntSize
		ulTotalSfntSize = hy_cdr_int32_to(ulTotalSfntSize);
		fwrite(&ulTotalSfntSize,4,1,pWff);

		//majorVersion
		usTmp = hy_cdr_int16_to(1);
		fwrite(&usTmp,2,1,pWff);

		//minorVersion
		usTmp = 0;
		fwrite(&usTmp,2,1,pWff);

		//metaOffset
		ulTmp = hy_cdr_int32_to(ulMetaOffset);
		fwrite(&ulTmp, 4,1,pWff);

		//metaLength
		ulTmp = hy_cdr_int32_to(m_ulCmprsMetaBufSize);
		fwrite(&ulTmp, 4,1,pWff);

		//metaOrigLength
		ulTmp = hy_cdr_int32_to(m_ulOrigMetaBufSize);
		fwrite(&ulTmp, 4,1,pWff);

		//privOffset
		ulTmp = hy_cdr_int32_to(ulPrivteOffset);
		fwrite(&ulTmp, 4,1,pWff);

		//privLength
		ulTmp = hy_cdr_int32_to(m_ulPrivteBufSize);
		fwrite(&ulTmp, 4,1,pWff);

		return 0;

	}	// end of int WoffCodec::EncodeWoffHeader()

	int WoffCodec::EncodeWoffTableDiecrtory(FILE* pWff)
	{
		unsigned long ulTmp = 0;
		for(size_t i=0; i<m_vtWoffTblDict.size();i++)
		{
			WOFFTableDictEntry&  entry = m_vtWoffTblDict[i];

			//tag
			ulTmp = hy_cdr_int32_to(entry.tag);
			fwrite(&ulTmp,4,1,pWff);

			//offset
			ulTmp = hy_cdr_int32_to(entry.offset);
			fwrite(&ulTmp,4,1,pWff);

			//compLength
			ulTmp = hy_cdr_int32_to(entry.complength);
			fwrite(&ulTmp,4,1,pWff);

			//origLength
			ulTmp = hy_cdr_int32_to(entry.origlength);
			fwrite(&ulTmp,4,1,pWff);

			//origChecksum
			ulTmp = hy_cdr_int32_to(entry.origchecksum);
			fwrite(&ulTmp,4,1,pWff);			
		}

		return 0;

	}	// end of int WoffCodec::EncodeWoffTableDiecrtory()	

	int WoffCodec::EncodeWoffTableData(FILE* pWff)
	{
		unsigned long ulTmp = 0;
		for(size_t i=0; i<m_vtWoffTblDict.size();i++)
		{
			WOFFTableDictEntry&  entry = m_vtWoffTblDict[i];				
			entry.offset = ftell(pWff);
			char* psnftBuf = m_vtsfntcmprsTblBuf[i];

			ulTmp =	(entry.complength+3)&0xFFFFFFFC;
			fwrite(psnftBuf, ulTmp,1,pWff);				
		}

		return 0;

	}	// end of int WoffCodec::EncodeWoffTableData()


}
