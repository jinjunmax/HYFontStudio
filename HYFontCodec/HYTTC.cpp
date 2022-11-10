#include "stdafx.h"
#include "HYTTC.h"

namespace HYFONTCODEC
{
	CHYTableData::CHYTableData()
	{		

	}	// end of CHYTableData::CHYTableData()

	CHYTableData::~CHYTableData()
	{

	}	// end of CHYTableData::~CHYTableData()

	CHYTabelShareInfo::CHYTabelShareInfo()
	{
		FontID = 0;
		checkSum = 0;
		tbTag = 0;
		lenth = 0;
		Pos = 0;

	}	//end of CHYTableDataVertor::CHYTabelShareInfo()

	CHYTabelShareInfo::~CHYTabelShareInfo()
	{


	}	// end of CHYTableDataVertor::~CHYTabelShareInfo()

	CHYFontData::CHYFontData()
	{
	

	}	// end of CHYFontData::CHYFontData()

	CHYFontData::~CHYFontData()
	{

	}	// end of CHYFontData::~CHYFontData()

	CHYTableEntry* CHYFontData::FindTableEntry(unsigned long& tag)
	{
		size_t stTbDrty = TableDirectory.numTables;
		for (size_t i = 0; i < stTbDrty; i++)
		{
			if (TableDirectory.vtTableEntry[i].tag == tag)
				return &TableDirectory.vtTableEntry[i];
		}

		return NULL;

	}	// end of CHYTableEntry& CHYFontData::FindTableEntry()

	CHYTableData* CHYFontData::FindTableData(unsigned long& tag)
	{
		size_t stTBSize = vtTableData.size();
		for (size_t i = 0; i < stTBSize; i++)
		{
			if (TableDirectory.vtTableEntry[i].tag == tag)
				return &vtTableData[i];
		}

		return NULL;

	}	//end of CHYTableData& CHYFontData::FindTableData()

	CTTCHeader::CTTCHeader()
	{
		ttcTag = 0;
		majorVersion = 0;
		minorVersion = 0;
		numFonts = 0;
		dsigTag = 0;
		dsigLength = 0;
		dsigOffset = 0;

	}	// end of CTTCHeader::CTTCHeader()

	CTTCHeader::~CTTCHeader()
	{

	}	// end of CTTCHeader::~CTTCHeader()

	CHYTTC::CHYTTC()
	{

	}	// end of CHYTTC::CHYTTC()

	CHYTTC::~CHYTTC()
	{

	}	// end of CHYTTC::~CHYTTC()

	int CHYTTC::TTC2Font(std::string strTTC)
	{
		m_vtFontData.clear();

		int iFilestat = ::HY_FileExist(strTTC.data());
		if (iFilestat == 0)			return HY_ERR_FILE_ZERO;
		if (iFilestat == -1)		return HY_ERR_FILE_NOEXIST;
		if (iFilestat == -2)		return HY_ERR_FILE_STAT;
		if (iFilestat == -3)		return HY_ERR_FILE_OTHER;

		FILE* pttcFile = fopen(strTTC.data(), "rb");

		// TAG  'ttcf'
		fread(&m_TTCHead.ttcTag, 4, 1, pttcFile);
		m_TTCHead.ttcTag = hy_cdr_int32_to(m_TTCHead.ttcTag);
		if (m_TTCHead.ttcTag != TTC_TAG) {
			fflush(pttcFile);
			fclose(pttcFile);
			return FONT_ERR_NO_TTC;
		}

		fread(&m_TTCHead.majorVersion, 2, 1, pttcFile);
		m_TTCHead.majorVersion = hy_cdr_int16_to(m_TTCHead.majorVersion);
		fread(&m_TTCHead.minorVersion, 2, 1, pttcFile);
		m_TTCHead.minorVersion = hy_cdr_int16_to(m_TTCHead.minorVersion);

		fread(&m_TTCHead.numFonts, 4, 1, pttcFile);
		m_TTCHead.numFonts = hy_cdr_int32_to(m_TTCHead.numFonts);

		std::string strFontFile;
		for (int i = 0; i < m_TTCHead.numFonts; i++)
		{
			CHYFontData FontData;

			unsigned long fntOffset = 0;
			fread(&fntOffset, 4, 1, pttcFile);
			fntOffset = hy_cdr_int32_to(fntOffset);

			long FCPos = ftell(pttcFile);
			fseek(pttcFile, fntOffset, SEEK_SET);
			GetFontData(pttcFile, FontData);

			if (FontData.TableDirectory.version.value == 1 && FontData.TableDirectory.version.fract == 0)
				strFontFile = HY_GetDirFromPath(strTTC) + HY_GetFileNameFromPath(strTTC) + std::to_string(i) + ".ttf";
			else
				strFontFile = HY_GetDirFromPath(strTTC) + HY_GetFileNameFromPath(strTTC) + std::to_string(i) + ".otf";

			MakeFont(FontData, strFontFile);
			fseek(pttcFile, FCPos, SEEK_SET);

			m_vtFontData.push_back(FontData);
		}

		if (pttcFile != NULL)
		{
			fflush(pttcFile);
			fclose(pttcFile);
		}

		return HY_NOERROR;

	}	 //end of void CHYTTC::TTC2Font()

	int CHYTTC::MakeFont(CHYFontData& FontData, const std::string& strFontName)
	{
		FILE* FontFile = fopen(strFontName.data(), "w+b");
		if (FontFile == NULL) {
			_set_errno(0);
			return HY_ERR_FILE_WRITE;
		}

		CHYTableDirectory& TDtry = FontData.TableDirectory;

		unsigned short	usTmp;
		unsigned long	ulTmp;	

		// sfnt version
		usTmp = hy_cdr_int16_to(TDtry.version.value);
		fwrite(&usTmp, 2, 1, FontFile);
		usTmp = hy_cdr_int16_to(TDtry.version.fract);
		fwrite(&usTmp, 2, 1, FontFile);
		// numTables
		usTmp = hy_cdr_int16_to(TDtry.numTables);
		fwrite(&usTmp, 2, 1, FontFile);
		// searchRange
		usTmp = hy_cdr_int16_to(TDtry.searchRange);
		fwrite(&usTmp, 2, 1, FontFile);
		// entrySelector
		usTmp = hy_cdr_int16_to(TDtry.entrySelector);
		fwrite(&usTmp, 2, 1, FontFile);
		// rangeShift
		usTmp = hy_cdr_int16_to(TDtry.rangeShift);
		fwrite(&usTmp, 2, 1, FontFile);

		long lDirPos = ftell(FontFile);
		for (int i = 0; i<TDtry.numTables; i++)
		{
			CHYTableEntry&	HYEntry = TDtry.vtTableEntry[i];
			//tag		
			ulTmp = hy_cdr_int32_to(HYEntry.tag);
			fwrite(&ulTmp, 4, 1, FontFile);
			// checkSum
			ulTmp = hy_cdr_int32_to(HYEntry.checkSum);
			fwrite(&ulTmp, 4, 1, FontFile);
			//offset
			ulTmp = hy_cdr_int32_to(HYEntry.offset);
			fwrite(&ulTmp, 4, 1, FontFile);
			//length
			ulTmp = hy_cdr_int32_to(HYEntry.length);
			fwrite(&ulTmp, 4, 1, FontFile);
		}

		for (int i = 0; i < TDtry.numTables; i++)
		{
			CHYTableEntry&	HYEntry = TDtry.vtTableEntry[i];
			CHYTableData&  TbData = FontData.vtTableData[i];

			HYEntry.offset = ftell(FontFile);
			for (size_t j = 0; j < TbData.vtData.size(); j++)
			{
				fwrite(&TbData.vtData[j], 1, 1, FontFile);
			}
		}

		fseek(FontFile, lDirPos, SEEK_SET);
		for (int i = 0; i<TDtry.numTables; i++)
		{
			CHYTableEntry&	HYEntry = TDtry.vtTableEntry[i];
			//tag		
			ulTmp = hy_cdr_int32_to(HYEntry.tag);
			fwrite(&ulTmp, 4, 1, FontFile);
			// checkSum
			ulTmp = hy_cdr_int32_to(HYEntry.checkSum);
			fwrite(&ulTmp, 4, 1, FontFile);
			//offset
			ulTmp = hy_cdr_int32_to(HYEntry.offset);
			fwrite(&ulTmp, 4, 1, FontFile);
			//length
			ulTmp = hy_cdr_int32_to(HYEntry.length);
			fwrite(&ulTmp, 4, 1, FontFile);
		}

		fflush(FontFile);
		fclose(FontFile);

		return HY_NOERROR;

	}	//end of int CHYTTC::MakeFont()

	int  CHYTTC::Font2TTC(std::string strTTC, std::vector<string>& Fonts, std::vector<string>* pCharsTable /*=NULL*/)
	{	
		if (pCharsTable == NULL)
		{
			return Font2TTCSimaple(strTTC, Fonts);
		}

		return HY_NOERROR;

	}	// end of void CHYTTC::Font2TT()

	int CHYTTC::Font2TTCSimaple(std::string strTTC, std::vector<string>& Fonts)
	{
		FILE* FontFile = fopen(strTTC.data(), "w+b");
		if (FontFile == NULL) {
			_set_errno(0);
			return HY_ERR_FILE_WRITE;
		}

		m_vtFontData.clear();
		size_t stSize = Fonts.size();
		for (size_t i = 0; i < stSize; i++)
		{
			FILE* pFntFile = fopen(Fonts[i].data(), "rb");

			CHYFontData FontData;
			GetFontData(pFntFile, FontData);

			if (pFntFile != NULL){
				fflush(pFntFile);
				fclose(pFntFile);
			}

			m_vtFontData.push_back(FontData);
		}

		unsigned short	usTmp = 0;
		unsigned long	ulTmp = 0;

		// TAG Flag		
		ulTmp = hy_cdr_int32_to(TTC_TAG);
		fwrite(&ulTmp, 4, 1, FontFile);
		// Major version of the TTC Header, = 1.
		usTmp = hy_cdr_int16_to(1);
		fwrite(&usTmp, 2, 1, FontFile);
		//Minor version of the TTC Header, = 0.
		usTmp = 0;
		fwrite(&usTmp, 2, 1, FontFile);
		//numFonts
		ulTmp = hy_cdr_int32_to((unsigned short)m_vtFontData.size());
		fwrite(&ulTmp, 4, 1, FontFile);

		unsigned long ulOffsetPos = ftell(FontFile);
		// offset Õ¼Î»
		for (int i = 0; i < (int)m_vtFontData.size(); i++)
		{
			ulTmp = 0;
			fwrite(&ulTmp, 4, 1, FontFile);
		}

		m_vttbShareInf.clear();
		std::vector<unsigned long> vtFontpos;
		for (int i = 0; i < (int)m_vtFontData.size(); i++)
		{
			unsigned long ulPos = ftell(FontFile);
			vtFontpos.push_back(ulPos);

			MakeTTCFont(FontFile, i);
		}

		unsigned long ulEndPos = ftell(FontFile);

		// ÖØÐ´offset
		fseek(FontFile, ulOffsetPos, SEEK_SET);
		for (int i = 0; i < (int)m_vtFontData.size(); i++)
		{
			ulTmp = hy_cdr_int32_to(vtFontpos[i]);
			fwrite(&ulTmp, 4, 1, FontFile);
		}

		fseek(FontFile, ulEndPos, SEEK_SET);
		fflush(FontFile);
		fclose(FontFile);
		
		return HY_NOERROR;

	}	// endo f void Font2TTCSimaple()

	int CHYTTC::Font2TTCComplex(std::string strTTC, std::vector<string>& Fonts, std::vector<string>* pCharsTable)
	{
		
		return HY_NOERROR;

	}	// end of void CHYTTC::Font2TTCComplex()

	void CHYTTC::MakeTTCFont(FILE* FontFile,int iFontID)
	{
		CHYFontData& fntData = m_vtFontData[iFontID];
		CHYTableDirectory&  TDtry = fntData.TableDirectory;
		unsigned short	usTmp = 0;
		unsigned long	ulTmp = 0;
		// sfnt version
		usTmp = hy_cdr_int16_to(TDtry.version.value);
		fwrite(&usTmp, 2, 1, FontFile);
		usTmp = hy_cdr_int16_to(TDtry.version.fract);
		fwrite(&usTmp, 2, 1, FontFile);
		// numTables
		usTmp = hy_cdr_int16_to(TDtry.numTables);
		fwrite(&usTmp, 2, 1, FontFile);
		// searchRange
		usTmp = hy_cdr_int16_to(TDtry.searchRange);
		fwrite(&usTmp, 2, 1, FontFile);
		// entrySelector
		usTmp = hy_cdr_int16_to(TDtry.entrySelector);
		fwrite(&usTmp, 2, 1, FontFile);
		// rangeShift
		usTmp = hy_cdr_int16_to(TDtry.rangeShift);
		fwrite(&usTmp, 2, 1, FontFile);

		unsigned long lDirPos = ftell(FontFile);
		for (int i = 0; i<TDtry.numTables; i++)
		{
			CHYTableEntry&	HYEntry = TDtry.vtTableEntry[i];
			//tag		
			ulTmp = hy_cdr_int32_to(HYEntry.tag);
			fwrite(&ulTmp, 4, 1, FontFile);
			// checkSum
			ulTmp = hy_cdr_int32_to(HYEntry.checkSum);
			fwrite(&ulTmp, 4, 1, FontFile);
			//offset
			ulTmp = 0;
			fwrite(&ulTmp, 4, 1, FontFile);
			//length
			ulTmp = hy_cdr_int32_to(HYEntry.length);
			fwrite(&ulTmp, 4, 1, FontFile);
		}
		
		int  tbShareID = -1;
		for (int i = 0; i < TDtry.numTables; i++)
		{
			CHYTableEntry&	HYEntry = TDtry.vtTableEntry[i];
			CHYTableData&  TbData = fntData.vtTableData[i];

			tbShareID = IsShare(HYEntry.tag, HYEntry.checkSum);
			if (tbShareID == -1)
			{
				HYEntry.offset = ftell(FontFile);

				CHYTabelShareInfo shareInf;
				shareInf.tbTag = HYEntry.tag;
				shareInf.FontID = iFontID;
				shareInf.Pos = HYEntry.offset;
				shareInf.lenth = HYEntry.length;
				shareInf.checkSum = HYEntry.checkSum;
				m_vttbShareInf.push_back(shareInf);

				for (size_t j = 0; j < TbData.vtData.size(); j++)
				{
					fwrite(&TbData.vtData[j], 1, 1, FontFile);
				}
			}
			else
			{
				CHYTabelShareInfo& ShareInf = m_vttbShareInf[tbShareID];
				HYEntry.offset = ShareInf.Pos;
			}

			unsigned long lCurPos = ftell(FontFile);

			fseek(FontFile, lDirPos, SEEK_SET);
			for (int i = 0; i<TDtry.numTables; i++)
			{
				CHYTableEntry&	HYEntry = TDtry.vtTableEntry[i];
				//tag		
				ulTmp = hy_cdr_int32_to(HYEntry.tag);
				fwrite(&ulTmp, 4, 1, FontFile);
				// checkSum
				ulTmp = hy_cdr_int32_to(HYEntry.checkSum);
				fwrite(&ulTmp, 4, 1, FontFile);
				//offset
				ulTmp = hy_cdr_int32_to(HYEntry.offset);
				fwrite(&ulTmp, 4, 1, FontFile);
				//length
				ulTmp = hy_cdr_int32_to(HYEntry.length);
				fwrite(&ulTmp, 4, 1, FontFile);
			}

			fseek(FontFile, lCurPos, SEEK_SET);

		}		

	}	// end of void CHYTTC::MakeTTCFont()

	int CHYTTC::IsShare(unsigned long usTag, unsigned long checkSum)
	{
		size_t stSize = m_vttbShareInf.size();
		for (int i = 0; i < stSize; i++)
		{
			CHYTabelShareInfo& ShareInf = m_vttbShareInf[i];
			if (ShareInf.tbTag == usTag)
			{
				if (ShareInf.checkSum == checkSum)
				{
					return i;
				}
			}
		}

		return -1;

	}	// end of bool CHYTTC::IsShare()	

	void CHYTTC::GetFontData(FILE* pFile, CHYFontData& FontData)
	{
		CHYTableDirectory& TBDirtry = FontData.TableDirectory;
		// sfnt version
		fread(&TBDirtry.version.value, 2, 1, pFile);
		TBDirtry.version.value = hy_cdr_int16_to(TBDirtry.version.value);
		fread(&TBDirtry.version.fract, 2, 1, pFile);
		TBDirtry.version.fract = hy_cdr_int16_to(TBDirtry.version.fract);
		//numTables
		fread(&TBDirtry.numTables, 2, 1, pFile);
		TBDirtry.numTables = hy_cdr_int16_to(TBDirtry.numTables);
		// searchRange
		fread(&TBDirtry.searchRange, 2, 1, pFile);
		TBDirtry.searchRange = hy_cdr_int16_to(TBDirtry.searchRange);
		// entrySelector
		fread(&TBDirtry.entrySelector, 2, 1, pFile);
		TBDirtry.entrySelector = hy_cdr_int16_to(TBDirtry.entrySelector);
		//rangeShift
		fread(&TBDirtry.rangeShift, 2, 1, pFile);
		TBDirtry.rangeShift = hy_cdr_int16_to(TBDirtry.rangeShift);

		CHYTableEntry	HYEntry;
		for (int i = 0; i<TBDirtry.numTables; i++)
		{
			CHYTableData tbData;			

			//tag							
			fread(&HYEntry.tag, 4, 1, pFile);
			HYEntry.tag = hy_cdr_int32_to(HYEntry.tag);			

			// checkSum
			fread(&HYEntry.checkSum, 4, 1, pFile);
			HYEntry.checkSum = hy_cdr_int32_to(HYEntry.checkSum);			

			//offset
			fread(&HYEntry.offset, 4, 1, pFile);
			HYEntry.offset = hy_cdr_int32_to(HYEntry.offset);
			//length
			fread(&HYEntry.length, 4, 1, pFile);
			HYEntry.length = hy_cdr_int32_to(HYEntry.length);			

			long lPos = ftell(pFile);
			fseek(pFile, HYEntry.offset, SEEK_SET);
			unsigned char ucDate = 0;
			int iRealLeath = HYEntry.length + (4 - HYEntry.length % 4);
			for (int j = 0; j < iRealLeath; j++)
			{
				fread(&ucDate, 1, 1, pFile);
				tbData.vtData.push_back(ucDate);
			}

			fseek(pFile, lPos, SEEK_SET);

			FontData.vtTableData.push_back(tbData);
			TBDirtry.vtTableEntry.push_back(HYEntry);
		}

	}	// end of void CHYTTC::GetFontData()

}