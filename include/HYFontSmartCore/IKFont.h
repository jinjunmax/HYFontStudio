#pragma once

#include "DefIK.h"
class AFX_EXT_CLASS CIKFont
{
public:
	CIKFont(void);
	virtual ~CIKFont(void);

public:
	void SetDefault();
	void operator=(CIKFont& other);
	BOOL operator==(CIKFont& other);
	BOOL operator!=(CIKFont& other);

public:
	PIK_HEAD			GetHeadInfo()		{ return &m_tagIKFont.tagHead; }	
	PIK_CHARACTER		GetAllCharacter()	{ return m_tagIKFont.pCharacter; }	
	PIK_FONT			GetFont()			{ return &m_tagIKFont; }

	PIK_CHARACTER		GetCharacterByIndex(int iIndex );
	PIK_CHARACTER		GetCharacterByCharNo(int iNo );
	int					GetIndexByCharNo(int iNo);

	int					GetCharacterNums()	{ return m_tagIKFont.tagHead.tagIKFontInf.usCharNum; }

protected:
	IK_FONT				m_tagIKFont;
};
