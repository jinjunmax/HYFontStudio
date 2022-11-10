#pragma once

#include "HYFontCodecDef.h"
#include <string>
#include <vector>

namespace HYFONTCODEC
{
	class HY_DLL_CLASS CHYTableEntry
	{
	public:
		CHYTableEntry(void);
		~CHYTableEntry(void);

	public:	
		unsigned long			tag;
		unsigned long			checkSum;	
		unsigned long			offset;
		unsigned long			length;
	};

	class HY_DLL_CLASS CHYTableDirectory
	{
	public:
		CHYTableDirectory(void);
		~CHYTableDirectory(void);

		void		SetDefault();
		int 		FindTableEntry(unsigned long  tag);

	public:
		HYFIXED							version;
		unsigned short					numTables;
		unsigned short					searchRange;
		unsigned short					entrySelector;
		unsigned short					rangeShift;
		std::vector<CHYTableEntry>		vtTableEntry;
	};
};