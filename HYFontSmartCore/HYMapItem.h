#pragma once

class AFX_EXT_CLASS CHYMapItem
{
public:
	CHYMapItem(void);
	virtual ~CHYMapItem(void);

	void			SetDefault();
	void			operator=(const CHYMapItem& other);	
	bool			operator<( CHYMapItem& other ) const;
	bool			operator>( CHYMapItem& other ) const;


	void			SetGlyphIndex(int i)			{ m_iGlyphIndex = i; }
	int				GetGlyphIndex()					{ return m_iGlyphIndex; } 	
	void			SetGlyphNo(unsigned long l)		{ m_ulGlyphNo = l;}	
	unsigned long	GetGlyphNo()					{ return m_ulGlyphNo; }

	CString			GetCharName()					{ return m_strCharName; }
	void			SetCharName(CString strChar)	{ m_strCharName = strChar;}

public:	
	int				m_iGlyphIndex;
	unsigned long  	m_ulGlyphNo;
	CString			m_strCharName;
};


typedef std::vector<CHYMapItem> CHYMapItems;
class AFX_EXT_CLASS CHYMapList : public CHYMapItems
{

public:
	CHYMapList(void);
	virtual ~CHYMapList(void);

public:
	void		SetDefault();	
	void		operator=( CHYMapList& other) ;	
	
public:
	CHYMapItem*		FindItem(int iFocusedItem);
	int				InsertItem(CHYMapItem& info, int iFocusedItem = -1 );
	int				AddLast(CHYMapItem& info);
	int				RemoveItem(int iFocusedItem);
	int				SetItem(CHYMapItem& info, int iFocusedItem);

	// 为了应对一个字型对应多个Unicode时使用
	CHYMapItem*		FindNextItemByGryphIndex(int iGryphIndex, int &iNext);
	// 假设字型与Unicode一一对应
	CHYMapItem*		FindItemByGryphIndex(int iGryphIndex);
	CHYMapItem*		FindItemByUnicode(unsigned long ulUnicode);
	int 			FindIndexByUnicode(unsigned long ulUnicode);

	void			QuickSortbyUnicode();
public:
	int				NumItems();
	BOOL			IsNull();

protected:
	BOOL			IsItemValid(int iFocusedItem);
};