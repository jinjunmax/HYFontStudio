#include "StdAfx.h"
#include "IKName.h"

namespace HYFONTCODEC
{

	CIKName::CIKName(void)
	{	
		SetDefault();

	}	// end of CIKName::CIKName(void)

	CIKName::~CIKName(void)
	{

	}	// end of CIKName::~CIKName(void)

	void CIKName::SetDefault()
	{
		sNameLength								= 0;						// 文件中Name部分的长度，不是结构体的大小
		usURWLstNumber							= 0;						// = 0	
		memset(FileName,0,IK_FILENAME_LENG);
		memset(FontName,0,IK_FONTNAME_LENG);	
		sDataFormat								= 0;						// IK	
		memset(sProductDate,0,IK_PRODUCTDATE_LENG);
		memset(sChangeDate,0,IK_CHANGEDATE_LENG);

	}	// end of void CIKName::SetDefault()

	CIKFontInf::CIKFontInf()
	{
		SetDefault();

	}	// end of CIKFontInf::CIKFontInf()

	CIKFontInf::~CIKFontInf()
	{

	}	// end of CIKFontInf::~CIKFontInf()

	void CIKFontInf::SetDefault()
	{
		sLength					= 0;
		sIndicator				= 0;
		sCharNums				= 0;
		sCapHeight				= 0;
		sBodySize				= 0;
		sX_Height				= 0;
		sTextLine				= 0;
		sStemThickness			= 0;
		sAngle					= 0;
		sOptimum				= 0;
		sAverageWidth			= 0;
		
	}	// end of void CIKFontInf::SetDefault()

	CIKCharIndex::CIKCharIndex()
	{
		SetDefault();

	}	// end of CIKCharIndex::CIKCharIndex()

	CIKCharIndex::~CIKCharIndex()
	{

	}	// end of CIKCharIndex::~CIKCharIndex()

	void CIKCharIndex::SetDefault()
	{
		sCharNumber		= 0;
		sStartRecord	= 0;
		sWordPoint		= 0;

	}	// end of void CIKCharIndex::SetDefault()

	CIKCharIndexInf::CIKCharIndexInf()
	{
		usCharIndexLength	= 0;
		usLastReocrd		= 0;
		usLastWord			= 0;

	}	// end of CIKCharIndexInf::CIKCharIndexInf()

	CIKCharIndexInf::~CIKCharIndexInf()
	{

	}	// end of CIKCharIndexInf::~CIKCharIndexInf()

	void CIKCharIndexInf::SetDefault()
	{
		usCharIndexLength	= 0;
		usLastReocrd			= 0;
		usLastWord			= 0;
		vtCharIndex.clear();	

	}	// end of void CIKCharIndexInf::SetDefault()

	CIKHeader::CIKHeader()
	{
		usLength			= 0;
		sHierarchy			= 0;		

	}	// end of void CIKHeader::CIKHeader()

	CIKHeader::~CIKHeader()
	{

	}	// end of void CIKHeader::~CIKHeader()

	void CIKHeader::SetDefault()
	{
		usLength			= 0;
		sHierarchy		= 0;

		Name.SetDefault();
		FontInf.SetDefault();
		CharIndexInf.SetDefault();

	}	// end of void CIKHeader::SetDefault()

	CIKCharName::CIKCharName()
	{
		SetDefault();

	}	// end of CIKCharName::CIKCharName()

	CIKCharName::~CIKCharName()
	{

	}	// end of CIKCharName::~CIKCharName()

	void CIKCharName::SetDefault()
	{
		sNameLength		= 0;
		sCharNumber		= 0;
		Reserved		= 0;

	}	// end of void CIKCharName::SetDefault()

	CIKCharSet::CIKCharSet()
	{
		SetDefault();

	}	// end of CIKCharSet::CIKCharSet()

	CIKCharSet::~CIKCharSet()
	{

	}	// end of CIKCharSet::~CIKCharSet()

	void CIKCharSet::SetDefault()
	{
		sSetLength			= 0;
		sCharacterType		= 0;
		sDigitiNumber		= 0;
		sTotalWidth			= 0;
		sLeftBearing		= 0;
		sWidth				= 0;
		sRightBearing		= 0;
		sXMin				= 0;
		sXMax				= 0;
		sYMin				= 0;
		sYMax				= 0;
		sUnit				= 0;

	}	// end of void CIKCharSet::SetDefault()

	CIKContourIndex::CIKContourIndex()
	{
		SetDefault();

	}	// end of CIKContourIndex::CIKContourIndex()

	CIKContourIndex::~CIKContourIndex()
	{

	}	// end of CIKContourIndex::~CIKContourIndex()

	void CIKContourIndex::SetDefault()
	{
		sRecordPoint	= 0;
		sWordPoint		= 0;
		sRotation		= 0;
		sHierarchy		= 0;
		sColor			= 0;
		sNumber			= 0;
		vtIKPoint.clear();

	}	// end of void CIKContourIndex::SetDefault()

	CIKContourInf::CIKContourInf()
	{
		sRecords		= 0;
		sWords			= 0;

	}	// end of CIKContourInf::CIKContourInf()

	CIKContourInf::~CIKContourInf()
	{

	}	// end of CIKContourInf::~CIKContourInf()

	void CIKContourInf::SetDefault()
	{
		sRecords		= 0;
		sWords			= 0;
		vtContourIndex.clear();	

	}	// end of void CIKContourInf::SetDefault()

	CIKPoint::CIKPoint()
	{
		x = 0;
		y = 0;
		t = 0;

	}	// end of CIKPoint::CIKPoint()

	CIKPoint::~CIKPoint()
	{

	}	// end of CIKPoint::~CIKPoint()

	void CIKPoint::SetDefault()
	{
		x = 0;
		y = 0;
		t = 0;

	}	// end of void CIKPoint::SetDefault()

	CIKCharacter::CIKCharacter()
	{
		sRecords	= 0;
		sWords		= 0;
		ImgLen		= 0;
		MapCode		= 0;

	}	// end of CIKCharacter::CIKCharacter()

	CIKCharacter::~CIKCharacter()
	{

	}	// end of CIKCharacter::~CIKCharacter()

	void CIKCharacter::SetDefault()
	{
		sRecords	= 0;
		sWords		= 0;
		ImgLen		= 0;
		MapCode		= 0;

		CharNameInf.SetDefault();
		CharSet.SetDefault();
		ContourInfo.SetDefault();	

	}	// end of void CIKCharacter::SetDefault()

	CIKFont::CIKFont()
	{

	}	// end of CIKFont::CIKFont()

	CIKFont::~CIKFont()
	{

	}	// end of CIKFont::~CIKFont()

	void CIKFont::SetDefault()
	{
		Header.SetDefault();	
		vtCharacter.clear();

	}	// end of void CIKFont::SetDefault()

	int	CIKFont::SetIkChar(size_t stIKIndex, CIKCharacter& IKChar)
	{
#if 0
		int iRT = IK_ERR_CHAR_NOEXIST;
		size_t szChars = vtCharacter.size();
		for (size_t i=0; i<szChars; i++)
		{
			CIKCharacter& Ik = vtCharacter[i];
			if (Ik.MapCode == ulUni)
			{				
				if (Ik.CharNameInf.sCharNumber != 0)
					iRT = IK_ERR_CHAR_REPEAT;
				else
					iRT  = HY_NOERROR;

				Ik			= IKChar;
				Ik.MapCode  = ulUni;
				return iRT;
			}
		}		
#else 
		if (stIKIndex == -1) return IK_ERR_CHAR_NOEXIST;

		int iRT = IK_ERR_CHAR_NOEXIST;
		CIKCharacter& Ik = vtCharacter[stIKIndex];					
		if (Ik.CharNameInf.sCharNumber!= 0)
			iRT = IK_ERR_CHAR_REPEAT;
		else
			iRT  = HY_NOERROR;

		Ik			= IKChar;		
#endif 	
		return iRT;

	}	// end of int	CIKFont::SetIkChar()

	size_t CIKFont::IkCharExistByMapCode(unsigned long ulUni)
	{
		int iRt = IK_ERR_CHAR_NOEXIST;
		size_t szChars = vtCharacter.size();
		size_t i=-1;
		for (i=0; i<szChars; i++)
		{
			CIKCharacter& Ik = vtCharacter[i];
			if (Ik.MapCode == ulUni) 
				return i;	
		}

		return -1;

	}	// end of int	CIKFont::IkCharExistByMapCode()

	size_t CIKFont::IkCharExistByIKNumber(unsigned short usIKNumber)
	{
		int iRt = IK_ERR_CHAR_NOEXIST;
		size_t szChars = vtCharacter.size();
		size_t i=-1;
		for (i=0; i<szChars; i++)
		{
			CIKCharacter& Ik = vtCharacter[i];
			if (Ik.CharNameInf.sCharNumber == usIKNumber)  
				return i;	
		}

		return -1;

	}	// end of int	CIKFont::IkCharExistByIKNumber()

	bool CompareChar(const CIKCharacter& char1, const CIKCharacter &char2)
	{	
		return char1.MapCode < char2.MapCode;
	
	}	// end of bool SortChar()

	void CIKFont::SortCharASC()
	{
		std::sort(vtCharacter.begin(), vtCharacter.end(),CompareChar);

	}	// end of bool CIKFont::SortCharASC()


}
