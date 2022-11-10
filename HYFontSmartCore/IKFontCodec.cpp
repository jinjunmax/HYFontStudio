#include "StdAfx.h"
#include "IKFontCodec.h"

CIKFontCodec::CIKFontCodec(void)
{
	InitParam();

}	// end of CIKFontCodec::CIKFontCodec()

CIKFontCodec::~CIKFontCodec(void)
{
	UninitParam();
	
}	// end of CIKFontCodec::~CIKFontCodec()

int		CIKFontCodec::MakeUnicode()
{
	PIK_HEAD	pIkHead = m_IKFont.GetHeadInfo();
	PIK_FONT	pIkFont = m_IKFont.GetFont();

	if (pIkHead->tagIKFontInf.usCharNum == 0 )		return	HY_ERR_UNKNOW;

	for (int i =0; i < pIkHead->tagIKFontInf.usCharNum; i ++ )
	{

		PIK_CHARACTER	 pIkchar = &pIkFont->pCharacter[i];
		ZeroMemory( pIkchar->szUnicode, IK_UNICODE_LEN * sizeof(unsigned short) );
		pIkchar->RealUnicodeLen = 1;
		if ( i < (int)m_dwUnicodLstLen)
		{			
			pIkchar->szUnicode[0] = m_pUnicodeLst[i];
		}
		else 
		{
			pIkchar->szUnicode[0] = HY_MISS_CHAR;
		} 
	}

	return HY_NOERROR;

}	// end of int	CIKFontCodec::MakeUnicodeEx()

int CIKFontCodec::EncodeIK(CString strFileName, CIKFont* pIKFont)
{
	DeleteFile(strFileName);

	UinitVectorCharBuffer();
	m_vtCharSize.clear();
	m_vtCharNum.clear();

	Build_IK(pIKFont);
	CFile fIK(strFileName,CFile::modeCreate|CFile::modeWrite|CFile::typeBinary);
	fIK.Write(m_pIKBuf, m_dwIKBufLen);
	fIK.Flush();
	fIK.Close();

	return HY_NOERROR;

}	// end of int CIKFontCodec::EncodeIK()

void	CIKFontCodec::Build_IK(CIKFont* pIKFont)
{
	if (m_pHeadBuf) delete[] m_pHeadBuf;
	m_pHeadBuf = NULL;
	m_dwHeadBufSize = 0;

	if ( m_pIKBuf ) delete[] m_pIKBuf;
	m_pIKBuf = NULL;
	m_dwIKBufLen = 0;

	PIK_FONT	pIK				= pIKFont->GetFont();
	PIK_HEAD	pIKHead			= &pIK->tagHead;
	int			iBuildCharIndex		= 0;	
	int			i = 0;

	CHYMapList	MapList;
	for ( i = 0; i < pIKHead->tagIKFontInf.usCharNum; i++ )
	{
		CHYMapItem	MapItem;
		PIK_CHARACTER			pIkChar = &pIK->pCharacter[i];

		MapItem.SetGlyphNo(pIkChar->tagCharNameinf.usCharacterNo);
		MapItem.SetGlyphIndex(i);
		MapList.AddLast(MapItem);
	}

	MapList.QuickSortbyUnicode();
	for ( i = 0; i < MapList.NumItems(); i++ )
	{
		PIK_CHARACTER			pIkChar = &pIK->pCharacter[MapList[i].GetGlyphIndex()];
		unsigned  long ulCharSize = BuildCharBuffer(pIkChar, pIKHead->tagIKFontInf.usBaseLine);
		if ( ulCharSize > 0 ) iBuildCharIndex++;
	}

	// bulid character index
	// 计算character inf 部分的数据大小 第一6 为characherInf 的头部分预留
	// 第二个6 为每个char index的空间大小 3 * 2
	unsigned long	ulCharInfSize = 6 + iBuildCharIndex * 6;
	DWORD			dwHeadOffset = 0;
	short			s = 0;

	m_dwHeadBufSize	 = 69 * 2 + ulCharInfSize;
	m_pHeadBuf = new BYTE[m_dwHeadBufSize];
	ZeroMemory(m_pHeadBuf, m_dwHeadBufSize);

	// build head 
	// 构建文件头长度信息
	s = (short)m_dwHeadBufSize>>1;
	CopyMemory(m_pHeadBuf + dwHeadOffset, &s, sizeof(short));
	dwHeadOffset += 2;

	// NameSection
	// legth of name section
	s = 55;
	CopyMemory(m_pHeadBuf + dwHeadOffset, &s, sizeof(short));
	dwHeadOffset += 2;

	// number on urw list
	s = 0;
	CopyMemory(m_pHeadBuf + dwHeadOffset, &s, sizeof(short));
	dwHeadOffset += 2;

	// File name 
	CopyMemory(m_pHeadBuf + dwHeadOffset, pIKHead->tagName.FileName, IK_FILENAME_LENG);
	dwHeadOffset += IK_FILENAME_LENG;

	// font name 
	CopyMemory(m_pHeadBuf + dwHeadOffset, pIKHead->tagName.FontName, IK_FONTNAME_LENG);
	dwHeadOffset += IK_FONTNAME_LENG;

	// data format
	CopyMemory(m_pHeadBuf + dwHeadOffset, &pIKHead->tagName.usDataFormat, sizeof(short));
	dwHeadOffset += sizeof(short);

	// Production date
	CopyMemory(m_pHeadBuf + dwHeadOffset, &pIKHead->tagName.usProductDate, sizeof(short) * 3);
	dwHeadOffset += sizeof(short) * 3;

	// date of change 
	CopyMemory(m_pHeadBuf + dwHeadOffset, &pIKHead->tagName.usChangeDate, sizeof(short) * 3);
	dwHeadOffset += sizeof(short) * 3;

	// font information
	CopyMemory(m_pHeadBuf + dwHeadOffset, &pIKHead->tagIKFontInf.usFontLength, sizeof(short));
	dwHeadOffset += sizeof(short);

	// indicator for font
	CopyMemory(m_pHeadBuf + dwHeadOffset, &pIKHead->tagIKFontInf.usIndicator, sizeof(short));
	dwHeadOffset += sizeof(short);

	// number of characters
	CopyMemory(m_pHeadBuf + dwHeadOffset, &iBuildCharIndex, sizeof(short));
	dwHeadOffset += sizeof(short);

	// cap height
	CopyMemory(m_pHeadBuf + dwHeadOffset, &pIKHead->tagIKFontInf.usCapHeight, sizeof(short));
	dwHeadOffset += sizeof(short);

	// Body size
	CopyMemory(m_pHeadBuf + dwHeadOffset, &pIKHead->tagIKFontInf.usBodySize, sizeof(short));
	dwHeadOffset += sizeof(short);

	// x-height
	CopyMemory(m_pHeadBuf + dwHeadOffset, &pIKHead->tagIKFontInf.usX_Height, sizeof(short));
	dwHeadOffset += sizeof(short);

	// Distance base line
	CopyMemory(m_pHeadBuf + dwHeadOffset, &pIKHead->tagIKFontInf.usBaseLine, sizeof(short));
	dwHeadOffset += sizeof(short);

	// Text line distance for setting
	CopyMemory(m_pHeadBuf + dwHeadOffset, &pIKHead->tagIKFontInf.usTextLine, sizeof(short));
	dwHeadOffset += sizeof(short);

	// stem thickness
	CopyMemory(m_pHeadBuf + dwHeadOffset, &pIKHead->tagIKFontInf.usStemThickness, sizeof(short));
	dwHeadOffset += sizeof(short);

	// angle of italisation
	CopyMemory(m_pHeadBuf + dwHeadOffset, &pIKHead->tagIKFontInf.usAngle, sizeof(short));
	dwHeadOffset += sizeof(short);

	// optimum point size for setting
	CopyMemory(m_pHeadBuf + dwHeadOffset, &pIKHead->tagIKFontInf.usOptimum, sizeof(short));
	dwHeadOffset += sizeof(short);

	// average character width
	CopyMemory(m_pHeadBuf + dwHeadOffset, &pIKHead->tagIKFontInf.usAverageWidth, sizeof(short));
	dwHeadOffset += sizeof(short);

	CopyMemory(m_pHeadBuf + dwHeadOffset, &pIKHead->usHierarchy, sizeof(short));
	dwHeadOffset += sizeof(short);

	PBYTE	pCharIndexBuf = new BYTE[ulCharInfSize];
	ZeroMemory(pCharIndexBuf, ulCharInfSize);
	// dwCharOffset 在整个文件中的偏移量
	// dwCharIndexBufOffset 在 pcharindexbuf 中的偏移量
	DWORD	dwCharIndexBufOffset = 0, dwCharOffset = m_dwHeadBufSize;

	// length of character index	
	s = (short)(ulCharInfSize>>1);
	CopyMemory(pCharIndexBuf + dwCharIndexBufOffset, &s, sizeof(short) );
	dwCharIndexBufOffset +=2;

	// recods and word 暂时跳过
	dwCharIndexBufOffset +=4;

	short sRecord = 0, sWord = 0;
	for ( i = 0; i < iBuildCharIndex; i++ )
	{
		// character number
		WORD wCharNumber = (short)m_vtCharNum[i];

		if (wCharNumber >= 0x8000 )
			wCharNumber++;
		CopyMemory(pCharIndexBuf + dwCharIndexBufOffset, &wCharNumber, sizeof(short));
		dwCharIndexBufOffset += 2;

		// record
		//s = (short)(((dwCharOffset >>1) + 1)/IK_RECORD );
		if ( (((dwCharOffset>>1) + 1) % IK_RECORD) == 0 )
		{
			sRecord = (short)(((dwCharOffset >>1) + 1 )/IK_RECORD);
			sWord = IK_RECORD;
		}
		else 
		{
			sRecord = (short)(((dwCharOffset >>1) + 1 )/IK_RECORD) + 1;
			sWord = (short)(((dwCharOffset>>1) + 1) % IK_RECORD);
		}

		CopyMemory(pCharIndexBuf+ dwCharIndexBufOffset, &sRecord, sizeof(short));
		dwCharIndexBufOffset += 2;

		// word 
		CopyMemory(pCharIndexBuf + dwCharIndexBufOffset, &sWord, sizeof(short));
		dwCharIndexBufOffset += 2;

		dwCharOffset += m_vtCharSize[i];
	}

	if ( (((dwCharOffset>>1) + 1) % IK_RECORD) == 0 )
	{
		sRecord = (short)(((dwCharOffset >>1) + 1)/IK_RECORD);
		sWord = IK_RECORD;
	}
	else 
	{
		sRecord = (short)(((dwCharOffset >>1) + 1)/IK_RECORD) + 1;
		// 这里不需要+1 因为是last word
		//sWord = (short)(((dwCharOffset>>1) + 1) % IK_RECORD);
		sWord = (short)(((dwCharOffset>>1)) % IK_RECORD);
	}

	// 反填length of font in physical records
	CopyMemory(pCharIndexBuf + 2, &sRecord, sizeof(short) );
	// 反填word
	CopyMemory(pCharIndexBuf + 4, &sWord, sizeof(short) );

	// copy character index 部分	
	CopyMemory(m_pHeadBuf + dwHeadOffset, pCharIndexBuf, dwCharIndexBufOffset);
	dwHeadOffset += dwCharIndexBufOffset;

	// copy charactr buffer	
	DWORD dwIKBuffOffset = 0;
	m_dwIKBufLen = dwCharOffset;
	m_pIKBuf = new BYTE[m_dwIKBufLen];
	ZeroMemory(m_pIKBuf, m_dwIKBufLen);

	// 
	CopyMemory(m_pIKBuf + dwIKBuffOffset, m_pHeadBuf, m_dwHeadBufSize);
	dwIKBuffOffset += m_dwHeadBufSize;

	for ( i = 0; i < iBuildCharIndex; i++ )
	{		
		PBYTE pCharBuffer = (PBYTE)m_vtBuildCharBuffer[i];
		CopyMemory(m_pIKBuf + dwIKBuffOffset, pCharBuffer , m_vtCharSize[i]);
		dwIKBuffOffset += m_vtCharSize[i];
	}

	if (pCharIndexBuf) delete[] pCharIndexBuf;
	pCharIndexBuf = NULL;

}	// end of void	CIKFontCodec::Build_IK()

unsigned long CIKFontCodec::BuildCharBuffer(PIK_CHARACTER pCharacter, short sBaseLine)
{
	//if (pCharacter->tagImageInfo.usPointNum == 0 ) return 0;

	short s = 0;
	//构建IMAGE buffer
	DWORD	pImagePoint[HY_POINT_MAXLEN];
	int		ImageOffset = 0;	// 按字节偏移

	IK_CHAR_CONTOUR_INDEX sztagContour[HY_POINT_MAXLEN];
	ZeroMemory(sztagContour, sizeof(IK_CHAR_CONTOUR_INDEX) * HY_POINT_MAXLEN);
	int		ContourIndex = 0;

	// Image
	// records << 1 因为一个点包含了2个WORD + 2 要把 record 和 words加进去
	s = ((pCharacter->tagImageInfo.usPointNum<<1) + 2)/IK_RECORD;
	pCharacter->tagImageInfo.usRecord = s;
	CopyMemory((PBYTE)pImagePoint + ImageOffset, &s, sizeof(short));
	ImageOffset += 2;

	// words 
	s = ((pCharacter->tagImageInfo.usPointNum << 1) + 2)% IK_RECORD;
	pCharacter->tagImageInfo.usWords = s;
	CopyMemory((PBYTE)pImagePoint + ImageOffset, &s, sizeof(short));
	ImageOffset += 2;

	int i = 0;
	for ( i = 0; i < pCharacter->tagImageInfo.usPointNum; i++)
	{
		if ( pCharacter->tagImageInfo.pIKPoint[i].usK == IK_PT_FLG_START )
		{
			// X
			s = -( 1 + pCharacter->tagImageInfo.pIKPoint[i].usRealX - pCharacter->tagCharSetting.usLeftBearing - pCharacter->tagCharSetting.usXMin ); 
			s = s == 0 ? -1: s;
			CopyMemory((PBYTE)pImagePoint + ImageOffset, &s, sizeof(short));
			ImageOffset += 2;

			pCharacter->tagImageInfo.pIKPoint[i].usSrcX = s;

			// Y 
			s =  1 + pCharacter->tagImageInfo.pIKPoint[i].usRealY - sBaseLine - pCharacter->tagCharSetting.usYMin;
			s = s == 0 ? 1: s;
			CopyMemory((PBYTE)pImagePoint + ImageOffset, &s, sizeof(short));
			ImageOffset += 2;

			pCharacter->tagImageInfo.pIKPoint[i].usSrcY = s;

			if (i > 0 )
			{
				if ( ContourIndex == 1 ) sztagContour[ContourIndex - 1].usDigitiNumber = i;
				else sztagContour[ContourIndex - 1].usDigitiNumber = i - sztagContour[ContourIndex - 1].usStartPointOffset;
			}
			sztagContour[ContourIndex].usRotation = 0xFFFF;
			sztagContour[ContourIndex].usStartPointOffset = i;
			ContourIndex++;
		}

		if ( pCharacter->tagImageInfo.pIKPoint[i].usK == IK_PT_FLG_CORNER )
		{
			// X
			s = -( 1 + pCharacter->tagImageInfo.pIKPoint[i].usRealX - pCharacter->tagCharSetting.usLeftBearing - pCharacter->tagCharSetting.usXMin ); 
			s = s == 0 ? -1: s;
			CopyMemory((PBYTE)pImagePoint + ImageOffset, &s, sizeof(short));
			ImageOffset += 2;

			pCharacter->tagImageInfo.pIKPoint[i].usSrcX = s;

			// Y 
			s = -( 1 + pCharacter->tagImageInfo.pIKPoint[i].usRealY - sBaseLine - pCharacter->tagCharSetting.usYMin );
			s = s == 0 ? -1: s;
			CopyMemory((PBYTE)pImagePoint + ImageOffset, &s, sizeof(short));
			ImageOffset += 2;

			pCharacter->tagImageInfo.pIKPoint[i].usSrcY = s;
		}

		if ( pCharacter->tagImageInfo.pIKPoint[i].usK == IK_PT_FLG_CURVE )
		{
			// X
			s =  1 + pCharacter->tagImageInfo.pIKPoint[i].usRealX - pCharacter->tagCharSetting.usLeftBearing - pCharacter->tagCharSetting.usXMin; 
			s = s == 0 ? 1: s; 
			CopyMemory((PBYTE)pImagePoint + ImageOffset, &s, sizeof(short));
			ImageOffset += 2;
			pCharacter->tagImageInfo.pIKPoint[i].usSrcX = s;

			// Y 
			s =  1 + pCharacter->tagImageInfo.pIKPoint[i].usRealY - sBaseLine - pCharacter->tagCharSetting.usYMin;
			s = s == 0 ? 1: s; 
			CopyMemory((PBYTE)pImagePoint + ImageOffset, &s, sizeof(short));
			ImageOffset += 2;

			pCharacter->tagImageInfo.pIKPoint[i].usSrcY= s;
		}

		if ( pCharacter->tagImageInfo.pIKPoint[i].usK == IK_PT_FLG_TANGENT )
		{
			// X
			s =  1 + pCharacter->tagImageInfo.pIKPoint[i].usRealX - pCharacter->tagCharSetting.usLeftBearing - pCharacter->tagCharSetting.usXMin; 
			s = s == 0 ? 1: s;
			CopyMemory((PBYTE)pImagePoint + ImageOffset, &s, sizeof(short));
			ImageOffset += 2;

			pCharacter->tagImageInfo.pIKPoint[i].usSrcX = s;

			// Y 
			s = -( 1 + pCharacter->tagImageInfo.pIKPoint[i].usRealY - sBaseLine - pCharacter->tagCharSetting.usYMin );
			s = s == 0 ? -1: s;
			CopyMemory((PBYTE)pImagePoint + ImageOffset, &s, sizeof(short));
			ImageOffset += 2;

			pCharacter->tagImageInfo.pIKPoint[i].usSrcY = s;
		}
	}
	if (ContourIndex > 0) 
	{
		if (ContourIndex == 1 ) sztagContour[ContourIndex - 1].usDigitiNumber = pCharacter->tagImageInfo.usPointNum;
		else sztagContour[ContourIndex - 1].usDigitiNumber= pCharacter->tagImageInfo.usPointNum - sztagContour[ContourIndex - 1].usStartPointOffset;
	}

	//构建Contour buffer
	PWORD	pContourBuf[HY_POINT_MAXLEN];
	int		iContourOffset = 0;	// 按字节偏移

	// record
	s = ContourIndex/IK_RECORD;
	CopyMemory((PBYTE)pContourBuf + iContourOffset, &s, sizeof(short));
	iContourOffset += 2;
	// words
	s = (ContourIndex * 6 +2)% IK_RECORD;
	CopyMemory((PBYTE)pContourBuf + iContourOffset, &s, sizeof(short));
	iContourOffset += 2;

	int iWrodPointer = 0;
	for ( i = 0; i < ContourIndex; i++ )
	{
		// record pointer for n coutour 
		if ( i == 0 )
		{
			// record
			s = 0;
			CopyMemory((PBYTE)pContourBuf + iContourOffset, &s, sizeof(short));
			iContourOffset += 2;

			// word
			iWrodPointer = s = 3;
			CopyMemory((PBYTE)pContourBuf + iContourOffset, &s, sizeof(short));
			iContourOffset += 2; 
		}
		else
		{
			// record 一个点需要2个word 所以usDigitiNumber 需要乘以2
			s = (iWrodPointer + (sztagContour[i -1].usDigitiNumber << 1))/IK_RECORD ;
			CopyMemory((PBYTE)pContourBuf + iContourOffset, &s, sizeof(short));
			iContourOffset += 2;

			// word point 
			s = iWrodPointer + (sztagContour[i -1].usDigitiNumber << 1) % IK_RECORD ;
			CopyMemory((PBYTE)pContourBuf + iContourOffset, &s, sizeof(short));
			iContourOffset += 2;

			iWrodPointer += sztagContour[i -1].usDigitiNumber << 1;
		}

		// sense of ration
		s = sztagContour[i].usRotation;
		CopyMemory((PBYTE)pContourBuf + iContourOffset, &s, sizeof(short));
		iContourOffset += 2;

		// nesting
		s = sztagContour[i].usNest;
		CopyMemory((PBYTE)pContourBuf + iContourOffset, &s, sizeof(short));
		iContourOffset += 2;

		// colour
		s = sztagContour[i].usColor;
		CopyMemory((PBYTE)pContourBuf + iContourOffset, &s, sizeof(short));
		iContourOffset += 2;
		// number of digitizations
		s = sztagContour[i].usDigitiNumber; 
		CopyMemory((PBYTE)pContourBuf + iContourOffset, &s, sizeof(short));
		iContourOffset += 2;
	}

	// 构建CharBuf
	unsigned long CharBufLength = 17 * 2;
	CharBufLength += iContourOffset;
	CharBufLength += ImageOffset;

	PBYTE  pCharBuf = new BYTE[CharBufLength];	
	ZeroMemory(pCharBuf, CharBufLength);

	DWORD  dwOffset = 0;	
	// recodes 
	s=  (short)(CharBufLength>>1)/IK_RECORD;
	CopyMemory((PBYTE)pCharBuf + dwOffset, &s, sizeof(short));
	dwOffset += 2;

	// words
	s=(short)(CharBufLength>>1)%IK_RECORD;
	CopyMemory((PBYTE)pCharBuf + dwOffset, &s, sizeof(short));
	dwOffset += 2;

	// length of name section
	s=pCharacter->tagCharNameinf.usNameLength;
	CopyMemory((PBYTE)pCharBuf + dwOffset, &s, sizeof(short));
	dwOffset += 2;

	// character number
	WORD wCharNum =pCharacter->tagCharNameinf.usCharacterNo;
	if (wCharNum >= 0x8000 )
		wCharNum ++;
	CopyMemory((PBYTE)pCharBuf + dwOffset, &wCharNum, sizeof(short));
	dwOffset += 2;

	// number of follwing part
	s=pCharacter->tagCharNameinf.usfollowNumber;
	CopyMemory((PBYTE)pCharBuf + dwOffset, &s, sizeof(short));
	dwOffset += 2;

	// length of setting information
	s=pCharacter->tagCharSetting.usSetLength;
	CopyMemory((PBYTE)pCharBuf + dwOffset, &s, sizeof(short));
	dwOffset += 2;

	// character type = letter
	s=pCharacter->tagCharSetting.usCharacterType;
	CopyMemory((PBYTE)pCharBuf + dwOffset, &s, sizeof(short));
	dwOffset += 2;

	// number of digitizations(多少个点)
	s= pCharacter->tagImageInfo.usPointNum;
	CopyMemory((PBYTE)pCharBuf + dwOffset, &s, sizeof(short));
	dwOffset += 2;

	// Total set Width
	s=pCharacter->tagCharSetting.usTotalWidth;
	CopyMemory((PBYTE)pCharBuf + dwOffset, &s, sizeof(short));
	dwOffset += 2;

	// left side bearing
	s=pCharacter->tagCharSetting.usLeftBearing;
	CopyMemory((PBYTE)pCharBuf + dwOffset, &s, sizeof(short));
	dwOffset += 2;

	// width
	s=pCharacter->tagCharSetting.usWidth;
	CopyMemory((PBYTE)pCharBuf + dwOffset, &s, sizeof(short));
	dwOffset += 2;

	// right side bearing
	s=pCharacter->tagCharSetting.usRightBearing;
	CopyMemory((PBYTE)pCharBuf + dwOffset, &s, sizeof(short));
	dwOffset += 2;

	// x-minimum
	s=pCharacter->tagCharSetting.usXMin;
	CopyMemory((PBYTE)pCharBuf + dwOffset, &s, sizeof(short));
	dwOffset += 2;

	// x-maximum
	s=pCharacter->tagCharSetting.usXMax;
	CopyMemory((PBYTE)pCharBuf + dwOffset, &s, sizeof(short));
	dwOffset += 2;

	// y-minimum
	s=pCharacter->tagCharSetting.usYMin;
	CopyMemory((PBYTE)pCharBuf + dwOffset, &s, sizeof(short));
	dwOffset += 2;

	// y-maximum
	s=pCharacter->tagCharSetting.usYMax;
	CopyMemory((PBYTE)pCharBuf + dwOffset, &s, sizeof(short));
	dwOffset += 2;

	// unit is 1/100
	s=pCharacter->tagCharSetting.usUnit;
	CopyMemory((PBYTE)pCharBuf + dwOffset, &s, sizeof(short));
	dwOffset += 2;

	// 拷贝Contour
	if (ContourIndex > 0)
	{
		CopyMemory((PBYTE)pCharBuf + dwOffset, (PBYTE)pContourBuf, iContourOffset);
		dwOffset += iContourOffset;
	}

	if (pCharacter->tagImageInfo.usPointNum > 0)
	{
		// 拷贝image
		CopyMemory((PBYTE)pCharBuf + dwOffset, (PBYTE)pImagePoint, ImageOffset);
		dwOffset += ImageOffset;
	}	

	m_vtBuildCharBuffer.push_back(pCharBuf);
	m_vtCharSize.push_back(CharBufLength);
	m_vtCharNum.push_back(pCharacter->tagCharNameinf.usCharacterNo);

	return CharBufLength;

}	// end of int	CIKFontCodec::BuildCharBuffer()

CIKFont*  CIKFontCodec::DecodeIK(const CString& strFileName)	
{
	UninitParam();

	//TRY 
	//{
		CFile	file(strFileName, CFile::modeRead );
		m_dwIKBufLen = (DWORD)file.GetLength();

		m_pIKBuf = new unsigned char[m_dwIKBufLen];
		ZeroMemory(m_pIKBuf, m_dwIKBufLen);

		file.Read(m_pIKBuf, m_dwIKBufLen );
		file.Close();

		int iDataFormate = 0;
		iDataFormate = AnalyseDataFormat(m_pIKBuf);
		if (IK_DTFMT_UNKOWN == iDataFormate ) 
		{
			UninitParam();
			return NULL;
		}

		if ( IK_DTFMT_BIG_ENDIAN == iDataFormate)
		{
			ConvtertBigTOLittle(m_pIKBuf, m_dwIKBufLen);
		}

		if ( Analyse_HEAD(m_pIKBuf,  m_dwIKOffset, m_IKFont.GetFont()) != HY_NOERROR)
		{
			UninitParam();
			return NULL;
		}

		//m_IKFont.GetFont()->tagHead.tagIKFontInf.usBodySize = 12500;

		if ( Analyse_CHARACTER(m_pIKBuf , m_dwIKOffset, m_IKFont.GetFont()) != HY_NOERROR )
		{
			UninitParam();
			return NULL;
		}
	//}
	//CATCH (CMemoryException, e)
	//{
		//UninitParam();
	//}
	//END_CATCH

	return &m_IKFont;

}	// end of int CIKFontCodec::DecodeIK()

int	CIKFontCodec::AnalyseDataFormat(unsigned char* pBuf)
{
	DWORD dwOffset = 2;
	WORD  wFlag = 0;

	CopyMemory( &wFlag, pBuf + dwOffset, sizeof(WORD));

	if (wFlag == 0x0037) 
		return IK_DTFMT_LITTLE_ENDIAN;

	if (wFlag == 0x3700) 
		return IK_DTFMT_BIG_ENDIAN;


	/*
	if (wFlag == 0x494B || wFlag == 0x696B) 
		return IK_DTFMT_LITTLE_ENDIAN;

	if (wFlag == 0x4B49 || wFlag == 0x6B69) 
		return IK_DTFMT_BIG_ENDIAN;
	*/
	return IK_DTFMT_UNKOWN;

}	// end of int	CIKFontCodec::AnalyseDataFormat()

int	CIKFontCodec::ConvtertBigTOLittle(unsigned char* pBuf, DWORD dwBufLeng)
{
	DWORD	dwOffset = 0;

	//unsigned char* pTmpBuf = new unsigned char[dwBufLeng];
	//ZeroMemory(pTmpBuf, dwBufLeng);

	WORD wTmp = 0, wTmp2 =0;
	while( dwOffset < dwBufLeng )
	{
		CopyMemory( &wTmp, pBuf + dwOffset, sizeof(WORD));
		wTmp2 = ntohs(wTmp); 
		CopyMemory( pBuf + dwOffset, &wTmp2, sizeof(WORD));
		dwOffset+= sizeof(WORD);
	}

	return HY_NOERROR;

}	// end of int	CIKFontCodec::ConvtertBigTOLittle()

int CIKFontCodec::Analyse_HEAD(unsigned char* pBuf, DWORD& dwOffset, PIK_FONT pIK)
{
	IK_HEAD& tagHead = pIK->tagHead;

	// length of header
	CopyMemory( &tagHead.usIKLength, pBuf + dwOffset, sizeof(WORD));
	dwOffset += sizeof(WORD);

	// name section
	// lenght of name seciton
	CopyMemory( &tagHead.tagName.usNameLength, pBuf + dwOffset, sizeof(WORD));
	dwOffset += sizeof(WORD);

	// nubmer on URW list
	CopyMemory( &tagHead.tagName.usURWLstNum, pBuf + dwOffset, sizeof(WORD));
	dwOffset += sizeof(WORD);

	// File name
	CopyMemory( tagHead.tagName.FileName, pBuf + dwOffset, IK_FILENAME_LENG);
	dwOffset += IK_FILENAME_LENG;

	// Font name
	CopyMemory( tagHead.tagName.FontName, pBuf + dwOffset, IK_FONTNAME_LENG);
	dwOffset += IK_FONTNAME_LENG;

	// Data format
	CopyMemory( &tagHead.tagName.usDataFormat, pBuf + dwOffset, sizeof(WORD));
	dwOffset += sizeof(WORD);

	// Production date
	CopyMemory( tagHead.tagName.usProductDate, pBuf + dwOffset, sizeof(WORD) * 3);
	dwOffset += sizeof(WORD) * 3;

	// date of change 
	CopyMemory( tagHead.tagName.usChangeDate, pBuf + dwOffset, sizeof(WORD) * 3);
	dwOffset += sizeof(WORD) * 3;

	// font information
	// length of font information
	CopyMemory( &tagHead.tagIKFontInf.usFontLength, pBuf + dwOffset, sizeof(WORD));
	dwOffset += sizeof(WORD);

	// indicator for font
	CopyMemory( &tagHead.tagIKFontInf.usIndicator, pBuf + dwOffset, sizeof(WORD));
	dwOffset += sizeof(WORD);

	// number of characters
	CopyMemory( &tagHead.tagIKFontInf.usCharNum, pBuf + dwOffset, sizeof(WORD));
	dwOffset += sizeof(WORD);

	// cap height
	CopyMemory( &tagHead.tagIKFontInf.usCapHeight, pBuf + dwOffset, sizeof(WORD));
	dwOffset += sizeof(WORD);

	// Body size
	CopyMemory( &tagHead.tagIKFontInf.usBodySize, pBuf + dwOffset, sizeof(WORD));
	dwOffset += sizeof(WORD);

	// x-height
	CopyMemory( &tagHead.tagIKFontInf.usX_Height, pBuf + dwOffset, sizeof(WORD));
	dwOffset += sizeof(WORD);

	// Distance base line
	CopyMemory( &tagHead.tagIKFontInf.usBaseLine, pBuf + dwOffset, sizeof(WORD));
	dwOffset += sizeof(WORD);

	// Text line distance for setting
	CopyMemory( &tagHead.tagIKFontInf.usTextLine, pBuf + dwOffset, sizeof(WORD));
	dwOffset += sizeof(WORD);

	// stem thickness
	CopyMemory( &tagHead.tagIKFontInf.usStemThickness, pBuf + dwOffset, sizeof(WORD));
	dwOffset += sizeof(WORD);

	// angle of italisation
	CopyMemory( &tagHead.tagIKFontInf.usAngle, pBuf + dwOffset, sizeof(WORD));
	dwOffset += sizeof(WORD);

	// optimum point size for setting
	CopyMemory( &tagHead.tagIKFontInf.usOptimum, pBuf + dwOffset, sizeof(WORD));
	dwOffset += sizeof(WORD);

	// average character width
	CopyMemory( &tagHead.tagIKFontInf.usAverageWidth, pBuf + dwOffset, sizeof(WORD));
	dwOffset += sizeof(WORD);

	// hierarchy section
	CopyMemory( &tagHead.usHierarchy, pBuf + dwOffset, sizeof(WORD));
	dwOffset += sizeof(WORD);	

	// character index
	CopyMemory( &tagHead.tagCharacterInf.usCharInfLength, pBuf + dwOffset, sizeof(WORD));
	dwOffset += sizeof(WORD);

	CopyMemory( &tagHead.tagCharacterInf.usPhysicalLength, pBuf + dwOffset, sizeof(WORD));
	dwOffset += sizeof(WORD);

	CopyMemory( &tagHead.tagCharacterInf.usLastWord, pBuf + dwOffset, sizeof(WORD));
	dwOffset += sizeof(WORD); 

	unsigned long ulBaseOffset = 0;
	tagHead.tagCharacterInf.pCharaterIndex = new IK_CHARACTER_INDEX[tagHead.tagIKFontInf.usCharNum];
	for (int i = 0; i < tagHead.tagIKFontInf.usCharNum; i++)
	{
		CopyMemory( &tagHead.tagCharacterInf.pCharaterIndex[i].usCharNum, pBuf + dwOffset, sizeof(WORD));
		dwOffset += sizeof(WORD);

		CopyMemory( &tagHead.tagCharacterInf.pCharaterIndex[i].usStartRecord, pBuf + dwOffset, sizeof(WORD));
		dwOffset += sizeof(WORD);
		
		CopyMemory( &tagHead.tagCharacterInf.pCharaterIndex[i].usWordPoint, pBuf + dwOffset, sizeof(WORD));
		dwOffset += sizeof(WORD);

		// 因为IK中索引以1开头，所以需要-1才能变成以0开头
		ulBaseOffset = (( tagHead.tagCharacterInf.pCharaterIndex[i].usStartRecord - 1 ) * IK_RECORD + tagHead.tagCharacterInf.pCharaterIndex[i].usWordPoint - 1 ) << 1;

		tagHead.tagCharacterInf.pCharaterIndex[i].ulCharOffset = ulBaseOffset;

		// 容错处理 在IK中大于0x800的char num 需要减一(short)类型
		if (tagHead.tagCharacterInf.pCharaterIndex[i].usCharNum > 0x8000)
		{
			tagHead.tagCharacterInf.pCharaterIndex[i].usCharNum -=1;
		}
	}

	return HY_NOERROR;

}	// end of int CIKFontCodec::Analyse_HEAD()

int	CIKFontCodec::Analyse_CHARACTER(unsigned char* pBuf, DWORD& dwOffset,PIK_FONT pIK)
{
	CString strPrint;
	IK_HEAD	tagHead = pIK->tagHead;
	int i = 0, j = 0;
	pIK->pCharacter = new IK_CHARACTER[tagHead.tagIKFontInf.usCharNum];
	ZeroMemory(pIK->pCharacter, sizeof(IK_CHARACTER) * tagHead.tagIKFontInf.usCharNum);

#if 0
	CString strFileName;
	CTime	tm = CTime::GetCurrentTime();
	strFileName.Format(_T("D:\\%d-%d-%d.txt"), tm.GetHour(), tm.GetMinute(), tm.GetSecond() );
	CStdioFile		Infoxfile(strFileName, CFile::modeCreate | CFile::modeWrite | CFile::typeText );
#endif 

	for ( i = 0; i < tagHead.tagIKFontInf.usCharNum; i++ )
	{
		IK_CHARACTER&			tagIkChar = pIK->pCharacter[i];
		IK_CHARACTER_INDEX&		tagCharacterIndex = pIK->tagHead.tagCharacterInf.pCharaterIndex[i];
		dwOffset = tagCharacterIndex.ulCharOffset;

		CopyMemory( &tagIkChar.usRecords, pBuf + dwOffset, sizeof(WORD) );
		dwOffset += sizeof(WORD);

		CopyMemory( &tagIkChar.usWords, pBuf + dwOffset, sizeof(WORD) );
		dwOffset += sizeof(WORD);

		// name seciton
		CopyMemory( &tagIkChar.tagCharNameinf.usNameLength, pBuf + dwOffset, sizeof(WORD) );
		dwOffset += sizeof(WORD);
		
		// 以 head 中 character index中的 character num 为准，
		CopyMemory( &tagIkChar.tagCharNameinf.usCharacterNo, &tagHead.tagCharacterInf.pCharaterIndex[i].usCharNum, sizeof(WORD) );
		dwOffset += sizeof(WORD);

		CopyMemory( &tagIkChar.tagCharNameinf.usfollowNumber, pBuf + dwOffset, sizeof(WORD) );
		dwOffset += sizeof(WORD);

		// setting information
		CopyMemory( &tagIkChar.tagCharSetting.usSetLength, pBuf + dwOffset, sizeof(SHORT) );
		dwOffset += sizeof(SHORT);

		CopyMemory( &tagIkChar.tagCharSetting.usCharacterType, pBuf + dwOffset, sizeof(SHORT) );
		dwOffset += sizeof(SHORT);

		CopyMemory( &tagIkChar.tagCharSetting.usDigitiNumber, pBuf + dwOffset, sizeof(SHORT) );
		dwOffset += sizeof(SHORT);

		CopyMemory( &tagIkChar.tagCharSetting.usTotalWidth, pBuf + dwOffset, sizeof(SHORT) );
		dwOffset += sizeof(SHORT);

		CopyMemory( &tagIkChar.tagCharSetting.usLeftBearing, pBuf + dwOffset, sizeof(SHORT) );
		dwOffset += sizeof(SHORT);

		CopyMemory( &tagIkChar.tagCharSetting.usWidth, pBuf + dwOffset, sizeof(SHORT) );
		dwOffset += sizeof(SHORT);

		CopyMemory( &tagIkChar.tagCharSetting.usRightBearing, pBuf + dwOffset, sizeof(SHORT) );
		dwOffset += sizeof(SHORT);

		CopyMemory( &tagIkChar.tagCharSetting.usXMin, pBuf + dwOffset, sizeof(SHORT) );
		dwOffset += sizeof(SHORT);

		CopyMemory( &tagIkChar.tagCharSetting.usXMax, pBuf + dwOffset, sizeof(SHORT) );
		dwOffset += sizeof(SHORT);

		CopyMemory( &tagIkChar.tagCharSetting.usYMin, pBuf + dwOffset, sizeof(SHORT) );
		dwOffset += sizeof(SHORT);

		CopyMemory( &tagIkChar.tagCharSetting.usYMax, pBuf + dwOffset, sizeof(SHORT) );
		dwOffset += sizeof(SHORT);

		CopyMemory( &tagIkChar.tagCharSetting.usUnit, pBuf + dwOffset, sizeof(SHORT) );
		dwOffset += sizeof(SHORT);

		// contour index
		CopyMemory( &tagIkChar.tagContourInfo.usRecords, pBuf + dwOffset, sizeof(WORD) );
		dwOffset += sizeof(WORD);

		CopyMemory( &tagIkChar.tagContourInfo.usWords, pBuf + dwOffset, sizeof(WORD) );
		dwOffset += sizeof(WORD);

		//int iCoutourNum =((tagIkChar.usRecords * IK_RECORD) + (tagIkChar.tagContourInfo.usWords - 2)) / 6; // size of IK_CHAR_CONTOUR_INDEX == 6
		int iCoutourNum =((tagIkChar.tagContourInfo.usRecords * IK_RECORD) + (tagIkChar.tagContourInfo.usWords - 2)) / 6; // size of IK_CHAR_CONTOUR_INDEX == 6
		iCoutourNum = iCoutourNum > 0 ? iCoutourNum : 0;
		tagIkChar.tagContourInfo.usContourNum = iCoutourNum;

		if (iCoutourNum  > 0 )
		{
			tagIkChar.tagContourInfo.pContourIndex = new IK_CHAR_CONTOUR_INDEX[iCoutourNum];
			ZeroMemory(tagIkChar.tagContourInfo.pContourIndex, sizeof(IK_CHAR_CONTOUR_INDEX) * iCoutourNum);
		}

		int  iPointOffset = 0;
		for ( j = 0; j < iCoutourNum; j++ )
		{
			CopyMemory( &tagIkChar.tagContourInfo.pContourIndex[j].usRecordsPointer, pBuf + dwOffset, sizeof(WORD) );
			dwOffset += sizeof(WORD);

			CopyMemory( &tagIkChar.tagContourInfo.pContourIndex[j].usWordPointer, pBuf + dwOffset, sizeof(WORD) );
			dwOffset += sizeof(WORD);

			CopyMemory( &tagIkChar.tagContourInfo.pContourIndex[j].usRotation, pBuf + dwOffset, sizeof(WORD) );
			dwOffset += sizeof(WORD);

			CopyMemory( &tagIkChar.tagContourInfo.pContourIndex[j].usNest, pBuf + dwOffset, sizeof(WORD) );
			dwOffset += sizeof(WORD);

			CopyMemory( &tagIkChar.tagContourInfo.pContourIndex[j].usColor, pBuf + dwOffset, sizeof(WORD) );
			dwOffset += sizeof(WORD);

			CopyMemory( &tagIkChar.tagContourInfo.pContourIndex[j].usDigitiNumber, pBuf + dwOffset, sizeof(WORD) );
			dwOffset += sizeof(WORD);

			// 计算轮廓起始点在IMAGE中的位置偏移
			tagIkChar.tagContourInfo.pContourIndex[j].usStartPointOffset = iPointOffset;
			iPointOffset += tagIkChar.tagContourInfo.pContourIndex[j].usDigitiNumber;
		}

		// image information
		CopyMemory( &tagIkChar.tagImageInfo.usRecord, pBuf + dwOffset, sizeof(WORD) );
		dwOffset += sizeof(WORD);

		CopyMemory( &tagIkChar.tagImageInfo.usWords, pBuf + dwOffset, sizeof(WORD) );
		dwOffset += sizeof(WORD);

		int	 iPointNum = ((tagIkChar.tagImageInfo.usRecord * IK_RECORD ) + (tagIkChar.tagImageInfo.usWords - 2) )/  2;
		iPointNum = iPointNum > 0 ? iPointNum : 0;
		tagIkChar.tagImageInfo.usPointNum = iPointNum;
		if (iPointNum > 0)
		{
			tagIkChar.tagImageInfo.pIKPoint = new IK_POINT[iPointNum];
			ZeroMemory(tagIkChar.tagImageInfo.pIKPoint, sizeof(IK_POINT) * iPointNum);
		}

		for ( j = 0; j < iPointNum; j++ )
		{
			CopyMemory( &tagIkChar.tagImageInfo.pIKPoint[j].usSrcX, pBuf + dwOffset, sizeof(WORD) );
			dwOffset += sizeof(WORD);

			CopyMemory( &tagIkChar.tagImageInfo.pIKPoint[j].usSrcY, pBuf + dwOffset, sizeof(WORD) );
			dwOffset += sizeof(WORD);

			// k == 12  start
			if (tagIkChar.tagImageInfo.pIKPoint[j].usSrcX < 0 && tagIkChar.tagImageInfo.pIKPoint[j].usSrcY > 0)
			{
				tagIkChar.tagImageInfo.pIKPoint[j].usK = IK_PT_FLG_START;
				tagIkChar.tagImageInfo.pIKPoint[j].usRealX = (-1 - tagIkChar.tagImageInfo.pIKPoint[j].usSrcX) + tagIkChar.tagCharSetting.usLeftBearing + tagIkChar.tagCharSetting.usXMin;
				tagIkChar.tagImageInfo.pIKPoint[j].usRealY = (-1 + tagIkChar.tagImageInfo.pIKPoint[j].usSrcY + tagIkChar.tagCharSetting.usYMin) + m_IKFont.GetHeadInfo()->tagIKFontInf.usBaseLine;
			}

			// k == 13 CORNER
			if (tagIkChar.tagImageInfo.pIKPoint[j].usSrcX  < 0 && tagIkChar.tagImageInfo.pIKPoint[j].usSrcY  < 0)
			{
				tagIkChar.tagImageInfo.pIKPoint[j].usK = IK_PT_FLG_CORNER;

				tagIkChar.tagImageInfo.pIKPoint[j].usRealX = (-1 - tagIkChar.tagImageInfo.pIKPoint[j].usSrcX) + tagIkChar.tagCharSetting.usLeftBearing + tagIkChar.tagCharSetting.usXMin;
				tagIkChar.tagImageInfo.pIKPoint[j].usRealY = (-1 - tagIkChar.tagImageInfo.pIKPoint[j].usSrcY + tagIkChar.tagCharSetting.usYMin) + m_IKFont.GetHeadInfo()->tagIKFontInf.usBaseLine;
			}

			// k == 14  CURVE
			if (tagIkChar.tagImageInfo.pIKPoint[j].usSrcX  > 0 && tagIkChar.tagImageInfo.pIKPoint[j].usSrcY > 0)
			{
				tagIkChar.tagImageInfo.pIKPoint[j].usK = IK_PT_FLG_CURVE;
				tagIkChar.tagImageInfo.pIKPoint[j].usRealX = (tagIkChar.tagImageInfo.pIKPoint[j].usSrcX - 1) + tagIkChar.tagCharSetting.usLeftBearing + tagIkChar.tagCharSetting.usXMin;
				tagIkChar.tagImageInfo.pIKPoint[j].usRealY = (-1 + tagIkChar.tagImageInfo.pIKPoint[j].usSrcY + tagIkChar.tagCharSetting.usYMin) + m_IKFont.GetHeadInfo()->tagIKFontInf.usBaseLine;
			}

			// k == 15 TANGENT
			if (tagIkChar.tagImageInfo.pIKPoint[j].usSrcX  > 0 && tagIkChar.tagImageInfo.pIKPoint[j].usSrcY < 0)
			{
				tagIkChar.tagImageInfo.pIKPoint[j].usK = IK_PT_FLG_TANGENT;
				tagIkChar.tagImageInfo.pIKPoint[j].usRealX  = (tagIkChar.tagImageInfo.pIKPoint[j].usSrcX - 1) + tagIkChar.tagCharSetting.usLeftBearing + tagIkChar.tagCharSetting.usXMin;
				tagIkChar.tagImageInfo.pIKPoint[j].usRealY  = (-1 - tagIkChar.tagImageInfo.pIKPoint[j].usSrcY + tagIkChar.tagCharSetting.usYMin) + m_IKFont.GetHeadInfo()->tagIKFontInf.usBaseLine;
			}
		}
	}

#if 0
	Infoxfile.Flush();
	Infoxfile.Close();
#endif 

	return HY_NOERROR;

}	// end of int	CIKFontCodec::Analyse_CHARACTER()id CIKFontCodec::Analyse_CHARACTER()

void CIKFontCodec::InitParam()
{
	InitIKBuf();

	m_IKFont.SetDefault();
	m_lstHYMaplst.SetDefault();

	m_pUnicodeLst = NULL;
	m_dwUnicodLstLen = 0;

}	// end of void CIKFontCodec::InitParam()

void CIKFontCodec::UninitParam()
{	
	UninitUnicodeLst();
	m_lstHYMaplst.SetDefault();
	m_IKFont.SetDefault();

	UinitIKBuf();

}	// end of void CIKFontCodec::UninitParam()

void CIKFontCodec::InitIKBuf()
{
	m_pIKBuf		= NULL;
	m_dwIKBufLen	= 0;
	m_dwIKOffset	= 0;
	InitVectorCharBuffer();
	m_vtCharSize.clear();
	m_vtCharNum.clear();

	m_pHeadBuf	= NULL;
	m_dwHeadBufSize = 0;

}	// end of void CIKFontCodec::InitIKBuf()

void CIKFontCodec::UinitIKBuf()
{	
	m_dwHeadBufSize = 0;
	if (m_pHeadBuf) delete[] m_pHeadBuf;
	m_pHeadBuf = NULL;

	m_vtCharNum.clear();
	m_vtCharSize.clear();
	UinitVectorCharBuffer();
	
	m_dwIKOffset	= 0;
	m_dwIKBufLen	= 0;
	if ( m_pIKBuf ) delete[] m_pIKBuf;
	m_pIKBuf		= NULL;	
	
}	// end of void CIKFontCodec::UinitIKBuf()

void CIKFontCodec::InitVectorCharBuffer()
{
	UinitVectorCharBuffer();

	m_vtBuildCharBuffer.clear();

}	// end of void CIKFontCodec::InitVectorCharBuffer()

void CIKFontCodec::UinitVectorCharBuffer()
{
	for (int i = 0 ; i < (int)m_vtBuildCharBuffer.size(); i++ )
	{
		PBYTE pCharBuffer = m_vtBuildCharBuffer.at(i);
		if ( pCharBuffer )
		{
			delete[] pCharBuffer;
			pCharBuffer = NULL;
		}
	}
	m_vtBuildCharBuffer.clear();

}	// end of void CIKFontCodec::UinitVectorCharBuffer()

void CIKFontCodec::InitUnicodeLst(int iLstLen)
{
	UninitUnicodeLst();

	m_pUnicodeLst = new DWORD[iLstLen];
	ZeroMemory(m_pUnicodeLst, iLstLen<<2 );

}	// end of void CIKFontCodec::InitUnicodeLst()

void CIKFontCodec::UninitUnicodeLst()
{
	if (m_pUnicodeLst) delete[] m_pUnicodeLst;
	m_pUnicodeLst = NULL;

	m_dwUnicodLstLen = 0;

}	// end of void CIKFontCodec::UninitUnicodeLst()


