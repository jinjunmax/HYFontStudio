#include "StdAfx.h"
#include "HYTableDirectory.h"

namespace HYFONTCODEC
{
	CHYTableEntry::CHYTableEntry(void)
	{
		tag					= 0;
		checkSum			= 0;	
		offset				= 0;
		length				= 0;

	}	// end of CHYTableEntry::CHYTableEntry()

	CHYTableEntry::~CHYTableEntry(void)
	{

	}	// end of CHYTableEntry::~CHYTableEntry()

	CHYTableDirectory::CHYTableDirectory(void)
	{
		version.fract	= 0;
		version.value	= 0;
		numTables		= 0;
		searchRange		= 0;
		entrySelector	= 0;
		rangeShift		= 0;

	}	// end of CHYTableEntry::CHYTableDirectory()

	CHYTableDirectory::~CHYTableDirectory()
	{
		vtTableEntry.clear();

	}	// end of CHYTableEntry::~CHYTableDirectory()

	void CHYTableDirectory::SetDefault()
	{
		version.fract	= 0;
		version.value	= 0;
		numTables		= 0;
		searchRange		= 0;
		entrySelector	= 0;
		rangeShift		= 0;
		vtTableEntry.clear();

	}	// end of void CHYTableDirectory::SetDefault()

	int CHYTableDirectory::FindTableEntry(unsigned long  tag)
	{
		for (size_t i=0; i<vtTableEntry.size(); i++)
		{
			if (vtTableEntry[i].tag == tag)	 
				return i;
		}

		return -1;

	}	// end of CHYTableEntry& CHYTableDirectory::FindTableEntry()
}

