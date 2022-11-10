#include "StdAfx.h"
#include "IKFont.h"

CIKFont::CIKFont(void)
{
	ZeroMemory( &m_tagIKFont, sizeof(IK_FONT));

}	// end of CIKFont::CIKFont()

CIKFont::~CIKFont(void)
{
	SetDefault();

}	// end of CIKFont::~CIKFont()

void CIKFont::SetDefault()
{
	for (int i = 0; i < m_tagIKFont.tagHead.tagIKFontInf.usCharNum; i++ )
	{
		if ( m_tagIKFont.pCharacter[i].tagContourInfo.pContourIndex ) delete[] m_tagIKFont.pCharacter[i].tagContourInfo.pContourIndex;
		m_tagIKFont.pCharacter[i].tagContourInfo.pContourIndex = NULL;

		if ( m_tagIKFont.pCharacter[i].tagImageInfo.pIKPoint ) delete[] m_tagIKFont.pCharacter[i].tagImageInfo.pIKPoint;
		m_tagIKFont.pCharacter[i].tagImageInfo.pIKPoint = NULL;
	}

	if ( m_tagIKFont.pCharacter ) delete[] m_tagIKFont.pCharacter;
	m_tagIKFont.pCharacter = NULL;
	
	if ( m_tagIKFont.tagHead.tagCharacterInf.pCharaterIndex ) delete[] m_tagIKFont.tagHead.tagCharacterInf.pCharaterIndex;
	m_tagIKFont.tagHead.tagCharacterInf.pCharaterIndex = NULL;
	
	ZeroMemory(&m_tagIKFont, sizeof(IK_FONT));

}	// end of void CIKFont::SetDefault()

void CIKFont::operator=(CIKFont& other)
{
	SetDefault();	

	// IK_HEAD
	m_tagIKFont.tagHead.usIKLength = other.m_tagIKFont.tagHead.usIKLength;
	m_tagIKFont.tagHead.usHierarchy = other.m_tagIKFont.tagHead.usHierarchy;

	CopyMemory(&m_tagIKFont.tagHead.tagName, &other.m_tagIKFont.tagHead.tagName, sizeof(IK_NAME_INF) );
	CopyMemory(&m_tagIKFont.tagHead.tagIKFontInf, &other.m_tagIKFont.tagHead.tagIKFontInf, sizeof(IK_FONT_INF) );

	m_tagIKFont.tagHead.tagCharacterInf.usCharInfLength		=	other.m_tagIKFont.tagHead.tagCharacterInf.usCharInfLength;	
	m_tagIKFont.tagHead.tagCharacterInf.usPhysicalLength	=	other.m_tagIKFont.tagHead.tagCharacterInf.usPhysicalLength;  
	m_tagIKFont.tagHead.tagCharacterInf.usLastWord			=	other.m_tagIKFont.tagHead.tagCharacterInf.usLastWord;
	m_tagIKFont.tagHead.tagCharacterInf.usIndexNum			=	other.m_tagIKFont.tagHead.tagCharacterInf.usIndexNum;	
	m_tagIKFont.tagHead.tagCharacterInf.pCharaterIndex		= new IK_CHARACTER_INDEX[m_tagIKFont.tagHead.tagCharacterInf.usIndexNum];
	CopyMemory(m_tagIKFont.tagHead.tagCharacterInf.pCharaterIndex,other.m_tagIKFont.tagHead.tagCharacterInf.pCharaterIndex, sizeof(IK_CHARACTER_INDEX) * m_tagIKFont.tagHead.tagCharacterInf.usIndexNum);		

	// PIK_CHARACTER
	m_tagIKFont.pCharacter			= new IK_CHARACTER[m_tagIKFont.tagHead.tagIKFontInf.usCharNum];						
	for (int i = 0; i < m_tagIKFont.tagHead.tagIKFontInf.usCharNum; i++)
	{
		m_tagIKFont.pCharacter[i].usRecords			= other.m_tagIKFont.pCharacter[i].usRecords;
		m_tagIKFont.pCharacter[i].usWords			= other.m_tagIKFont.pCharacter[i].usWords;

		// IK_CHAR_NAME_INF
		CopyMemory( &m_tagIKFont.pCharacter[i].tagCharNameinf, &other.m_tagIKFont.pCharacter[i].tagCharNameinf, sizeof(IK_CHAR_NAME_INF) );
		// IK_CHAR_SETTING_INF
		CopyMemory( &m_tagIKFont.pCharacter[i].tagCharSetting, &other.m_tagIKFont.pCharacter[i].tagCharSetting, sizeof(IK_CHAR_SETTING_INF));

		// IK_CHAR_CONTOUR_INF
		m_tagIKFont.pCharacter[i].tagContourInfo.usRecords		= other.m_tagIKFont.pCharacter[i].tagContourInfo.usRecords;
		m_tagIKFont.pCharacter[i].tagContourInfo.usWords		= other.m_tagIKFont.pCharacter[i].tagContourInfo.usWords;
		m_tagIKFont.pCharacter[i].tagContourInfo.usContourNum	= other.m_tagIKFont.pCharacter[i].tagContourInfo.usContourNum;

		for (int j = 0; j < m_tagIKFont.pCharacter[i].tagContourInfo.usContourNum; j++ )
		{
			CopyMemory(&m_tagIKFont.pCharacter[i].tagContourInfo.pContourIndex[j],&other.m_tagIKFont.pCharacter[i].tagContourInfo.pContourIndex[j], sizeof(IK_CHAR_CONTOUR_INDEX) );
		}

		// IK_IMAGE_INF
		m_tagIKFont.pCharacter[i].tagImageInfo.usRecord		= other.m_tagIKFont.pCharacter[i].tagImageInfo.usRecord;
		m_tagIKFont.pCharacter[i].tagImageInfo.usWords		= other.m_tagIKFont.pCharacter[i].tagImageInfo.usWords;	
		m_tagIKFont.pCharacter[i].tagImageInfo.usPointNum	= other.m_tagIKFont.pCharacter[i].tagImageInfo.usPointNum;

		for (int j = 0; j < m_tagIKFont.pCharacter[i].tagImageInfo.usPointNum; j++  )
		{
			CopyMemory(&m_tagIKFont.pCharacter[i].tagImageInfo.pIKPoint[j], &other.m_tagIKFont.pCharacter[i].tagImageInfo.pIKPoint[j], sizeof(PIK_POINT) );	
		}
	}

}	// end of void operator=()

BOOL CIKFont::operator==(CIKFont& other)
{
	return FALSE;

}	// end of BOOL CIKFont::operator==()

BOOL CIKFont::operator!=(CIKFont& other)
{

	return !(*this == other);

}	// end of BOOL CIKFont::operator!=()

PIK_CHARACTER CIKFont::GetCharacterByIndex(int iIndex )
{
	if ( iIndex < 0 ) return NULL;
	if ( iIndex >= m_tagIKFont.tagHead.tagIKFontInf.usCharNum ) return NULL;

	return &m_tagIKFont.pCharacter[iIndex];

}	// end of PIK_CHARACTER CIKFont::GetCharacterByIndex()

PIK_CHARACTER CIKFont::GetCharacterByCharNo(int iNo )
{
	for (int i = 0; i < m_tagIKFont.tagHead.tagIKFontInf.usCharNum; i++ )
	{
		if ( m_tagIKFont.pCharacter[i].tagCharNameinf.usCharacterNo == iNo )
		{
			return &m_tagIKFont.pCharacter[i];
		}
	}

	return NULL;

}	// end of PIK_CHARACTER	CIKFont::GetCharacterByCharNo( )

int	CIKFont::GetIndexByCharNo(int iNo)
{
	for (int i = 0; i < m_tagIKFont.tagHead.tagIKFontInf.usCharNum; i++ )
	{
		if ( m_tagIKFont.pCharacter[i].tagCharNameinf.usCharacterNo == iNo )
		{
			return i;
		}
	}

	return -1;

}	// end of int	CIKFont::GetIndexByCharNo()
