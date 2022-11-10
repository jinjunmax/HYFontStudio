#include "StdAfx.h"
#include "HYMapItem.h"

CHYMapItem::CHYMapItem(void)
{

}	// end of CHYMapItem::CHYMapItem()

CHYMapItem::~CHYMapItem(void)
{
	SetDefault();

}	// end of CHYMapItem::~CHYMapItem()

void CHYMapItem::SetDefault()
{	
	m_iGlyphIndex	= 0;
	m_ulGlyphNo		= 0;
	m_strCharName	= _T("");
	
}	// end of void CHYMapItem::SetDefault()

void CHYMapItem::operator=(const CHYMapItem& other)
{
	m_iGlyphIndex	= other.m_iGlyphIndex;
	m_ulGlyphNo		= other.m_ulGlyphNo;
	m_strCharName	= other.m_strCharName;

}	// end of void CHYMapItem::operator=()

bool CHYMapItem::operator<( CHYMapItem& other ) const 
{
	return  m_ulGlyphNo < other.m_ulGlyphNo;

}	// end of bool CHYMapItem::operator<( ) const 

bool CHYMapItem::operator>( CHYMapItem& other ) const
{
	return m_ulGlyphNo < other.m_ulGlyphNo;

}	// end of bool CHYMapItem::operator>( ) const

//////////////////////////////////////////////////////////////////////////////////////
// CHYMapList
CHYMapList::CHYMapList()
{
	SetDefault();

}	// end of CHYMapList::CHYMapList()

CHYMapList::~CHYMapList()
{	
	SetDefault();

}	// end of CHYMapList::CHYMapList()

void CHYMapList::SetDefault()
{	
	clear();

}	// end of void CHYMapList::SetDefault()

void CHYMapList::operator=( CHYMapList& other) 
{
	SetDefault();

	for (int i =0; i < const_cast<CHYMapList&>(other).NumItems(); i++ )
	{
		CHYMapItem* pItem = const_cast<CHYMapList&>(other).FindItem(i);
		AddLast(*pItem);
	}

}	// end of void CHYMapList::operator=()

CHYMapItem*	CHYMapList::FindItem(int iFocusedItem)
{
	vector<int>::size_type iIndex = iFocusedItem;
	if ( !IsItemValid(iIndex) ) return NULL;
	return &at(iIndex);

}	// end of CHYMapItem*	CHYMapList::FindItem()

int	 CHYMapList::InsertItem(CHYMapItem& info, int iFocusedItem )
{
	if ( iFocusedItem == -1 )
	{	
		insert( begin(),info);
		return 0;
	}

	if ( !IsItemValid(iFocusedItem) ) return -1;
	insert( begin()+ iFocusedItem, info );

	return iFocusedItem;

}	// end of int	 CHYMapList::InsertItem()

int	 CHYMapList::AddLast(CHYMapItem& info)
{
	push_back(info);
	return NumItems() - 1;

}	// end of int	 CHYMapList::AddTail()

int	 CHYMapList::RemoveItem(int iFocusedItem)
{
	if ( !IsItemValid(iFocusedItem) ) return -1;

	erase( begin() + iFocusedItem );

	int iNumItems = NumItems();
	if ( iNumItems == 0 )				return -1;
	if ( iNumItems == iFocusedItem )	return iFocusedItem - 1;

	return iFocusedItem;

}	// end of int CHYMapList::RemoveItem()

int	CHYMapList::SetItem(CHYMapItem& info, int iFocusedItem)
{	
	if ( !IsItemValid(iFocusedItem) )	return -1;

	vector<int>::size_type Index = iFocusedItem;

	at(Index) = info;
	return iFocusedItem;

}	// end of int CHYMapList::SetItem()

CHYMapItem*	 CHYMapList::FindNextItemByGryphIndex(int iGryphIndex, int &iNext)
{
	CHYMapItem* pItem = NULL;	
	if (iNext == -1 ) return NULL;

	int iFirstPos = -1; 
	int iBegin = iNext;
	iNext = -1;
	for ( int i = iBegin; i < NumItems(); i++ )
	{
		pItem = FindItem(i);
		if ( pItem->GetGlyphIndex() == iGryphIndex )
		{
			//frist find
			if ( iFirstPos == -1 )
			{
				iFirstPos = i;
			}
			else // second find
			{
				iNext = i;
				break;
			}
		}
	}

	if ( iFirstPos != -1 ) 
	{
		return FindItem(iFirstPos);
	}

	return NULL;

}	// end of CHYMapItem*	 CHYMapList::FindNextItemBy()

CHYMapItem*	 CHYMapList::FindItemByGryphIndex(int iGryphIndex)
{
	CHYMapItem* pItem = NULL;
	for ( int i = 0; i < NumItems(); i++ )
	{
		pItem = FindItem(i);
		if ( pItem->GetGlyphIndex() == iGryphIndex )
		{
			return pItem;
		}
	}
	
	return NULL;
	
}	// end of CHYMapItem*	 CHYMapList::FindItemByGryphIndex()

CHYMapItem*		CHYMapList::FindItemByUnicode(unsigned long ulUnicode)
{
	CHYMapItem* pItem = NULL;
	int iSize = size();
	for ( int i=0; i<iSize; i++)
	{		
		if ((*this)[i].m_ulGlyphNo == ulUnicode )
		{
			return &(*this)[i];
		}
	}

	return NULL;

}	// end of CHYMapItem*		CHYMapList::FindItemByUnicode()

int 	CHYMapList::FindIndexByUnicode(unsigned long ulUnicode)
{	
	CHYMapItem* pItem = NULL;
	int iSize = size();
	for ( int i=0; i<iSize; i++ )
	{
		if ( (*this)[i].m_ulGlyphNo == ulUnicode )
		{
			return i;
		}
	}

	return -1;

}	// end of int 	CHYMapList::FindItemByUnicode()

bool SortPredicate( CHYMapItem& d1,  CHYMapItem& d2)
{
	return ( d1.GetGlyphNo()< d2.GetGlyphNo());

}	// end of BOOL SortPredicate()

#include <algorithm>
void CHYMapList::QuickSortbyUnicode()
{
	std::sort( begin(), end(), SortPredicate );

}	// end of void CHYMapList::QuickSortbyUnicode()

int CHYMapList::NumItems() 
{
	return size(); 

}	// end of int CHYMapList::NumItems()

BOOL CHYMapList::IsNull()
{
	return empty();

}	// end of BOOL CHYMapList::IsNull()

BOOL CHYMapList::IsItemValid(int iFocusedItem)
{
	if( empty() ) return FALSE;

	return (iFocusedItem >= 0) && (iFocusedItem < NumItems()); 

}	// end of BOOL CHYMapList::IsItemValid()