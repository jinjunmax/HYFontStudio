#include "stdafx.h"
#include "HYFontUpdate.h"

namespace HYFONTCODEC
{
	CHYFontUpdate::CHYFontUpdate()
	{


	}	//end of CHYFontUpdate::CHYFontUpdate()

	CHYFontUpdate::~CHYFontUpdate()
	{

	}	// end of CHYFontUpdate::~CHYFontUpdate()

	int CHYFontUpdate::Update_glyf(char* pFontFile, std::vector<unsigned long> vtFlag)
	{		
		int iRtn = CreateFile(pFontFile);
		if (iRtn != HY_NOERROR)  return iRtn;
		m_iFontType = FONTTYPE_TTF;

		MakeTableDirectory(vtFlag);
		EncodeTableDirectory();

		size_t stEntry = m_HYTbDirectory.vtTableEntry.size();
		for (size_t st = 0; st < stEntry; st++)
		{
			unsigned long flg = vtFlag[st];
			if (flg==GLYF_TAG) {
				EncodeGlyph();
			}
			else if (flg== LOCA_TAG) {
				Encodeloca();
			}
			else
			{	
				int iEntryIndex = m_HYTbDirectory.FindTableEntry(flg);
				if (iEntryIndex != -1) {
					CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
					tbEntry.offset = ftell(m_pFontFile);

					TableData* pData = m_mulpTableData.FindTableData(flg);
					if (pData) {
						for (int i = 0; i < pData->ulDataSize; i++) {
							fwrite(&pData->vtData[i], 1, 1, m_pFontFile);
						}
					}
					tbEntry.length = ftell(m_pFontFile) - tbEntry.offset;
					::HY_4ByteAlign(m_pFontFile, tbEntry.length);
				}
			}
		}

		// 对每一个表生成校验码
		fflush(m_pFontFile);
		for (int i = 0; i < m_HYTbDirectory.numTables; i++) {
			CHYTableEntry& HYEntry = m_HYTbDirectory.vtTableEntry[i];
			int CheckBufSz = (HYEntry.length + 3) / 4 * 4;
			char* pCheckBuf = new char[CheckBufSz];
			fseek(m_pFontFile, HYEntry.offset, SEEK_SET);
			fread(pCheckBuf, CheckBufSz, 1, m_pFontFile);
			HYEntry.checkSum = HY_CalcFontTableChecksum((unsigned long*)pCheckBuf, HYEntry.length);
			if (pCheckBuf) delete[] pCheckBuf;
		}
		fflush(m_pFontFile);
		EncodeTableDirectory();
		CloseFile();

		return HY_NOERROR;

	}	// end of Update_glyf()

}