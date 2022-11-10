#include "StdAfx.h"
#include "HYsbix.h"

namespace HYFONTCODEC
{
	CMaskData::CMaskData()
	{

	}	// end of CMaskData::CMaskData()

	CMaskData::~CMaskData()
	{

	}	// end of CMaskData::~CMaskData()

	void CMaskData::setdefault()
	{
		vtData.clear();

	}	// end of void CMaskData::setdefault()

	CStrikesMask::CStrikesMask()
	{

	}	// end of CStrikesMask::CStrikesMask()

	CStrikesMask::~CStrikesMask()
	{

	}	// end of CStrikesMask::~CStrikesMask()

	void CStrikesMask::setdefault()
	{
		vtMaskData.clear();

	}	// end of void CStrikesMask::setdefault()

	CStrikesDatarecord::CStrikesDatarecord()
	{
		gid = -1;
		originOffsetY = 0;
		originOffsetX = 0;
		memset(graphicType,0,4);

	}	// end of CStrikesDatarecord::CStrikesDatarecord()

	CStrikesDatarecord::~CStrikesDatarecord()
	{


	}	// end of CStrikesDatarecord::~CStrikesDatarecord()

	void CStrikesDatarecord::setdefault()
	{
		vtdata.clear();
		vtMask.clear();
		gid = -1;
		originOffsetY = 0;
		originOffsetX = 0;
		memset(graphicType,0,4);

	}	// end of void CStrikesDatarecord::setdefault()

	CSbixStrikes::CSbixStrikes(void)
	{
		ppem = 0;
		resolution = 0;

	}	// end of CSbixStrikes::CSbixStrikes()

	CSbixStrikes::~CSbixStrikes(void)
	{

	}	// end of CSbixStrikes::~CSbixStrikes()

	void CSbixStrikes::setdefault()
	{
		vtglyphDataOffset.clear();
		vtStrikesDatarecord.clear();

	}	// end of void CHYsbix::setdefault()

	CHYsbix::CHYsbix(void)
	{
		version=0;
		flags=0;
		numStrikes=0;

	}	// end of CHYsbix::CHYsbix()

	CHYsbix::~CHYsbix(void)
	{

	}	// end of CHYsbix::~CHYsbix()

	void CHYsbix::setdefault()
	{
		vtstrikeOffset.clear();
		vtSbixStrikes.clear();

	}	// end of void CHYsbix::setdefault()


}
