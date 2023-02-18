// HYFontCodec.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "HYFontCodec.h"
#include "HYStack.h"
#include <atltime.h>
#include <algorithm>

namespace HYFONTCODEC
{
	struct Point
	{
		Point()
			:	x (0.0), y (0.0)
		{}

		Point (double _x, double _y)
			:	x (_x), y (_y)
		{}

		double	x;
		double	y;
	};

	Point operator + (Point const &a, Point const &b)
	{
		return Point (a.x + b.x, a.y + b.y);
	}

	Point operator - (Point const &a, Point const &b)
	{
		return Point (a.x - b.x, a.y - b.y);
	}

	class CCharStringParam
	{
	public:
		CCharStringParam()
			:	lastOp (0xffff), hintCount (0), haveWidth (false), hstem (false), vstem (false), arguments (32),
			movePath (false), hstemPos (0xffffffff), injectPos (0xffffffff)
		{}

		double						transient[32];
		CHYStack<double>			arguments;
		//unsigned long				bufStart;
		unsigned char const *		bufStart;
		RECT						bounds;
		size_t						hstemPos;		// position of the hstem or hstemhm operator
		size_t						hstemArgs;		// count of hstem arguments
		size_t						injectPos;		// Position in the charstring of the first move or path operator
		unsigned					lastOp;
		unsigned					hintCount;
		bool						haveWidth;
		bool						hstem;
		bool						vstem;
		bool						movePath;
		Point						position;

		int TakeWidth(double &width, size_t paramCount)
		{
			size_t		result = 0;

			if (!haveWidth)
			{
				if (arguments.size() > paramCount)
				{
					width = arguments[0];
					++result;
				}

				haveWidth = true;
			}
			return result;		
		}

	};

	static CHYFontCodec* gpFontCodec;
	static unsigned __stdcall ThreadEntryPtForGlyf(void * pThis)  
	{  
		gpFontCodec=(CHYFontCodec*)pThis;   // the tricky cast  	
		gpFontCodec->Decodeglyf();

		return 1;                           // the thread exit code  

	}	// end of ThreadStaticEntryPoint()  

	static unsigned __stdcall ThreadEntryPtForCFF(void * pThis)  
	{
		gpFontCodec=(CHYFontCodec*)pThis;
		gpFontCodec->DecodeCFF();

		return 1;

	}	// end of static unsigned __stdcall ThreadEntryPtForCFF() 

	static unsigned __stdcall ThreadEntryPtForCode(void * pThis)  
	{
		gpFontCodec= (CHYFontCodec*)pThis;   // the tricky cast

		std::vector<CHYGlyph>& vtHYGlyph = gpFontCodec->m_vtHYGlyphs;
		size_t stGlyphNums = vtHYGlyph.size();
		CHYCmap& HYCmap = gpFontCodec->m_HYCmap;

		int iEntryIndex = HYCmap.FindSpecificEntryIndex(CMAP_ENCODE_FT_12);
		if (iEntryIndex!=-1)
		{ 
			CMAP_TABLE_ENTRY& TbEntry = HYCmap.vtCamp_tb_entry[iEntryIndex];
			CMAP_ENCODE_FORMAT_12& Format12 = TbEntry.Format12;
			unsigned long ulloop = Format12.nGroups;
			for (unsigned long i=0; i<ulloop; i++)
			{
				CMAP_ENCODE_FORMAT_12_GROUP&  tagF12Group = Format12.vtGroup[i];
				unsigned long GID = tagF12Group.startGlyphID;
				for (unsigned long l = tagF12Group.startCharCode; l<=tagF12Group.endCharCode; l++)
				{					
					if (GID<(unsigned long)vtHYGlyph.size())
					{
						CHYGlyph& glyph = vtHYGlyph[GID++];					
						glyph.vtUnicode.push_back(l);
					}									
				}				
			}
		}		
		else 
		{
			iEntryIndex = HYCmap.FindSpecificEntryIndex(3,1,CMAP_ENCODE_FT_4);
			if (iEntryIndex==-1) 
			{
				iEntryIndex = HYCmap.FindSpecificEntryIndex(0,3,CMAP_ENCODE_FT_4);
			}
			if (iEntryIndex!=-1)
			{
				CMAP_TABLE_ENTRY& TbEntry = HYCmap.vtCamp_tb_entry[iEntryIndex];
				CMAP_ENCODE_FORMAT_4& Format4 = TbEntry.Format4;
				unsigned long ulloop = Format4.segCountX2>>1;

				for (unsigned short i=0; i<ulloop;i++)
				{
					int iRang		= Format4.vtEndCount[i]-Format4.vtstartCount[i];
					if (Format4.vtidRangeOffset[i]!=0)
					{
						for (int x=0; x<=iRang; x++)
						{
							int iID = (Format4.vtidRangeOffset[i] - Format4.segCountX2)/2+i+x;
							iID = Format4.vtglyphIdArray[iID];
							CHYGlyph& glyph = vtHYGlyph[iID];							
							glyph.vtUnicode.push_back(Format4.vtstartCount[i]+x);
						}
					}
					else 
					{	
						for (int x=0; x<=iRang; x++)
						{
							int GID =(Format4.vtstartCount[i]+x+(Format4.vtidDelta[i]))%65536;
							if (GID!=65535)
							{
								CHYGlyph& glyph = vtHYGlyph[GID];
								glyph.vtUnicode.push_back(Format4.vtstartCount[i]+x);							
							}							
						}
					}
				}
			}
		}

		return 1;

	}	// end of static unsigned __stdcall ThreadEntryPtForCode()

	CHYFontCodec::CHYFontCodec(void)
	{
		m_pFontFile		= 0;
		m_iFontType		= -1;
		m_iStepNo		= 0;

		::memset(&m_tagOption, 0, sizeof(HY_OPTION_PRM));

		InitKangxi();
		InitVariantChar();
		InitCompatChar();
		InitJFChar();

	}	// end of CHYFontCodec::CHYFontCodec()

	CHYFontCodec::~CHYFontCodec(void)
	{	
		CloseFile();
		if(m_mpVariantChar.empty())
			m_mpVariantChar.clear();
		if (!m_mpCompatChar.empty())
			m_mpCompatChar.clear();

	}	// end of CHYFontCodec::~CHYFontCodec()

	void CHYFontCodec::InitKangxi()
	{
		m_mpKangxiChar.clear();
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F00, 0x4E00));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x4E00, 0x2F00));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F01, 0x4E28));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x4E28, 0x2F01));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F02, 0x4E36));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x4E36, 0x2F02));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F03, 0x4E3F));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x4E3F, 0x2F03));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F04, 0x4E59));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x4E59, 0x2F04));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F05, 0x4E85));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x4E85, 0x2F05));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F06, 0x4E8C));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x4E8C, 0x2F06));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F07, 0x4EA0));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x4EA0, 0x2F07));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F08, 0x4EBA));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x4EBA, 0x2F08));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F09, 0x513F));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x513F, 0x2F09));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F0A, 0x5165));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x5165, 0x2F0A));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F0B, 0x516B));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x516B, 0x2F0B));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F0C, 0x5182));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x5182, 0x2F0C));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F0D, 0x5196));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x5196, 0x2F0D));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F0E, 0x51AB));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x51AB, 0x2F0E));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F0F, 0x51E0));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x51E0, 0x2F0F));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F10, 0x51F5));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x51F5, 0x2F10));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F11, 0x5200));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x5200, 0x2F11));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F12, 0x529B));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x529B, 0x2F12));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F13, 0x52F9));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x52F9, 0x2F13));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F14, 0x5315));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x5315, 0x2F14));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F15, 0x531A));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x531A, 0x2F15));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F16, 0x5338));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x5338, 0x2F16));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F17, 0x5341));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x5341, 0x2F17));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F18, 0x535C));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x535C, 0x2F18));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F19, 0x5369));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x5369, 0x2F19));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F1A, 0x5382));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x5382, 0x2F1A));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F1B, 0x53B6));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x53B6, 0x2F1B));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F1C, 0x53C8));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x53C8, 0x2F1C));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F1D, 0x53E3));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x53E3, 0x2F1D));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F1E, 0x56D7));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x56D7, 0x2F1E));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F1F, 0x571F));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x571F, 0x2F1F));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F20, 0x58EB));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x58EB, 0x2F20));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F21, 0x5902));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x5902, 0x2F21));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F22, 0x590A));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x590A, 0x2F22));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F23, 0x5915));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x5915, 0x2F23));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F24, 0x5927));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x5927, 0x2F24));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F25, 0x5973));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x5973, 0x2F25));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F26, 0x5B50));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x5B50, 0x2F26));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F27, 0x5B80));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x5B80, 0x2F27));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F28, 0x5BF8));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x5BF8, 0x2F28));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F29, 0x5C0F));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x5C0F, 0x2F29));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F2A, 0x5C22));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x5C22, 0x2F2A));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F2B, 0x5C38));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x5C38, 0x2F2B));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F2C, 0x5C6E));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x5C6E, 0x2F2C));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F2D, 0x5C71));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x5C71, 0x2F2D));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F2E, 0x5DDB));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x5DDB, 0x2F2E));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F2F, 0x5DE5));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x5DE5, 0x2F2F));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F30, 0x5DF1));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x5DF1, 0x2F30));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F31, 0x5DFE));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x5DFE, 0x2F31));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F32, 0x5E72));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x5E72, 0x2F32));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F33, 0x5E7A));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x5E7A, 0x2F33));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F33, 0x2E93));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2E93, 0x2F33));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F34, 0x5E7F));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x5E7F, 0x2F34));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F35, 0x5EF4));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x5EF4, 0x2F35));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F36, 0x5EFE));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x5EFE, 0x2F36));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F37, 0x5F0B));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x5F0B, 0x2F37));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F38, 0x5F13));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x5F13, 0x2F38));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F38, 0x5F13));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x5F13, 0x2F38));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F39, 0x5F50));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x5F50, 0x2F39));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F3A, 0x5F61));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x5F61, 0x2F3A));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F3B, 0x5F73));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x5F73, 0x2F3B));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F3C, 0x5FC3));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x5FC3, 0x2F3C));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F3D, 0x6208));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x6208, 0x2F3D));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F3E, 0x6236));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x6236, 0x2F3E));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F3F, 0x624B));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x624B, 0x2F3F));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F40, 0x652F));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x652F, 0x2F40));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F41, 0x6534));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x6534, 0x2F41));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F42, 0x6587));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x6587, 0x2F42));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F43, 0x6597));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x6597, 0x2F43));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F44, 0x65A4));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x65A4, 0x2F44));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F45, 0x65B9));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x65B9, 0x2F45));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F46, 0x65E0));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x65E0, 0x2F46));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F47, 0x65E5));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x65E5, 0x2F47));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F48, 0x66F0));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x66F0, 0x2F48));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F49, 0x6708));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x6708, 0x2F49));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F4A, 0x6728));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x6728, 0x2F4A));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F4B, 0x6B20));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x6B20, 0x2F4B));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F4C, 0x6B62));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x6B62, 0x2F4C));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F4D, 0x6B79));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x6B79, 0x2F4D));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F4E, 0x6BB3));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x6BB3, 0x2F4E));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F4F, 0x6BCB));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x6BCB, 0x2F4F));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F50, 0x6BD4));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x6BD4, 0x2F50));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F51, 0x6BDB));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x6BDB, 0x2F51));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F52, 0x6C0F));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x6C0F, 0x2F52));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F53, 0x6C14));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x6C14, 0x2F53));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F54, 0x6C34));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x6C34, 0x2F54));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F55, 0x706B));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x706B, 0x2F55));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F56, 0x722A));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x722A, 0x2F56));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F57, 0x7236));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x7236, 0x2F57));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F58, 0x723B));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x723B, 0x2F58));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F59, 0x723F));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x723F, 0x2F59));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F5A, 0x7247));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x7247, 0x2F5A));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F5B, 0x7259));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x7259, 0x2F5B));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F5C, 0x725B));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x725B, 0x2F5C));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F5D, 0x72AC));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x72AC, 0x2F5D));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F5E, 0x7384));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x7384, 0x2F5E));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F5F, 0x7389));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x7389, 0x2F5F));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F60, 0x74DC));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x74DC, 0x2F60));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F61, 0x74E6));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x74E6, 0x2F61));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F62, 0x7518));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x7518, 0x2F62));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F63, 0x751F));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x751F, 0x2F63));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F64, 0x7528));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x7528, 0x2F64));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F65, 0x7530));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x7530, 0x2F65));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F66, 0x758B));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x758B, 0x2F66));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F67, 0x7592));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x7592, 0x2F67));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F68, 0x7676));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x7676, 0x2F68));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F69, 0x767D));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x767D, 0x2F69));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F6A, 0x76AE));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x76AE, 0x2F6A));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F6B, 0x76BF));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x76BF, 0x2F6B));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F6C, 0x76EE));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x76EE, 0x2F6C));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F6D, 0x77DB));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x77DB, 0x2F6D));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F6E, 0x77E2));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x77E2, 0x2F6E));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F6F, 0x77F3));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x77F3, 0x2F6F));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F70, 0x793A));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x793A, 0x2F70));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F71, 0x79B8));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x79B8, 0x2F71));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F72, 0x79BE));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x79BE, 0x2F72));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F73, 0x7A74));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x7A74, 0x2F73));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F74, 0x7ACB));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x7ACB, 0x2F74));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F75, 0x7AF9));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x7AF9, 0x2F75));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F76, 0x7C73));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x7C73, 0x2F76));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F77, 0x7CF8));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x7CF8, 0x2F77));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F78, 0x7F36));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x7F36, 0x2F78));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F79, 0x7F51));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x7F51, 0x2F79));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F7A, 0x7F8A));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x7F8A, 0x2F7A));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F7B, 0x7FBD));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x7FBD, 0x2F7B));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F7C, 0x8001));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x8001, 0x2F7C));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F7D, 0x800C));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x800C, 0x2F7D));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F7E, 0x8012));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x8012, 0x2F7E));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F7F, 0x8033));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x8033, 0x2F7F));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F80, 0x807F));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x807F, 0x2F80));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F81, 0x8089));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x8089, 0x2F81));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F82, 0x81E3));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x81E3, 0x2F82));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F83, 0x81EA));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x81EA, 0x2F83));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F84, 0x81F3));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x81F3, 0x2F84));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F85, 0x81FC));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x81FC, 0x2F85));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F86, 0x820C));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x820C, 0x2F86));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F87, 0x821B));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x821B, 0x2F87));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F88, 0x821F));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x821F, 0x2F88));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F89, 0x826E));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x826E, 0x2F89));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F8A, 0x8272));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x8272, 0x2F8A));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F8B, 0x8278));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x8278, 0x2F8B));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F8C, 0x864D));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x864D, 0x2F8C));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F8D, 0x866B));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x866B, 0x2F8D));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F8E, 0x8840));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x8840, 0x2F8E));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F8F, 0x884C));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x884C, 0x2F8F));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F90, 0x8863));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x8863, 0x2F90));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F91, 0x897E));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x897E, 0x2F91));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F92, 0x898B));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x898B, 0x2F92));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F93, 0x89D2));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x89D2, 0x2F93));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F94, 0x8A00));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x8A00, 0x2F94));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F95, 0x8C37));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x8C37, 0x2F95));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F96, 0x8C46));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x8C46, 0x2F96));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F97, 0x8C55));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x8C55, 0x2F97));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F98, 0x8C78));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x8C78, 0x2F98));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F99, 0x8C9D));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x8C9D, 0x2F99));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F9A, 0x8D64));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x8D64, 0x2F9A));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F9B, 0x8D70));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x8D70, 0x2F9B));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F9C, 0x8DB3));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x8DB3, 0x2F9C));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F9D, 0x8EAB));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x8EAB, 0x2F9D));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F9E, 0x8ECA));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x8ECA, 0x2F9E));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2F9F, 0x8F9B));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x8F9B, 0x2F9F));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2FA0, 0x8FB0));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x8FB0, 0x2FA0));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2FA1, 0x8FB5));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x8FB5, 0x2FA1));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2FA2, 0x9091));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x9091, 0x2FA2));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2FA3, 0x9149));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x9149, 0x2FA3));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2FA4, 0x91C6));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x91C6, 0x2FA4));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2FA5, 0x91CC));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x91CC, 0x2FA5));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2FA6, 0x91D1));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x91D1, 0x2FA6));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2FA7, 0x9577));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x9577, 0x2FA7));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2FA8, 0x9580));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x9580, 0x2FA8));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2FA9, 0x961C));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x961C, 0x2FA9));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2FAA, 0x96B6));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x96B6, 0x2FAA));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2FAB, 0x96B9));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x96B9, 0x2FAB));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2FAC, 0x96E8));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x96E8, 0x2FAC));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2FAD, 0x9751));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x9751, 0x2FAD));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2FAE, 0x975E));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x975E, 0x2FAE));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2FAF, 0x9762));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x9762, 0x2FAF));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2FB0, 0x9769));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x9769, 0x2FB0));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2FB1, 0x97CB));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x97CB, 0x2FB1));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2FB2, 0x97ED));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x97ED, 0x2FB2));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2FB3, 0x97F3));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x97F3, 0x2FB3));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2FB4, 0x9801));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x9801, 0x2FB4));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2FB5, 0x98A8));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x98A8, 0x2FB5));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2FB6, 0x98DB));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x98DB, 0x2FB6));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2FB7, 0x98DF));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x98DF, 0x2FB7));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2FB8, 0x9996));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x9996, 0x2FB8));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2FB9, 0x9999));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x9999, 0x2FB9));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2FBA, 0x99AC));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x99AC, 0x2FBA));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2FBB, 0x9AA8));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x9AA8, 0x2FBB));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2FBC, 0x9AD8));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x9AD8, 0x2FBC));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2FBD, 0x9ADF));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x9ADF, 0x2FBD));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2FBE, 0x9B25));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x9B25, 0x2FBE));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2FBF, 0x9B2F));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x9B2F, 0x2FBF));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2FC0, 0x9B32));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x9B32, 0x2FC0));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2FC1, 0x9B3C));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x9B3C, 0x2FC1));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2FC2, 0x9B5A));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x9B5A, 0x2FC2));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2FC3, 0x9CE5));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x9CE5, 0x2FC3));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2FC4, 0x9E75));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x9E75, 0x2FC4));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2FC5, 0x9E7F));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x9E7F, 0x2FC5));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2FC6, 0x9EA5));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x9EA5, 0x2FC6));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2FC7, 0x9EBB));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x9EBB, 0x2FC7));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2FC8, 0x9EC3));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x9EC3, 0x2FC8));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2FC9, 0x9ECD));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x9ECD, 0x2FC9));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2FCA, 0x9ED1));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x9ED1, 0x2FCA));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2FCB, 0x9EF9));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x9EF9, 0x2FCB));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2FCC, 0x9EFD));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x9EFD, 0x2FCC));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2FCD, 0x9F0E));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x9F0E, 0x2FCD));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2FCE, 0x9F13));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x9F13, 0x2FCE));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2FCF, 0x9F20));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x9F20, 0x2FCF));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2FD0, 0x9F3B));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x9F3B, 0x2FD0));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2FD1, 0x9F4A));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x9F4A, 0x2FD1));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2FD2, 0x9F52));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x9F52, 0x2FD2));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2FD3, 0x9F8D));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x9F8D, 0x2FD3));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2FD4, 0x9F9C));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x9F9C, 0x2FD4));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x2FD5, 0x9FA0));
		m_mpKangxiChar.insert(pair<unsigned long, unsigned long>(0x9FA0, 0x2FD5));
	
	}	//end of void CHYFontCodec::InitKangxi()
		
	void CHYFontCodec::InitVariantChar()
	{
		m_mpVariantChar.clear();		
		
		// CJK Compatibility Ideographs Range: F900 - FAFF

		//F900 - F921
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF900,0x8C48));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x8C48,0xF900));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF901,0x66F4));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x66F4,0xF901));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF902,0x8ECA));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x8ECA,0xF902));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF903,0x8CC8));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x8CC8,0xF903));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF904,0x6ED1));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6ED1,0xF904));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF905,0x4E32));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x4E32,0xF905));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF906,0x53E5));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x53E5,0xF906));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF907,0x9F9C));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x9F9C,0xF907));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF908,0x9F9C));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x9F9C,0xF908));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF909,0x5951));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x5951,0xF909));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF90A,0x91D1));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x91D1,0xF90A));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF90B,0x5587));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x5587,0xF90B));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF90C,0x5948));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x5948,0xF90C));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF90D,0x61F6));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x61F6,0xF90D));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF90E,0x7669));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7669,0xF90E));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF90F,0x7F85));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7F85,0xF90F));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF910,0x863F));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x863F,0xF910));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF911,0x87BA));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x87BA,0xF911));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF912,0x88F8));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x88F8,0xF912));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF913,0x908F));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x908F,0xF913));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF914,0x6A02));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6A02,0xF914));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF915,0x6D1B));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6D1B,0xF915));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF916,0x70D9));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x70D9,0xF916));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF917,0x73DE));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x73DE,0xF917));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF918,0x843D));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x843D,0xF918));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF919,0x916A));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x916A,0xF919));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF91A,0x99F1));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x99F1,0xF91A));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF91B,0x4E82));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x4E82,0xF91B));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF91C,0x5375));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x5375,0xF91C));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF91D,0x6B04));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6B04,0xF91D));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF91E,0x721B));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x721B,0xF91E));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF91F,0x862D));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x862D,0xF91F));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF920,0x9E1E));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x9E1E,0xF920));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF921,0x5D50));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x5D50,0xF921));

		//F922-F945
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF922,0x6FEB));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6FEB,0xF922));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF923,0x85CD));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x85CD,0xF923));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF924,0x8964));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x8964,0xF924));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF925,0x62C9));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x62C9,0xF925));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF926,0x81D8));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x81D8,0xF926));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF927,0x881F));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x881F,0xF927));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF928,0x5ECA));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x5ECA,0xF928));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF929,0x6717));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6717,0xF929));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF92A,0x6D6A));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6D6A,0xF92A));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF92B,0x72FC));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x72FC,0xF92B));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF92C,0x90CE));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x90CE,0xF92C));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF92D,0x4F86));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x4F86,0xF92D));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF92E,0x51B7));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x51B7,0xF92E));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF92F,0x52DE));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x52DE,0xF92F));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF930,0x64C4));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x64C4,0xF930));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF931,0x6AD3));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6AD3,0xF931));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF932,0x7210));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7210,0xF932));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF933,0x76E7));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x76E7,0xF933));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF934,0x8001));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x8001,0xF934));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF935,0x8606));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x8606,0xF935));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF936,0x865C));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x865C,0xF936));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF937,0x8DEF));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x8DEF,0xF937));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF938,0x9732));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x9732,0xF938));		
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF939,0x9B6F));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x9B6F,0xF939));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF93A,0x9DFA));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x9DFA,0xF93A));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF93B,0x788C));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x788C,0xF93B));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF93C,0x797F));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x797F,0xF93C));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF93D,0x7DA0));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7DA0,0xF93D));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF93E,0x83C9));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x83C9,0xF93E));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF93F,0x9304));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x9304,0xF93F));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF940,0x9E7F));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x9E7F,0xF940));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF941,0x8AD6));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x8AD6,0xF941));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF942,0x58DF));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x58DF,0xF942));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF943,0x5F04));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x5F04,0xF943));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF944,0x7C60));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7C60,0xF944));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF945,0x807E));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x807E,0xF945));
		
		//F946-F969
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF946,0x7262));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7262,0xF946));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF947,0x78CA));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x78CA,0xF947));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF948,0x8CC2));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x8CC2,0xF948));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF949,0x96F7));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x96F7,0xF949));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF94A,0x58D8));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x58D8,0xF94A));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF94B,0x5C62));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x5C62,0xF94B));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF94C,0x6A13));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6A13,0xF94C));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF94D,0x6DDA));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6DDA,0xF94D));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF94E,0x6F0F));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6F0F,0xF94E));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF94F,0x7D2F));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7D2F,0xF94F));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF950,0x7E37));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7E37,0xF950));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF951,0x964B));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x964B,0xF951));			
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF952,0x52D2));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x52D2,0xF952));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF953,0x808B));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x808B,0xF953));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF954,0x51DC));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x51DC,0xF954));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF955,0x51CC));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x51CC,0xF955));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF956,0x7A1C));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7A1C,0xF956));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF957,0x7DBE));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7DBE,0xF957));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF958,0x83F1));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x83F1,0xF958));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF959,0x9675));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x9675,0xF959));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF95A,0x8B80));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x8B80,0xF95A));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF95B,0x62CF));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x62CF,0xF95B));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF95C,0x6A02));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6A02,0xF95C));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF95D,0x8AFE));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x8AFE,0xF95D));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF95E,0x4E39));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x4E39,0xF95E));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF95F,0x5BE7));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x5BE7,0xF95F));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF960,0x6012));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6012,0xF960));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF961,0x7387));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7387,0xF961));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF962,0x7570));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7570,0xF962));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF963,0x5317));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x5317,0xF963));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF964,0x78FB));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x78FB,0xF964));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF965,0x4FBF));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x4FBF,0xF965));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF966,0x5FA9));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x5FA9,0xF966));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF967,0x4E0D));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x4E0D,0xF967));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF968,0x6CCC));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6CCC,0xF968));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF969,0x6578));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6578,0xF969));

		//F96A - F98D
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF96A,0x7D22));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7D22,0xF96A));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF96B,0x53C3));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x53C3,0xF96B));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF96C,0x585E));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x585E,0xF96C));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF96D,0x7701));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7701,0xF96D));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF96E,0x8449));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x8449,0xF96E));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF96F,0x8AAA));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x8AAA,0xF96F));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF970,0x6BBA));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6BBA,0xF970));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF971,0x8FB0));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x8FB0,0xF971));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF972,0x6C88));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6C88,0xF972));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF973,0x62FE));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x62FE,0xF973));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF974,0x82E5));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x82E5,0xF974));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF975,0x63A0));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x63A0,0xF975));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF976,0x7565));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7565,0xF976));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF977,0x4EAE));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x4EAE,0xF977));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF978,0x5169));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x5169,0xF978));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF979,0x51C9));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x51C9,0xF979));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF97A,0x6881));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6881,0xF97A));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF97B,0x7CE7));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7CE7,0xF97B));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF97C,0x826F));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x826F,0xF97C));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF97D,0x8AD2));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x8AD2,0xF97D));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF97E,0x91CF));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x91CF,0xF97E));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF97F,0x52F5));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x52F5,0xF97F));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF980,0x5442));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x5442,0xF980));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF981,0x5973));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x5973,0xF981));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF982,0x5EEC));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x5EEC,0xF982));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF983,0x65C5));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x65C5,0xF983));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF984,0x6FFE));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6FFE,0xF984));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF985,0x792A));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x792A,0xF985));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF986,0x95AD));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x95AD,0xF986));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF987,0x9A6A));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x9A6A,0xF987));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF988,0x9E97));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x9E97,0xF988));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF989,0x9ECE));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x9ECE,0xF989));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF98A,0x529B));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x529B,0xF98A));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF98B,0x66C6));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x66C6,0xF98B));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF98C,0x6B77));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6B77,0xF98C));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF98D,0x8F62));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x8F62,0xF98D));

		//F98E-F9B1
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF98E,0x5E74));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x5E74,0xF98E));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF98F,0x6190));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6190,0xF98F));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF990,0x6200));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6200,0xF990));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF991,0x649A));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x649A,0xF991));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF992,0x6F23));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6F23,0xF992));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF993,0x7149));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7149,0xF993));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF994,0x7489));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7489,0xF994));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF995,0x79CA));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x79CA,0xF995));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF996,0x7DF4));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7DF4,0xF996));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF997,0x806F));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x806F,0xF997));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF998,0x8F26));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x8F26,0xF998));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF999,0x84EE));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x84EE,0xF999));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF99A,0x9023));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x9023,0xF99A));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF99B,0x934A));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x934A,0xF99B));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF99C,0x5217));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x5217,0xF99C));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF99D,0x52A3));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x52A3,0xF99D));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF99E,0x54BD));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x54BD,0xF99E));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF99F,0x70C8));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x70C8,0xF99F));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9A0,0x88C2));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x88C2,0xF9A0));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9A1,0x8AAA));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x8AAA,0xF9A1));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9A2,0x5EC9));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x5EC9,0xF9A2));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9A3,0x5FF5));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x5FF5,0xF9A3));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9A4,0x637B));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x637B,0xF9A4));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9A5,0x6BAE));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6BAE,0xF9A5));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9A6,0x7C3E));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7C3E,0xF9A6));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9A7,0x7375));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7375,0xF9A7));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9A8,0x4EE4));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x4EE4,0xF9A8));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9A9,0x56F9));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x56F9,0xF9A9));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9AA,0x5BE7));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x5BE7,0xF9AA));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9AB,0x5DBA));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x5DBA,0xF9AB));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9AC,0x601C));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x601C,0xF9AC));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9AD,0x73B2));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x73B2,0xF9AD));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9AE,0x7469));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7469,0xF9AE));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9AF,0x7F9A));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7F9A,0xF9AF));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9B0,0x8046));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x8046,0xF9B0));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9B1,0x9234));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x9234,0xF9B1));
		
		//F9B2 - F9D5
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9B2,0x96F6));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x96F6,0xF9B2));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9B3,0x9748));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x9748,0xF9B3));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9B4,0x9818));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x9818,0xF9B4));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9B5,0x4F8B));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x4F8B,0xF9B5));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9B6,0x79AE));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x79AE,0xF9B6));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9B7,0x91B4));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x91B4,0xF9B7));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9B8,0x96B7));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x96B7,0xF9B8));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9B9,0x60E1));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x60E1,0xF9B9));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9BA,0x4E86));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x4E86,0xF9BA));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9BB,0x50DA));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x50DA,0xF9BB));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9BC,0x5BEE));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x5BEE,0xF9BC));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9BD,0x5C3F));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x5C3F,0xF9BD));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9BE,0x6599));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6599,0xF9BE));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9BF,0x6A02));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6A02,0xF9BF));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9C0,0x71CE));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x71CE,0xF9C0));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9C1,0x7642));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7642,0xF9C1));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9C2,0x84FC));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x84FC,0xF9C2));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9C3,0x907C));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x907C,0xF9C3));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9C4,0x9F8D));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x9F8D,0xF9C4));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9C5,0x6688));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6688,0xF9C5));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9C6,0x962E));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x962E,0xF9C6));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9C7,0x5289));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x5289,0xF9C7));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9C8,0x677B));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x677B,0xF9C8));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9C9,0x67F3));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x67F3,0xF9C9));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9CA,0x6D41));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6D41,0xF9CA));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9CB,0x6E9C));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6E9C,0xF9CB));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9CC,0x7409));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7409,0xF9CC));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9CD,0x7559));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7559,0xF9CD));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9CE,0x786B));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x786B,0xF9CE));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9CF,0x7D10));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7D10,0xF9CF));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9D0,0x985E));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x985E,0xF9D0));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9D1,0x516D));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x516D,0xF9D1));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9D2,0x622E));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x622E,0xF9D2));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9D3,0x9678));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x9678,0xF9D3));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9D4,0x502B));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x502B,0xF9D4));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9D5,0x5D19));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x5D19,0xF9D5));

		//F9D6 - F9F9
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9D6,0x6DEA));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6DEA,0xF9D6));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9D7,0x8F2A));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x8F2A,0xF9D7));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9D8,0x5F8B));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x5F8B,0xF9D8));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9D9,0x6144));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6144,0xF9D9));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9DA,0x6817));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6817,0xF9DA));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9DB,0x7387));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7387,0xF9DB));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9DC,0x9686));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x9686,0xF9DC));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9DD,0x5229));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x5229,0xF9DD));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9DE,0x540F));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x540F,0xF9DE));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9DF,0x5C65));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x5C65,0xF9DF));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9E0,0x6613));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6613,0xF9E0));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9E1,0x674E));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x674E,0xF9E1));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9E2,0x68A8));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x68A8,0xF9E2));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9E3,0x6CE5));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6CE5,0xF9E3));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9E4,0x7406));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7406,0xF9E4));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9E5,0x75E2));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x75E2,0xF9E5));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9E6,0x7F79));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7F79,0xF9E6));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9E7,0x88CF));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x88CF,0xF9E7));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9E8,0x88E1));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x88E1,0xF9E8));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9E9,0x91CC));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x91CC,0xF9E9));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9EA,0x96E2));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x96E2,0xF9EA));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9EB,0x533F));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x533F,0xF9EB));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9EC,0x6EBA));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6EBA,0xF9EC));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9ED,0x541D));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x541D,0xF9ED));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9EE,0x71D0));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x71D0,0xF9EE));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9EF,0x7498));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7498,0xF9EF));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9F0,0x85FA));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x85FA,0xF9F0));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9F1,0x96A3));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x96A3,0xF9F1));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9F2,0x9C57));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x9C57,0xF9F2));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9F3,0x9E9F));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x9E9F,0xF9F3));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9F4,0x6797));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6797,0xF9F4));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9F5,0x6DCB));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6DCB,0xF9F5));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9F6,0x81E8));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x81E8,0xF9F6));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9F7,0x7ACB));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7ACB,0xF9F7));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9F8,0x7B20));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7B20,0xF9F8));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9F9,0x7C92));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7C92,0xF9F9));

		//F9FA - FA1D
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9FA,0x72C0));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x72C0,0xF9FA));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9FB,0x7099));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7099,0xF9FB));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9FC,0x8B58));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x8B58,0xF9FC));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9FD,0x4EC0));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x4EC0,0xF9FD));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9FE,0x8336));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x8336,0xF9FE));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xF9FF,0x523A));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x523A,0xF9FF));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA00,0x5207));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x5207,0xFA00));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA01,0x5EA6));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x5EA6,0xFA01));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA02,0x62D3));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x62D3,0xFA02));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA03,0x7CD6));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7CD6,0xFA03));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA04,0x5B85));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x5B85,0xFA04));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA05,0x6D1E));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6D1E,0xFA05));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA06,0x66B4));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x66B4,0xFA06));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA07,0x8F3B));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x8F3B,0xFA07));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA08,0x884C));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x884C,0xFA08));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA09,0x964D));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x964D,0xFA09));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA0A,0x898B));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x898B,0xFA0A));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA0B,0x5ED3));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x5ED3,0xFA0B));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA0C,0x5140));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x5140,0xFA0C));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA0D,0x55C0));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x55C0,0xFA0D));			
		// 0xFA0E - 0xFA0F CJK unified ideograph
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA10,0x585A));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x585A,0xFA10));
		// 0xFA11 CJK unified ideograph
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA12,0x6674));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6674,0xFA12));
		// 0xFA13 - 0xFA14 CJK unified ideograph
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA15,0x51DE));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x51DE,0xFA15));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA16,0x732A));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x732A,0xFA16));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA17,0x76CA));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x76CA,0xFA17));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA18,0x793C));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x793C,0xFA18));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA19,0x795E));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x795E,0xFA19));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA1A,0x7965));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7965,0xFA1A));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA1B,0x798F));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x798F,0xFA1B));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA1C,0x9756));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x9756,0xFA1C));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA1D,0x7CBE));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7CBE,0xFA1D));

		//FA1E - FA43
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA1E,0x7FBD));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7FBD,0xFA1E));
		// 0xFA1F CJK unified ideograph
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA20,0x8612));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x8612,0xFA20));
		// 0xFA21 CJK unified ideograph
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA22,0x8AF8));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x8AF8,0xFA22));
		// 0xFA23 -0xFA24 CJK unified ideograph
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA25,0x9038));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x9038,0xFA25));		
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA26,0x90FD));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x90FD,0xFA26));
		// 0xFA27 - 0xFA29 CJK unified ideograph
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA2A,0x98EF));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x98EF,0xFA2A));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA2B,0x98FC));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x98FC,0xFA2B));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA2C,0x9928));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x9928,0xFA2C));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA2D,0x9DB4));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x9DB4,0xFA2D));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA2E,0x90DE));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x90DE,0xFA2E));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA2F,0x96B7));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x96B7,0xFA2F));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA30,0x4FAE));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x4FAE,0xFA30));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA31,0x50E7));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x50E7,0xFA31));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA32,0x514D));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x514D,0xFA32));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA33,0x52C9));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x52C9,0xFA33));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA34,0x52E4));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x52E4,0xFA34));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA35,0x5351));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x5351,0xFA35));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA36,0x559D));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x559D,0xFA36));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA37,0x5606));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x5606,0xFA37));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA38,0x5668));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x5668,0xFA38));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA39,0x5840));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x5840,0xFA39));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA3A,0x58A8));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x58A8,0xFA3A));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA3B,0x5C64));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x5C64,0xFA3B));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA3C,0x5C6E));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x5C6E,0xFA3C));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA3D,0x6094));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6094,0xFA3D));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA3E,0x6168));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6168,0xFA3E));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA3F,0x618E));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x618E,0xFA3F));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA40,0x61F2));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x61F2,0xFA40));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA41,0x654F));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x654F,0xFA41));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA42,0x65E2));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x65E2,0xFA42));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA43,0x6691));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6691,0xFA43));

		//FA44 - FA67
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA44,0x6885));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6885,0xFA44));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA45,0x6D77));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6D77,0xFA45));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA46,0x6E1A));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6E1A,0xFA46));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA47,0x6F22));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6F22,0xFA47));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA48,0x716E));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x716E,0xFA48));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA49,0x722B));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x722B,0xFA49));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA4A,0x7422));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7422,0xFA4A));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA4B,0x7891));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7891,0xFA4B));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA4C,0x793E));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x793E,0xFA4C));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA4D,0x7949));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7949,0xFA4D));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA4E,0x7948));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7948,0xFA4E));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA50,0x7956));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7956,0xFA50));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA51,0x795D));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x795D,0xFA51));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA52,0x798D));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x798D,0xFA52));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA53,0x798E));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x798E,0xFA53));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA54,0x7A40));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7A40,0xFA54));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA55,0x7A81));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7A81,0xFA55));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA56,0x7BC0));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7BC0,0xFA56));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA57,0x7DF4));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7DF4,0xFA57));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA58,0x7E09));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7E09,0xFA58));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA59,0x7E41));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7E41,0xFA59));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA5A,0x7F72));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7F72,0xFA5A));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA5B,0x8005));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x8005,0xFA5B));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA5C,0x81ED));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x81ED,0xFA5C));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA5D,0x8279));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x8279,0xFA5D));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA5E,0x8279));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x8279,0xFA5E));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA5F,0x8457));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x8457,0xFA5F));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA60,0x8910));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x8910,0xFA60));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA61,0x8996));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x8996,0xFA61));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA62,0x8B01));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x8B01,0xFA62));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA63,0x8B39));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x8B39,0xFA63));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA64,0x8CD3));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x8CD3,0xFA64));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA65,0x8D08));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x8D08,0xFA65));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA66,0x8FB6));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x8FB6,0xFA66));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA67,0x9038));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x9038,0xFA67));

		//FA68 - FA8D
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA68,0x96E3));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x96E3,0xFA68));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA69,0x97FF));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x97FF,0xFA69));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA6A,0x983B));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x983B,0xFA6A));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA6B,0x6075));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6075,0xFA6B));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA6C,0x242EE));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x242EE,0xFA6C));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA6D,0x8218));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x8218,0xFA6D));			
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA70,0x4E26));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x4E26,0xFA70));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA71,0x51B5));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x51B5,0xFA71));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA72,0x5168));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x5168,0xFA72));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA73,0x4F80));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x4F80,0xFA73));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA74,0x5145));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x5145,0xFA74));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA75,0x5180));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x5180,0xFA75));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA76,0x52C7));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x52C7,0xFA76));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA77,0x52FA));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x52FA,0xFA77));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA78,0x559D));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x559D,0xFA78));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA79,0x5555));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x5555,0xFA79));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA7A,0x5599));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x5599,0xFA7A));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA7B,0x55E2));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x55E2,0xFA7B));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA7C,0x585A));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x585A,0xFA7C));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA7D,0x58B3));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x58B3,0xFA7D));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA7E,0x5944));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x5944,0xFA7E));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA7F,0x5954));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x5954,0xFA7F));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA80,0x5A62));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x5A62,0xFA80));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA81,0x5B28));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x5B28,0xFA81));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA82,0x5ED2));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x5ED2,0xFA82));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA83,0x5ED9));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x5ED9,0xFA83));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA84,0x5F69));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x5F69,0xFA84));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA85,0x5FAD));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x5FAD,0xFA85));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA86,0x60D8));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x60D8,0xFA86));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA87,0x614E));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x614E,0xFA87));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA88,0x6108));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6108,0xFA88));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA89,0x618E));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x618E,0xFA89));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA8A,0x6160));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6160,0xFA8A));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA8B,0x61F2));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x61F2,0xFA8B));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA8C,0x6234));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6234,0xFA8C));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA8D,0x63C4));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x63C4,0xFA8D));

		//FA8E -FAB1
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA8E,0x641C));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x641C,0xFA8E));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA8F,0x6452));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6452,0xFA8F));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA90,0x6556));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6556,0xFA90));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA91,0x6674));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6674,0xFA91));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA92,0x6717));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6717,0xFA92));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA93,0x671B));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x671B,0xFA93));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA94,0x6756));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6756,0xFA94));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA95,0x6B79));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6B79,0xFA95));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA96,0x6BBA));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6BBA,0xFA96));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA97,0x6D41));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6D41,0xFA97));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA98,0x6EDB));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6EDB,0xFA98));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA99,0x6ECB));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6ECB,0xFA99));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA9A,0x6F22));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x6F22,0xFA9A));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA9B,0x701E));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x701E,0xFA9B));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA9C,0x716E));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x716E,0xFA9C));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA9D,0x77A7));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x77A7,0xFA9D));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA9E,0x7235));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7235,0xFA9E));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFA9F,0x72AF));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x72AF,0xFA9F));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFAA0,0x732A));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x732A,0xFAA0));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFAA1,0x7471));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7471,0xFAA1));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFAA2,0x7506));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7506,0xFAA2));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFAA3,0x753B));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x753B,0xFAA3));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFAA4,0x761D));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x761D,0xFAA4));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFAA5,0x761F));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x761F,0xFAA5));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFAA6,0x76CA));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x76CA,0xFAA6));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFAA7,0x76DB));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x76DB,0xFAA7));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFAA8,0x76F4));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x76F4,0xFAA8));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFAA9,0x774A));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x774A,0xFAA9));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFAAA,0x7740));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7740,0xFAAA));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFAAB,0x78CC));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x78CC,0xFAAB));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFAAC,0x7AB1));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7AB1,0xFAAC));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFAAD,0x7BC0));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7BC0,0xFAAD));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFAAE,0x7C7B));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7C7B,0xFAAE));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFAAF,0x7D5B));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7D5B,0xFAAF));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFAB0,0x7DF4));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7DF4,0xFAB0));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFAB1,0x7F3E));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x7F3E,0xFAB1));

		//FAB2 - FAD4
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFAB2,0x8005));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x8005,0xFAB2));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFAB3,0x8352));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x8352,0xFAB3));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFAB4,0x83EF));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x83EF,0xFAB4));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFAB5,0x8779));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x8779,0xFAB5));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFAB6,0x8941));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x8941,0xFAB6));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFAB7,0x8986));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x8986,0xFAB7));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFAB8,0x8996));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x8996,0xFAB8));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFAB9,0x8ABF));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x8ABF,0xFAB9));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFABA,0x8AF8));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x8AF8,0xFABA));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFABB,0x8ACB));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x8ACB,0xFABB));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFABC,0x8B01));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x8B01,0xFABC));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFABE,0x8AED));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x8AED,0xFABE));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFABF,0x8B39));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x8B39,0xFABF));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFAC0,0x8B8A));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x8B8A,0xFAC0));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFAC1,0x8D08));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x8D08,0xFAC1));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFAC2,0x8F38));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x8F38,0xFAC2));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFAC3,0x9072));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x9072,0xFAC3));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFAC4,0x9199));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x9199,0xFAC4));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFAC5,0x9276));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x9276,0xFAC5));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFAC6,0x967C));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x967C,0xFAC6));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFAC7,0x96E3));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x96E3,0xFAC7));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFAC8,0x9756));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x9756,0xFAC8));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFAC9,0x97DB));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x97DB,0xFAC9));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFACA,0x97FF));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x97FF,0xFACA));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFACB,0x980B));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x980B,0xFACB));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFACC,0x983B));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x983B,0xFACC));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFACD,0x9B12));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x9B12,0xFACD));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFACE,0x9F9C));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x9F9C,0xFACE));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFACF,0x2284A));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x2284A,0xFACF));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFAD0,0x22844));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x22844,0xFAD0));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFAD1,0x233D5));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x233D5,0xFAD1));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFAD2,0x3B9D));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x3B9D,0xFAD2));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFAD3,0x4018));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x4018,0xFAD3));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFAD4,0x4039));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x4039,0xFAD4));

		//FAD5 - FAD9
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFAD5,0x25249));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x25249,0xFAD5));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFAD6,0x25CD0));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x25CD0,0xFAD6));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFAD7,0x27ED3));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x27ED3,0xFAD7));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFAD8,0x9F43));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x9F43,0xFAD8));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0xFAD9,0x9F8E));
		m_mpVariantChar.insert(pair<unsigned long,unsigned long>(0x9F8E,0xFAD9));

	}	// end of void CHYFontCodec::InitVariantChar()
	
	void CHYFontCodec::InitCompatChar()
	{
		m_mpCompatChar.clear();
		m_mpCompatChar.insert(pair<unsigned long, unsigned long>(0xE816, 0x20087));
		m_mpCompatChar.insert(pair<unsigned long, unsigned long>(0x20087, 0xE816));

		m_mpCompatChar.insert(pair<unsigned long, unsigned long>(0xE817, 0x20089));
		m_mpCompatChar.insert(pair<unsigned long, unsigned long>(0x20089, 0xE817));

		m_mpCompatChar.insert(pair<unsigned long, unsigned long>(0xE818, 0x200CC));
		m_mpCompatChar.insert(pair<unsigned long, unsigned long>(0x200CC, 0xE818));

		m_mpCompatChar.insert(pair<unsigned long, unsigned long>(0xE831, 0x215D7));
		m_mpCompatChar.insert(pair<unsigned long, unsigned long>(0x215D7, 0xE831));

		m_mpCompatChar.insert(pair<unsigned long, unsigned long>(0xE83B, 0x2298F));
		m_mpCompatChar.insert(pair<unsigned long, unsigned long>(0x2298F, 0xE83B));

		m_mpCompatChar.insert(pair<unsigned long, unsigned long>(0xE855, 0x241FE));
		m_mpCompatChar.insert(pair<unsigned long, unsigned long>(0x241FE, 0xE855));

		m_mpCompatChar.insert(pair<unsigned long, unsigned long>(0xE81E, 0x9FB4));
		m_mpCompatChar.insert(pair<unsigned long, unsigned long>(0x9FB4, 0xE81E));

		m_mpCompatChar.insert(pair<unsigned long, unsigned long>(0xE826, 0x9FB5));
		m_mpCompatChar.insert(pair<unsigned long, unsigned long>(0x9FB5, 0xE826));

		m_mpCompatChar.insert(pair<unsigned long, unsigned long>(0xE82B, 0x9FB6));
		m_mpCompatChar.insert(pair<unsigned long, unsigned long>(0x9FB6, 0xE82B));

		m_mpCompatChar.insert(pair<unsigned long, unsigned long>(0xE82C, 0x9FB7));
		m_mpCompatChar.insert(pair<unsigned long, unsigned long>(0x9FB7, 0xE82C));

		m_mpCompatChar.insert(pair<unsigned long, unsigned long>(0xE832, 0x9FB8));
		m_mpCompatChar.insert(pair<unsigned long, unsigned long>(0x9FB8, 0xE832));	

		m_mpCompatChar.insert(pair<unsigned long, unsigned long>(0xE843, 0x9FB9));
		m_mpCompatChar.insert(pair<unsigned long, unsigned long>(0x9FB9, 0xE843));

		m_mpCompatChar.insert(pair<unsigned long, unsigned long>(0xE854, 0x9FBA));
		m_mpCompatChar.insert(pair<unsigned long, unsigned long>(0x9FBA, 0xE854));

		m_mpCompatChar.insert(pair<unsigned long, unsigned long>(0xE864, 0x9FBB));
		m_mpCompatChar.insert(pair<unsigned long, unsigned long>(0x9FBB, 0xE864));

	}	// end of void CHYFontCodec::InitCompatChar()

	void CHYFontCodec::InitJFChar()
	{
		m_mpJ2FChar.clear();
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x5965, 0x5967));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x5265, 0x525D));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x522B, 0x5225));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x5E76, 0x4E26));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x518C, 0x518A));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x6491, 0x6490));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x75F4, 0x7661));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x803B, 0x6065));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x6A71, 0x6AE5));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x53A8, 0x5EDA));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x8471, 0x8525));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x56F1, 0x56EA));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x51D1, 0x6E4A));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x76D7, 0x76DC));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x53E0, 0x758A));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x4E22, 0x4E1F));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x5151, 0x514C));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x73D0, 0x743A));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x79C6, 0x7A08));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x5BAB, 0x5BAE));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x591F, 0x5920));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x6302, 0x639B));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x6EDA, 0x6EFE));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x6A2A, 0x6A6B));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x6237, 0x6236));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x6362, 0x63DB));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x5524, 0x559A));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x75EA, 0x7613));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x7115, 0x7165));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x6DA3, 0x6E19));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x9EC4, 0x9EC3));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x6BC1, 0x6BC0));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x8FF9, 0x8DE1));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x51CF, 0x6E1B));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x811A, 0x8173));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x6770, 0x5091));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x5C4A, 0x5C46));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x664B, 0x6649));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x8346, 0x834A));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x9759, 0x975C));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x51C0, 0x6DE8));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x53A9, 0x5EC4));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x51B3, 0x6C7A));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x51B5, 0x6CC1));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x6CEA, 0x6DDA));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x51C9, 0x6DBC));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x51DB, 0x51DC));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x7984, 0x797F));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x5415, 0x5442));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x4FA3, 0x4FB6));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x8109, 0x8108));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x732B, 0x8C93));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x4E48, 0x9EBC));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x6CA1, 0x6C92));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x5E42, 0x51AA));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x5185, 0x5167));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x637B, 0x649A));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x629B, 0x62CB));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x6816, 0x68F2));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x51C4, 0x6DD2));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x5F03, 0x68C4));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x6266, 0x6261));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x6F5C, 0x6F5B));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x5F3A, 0x5F37));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x5374, 0x537B));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x5220, 0x522A));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x7A0E, 0x7A05));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x7B0B, 0x7B4D));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x5C49, 0x5C5C));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x79C3, 0x79BF));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x8715, 0x86FB));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x8131, 0x812B));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x6E29, 0x6EAB));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x74EE, 0x7515));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x5367, 0x81E5));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x6C61, 0x6C59));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x53A6, 0x5EC8));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x7FA1, 0x7FA8));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x53A2, 0x5EC2));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x874E, 0x880D));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x643A, 0x651C));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x6C79, 0x6D36));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x865A, 0x865B));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x5618, 0x5653));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x53D9, 0x6558));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x70DF, 0x7159));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x5F66, 0x5F65));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x7476, 0x7464));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x6447, 0x6416));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x9065, 0x9059));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x7A91, 0x7AAF));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x5F02, 0x7570));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x548F, 0x8A60));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x6D8C, 0x6E67));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x5A31, 0x5A1B));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x5CB3, 0x5DBD));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x7CA4, 0x7CB5));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x60A6, 0x6085));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x5300, 0x52FB));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x97F5, 0x97FB));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x707E, 0x707D));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x672D, 0x5284));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x6805, 0x67F5));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x6323, 0x6399));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x7741, 0x775C));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x72F0, 0x7319));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x4E89, 0x722D));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x732A, 0x8C6C));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x5179, 0x8332));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x8E2A, 0x8E64));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x4E98, 0x4E99));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x9F17, 0x9780));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x8288, 0x7F8B));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x53AE, 0x5EDD));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x4FE3, 0x4FC1));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x506C, 0x50AF));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x5156, 0x5157));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x886E, 0x889E));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x7980, 0x7A1F));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x537A, 0x5DF9));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x9667, 0x9689));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x90C4, 0x90E4));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x5942, 0x5950));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x51FC, 0x6C39));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x57A7, 0x5770));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x8279, 0x8278));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x82D8, 0x6ABE));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x839C, 0x84E7));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x8385, 0x849E));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x63F8, 0x6463));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x5423, 0x551A));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x5494, 0x54E2));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x54B4, 0x5645));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x5523, 0x5515));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x5E5E, 0x8946));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x5C99, 0x5DB4));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x5CE5, 0x5D22));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x5D5B, 0x5D33));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x72CD, 0x9E85));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x732C, 0x875F));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x5EEA, 0x5EE9));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x6120, 0x614D));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x61D4, 0x61CD));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x6E0C, 0x6DE5));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x6E86, 0x6F35));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x6F74, 0x7026));		
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x59D7, 0x59CD));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x5AAA, 0x5ABC));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x5B37, 0x5B24));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x73CF, 0x73A8));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x73C9, 0x7449));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x7487, 0x74BF));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x6800, 0x6894));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x684A, 0x68EC));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x6901, 0x69E8));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x6A65, 0x6AEB));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x6A90, 0x7C37));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x6AA9, 0x6A81));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x6B81, 0x6B7F));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x622C, 0x6229));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x7CC7, 0x9931));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x8DD6, 0x8E60));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x8E70, 0x8E95));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x96BD, 0x96CB));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x97B4, 0x97DD));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x9F39, 0x9F34));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x9F44, 0x9F47));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x7DB3, 0x7E43));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x9262, 0x7F3D));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x53A0, 0x5EC1));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x7523, 0x7522));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x885D, 0x6C96));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x920E, 0x9264));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x9DC4, 0x96DE));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x9E7C, 0x583F));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x596C, 0x734E));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x7D76, 0x7D55));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x6BBB, 0x6BBC));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x88CF, 0x88E1));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x9332, 0x9304));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x7DD1, 0x7DA0));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x99E1, 0x7F75));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x5699, 0x9F67));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x5553, 0x555F));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x58BB, 0x7246));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x92ED, 0x92B3));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x7AEA, 0x8C4E));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x8AAC, 0x8AAA));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x5606, 0x6B4E));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x7E27, 0x7D5B));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x983D, 0x9839));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x7232, 0x70BA));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x50DE, 0x507D));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x7DAB, 0x7DDA));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x92B9, 0x93FD));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x7D89, 0x7E61));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x52DB, 0x52F3));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x9854, 0x984F));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x8277, 0x8C54));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x8B21, 0x8B20));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x95B2, 0x95B1));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x85F4, 0x860A));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x9196, 0x919E));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x8D1C, 0x8D13));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x8846, 0x773E));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x8D0B, 0x8D17));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x8B1A, 0x8AE1));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x8B2D, 0x8B7E));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x8493, 0x84F4));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x5D2C, 0x5D20));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x6F59, 0x6E88));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x7067, 0x7069));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x5B00, 0x5AAF));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x7D32, 0x7D4F));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x7E6E, 0x97C1));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x8CEB, 0x9F4E));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x6164, 0x6128));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x9426, 0x9272));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x9387, 0x92C2));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x93B8, 0x942B));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x93BF, 0x933C));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x941D, 0x9481));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x9DC0, 0x9DBF));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x7667, 0x9B01));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x763B, 0x763A));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x88CA, 0x5ACB));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x9B9D, 0x9BD7));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x9C10, 0x9C77));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x9C32, 0x9F07));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x610D, 0x6E63));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x6C32, 0x6C33));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x6C93, 0x905D));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x6E38, 0x904A));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x7145, 0x7146));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x7266, 0x729B));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x75B1, 0x76B0));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x766F, 0x81D2));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x7726, 0x7725));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x7A06, 0x7A6D));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x7B5D, 0x7B8F));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x7C7C, 0x79C8));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x80B7, 0x8181));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x814C, 0x9183));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x816D, 0x9F76));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x817C, 0x9766));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x817D, 0x8183));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x866C, 0x866F));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x869D, 0x8814));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x8941, 0x7E48));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x7096, 0x71C9));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x5BE7, 0x752F));
		m_mpJ2FChar.insert(pair<unsigned long, unsigned long>(0x59DC, 0x8591));

	}	// end of void CHYFontCodec::InitJFChar()


	void CHYFontCodec::SetDefault()
	{
		m_iFontType = -1;	
		m_iStepNo = 0;		

		CloseFile();
		m_vtHYGlyphs.clear();
		m_HYCmap.SetDefault();
		m_HYMaxp.SetDefault();
		m_HYloca.SetDefault();
		m_HYhead.SetDefault();
		m_HYOS2.SetDefault();
		m_HYHhea.SetDefault();
		m_HYHmtx.SetDefault();
		m_HYVhea.SetDefault();
		m_HYVmtx.SetDefault();
		m_HYFpgm.SetDefault();
		m_HYPrep.SetDefault();
		m_HYCvt.SetDefault();
		m_HYName.SetDefault();
		m_HYPost.SetDefault();
		m_HYCFFInfo.SetDefault();
		m_HYTbDirectory.SetDefault();
		m_mulpTableData.setDefault();

	}	// end of void CHYFontCodec::SetDefault()

	int	CHYFontCodec::Decode(const char* pFontFile)
	{
		SetDefault();

		int iRTN = OpenFile(pFontFile);
		if (iRTN != HY_NOERROR)  
			return iRTN;

		// 获取字典目录
		DecodeTableDirectory();

		// 解析MAXP表
		if (Decodemaxp() != HY_NOERROR)return FONT_ERR_MAXP_DECODE;
 		// 解析OS表	
 		if (DecodeOS2() != HY_NOERROR) return FONT_ERR_OS2_DECODE;
		// 解析Head表	
		if (Decodehead() != HY_NOERROR) return FONT_ERR_HEAD_DECODE;	
		// 解析CMAP表
		if (Decodecmap() != HY_NOERROR) return FONT_ERR_CMAP_DECODE;
 		//解析Hhea
 		if (Decodehhea() != HY_NOERROR) return FONT_ERR_HHEA_DECODE;	
 		// 解析HMTX表
 		if (Decodehmtx() != HY_NOERROR) return  FONT_ERR_HMTX_DECODE;
 		// 解析Name表
 		if (Decodename() != HY_NOERROR) return  FONT_ERR_NAME_DECODE;

		// 解析POST表
		if(FindTableEntry(POST_TAG)){
			Decodepost();
		}
		// 解析GSUB表
		if(FindTableEntry(GSUB_TAG)){
			DecodeGSUB();
		}
		// 解析VHEA表
		if(FindTableEntry(VHEA_TAG)){
			Decodevhea();
		}
		// 解析VMTX表
		if(FindTableEntry(VMTX_TAG)){
			Decodevmtx();
		}

#if 0 //emjio 改造临时屏蔽
		// 解析CBLC表
		if(FindTableEntry(CBLC_TAG)){
			DecodeCBLC();
		}
		// 解析CBDT表
		if(FindTableEntry(CBDT_TAG)){
			DecodeCBDT();				
		}
		if (FindTableEntry(EBLC_TAG)) {
			DecodeEBLC();
		}
		// 解析CBDT表
		if (FindTableEntry(EBDT_TAG)) {
			DecodeEBDT();
		}
#endif
		//解析sbix表
		if(FindTableEntry(SBIX_TAG)){
			Decodesbix();				
		}
		// 解析DSIG表
		if(FindTableEntry(DSIG_TAG)){
			DecodeDSIG();
		}
		// 解析VORG表
		if (FindTableEntry(VORG_TAG)){
			DecodeVORG();
		}
		// 解析字形表 OTF(CFF) TTF(GLYF)	
		CollocateGlyf(m_iFontType);

		// 解析FVAR表
		if(FindTableEntry(FVAR_TAG)){
			Decodefvar();
		}
		// 解析STAT表
		if (FindTableEntry(STAT_TAG)){
			DecodeSTAT();
		}
		// 解析avar表
		if (FindTableEntry(AVAR_TAG)){
			Decodeavar();
		}
		// 解析GVAR表
		if (FindTableEntry(GVAR_TAG)){
			Decodegvar();
		}
		if (FindTableEntry(HVAR_TAG)){
			DecodeHVAR();
		}
		if (FindTableEntry(VVAR_TAG)){
			DecodeVVAR();
		}
		if(FindTableEntry(COLR_TAG)){
			DecodeCOLR();
		}
		//解析GASP表
		if (FindTableEntry(GASP_TAG)){		
			Decodegasp();
		}
		CloseFile();

		return HY_NOERROR;

	}	// end of int CHYFontCodec::Decode()

	void CHYFontCodec::Dump(char* pFontFile, char* pDumpFile, std::vector<unsigned long> vtTBFlag)
	{
		CMarkup	DumpMarkUp;
		SetDefault();		

		char strTmp[MAX_PATH] = { 0 };

		DumpMarkUp.SetDoc("<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n");

		int iRTN = OpenFile(pFontFile);
		if (iRTN != HY_NOERROR) return;

		// 必须解析项目
		DecodeTableDirectory();
		if (Decodehead() != HY_NOERROR)
		{
			DumpMarkUp.Save(strTmp);
			return;
		}
		if (Decodemaxp() != HY_NOERROR)
		{
			DumpMarkUp.Save(strTmp);
			return;
		}

		if (m_HYTbDirectory.version.value == 1 && m_HYTbDirectory.version.fract == 0){
			DumpMarkUp.AddElem("ttFont");
			sprintf_s(strTmp, MAX_PATH, "\\x%d,\\x%d,\\x%d,\\x%d", m_HYhead.fontRevision.value & 0xff00, m_HYhead.fontRevision.value & 0x00ff, m_HYhead.fontRevision.fract & 0xff00, m_HYhead.fontRevision.fract & 0x00ff);
			DumpMarkUp.SetAttrib("sfntVersion", strTmp);
		}
		else if (m_HYTbDirectory.version.value == 0x4F54 && m_HYTbDirectory.version.fract == 0x544F){
			DumpMarkUp.AddElem("opentype");
			DumpMarkUp.SetAttrib("sfntVersion", "OTTO");
		}
		else{
			return;
		}
		DumpMarkUp.SetAttrib("ttLibVersion", "3.9");
		DumpMarkUp.IntoElem();

		if (FindFlag(vtTBFlag, HEAD_TAG))
		{
			Dumphead(DumpMarkUp);
		}

		if (FindFlag(vtTBFlag, MAXP_TAG))
		{
			Dumpmaxp(DumpMarkUp);
		}

		if (FindFlag(vtTBFlag, STAT_TAG))
		{
			DecodeSTAT();
			DumpSTAT(DumpMarkUp);
		}

		if (FindFlag(vtTBFlag, FVAR_TAG))
		{
			Decodefvar();
			Dumpfvar(DumpMarkUp);
		}

		if (FindFlag(vtTBFlag, GVAR_TAG))
		{
			CollocateGlyf(FONTTYPE_TTF);		
			Decodepost();

			Decodefvar();
			Decodegvar();
			Dumpgvar(DumpMarkUp);
		}

		if (FindFlag(vtTBFlag, HVAR_TAG))
		{
			DecodeHVAR();
			DumpHVAR(DumpMarkUp);
		}

		DumpMarkUp.OutOfElem();
		DumpMarkUp.Save(pDumpFile);

	}	// end of void CHYFontCodec::Dump()

	int CHYFontCodec::VerifyFont(char* pFontFile, CFontVerifyResult& Result)
	{
		SetDefault();
		
		Result.strFontFile = pFontFile;
		Result.vtVerifyInf.clear();

		int iRTN = OpenFile(pFontFile);
		if (iRTN != HY_NOERROR)
			return iRTN;		

		// 获取字典目录
		DecodeTableDirectory();

		CVerifyInformation VerifyInf;
		if (FindTableEntry(MAXP_TAG))
		{
			if (Decodemaxp() != HY_NOERROR)
			{
				VerifyInf.iWarnLevel = WARNLEVEL_ERR;
				VerifyInf.strInf = "MAXP表结构错误";
				Result.vtVerifyInf.push_back(VerifyInf);
			}
			else
			{
				if (m_HYMaxp.numGlyphs == 0)
				{
					VerifyInf.iWarnLevel = WARNLEVEL_ERR;
					VerifyInf.strInf = "MAXP参数项numGlyphs=0";
					Result.vtVerifyInf.push_back(VerifyInf);
				}
			}
		}
		else
		{
			VerifyInf.iWarnLevel = WARNLEVEL_ERR;
			VerifyInf.strInf = "MAXP表不存在";
			Result.vtVerifyInf.push_back(VerifyInf);
		}
		
		// 解析OS表	
		if (FindTableEntry(OS2_TAG))
		{
			if (DecodeOS2() != HY_NOERROR)
			{
				VerifyInf.iWarnLevel = WARNLEVEL_ERR;
				VerifyInf.strInf = "OS/2表结构错误";
				Result.vtVerifyInf.push_back(VerifyInf);
			}
			else
			{
				if (m_HYOS2.xAvgCharWidth == 0)
				{
					VerifyInf.iWarnLevel = WARNLEVEL_WARN;
					VerifyInf.strInf = "OS/2参数项xAvgCharWidth==0";
					Result.vtVerifyInf.push_back(VerifyInf);
				}

				if (m_HYOS2.usWeightClass == 0)
				{
					VerifyInf.iWarnLevel = WARNLEVEL_WARN;
					VerifyInf.strInf = "OS/2参数项usWeightClass==0";
					Result.vtVerifyInf.push_back(VerifyInf);
				}

				if (m_HYOS2.usWidthClass == 0)
				{
					VerifyInf.iWarnLevel = WARNLEVEL_WARN;
					VerifyInf.strInf = "OS/2参数项usWidthClass==0";
					Result.vtVerifyInf.push_back(VerifyInf);
				}
			}
		}
		else
		{
			VerifyInf.iWarnLevel = WARNLEVEL_ERR;
			VerifyInf.strInf = "OS/2表不存在";
			Result.vtVerifyInf.push_back(VerifyInf);
		}

		// 解析Head表
		if (FindTableEntry(HEAD_TAG))
		{
			if (Decodehead() != HY_NOERROR)
			{
				VerifyInf.iWarnLevel = WARNLEVEL_ERR;
				VerifyInf.strInf = "Head表结构错误";
				Result.vtVerifyInf.push_back(VerifyInf);
			}
		}
		else
		{
			VerifyInf.iWarnLevel = WARNLEVEL_ERR;
			VerifyInf.strInf = "Head表不存在";
			Result.vtVerifyInf.push_back(VerifyInf);
		}
		

		// 解析CMAP表
		if (FindTableEntry(CMAP_TAG))
		{
			if (Decodecmap() != HY_NOERROR)	{
				VerifyInf.iWarnLevel = WARNLEVEL_ERR;
				VerifyInf.strInf = "CMAP表结构错误";
				Result.vtVerifyInf.push_back(VerifyInf);
			}
		}
		else
		{
			VerifyInf.iWarnLevel = WARNLEVEL_ERR;
			VerifyInf.strInf = "CMAP表不存在";
			Result.vtVerifyInf.push_back(VerifyInf);
		}
		
		if (FindTableEntry(HHEA_TAG))
		{
			//解析Hhea
			if (Decodehhea() != HY_NOERROR){
				VerifyInf.iWarnLevel = WARNLEVEL_ERR;
				VerifyInf.strInf = "HHEA表结构错误";
				Result.vtVerifyInf.push_back(VerifyInf);
			}
		}
		else
		{
			VerifyInf.iWarnLevel = WARNLEVEL_ERR;
			VerifyInf.strInf = "HHEA表不存在";
			Result.vtVerifyInf.push_back(VerifyInf);
		}
		
		// 解析HMTX表
		if (FindTableEntry(HMTX_TAG))
		{
			if (Decodehmtx() != HY_NOERROR)	{
				VerifyInf.iWarnLevel = WARNLEVEL_ERR;
				VerifyInf.strInf = "HMTX表结构错误";
				Result.vtVerifyInf.push_back(VerifyInf);
			}
			else
			{
				int iWidth = m_HYHmtx.GetAvgWidth();
				if (iWidth == -1 || iWidth == 0){
					VerifyInf.iWarnLevel = WARNLEVEL_ERR;
					VerifyInf.strInf = "HMTX表数据错误";
					Result.vtVerifyInf.push_back(VerifyInf);
				}
			}
		}
		else
		{	
			VerifyInf.iWarnLevel = WARNLEVEL_ERR;
			VerifyInf.strInf = "HMTX表不存在";
			Result.vtVerifyInf.push_back(VerifyInf);			
		}
		
		if (FindTableEntry(NAME_TAG))
		{
			// 解析Name表
			if (Decodename() != HY_NOERROR){
				VerifyInf.iWarnLevel = WARNLEVEL_ERR;
				VerifyInf.strInf = "NAME表结构错误";
				Result.vtVerifyInf.push_back(VerifyInf);
			}
		}
		else
		{
			VerifyInf.iWarnLevel = WARNLEVEL_ERR;
			VerifyInf.strInf = "NAME表不存在";
			Result.vtVerifyInf.push_back(VerifyInf);
		}

		//判断这个字库是否包含中文简体或繁体字形
		if ((m_HYOS2.ulCodePageRange1 & 0x140000) >0){
			
			// 解析VHEA表
			if (FindTableEntry(VHEA_TAG)){
				iRTN = Decodevhea();
				if (iRTN != NOERROR){
					VerifyInf.iWarnLevel = WARNLEVEL_WARN;
					VerifyInf.strInf = "VHEA表结构错误";
					Result.vtVerifyInf.push_back(VerifyInf);
				}				
			}
			else
			{
				VerifyInf.iWarnLevel = WARNLEVEL_WARN;
				VerifyInf.strInf = "VHEA表不存在";
				Result.vtVerifyInf.push_back(VerifyInf);
			}
			// 解析VMTX表
			if (FindTableEntry(VMTX_TAG)){
				iRTN = Decodevmtx();
				if (iRTN != NOERROR){
					VerifyInf.iWarnLevel = WARNLEVEL_WARN;
					VerifyInf.strInf = "vmtx表结构错误";
					Result.vtVerifyInf.push_back(VerifyInf);
				}
				else
				{
					int iHeight = m_HYVmtx.GetAvgHeight();
					if (iHeight == -1 || iHeight == 0){
						VerifyInf.iWarnLevel = WARNLEVEL_ERR;
						VerifyInf.strInf = "vmtx表数据错误";
						Result.vtVerifyInf.push_back(VerifyInf);
					}				

					int zeorIndex = m_HYVmtx.GetZeroHeight();
					if (zeorIndex != -1){					
						stringstream ss;
						ss << "vmtx索引第"<<zeorIndex<<"项ADH等于0";

						VerifyInf.iWarnLevel = WARNLEVEL_WARN;
						VerifyInf.strInf = ss.str();
						Result.vtVerifyInf.push_back(VerifyInf);
					}
				}
			}
			else
			{
				VerifyInf.iWarnLevel = WARNLEVEL_WARN;
				VerifyInf.strInf = "vmtx表不存在";
				Result.vtVerifyInf.push_back(VerifyInf);
			}

			// 解析GSUB表
			if (FindTableEntry(GSUB_TAG)){
				iRTN =  DecodeGSUB();
				if (iRTN != NOERROR){
					VerifyInf.iWarnLevel = WARNLEVEL_WARN;
					VerifyInf.strInf = "GSUB表结构错误";
					Result.vtVerifyInf.push_back(VerifyInf);
				}
				else
				{
					char pTag[5] = { 0 };
					strcpy(pTag,"vert");

					if (m_HYGsub.FindFeature(pTag) == -1){
						VerifyInf.iWarnLevel = WARNLEVEL_WARN;
						VerifyInf.strInf = "GSUB表中缺少vert特性";
						Result.vtVerifyInf.push_back(VerifyInf);
					}
				}
			}
			else{
				VerifyInf.iWarnLevel = WARNLEVEL_WARN;
				VerifyInf.strInf = "GSUB表不存在";
				Result.vtVerifyInf.push_back(VerifyInf);
			}
		}

		CloseFile();

		return HY_NOERROR;
	
	}	// end of void CHYFontCodec::VerifyFont()	

	int CHYFontCodec::Encode(char* pFontFile, std::vector<unsigned long> vtFlag)
	{		
		::memset(&m_tagOption,0,sizeof(HY_OPTION_PRM));
		m_tagOption.bCmplVert = TRUE;
		return Encode(pFontFile, vtFlag, m_tagOption);
	
	}	// end of int CHYFontCodec::Encode()

	int	CHYFontCodec::Encode(char* pFontFile, std::vector<unsigned long> vtFlag, HY_OPTION_PRM& tagOption)
	{		
		memcpy(&m_tagOption,&tagOption, sizeof(HY_OPTION_PRM));

		int iRtn = CreateFile(pFontFile);		
		if (iRtn != HY_NOERROR)  return iRtn;

		if (vtFlag.size() ==0) return FONT_ERR_BASE;
		if (FindFlag(vtFlag,CFF_TAG))	m_iFontType=FONTTYPE_OTF;	
		if (FindFlag(vtFlag,GLYF_TAG))	m_iFontType=FONTTYPE_TTF;
		MakeHYCodeMap(m_vtHYGlyphs);

		MakeTableDirectory(vtFlag);		
		EncodeTableDirectory();
#if 0	//emjio 改造临时屏蔽
		if (FindFlag(vtFlag, CBDT_TAG)) {
			EncodeCBDT();
		}
		if (FindFlag(vtFlag, CBLC_TAG)) {
			EncodeCBLC();
		}
		if (FindFlag(vtFlag, EBDT_TAG)) {
			EncodeEBDT();
		}
		if (FindFlag(vtFlag, EBLC_TAG)) {
			EncodeEBLC();
		}
#endif
		if (FindFlag(vtFlag, COLR_TAG)){
			EncodeCOLR();
		}		
		if (FindFlag(vtFlag, HEAD_TAG)){
			Encodehead();
		}
		if (FindFlag(vtFlag, HHEA_TAG)){
			Encodehhea();
		}			
		if (FindFlag(vtFlag, MAXP_TAG)){
			Encodemaxp();	
		}		
		if (FindFlag(vtFlag, OS2_TAG)){
			EncodeOS2();
		}
		if (FindFlag(vtFlag, SVG_TAG)) {
			EncodeSVG();
		}
		if (FindFlag(vtFlag, NAME_TAG)){
			Encodename();	
		}		
		if (FindFlag(vtFlag, CMAP_TAG)){			
			Encodecmap(m_pFontFile,m_HYTbDirectory);
		}
		if (FindFlag(vtFlag, VHEA_TAG)){
			//CustomADH 的优先级大于ReVert
			if (m_tagOption.bsetADH) {
				MakeVerticalMetrics();
			}
			else if (m_tagOption.bCmplVert) {
				CountVerticalMetrics();
				MakeVerticalMetrics();
			}
			Encodevhea();
		}		
		if (FindFlag(vtFlag, VMTX_TAG)){
			Encodevmtx();
		}		
		if (FindFlag(vtFlag,CFF_TAG)){
			HYFIXED fxVersion;
			fxVersion.value = 3;
			fxVersion.fract = 0;
			Encodepost(fxVersion);
			EncodeCFF();
			EncodeVORG();
		}
		if (FindFlag(vtFlag, HMTX_TAG)){
			Encodehmtx();
		}		
		if (FindFlag(vtFlag,GLYF_TAG)){
			if (FindFlag(vtFlag, PREP_TAG)){
				Encodeprep();
			}				
			if (FindFlag(vtFlag, FPGM_TAG)){
				Encodefpgm();
			}	
			if (FindFlag(vtFlag, CVT_TAG)){
				EncodeCVT();
			}
			HYFIXED fxVersion;
			fxVersion.value = 2;
			fxVersion.fract = 0;
			Encodepost(fxVersion);
			EncodeGlyph();
			Encodeloca();
		}

		if (FindFlag(vtFlag, GPOS_TAG)) {
			EncodeGPOS();
		}
		if (FindFlag(vtFlag, GSUB_TAG)){
			EncodeGSUB();
		}
		if (FindFlag(vtFlag, DSIG_TAG)){
			EncodeDSIG();
		}
		if(FindFlag(vtFlag,GASP_TAG)){
			MakeGasp_ClearType();
			Encodegasp();
		}
		if (FindFlag(vtFlag, STAT_TAG)){
			EncodeSTAT();
		}
		if (FindFlag(vtFlag, FVAR_TAG)){
			Encodefvar();
		}
		if (FindFlag(vtFlag, GVAR_TAG)){
			Encodegvar();
		}		

		// 对每一个表生成校验码
		fflush(m_pFontFile);
		for (int i=0; i<m_HYTbDirectory.numTables; i++){
			CHYTableEntry&	HYEntry = m_HYTbDirectory.vtTableEntry[i];
			int CheckBufSz = (HYEntry.length+3)/4*4;
			char* pCheckBuf = new char[CheckBufSz];
			fseek(m_pFontFile,HYEntry.offset,SEEK_SET);
			fread(pCheckBuf,CheckBufSz,1,m_pFontFile);
			HYEntry.checkSum = HY_CalcFontTableChecksum((unsigned long*)pCheckBuf,HYEntry.length);
			if (pCheckBuf) delete[] pCheckBuf;
		}
		fflush(m_pFontFile);
		EncodeTableDirectory();
		CloseFile();

#ifndef BIGFILE
		iRtn = SetCheckSumAdjustment(pFontFile);
#endif 
		return iRtn;

	}	// end of int CHYFontCodec::Encode()

	void CHYFontCodec::RestPsName()
	{		
		size_t GlyphsNum = m_vtHYGlyphs.size();
		if (GlyphsNum > 0) {
			m_vtHYGlyphs[0].strPostName = _T(".notdef");
		}
		// 第一个字符必须是.notdef，所以循环从第二个字符开始。
		for (int i = 1; i < GlyphsNum; i++) {
			char  buffer[200] = { 0 };
			if (m_vtHYGlyphs[i].vtUnicode.size() > 0) {
				unsigned long  unicode = m_vtHYGlyphs[i].vtUnicode[0];
				if (unicode >= 0x2E80 && unicode <= 0x2EFF) {
					_stprintf(buffer, _T("uni%X"), unicode);
					m_vtHYGlyphs[i].strPostName = buffer;
				}
				else if (unicode >= 0x2F00 && unicode <= 0x2FDF) {
					_stprintf(buffer, _T("uni%X"), unicode);
					m_vtHYGlyphs[i].strPostName = buffer;
				}
				else if (unicode >= 0x3000 && unicode <= 0x303F) {
					_stprintf(buffer, _T("uni%X"), unicode);
					m_vtHYGlyphs[i].strPostName = buffer;
				}
				else if (unicode >= 0x31C0 && unicode <= 0x31EF) {
					_stprintf(buffer, _T("uni%X"), unicode);
					m_vtHYGlyphs[i].strPostName = buffer;
				}
				else if (unicode >= 0x3300 && unicode <= 0x33FF) {
					_stprintf(buffer, _T("uni%X"), unicode);
					m_vtHYGlyphs[i].strPostName = buffer;
				}
				else if (unicode >= 0x3400 && unicode <= 0x4DBF) {
					_stprintf(buffer, _T("uni%X"), unicode);
					m_vtHYGlyphs[i].strPostName = buffer;
				}
				else if (unicode >= 0x4E00 && unicode <= 0x9FFF) {
					_stprintf(buffer, _T("uni%X"), unicode);
					m_vtHYGlyphs[i].strPostName = buffer;
				}
				else if (unicode >= 0xF900 && unicode <= 0xFAFF) {
					_stprintf(buffer, _T("uni%X"), unicode);
					m_vtHYGlyphs[i].strPostName = buffer;
				}
				else if (unicode >= 0xE000 && unicode <= 0xF8FF) {
					_stprintf(buffer, _T("uni%X"), unicode);
					m_vtHYGlyphs[i].strPostName = buffer;
				}
				else if (unicode >= 0x20000){
					_stprintf(buffer, _T("u%X"), unicode);
					m_vtHYGlyphs[i].strPostName = buffer;
				}
			}
		}
	
	}	// end of int CHYFontCodec::RestPsName()

	void CHYFontCodec::AddControlChar()
	{
		CHYGlyph glyph;
		int iWidth=0;
		int iIndex = FindGryphIndexByUnciodeEx(0x20); ;
		if (iIndex == -1) { 
			iWidth = m_HYhead.unitsPerEm / 2; 

			glyph.advanceWidth = 0;
			glyph.advanceHeight = m_HYOS2.sTypoAscender - m_HYOS2.sTypoDescender;
			glyph.vtUnicode.push_back(0x20);
			glyph.strPostName = "space";
			m_vtHYGlyphs.push_back(glyph);

		}
		else iWidth = m_vtHYGlyphs[iIndex].advanceWidth;

		if (FindGryphIndexByUnciodeEx(0x0A) == -1) {
			glyph.advanceWidth = iWidth;
			glyph.advanceHeight = m_HYOS2.sTypoAscender - m_HYOS2.sTypoDescender;
			glyph.vtUnicode.push_back(0x0A);
			glyph.strPostName = "LF";
			m_vtHYGlyphs.push_back(glyph);
		}

		if (FindGryphIndexByUnciodeEx(0x0D) == -1) {
			glyph.advanceWidth = iWidth;
			glyph.advanceHeight = m_HYOS2.sTypoAscender - m_HYOS2.sTypoDescender;
			glyph.vtUnicode.push_back(0x0D);
			glyph.strPostName = "CR";
			m_vtHYGlyphs.push_back(glyph);
		}

	}	// end of vjoid	CHYFontCodec::AddControlChar()

	int	CHYFontCodec::EncodeSVG(char* pSVGFile)
	{
		CloseFile();

		m_pFontFile = fopen(pSVGFile,"w+t");
		if (m_pFontFile == NULL) 
		{
			_set_errno(0);
			return HY_ERR_FILE_WRITE;		
		}

		std::string			fontname = m_HYName.GetFamilyNameEN();
		std::stringstream	stream;
		std::string			strContent;

		strContent = "<?xml version=\"1.0\" standalone=\"no\"?>\n";
		fwrite(strContent.data(),strContent.length(),1,m_pFontFile);

		strContent = "<svg xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" version=\"1.1\">\n";
		fwrite(strContent.data(),strContent.length(),1,m_pFontFile);
		
		// metadata
		stream<<"<metadata> Created by Hanyi Copyright (c) ";
		stream<<fontname;
		stream<<"</metadata>\n";
		strContent = stream.str();
		fwrite(strContent.data(),strContent.length(),1,m_pFontFile);

		//def		
		strContent = "<defs>\n";
		fwrite(strContent.data(),strContent.length(),1,m_pFontFile);

		//font		
		stream.str("");
		stream<<"<font id=";
		stream<<"\""<<fontname<<"\"";			
		stream<<" horiz-adv-x=";
		stream<<"\""<<m_HYHhea.advanceWidthMax<<"\"";
		stream<<">\n";
		strContent = stream.str();
		fwrite(strContent.data(),strContent.length(),1,m_pFontFile);

		//font-face
		stream.str("");
		stream<<"<font face ";
 		stream<<"font-family="<<"\""<<fontname<<"\"";
 		stream<<"\n font-weight="<<"\""<<m_HYOS2.usWeightClass<<"\"";
 		stream<<"\n units-per-em="<<"\""<<m_HYhead.unitsPerEm<<"\"";
 		stream<<"\n cap-height="<<"\""<<m_HYOS2.sCapHeight<<"\"";
 		stream<<"\n x-height="<<"\""<<m_HYOS2.sxHeight<<"\"";
 		stream<<"\n ascent="<<"\""<<m_HYHhea.Ascender<<"\"";
 		stream<<"\n descent="<<"\""<<m_HYHhea.Descender<<"\"";
		stream<<"\n />";
		strContent = stream.str();
		fwrite(strContent.data(),strContent.length(),1,m_pFontFile);

		//glyph
		stream.str("");
		stream<<"\n <missing-glyph ";
		stream<<"horiz-adv-x="<<"\""<<m_HYhead.unitsPerEm<<"\"";		
		stream<<"/>";
		strContent = stream.str();
		fwrite(strContent.data(),strContent.length(),1,m_pFontFile);

		for (int i=1; i<m_HYMaxp.numGlyphs; i++)
		{
			if(i== 39)
			{
				int ijinjuntest = 0;
			}
			stream.str("");
			stream<<"\n";
			CHYGlyph& glyph = m_vtHYGlyphs[i];
			std::vector<unsigned long> vtunicode;
			FindGryphUncidoByIndex(i,vtunicode);
			
			if (vtunicode.size()==0)
			{
				stream<<"<glyph glyph-name=\" \"";
				stream<<"unicode=\" \"";
			}
			else 
			{
				unsigned short uni = vtunicode[0];
				char p[MAX_PATH] = {0};
				sprintf_s(p,MAX_PATH,"%X",uni);
				stream<<"<glyph glyph-name="<<"uni"<<p<<" ";
				stream<<"unicode="<<"&#x"<<p<<";";
			}

			stream<<"\n";
			size_t szContourNum = glyph.vtContour.size();

			if (szContourNum>0)
			{
				stream<<"d=\"";
				for (int j=0; j<szContourNum; j++)
				{				
					CHYContour& contour = glyph.vtContour[j];
					size_t ptNum = contour.vtHYPoints.size();
					size_t ptIndex =0;
					while(ptIndex<ptNum)
					{
						CHYPoint& ptCUR = contour.vtHYPoints[ptIndex];
						if (ptIndex==0)
						{
							stream<<"\nM"<<ptCUR.x<<" "<<ptCUR.y;
						}
						else 
						{
							if (ptCUR.flag==POINT_FLG_ANCHOR)
							{
								stream<<"L"<<ptCUR.x<<" "<<ptCUR.y;								
							}

							if (ptCUR.flag==POINT_FLG_CONTROL)
							{
								stream<<"Q"<<ptCUR.x<<" "<<ptCUR.y;

								if (ptIndex+1<ptNum)
								{
									CHYPoint& ptJa = contour.vtHYPoints[++ptIndex];
									if (ptCUR.flag == POINT_FLG_CONTROL)
									{
										int x = (ptJa.x + contour.vtHYPoints[ptIndex-1].x)/2;
										int y = (ptJa.y + contour.vtHYPoints[ptIndex-1].y)/2;
										stream<<" "<<x<<" "<<y;
										continue;
									}

									if (ptCUR.flag == POINT_FLG_ANCHOR)
									{
										stream<<" "<<ptCUR.x<<" "<<ptCUR.y;
									}
								}
								else 
								{
									ptCUR = contour.vtHYPoints[0];
									stream<<" "<<ptCUR.x<<" "<<ptCUR.y;									
								}											
							}
						}		
						ptIndex++;
					}	
					stream<<"z";	
				}

				stream<<"\"";
			}
			stream<<"/>";
			strContent = stream.str();
			fwrite(strContent.data(),strContent.length(),1,m_pFontFile);
		}

		strContent = "\n</font>";
		fwrite(strContent.data(),strContent.length(),1,m_pFontFile);
		strContent = "\n</defs>";
		fwrite(strContent.data(),strContent.length(),1,m_pFontFile);
		strContent ="\n</svg>";
		fwrite(strContent.data(),strContent.length(),1,m_pFontFile);

		return HY_NOERROR;

	}	// end of int	CHYFontCodec::EncodeSVG()

	int	CHYFontCodec::EncodeEOT(const char* pEotFile, const char* pFontFile)
	{	
		//FontDataSize
		long FontDataSize = HY_FileLength(pFontFile);
		if (FontDataSize==0) return  HY_ERR_FILE_READ;

		char* pFontData = new char[FontDataSize];
		FILE*	FontF = fopen(pFontFile,"rb");		
		if (FontF==0)
		{
			_set_errno(0);
			return HY_ERR_FILE_READ;
		}
		fread(pFontData,FontDataSize,1,FontF);
		fclose(FontF);

		int iRet = Decode(pFontFile);
		if (HY_NOERROR != iRet)
		{
			return iRet;
		}

		CloseFile();
		m_pFontFile = fopen(pEotFile,"w+b");
		if (m_pFontFile == NULL) 
		{
			_set_errno(0);
			return HY_ERR_FILE_WRITE;
		}

		unsigned long	lTmp = 0;
		unsigned short  sTmp = 0;

		//EOTSize
		fwrite(&lTmp,4,1,m_pFontFile);
		//FontDataSize
		lTmp = FontDataSize;
		fwrite(&lTmp,4,1,m_pFontFile);
		//Version
		lTmp = 0x00020001;
		fwrite(&lTmp,4,1,m_pFontFile);
		//Flags
		lTmp = 0;
		fwrite(&lTmp,4,1,m_pFontFile);
		//FontPANOSE		
		fwrite(&m_HYOS2.panose.FamilyType,1,1,m_pFontFile);
		fwrite(&m_HYOS2.panose.SerifStyle,1,1,m_pFontFile);
		fwrite(&m_HYOS2.panose.Weight,1,1,m_pFontFile);
		fwrite(&m_HYOS2.panose.Proportion,1,1,m_pFontFile);
		fwrite(&m_HYOS2.panose.Contrast,1,1,m_pFontFile);
		fwrite(&m_HYOS2.panose.StrokeVariation,1,1,m_pFontFile);
		fwrite(&m_HYOS2.panose.ArmStyle,1,1,m_pFontFile);
		fwrite(&m_HYOS2.panose.Letterform,1,1,m_pFontFile);
		fwrite(&m_HYOS2.panose.Midline,1,1,m_pFontFile);
		fwrite(&m_HYOS2.panose.XHeight,1,1,m_pFontFile);
		//Charset
		byte bTmp = 0x01;
		fwrite(&bTmp,1,1,m_pFontFile);
		//Italic
		bTmp = 0;
		fwrite(&bTmp,1,1,m_pFontFile);
		//Weight
		lTmp = m_HYOS2.usWeightClass;
		fwrite(&lTmp,4,1,m_pFontFile);
		//fsType
		sTmp = m_HYOS2.fsType;
		fwrite(&sTmp,2,1,m_pFontFile);
		//MagicNumber
		sTmp = 0x504C;
		fwrite(&sTmp,2,1,m_pFontFile);
		//UnicodeRange1		
		fwrite(&m_HYOS2.ulUnicodeRange1,4,1,m_pFontFile);
		//UnicodeRange2		
		fwrite(&m_HYOS2.ulUnicodeRange2,4,1,m_pFontFile);
		//UnicodeRange3		
		fwrite(&m_HYOS2.ulUnicodeRange3,4,1,m_pFontFile);
		//UnicodeRange4		
		fwrite(&m_HYOS2.ulUnicodeRange4,4,1,m_pFontFile);
		//CodePageRange1
		fwrite(&m_HYOS2.ulCodePageRange1,4,1,m_pFontFile);
		//CodePageRange2
		fwrite(&m_HYOS2.ulCodePageRange2,4,1,m_pFontFile);
		//CheckSumAdjustment
		fwrite(&m_HYhead.checkSumAdjustment,4,1,m_pFontFile);
		//Reserved1
		lTmp = 0;
		fwrite(&lTmp,4,1,m_pFontFile);
		//Reserved2
		fwrite(&lTmp,4,1,m_pFontFile);
		//Reserved3
		fwrite(&lTmp,4,1,m_pFontFile);
		//Reserved4
		fwrite(&lTmp,4,1,m_pFontFile);		
		
		size_t stName = m_HYName.vtNameRecord.size();
		for(size_t i=0; i<stName; i++)
		{			
			NAMERECORD& record = m_HYName.vtNameRecord[i];
			if (record.platformID == 3 && record.encodingID == 1&&record.languageID==1033&&record.nameID==1)
			{
				char strRecord[1040] = {0};
				int strLen = 1040;
				//Padding1
				sTmp = 0;
				fwrite(&sTmp,2,1,m_pFontFile);
				//FamilyNameSize
				sTmp = record.length;
				fwrite(&sTmp,2,1,m_pFontFile);
				//FamilyName				
				MulToWChar(strRecord, strLen, record.strContent.c_str(), strlen(record.strContent.c_str()));
				fwrite(strRecord,strLen,1,m_pFontFile);
				break;
			}
		}

		for(size_t i=0; i<stName; i++)
		{
			NAMERECORD& record = m_HYName.vtNameRecord[i];
			if (record.platformID == 3 && record.encodingID == 1&&record.languageID==1033&&record.nameID==2)
			{
				char strRecord[1040] = {0};
				int strLen = 1040;
				//Padding2
				sTmp = 0;
				fwrite(&sTmp,2,1,m_pFontFile);
				//StyleNameSize
				sTmp = record.length;
				fwrite(&sTmp,2,1,m_pFontFile);
				//StyleName
				MulToWChar(strRecord, strLen, record.strContent.c_str(), strlen(record.strContent.c_str()));
				fwrite(strRecord,strLen,1,m_pFontFile);
				break;
			}
		}
		for(size_t i=0; i<stName; i++)
		{
			NAMERECORD& record = m_HYName.vtNameRecord[i];
			if (record.platformID == 3 && record.encodingID == 1&&record.languageID==1033&&record.nameID==5)			
			{
				char strRecord[1040] = {0};
				int strLen = 1040;
				//Padding3
				sTmp = 0;
				fwrite(&sTmp,2,1,m_pFontFile);
				//VersionNameSize
				sTmp = record.length;
				fwrite(&sTmp,2,1,m_pFontFile);
				//VersionName
				MulToWChar(strRecord, strLen, record.strContent.c_str(), strlen(record.strContent.c_str()));
				fwrite(strRecord,strLen,1,m_pFontFile);
				break;
			}
		}

		for(size_t i=0; i<stName; i++)
		{
			NAMERECORD& record = m_HYName.vtNameRecord[i];
			if (record.platformID == 3 && record.encodingID == 1&&record.languageID==1033&&record.nameID==4)
			{
				char strRecord[1040] = {0};
				int strLen = 1040;
				//Padding4
				sTmp = 0;
				fwrite(&sTmp,2,1,m_pFontFile);
				//FullNameSize
				sTmp = record.length;
				fwrite(&sTmp,2,1,m_pFontFile);
				//FullName
				MulToWChar(strRecord, strLen, record.strContent.c_str(), strlen(record.strContent.c_str()));
				fwrite(strRecord,strLen,1,m_pFontFile);
				break;
			}			
		}

		//Padding5
		sTmp = 0;
		fwrite(&sTmp,2,1,m_pFontFile);
		//RootStringSize
		fwrite(&sTmp,2,1,m_pFontFile);

		//FontData
		fwrite(pFontData,FontDataSize,1,m_pFontFile);		
		fflush(m_pFontFile);

		long lEotLength = ftell(m_pFontFile);
		fseek(m_pFontFile,0,SEEK_SET);
		fwrite(&lEotLength,4,1,m_pFontFile);

		fclose(m_pFontFile);

		if (pFontData != 0) delete[] pFontData;

		return HY_NOERROR;

	}	// end of int	CHYFontCodec::EncodeEOT()
#if 0//emjio 改造临时屏蔽
	int	CHYFontCodec::EncodeEmojiOpentypeFont(char* pFontFile, char* EmojiProfile)
	{
		int iRtn = CreateFile(pFontFile);	
		if (iRtn != HY_NOERROR)  return iRtn;

		// 构建目录						
		MakeBitmapGlyphs(EmojiProfile);		
		m_HYCodeMap.QuickSortbyUnicode();

		m_HYTbDirectory.vtTableEntry.clear();
		CHYTableEntry	HYEntry;	
		m_HYTbDirectory.version.value = 1;
		m_HYTbDirectory.version.fract = 0;
		HYEntry.tag =CBDT_TAG;
		m_HYTbDirectory.vtTableEntry.push_back(HYEntry);
		HYEntry.tag =CBLC_TAG;
		m_HYTbDirectory.vtTableEntry.push_back(HYEntry);
		HYEntry.tag = OS2_TAG;
		m_HYTbDirectory.vtTableEntry.push_back(HYEntry);
		HYEntry.tag	= CMAP_TAG;
		m_HYTbDirectory.vtTableEntry.push_back(HYEntry);
		HYEntry.tag	= HEAD_TAG;
		m_HYTbDirectory.vtTableEntry.push_back(HYEntry);
		HYEntry.tag	= HHEA_TAG;
		m_HYTbDirectory.vtTableEntry.push_back(HYEntry);
		HYEntry.tag	= HMTX_TAG;
		m_HYTbDirectory.vtTableEntry.push_back(HYEntry);
		HYEntry.tag = MAXP_TAG;
		m_HYTbDirectory.vtTableEntry.push_back(HYEntry);
		HYEntry.tag	= NAME_TAG;
		m_HYTbDirectory.vtTableEntry.push_back(HYEntry);
		HYEntry.tag	= POST_TAG;
		m_HYTbDirectory.vtTableEntry.push_back(HYEntry);

		MakeCmap();
		// 参照AppleColorEmoji2016.ttf 参数设置
		m_HYhead.unitsPerEm = 800;
		m_HYhead.yMin = -18;
		m_HYhead.yMax = 800;
		m_HYhead.xMax = 844;
		m_HYhead.xMin = -14;

		unsigned short	usTmp = 0;
		unsigned long	ulTmp = 0;

		EncodeTableDirectory();		
		Encodehead();			

		// HHEA表
		int iEntryIndex = m_HYTbDirectory.FindTableEntry(HHEA_TAG);
		if (iEntryIndex == -1) return FONT_ERR_HHEA_ENCODE;
		CHYTableEntry& tbEntryHHEA = m_HYTbDirectory.vtTableEntry[iEntryIndex];
		tbEntryHHEA.offset = ftell(m_pFontFile);
		// version 
		m_HYHhea.version.value = 1;
		usTmp = hy_cdr_int16_to(m_HYHhea.version.value);
		fwrite(&usTmp,2,1,m_pFontFile);
		m_HYHhea.version.fract = 0;
		usTmp = hy_cdr_int16_to(m_HYHhea.version.fract);
		fwrite(&usTmp,2,1,m_pFontFile);			
		//Ascender
		if (m_HYHhea.Ascender==0)
			m_HYHhea.Ascender = m_HYhead.yMax;
		usTmp = hy_cdr_int16_to(m_HYHhea.Ascender);
		fwrite(&usTmp,2,1,m_pFontFile);
		//Descender
		if (m_HYHhea.Descender==0)
			m_HYHhea.Descender = m_HYhead.yMin;			
		usTmp = hy_cdr_int16_to(m_HYHhea.Descender);
		fwrite(&usTmp,2,1,m_pFontFile);
		//LineGap			
		usTmp = hy_cdr_int16_to(m_HYHhea.LineGap);
		fwrite(&usTmp,2,1,m_pFontFile);
		//advanceWidthMax			
		usTmp =hy_cdr_int16_to(m_HYhead.unitsPerEm);
		fwrite(&usTmp,2,1,m_pFontFile);
		//minLeftSideBearing
		m_HYHhea.minLeftSideBearing = m_HYhead.xMin;
		usTmp =hy_cdr_int16_to(m_HYHhea.minLeftSideBearing);
		fwrite(&usTmp,2,1,m_pFontFile);
		//minRightSideBearing
		m_HYHhea.minRightSideBearing = m_HYhead.unitsPerEm - m_HYHhea.minLeftSideBearing-(m_HYhead.xMax-m_HYhead.xMin);
		usTmp =hy_cdr_int16_to(m_HYHhea.minRightSideBearing);
		fwrite(&usTmp,2,1,m_pFontFile);
		//xMaxExtent
		m_HYHhea.xMaxExtent = m_HYHhea.minLeftSideBearing+(m_HYhead.xMax-m_HYhead.xMin);
		usTmp =hy_cdr_int16_to(m_HYHhea.xMaxExtent);
		fwrite(&usTmp,2,1,m_pFontFile);
		//caretSlopeRise		
		usTmp = hy_cdr_int16_to(1);
		fwrite(&usTmp,2,1,m_pFontFile);
		//caretSlopeRun
		usTmp = 0;
		fwrite(&usTmp,2,1,m_pFontFile);
		//caretOffset
		usTmp = 0;
		fwrite(&usTmp,2,1,m_pFontFile);
		usTmp = 0;
		//reserved1			
		fwrite(&usTmp,2,1,m_pFontFile);
		//reserved2			
		fwrite(&usTmp,2,1,m_pFontFile);
		//reserved3			
		fwrite(&usTmp,2,1,m_pFontFile);
		//reserved4			
		fwrite(&usTmp,2,1,m_pFontFile);
		//metricDataFormat
		usTmp = 0;
		fwrite(&usTmp,2,1,m_pFontFile);
		//numberOfHMetrics			
		m_HYHhea.numberOfHMetrics = 2;	// 为了适配oppo emjio字库
		usTmp = hy_cdr_int16_to(m_HYHhea.numberOfHMetrics);
		fwrite(&usTmp,2,1,m_pFontFile);
		tbEntryHHEA.length = ftell(m_pFontFile) - tbEntryHHEA.offset;
		fourbytesAlign(tbEntryHHEA.length);	
		
		iEntryIndex = m_HYTbDirectory.FindTableEntry(MAXP_TAG);
		if (iEntryIndex == -1) return FONT_ERR_HMTX_ENCODE;
		CHYTableEntry& tbEntryNAXP = m_HYTbDirectory.vtTableEntry[iEntryIndex];
		tbEntryNAXP.offset = ftell(m_pFontFile);
		size_t			szGlyphNum			= m_HYCodeMap.vtHYCodeMap.size();			
		//Table version number
		m_HYMaxp.version.value = 1;
		usTmp = hy_cdr_int16_to(m_HYMaxp.version.value);
		fwrite(&usTmp,2,1,m_pFontFile);
		m_HYMaxp.version.fract = 0;
		fwrite(&m_HYMaxp.version.fract,2,1,m_pFontFile);
		//numGlyphs
		m_HYMaxp.numGlyphs = (unsigned short)szGlyphNum;
		usTmp = hy_cdr_int16_to(m_HYMaxp.numGlyphs);
		fwrite(&usTmp,2,1,m_pFontFile);		
		//maxPoints
		m_HYMaxp.maxPoints = 8;
		usTmp = hy_cdr_int16_to(m_HYMaxp.maxPoints);
		fwrite(&usTmp,2,1,m_pFontFile);
		//maxContours
		m_HYMaxp.maxContours = 2;
		usTmp = hy_cdr_int16_to(m_HYMaxp.maxContours);
		fwrite(&usTmp,2,1,m_pFontFile);
		//maxCompositePoints				
		usTmp = 0;
		fwrite(&usTmp,2,1,m_pFontFile);
		//maxCompositeContours
		usTmp = 0;	
		fwrite(&usTmp,2,1,m_pFontFile);
		//maxZones
		usTmp = hy_cdr_int16_to(2);
		fwrite(&usTmp,2,1,m_pFontFile);
		//maxTwilightPoints
		usTmp = hy_cdr_int16_to(4);
		fwrite(&usTmp,2,1,m_pFontFile);
		//maxStorage
		usTmp = hy_cdr_int16_to(32);
		fwrite(&usTmp,2,1,m_pFontFile);
		//maxFunctionDefs
		usTmp = hy_cdr_int16_to(96);
		fwrite(&usTmp,2,1,m_pFontFile);
		//maxInstructionDefs
		usTmp = hy_cdr_int16_to(96);
		fwrite(&usTmp,2,1,m_pFontFile);
		//maxStackElements
		usTmp = hy_cdr_int16_to(256);
		fwrite(&usTmp,2,1,m_pFontFile);
		//maxSizeOfInstructions
		usTmp = 8;
		fwrite(&usTmp,2,1,m_pFontFile);
		//maxComponentElements
		usTmp = 0;
		fwrite(&usTmp,2,1,m_pFontFile);
		//maxComponentDepth
		usTmp = 0;
		fwrite(&usTmp,2,1,m_pFontFile);
		tbEntryNAXP.length = ftell(m_pFontFile) - tbEntryNAXP.offset;
		fourbytesAlign(tbEntryNAXP.length);
					
		iEntryIndex = m_HYTbDirectory.FindTableEntry(OS2_TAG);
		if (iEntryIndex == -1) return FONT_ERR_OS2_ENCODE;
		CHYTableEntry& tbEntryOS2 = m_HYTbDirectory.vtTableEntry[iEntryIndex];
		tbEntryOS2.offset = ftell(m_pFontFile);
		//version
		usTmp = hy_cdr_int16_to(0x0003);
		fwrite(&usTmp,2,1,m_pFontFile);			
		//xAvgCharWidth			
		usTmp = hy_cdr_int16_to(1000);
		fwrite(&usTmp,2,1,m_pFontFile);
		//usWeightClass
		if (m_HYOS2.usWeightClass==0)
		{
			m_HYOS2.usWeightClass=400;
		}
		usTmp = hy_cdr_int16_to(m_HYOS2.usWeightClass);			
		fwrite(&usTmp,2,1,m_pFontFile);
		//usWidthClass
		if (m_HYOS2.usWidthClass==0)
		{
			float fRatioe = (float)((m_HYhead.xMax-m_HYhead.xMin)*100)/(float)(m_HYhead.yMax-m_HYhead.yMin);
			if (fRatioe<=50.0)							m_HYOS2.usWidthClass = 1;	
			if (fRatioe>50.0&&fRatioe<=62.5)			m_HYOS2.usWidthClass = 2;
			if (fRatioe>62.5&&fRatioe<=75.0)			m_HYOS2.usWidthClass = 3;
			if (fRatioe>75.0&&fRatioe<=87.5)			m_HYOS2.usWidthClass = 4;
			if (fRatioe>87.5&&fRatioe<112.5)			m_HYOS2.usWidthClass = 5;
			if (fRatioe>=112.5&&fRatioe<125.0)			m_HYOS2.usWidthClass = 6;
			if (fRatioe>=125.0&&fRatioe<150.0)			m_HYOS2.usWidthClass = 7;
			if (fRatioe>=150.0&&fRatioe<200)			m_HYOS2.usWidthClass = 8;
			if (fRatioe>=200.0)							m_HYOS2.usWidthClass = 9;
		}
		usTmp = hy_cdr_int16_to(m_HYOS2.usWidthClass);
		fwrite(&usTmp,2,1,m_pFontFile);
		//fsType
		if (m_HYOS2.fsType==0)
		{
			m_HYOS2.fsType = 0x0008;
		}
		usTmp = hy_cdr_int16_to(m_HYOS2.fsType);
		fwrite(&usTmp,2,1,m_pFontFile);			
		//ySubscriptXSize
		if (m_HYOS2.ySubscriptXSize==0)
		{
			m_HYOS2.ySubscriptXSize = m_HYhead.unitsPerEm/10;
		}
		usTmp = hy_cdr_int16_to(m_HYOS2.ySubscriptXSize);
		fwrite(&usTmp,2,1,m_pFontFile);
		//ySubscriptYSize
		if (m_HYOS2.ySubscriptYSize==0)
		{
			m_HYOS2.ySubscriptYSize = m_HYhead.unitsPerEm/10;
		}
		usTmp = hy_cdr_int16_to(m_HYOS2.ySubscriptYSize);
		fwrite(&usTmp,2,1,m_pFontFile);
		//ySubscriptXOffset			
		usTmp = hy_cdr_int16_to(m_HYOS2.ySubscriptXOffset);
		fwrite(&usTmp,2,1,m_pFontFile);
		//ySubscriptYOffset			
		usTmp = hy_cdr_int16_to(m_HYOS2.ySubscriptYOffset);
		fwrite(&usTmp,2,1,m_pFontFile);
		//ySuperscriptXSize
		if (m_HYOS2.ySuperscriptXSize==0)
		{
			m_HYOS2.ySuperscriptXSize = m_HYhead.unitsPerEm/10;
		}
		usTmp = hy_cdr_int16_to(m_HYOS2.ySuperscriptXSize);
		fwrite(&usTmp,2,1,m_pFontFile);
		//ySuperscriptYSize
		if (m_HYOS2.ySuperscriptYSize==0)
		{
			m_HYOS2.ySuperscriptYSize = m_HYhead.unitsPerEm/10;
		}
		usTmp = hy_cdr_int16_to(m_HYOS2.ySuperscriptYSize);
		fwrite(&usTmp,2,1,m_pFontFile);
		//ySuperscriptXOffset
		usTmp = hy_cdr_int16_to(m_HYOS2.ySuperscriptXOffset);
		fwrite(&usTmp,2,1,m_pFontFile);
		//ySuperscriptYOffset
		usTmp = hy_cdr_int16_to(m_HYOS2.ySuperscriptYOffset);
		fwrite(&usTmp,2,1,m_pFontFile);
		//yStrikeoutSize
		if (m_HYOS2.yStrikeoutSize==0)
		{
			m_HYOS2.yStrikeoutSize = m_HYhead.unitsPerEm/20;
		}
		usTmp = hy_cdr_int16_to(m_HYOS2.yStrikeoutSize);
		fwrite(&usTmp,2,1,m_pFontFile);
		//yStrikeoutPosition
		if (m_HYOS2.yStrikeoutPosition==0)
		{
			m_HYOS2.yStrikeoutPosition = m_HYhead.unitsPerEm/5;
		}
		usTmp = hy_cdr_int16_to(m_HYOS2.yStrikeoutPosition);
		fwrite(&usTmp,2,1,m_pFontFile);
		//sFamilyClass
		usTmp = hy_cdr_int16_to(m_HYOS2.sFamilyClass);
		fwrite(&usTmp,2,1,m_pFontFile);
		//Panose			
		fwrite(&m_HYOS2.panose.FamilyType,1,1,m_pFontFile);
		fwrite(&m_HYOS2.panose.SerifStyle,1,1,m_pFontFile);
		fwrite(&m_HYOS2.panose.Weight,1,1,m_pFontFile);
		fwrite(&m_HYOS2.panose.Proportion,1,1,m_pFontFile);
		fwrite(&m_HYOS2.panose.Contrast,1,1,m_pFontFile);
		fwrite(&m_HYOS2.panose.StrokeVariation,1,1,m_pFontFile);
		fwrite(&m_HYOS2.panose.ArmStyle,1,1,m_pFontFile);
		fwrite(&m_HYOS2.panose.Letterform,1,1,m_pFontFile);
		fwrite(&m_HYOS2.panose.Midline,1,1,m_pFontFile);
		fwrite(&m_HYOS2.panose.XHeight,1,1,m_pFontFile);
		m_HYOS2.ulUnicodeRange1 = 0;
		m_HYOS2.ulUnicodeRange2 = 0;
		m_HYOS2.ulUnicodeRange3 = 0;
		m_HYOS2.ulUnicodeRange4 = 0;
		CountUnicodeRange(m_HYOS2.ulUnicodeRange1,m_HYOS2.ulUnicodeRange2,m_HYOS2.ulUnicodeRange3,m_HYOS2.ulUnicodeRange4);
		//ulUnicodeRange1 
		ulTmp = hy_cdr_int32_to(m_HYOS2.ulUnicodeRange1);
		fwrite(&ulTmp,4,1,m_pFontFile);
		//ulUnicodeRange2 	
		ulTmp = hy_cdr_int32_to(m_HYOS2.ulUnicodeRange2);
		fwrite(&ulTmp,4,1,m_pFontFile);
		//ulUnicodeRange3 	
		ulTmp = hy_cdr_int32_to(m_HYOS2.ulUnicodeRange3);
		fwrite(&ulTmp,4,1,m_pFontFile);
		//ulUnicodeRange4
		ulTmp = hy_cdr_int32_to(m_HYOS2.ulUnicodeRange4);
		fwrite(&ulTmp,4,1,m_pFontFile);			
		//achVendID
		fwrite(&m_HYOS2.vtachVendID[0],1,1,m_pFontFile);
		fwrite(&m_HYOS2.vtachVendID[1],1,1,m_pFontFile);
		fwrite(&m_HYOS2.vtachVendID[2],1,1,m_pFontFile);
		fwrite(&m_HYOS2.vtachVendID[3],1,1,m_pFontFile);			
		//fsSelection
		usTmp = hy_cdr_int16_to(m_HYOS2.fsSelection);
		fwrite(&usTmp,2,1,m_pFontFile);
		//usFirstCharIndex
		m_HYOS2.usFirstCharIndex =	(unsigned short)m_HYCodeMap.vtHYCodeMap[0].ulGlyphNo;
		usTmp = hy_cdr_int16_to(m_HYOS2.usFirstCharIndex);
		fwrite(&usTmp,2,1,m_pFontFile);
		//usLastCharIndex
		if (m_HYCodeMap.vtHYCodeMap.size()>1)
		{
			unsigned long uluni = m_HYCodeMap.vtHYCodeMap[m_HYCodeMap.vtHYCodeMap.size()-2].ulGlyphNo;			
			if (uluni>0xffff)
			{
				m_HYOS2.usLastCharIndex = 0xfdfd;//基本多平面最后一个字形。（阿拉拍语 - 真主阿拉）
			}
			else 
			{
				m_HYOS2.usLastCharIndex = (unsigned short)uluni;
			}
			usTmp = hy_cdr_int16_to(m_HYOS2.usLastCharIndex);
			fwrite(&usTmp,2,1,m_pFontFile);
		}
		else 
		{
			usTmp = 0xffff;
			fwrite(&usTmp,2,1,m_pFontFile);
		}
		//sTypoAscender
		if (m_HYOS2.sTypoAscender==0)
		{
			m_HYOS2.sTypoAscender = m_HYhead.yMax;
		}			
		usTmp = hy_cdr_int16_to(m_HYOS2.sTypoAscender);			
		fwrite(&usTmp,2,1,m_pFontFile);
		//sTypoDescender
		if (m_HYOS2.sTypoDescender==0)
		{			
			m_HYOS2.sTypoDescender = m_HYhead.yMin;
		}			
		usTmp = hy_cdr_int16_to(m_HYOS2.sTypoDescender);			
		fwrite(&usTmp,2,1,m_pFontFile);
		//sTypoLineGap			
		usTmp = hy_cdr_int16_to(m_HYOS2.sTypoLineGap);			
		fwrite(&usTmp,2,1,m_pFontFile);
		//usWinAscent
		if (m_HYOS2.usWinAscent==0)
		{
			m_HYOS2.usWinAscent = m_HYhead.yMax;
		}			
		usTmp = hy_cdr_int16_to(m_HYOS2.usWinAscent);			
		fwrite(&usTmp,2,1,m_pFontFile);
		//usWinDescent
		if (m_HYOS2.usWinDescent==0)
		{
			m_HYOS2.usWinDescent = abs(m_HYhead.yMin);
		}			
		usTmp = hy_cdr_int16_to(m_HYOS2.usWinDescent);			
		fwrite(&usTmp,2,1,m_pFontFile);
		//ulCodePageRange1 
		ulTmp = hy_cdr_int32_to(m_HYOS2.ulCodePageRange1);
		fwrite(&ulTmp,4,1,m_pFontFile);
		//ulCodePageRange2 
		ulTmp = hy_cdr_int32_to(m_HYOS2.ulCodePageRange2);
		fwrite(&ulTmp,4,1,m_pFontFile);
		//sxHeight		
		usTmp = 0;
		fwrite(&usTmp,2,1,m_pFontFile);
		//sCapHeight			
		usTmp = 0;
		fwrite(&usTmp,2,1,m_pFontFile);
		//usDefaultChar		
		usTmp = 0;
		fwrite(&usTmp,2,1,m_pFontFile);
		//usBreakChar
		usTmp = 0x2000;
		fwrite(&usTmp,2,1,m_pFontFile);
		//usMaxContext		
		usTmp = hy_cdr_int16_to(2);
		fwrite(&usTmp,2,1,m_pFontFile);
		tbEntryOS2.length = ftell(m_pFontFile) - tbEntryOS2.offset;
		fourbytesAlign(tbEntryOS2.length);			
			
		Encodename();		
		Encodecmap(m_pFontFile, m_HYTbDirectory);
		HYFIXED fxVrn;
		fxVrn.value = 3;
		fxVrn.fract = 0;
		Encodepost(fxVrn);
		
		iEntryIndex = m_HYTbDirectory.FindTableEntry(HMTX_TAG);
		if (iEntryIndex == -1) return FONT_ERR_HMTX_ENCODE;
		CHYTableEntry& tbEntryHMDTX = m_HYTbDirectory.vtTableEntry[iEntryIndex];
		tbEntryHMDTX.offset = ftell(m_pFontFile);
		szGlyphNum			= m_HYMaxp.numGlyphs;
		unsigned short longhormetricNums	= m_HYHhea.numberOfHMetrics;
		unsigned short lefsidebearNums		= (unsigned short)szGlyphNum - m_HYHhea.numberOfHMetrics;
		for (size_t i=0; i<szGlyphNum; i++)
		{		
			if (i<longhormetricNums)
			{	
#if 0
				usTmp = hy_cdr_int16_to(m_HYhead.unitsPerEm);
				fwrite(&usTmp,2,1,m_pFontFile);
				usTmp = hy_cdr_int16_to(0);
				fwrite(&usTmp,2,1,m_pFontFile);

#else			// 为了适配oppo emjio字库
				if (i==0)
				{
					usTmp = hy_cdr_int16_to(800);	
					fwrite(&usTmp,2,1,m_pFontFile);
					usTmp = hy_cdr_int16_to(90);
					fwrite(&usTmp,2,1,m_pFontFile);
				}
				else 
				{
					usTmp = hy_cdr_int16_to(1050);
					fwrite(&usTmp,2,1,m_pFontFile);
					usTmp = hy_cdr_int16_to(0);
					fwrite(&usTmp,2,1,m_pFontFile);			
				}				

#endif 
				
			}
			else 
			{
				usTmp = hy_cdr_int16_to(0);
				fwrite(&usTmp,2,1,m_pFontFile);					
			}
		}
		tbEntryHMDTX.length = ftell(m_pFontFile) - tbEntryHMDTX.offset;
		fourbytesAlign(tbEntryHMDTX.length);	

		int iRt = EncodeCBDT();
		if (iRt != HY_NOERROR)	return iRt;
		iRt = EncodeCBLC();
		if(iRt != HY_NOERROR)	return iRt;		

		// 对每一个表生成校验码
		fflush(m_pFontFile);
		for (int i=0; i<m_HYTbDirectory.numTables; i++)
		{
			CHYTableEntry&	HYEntry = m_HYTbDirectory.vtTableEntry[i];
			int CheckBufSz = (HYEntry.length+3)/4*4;
			char* pCheckBuf = new char[CheckBufSz];

			fseek(m_pFontFile,HYEntry.offset,SEEK_SET);
			fread(pCheckBuf,CheckBufSz,1,m_pFontFile);

			HYEntry.checkSum = HY_CalcFontTableChecksum((unsigned long*)pCheckBuf,HYEntry.length);
			if (pCheckBuf) delete[] pCheckBuf;
		}
		fflush(m_pFontFile);
		EncodeTableDirectory();
		CloseFile();

		SetCheckSumAdjustment(pFontFile);
		
		return HY_NOERROR;

	}	// end of int	CHYFontCodec::EncodeEmojiOpentypeFont()

	int CHYFontCodec::EncodeEmojiOAppleFont(char* pFontFile, char* EmojiProfile)
	{
		return HY_ERR_BASE;

		int iRtn = CreateFile(pFontFile);	
		if (iRtn != HY_NOERROR)  return iRtn;

		// 构建目录						
		MakeBitmapGlyphs(EmojiProfile);		
		m_HYCodeMap.QuickSortbyUnicode();

		m_HYTbDirectory.vtTableEntry.clear();
		CHYTableEntry	HYEntry;	
		m_HYTbDirectory.version.value = 1;
		m_HYTbDirectory.version.fract = 0;	
		HYEntry.tag =CBDT_TAG;
		m_HYTbDirectory.vtTableEntry.push_back(HYEntry);
		HYEntry.tag =CBLC_TAG;
		m_HYTbDirectory.vtTableEntry.push_back(HYEntry);
		HYEntry.tag = OS2_TAG;
		m_HYTbDirectory.vtTableEntry.push_back(HYEntry);
		HYEntry.tag	= CMAP_TAG;
		m_HYTbDirectory.vtTableEntry.push_back(HYEntry);
		HYEntry.tag	= HEAD_TAG;
		m_HYTbDirectory.vtTableEntry.push_back(HYEntry);
		HYEntry.tag	= HHEA_TAG;
		m_HYTbDirectory.vtTableEntry.push_back(HYEntry);
		HYEntry.tag	= HMTX_TAG;
		m_HYTbDirectory.vtTableEntry.push_back(HYEntry);
		HYEntry.tag = MAXP_TAG;
		m_HYTbDirectory.vtTableEntry.push_back(HYEntry);
		HYEntry.tag	= NAME_TAG;
		m_HYTbDirectory.vtTableEntry.push_back(HYEntry);
		HYEntry.tag	= POST_TAG;
		m_HYTbDirectory.vtTableEntry.push_back(HYEntry);

		MakeCmap();
		m_HYhead.unitsPerEm = 1000;
		m_HYhead.yMin = -150;
		m_HYhead.yMax = 850;
		m_HYhead.xMax = 1000;
		m_HYhead.xMin = 0;

		unsigned short	usTmp = 0;
		unsigned long	ulTmp = 0;

		EncodeTableDirectory();		
		Encodehead();
		
		int iEntryIndex = m_HYTbDirectory.FindTableEntry(HHEA_TAG);
		if (iEntryIndex == -1) return FONT_ERR_HHEA_ENCODE;
		CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
		tbEntry.offset = ftell(m_pFontFile);
		// version 
		m_HYHhea.version.value = 1;
		usTmp = hy_cdr_int16_to(m_HYHhea.version.value);
		fwrite(&usTmp,2,1,m_pFontFile);
		m_HYHhea.version.fract = 0;
		usTmp = hy_cdr_int16_to(m_HYHhea.version.fract);
		fwrite(&usTmp,2,1,m_pFontFile);			
		//Ascender
		if (m_HYHhea.Ascender==0)
			m_HYHhea.Ascender = m_HYhead.yMax;
		usTmp = hy_cdr_int16_to(m_HYHhea.Ascender);
		fwrite(&usTmp,2,1,m_pFontFile);
		//Descender
		if (m_HYHhea.Descender==0)
			m_HYHhea.Descender = m_HYhead.yMin;			
		usTmp = hy_cdr_int16_to(m_HYHhea.Descender);
		fwrite(&usTmp,2,1,m_pFontFile);
		//LineGap			
		usTmp = hy_cdr_int16_to(m_HYHhea.LineGap);
		fwrite(&usTmp,2,1,m_pFontFile);
		//advanceWidthMax			
		usTmp =hy_cdr_int16_to(m_HYhead.unitsPerEm);
		fwrite(&usTmp,2,1,m_pFontFile);
		//minLeftSideBearing
		m_HYHhea.minLeftSideBearing = m_HYhead.xMin;
		usTmp =hy_cdr_int16_to(m_HYHhea.minLeftSideBearing);
		fwrite(&usTmp,2,1,m_pFontFile);
		//minRightSideBearing
		m_HYHhea.minRightSideBearing = m_HYhead.unitsPerEm - m_HYHhea.minLeftSideBearing-(m_HYhead.xMax-m_HYhead.xMin);
		usTmp =hy_cdr_int16_to(m_HYHhea.minRightSideBearing);
		fwrite(&usTmp,2,1,m_pFontFile);
		//xMaxExtent
		m_HYHhea.xMaxExtent = m_HYHhea.minLeftSideBearing+(m_HYhead.xMax-m_HYhead.xMin);
		usTmp =hy_cdr_int16_to(m_HYHhea.xMaxExtent);
		fwrite(&usTmp,2,1,m_pFontFile);
		//caretSlopeRise		
		usTmp = hy_cdr_int16_to(1);
		fwrite(&usTmp,2,1,m_pFontFile);
		//caretSlopeRun
		usTmp = 0;
		fwrite(&usTmp,2,1,m_pFontFile);
		//caretOffset
		usTmp = 0;
		fwrite(&usTmp,2,1,m_pFontFile);
		usTmp = 0;
		//reserved1			
		fwrite(&usTmp,2,1,m_pFontFile);
		//reserved2			
		fwrite(&usTmp,2,1,m_pFontFile);
		//reserved3			
		fwrite(&usTmp,2,1,m_pFontFile);
		//reserved4			
		fwrite(&usTmp,2,1,m_pFontFile);
		//metricDataFormat
		usTmp = 0;
		fwrite(&usTmp,2,1,m_pFontFile);
		//numberOfHMetrics			
		m_HYHhea.numberOfHMetrics = 1;
		usTmp = hy_cdr_int16_to(m_HYHhea.numberOfHMetrics);
		fwrite(&usTmp,2,1,m_pFontFile);
		tbEntry.length = ftell(m_pFontFile) - tbEntry.offset;
		::HY_4ByteAlign(m_pFontFile, tbEntry.length);		

		iEntryIndex = m_HYTbDirectory.FindTableEntry(MAXP_TAG);
		if (iEntryIndex == -1) return FONT_ERR_HMTX_ENCODE;
		tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
		tbEntry.offset = ftell(m_pFontFile);
		size_t			szGlyphNum = m_HYCodeMap.vtHYCodeMap.size();
		//Table version number
		m_HYMaxp.version.value = 1;
		usTmp = hy_cdr_int16_to(m_HYMaxp.version.value);
		fwrite(&usTmp,2,1,m_pFontFile);
		m_HYMaxp.version.fract = 0;
		fwrite(&m_HYMaxp.version.fract,2,1,m_pFontFile);
		//numGlyphs
		m_HYMaxp.numGlyphs = (unsigned short)szGlyphNum;
		usTmp = hy_cdr_int16_to(m_HYMaxp.numGlyphs);
		fwrite(&usTmp,2,1,m_pFontFile);		
		//maxPoints
		m_HYMaxp.maxPoints = 8;
		usTmp = hy_cdr_int16_to(m_HYMaxp.maxPoints);
		fwrite(&usTmp,2,1,m_pFontFile);
		//maxContours
		m_HYMaxp.maxContours = 2;
		usTmp = hy_cdr_int16_to(m_HYMaxp.maxContours);
		fwrite(&usTmp,2,1,m_pFontFile);
		//maxCompositePoints				
		usTmp = 0;
		fwrite(&usTmp,2,1,m_pFontFile);
		//maxCompositeContours
		usTmp = 0;	
		fwrite(&usTmp,2,1,m_pFontFile);
		//maxZones
		usTmp = hy_cdr_int16_to(2);
		fwrite(&usTmp,2,1,m_pFontFile);
		//maxTwilightPoints
		usTmp = hy_cdr_int16_to(4);
		fwrite(&usTmp,2,1,m_pFontFile);
		//maxStorage
		usTmp = hy_cdr_int16_to(32);
		fwrite(&usTmp,2,1,m_pFontFile);
		//maxFunctionDefs
		usTmp = hy_cdr_int16_to(96);
		fwrite(&usTmp,2,1,m_pFontFile);
		//maxInstructionDefs
		usTmp = hy_cdr_int16_to(96);
		fwrite(&usTmp,2,1,m_pFontFile);
		//maxStackElements
		usTmp = hy_cdr_int16_to(256);
		fwrite(&usTmp,2,1,m_pFontFile);
		//maxSizeOfInstructions
		usTmp = 0;
		fwrite(&usTmp,2,1,m_pFontFile);
		//maxComponentElements
		usTmp = 0;
		fwrite(&usTmp,2,1,m_pFontFile);
		//maxComponentDepth
		usTmp = 0;
		fwrite(&usTmp,2,1,m_pFontFile);
		tbEntry.length = ftell(m_pFontFile) - tbEntry.offset;
		::HY_4ByteAlign(m_pFontFile, tbEntry.length);		

		iEntryIndex = m_HYTbDirectory.FindTableEntry(OS2_TAG);
		if (iEntryIndex == -1) return FONT_ERR_OS2_ENCODE;
		tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
		tbEntry.offset = ftell(m_pFontFile);
		//version
		usTmp = hy_cdr_int16_to(0x0003);
		fwrite(&usTmp,2,1,m_pFontFile);			
		//xAvgCharWidth			
		usTmp = hy_cdr_int16_to(1000);
		fwrite(&usTmp,2,1,m_pFontFile);
		//usWeightClass
		if (m_HYOS2.usWeightClass==0)
		{
			m_HYOS2.usWeightClass=400;
		}
		usTmp = hy_cdr_int16_to(m_HYOS2.usWeightClass);			
		fwrite(&usTmp,2,1,m_pFontFile);
		//usWidthClass
		if (m_HYOS2.usWidthClass==0)
		{
			float fRatioe = (float)((m_HYhead.xMax-m_HYhead.xMin)*100)/(float)(m_HYhead.yMax-m_HYhead.yMin);
			if (fRatioe<=50.0)							m_HYOS2.usWidthClass = 1;	
			if (fRatioe>50.0&&fRatioe<=62.5)			m_HYOS2.usWidthClass = 2;
			if (fRatioe>62.5&&fRatioe<=75.0)			m_HYOS2.usWidthClass = 3;
			if (fRatioe>75.0&&fRatioe<=87.5)			m_HYOS2.usWidthClass = 4;
			if (fRatioe>87.5&&fRatioe<112.5)			m_HYOS2.usWidthClass = 5;
			if (fRatioe>=112.5&&fRatioe<125.0)			m_HYOS2.usWidthClass = 6;
			if (fRatioe>=125.0&&fRatioe<150.0)			m_HYOS2.usWidthClass = 7;
			if (fRatioe>=150.0&&fRatioe<200)			m_HYOS2.usWidthClass = 8;
			if (fRatioe>=200.0)							m_HYOS2.usWidthClass = 9;
		}
		usTmp = hy_cdr_int16_to(m_HYOS2.usWidthClass);
		fwrite(&usTmp,2,1,m_pFontFile);
		//fsType
		if (m_HYOS2.fsType==0)
		{
			m_HYOS2.fsType = 0x0008;
		}
		usTmp = hy_cdr_int16_to(m_HYOS2.fsType);
		fwrite(&usTmp,2,1,m_pFontFile);			
		//ySubscriptXSize
		if (m_HYOS2.ySubscriptXSize==0)
		{
			m_HYOS2.ySubscriptXSize = m_HYhead.unitsPerEm/10;
		}
		usTmp = hy_cdr_int16_to(m_HYOS2.ySubscriptXSize);
		fwrite(&usTmp,2,1,m_pFontFile);
		//ySubscriptYSize
		if (m_HYOS2.ySubscriptYSize==0)
		{
			m_HYOS2.ySubscriptYSize = m_HYhead.unitsPerEm/10;
		}
		usTmp = hy_cdr_int16_to(m_HYOS2.ySubscriptYSize);
		fwrite(&usTmp,2,1,m_pFontFile);
		//ySubscriptXOffset			
		usTmp = hy_cdr_int16_to(m_HYOS2.ySubscriptXOffset);
		fwrite(&usTmp,2,1,m_pFontFile);
		//ySubscriptYOffset			
		usTmp = hy_cdr_int16_to(m_HYOS2.ySubscriptYOffset);
		fwrite(&usTmp,2,1,m_pFontFile);
		//ySuperscriptXSize
		if (m_HYOS2.ySuperscriptXSize==0)
		{
			m_HYOS2.ySuperscriptXSize = m_HYhead.unitsPerEm/10;
		}
		usTmp = hy_cdr_int16_to(m_HYOS2.ySuperscriptXSize);
		fwrite(&usTmp,2,1,m_pFontFile);
		//ySuperscriptYSize
		if (m_HYOS2.ySuperscriptYSize==0)
		{
			m_HYOS2.ySuperscriptYSize = m_HYhead.unitsPerEm/10;
		}
		usTmp = hy_cdr_int16_to(m_HYOS2.ySuperscriptYSize);
		fwrite(&usTmp,2,1,m_pFontFile);
		//ySuperscriptXOffset
		usTmp = hy_cdr_int16_to(m_HYOS2.ySuperscriptXOffset);
		fwrite(&usTmp,2,1,m_pFontFile);
		//ySuperscriptYOffset
		usTmp = hy_cdr_int16_to(m_HYOS2.ySuperscriptYOffset);
		fwrite(&usTmp,2,1,m_pFontFile);
		//yStrikeoutSize
		if (m_HYOS2.yStrikeoutSize==0)
		{
			m_HYOS2.yStrikeoutSize = m_HYhead.unitsPerEm/20;
		}
		usTmp = hy_cdr_int16_to(m_HYOS2.yStrikeoutSize);
		fwrite(&usTmp,2,1,m_pFontFile);
		//yStrikeoutPosition
		if (m_HYOS2.yStrikeoutPosition==0)
		{
			m_HYOS2.yStrikeoutPosition = m_HYhead.unitsPerEm/5;
		}
		usTmp = hy_cdr_int16_to(m_HYOS2.yStrikeoutPosition);
		fwrite(&usTmp,2,1,m_pFontFile);
		//sFamilyClass
		usTmp = hy_cdr_int16_to(m_HYOS2.sFamilyClass);
		fwrite(&usTmp,2,1,m_pFontFile);
		//Panose			
		fwrite(&m_HYOS2.panose.FamilyType,1,1,m_pFontFile);
		fwrite(&m_HYOS2.panose.SerifStyle,1,1,m_pFontFile);
		fwrite(&m_HYOS2.panose.Weight,1,1,m_pFontFile);
		fwrite(&m_HYOS2.panose.Proportion,1,1,m_pFontFile);
		fwrite(&m_HYOS2.panose.Contrast,1,1,m_pFontFile);
		fwrite(&m_HYOS2.panose.StrokeVariation,1,1,m_pFontFile);
		fwrite(&m_HYOS2.panose.ArmStyle,1,1,m_pFontFile);
		fwrite(&m_HYOS2.panose.Letterform,1,1,m_pFontFile);
		fwrite(&m_HYOS2.panose.Midline,1,1,m_pFontFile);
		fwrite(&m_HYOS2.panose.XHeight,1,1,m_pFontFile);
		m_HYOS2.ulUnicodeRange1 = 0;
		m_HYOS2.ulUnicodeRange2 = 0;
		m_HYOS2.ulUnicodeRange3 = 0;
		m_HYOS2.ulUnicodeRange4 = 0;
		CountUnicodeRange(m_HYOS2.ulUnicodeRange1,m_HYOS2.ulUnicodeRange2,m_HYOS2.ulUnicodeRange3,m_HYOS2.ulUnicodeRange4);
		//ulUnicodeRange1 
		ulTmp = hy_cdr_int32_to(m_HYOS2.ulUnicodeRange1);
		fwrite(&ulTmp,4,1,m_pFontFile);
		//ulUnicodeRange2 	
		ulTmp = hy_cdr_int32_to(m_HYOS2.ulUnicodeRange2);
		fwrite(&ulTmp,4,1,m_pFontFile);
		//ulUnicodeRange3 	
		ulTmp = hy_cdr_int32_to(m_HYOS2.ulUnicodeRange3);
		fwrite(&ulTmp,4,1,m_pFontFile);
		//ulUnicodeRange4
		ulTmp = hy_cdr_int32_to(m_HYOS2.ulUnicodeRange4);
		fwrite(&ulTmp,4,1,m_pFontFile);			
		//achVendID
		fwrite(&m_HYOS2.vtachVendID[0],1,1,m_pFontFile);
		fwrite(&m_HYOS2.vtachVendID[1],1,1,m_pFontFile);
		fwrite(&m_HYOS2.vtachVendID[2],1,1,m_pFontFile);
		fwrite(&m_HYOS2.vtachVendID[3],1,1,m_pFontFile);			
		//fsSelection
		usTmp = hy_cdr_int16_to(m_HYOS2.fsSelection);
		fwrite(&usTmp,2,1,m_pFontFile);
		//usFirstCharIndex
		m_HYOS2.usFirstCharIndex =	(unsigned short)m_HYCodeMap.vtHYCodeMap[0].ulGlyphNo;
		usTmp = hy_cdr_int16_to(m_HYOS2.usFirstCharIndex);
		fwrite(&usTmp,2,1,m_pFontFile);
		//usLastCharIndex
		if (m_HYCodeMap.vtHYCodeMap.size()>1)
		{
			unsigned long uluni = m_HYCodeMap.vtHYCodeMap[m_HYCodeMap.vtHYCodeMap.size()-2].ulGlyphNo;			
			if (uluni>0xffff)
			{
				m_HYOS2.usLastCharIndex = 0xffff;
			}
			else 
			{
				m_HYOS2.usLastCharIndex = (unsigned short)uluni;
			}
			usTmp = hy_cdr_int16_to(m_HYOS2.usLastCharIndex);
			fwrite(&usTmp,2,1,m_pFontFile);
		}
		else 
		{
			usTmp = 0xffff;
			fwrite(&usTmp,2,1,m_pFontFile);
		}
		//sTypoAscender
		if (m_HYOS2.sTypoAscender==0)
		{
			m_HYOS2.sTypoAscender = m_HYhead.yMax;
		}			
		usTmp = hy_cdr_int16_to(m_HYOS2.sTypoAscender);			
		fwrite(&usTmp,2,1,m_pFontFile);
		//sTypoDescender
		if (m_HYOS2.sTypoDescender==0)
		{			
			m_HYOS2.sTypoDescender = m_HYhead.yMin;
		}			
		usTmp = hy_cdr_int16_to(m_HYOS2.sTypoDescender);			
		fwrite(&usTmp,2,1,m_pFontFile);
		//sTypoLineGap			
		usTmp = hy_cdr_int16_to(m_HYOS2.sTypoLineGap);			
		fwrite(&usTmp,2,1,m_pFontFile);
		//usWinAscent
		if (m_HYOS2.usWinAscent==0)
		{
			m_HYOS2.usWinAscent = m_HYhead.yMax;
		}			
		usTmp = hy_cdr_int16_to(m_HYOS2.usWinAscent);			
		fwrite(&usTmp,2,1,m_pFontFile);
		//usWinDescent
		if (m_HYOS2.usWinDescent==0)
		{
			m_HYOS2.usWinDescent = abs(m_HYhead.yMin);
		}			
		usTmp = hy_cdr_int16_to(m_HYOS2.usWinDescent);			
		fwrite(&usTmp,2,1,m_pFontFile);
		//ulCodePageRange1 
		ulTmp = hy_cdr_int32_to(m_HYOS2.ulCodePageRange1);
		fwrite(&ulTmp,4,1,m_pFontFile);
		//ulCodePageRange2 
		ulTmp = hy_cdr_int32_to(m_HYOS2.ulCodePageRange2);
		fwrite(&ulTmp,4,1,m_pFontFile);
		//sxHeight		
		usTmp = 0;
		fwrite(&usTmp,2,1,m_pFontFile);
		//sCapHeight			
		usTmp = 0;
		fwrite(&usTmp,2,1,m_pFontFile);
		//usDefaultChar		
		usTmp = 0;
		fwrite(&usTmp,2,1,m_pFontFile);
		//usBreakChar
		usTmp = 0x2000;
		fwrite(&usTmp,2,1,m_pFontFile);
		//usMaxContext
		usTmp = 256;
		fwrite(&usTmp,2,1,m_pFontFile);
		tbEntry.length = ftell(m_pFontFile) - tbEntry.offset;
		::HY_4ByteAlign(m_pFontFile, tbEntry.length);

		Encodename();		
		Encodecmap(m_pFontFile, m_HYTbDirectory);

		HYFIXED fxVersion;
		fxVersion.value = 3;
		fxVersion.fract = 0;
		Encodepost(fxVersion);

		iEntryIndex = m_HYTbDirectory.FindTableEntry(HMTX_TAG);
		if (iEntryIndex == -1) return FONT_ERR_HMTX_ENCODE;
		tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
		tbEntry.offset = ftell(m_pFontFile);
		szGlyphNum			= m_HYCodeMap.vtHYCodeMap.size();
		unsigned short longhormetricNums	= m_HYHhea.numberOfHMetrics;
		unsigned short lefsidebearNums		= (unsigned short)szGlyphNum - m_HYHhea.numberOfHMetrics;
		for (size_t i=0; i<szGlyphNum; i++)
		{		
			if (i<longhormetricNums)
			{	
				usTmp = hy_cdr_int16_to(m_HYhead.unitsPerEm);
				fwrite(&usTmp,2,1,m_pFontFile);
				usTmp = hy_cdr_int16_to(0);
				fwrite(&usTmp,2,1,m_pFontFile);				
			}
			else 
			{
				usTmp = hy_cdr_int16_to(0);
				fwrite(&usTmp,2,1,m_pFontFile);					
			}
		}
		tbEntry.length = ftell(m_pFontFile) - tbEntry.offset;
		::HY_4ByteAlign(m_pFontFile, tbEntry.length);

		int iRt = EncodeCBDT();
		if (iRt != HY_NOERROR)	return iRt;
		iRt = EncodeCBLC();
		if(iRt != HY_NOERROR)	return iRt;		

		// 对每一个表生成校验码
		fflush(m_pFontFile);
		for (int i=0; i<m_HYTbDirectory.numTables; i++)
		{
			CHYTableEntry&	HYEntry = m_HYTbDirectory.vtTableEntry[i];
			int CheckBufSz = (HYEntry.length+3)/4*4;
			char* pCheckBuf = new char[CheckBufSz];

			fseek(m_pFontFile,HYEntry.offset,SEEK_SET);
			fread(pCheckBuf,CheckBufSz,1,m_pFontFile);

			HYEntry.checkSum = HY_CalcFontTableChecksum((unsigned long*)pCheckBuf,HYEntry.length);
			if (pCheckBuf) delete[] pCheckBuf;
		}
		fflush(m_pFontFile);
		EncodeTableDirectory();
		CloseFile();

		SetCheckSumAdjustment(pFontFile);

		return HY_NOERROR;

	}	// end of int CHYFontCodec::EncodeEmojiOAppleFont()
#endif 
	int	CHYFontCodec::fourbytesAlign(unsigned long Entrylength)
	{
		int iTail = 4-Entrylength%4;
		if (iTail>0)
		{
			char c = 0;
			for (int i=0; i<iTail; i++)
			{
				fwrite(&c,1,1,m_pFontFile);
			}				
		}

		return HY_NOERROR;

	}	// end of int	CHYFontCodec::fourbytesAlign()

	void CHYFontCodec::CountGlyphBox(unsigned int iGid, std::vector<CHYGlyph>&	vtHYGlyphs)
	{		
		if (iGid >= (unsigned int)vtHYGlyphs.size()) return; 

		CHYGlyph& TgGlyph = vtHYGlyphs[iGid];	

		if (TgGlyph.glyfType == GLYF_TYPE_COMPOSITE)
		{				
			// 针对组合轮廓转换
			SHORT sValue = 0;
			float fXScale = 1.0f, fYScale = 1.0f, fValue = 0.0f, fFraction = 0.0f;			
			size_t szCmpnnts =TgGlyph.vtComponents.size();

			for (size_t j=0; j<szCmpnnts; j++)
			{				
				fXScale = 1.0f; 
				fYScale = 1.0f;				
				CHYCmpst& hyCmpst = TgGlyph.vtComponents[j];

				USHORT sGlyphIndex  = hyCmpst.glyphIndex;
				if (hyCmpst.flags&GLYF_CMPST_ARGS_ARE_XY_VALUES)
				{
					if (hyCmpst.flags&GLYF_CMPST_WE_HAVE_A_SCALE)
					{
						fXScale = ::HY_F2DOT14_to_float(hyCmpst.scale);
						fYScale = fXScale;
					}

					if (hyCmpst.flags&GLYF_CMPST_WE_HAVE_AN_X_AND_Y_SCALE)
					{
						fXScale = ::HY_F2DOT14_to_float(hyCmpst.vctr.xscale);
						fYScale = ::HY_F2DOT14_to_float(hyCmpst.vctr.yscale);
					}
				}
							
				// 计算路径				
				if (sGlyphIndex>=vtHYGlyphs.size()) continue;
				if (sGlyphIndex<0)	continue;

				CHYGlyph&	cmpGlyph = vtHYGlyphs[sGlyphIndex];

				float xcmpMin = cmpGlyph.minX;
				float xcmpMax = cmpGlyph.maxX;
				float YcmpMin = cmpGlyph.minY;
				float YcmpMax = cmpGlyph.maxX;

				// 先缩放
				float XminTmp = cmpGlyph.minX*fXScale;
				float XmaxTmp = cmpGlyph.maxX*fXScale;
				float YminTmp = cmpGlyph.minY*fYScale;
				float YmaxTmp = cmpGlyph.maxY*fYScale;

				// 后偏移
				xcmpMin = XminTmp + hyCmpst.args[0];
				xcmpMax = XmaxTmp + hyCmpst.args[0];
				YcmpMin = YminTmp + hyCmpst.args[1];
				YcmpMax = YmaxTmp + hyCmpst.args[1];													
					
				if (j==0)
				{
					TgGlyph.minX = HY_RealRount(xcmpMin);
					TgGlyph.maxX = HY_RealRount(xcmpMax);
					TgGlyph.minY = HY_RealRount(YcmpMin);
					TgGlyph.maxY = HY_RealRount(YcmpMax);
				}
				else 
				{
					if (TgGlyph.minX > HY_RealRount(xcmpMin)) TgGlyph.minX = HY_RealRount(xcmpMin);
					if (TgGlyph.maxX < HY_RealRount(xcmpMax)) TgGlyph.maxX = HY_RealRount(xcmpMax);
					if (TgGlyph.minY > HY_RealRount(YcmpMin)) TgGlyph.minY = HY_RealRount(YcmpMin);
					if (TgGlyph.maxY < HY_RealRount(YcmpMax)) TgGlyph.maxY = HY_RealRount(YcmpMax);
				}								
			}
		}
		else 
		{			
			TgGlyph.CountBoundBox();			
		}

	}	// end of void CHYFontCodec::CountGlyphBox()

	bool CHYFontCodec::FindFlag(std::vector<unsigned long>vtFlag, unsigned long  FindFlag)
	{
		size_t st= vtFlag.size();
		for(int i=0; i<st; i++)
		{
			if (vtFlag[i] == FindFlag)
				return true;
		}

		return  false;

	}	// end of bool CHYFontCodec::FindFlag()

	bool CHYFontCodec::FindFlag(unsigned long  FindFlag)
	{
		return m_HYTbDirectory.FindTableEntry(FindFlag)==-1?false:true;		

	}	// end of bool CHYFontCodec::FindFlag()

	void CHYFontCodec::RemoveRepeatpoint()
	{
		size_t stGlyph = m_vtHYGlyphs.size();
		for (size_t i = 0; i < stGlyph; i++) {
			CHYGlyph& hyGlyph = m_vtHYGlyphs[i];
			size_t stCntcur = hyGlyph.vtContour.size();
			for (size_t j = 0; j < stCntcur; j++) {
				CHYContour& Cntur = hyGlyph.vtContour[j];
				size_t stPnts = Cntur.vtHYPoints.size();
				CHYPoint Pnt1 = Cntur.vtHYPoints[0];
				CHYPoint Pnt2 = Cntur.vtHYPoints[stPnts - 2];
				CHYPoint Pnt3 = Cntur.vtHYPoints[stPnts - 1];

				if (Pnt1.x == Pnt3.x &&
					Pnt1.y == Pnt3.y &&
					Pnt1.flag == Pnt3.flag) {
					if (Pnt2.flag == 1)
						Cntur.vtHYPoints.pop_back();
				}
			}
		}

	}	// end of void CHYFontCodec::RemoveRepeatpoint()

	void CHYFontCodec::TailAddRepeatpoint()
	{
		
	
	}	// end of void CHYFontCodec::TailAddRepeatpoint()

	void CHYFontCodec::GetFontHist(std::vector<int>& vtHist, int iDirct)
	{
		int xOffset = m_HYhead.xMin<0 ? abs(m_HYhead.xMin) : 0;
		int yOffset = m_HYhead.yMin<0 ? abs(m_HYhead.yMin) : 0;

		// 沿X轴方向求Y的值。
		if (iDirct == 0)
		{
			vtHist.resize(m_HYhead.xMax - m_HYhead.xMin);
			for (int i = 0; i < m_HYMaxp.numGlyphs; i++)
			{
				CHYGlyph& glyph = m_vtHYGlyphs[i];
				for (size_t j = 0; j < glyph.vtContour.size(); j++)
				{
					CHYContour& Contour = glyph.vtContour[j];
					for (size_t k = 0; k < Contour.vtHYPoints.size(); k++)
					{
						CHYPoint& hypt = Contour.vtHYPoints[k];

						int index = hypt.x + xOffset;
						vtHist[index] += hypt.y + yOffset;
					}
				}
			}
		}
		else if (iDirct == 1)
		{
			vtHist.resize(m_HYhead.yMax - m_HYhead.yMin);

			for (int i = 0; i < m_HYMaxp.numGlyphs; i++)
			{
				CHYGlyph& glyph = m_vtHYGlyphs[i];
				for (size_t j = 0; j < glyph.vtContour.size(); j++)
				{
					CHYContour& Contour = glyph.vtContour[j];
					for (size_t k = 0; k < Contour.vtHYPoints.size(); k++)
					{
						CHYPoint& hypt = Contour.vtHYPoints[k];

						int index = hypt.y + yOffset;
						vtHist[index] += hypt.x + xOffset;
					}
				}
			}
		}

	}	// end of int CFontExtract::GetFontHist()

	int	CHYFontCodec::MakeTableDirectory(std::vector<unsigned long> vtFlag)
	{
		m_HYTbDirectory.vtTableEntry.clear();

		CHYTableEntry	HYEntry;	
		if (m_iFontType==FONTTYPE_TTF){
			m_HYTbDirectory.version.value = 1;
			m_HYTbDirectory.version.fract = 0;
		}
		else if (m_iFontType==FONTTYPE_OTF){
			m_HYTbDirectory.version.value = 0x4f54;
			m_HYTbDirectory.version.fract = 0x544f;
		}

		//BASE 
		if (FindFlag(vtFlag,BASE_TAG)){
			HYEntry.tag	= BASE_TAG;
			m_HYTbDirectory.vtTableEntry.push_back(HYEntry);
		}
		//CFF
		if (m_iFontType==FONTTYPE_OTF){
			HYEntry.tag	= CFF_TAG;
			m_HYTbDirectory.vtTableEntry.push_back(HYEntry);
		}
		//CBDT
		if (FindFlag(vtFlag, CBDT_TAG)) {
			HYEntry.tag = CBDT_TAG;
			m_HYTbDirectory.vtTableEntry.push_back(HYEntry);
		}
		//CBLC
		if (FindFlag(vtFlag, CBLC_TAG)) {
			HYEntry.tag = CBLC_TAG;
			m_HYTbDirectory.vtTableEntry.push_back(HYEntry);
		}
		//COLR
		if (FindFlag(vtFlag,COLR_TAG)){
			HYEntry.tag = COLR_TAG;
			m_HYTbDirectory.vtTableEntry.push_back(HYEntry);
		}
		//EBDT
		if (FindFlag(vtFlag, EBDT_TAG)) {
			HYEntry.tag = EBDT_TAG;
			m_HYTbDirectory.vtTableEntry.push_back(HYEntry);
		}
		//EBLC
		if (FindFlag(vtFlag, EBLC_TAG)) {
			HYEntry.tag = EBLC_TAG;
			m_HYTbDirectory.vtTableEntry.push_back(HYEntry);
		}
		//DSIG
		if (FindFlag(vtFlag,DSIG_TAG)){
			HYEntry.tag	= DSIG_TAG;
			m_HYTbDirectory.vtTableEntry.push_back(HYEntry);
		}
		//GDEF
		if (FindFlag(vtFlag,GDEF_TAG)){
			HYEntry.tag	= GDEF_TAG;
			m_HYTbDirectory.vtTableEntry.push_back(HYEntry);
		}
		//GPOS
		if (FindFlag(vtFlag,GPOS_TAG)){
			HYEntry.tag	= GPOS_TAG;
			m_HYTbDirectory.vtTableEntry.push_back(HYEntry);
		}
		//GSUB
		if (FindFlag(vtFlag,GSUB_TAG)){
			HYEntry.tag	= GSUB_TAG;
			m_HYTbDirectory.vtTableEntry.push_back(HYEntry);
		}
		//OS2
		if (FindFlag(vtFlag,OS2_TAG)){
			HYEntry.tag = OS2_TAG;
			m_HYTbDirectory.vtTableEntry.push_back(HYEntry);
		}
		//VORG
		if (m_iFontType==FONTTYPE_OTF){
			HYEntry.tag	= VORG_TAG;
			m_HYTbDirectory.vtTableEntry.push_back(HYEntry);
		}
		//CMAP
		if (FindFlag(vtFlag,CMAP_TAG)){
			HYEntry.tag	= CMAP_TAG;
			m_HYTbDirectory.vtTableEntry.push_back(HYEntry);
		}
		//CVT
		if (FindFlag(vtFlag,CVT_TAG)){
			HYEntry.tag	= CVT_TAG;
			m_HYTbDirectory.vtTableEntry.push_back(HYEntry);
		}
		//frgm
		if (FindFlag(vtFlag,FPGM_TAG)){
			HYEntry.tag	= FPGM_TAG;
			m_HYTbDirectory.vtTableEntry.push_back(HYEntry);
		}
		//fvar
		if (FindFlag(vtFlag,FVAR_TAG)){
			HYEntry.tag	= FVAR_TAG;
			m_HYTbDirectory.vtTableEntry.push_back(HYEntry);
		}
		//GASP
		if (FindFlag(vtFlag,GASP_TAG)){
			HYEntry.tag = GASP_TAG;
			m_HYTbDirectory.vtTableEntry.push_back(HYEntry);
		}
		//GLYF
		if (m_iFontType==FONTTYPE_TTF){
			HYEntry.tag	= GLYF_TAG;
			m_HYTbDirectory.vtTableEntry.push_back(HYEntry);
		}

		//GVAR
		if (FindFlag(vtFlag,GVAR_TAG)){
			HYEntry.tag = GVAR_TAG;
			m_HYTbDirectory.vtTableEntry.push_back(HYEntry);
		}

		//Head
		if (FindFlag(vtFlag,HEAD_TAG)){
			HYEntry.tag	= HEAD_TAG;
			m_HYTbDirectory.vtTableEntry.push_back(HYEntry);			
		}
		//HHEA
		if (FindFlag(vtFlag,HHEA_TAG)){
			HYEntry.tag	= HHEA_TAG;
			m_HYTbDirectory.vtTableEntry.push_back(HYEntry);
		}
		//HMTX
		if (FindFlag(vtFlag,HMTX_TAG)){
			HYEntry.tag	= HMTX_TAG;
			m_HYTbDirectory.vtTableEntry.push_back(HYEntry);
		}
		//HVAR
		if (FindFlag(vtFlag,HVAR_TAG)){
			HYEntry.tag	= HVAR_TAG;
			m_HYTbDirectory.vtTableEntry.push_back(HYEntry);
		}
		//LOCA
		if (m_iFontType==FONTTYPE_TTF){
			HYEntry.tag	= LOCA_TAG;
			m_HYTbDirectory.vtTableEntry.push_back(HYEntry);				
		}
		//MAXP
		if (FindFlag(vtFlag,MAXP_TAG)){
			HYEntry.tag = MAXP_TAG;
			m_HYTbDirectory.vtTableEntry.push_back(HYEntry);		
		}
		//NAME
		if (FindFlag(vtFlag,NAME_TAG)){
			HYEntry.tag	= NAME_TAG;
			m_HYTbDirectory.vtTableEntry.push_back(HYEntry);
		}
		//POST
		if (FindFlag(vtFlag,POST_TAG)){
			HYEntry.tag	= POST_TAG;
			m_HYTbDirectory.vtTableEntry.push_back(HYEntry);
		}
		//PREP
		if (FindFlag(vtFlag,PREP_TAG)){
			HYEntry.tag	= PREP_TAG;
			m_HYTbDirectory.vtTableEntry.push_back(HYEntry);
		}
		//STAT
		if (FindFlag(vtFlag,STAT_TAG)){
			HYEntry.tag	= STAT_TAG;
			m_HYTbDirectory.vtTableEntry.push_back(HYEntry);
		}

		//VHEA
		if (FindFlag(vtFlag,VHEA_TAG)){
			HYEntry.tag =VHEA_TAG;
			m_HYTbDirectory.vtTableEntry.push_back(HYEntry);
		}
		//VMTX
		if (FindFlag(vtFlag,VMTX_TAG)){
			HYEntry.tag = VMTX_TAG;
			m_HYTbDirectory.vtTableEntry.push_back(HYEntry);
		}
		
		return HY_NOERROR;

	}	// end of int	CHYFontCodec::MakeTableDirectory()

	int	CHYFontCodec::MakeHYCodeMap(std::vector<CHYGlyph>& vtHYGlyphs)
	{
		m_HYCodeMap.vtHYCodeMap.clear();

		CHYCodeMapItem  mapItm;
		mapItm.ulGlyphNo = 0xffff;
		mapItm.iGlyphIndex = 0;
		m_HYCodeMap.vtHYCodeMap.push_back(mapItm);

		size_t	stGlyphNum = vtHYGlyphs.size();		
		for (size_t i=1; i<stGlyphNum; i++){		
			std::vector<unsigned  long>& vtUnicode = vtHYGlyphs[i].vtUnicode;
			size_t stUnicodeNum = vtUnicode.size();

			for (size_t j=0; j<stUnicodeNum; j++){			
				CHYCodeMapItem  mapItm;
				mapItm.ulGlyphNo = vtUnicode[j];
				mapItm.iGlyphIndex = i;
				m_HYCodeMap.vtHYCodeMap.push_back(mapItm);				
			}				
		}

		// 异体字
		if (m_tagOption.bYitizi){			
			MixVariantcharacters();
		}
		//兼容新旧unicode标准
		MixCompatcharacters();
		//为了JF互换
		//MixJFCharacters();

		if (m_tagOption.bKangXi){
			MixKangxiChars();
		}	

		m_HYCodeMap.QuickSortbyUnicode();

		return HY_NOERROR;

	}	// end of int CHYFontCodec::MakeHYCodeMap()

	void CHYFontCodec::MixKangxiChars()
	{
		size_t stHYCodemap = m_HYCodeMap.vtHYCodeMap.size();
		for (size_t i = 0; i < stHYCodemap; i++)
		{
			unsigned long MapCode = m_HYCodeMap.vtHYCodeMap[i].ulGlyphNo;
			int			  iGlyphIndex = m_HYCodeMap.vtHYCodeMap[i].iGlyphIndex;

			typedef multimap<unsigned long, unsigned long>::const_iterator VCharRcIter;
			VCharRcIter	beg = m_mpKangxiChar.lower_bound(MapCode);
			VCharRcIter	end = m_mpKangxiChar.upper_bound(MapCode);

			while (beg != end)
			{
				unsigned long ulCodeValue = beg->second;
				if (m_HYCodeMap.FindIndexByUnicode(ulCodeValue) == -1)
				{
					CHYCodeMapItem item;
					item.iGlyphIndex = iGlyphIndex;
					item.ulGlyphNo = ulCodeValue;
					m_HYCodeMap.vtHYCodeMap.push_back(item);
				}
				++beg;
			}
		}
	
	}	// end of void CHYFontCodec::MixKangxiChars()

	void CHYFontCodec::MixVariantcharacters()
	{
		size_t stHYCodemap = m_HYCodeMap.vtHYCodeMap.size();
		for (size_t i=0; i<stHYCodemap; i++)
		{	
			unsigned long MapCode = m_HYCodeMap.vtHYCodeMap[i].ulGlyphNo;
			int			  iGlyphIndex = m_HYCodeMap.vtHYCodeMap[i].iGlyphIndex;

			typedef multimap<unsigned long, unsigned long>::const_iterator VCharRcIter;
			VCharRcIter	beg = m_mpVariantChar.lower_bound(MapCode);
			VCharRcIter	end = m_mpVariantChar.upper_bound(MapCode);

			while(beg != end)
			{
				unsigned long ulCodeValue = beg->second;
				if (m_HYCodeMap.FindIndexByUnicode(ulCodeValue) ==-1)
				{
					CHYCodeMapItem item;
					item.iGlyphIndex =iGlyphIndex;
					item.ulGlyphNo = ulCodeValue;
					m_HYCodeMap.vtHYCodeMap.push_back(item);
				}
				++beg;
			}
		}		

	}	// end of void CHYFontCodec::MixVariantcharacters()

	void CHYFontCodec::MixJFCharacters()
	{
		size_t stHYCodemap = m_HYCodeMap.vtHYCodeMap.size();
		for (size_t i = 0; i<stHYCodemap; i++)
		{
			unsigned long MapCode = m_HYCodeMap.vtHYCodeMap[i].ulGlyphNo;
			int			  iGlyphIndex = m_HYCodeMap.vtHYCodeMap[i].iGlyphIndex;

			typedef multimap<unsigned long, unsigned long>::const_iterator VCharRcIter;
			VCharRcIter	beg = m_mpJ2FChar.lower_bound(MapCode);
			VCharRcIter	end = m_mpJ2FChar.upper_bound(MapCode);

			while (beg != end)
			{
				unsigned long ulCodeValue = beg->second;
				if (m_HYCodeMap.FindIndexByUnicode(ulCodeValue) == -1)
				{
					CHYCodeMapItem item;
					item.iGlyphIndex = iGlyphIndex;
					item.ulGlyphNo = ulCodeValue;
					m_HYCodeMap.vtHYCodeMap.push_back(item);
				}
				++beg;
			}
		}
	
	}	// end of void CHYFontCodec::MixJFCharacters()

	void CHYFontCodec::MixCompatcharacters()
	{
		size_t stHYCodemap = m_HYCodeMap.vtHYCodeMap.size();
		for (size_t i = 0; i < stHYCodemap; i++)
		{
			unsigned long MapCode = m_HYCodeMap.vtHYCodeMap[i].ulGlyphNo;
			int			  iGlyphIndex = m_HYCodeMap.vtHYCodeMap[i].iGlyphIndex;

			typedef multimap<unsigned long, unsigned long>::const_iterator VCharRcIter;
			VCharRcIter	beg = m_mpCompatChar.lower_bound(MapCode);
			VCharRcIter	end = m_mpCompatChar.upper_bound(MapCode);

			while (beg != end)
			{
				unsigned long ulCodeValue = beg->second;
				if (m_HYCodeMap.FindIndexByUnicode(ulCodeValue) == -1)
				{
					CHYCodeMapItem item;
					item.iGlyphIndex = iGlyphIndex;
					item.ulGlyphNo = ulCodeValue;
					m_HYCodeMap.vtHYCodeMap.push_back(item);
				}
				++beg;
			}
		}	
	
	}	// end of void CHYFontCodec::MixCompatcharacters()

	void CHYFontCodec::SetCmapEntry(std::vector<CMAP_TABLE_ENTRY>& vtEntry)
	{
		m_HYCmap.SetDefault();

		size_t stEntry = vtEntry.size();
		for (size_t i = 0; i < stEntry; i++){
			CMAP_TABLE_ENTRY&	entry = vtEntry[i];
			m_HYCmap.vtCamp_tb_entry.push_back(entry);
		}
		m_HYCmap.version = 0;
		m_HYCmap.numSubTable = (unsigned short)m_HYCmap.vtCamp_tb_entry.size();
	
	}	// end of int CHYFontCodec::SetCmapEntry()

	void CHYFontCodec::MakeCmap()
	{
		m_HYCmap.SetDefault();

		CMAP_TABLE_ENTRY	entry;
		entry.plat_ID		= 0;
		entry.plat_encod_ID = 3;
		entry.format		= CMAP_ENCODE_FT_4;
		m_HYCmap.vtCamp_tb_entry.push_back(entry);

 		entry.plat_ID		= 0;
 		entry.plat_encod_ID = 4;
 		entry.format		= CMAP_ENCODE_FT_12;
 		m_HYCmap.vtCamp_tb_entry.push_back(entry);

		entry.plat_ID		= 3;
	 	entry.plat_encod_ID = 1;
 		entry.format		= CMAP_ENCODE_FT_4;
 		m_HYCmap.vtCamp_tb_entry.push_back(entry);

  		entry.plat_ID		= 3;
  		entry.plat_encod_ID = 10;
  		entry.format		= CMAP_ENCODE_FT_12;
  		m_HYCmap.vtCamp_tb_entry.push_back(entry);

		m_HYCmap.version = 0;
		m_HYCmap.numSubTable = (unsigned short)m_HYCmap.vtCamp_tb_entry.size();

	}	// end of  int	CHYFontCodec::MakeCmap()

	int	CHYFontCodec::MakeBitmapGlyphs(char* pProfile)
	{
		/*
		m_HYCodeMap.vtHYCodeMap.clear();

		CHYCodeMapItem	MapItem;
		MapItem.iGlyphIndex = 0;
		MapItem.ulGlyphNo = 0x00000000;
		m_HYCodeMap.vtHYCodeMap.push_back(MapItem);
		MapItem.iGlyphIndex = 1;
		MapItem.ulGlyphNo = 0x0000000d;
		m_HYCodeMap.vtHYCodeMap.push_back(MapItem);
		MapItem.iGlyphIndex = 2;
		MapItem.ulGlyphNo = 0x00000020;
		m_HYCodeMap.vtHYCodeMap.push_back(MapItem);

		CMarkup		MarkUp;
		std::string strImageDir;
		if (MarkUp.Load(pProfile))
		{
			MarkUp.ResetMainPos();
			if (MarkUp.FindElem("Emoji"))
			{
				std::string strFontType = MarkUp.GetChildAttrib("type");
				int FontType = atoi(strFontType.data());				

				MarkUp.IntoElem();
				if (MarkUp.FindElem("head"))
				{
					if (MarkUp.FindChildElem("ImageFileDir"))
					{
						strImageDir = MarkUp.GetChildData();
					}
				}

				std::string		strTmp;
				if (FontType == 0)
				{
					if (MarkUp.FindElem("OpenType"))
					{
						m_HYCblc.SetDefault();
						m_HYCBDT.SetDefault();

						MarkUp.IntoElem();
						// 默认是v2.0
						m_HYCblc.Header.version.value = 2;
						m_HYCblc.Header.version.fract = 0;
						// load Scriptlist
						while(MarkUp.FindElem("bitmapSizeTable"))
						{
							m_HYCblc.Header.numsizes++;
							BitmapSize	bitMapsizeTable;
							MarkUp.IntoElem();
							if (MarkUp.FindElem("sbitLineMetrics_hori"))
							{	
								MarkUp.FindChildElem("ascender");
								strTmp = MarkUp.GetChildData();
								bitMapsizeTable.Hori.ascender = atoi(strTmp.data());
								MarkUp.FindChildElem("descender");
								strTmp = MarkUp.GetChildData();
								bitMapsizeTable.Hori.descender = atoi(strTmp.data());
								MarkUp.FindChildElem("widthMax");
								strTmp = MarkUp.GetChildData();
								bitMapsizeTable.Hori.widthMax = atoi(strTmp.data());
								MarkUp.FindChildElem("caretSlopeNumerator");
								strTmp = MarkUp.GetChildData();
								bitMapsizeTable.Hori.caretSlopeNumerator = atoi(strTmp.data());
								MarkUp.FindChildElem("caretSlopeDenominator");
								strTmp = MarkUp.GetChildData();
								bitMapsizeTable.Hori.caretSlopeDenominator = atoi(strTmp.data());
								MarkUp.FindChildElem("caretOffset");
								strTmp = MarkUp.GetChildData();
								bitMapsizeTable.Hori.caretOffset = atoi(strTmp.data());
								MarkUp.FindChildElem("minOriginSB");
								strTmp = MarkUp.GetChildData();
								bitMapsizeTable.Hori.minOriginSB = atoi(strTmp.data());
								MarkUp.FindChildElem("minAdvanceSB");
								strTmp = MarkUp.GetChildData();
								bitMapsizeTable.Hori.minAdvanceSB = atoi(strTmp.data());
								MarkUp.FindChildElem("maxBeforeBL");
								strTmp = MarkUp.GetChildData();
								bitMapsizeTable.Hori.maxBeforeBL = atoi(strTmp.data());
								MarkUp.FindChildElem("minAfterBL");
								strTmp = MarkUp.GetChildData();
								bitMapsizeTable.Hori.minAfterBL = atoi(strTmp.data());
								MarkUp.FindChildElem("pad1");
								strTmp = MarkUp.GetChildData();
								bitMapsizeTable.Hori.pad1 = atoi(strTmp.data());
								MarkUp.FindChildElem("pad2");
								strTmp = MarkUp.GetChildData();
								bitMapsizeTable.Hori.pad2 = atoi(strTmp.data());								
							}

							if (MarkUp.FindElem("sbitLineMetrics_vert"))
							{	
								MarkUp.FindChildElem("ascender");
								strTmp = MarkUp.GetChildData();
								bitMapsizeTable.Vert.ascender = atoi(strTmp.data());
								MarkUp.FindChildElem("descender");
								strTmp = MarkUp.GetChildData();
								bitMapsizeTable.Vert.descender = atoi(strTmp.data());
								MarkUp.FindChildElem("widthMax");
								strTmp = MarkUp.GetChildData();
								bitMapsizeTable.Vert.widthMax = atoi(strTmp.data());
								MarkUp.FindChildElem("caretSlopeNumerator");
								strTmp = MarkUp.GetChildData();
								bitMapsizeTable.Vert.caretSlopeNumerator = atoi(strTmp.data());
								MarkUp.FindChildElem("caretSlopeDenominator");
								strTmp = MarkUp.GetChildData();
								bitMapsizeTable.Vert.caretSlopeDenominator = atoi(strTmp.data());
								MarkUp.FindChildElem("caretOffset");
								strTmp = MarkUp.GetChildData();
								bitMapsizeTable.Vert.caretOffset = atoi(strTmp.data());
								MarkUp.FindChildElem("minOriginSB");
								strTmp = MarkUp.GetChildData();
								bitMapsizeTable.Vert.minOriginSB = atoi(strTmp.data());
								MarkUp.FindChildElem("minAdvanceSB");
								strTmp = MarkUp.GetChildData();
								bitMapsizeTable.Vert.minAdvanceSB = atoi(strTmp.data());
								MarkUp.FindChildElem("maxBeforeBL");
								strTmp = MarkUp.GetChildData();
								bitMapsizeTable.Vert.maxBeforeBL = atoi(strTmp.data());
								MarkUp.FindChildElem("minAfterBL");
								strTmp = MarkUp.GetChildData();
								bitMapsizeTable.Vert.minAfterBL = atoi(strTmp.data());
								MarkUp.FindChildElem("pad1");
								strTmp = MarkUp.GetChildData();
								bitMapsizeTable.Vert.pad1 = atoi(strTmp.data());
								MarkUp.FindChildElem("pad2");
								strTmp = MarkUp.GetChildData();
								bitMapsizeTable.Vert.pad2 = atoi(strTmp.data());								
							}
							if (MarkUp.FindElem("startGlyphIndex"))
							{						
								strTmp = MarkUp.GetData();
								bitMapsizeTable.startGlyphIndex = atoi(strTmp.data());
							}
							if (MarkUp.FindElem("endGlyphIndex"))
							{						
								strTmp = MarkUp.GetData();
								bitMapsizeTable.endGlyphIndex = atoi(strTmp.data());
							}
							if (MarkUp.FindElem("ppemX"))
							{						
								strTmp = MarkUp.GetData();
								bitMapsizeTable.ppemX = atoi(strTmp.data());
							}
							if (MarkUp.FindElem("ppemY"))
							{						
								strTmp = MarkUp.GetData();
								bitMapsizeTable.ppemY = atoi(strTmp.data());
							}
							if (MarkUp.FindElem("bitDepth"))
							{						
								strTmp = MarkUp.GetData();
								bitMapsizeTable.bitDepth = atoi(strTmp.data());
							}
							if (MarkUp.FindElem("flags"))
							{						
								strTmp = MarkUp.GetData();
								bitMapsizeTable.Flags = atoi(strTmp.data());
							}
							while (MarkUp.FindElem("indexSubTableArray"))
							{
								IndexSubTableArray  TableArry;

								strTmp = MarkUp.GetAttrib("firstGlyphIndex");
								TableArry.firstGlyphIndex = atoi(strTmp.data());
								strTmp = MarkUp.GetAttrib("lastGlyphIndex");
								TableArry.lastGlyphIndex = atoi(strTmp.data());

								MarkUp.IntoElem();
								TableArry.IndexSubTb.Header.indexFormat = 1;
								int  iGlyphIndex = TableArry.firstGlyphIndex;
								while (MarkUp.FindElem("glyphLoc"))
								{
									CHYCodeMapItem MapItem;							
									strTmp = MarkUp.GetAttrib("id");
									MapItem.iGlyphIndex = iGlyphIndex++;//atoi(strTmp.data());
									strTmp = MarkUp.GetAttrib("unicode");
									if (!strTmp.empty())
									{
										MapItem.ulGlyphNo = (unsigned long)strtol(strTmp.data(),NULL,16);
										m_HYCodeMap.vtHYCodeMap.push_back(MapItem);
									}

									MarkUp.IntoElem();
									if (MarkUp.FindElem("bigGlyphMetrics"))
									{	
										TableArry.IndexSubTb.Header.imageFormat = 18;
										CBDTFormat18	Format18;										
										MarkUp.FindChildElem("height");
										strTmp = MarkUp.GetChildData();
										Format18.glyphMetrics.height = atoi(strTmp.data());
										MarkUp.FindChildElem("width");
										strTmp = MarkUp.GetChildData();
										Format18.glyphMetrics.width = atoi(strTmp.data());
										MarkUp.FindChildElem("horiBearingX");
										strTmp = MarkUp.GetChildData();
										Format18.glyphMetrics.horiBearingX = atoi(strTmp.data());
										MarkUp.FindChildElem("horiBearingY");
										strTmp = MarkUp.GetChildData();
										Format18.glyphMetrics.horiBearingY = atoi(strTmp.data());
										MarkUp.FindChildElem("horiAdvance");
										strTmp = MarkUp.GetChildData();
										Format18.glyphMetrics.horiAdvance = atoi(strTmp.data());
										MarkUp.FindChildElem("vertBearingX");
										strTmp = MarkUp.GetChildData();
										Format18.glyphMetrics.vertBearingX = atoi(strTmp.data());
										MarkUp.FindChildElem("vertBearingY");
										strTmp = MarkUp.GetChildData();
										Format18.glyphMetrics.vertBearingY = atoi(strTmp.data());
										MarkUp.FindChildElem("vertAdvance");
										strTmp = MarkUp.GetChildData();
										Format18.glyphMetrics.vertAdvance = atoi(strTmp.data());										

										MarkUp.FindChildElem("Path");
										strTmp = MarkUp.GetChildData();
										string strFile = strImageDir+strTmp;

										FILE* PngFile = fopen(strFile.data(),"r");
										if (PngFile != NULL) 
										{
											Format18.dataLen = HY_FileLength(strFile.data());
											fseek(PngFile,0,SEEK_SET);

											unsigned char uc;
											for (unsigned long z=0; z<Format18.dataLen; z++)
											{
												fread(&uc,1,1,PngFile);
												Format18.data.push_back(uc);
											}									
											fclose(PngFile);
										}
										TableArry.vtDdataForm18.push_back(Format18);										

									}
									else if (MarkUp.FindElem("smallGlyphMetrics"))
									{
										TableArry.IndexSubTb.Header.imageFormat = 17;
										CBDTFormat17	Format17;										
										MarkUp.FindChildElem("height");
										strTmp = MarkUp.GetChildData();
										Format17.glyphMetrics.height = atoi(strTmp.data());
										MarkUp.FindChildElem("width");
										strTmp = MarkUp.GetChildData();
										Format17.glyphMetrics.width = atoi(strTmp.data());
										MarkUp.FindChildElem("BearingX");
										strTmp = MarkUp.GetChildData();
										Format17.glyphMetrics.BearingX = atoi(strTmp.data());
										MarkUp.FindChildElem("BearingY");
										strTmp = MarkUp.GetChildData();
										Format17.glyphMetrics.BearingY = atoi(strTmp.data());
										MarkUp.FindChildElem("Advance");
										strTmp = MarkUp.GetChildData();
										Format17.glyphMetrics.Advance = atoi(strTmp.data());										

										MarkUp.FindElem("Path");
										strTmp = MarkUp.GetData();
										//string strFile = strImageDir+strTmp;
										string strFile = strTmp;
										FILE* PngFile = fopen(strFile.data(),"r+b");
										if (PngFile != NULL) 
										{
											Format17.dataLen = HY_FileLength(strFile.data());
											fseek(PngFile,0,SEEK_SET);

											unsigned char uc;
											for (unsigned long z=0; z<Format17.dataLen; z++)
											{
												fread(&uc,1,1,PngFile);
												Format17.data.push_back(uc);
											}									
											fclose(PngFile);
										}
										Format17.dataLen = Format17.data.size();
										TableArry.vtDdataForm17.push_back(Format17);							
									}
									MarkUp.OutOfElem();
								}
								TableArry.lastGlyphIndex = iGlyphIndex-1;
								bitMapsizeTable.vtIndexSubTableArray.push_back(TableArry);
								MarkUp.OutOfElem();
							}
							m_HYCblc.vtBitmapSizeTb.push_back(bitMapsizeTable);
							MarkUp.OutOfElem();
						}
						MarkUp.OutOfElem();
					}					
				}	
				else if (FontType == 1)
				{
					if (MarkUp.FindElem("Apple"))
					{
						MarkUp.IntoElem();
						m_HYsbix.setdefault();
						if(MarkUp.FindElem("sbix"))
						{
							MarkUp.IntoElem();
							MarkUp.FindElem("flag");
							strTmp = MarkUp.GetAttrib("value");
							m_HYsbix.flags = atoi(strTmp.data());

							while(MarkUp.FindElem("strike"))
							{
								CSbixStrikes	sbixtrikes;
								strTmp = MarkUp.GetAttrib("ppem");
								sbixtrikes.ppem= atoi(strTmp.data());
								strTmp = MarkUp.GetAttrib("resolution");
								sbixtrikes.resolution= atoi(strTmp.data());

								MarkUp.IntoElem();
								while (MarkUp.FindElem("glyph"))
								{
									CStrikesDatarecord	datarecord;
									strTmp = MarkUp.GetAttrib("id");
									datarecord.gid = atoi(strTmp.data());

									strTmp = MarkUp.GetAttrib("unicode");
									memcpy_s(datarecord.graphicType,4,strTmp.data(),4);
									strTmp = MarkUp.GetAttrib("originOffsetX");
									datarecord.originOffsetX = atoi(strTmp.data());
									strTmp = MarkUp.GetAttrib("originOffsetY");
									datarecord.originOffsetY = atoi(strTmp.data());

									if (MarkUp.FindChildElem("Path"))
									{
										strTmp = MarkUp.GetChildData();
										string strFile = strImageDir+strTmp;

										FILE* PngFile = fopen(strFile.data(),"r");
										if (PngFile != NULL) 
										{
											unsigned long fLength = HY_FileLength(strFile.data());
											fseek(PngFile,0,SEEK_SET);

											unsigned char uc;
											for (unsigned long z=0; z<fLength; z++)
											{
												fread(&uc,1,1,PngFile);
												datarecord.vtdata.push_back(uc);
											}									
											fclose(PngFile);
										}									
									}

									sbixtrikes.vtStrikesDatarecord.push_back(datarecord);
								}									
								MarkUp.OutOfElem();
								m_HYsbix.vtSbixStrikes.push_back(sbixtrikes);
							}					
							MarkUp.OutOfElem();
						}					
						MarkUp.OutOfElem();
					}
				}
				MarkUp.OutOfElem();
			}
		}
		*/

		return HY_NOERROR;

	}	// end of int CHYFontCodec::MakeBitmapGlyphs()

	void CHYFontCodec::MakeVheaVmtx()
	{
		m_HYVhea.SetDefault();
		m_HYVmtx.SetDefault();

		unsigned  short usADH = m_HYOS2.sTypoAscender-m_HYOS2.sTypoDescender;//当传入的数据没有ADH值得时候，我们用排版线计算

		m_HYVhea.version.value		= 0x0001;
		m_HYVhea.version.fract		= 0x0000;
		m_HYVhea.ascent				= m_HYhead.unitsPerEm/2;
		m_HYVhea.descent			= 0 - m_HYhead.unitsPerEm/2;
		m_HYVhea.lineGap			= 0;
		m_HYVhea.advanceHeightMax	= usADH;
		m_HYVhea.numOfLongVerMetrics = 1;	// 当传入数据中没有VHEA和VMTX时，只能认为所有字形ADH都是一样的。

		size_t stGlyphNum = m_vtHYGlyphs.size();
		m_HYVhea.yMaxExtent	= m_HYhead.yMin;
		for (size_t i=0; i<stGlyphNum; i++)	{			
			CHYGlyph& Gryph = m_vtHYGlyphs[i];

			if (i == 345)
			{
				int ijin = 0;

			}
			// 此种计算方式与蒙纳的计算方式一致。
			short Tsb = m_HYOS2.sTypoAscender-m_vtHYGlyphs[i].maxY;
			if (i<m_HYVhea.numOfLongVerMetrics)	{
				VMTX_LONGVRTMETRIC vmtxLongHormetric;				
				vmtxLongHormetric.advanceHeight = usADH;
				vmtxLongHormetric.tsb = Tsb;
				m_HYVmtx.vtLongVrtmetric.push_back(vmtxLongHormetric);
			}
			else{				
				m_HYVmtx.vtTopsidebearing.push_back(Tsb);				
			}

			short stmp = Tsb - (Gryph.maxY - Gryph.minY);
			if(stmp>m_HYVhea.yMaxExtent){
				m_HYVhea.yMaxExtent = stmp;
			}
		}

		m_HYVhea.minTop = FindVertMinTop();
		m_HYVhea.minBottom = FindVertMinBottom();
		
		m_HYVhea.caretSlopeRise		= 0;
		m_HYVhea.caretSlopeRun		= 1;
		m_HYVhea.caretOffset		= 0;	//Set value equal to 0 for nonslanted fonts.
		m_HYVhea.reserved1			= 0;
		m_HYVhea.reserved2			= 0;
		m_HYVhea.reserved3			= 0;
		m_HYVhea.reserved4			= 0;
		m_HYVhea.metricDataFormat	= 0;	

	}	// end of void CHYFontCodec::MakeVheaVmtx()

	void CHYFontCodec::MakeVerticalMetrics()
	{	
		m_HYVhea.SetDefault();
		m_HYVmtx.SetDefault();

		unsigned  short usADH = m_HYOS2.sTypoAscender - m_HYOS2.sTypoDescender;
		m_HYVhea.version.value = 0x0001;
		m_HYVhea.version.fract = 0x0000;
		m_HYVhea.ascent = m_HYhead.unitsPerEm / 2;
		m_HYVhea.descent = 0 - m_HYhead.unitsPerEm / 2;
		m_HYVhea.lineGap = 0;
		m_HYVhea.advanceHeightMax = usADH;

		int szGlyphNum = m_HYMaxp.numGlyphs;
		m_HYVhea.numOfLongVerMetrics = 0;
		if (szGlyphNum > 0) {
			int iBaseadvanceHight = m_vtHYGlyphs[--szGlyphNum].advanceHeight;
			while (--szGlyphNum >= 0) {
				if (m_vtHYGlyphs[szGlyphNum].advanceHeight == iBaseadvanceHight)
					m_HYVhea.numOfLongVerMetrics++;
				else
					break;
			}
			m_HYVhea.numOfLongVerMetrics = (unsigned short)m_HYMaxp.numGlyphs - m_HYVhea.numOfLongVerMetrics;
		}

		m_HYVhea.yMaxExtent = m_HYhead.yMin;
		for (int i = 0; i < m_HYMaxp.numGlyphs; i++) {
			CHYGlyph& Gryph = m_vtHYGlyphs[i];
			if (i < m_HYVhea.numOfLongVerMetrics) {
				VMTX_LONGVRTMETRIC vmtxLongHormetric;
				vmtxLongHormetric.advanceHeight = Gryph.advanceHeight;
				vmtxLongHormetric.tsb = Gryph.topSB;
				m_HYVmtx.vtLongVrtmetric.push_back(vmtxLongHormetric);
			}
			else {
				m_HYVmtx.vtTopsidebearing.push_back(Gryph.topSB);
			}
			short stmp = Gryph.topSB - (Gryph.maxY - Gryph.minY);
			if (stmp > m_HYVhea.yMaxExtent) {
				m_HYVhea.yMaxExtent = stmp;
			}
		}
		m_HYVhea.minTop = FindVertMinTop();
		m_HYVhea.minBottom = FindVertMinBottom();
		m_HYVhea.caretSlopeRise = 0;
		m_HYVhea.caretSlopeRun = 1;
		m_HYVhea.caretOffset = 0;
		m_HYVhea.reserved1 = 0;
		m_HYVhea.reserved2 = 0;
		m_HYVhea.reserved3 = 0;
		m_HYVhea.reserved4 = 0;
		m_HYVhea.metricDataFormat = 0;

	}	// end of void CHYFontCodec::MakeVerticalMetrics()

	void CHYFontCodec::CountVerticalMetrics()
	{
		unsigned  short usADH = m_HYOS2.sTypoAscender - m_HYOS2.sTypoDescender;//当传入的数据没有ADH值得时候，我们用排版线计算

		size_t stGlyphNum = m_vtHYGlyphs.size();
		for (size_t i = 0; i < stGlyphNum; i++) {
			CHYGlyph& Gryph = m_vtHYGlyphs[i];			
			// 此种计算方式与蒙纳的计算方式一致。
			short Tsb = m_HYOS2.sTypoAscender - m_vtHYGlyphs[i].maxY;
			Gryph.topSB = Tsb;
			Gryph.advanceHeight = usADH;
		}

	}	// end of void CHYFontCodec::CountVerticalMetrics()
	
	void	CHYFontCodec::CountUnicodeRange(unsigned long& uni1, unsigned long& uni2,unsigned long& uni3,unsigned long& uni4)
	{
		uni1 = 0,uni2 = 0,uni3 = 0,uni4 = 0;
		for (size_t i=0; i<m_HYCmap.vtCamp_tb_entry.size();i++){
			if (m_HYCmap.vtCamp_tb_entry[i].plat_encod_ID == 10){
				//57	Non-Plane 0 *	D800-DFFF				
				uni2 |= 0x02000000;
				break;				
			}
		}

		size_t stGlyphNum = m_vtHYGlyphs.size();
		for(size_t i=0; i<stGlyphNum; i++){
			CHYGlyph& glyph = m_vtHYGlyphs[i];
			size_t uninum = glyph.vtUnicode.size();
			for(size_t j=0; j<uninum; j++){
				unsigned long& unicode = glyph.vtUnicode[j];
				//Basic Latin  0
				if (unicode<=0x007F){
					uni1 |= 0x00000001;
					continue;
				}
				//Latin-1 Supplement	1
				if (unicode>=0x0080 && unicode<=0x00FF){
					uni1 |= 0x00000002;					
					continue;
				}
				//Latin Extended-A		2
				if (unicode>=0x0100 && unicode<=0x017F){
					uni1 |= 0x00000004;
					continue;
				}
				//Latin Extended-B		3
				if (unicode>=0x0180 && unicode<=0x024F){
					uni1 |= 0x00000008;
					continue;
				}				
				//	IPA Extensions		4
				if (unicode>=0x0250 && unicode<=0x02AF){
					uni1 |= 0x00000010;
					continue;
				}
				//	Phonetic Extensions 4
				if (unicode>=0x1D00 && unicode<=0x1D7F){
					uni1 |= 0x00000010;
					continue;
				}
				//	Phonetic Extensions Supplement 4
				if (unicode>=0x1D80 && unicode<=0x1DBF){
					uni1 |= 0x00000010;
					continue;
				}
				//Spacing Modifier Letters 5
				if (unicode>=0x02B0 && unicode<=0x02FF){
					uni1 |= 0x00000020;
					continue;
				}
				//Modifier Tone Letters 5
				if (unicode>=0xA700 && unicode<=0xA71F){
					uni1 |= 0x00000020;
					continue;
				}
				//Combining Diacritical Marks 6
				if (unicode>=0x0300 && unicode<=0x036F){
					uni1 |= 0x00000040;
					continue;
				}
				//Combining Diacritical Marks Supplement 6
				if (unicode>=0x1DC0 && unicode<=0x1DFF){
					uni1 |= 0x00000040;
					continue;
				}
				//Greek and Coptic 7
				if (unicode>=0x0370 && unicode<=0x03FF){
					uni1 |= 0x00000080;
					continue;
				}
				//Coptic 8
				if (unicode>=0x2C80 && unicode<=0x2CFF){
					uni1 |= 0x00000100;
					continue;
				}
				// 	Cyrillic 9
				if (unicode>=0x0400&& unicode<=0x04FF){
					uni1 |= 0x00000200;
					continue;
				}
				// Cyrillic Supplement 9
				if (unicode>=0x0500&& unicode<=0x052F){
					uni1 |= 0x00000200;
					continue;
				}
				//Cyrillic Extended-A 9
				if (unicode>=0xA640&& unicode<=0xA69F){
					uni1 |= 0x00000200;
					continue;
				}
				//Cyrillic Extended-B 9
				if (unicode>=0xA640&& unicode<=0xA69F){
					uni1 |= 0x00000200;
					continue;
				}
				//Armenian	10
				if (unicode>=0x0530&& unicode<=0x058F){
					uni1 |= 0x00000400;
					continue;
				}
				//Hebrew	11
				if (unicode>=0x0590&& unicode<=0x05FF){
					uni1 |= 0x00000800;
					continue;
				}
				// Vai	12
				if (unicode>=0xA500&& unicode<=0xA63F){
					uni1 |= 0x00001000;
					continue;
				}
				// Arabic	13
				if (unicode>=0x0600&& unicode<=0x06FF){
					uni1 |= 0x00002000;
					continue;
				}
				// Arabic  Supplement	13
				if (unicode>=0x0750&& unicode<=0x077F){
					uni1 |= 0x00002000;
					continue;
				}
				// NKo 14
				if (unicode>=0x07C0&& unicode<=0x07FF){
					uni1 |= 0x00004000;
					continue;
				}
				// Devanagari 15
				if (unicode>=0x0900&& unicode<=0x097F){
					uni1 |= 0x00008000;
					continue;
				}
				//Bengali 16
				if (unicode>=0x0980&& unicode<=0x09FF){
					uni1 |= 0x00010000;
					continue;
				}
				//Gurmukhi	17
				if (unicode>=0x0A00&& unicode<=0x0A7F){
					uni1 |= 0x00020000;
					continue;
				}
				//Gujarati 18
				if (unicode>=0x0A80&& unicode<=0x0AFF){
					uni1 |= 0x00040000;
					continue;
				}
				//Oriya 19
				if (unicode>=0x0B00&& unicode<=0x0B7F){
					uni1 |= 0x00080000;
					continue;
				}
				//Tamil 20
				if (unicode>=0x0B80&& unicode<=0x0BFF){
					uni1 |= 0x00100000;
					continue;
				}
				//Telugu 21
				if (unicode>=0x0C00&& unicode<=0x0C7F){
					uni1 |= 0x00200000;
					continue;
				}
				//Kannada 22
				if (unicode>=0x0C80&& unicode<=0x0CFF){
					uni1 |= 0x00400000;
					continue;
				}
				//Malayalam 23
				if (unicode>=0x0D00&& unicode<=0x0D7F){
					uni1 |= 0x00800000;
					continue;
				}
				//Thai 24
				if (unicode>=0x0E00&& unicode<=0x0E7F){
					uni1 |= 0x01000000;
					continue;
				}
				//Lao 25
				if (unicode>=0x0E80&& unicode<=0x0EFF){
					uni1 |= 0x02000000;
					continue;
				}
				//Georgian 26
				if (unicode>=0x10A0&& unicode<=0x10FF){
					uni1 |= 0x04000000;
					continue;
				}
				//Georgian Supplement	26
				if (unicode>=0x2D00&& unicode<=0x2D2F){
					uni1 |= 0x04000000;
					continue;
				}
				//Balinese	27
				if (unicode>=0x2D00&& unicode<=0x2D2F){
					uni1 |= 0x08000000;
					continue;
				}
				//Hangul Jamo	28
				if (unicode>=0x1100&& unicode<=0x11FF){
					uni1 |= 0x10000000;
					continue;
				}
				//Latin Extended Additional		29
				if (unicode>=0x1E00&& unicode<=0x1EFF){
					uni1 |= 0x20000000;
					continue;
				}
				//Latin Extended-C			29
				if (unicode>=0x2C60&& unicode<=0x2C7F){
					uni1 |= 0x20000000;
					continue;
				}
				//Latin Extended-D		29
				if (unicode>=0xA720&& unicode<=0xA7FF){
					uni1 |= 0x20000000;
					continue;
				}
				//Greek Extended		30
				if (unicode>=0x1F00&& unicode<=0x1FFF){
					uni1 |= 0x40000000;
					continue;
				}
				//General Punctuation	31
				if (unicode>=0x2000&& unicode<=0x206F){
					uni1 |= 0x80000000;
					continue;
				}
				//Supplemental Punctuation	31
				if (unicode>=0x2E00&& unicode<=0x2E7F){
					uni1 |= 0x80000000;
					continue;
				}
				//Superscripts And Subscripts 32
				if (unicode>=0x2070&& unicode<=0x209F){
					uni2 |= 0x00000001;
					continue;
				}
				//Currency Symbols	 33
				if (unicode>=0x20A0&& unicode<=0x20CF){
					uni2 |= 0x00000002;
					continue;
				}
				//Combining Diacritical Marks For Symbols	 34
				if (unicode>=0x20D0&& unicode<=0x20FF){
					uni2 |= 0x00000004;
					continue;
				}
				//Letterlike Symbols	35
				if (unicode>=0x2100&& unicode<=0x214F){
					uni2 |= 0x00000008;
					continue;
				}
				//Number Forms	36
				if (unicode>=0x2150&& unicode<=0x218F){
					uni2 |= 0x00000010;
					continue;
				}
				//Arrows	37
				if (unicode>=0x2190&& unicode<=0x21FF){
					uni2 |= 0x00000020;
					continue;
				}
				//	Supplemental Arrows-A 37
				if (unicode>=0x27F0&& unicode<=0x27FF){
					uni2 |= 0x00000020;
					continue;
				}
				//	Supplemental Arrows-B 37
				if (unicode>=0x2900 && unicode<=0x297F){
					uni2 |= 0x00000020;
					continue;
				}
				//	Miscellaneous Symbols and Arrows  37
				if (unicode>=0x2B00 && unicode<=0x2BFF){
					uni2 |= 0x00000020;
					continue;
				}
				//	Mathematical Operators	 38
				if (unicode>=0x2200 && unicode<=0x22FF){
					uni2 |= 0x00000040;
					continue;
				}
				//	Supplemental Mathematical Operators	 38
				if (unicode>=0x2A00 && unicode<=0x2AFF){
					uni2 |= 0x00000040;
					continue;
				}
				//	Miscellaneous Mathematical Symbols-A  38
				if (unicode>=0x27C0 && unicode<=0x27EF){
					uni2 |= 0x00000040;
					continue;
				}
				//	Miscellaneous Mathematical Symbols-B  38
				if (unicode>=0x2980 && unicode<=0x29FF){
					uni2 |= 0x00000040;
					continue;
				}
				//	Miscellaneous Mathematical Symbols-B  38
				if (unicode>=0x2980 && unicode<=0x29FF){
					uni2 |= 0x00000040;
					continue;
				}
				//	Miscellaneous Technical	39
				if (unicode>=0x2300 && unicode<=0x23FF){
					uni2 |= 0x00000080;
					continue;
				}
				//	Control Pictures	40
				if (unicode>=0x2400 && unicode<=0x243F){
					uni2 |= 0x00000100;
					continue;
				}				
				//	Optical Character Recognition	41
				if (unicode>=0x2440 && unicode<=0x245F){
					uni2 |= 0x00000200;
					continue;
				}
				//42	Enclosed Alphanumerics
				if (unicode>=0x2460 && unicode<=0x24FF){
					uni2 |= 0x00000400;
					continue;
				}
				//43	Box Drawing
				if (unicode>=0x2500 && unicode<=0x257F){
					uni2 |= 0x00000800;
					continue;
				}
				//44	Block Elements
				if (unicode>=0x2580 && unicode<=0x259F){
					uni2 |= 0x00001000;
					continue;
				}
				//45	Geometric  Shapes
				if (unicode>=0x25A0 && unicode<=0x25FF){
					uni2 |= 0x00002000;
					continue;
				}
				//46	Miscellaneous Symbols
				if (unicode>=0x2600 && unicode<=0x26FF){
					uni2 |= 0x00004000;
					continue;
				}
				//47	Dingbats
				if (unicode>=0x2700 && unicode<=0x27BF){
					uni2 |= 0x00008000;
					continue;
				}
				//48	CJK Symbols And Punctuation
				if (unicode>=0x3000 && unicode<=0x303F){
					uni2 |= 0x00010000;
					continue;
				}
				//49	Hiragana	3040-309F
				if (unicode>=0x3040 && unicode<=0x309F){
					uni2 |= 0x00020000;
					continue;
				}
				//50	Katakana	
				if (unicode>=0x30A0 && unicode<=0x30FF){
					uni2 |= 0x00040000;
					continue;
				}
				//Katakana Phonetic Extensions
				if (unicode>=0x31F0 && unicode<=0x31FF){
					uni2 |= 0x00040000;
					continue;
				}
				//51	Bopomofo
				if (unicode>=0x3100 && unicode<=0x312F){
					uni2 |= 0x00080000;
					continue;
				}
				//Bopomofo Extended
				if (unicode>=0x31A0 && unicode<=0x31BF){
					uni2 |= 0x00080000;
					continue;
				}
				//52	Hangul Compatibility Jamo
				if (unicode>=0x3130 && unicode<=0x318F){
					uni2 |= 0x00100000;
					continue;
				}
				//53	Phags-pa
				if (unicode>=0xA840 && unicode<=0xA87F){
					uni2 |= 0x00200000;
					continue;
				}
				//54	Enclosed CJK Letters And Months
				if (unicode>=0x3200 && unicode<=0x32FF){
					uni2 |= 0x00400000;
					continue;
				}
				//55	CJK Compatibility
				if (unicode>=0x3300 && unicode<=0x33FF){
					uni2 |= 0x00800000;
					continue;
				}				
				//56	Hangul Syllables	
				if (unicode>=0xAC00 && unicode<=0xD7AF){
					uni2 |= 0x01000000;
					continue;
				}				
				//57	Non-Plane 0 *	D800-DFFF
				if (unicode>=0xD800 && unicode<=0xDFFF){
					uni2 |= 0x02000000;
					continue;
				}
				//58	Phoenician	10900-1091F
				if (unicode>=0x10900 && unicode<=0x1091F){
					uni2 |= 0x04000000;
					continue;
				}
				//59	CJK Unified Ideographs	4E00-9FFF
				if (unicode>=0x4E00 && unicode<=0x9FFF){
					uni2 |= 0x08000000;
					continue;
				}
				//CJK Radicals Supplement
				if (unicode>=0x2E80 && unicode<=0x2EFF){
					uni2 |= 0x08000000;
					continue;
				}
				//Kangxi Radicals	2F00-2FDF
				if (unicode>=0x2F00 && unicode<=0x2FDF){
					uni2 |= 0x08000000;
					continue;
				}
				//Ideographic Description Characters	2FF0-2FFF
				if (unicode>=0x2FF0 && unicode<=0x2FFF){
					uni2 |= 0x08000000;
					continue;
				}
				//CJK Unified Ideographs Extension A	3400-4DBF
				if (unicode>=0x3400 && unicode<=0x4DBF){
					uni2 |= 0x08000000;
					continue;
				}
				//CJK Unified Ideographs Extension B	20000-2A6DF
				if (unicode>=0x20000 && unicode<=0x2A6DF){
					uni2 |= 0x08000000;
					continue;
				}
				//Kanbun	3190-319F
				if (unicode>=0x3190 && unicode<=0x319F)	{
					uni2 |= 0x08000000;
					continue;
				}
				//60	Private Use Area (plane 0)	E000-F8FF
				if (unicode>=0xE000 && unicode<=0xF8FF){
					uni2 |= 0x10000000;
					continue;
				}
				//61	CJK Strokes	31C0-31EF
				if (unicode>=0x31C0 && unicode<=0x31EF){
					uni2 |= 0x20000000;
					continue;
				}
				//CJK Compatibility Ideographs	F900-FAFF
				if (unicode>=0xF900 && unicode<=0xFAFF){
					uni2 |= 0x20000000;
					continue;
				}
				//CJK Compatibility Ideographs Supplement	2F800-2FA1F
				if (unicode>=0x2F800 && unicode<=0x2FA1F){
					uni2 |= 0x20000000;
					continue;
				}
				//62	Alphabetic Presentation Forms	FB00-FB4F
				if (unicode>=0xFB00 && unicode<=0xFB4F){
					uni2 |= 0x40000000;
					continue;
				}
				//63	Arabic Presentation Forms-A	FB50-FDFF
				if (unicode>=0xFB50 && unicode<=0xFDFF){
					uni2 |= 0x80000000;
					continue;
				}
				//64	Combining Half Marks	FE20-FE2F
				if (unicode>=0xFE20 && unicode<=0xFE2F){
					uni3 |= 0x00000001;
					continue;
				}
				//65	Vertical Forms	FE10-FE1F
				if (unicode>=0xFE10 && unicode<=0xFE1F){
					uni3 |= 0x00000002;
					continue;
				}
				//CJK Compatibility Forms	FE30-FE4F
				if (unicode>=0xFE30 && unicode<=0xFE4F){
					uni3 |= 0x00000002;
					continue;
				}
				//66	Small Form Variants	FE50-FE6F
				if (unicode>=0xFE50 && unicode<=0xFE6F){
					uni3 |= 0x00000004;
					continue;
				}
				//67	Arabic Presentation Forms-B	FE70-FEFF
				if (unicode>=0xFE70 && unicode<=0xFEFF){
					uni3 |= 0x00000008;
					continue;
				}
				//68	Halfwidth And Fullwidth Forms	FF00-FFEF
				if (unicode>=0xFF00 && unicode<=0xFFEF){
					uni3 |= 0x00000010;
					continue;
				}
				//69	Specials	FFF0-FFFF
				if (unicode>=0xFFF0 && unicode<0xFFFF){
					uni3 |= 0x00000020;
					continue;
				}				
				//70	Tibetan	0F00-0FFF
				if (unicode>=0x0F00 && unicode<=0x0FFF){
					uni3 |= 0x00000040;
					continue;
				}
				//71	Syriac	0700-074F
				if (unicode>=0x0700 && unicode<=0x074F){
					uni3 |= 0x00000080;
					continue;
				}
				//72	Thaana	0780-07BF
				if (unicode>=0x0700 && unicode<=0x07BF){
					uni3 |= 0x00000100;
					continue;
				}
				//73	Sinhala	0D80-0DFF
				if (unicode>=0x0D80 && unicode<=0x0DFF){
					uni3 |= 0x00000200;
					continue;
				}
				//74	Myanmar	1000-109F
				if (unicode>=0x1000 && unicode<=0x109F){
					uni3 |= 0x00000400;
					continue;
				}
				//75	Ethiopic	1200-137F
				if (unicode>=0x1200 && unicode<=0x137F){
					uni3 |= 0x00000800;
					continue;
				}
				//Ethiopic Supplement	1380-139F
				if (unicode>=0x1380 && unicode<=0x139F){
					uni3 |= 0x00000800;
					continue;
				}
				//Ethiopic Extended	2D80-2DDF
				if (unicode>=0x2D80 && unicode<=0x2DDF){
					uni3 |= 0x00000800;
					continue;
				}
				//76	Cherokee	13A0-13FF
				if (unicode>=0x13A0 && unicode<=0x13FF){
					uni3 |= 0x00001000;
					continue;
				}
				//77	Unified Canadian Aboriginal Syllabics	1400-167F
				if (unicode>=0x1400 && unicode<=0x167F){
					uni3 |= 0x00002000;
					continue;
				}
				//78	Ogham	1680-169F
				if (unicode>=0x1680 && unicode<=0x169F){
					uni3 |= 0x00004000;
					continue;
				}
				//79	Runic	16A0-16FF
				if (unicode>=0x16A0 && unicode<=0x16FF){
					uni3 |= 0x00008000;
					continue;
				}
				//80	Khmer	1780-17FF
				if (unicode>=0x1780 && unicode<=0x17FF){
					uni3 |= 0x00010000;
					continue;
				}
				//Khmer Symbols	19E0-19FF
				if (unicode>=0x19E0 && unicode<=0x19FF){
					uni3 |= 0x00010000;
					continue;
				}
				//81	Mongolian	1800-18AF
				if (unicode>=0x1800 && unicode<=0x18AF){
					uni3 |= 0x00020000;
					continue;
				}
				//82	Braille Patterns	2800-28FF
				if (unicode>=0x2800 && unicode<=0x28FF)
				{
					uni3 |= 0x00040000;
					continue;
				}
				//83	Yi Syllables	A000-A48F
				if (unicode>=0xA000 && unicode<=0xA48F)
				{
					uni3 |= 0x00080000;
					continue;
				}
				//Yi Radicals	A490-A4CF
				if (unicode>=0xA490 && unicode<=0xA4CF)
				{
					uni3 |= 0x00080000;
					continue;
				}
				//84	Tagalog	1700-171F
				if (unicode>=0x1700 && unicode<=0x171F)
				{
					uni3 |= 0x00100000;
					continue;
				}
				//Hanunoo	1720-173F
				if (unicode>=0x1720 && unicode<=0x173F)
				{
					uni3 |= 0x00100000;
					continue;
				}
				//Buhid	1740-175F
				if (unicode>=0x1740 && unicode<=0x175F)
				{
					uni3 |= 0x00100000;
					continue;
				}
				//Tagbanwa	1760-177F
				if (unicode>=0x1760 && unicode<=0x177F)
				{
					uni3 |= 0x00100000;
					continue;
				}
				//85	Old Italic	10300-1032F
				if (unicode>=0x10300 && unicode<=0x1032F)
				{
					uni3 |= 0x00200000;
					continue;
				}
				//86	Gothic	10330-1034F
				if (unicode>=0x10330 && unicode<=0x1034F)
				{
					uni3 |= 0x00400000;
					continue;
				}
				//87	Deseret	10400-1044F
				if (unicode>=0x10400 && unicode<=0x1044F)
				{
					uni3 |= 0x00800000;
					continue;
				}
				//88	Byzantine Musical Symbols	1D000-1D0FF
				if (unicode>=0x1D000 && unicode<=0x1D0FF)
				{
					uni3 |= 0x01000000;
					continue;
				}
				//Musical Symbols	1D100-1D1FF
				if (unicode>=0x1D100 && unicode<=0x1D1FF)
				{
					uni3 |= 0x01000000;
					continue;
				}
				//Ancient Greek Musical Notation	1D200-1D24F
				if (unicode>=0x1D200 && unicode<=0x1D24F)
				{
					uni3 |= 0x01000000;
					continue;
				}
				//89	Mathematical Alphanumeric Symbols	1D400-1D7FF
				if (unicode>=0x1D400 && unicode<=0x1D7FF)
				{
					uni3 |= 0x02000000;
					continue;
				}
				//90	Private Use (plane 15)	FF000-FFFFD
				if (unicode>=0xFF000 && unicode<=0xFFFFD)
				{
					uni3 |= 0x04000000;
					continue;
				}
				//Private Use (plane 16)	100000-10FFFD
				if (unicode>=0x100000 && unicode<=0x10FFFD)
				{
					uni3 |= 0x04000000;
					continue;
				}
				//91	Variation Selectors	FE00-FE0F
				if (unicode>=0xFE00&& unicode<=0xFE0F)
				{
					uni3 |= 0x08000000;
					continue;
				}
				//Variation Selectors Supplement	E0100-E01EF
				if (unicode>=0xFE00&& unicode<=0xFE0F)
				{
					uni3 |= 0x08000000;
					continue;
				}
				//92	Tags	E0000-E007F
				if (unicode>=0xE0000&& unicode<=0xE007F)
				{
					uni3 |= 0x10000000;
					continue;
				}
				//93	Limbu	1900-194F
				if (unicode>=0x1900&& unicode<=0x194F)
				{
					uni3 |= 0x20000000;
					continue;
				}
				//94	Tai Le	1950-197F
				if (unicode>=0x1950&& unicode<=0x197F)
				{
					uni3 |= 0x40000000;
					continue;
				}
				//95	New Tai Lue	1980-19DF
				if (unicode>=0x1980&& unicode<=0x19DF)
				{
					uni3 |= 0x80000000;
					continue;
				}
				//96	Buginese	1A00-1A1F
				if (unicode>=0x1A00 && unicode<=0x1A1F)
				{
					uni4 |= 0x00000001;
					continue;
				}
				
				//97	Glagolitic	2C00-2C5F
				if (unicode>=0x2C00 && unicode<=0x2C5F)
				{
					uni4 |= 0x00000002;
					continue;
				}
				//98	Tifinagh	2D30-2D7F
				if (unicode>=0x2D30 && unicode<=0x2D7F)
				{
					uni4 |= 0x00000004;
					continue;
				}
				//99	Yijing Hexagram Symbols	4DC0-4DFF
				if (unicode>=0x4DC0 && unicode<=0x4DFF)
				{
					uni4 |= 0x00000008;
					continue;
				}
				//100	Syloti Nagri	A800-A82F
				if (unicode>=0xA800 && unicode<=0xA82F)
				{
					uni4 |= 0x00000010;
					continue;
				}
				//101	Linear B Syllabary	10000-1007F
				if (unicode>=0x10000 && unicode<=0x1007F)
				{
					uni4 |= 0x00000020;
					continue;
				}
				//Linear B Ideograms	10080-100FF
				if (unicode>=0x10080 && unicode<=0x100FF)
				{
					uni4 |= 0x00000020;
					continue;
				}
				//Aegean Numbers	10100-1013F
				if (unicode>=0x10100 && unicode<=0x1013F)
				{
					uni4 |= 0x00000020;
					continue;
				}
				//102	Ancient Greek Numbers	10140-1018F
				if (unicode>=0x10140 && unicode<=0x1018F)
				{
					uni4 |= 0x00000040;
					continue;
				}
				//103	Ugaritic	10380-1039F
				if (unicode>=0x10380 && unicode<=0x1039F)
				{
					uni4 |= 0x00000080;
					continue;
				}
				//104	Old Persian	103A0-103DF
				if (unicode>=0x103A0 && unicode<=0x103DF)
				{
					uni4 |= 0x00000100;
					continue;
				}
				//105	Shavian	10450-1047F
				if (unicode>=0x10450 && unicode<=0x1047F)
				{
					uni4 |= 0x00000200;
					continue;
				}
				//106	Osmanya	10480-104AF
				if (unicode>=0x10480 && unicode<=0x104AF)
				{
					uni4 |= 0x00000400;
					continue;
				}
				//107	Cypriot Syllabary	10800-1083F
				if (unicode>=0x10800 && unicode<=0x1083F)
				{
					uni4 |= 0x00000800;
					continue;
				}
				//108	Kharoshthi	10A00-10A5F
				if (unicode>=0x10A00 && unicode<=0x10A5F)
				{
					uni4 |= 0x00001000;
					continue;
				}
				//109	Tai Xuan Jing Symbols	1D300-1D35F
				if (unicode>=0x1D300 && unicode<=0x1D35F)
				{
					uni4 |= 0x00002000;
					continue;
				}
				//110	Cuneiform	12000-123FF
				if (unicode>=0x12000 && unicode<=0x123FF)
				{
					uni4 |= 0x00004000;
					continue;
				}
				//Cuneiform Numbers and Punctuation	12400-1247F
				if (unicode>=0x12400 && unicode<=0x1247F)
				{
					uni4 |= 0x00004000;
					continue;
				}
				//111	Counting Rod Numerals	1D360-1D37F
				if (unicode>=0x1D360 && unicode<=0x1D37F)
				{
					uni4 |= 0x00008000;
					continue;
				}
				//112	Sundanese	1B80-1BBF
				if (unicode>=0x1B80 && unicode<=0x1BBF)
				{
					uni4 |= 0x00010000;
					continue;
				}
				//113	Lepcha	1C00-1C4F
				if (unicode>=0x1C00 && unicode<=0x1C4F)
				{
					uni4 |= 0x00020000;
					continue;
				}
				//114	Ol Chiki	1C50-1C7F
				if (unicode>=0x1C50 && unicode<=0x1C7F)
				{
					uni4 |= 0x00040000;
					continue;
				}
				//115	Saurashtra	A880-A8DF
				if (unicode>=0xA880 && unicode<=0xA8DF)
				{
					uni4 |= 0x00080000;
					continue;
				}
				//116	Kayah Li	A900-A92F
				if (unicode>=0xA900 && unicode<=0xA92F)
				{
					uni4 |= 0x00100000;
					continue;
				}
				//117	Rejang	A930-A95F
				if (unicode>=0xA930 && unicode<=0xA95F)
				{
					uni4 |= 0x00200000;
					continue;
				}
				//118	Cham	AA00-AA5F
				if (unicode>=0xAA00 && unicode<=0xAA5F)
				{
					uni4 |= 0x00400000;
					continue;
				}
				//119	Ancient Symbols	10190-101CF
				if (unicode>=0x10190 && unicode<=0x101CF)
				{
					uni4 |= 0x00800000;
					continue;
				}
				//120	Phaistos Disc	101D0-101FF
				if (unicode>=0x101D0 && unicode<=0x101FF)
				{
					uni4 |= 0x01000000;
					continue;
				}
				//121	Carian	102A0-102DF
				if (unicode>=0x102A0 && unicode<=0x102DF)
				{
					uni4 |= 0x02000000;
					continue;
				}
				//Lycian	10280-1029F
				if (unicode>=0x10280 && unicode<=0x1029F)
				{
					uni4 |= 0x02000000;
					continue;
				}
				//Lydian	10920-1093F
				if (unicode>=0x10920 && unicode<=0x1093F)
				{
					uni4 |= 0x02000000;
					continue;
				}				
				//122	Domino Tiles	1F030-1F09F
				if (unicode>=0x1F030 && unicode<=0x1F09F)
				{
					uni4 |= 0x04000000;
					continue;
				}
				if (unicode>=0x1F000 && unicode<=0x1F02F)
				{
					uni4 |= 0x04000000;
					continue;
				}
			}

		}

	}	// end of void	CHYFontCodec::CountUnicodeRange()

	int		CHYFontCodec::SetCheckSumAdjustment(const char* pFontFile)
	{
		CloseFile();

		unsigned long size = ::HY_FileExist(pFontFile);
		if (size == 0)  
			return HY_ERR_FILE_ZERO;	
		if (size == -1)  
			return HY_ERR_FILE_NOEXIST;	
		if (size == -2)  
			return HY_ERR_FILE_STAT;	
		if (size == -3)  
			return HY_ERR_FILE_OTHER;		

		m_pFontFile = fopen(pFontFile,"r+b");
		if (m_pFontFile==0) return -1;

		char* pFileBuf = new char[size];
		fread(pFileBuf,size,1,m_pFontFile);

		unsigned long	ulcheckSum = HY_CalcFontTableChecksum((unsigned long*)pFileBuf, size);
		ulcheckSum = 0xB1B0AFBA -  ulcheckSum;
		ulcheckSum = hy_cdr_int32_to(ulcheckSum);

		fflush(m_pFontFile);
		fseek(m_pFontFile,0,SEEK_SET);
		DecodeTableDirectory();
		int iEntryIndex = m_HYTbDirectory.FindTableEntry(HEAD_TAG);
		if (iEntryIndex!=-1) 
		{			
			CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
			unsigned long HeadTableOffset = tbEntry.offset+8;
			fseek(m_pFontFile,HeadTableOffset,SEEK_SET);

			size_t sz = fwrite(&ulcheckSum,4,1,m_pFontFile);

			CloseFile();			
			
			if (pFileBuf)
				delete[] pFileBuf;
			return HY_NOERROR;
		}

		CloseFile();

		if (pFileBuf)
			delete[] pFileBuf;	

		return FONT_ERR_SETCHECKSUM;

	}	// end of int	CHYFontCodec::SetCheckSumAdjustment()

	int CHYFontCodec::OpenFile(const char* pFontFile)
	{
		int iFilestat = ::HY_FileExist(pFontFile);
		if (iFilestat == 0)  
			return HY_ERR_FILE_ZERO;	
		if (iFilestat == -1)  
			return HY_ERR_FILE_NOEXIST;	
		if (iFilestat == -2)  
			return HY_ERR_FILE_STAT;
		if (iFilestat == -3)  
			return HY_ERR_FILE_OTHER;		

		CloseFile();

		m_pFontFile = fopen(pFontFile,"rb");
		if (m_pFontFile==0)
		{
			_set_errno(0);
			return HY_ERR_FILE_READ;
		}

		return HY_NOERROR;

	}	// end of int CHYFontCodec::OpenFile()

	int	CHYFontCodec::CreateFile(const char* pFontFile)
	{
		CloseFile();

		m_pFontFile = fopen(pFontFile,"w+b");
		if (m_pFontFile == NULL) 
		{
			_set_errno(0);
			return HY_ERR_FILE_WRITE;		
		}

		return HY_NOERROR;

	}	// end of int CHYFontCodec::CreateFile()

	void CHYFontCodec::CloseFile()
	{
		if (m_pFontFile)		
		{
			fflush(m_pFontFile);
			fclose(m_pFontFile);	
			_set_errno(0);
		}
		m_pFontFile = 0;

	}	// end of int	CHYFontCodec::CloseFile()

	int	CHYFontCodec::CollocateGlyf(int iFontType)
	{
		m_vtHYGlyphs.clear();
		if(m_pFontFile)	{
			int iEntryIndex=0;

			int iGlyfNum = m_HYMaxp.numGlyphs;
			if (iGlyfNum == 0) 
				return FONT_ERR_GLYF_DECODE;

			m_vtHYGlyphs.resize(iGlyfNum);
			unsigned  uiThreadadd1=0, uiThreadadd2=0; 
			HANDLE hth1=0, hth2=0; 

			// 解码CMAP表
			hth1 = (HANDLE)_beginthreadex( NULL,         // security  
				0,            // stack size  
				ThreadEntryPtForCode,  
				(void*)this,           // arg list  
				0,  // so we can later call ResumeThread()  //CREATE_SUSPENDED
				&uiThreadadd1);

			// 解码字形表
			if (iFontType == FONTTYPE_TTF )
			{
				Decodeloca();

				iEntryIndex = m_HYTbDirectory.FindTableEntry(GLYF_TAG);				
				if (iEntryIndex != -1){
					CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
					fseek(m_pFontFile,tbEntry.offset,SEEK_SET);
					hth2 = (HANDLE)_beginthreadex( NULL,         // security  
						0,										 // stack size  
						ThreadEntryPtForGlyf,  
						(void*)this,							 // arg list  
						0,										// run
						&uiThreadadd2); 
				}
			}
			else if (iFontType == FONTTYPE_OTF ){
				iEntryIndex = m_HYTbDirectory.FindTableEntry(CFF_TAG);				
				if (iEntryIndex != -1){
					CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
					fseek(m_pFontFile,tbEntry.offset,SEEK_SET);

					hth2 = (HANDLE)_beginthreadex( NULL,         // security  
						0,										// stack size  
						ThreadEntryPtForCFF,  
						(void*)this,							// arg list  
						0,										// run
						&uiThreadadd2);
				}
			}	

			if (hth1){
				WaitForSingleObject(hth1,INFINITE);
			}
			if (hth2){
				WaitForSingleObject(hth2,INFINITE);
			}

			if (hth1)
				CloseHandle(hth1);
			if (hth2)
				CloseHandle(hth2);
		}

		return HY_NOERROR;

	}	// end of int CHYFontCodec::CollocateGlyf()

	int CHYFontCodec::DecodeTableDirectory()
	{	
		m_HYTbDirectory.SetDefault();
		if (m_pFontFile)
		{
			// sfnt version
			fread(&m_HYTbDirectory.version.value,2,1,m_pFontFile);
			m_HYTbDirectory.version.value = hy_cdr_int16_to(m_HYTbDirectory.version.value);
			fread(&m_HYTbDirectory.version.fract,2,1,m_pFontFile);
			m_HYTbDirectory.version.fract = hy_cdr_int16_to(m_HYTbDirectory.version.fract);

			if (m_HYTbDirectory.version.value==0x4f54&&m_HYTbDirectory.version.fract==0x544f)
			{
				m_iFontType=FONTTYPE_OTF;
			}

			if (m_HYTbDirectory.version.value==0x0001&&m_HYTbDirectory.version.fract==0x0000)
			{
				m_iFontType=FONTTYPE_TTF;
			}		

			//numTables
			fread(&m_HYTbDirectory.numTables,2,1,m_pFontFile);
			m_HYTbDirectory.numTables = hy_cdr_int16_to(m_HYTbDirectory.numTables);
			// searchRange
			fread(&m_HYTbDirectory.searchRange,2,1,m_pFontFile);
			m_HYTbDirectory.searchRange = hy_cdr_int16_to(m_HYTbDirectory.searchRange);
			// entrySelector
			fread(&m_HYTbDirectory.entrySelector,2,1,m_pFontFile);
			m_HYTbDirectory.entrySelector = hy_cdr_int16_to(m_HYTbDirectory.entrySelector);
			//rangeShift
			fread(&m_HYTbDirectory.rangeShift, 2,1,m_pFontFile);
			m_HYTbDirectory.rangeShift = hy_cdr_int16_to(m_HYTbDirectory.rangeShift);

			CHYTableEntry	HYEntry;	
			m_mulpTableData.setDefault();
			for (int i=0; i<m_HYTbDirectory.numTables; i++)
			{
				//tag							
				fread(&HYEntry.tag,4,1,m_pFontFile);
				HYEntry.tag = hy_cdr_int32_to(HYEntry.tag);

				// checkSum
				fread(&HYEntry.checkSum,4,1,m_pFontFile);
				HYEntry.checkSum = hy_cdr_int32_to(HYEntry.checkSum);			
				//offset
				fread(&HYEntry.offset,4,1,m_pFontFile);
				HYEntry.offset = hy_cdr_int32_to(HYEntry.offset);				
				//length
				fread(&HYEntry.length,4,1,m_pFontFile);			
				HYEntry.length = hy_cdr_int32_to(HYEntry.length);

				m_HYTbDirectory.vtTableEntry.push_back(HYEntry);

				long curpos = ftell(m_pFontFile);

				TableData  tbData;				
				tbData.iTableFlg = HYEntry.tag;
				tbData.ulDataSize = HYEntry.length;
				fseek(m_pFontFile, HYEntry.offset,SEEK_SET);

				for (int j = 0; j < tbData.ulDataSize; j++)	{
					char p;
					fread(&p, 1, 1, m_pFontFile);
					tbData.vtData.push_back(p);
				}				
				m_mulpTableData.vtTableData.push_back(tbData);
				fseek(m_pFontFile, curpos, SEEK_SET);
			}
			return HY_NOERROR;
		}

		return FONT_ERR_DIRECTORY_DECODE;

	}	// end of int CHYFontCodec::DecodeTableDirectory()

	int	CHYFontCodec::EncodeTableDirectory()
	{
		if (m_pFontFile)
		{
			m_HYTbDirectory.numTables = (unsigned short)m_HYTbDirectory.vtTableEntry.size();	

			unsigned short	searchRange = 0; 
			searchRange = (unsigned short)(log((double)m_HYTbDirectory.numTables)/log(2.0));
			searchRange = (unsigned short)(pow(2.0, searchRange));
			searchRange = searchRange * 16;

			m_HYTbDirectory.searchRange = searchRange;
			m_HYTbDirectory.entrySelector = (unsigned short)(log((float)(m_HYTbDirectory.searchRange/16))/log(2.0));
			m_HYTbDirectory.rangeShift = m_HYTbDirectory.numTables*16 - m_HYTbDirectory.searchRange;

			fseek(m_pFontFile,0,SEEK_SET);

			unsigned short	usTmp;
			unsigned long	ulTmp;
			// sfnt version
			usTmp = hy_cdr_int16_to(m_HYTbDirectory.version.value);
			fwrite(&usTmp,2,1,m_pFontFile);

			usTmp = hy_cdr_int16_to(m_HYTbDirectory.version.fract);
			fwrite(&usTmp,2,1,m_pFontFile);				

			//numTables
			usTmp = hy_cdr_int16_to(m_HYTbDirectory.numTables);
			fwrite(&usTmp,2,1,m_pFontFile);

			// searchRange
			usTmp = hy_cdr_int16_to(m_HYTbDirectory.searchRange);
			fwrite(&usTmp,2,1,m_pFontFile);

			// entrySelector
			usTmp = hy_cdr_int16_to(m_HYTbDirectory.entrySelector);
			fwrite(&usTmp,2,1,m_pFontFile);

			//rangeShift
			usTmp = hy_cdr_int16_to(m_HYTbDirectory.rangeShift);
			fwrite(&usTmp, 2,1,m_pFontFile);

			for (int i=0; i<m_HYTbDirectory.numTables; i++)
			{
				CHYTableEntry&	HYEntry = m_HYTbDirectory.vtTableEntry[i];

				//tag		
				ulTmp = hy_cdr_int32_to(HYEntry.tag);			
				fwrite(&ulTmp,4,1,m_pFontFile);
				// checkSum
				ulTmp = hy_cdr_int32_to(HYEntry.checkSum);			
				fwrite(&ulTmp,4,1,m_pFontFile);		
				//offset
				ulTmp = hy_cdr_int32_to(HYEntry.offset);
				fwrite(&ulTmp,4,1,m_pFontFile);		
				//length
				ulTmp = hy_cdr_int32_to(HYEntry.length);
				fwrite(&ulTmp,4,1,m_pFontFile);
			}
			return HY_NOERROR;
		}		

		return FONT_ERR_DIRECTORY_ENCODE;		

	}	// end of int	CHYFontCodec::EncodeTableDirectory()

	CHYTableEntry*	CHYFontCodec::FindTableEntry(unsigned long ulTableFlg)
	{
		for (size_t i=0; i<m_HYTbDirectory.vtTableEntry.size();i++){
			CHYTableEntry& TableEntry = m_HYTbDirectory.vtTableEntry[i];
			if (TableEntry.tag == ulTableFlg){
				return &TableEntry;
			}
		}

		return NULL;

	}	// end of int CHYFontCodec::FindTableEntry()

	void CHYFontCodec::GetTableFlag(std::vector<unsigned long>& vtFlag)
	{
		vtFlag.clear();
		for (size_t i = 0; i < m_HYTbDirectory.vtTableEntry.size(); i++){
			CHYTableEntry& TableEntry = m_HYTbDirectory.vtTableEntry[i];			
			vtFlag.push_back(TableEntry.tag);
		}
	
	}	// end of void CHYFontCodec::GetTableFlag()

	int	CHYFontCodec::Decodecmap()
	{		
		if (m_pFontFile)
		{
			int iEntryIndex = m_HYTbDirectory.FindTableEntry(CMAP_TAG);
			if (iEntryIndex ==-1)  return FONT_ERR_CMAP_DECODE;
			CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
			fseek(m_pFontFile,tbEntry.offset,SEEK_SET);

			unsigned long  ulTableStartPt =  ftell(m_pFontFile);

			fread(&m_HYCmap.version,2,1,m_pFontFile);
			m_HYCmap.version=hy_cdr_int16_to(m_HYCmap.version);
			fread(&m_HYCmap.numSubTable,2,1,m_pFontFile);
			m_HYCmap.numSubTable=hy_cdr_int16_to(m_HYCmap.numSubTable);

			int i = 0;
			for (i=0; i<m_HYCmap.numSubTable; i++)
			{
				CMAP_TABLE_ENTRY	tbEntry;
				fread(&tbEntry.plat_ID,2,1,m_pFontFile);
				tbEntry.plat_ID=hy_cdr_int16_to(tbEntry.plat_ID);
				fread(&tbEntry.plat_encod_ID,2,1,m_pFontFile);
				tbEntry.plat_encod_ID=hy_cdr_int16_to(tbEntry.plat_encod_ID);
				fread(&tbEntry.offset,4,1,m_pFontFile);
				tbEntry.offset=hy_cdr_int32_to(tbEntry.offset);

				m_HYCmap.vtCamp_tb_entry.push_back(tbEntry);
			}

			for (i=0; i<m_HYCmap.numSubTable; i++)
			{
				CMAP_TABLE_ENTRY&	tbEntry = m_HYCmap.vtCamp_tb_entry[i];			
				fseek(m_pFontFile, ulTableStartPt+tbEntry.offset,SEEK_SET);
				fread(&tbEntry.format,2,1,m_pFontFile);
				tbEntry.format = hy_cdr_int16_to(tbEntry.format);

				switch(tbEntry.format)
				{
					case CMAP_ENCODE_FT_0:
						DecodeCmapFmt0(m_pFontFile,tbEntry);
						break;
					case CMAP_ENCODE_FT_2:
						DecodeCmapFmt2(m_pFontFile,tbEntry);
						break;
					case CMAP_ENCODE_FT_4:
						DecodeCmapFmt4(m_pFontFile,tbEntry);
						break;
					case CMAP_ENCODE_FT_6:
						DecodeCmapFmt6(m_pFontFile,tbEntry);
						break;
					case CMAP_ENCODE_FT_8:
						DecodeCmapFmt8(m_pFontFile,tbEntry);
						break;
					case CMAP_ENCODE_FT_10:
						DecodeCmapFmt10(m_pFontFile,tbEntry);
						break;
					case CMAP_ENCODE_FT_12:
						DecodeCmapFmt12(m_pFontFile,tbEntry);
						break;
					case CMAP_ENCODE_FT_13:
						DecodeCmapFmt13(m_pFontFile,tbEntry);
						break;
					case CMAP_ENCODE_FT_14:
						DecodeCmapFmt14(m_pFontFile,tbEntry);
						break;
					default:
						break;
				}
			}			
		}

		return HY_NOERROR;

	}	// end of int	CHYFontCodec::Decodecmap()

	void CHYFontCodec::Dumpcmap()
	{
	
	}	// end of void CHYFontCodec::Dumpcmap()

	int	CHYFontCodec::Encodecmap(FILE*	pFontFile, CHYTableDirectory& HYTbDirectory)
	{
		unsigned long	ulTmp = 0;
		unsigned short  usTmp = 0;

		if (pFontFile){
			int iEntryIndex = HYTbDirectory.FindTableEntry(CMAP_TAG);
			if (iEntryIndex ==-1)  return FONT_ERR_CMAP_DECODE;

			CHYTableEntry& tbTableEntry = HYTbDirectory.vtTableEntry[iEntryIndex];
			tbTableEntry.offset = ftell(pFontFile);

			usTmp = hy_cdr_int16_to(m_HYCmap.version);
			fwrite(&usTmp,2,1,pFontFile);
			usTmp = hy_cdr_int16_to(m_HYCmap.numSubTable);
			fwrite(&usTmp ,2,1,pFontFile);		
			
			int i = 0;
			for (i=0; i<m_HYCmap.numSubTable; i++){
				CMAP_TABLE_ENTRY&	tbEntry = m_HYCmap.vtCamp_tb_entry[i];
				// platformID
				usTmp = hy_cdr_int16_to(tbEntry.plat_ID);
				fwrite(&usTmp,2,1,pFontFile);
				// encodingID
				usTmp = hy_cdr_int16_to(tbEntry.plat_encod_ID);
				fwrite(&usTmp,2,1,pFontFile);				
				// offset				
				tbEntry.offset = tbTableEntry.offset;
				ulTmp = hy_cdr_int32_to(tbEntry.offset);
				fwrite(&ulTmp,4,1,pFontFile);	
			}

			for (i=0; i<m_HYCmap.numSubTable; i++){
				CMAP_TABLE_ENTRY&	tbEntry = m_HYCmap.vtCamp_tb_entry[i];
				switch(tbEntry.format){
				case CMAP_ENCODE_FT_0:
					EncodeCmapFmt0(pFontFile,tbEntry);
					break;
				case CMAP_ENCODE_FT_2:
					EncodeCmapFmt2(pFontFile,tbEntry);
					break;
				case CMAP_ENCODE_FT_4:
					if (EncodeCmapFmt4(pFontFile,tbEntry) == -1)	{
						EncodeCmapFmt4forGB_CJK(pFontFile,tbEntry);
					}
					break;
				case CMAP_ENCODE_FT_6:
					EncodeCmapFmt6(pFontFile,tbEntry);
					break;
				case CMAP_ENCODE_FT_8:
					EncodeCmapFmt8(pFontFile,tbEntry);
					break;
				case CMAP_ENCODE_FT_10:
					EncodeCmapFmt10(pFontFile,tbEntry);
					break;
				case CMAP_ENCODE_FT_12:
					EncodeCmapFmt12(pFontFile,tbEntry);
					break;
				case CMAP_ENCODE_FT_13:
					EncodeCmapFmt13(pFontFile,tbEntry);
					break;
				case CMAP_ENCODE_FT_14:
					EncodeCmapFmt14(pFontFile,tbEntry);
					break;
				default:
					break;
				}
			}

			unsigned long ulCmapEndPos = ftell(pFontFile);
			fflush(pFontFile);
			fseek(pFontFile, tbTableEntry.offset+4,SEEK_SET);
			for (i=0; i<m_HYCmap.numSubTable; i++){
				CMAP_TABLE_ENTRY&	tbEntry = m_HYCmap.vtCamp_tb_entry[i];
				usTmp = hy_cdr_int16_to(tbEntry.plat_ID);
				fwrite(&usTmp,2,1,pFontFile);
				usTmp = hy_cdr_int16_to(tbEntry.plat_encod_ID);
				fwrite(&usTmp,2,1,pFontFile);				
				ulTmp = hy_cdr_int32_to(tbEntry.offset);
				fwrite(&ulTmp ,4,1,pFontFile);
			}

			fflush(pFontFile);
			fseek(pFontFile,ulCmapEndPos,SEEK_SET);

			tbTableEntry.length = ftell(pFontFile) - tbTableEntry.offset;
			int iTail = tbTableEntry.length%4;
			if (iTail>0){
				char c = 0;
				for (int i=0; i<iTail; i++){
					fwrite(&c,1,1,pFontFile);
				}				
			}

			return HY_NOERROR;
		}

		return FONT_ERR_CMAP_ENCODE;		

	}	// end of int CHYFontCodec::Encodecmap()

	int CHYFontCodec::DecodeCmapFmt0(FILE* pFontFile,CMAP_TABLE_ENTRY& entry)
	{
		if (pFontFile){
			CMAP_ENCODE_FORMAT_0&		CEF0 = entry.Format0;
			CEF0.format = CMAP_ENCODE_FT_0;

			// length
			fread(&CEF0.length,2,1,pFontFile);
			CEF0.length = hy_cdr_int16_to(CEF0.length);
			// language
			fread(&CEF0.language,2,1,pFontFile);
			CEF0.language = hy_cdr_int16_to(CEF0.language);
			// glyphIdArray
			unsigned char GID;
			for (int i=0; i<256; i++){
				fread(&GID,1,1,pFontFile);
				CEF0.vtGlyphId.push_back(GID);
			}
		}

		return HY_NOERROR;

	}	// end of int CHYFontCodec::DecodeCmapFmt0()

	void CHYFontCodec::DumpcmapFmt0()
	{
	
	}	// end of void CHYFontCodec::DumpcmapFmt0()

	int	CHYFontCodec::DecodeCmapFmt2(FILE* pFontFile,CMAP_TABLE_ENTRY& entry)
	{
		CMAP_ENCODE_FORMAT_2&		CEF2 = entry.Format2;
		CEF2.format	=	CMAP_ENCODE_FT_2;

		// length
		fread(&CEF2.length,2,1,pFontFile);
		CEF2.length = hy_cdr_int16_to(CEF2.length);
		//language
		fread(&CEF2.language,2,1,pFontFile);
		CEF2.language = hy_cdr_int16_to(CEF2.language);
		//subHeaderKeys
		int i=0;
		unsigned short uskey=0;
		for (i=0; i<256; i++){		
			fread(&uskey,2,1,pFontFile);
			uskey = hy_cdr_int16_to(uskey);
			CEF2.vtsubHeaderKeys.push_back(uskey);		
		}

		//subHeaders
		int iMaxIndex = 0;
		for (i=0; i<256; i++){
			uskey = CEF2.vtsubHeaderKeys[i]/8;
			if (iMaxIndex<uskey){
				iMaxIndex=uskey;
			}
		}	
		int SubHeadLen = iMaxIndex+1;
		for (i=0; i<SubHeadLen; i++){
			CMAP_ENCODE_FORMAT_2_SUBHEAD  subHead;

			fread(&subHead.firstCode,2,1,pFontFile);
			subHead.firstCode = hy_cdr_int16_to(subHead.firstCode);
			fread(&subHead.entryCount,2,1,pFontFile);
			subHead.entryCount = hy_cdr_int16_to(subHead.entryCount);
			fread(&subHead.idDelta,2,1,pFontFile);
			subHead.idDelta = hy_cdr_int16_to(subHead.idDelta);
			fread(&subHead.idRangeOffset,2,1,pFontFile);
			subHead.idRangeOffset = hy_cdr_int16_to(subHead.idRangeOffset);
			if (subHead.idRangeOffset != 0)	{
				subHead.idRangeOffset -= 2+SubHeadLen-i-1*8;
			}
			CEF2.vtSubHead.push_back(subHead);
		}

		//glyphIndexArray
		int numGlyphId = (CEF2.length-(256+3)*2-SubHeadLen*8)/2;
		for (i=0; i<numGlyphId;i++)	{
			fread(&uskey,2,1,pFontFile);
			uskey = hy_cdr_int16_to(uskey);
			CEF2.vtglyphIndex.push_back(uskey);
		}

		return HY_NOERROR;

	}	// end of int	CHYFontCodec::DecodeCmapFmt2()

	void CHYFontCodec::DumpcmapFmt2()
	{
	
	}	// end of void CHYFontCodec::DumpcmapFmt2()

	int	CHYFontCodec::DecodeCmapFmt4(FILE* pFontFile, CMAP_TABLE_ENTRY& entry)
	{
		CMAP_ENCODE_FORMAT_4&		CEF4 = entry.Format4;
		CEF4.format = CMAP_ENCODE_FT_4;

		unsigned long ulStart = ftell(pFontFile)-2;
		fread(&CEF4.length,2,1,pFontFile);
		CEF4.length=hy_cdr_int16_to(CEF4.length);
		fread(&CEF4.language,2,1,pFontFile);
		CEF4.language=hy_cdr_int16_to(CEF4.language);
		fread(&CEF4.segCountX2,2,1,pFontFile);
		CEF4.segCountX2=hy_cdr_int16_to(CEF4.segCountX2);
		fread(&CEF4.searchRange,2,1,pFontFile);
		CEF4.searchRange=hy_cdr_int16_to(CEF4.searchRange);
		fread(&CEF4.entrySelector,2,1,pFontFile);
		CEF4.entrySelector=hy_cdr_int16_to(CEF4.entrySelector);
		fread(&CEF4.rangeShift,2,1,pFontFile);
		CEF4.rangeShift=hy_cdr_int16_to(CEF4.rangeShift);

		int i=0;
		unsigned short usTmp=0;
		for (i=0; i<CEF4.segCountX2>>1;i++){
			fread(&usTmp,2,1,pFontFile);
			usTmp=hy_cdr_int16_to(usTmp);
			CEF4.vtEndCount.push_back(usTmp);
		}
		fread(&CEF4.reservedPad,2,1,pFontFile);
		CEF4.reservedPad=hy_cdr_int16_to(CEF4.reservedPad);

		for (i=0; i<CEF4.segCountX2>>1;i++){
			fread(&usTmp,2,1,pFontFile);
			usTmp=hy_cdr_int16_to(usTmp);
			CEF4.vtstartCount.push_back(usTmp);
		}

		for (i=0; i<CEF4.segCountX2>>1;i++){
			fread(&usTmp,2,1,pFontFile);
			usTmp=hy_cdr_int16_to(usTmp);
			CEF4.vtidDelta.push_back(usTmp);
		}

		for (i=0; i<CEF4.segCountX2>>1;i++){
			fread(&usTmp,2,1,pFontFile);
			usTmp=hy_cdr_int16_to(usTmp);
			CEF4.vtidRangeOffset.push_back(usTmp);
		}

		unsigned long ulCurrent = ftell(pFontFile);
		int ipglyphIdArrayLen  = (CEF4.length-(ulCurrent-ulStart))/2;	
		for (i=0; i<ipglyphIdArrayLen; i++){
			fread(&usTmp,2,1,pFontFile);
			usTmp=hy_cdr_int16_to(usTmp);
			CEF4.vtglyphIdArray.push_back(usTmp);
		}		

		return HY_NOERROR;

	}	// end of int	CHYFontCodec::DecodeCmapFmt4()

	void CHYFontCodec::DumpcmapFmt4()
	{
	
	}	// end of void CHYFontCodec::DumpcmapFmt4()

	int	CHYFontCodec::DecodeCmapFmt6(FILE* pFontFile,CMAP_TABLE_ENTRY& entry)
	{
		CMAP_ENCODE_FORMAT_6&			CEF6 = entry.Format6;
		CEF6.format = CMAP_ENCODE_FT_6;

		fread(&CEF6.length,2,1,pFontFile);
		CEF6.length = hy_cdr_int16_to(CEF6.length);

		fread(&CEF6.language,2,1,pFontFile);
		CEF6.language = hy_cdr_int16_to(CEF6.language);

		fread(&CEF6.firstCode,2,1,pFontFile);
		CEF6.firstCode = hy_cdr_int16_to(CEF6.firstCode);

		fread(&CEF6.entryCount,2,1,pFontFile);
		CEF6.entryCount = hy_cdr_int16_to(CEF6.entryCount);

		unsigned short usGid;
		for (int i=0; i<CEF6.entryCount;i++){
			fread(&usGid,2,1,pFontFile);
			usGid=hy_cdr_int16_to(usGid);

			CEF6.vtGlyphId.push_back(usGid);
		}

		return HY_NOERROR;

	}	// end of int	CHYFontCodec::DecodeCmapFmt6()

	void CHYFontCodec::DumpcmapFmt6()
	{
	
	}	//end of void CHYFontCodec::DumpcmapFmt6()

	int	CHYFontCodec::DecodeCmapFmt8(FILE* pFontFile,CMAP_TABLE_ENTRY& entry)
	{
		return HY_NOERROR;

	}	// end of int	CHYFontCodec::DecodeCmapFmt8()

	void CHYFontCodec::DumpcmapFmt8()
	{

	}	// end of void CHYFontCodec::DumpcmapFmt8()

	int	CHYFontCodec::DecodeCmapFmt10(FILE* pFontFile,CMAP_TABLE_ENTRY& entry)
	{
		return HY_NOERROR;

	}	// end of int	CHYFontCodec::DecodeCmapFmt10()

	void CHYFontCodec::DumpcmapFmt10()
	{
	
	}	// end of void CHYFontCodec::DumpcmapFmt10()

	int	CHYFontCodec::DecodeCmapFmt12(FILE* pFontFile,CMAP_TABLE_ENTRY& entry)
	{
		CMAP_ENCODE_FORMAT_12&			CEF12 = entry.Format12;
		CEF12.format = CMAP_ENCODE_FT_12;
		fread(&CEF12.reserved,2,1,pFontFile);
		CEF12.reserved = hy_cdr_int16_to(CEF12.reserved);
		fread(&CEF12.length,4,1,pFontFile);
		CEF12.length = hy_cdr_int32_to(CEF12.length);
		fread(&CEF12.language,4,1,pFontFile);
		CEF12.language = hy_cdr_int32_to(CEF12.language);
		fread(&CEF12.nGroups,4,1,pFontFile);
		CEF12.nGroups = hy_cdr_int32_to(CEF12.nGroups);

		for (unsigned long i=0; i<CEF12.nGroups; i++){

			CMAP_ENCODE_FORMAT_12_GROUP  group;
			fread(&group.startCharCode,4,1,pFontFile);
			group.startCharCode = hy_cdr_int32_to(group.startCharCode);
			fread(&group.endCharCode,4,1,pFontFile);
			group.endCharCode = hy_cdr_int32_to(group.endCharCode);
			fread(&group.startGlyphID,4,1,pFontFile);
			group.startGlyphID = hy_cdr_int32_to(group.startGlyphID);
			CEF12.vtGroup.push_back(group);
		}

		return HY_NOERROR;

	}	// end of int	CHYFontCodec::DecodeCmapFmt12()

	void CHYFontCodec::DumpcmapFmt12()
	{
	
	}	// end of void CHYFontCodec::DumpcmapFmt12()

	int	CHYFontCodec::DecodeCmapFmt13(FILE* pFontFile,CMAP_TABLE_ENTRY& entry)
	{

		return HY_NOERROR;

	}	// end of int	CHYFontCodec::DecodeCmapFmt13()

	void CHYFontCodec::DumpcmapFmt13()
	{
	
	}	// end of void CHYFontCodec::DumpcmapFmt13()

	int	CHYFontCodec::DecodeCmapFmt14(FILE* pFontFile,CMAP_TABLE_ENTRY& entry)
	{
		CMAP_ENCODE_FORMAT_14& CEF14 = entry.Format14;
		CEF14.format = CMAP_ENCODE_FT_14;

		unsigned long ulStart = ftell(pFontFile) - 2;
		fread(&CEF14.length, 4, 1, pFontFile);
		CEF14.length = hy_cdr_int32_to(CEF14.length);

		fread(&CEF14.numVarSelectorRecords, 4, 1, pFontFile);
		CEF14.numVarSelectorRecords = hy_cdr_int32_to(CEF14.numVarSelectorRecords);

		for (int i = 0; i < CEF14.numVarSelectorRecords; i++) {
			CVariationSelector		vrSltr;
			fread(&vrSltr.varSelector[0], 1, 1, pFontFile);
			fread(&vrSltr.varSelector[1], 1, 1, pFontFile);
			fread(&vrSltr.varSelector[2], 1, 1, pFontFile);

			fread(&vrSltr.defaultUVSOffset, 4, 1, pFontFile);
			vrSltr.defaultUVSOffset = hy_cdr_int32_to(vrSltr.defaultUVSOffset);
			fread(&vrSltr.nonDefaultUVSOffset, 4, 1, pFontFile);
			vrSltr.nonDefaultUVSOffset = hy_cdr_int32_to(vrSltr.nonDefaultUVSOffset);

			unsigned long ulCur = ftell(pFontFile);

			if (vrSltr.defaultUVSOffset > 0) {
				fseek(pFontFile, ulStart + vrSltr.defaultUVSOffset, SEEK_SET);
				fread(&vrSltr.DefaultUVS.numUnicodeValueRanges, 4, 1, pFontFile);
				vrSltr.DefaultUVS.numUnicodeValueRanges = hy_cdr_int32_to(vrSltr.DefaultUVS.numUnicodeValueRanges);
				for (int df = 0; df < vrSltr.DefaultUVS.numUnicodeValueRanges; df++) {
					CUnicodeRange		uniRng;
					fread(&uniRng.startUnicodeValue[0], 1, 1, pFontFile);
					fread(&uniRng.startUnicodeValue[1], 1, 1, pFontFile);
					fread(&uniRng.startUnicodeValue[2], 1, 1, pFontFile);
					fread(&uniRng.additionalCount, 1, 1, pFontFile);
					vrSltr.DefaultUVS.vtranges.push_back(uniRng);
				}
			}
			if (vrSltr.nonDefaultUVSOffset > 0)	{
				fseek(pFontFile, ulStart + vrSltr.nonDefaultUVSOffset, SEEK_SET);
				fread(&vrSltr.NonDefaultUVS.numUVSMappings, 4, 1, pFontFile);
				vrSltr.NonDefaultUVS.numUVSMappings = hy_cdr_int32_to(vrSltr.NonDefaultUVS.numUVSMappings);
				for (int mp = 0; mp < vrSltr.NonDefaultUVS.numUVSMappings; mp++) {
					CUVSMapping 		mpp;
					fread(&mpp.unicodeValue[0], 1, 1, pFontFile);
					fread(&mpp.unicodeValue[1], 1, 1, pFontFile);
					fread(&mpp.unicodeValue[2], 1, 1, pFontFile);
					fread(&mpp.glyphID, 2, 1, pFontFile);
					mpp.glyphID = hy_cdr_int16_to(mpp.glyphID);
					vrSltr.NonDefaultUVS.vtuvsMappings.push_back(mpp);
				}
			}
			fseek(pFontFile, ulCur,SEEK_SET);
			CEF14.vtVariationSelector.push_back(vrSltr);
		}

		return HY_NOERROR;

	}	// end of int CHYFontCodec::DecodeCmapFmt14()

	void CHYFontCodec::DumpcmapFmt14()
	{
	
	}	// end of void CHYFontCodec::DumpcmapFmt14()

	int CHYFontCodec::EncodeCmapFmt0(FILE* pFontFile,CMAP_TABLE_ENTRY& entry)
	{	
		std::vector<unsigned char> vtCmap;
		if (pFontFile){
			if (vtCmap.size()==0){
				CMAP_ENCODE_FORMAT_0&		CEF0 = entry.Format0;
				CEF0.format = CMAP_ENCODE_FT_0;			
				vtCmap.push_back(CEF0.format>>0);
				vtCmap.push_back(CEF0.format&0x0f);

				// length
				CEF0.length = 262;
				vtCmap.push_back(CEF0.length>>0);
				vtCmap.push_back(CEF0.length&0x0f);

				// language
				CEF0.language = 0;
				vtCmap.push_back(CEF0.language>>0);
				vtCmap.push_back(CEF0.language&0x0f);

				// glyphIdArray
				if (m_HYCodeMap.vtHYCodeMap.size() >= 256 )	{
					// 第一个必须是GID == 0;
					vtCmap.push_back(0);
					vtCmap.push_back(0);

					for (int i=1; i<256; i++){
						std::vector<int> vtGIDs;
						int iGID = m_HYCodeMap.FindIndexByUnicode(i);
						if (iGID>0&&iGID<256){
							vtCmap.push_back(iGID>>8);
							vtCmap.push_back(iGID&0x0f);
						}					
						else{
							vtCmap.push_back(0);
							vtCmap.push_back(0);
						}
					}				
				}
			}		

			entry.offset = ftell(pFontFile)-entry.offset;
			for(size_t i=0; i<vtCmap.size(); i++){
				fwrite(&vtCmap[i],1,1,pFontFile);						
			}			
		}
		
		return HY_NOERROR;

	}	// end of int CHYFontCodec::EncodeCmapFmt0()

	int	CHYFontCodec::EncodeCmapFmt2(FILE* pFontFile,CMAP_TABLE_ENTRY& entry)
	{
		std::vector<unsigned char> vtCmap;

		return HY_NOERROR;

	}	// end of int	CHYFontCodec::EncodeCmapFmt2()

	int	CHYFontCodec::EncodeCmapFmt4forGB_CJK(FILE* pFontFile,CMAP_TABLE_ENTRY& entry)
	{
		std::vector<unsigned char> vtCmap;
		if (pFontFile){
			if (vtCmap.size()==0){
				std::list<GroupRecord>		groupList;		
				int		eov = m_HYCodeMap.vtHYCodeMap.size();
				int		ix = 0;

				while (ix != eov && m_HYCodeMap.vtHYCodeMap[ix].ulGlyphNo <= 0xffff){
					GroupRecord				group;
					int						nextIx = NextGroup(ix, eov, group);		

					// This format does not code above 0xfffe
					if (group.startCode < 0xffff){	
						if (group.endCode > 0xfffe)
							group.endCode = 0xfffe;

						groupList.push_back (group);
					}
					ix = nextIx;
				}

				std::list <GroupRecord>::iterator		gix = groupList.begin();
				std::list <GroupRecord>::iterator		glast = groupList.begin();
				if (gix != groupList.end())
					++gix;
#if 1
				while (gix != groupList.end()){
					unsigned long		curCodeCount = gix->endCode - gix->startCode + 1;
					unsigned long		lastCodeCount = glast->endCode - glast->startCode + 1;

					// These constants are empirical. They give the most compact results for CJK-style encodings.                  
					if (gix->startCode - glast->endCode < 4){
						if (!glast->contiguous && curCodeCount < 5 ||
							gix->contiguous == glast->contiguous &&
							(	!gix->contiguous ||
							static_cast <int> (gix->startCode) -
							static_cast <int> (static_cast <int> (gix->startGID)) ==
							static_cast <int> (glast->startCode) -
							static_cast <int> (static_cast <int> (glast->startGID)))){
							glast->contiguous = false;
							glast->endCode = gix->endCode;
							groupList.erase (gix);
							gix = glast;
						}
						else if (glast->contiguous && lastCodeCount < 5){
							// Don't bother with the contiguous nature of the last run because it's so short. Consolidate.
							glast->contiguous = false;
							glast->endCode = gix->endCode;
							groupList.erase (gix);
							gix = glast;
						}
					}
					glast = gix;
					++gix;
				}			
#endif 
				std::list <GroupRecord>::iterator		gend = groupList.end();
				ULONG									lastCode = 0;
				int										entryCount = 0;
				int										groupCount = 0;

				gix = groupList.begin();
				while (gix != gend){
					if (!gix->contiguous)
						entryCount += gix->endCode - gix->startCode+1;

					lastCode = gix->endCode;
					++gix;
					groupCount+=1;
				}

				if (lastCode!=0xffff)		// We are required to terminate the table with a group containing 0xffff
					groupCount+=1;

				size_t					length = 2*(8+groupCount*4+entryCount);
				if (length>65535)	
					return -1;

				std::vector <unsigned short>	startTable (groupCount);
				std::vector <unsigned short>	endTable (groupCount);
				std::vector <short>				idDelta (groupCount);
				std::vector <unsigned short>	idRangeOffset (groupCount);
				std::vector <unsigned short>	glyphIdArray;
				size_t							groupIx = 0;
				entryCount = 0;

				gix = groupList.begin();

				while (gix != gend){
					size_t				entries = gix->endCode - gix->startCode + 1;
					startTable [groupIx] = (unsigned short)gix->startCode;
					endTable [groupIx] = (unsigned short)gix->endCode;

					if (gix->contiguous){
						// If the glyphIDs in the range are consecutive then we don't need entries in the GlyphIdArray.
						idDelta [groupIx] = (short)gix->startGID - (short)gix->startCode;
						idRangeOffset [groupIx] = 0xffff;
					}
					else{
						//Theoretically the table can be compressed by finding multiple ranges with the same differences between each
						// code point in the range and the corresponding glyphID, and then adjusting the bias via idDelta. We're not going to
						// bother. It's computationally expensive and probably doesn't yield much savings for most circumstances.
						idDelta [groupIx] = 0;
						idRangeOffset [groupIx] = (unsigned short)entryCount;

						unsigned long		code = gix->startCode;
						for (size_t i=0; i<entries; ++i){
							int iGID = m_HYCodeMap.FindIndexByUnicode(code++);
							if (iGID!=-1)
								glyphIdArray.push_back((unsigned short)iGID);
							else
								glyphIdArray.push_back(0);
						}

						if (entryCount+entries > 0xffff) 
							return -1;

						entryCount += static_cast <unsigned short> (entries);
					}

					++groupIx;
					++gix;
				}

				if (lastCode != 0xffff){
					startTable [groupIx]	= 0xffff;
					endTable [groupIx]		= 0xffff;
					idDelta [groupIx]		= 1;
					idRangeOffset [groupIx] = 0xffff;
				}

				//format
				entry.Format4.format = CMAP_ENCODE_FT_4;
				vtCmap.push_back(entry.Format4.format>>8);
				vtCmap.push_back(entry.Format4.format&0x00ff);

				//length
				entry.Format4.length = (unsigned short)length;
				vtCmap.push_back(entry.Format4.length>>8);
				vtCmap.push_back(entry.Format4.length&0x00ff);

				//language
				entry.Format4.language = 0;
				vtCmap.push_back(0);
				vtCmap.push_back(0);

				//segCountX2
				entry.Format4.segCountX2	= (unsigned short)groupCount*2;
				vtCmap.push_back(entry.Format4.segCountX2>>8);
				vtCmap.push_back(entry.Format4.segCountX2&0x00ff);

				//searchRange
				entry.Format4.searchRange	= (unsigned short)(2* pow(2,floor((log((double)groupCount)/log(2.0))))); 
				vtCmap.push_back(entry.Format4.searchRange>>8);
				vtCmap.push_back(entry.Format4.searchRange&0x00ff);

				//entrySelector
				entry.Format4.entrySelector		=	(unsigned short) (log(entry.Format4.searchRange / 2.0)/log(2.0));
				vtCmap.push_back(entry.Format4.entrySelector>>8);
				vtCmap.push_back(entry.Format4.entrySelector&0x00ff);

				//rangeShift
				entry.Format4.rangeShift		=	entry.Format4.segCountX2-entry.Format4.searchRange;
				vtCmap.push_back(entry.Format4.rangeShift>>8);
				vtCmap.push_back(entry.Format4.rangeShift&0x00ff);

				//endCount
				unsigned short usTmp;
				for (int i=0; i<groupCount; i++){
					usTmp = endTable[i];

					vtCmap.push_back(usTmp>>8);
					vtCmap.push_back(usTmp&0x00ff);				
				}

				//reservedPad
				entry.Format4.reservedPad		=	0;
				vtCmap.push_back(0);
				vtCmap.push_back(0);

				//startCount
				for (int i=0; i<groupCount; i++){
					usTmp = startTable[i];

					vtCmap.push_back(usTmp>>8);
					vtCmap.push_back(usTmp&0x00ff);				
				}	

				//idDelta
				for (int i=0; i<groupCount; i++){
					usTmp = idDelta[i];

					vtCmap.push_back(usTmp>>8);
					vtCmap.push_back(usTmp&0x00ff);				
				}					

				//idRangeOffset
				for (int i=0; i<groupCount; i++){
					if (idRangeOffset[i] == 0xffff){
						usTmp = 0;
					}
					else{
						usTmp = static_cast <unsigned short> (2*(idRangeOffset[i]+groupCount-i));						
					}
					vtCmap.push_back(usTmp>>8);
					vtCmap.push_back(usTmp&0x00ff);				
				}			

				//glyphIdArray
				for (size_t i = 0; i < glyphIdArray.size(); i++){		
					usTmp = glyphIdArray[i];
					vtCmap.push_back(usTmp>>8);
					vtCmap.push_back(usTmp&0x00ff);					
				}
			}			 

			entry.offset = ftell(pFontFile)-entry.offset;
			for(size_t i=0; i<vtCmap.size(); i++){
				fwrite(&vtCmap[i],1,1,pFontFile);						
			}			
		}

		return HY_NOERROR;
	
	} // end of int	CHYFontCodec::EncodeCmapFmt4forGB_CJK()

	int	CHYFontCodec::EncodeCmapFmt4(FILE* pFontFile, CMAP_TABLE_ENTRY& entry)
	{
		std::vector<unsigned char> vtCmap;
		if (pFontFile){
			if (vtCmap.size()==0){
				std::list<GroupRecord>		groupList;		
				int		eov = m_HYCodeMap.vtHYCodeMap.size();
				int		ix = 0;
				while (ix != eov && m_HYCodeMap.vtHYCodeMap[ix].ulGlyphNo <= 0xffff){
					GroupRecord				group;
					int						nextIx = NextGroup(ix, eov, group);		

					if (group.startCode < 0xffff){
						// This format does not code above 0xfffe
						if (group.endCode > 0xfffe)
							group.endCode = 0xfffe;

						groupList.push_back (group);
					}
					ix = nextIx;
				}

				std::list <GroupRecord>::iterator		gix = groupList.begin();
				std::list <GroupRecord>::iterator		glast = groupList.begin();
				if (gix != groupList.end())
					++gix;

				std::list <GroupRecord>::iterator		gend = groupList.end();
				ULONG									lastCode = 0;
				int										entryCount = 0;
				int										groupCount = 0;

				gix = groupList.begin();
				while (gix != gend)	{
					if (!gix->contiguous)
						entryCount += gix->endCode - gix->startCode+1;

					lastCode = gix->endCode;
					++gix;
					groupCount+=1;
				}

				if (lastCode!=0xffff)		// We are required to terminate the table with a group containing 0xffff
					groupCount+=1;

				size_t					length = 2*(8+groupCount*4+entryCount);
				if (length>65535)	
					return -1;

				std::vector <unsigned short>	startTable (groupCount);
				std::vector <unsigned short>	endTable (groupCount);
				std::vector <short>				idDelta (groupCount);
				std::vector <unsigned short>	idRangeOffset (groupCount);
				std::vector <unsigned short>	glyphIdArray;
				size_t							groupIx = 0;
				entryCount = 0;
				gix = groupList.begin();

				while (gix != gend){
					size_t				entries = gix->endCode - gix->startCode + 1;
					startTable [groupIx] = (unsigned short)gix->startCode;
					endTable [groupIx] = (unsigned short)gix->endCode;

					if (gix->contiguous){
						// If the glyphIDs in the range are consecutive then we don't need entries in the GlyphIdArray.
						idDelta [groupIx] = (short)gix->startGID - (short)gix->startCode;
						idRangeOffset [groupIx] = 0xffff;
					}
					else{
						//Theoretically the table can be compressed by finding multiple ranges with the same differences between each
						// code point in the range and the corresponding glyphID, and then adjusting the bias via idDelta. We're not going to
						// bother. It's computationally expensive and probably doesn't yield much savings for most circumstances.
						idDelta [groupIx] = 0;
						idRangeOffset [groupIx] = (unsigned short)entryCount;
						unsigned long		code = gix->startCode;
						for (size_t i=0; i<entries; ++i){
							int iGID = m_HYCodeMap.FindIndexByUnicode(code++);
							if (iGID!=-1)
								glyphIdArray.push_back((unsigned short)iGID);
							else
								glyphIdArray.push_back(0);
						}

						if (entryCount+entries > 0xffff) 
							return -1;

						entryCount += static_cast <unsigned short> (entries);
					}
					++groupIx;
					++gix;
				}

				if (lastCode != 0xffff)	{
					startTable [groupIx]	= 0xffff;
					endTable [groupIx]		= 0xffff;
					idDelta [groupIx]		= 1;
					idRangeOffset [groupIx] = 0xffff;
				}

				//format
				entry.Format4.format = CMAP_ENCODE_FT_4;
				vtCmap.push_back(entry.Format4.format>>8);
				vtCmap.push_back(entry.Format4.format&0x00ff);
				
				//length
				entry.Format4.length = (unsigned short)length;
				vtCmap.push_back(entry.Format4.length>>8);
				vtCmap.push_back(entry.Format4.length&0x00ff);

				//language
				entry.Format4.language = 0;
				vtCmap.push_back(0);
				vtCmap.push_back(0);

				//segCountX2
				entry.Format4.segCountX2	= (unsigned short)groupCount*2;
				vtCmap.push_back(entry.Format4.segCountX2>>8);
				vtCmap.push_back(entry.Format4.segCountX2&0x00ff);

				//searchRange
				entry.Format4.searchRange	= (unsigned short)(2* pow(2,floor((log((double)groupCount)/log(2.0))))); 
				vtCmap.push_back(entry.Format4.searchRange>>8);
				vtCmap.push_back(entry.Format4.searchRange&0x00ff);
				
				//entrySelector
				entry.Format4.entrySelector		=	(unsigned short) (log(entry.Format4.searchRange / 2.0)/log(2.0));
				vtCmap.push_back(entry.Format4.entrySelector>>8);
				vtCmap.push_back(entry.Format4.entrySelector&0x00ff);

				//rangeShift
				entry.Format4.rangeShift		=	entry.Format4.segCountX2-entry.Format4.searchRange;
				vtCmap.push_back(entry.Format4.rangeShift>>8);
				vtCmap.push_back(entry.Format4.rangeShift&0x00ff);

				//endCount
				unsigned short usTmp;
				for (int i=0; i<groupCount; i++){
					usTmp = endTable[i];
					vtCmap.push_back(usTmp>>8);
					vtCmap.push_back(usTmp&0x00ff);				
				}

				//reservedPad
				entry.Format4.reservedPad		=	0;
				vtCmap.push_back(0);
				vtCmap.push_back(0);

				//startCount
				for (int i=0; i<groupCount; i++){
					usTmp = startTable[i];
					vtCmap.push_back(usTmp>>8);
					vtCmap.push_back(usTmp&0x00ff);				
				}	

				//idDelta
				for (int i=0; i<groupCount; i++){
					usTmp = idDelta[i];
					vtCmap.push_back(usTmp>>8);
					vtCmap.push_back(usTmp&0x00ff);				
				}					

				//idRangeOffset
				for (int i=0; i<groupCount; i++){
					if (idRangeOffset[i] == 0xffff)	{
						usTmp = 0;
					}
					else {
						usTmp = static_cast <unsigned short> (2*(idRangeOffset[i]+groupCount-i));						
					}					

					vtCmap.push_back(usTmp>>8);
					vtCmap.push_back(usTmp&0x00ff);				
				}			
				
				//glyphIdArray
				for (size_t i = 0; i < glyphIdArray.size(); i++){		
					usTmp = glyphIdArray[i];
					vtCmap.push_back(usTmp>>8);
					vtCmap.push_back(usTmp&0x00ff);					
				}
			}
			
			entry.offset = ftell(pFontFile)-entry.offset;
			for(size_t i=0; i<vtCmap.size(); i++){
				fwrite(&vtCmap[i],1,1,pFontFile);						
			}			
		}

		return HY_NOERROR;

	}	// end of int	CHYFontCodec::EncodeCmapFmt4()

	int CHYFontCodec::NextGroup(int ix, int eov, GroupRecord& group) 
	{
		int				entryCount = 0;
		bool			haveContiguous = false;		// Neither contiguous nor discontiguous yet
		bool			contiguous = true;			// Default to true for single glyph range.

		std::vector<CHYCodeMapItem>& vtMapItm = m_HYCodeMap.vtHYCodeMap;
		if (ix != eov){
			unsigned long 		startCharCode = vtMapItm[ix].ulGlyphNo;
			int					startGID = vtMapItm[ix].iGlyphIndex;
			int					hold = ix;
			++ix;
			++entryCount;

			group.startCode = startCharCode;
			group.startGID = startGID;
			group.endCode = startCharCode;

			while (ix!=eov && vtMapItm[ix].ulGlyphNo == startCharCode+1){
				int			gid  = vtMapItm[ix].iGlyphIndex;
				bool		successive = gid == startGID+1;

				startCharCode = vtMapItm[ix].ulGlyphNo;
				++group.endCode;
				if (haveContiguous)	{
					if (successive != contiguous){
						ix = hold;
						entryCount -= 1;
						group.endCode -= 1;
						break;
					}
				}
				else
					contiguous = successive;

				haveContiguous = true;
				startGID = gid;
				hold = ++ix;
				++entryCount;
			}
		}
		group.contiguous = contiguous;

		return ix;

	}	// end of CHYFontCodec::NextGroup () 


	int	CHYFontCodec::EncodeCmapFmt6(FILE* pFontFile,CMAP_TABLE_ENTRY& entry)
	{
		std::vector<unsigned char> vtCmap;
		if (pFontFile){
			unsigned long psBegin = ftell(pFontFile);
			entry.offset = psBegin - entry.offset;

			unsigned short usTmp = hy_cdr_int16_to(entry.format);
			fwrite(&usTmp, 2, 1, m_pFontFile);
			usTmp = 0;
			fwrite(&usTmp, 2, 1, m_pFontFile);
			usTmp = hy_cdr_int16_to(entry.Format6.language);
			fwrite(&usTmp, 2, 1, m_pFontFile);
			usTmp = hy_cdr_int16_to(entry.Format6.firstCode);
			fwrite(&usTmp, 2, 1, m_pFontFile);
			usTmp = hy_cdr_int16_to(entry.Format6.entryCount);
			fwrite(&usTmp, 2, 1, m_pFontFile);
			for (int i = 0; i < entry.Format6.entryCount; i++){
				usTmp = hy_cdr_int16_to(entry.Format6.vtGlyphId[i]);
				fwrite(&usTmp, 2, 1, m_pFontFile);
			}

			unsigned long psEnd = ftell(pFontFile);
			entry.Format6.length = psEnd - psBegin;
			fseek(pFontFile, psBegin+2,SEEK_SET);
			usTmp = hy_cdr_int16_to(entry.Format6.length);
			fwrite(&usTmp, 2, 1, m_pFontFile);
			fseek(pFontFile, psEnd, SEEK_SET);
		}

		return HY_NOERROR;

	}	//end of int	CHYFontCodec::EncodeCmapFmt6()

	int	CHYFontCodec::EncodeCmapFmt8(FILE* pFontFile,CMAP_TABLE_ENTRY& entry)
	{
		std::vector<unsigned char> vtCmap;

		return HY_NOERROR;
	}

	int	CHYFontCodec::EncodeCmapFmt10(FILE* pFontFile,CMAP_TABLE_ENTRY& entry)
	{
		std::vector<unsigned char> vtCmap;
	
		return HY_NOERROR;
	}

	int	CHYFontCodec::EncodeCmapFmt12(FILE* pFontFile,CMAP_TABLE_ENTRY& entry)
	{
		std::vector<unsigned char> vtCmap;

		if (pFontFile){
			if (vtCmap.size() == 0)	{
				//std::vector<CMAP_ENCODE_F12_GROUP> sztagF12Group;
				int iGlyphIndex	= -1, iExpectGlyphIndex = -1;
				ULONG ulUnicode	= 0, ulExpectUnicode = 0;
				size_t i = 0, j = 0;

				size_t iCharNumber = m_HYCodeMap.vtHYCodeMap.size(); 
				while (i<iCharNumber){
					CMAP_ENCODE_FORMAT_12_GROUP	 tagF12Group;
					CHYCodeMapItem& HYMapItem	= m_HYCodeMap.vtHYCodeMap[i];

					iGlyphIndex					= HYMapItem.iGlyphIndex;
					ulUnicode					= HYMapItem.ulGlyphNo;

					tagF12Group.startCharCode = ulUnicode;
					tagF12Group.startGlyphID = iGlyphIndex;
					for ( j=i+1; j<iCharNumber; j++ ){
						CHYCodeMapItem& HYMapItem	= m_HYCodeMap.vtHYCodeMap[j];
						iExpectGlyphIndex	= HYMapItem.iGlyphIndex;
						ulExpectUnicode		= HYMapItem.ulGlyphNo;

						if (iExpectGlyphIndex == 0) continue;

						iGlyphIndex++;
						ulUnicode++;
						// Index和unicode都必须是连续的,否则段结束
						if (iExpectGlyphIndex != iGlyphIndex || ulExpectUnicode != ulUnicode){
							tagF12Group.endCharCode = ulUnicode - 1;
							entry.Format12.vtGroup.push_back(tagF12Group);							
							i = j;
							break;
						}
					}

					// 数组末尾
					if (iCharNumber==1)	{
						tagF12Group.startCharCode = 65535;
						tagF12Group.endCharCode = 65535;
						tagF12Group.startGlyphID = 0;
						entry.Format12.vtGroup.push_back(tagF12Group);
						break;

					}					
					else if (j==iCharNumber){
						if (tagF12Group.endCharCode==0){
							if (m_HYCodeMap.vtHYCodeMap[j-1].iGlyphIndex == 0)								
								tagF12Group.endCharCode = m_HYCodeMap.vtHYCodeMap[j-2].ulGlyphNo;
							else 
								tagF12Group.endCharCode = m_HYCodeMap.vtHYCodeMap[j-1].ulGlyphNo;

							entry.Format12.vtGroup.push_back(tagF12Group);
						}
						break;
					}
				}

				// 向CMAP_ENCODE_F12赋值	
				entry.Format12.format =CMAP_ENCODE_FT_12;
				vtCmap.push_back((entry.Format12.format&0xff00)>>8);
				vtCmap.push_back(entry.Format12.format&0x00ff);

				entry.Format12.reserved				= 0;
				vtCmap.push_back(0);
				vtCmap.push_back(0);

				// 16 == format + reserved + length + language + nGroups
				// 12 == startCharCode + endCharCode + startGlyphID
				entry.Format12.length = 16+entry.Format12.vtGroup.size()*12;
				vtCmap.push_back((entry.Format12.length&0xff000000)>>24);
				vtCmap.push_back((entry.Format12.length&0x00ff0000)>>16);
				vtCmap.push_back((entry.Format12.length&0x0000ff00)>>8);
				vtCmap.push_back(entry.Format12.length&0x000000ff);

				entry.Format12.language	= 0;
				vtCmap.push_back(0);
				vtCmap.push_back(0);
				vtCmap.push_back(0);
				vtCmap.push_back(0);

				entry.Format12.nGroups = (unsigned long)entry.Format12.vtGroup.size();
				vtCmap.push_back((entry.Format12.nGroups&0xff000000)>>24);
				vtCmap.push_back((entry.Format12.nGroups&0x00ff0000)>>16);
				vtCmap.push_back((entry.Format12.nGroups&0x0000ff00)>>8);
				vtCmap.push_back(entry.Format12.nGroups&0x000000ff);

				unsigned long ulTmp = 0;				
				for (size_t i=0; i<entry.Format12.vtGroup.size(); i++)	{
					std::vector<CMAP_ENCODE_FORMAT_12_GROUP>& vtGroups = entry.Format12.vtGroup;

					ulTmp = vtGroups[i].startCharCode;
					vtCmap.push_back((ulTmp&0xff000000)>>24);
					vtCmap.push_back((ulTmp&0x00ff0000)>>16);
					vtCmap.push_back((ulTmp&0x0000ff00)>>8);
					vtCmap.push_back(ulTmp&0x000000ff);				

					ulTmp = vtGroups[i].endCharCode;
					vtCmap.push_back((ulTmp&0xff000000)>>24);
					vtCmap.push_back((ulTmp&0x00ff0000)>>16);
					vtCmap.push_back((ulTmp&0x0000ff00)>>8);
					vtCmap.push_back(ulTmp&0x000000ff);

					ulTmp = vtGroups[i].startGlyphID;
					vtCmap.push_back((ulTmp&0xff000000)>>24);
					vtCmap.push_back((ulTmp&0x00ff0000)>>16);
					vtCmap.push_back((ulTmp&0x0000ff00)>>8);
					vtCmap.push_back(ulTmp&0x000000ff);

// 					CString strTrace;
// 					strTrace.Format("STartCode = 0x%x  0x%x  %d\n", vtGroups[i].startCharCode, vtGroups[i].endCharCode, vtGroups[i].startGlyphID);
// 					::OutputDebugString(strTrace);
				}
			}

			entry.offset = ftell(pFontFile)-entry.offset;
			for(size_t i=0; i<vtCmap.size(); i++){
				fwrite(&vtCmap[i],1,1,pFontFile);						
			}			
		}

		return HY_NOERROR;

	}	// end of int	CHYFontCodec::EncodeCmapFmt12()

	int	CHYFontCodec::EncodeCmapFmt13(FILE* pFontFile,CMAP_TABLE_ENTRY& entry)
	{
		std::vector<unsigned char> vtCmap;
	
		return HY_NOERROR;

	}	// end of int	CHYFontCodec::EncodeCmapFmt13()

	int	CHYFontCodec::EncodeCmapFmt14(FILE* pFontFile,CMAP_TABLE_ENTRY& entry)
	{
		entry.offset = ftell(pFontFile) - entry.offset;
		unsigned long ulBegin =  ftell(pFontFile);

		CMAP_ENCODE_FORMAT_14& format14 = entry.Format14;
		format14.format = CMAP_ENCODE_FT_14;
		unsigned short usTmp = hy_cdr_int16_to(format14.format);
		fwrite(&usTmp, 2, 1, m_pFontFile);
		unsigned long ulTmp = hy_cdr_int32_to(format14.length);	
		fwrite(&ulTmp, 4, 1, m_pFontFile);
		ulTmp = hy_cdr_int32_to(format14.numVarSelectorRecords);
		fwrite(&ulTmp, 4, 1, m_pFontFile);
		for (int i = 0; i < format14.numVarSelectorRecords; i++) {
			CVariationSelector& vrSltr = format14.vtVariationSelector[i];
			fwrite(&vrSltr.varSelector[0], 1, 1, m_pFontFile);
			fwrite(&vrSltr.varSelector[1], 1, 1, m_pFontFile);
			fwrite(&vrSltr.varSelector[2], 1, 1, m_pFontFile);
			ulTmp = 0;
			vrSltr.defaultUVSOffset = ftell(m_pFontFile);
			fwrite(&ulTmp, 4, 1, m_pFontFile);
			vrSltr.nonDefaultUVSOffset = ftell(m_pFontFile);
			fwrite(&ulTmp, 4, 1, m_pFontFile);
		}
		for (int i = 0; i < format14.numVarSelectorRecords; i++) {
			CVariationSelector& vrSltr = format14.vtVariationSelector[i];
			//DefaultUVS
			if (vrSltr.DefaultUVS.numUnicodeValueRanges > 0){
				unsigned long ultmp1 = ftell(m_pFontFile);
				fseek(m_pFontFile, vrSltr.defaultUVSOffset, SEEK_SET);
				ulTmp = hy_cdr_int32_to(ultmp1-ulBegin);
				fwrite(&ulTmp, 4, 1, m_pFontFile);
				fseek(m_pFontFile, ultmp1, SEEK_SET);

				//UnicodeRange
				ulTmp = hy_cdr_int32_to(vrSltr.DefaultUVS.numUnicodeValueRanges);
				fwrite(&ulTmp, 4, 1, m_pFontFile);
				for (int uniVR = 0; uniVR < vrSltr.DefaultUVS.numUnicodeValueRanges; uniVR++) {
					CUnicodeRange&	range = vrSltr.DefaultUVS.vtranges[uniVR];
					fwrite(&range.startUnicodeValue[0], 1, 1, m_pFontFile);
					fwrite(&range.startUnicodeValue[1], 1, 1, m_pFontFile);
					fwrite(&range.startUnicodeValue[2], 1, 1, m_pFontFile);
					fwrite(&range.additionalCount, 1, 1, m_pFontFile);
				}
			}

			//Non-Default UVS
			if (vrSltr.NonDefaultUVS.numUVSMappings > 0) {
				unsigned long ultmp1 = ftell(m_pFontFile);
				fseek(m_pFontFile, vrSltr.nonDefaultUVSOffset, SEEK_SET);
				ulTmp = hy_cdr_int32_to(ultmp1-ulBegin);
				fwrite(&ulTmp, 4, 1, m_pFontFile);
				fseek(m_pFontFile, ultmp1, SEEK_SET);

				//numUVSMappings
				ulTmp = hy_cdr_int32_to(vrSltr.NonDefaultUVS.numUVSMappings);
				fwrite(&ulTmp, 4, 1, m_pFontFile);
				for (int mp = 0; mp < vrSltr.NonDefaultUVS.numUVSMappings; mp++) {
					CUVSMapping& Mapping = vrSltr.NonDefaultUVS.vtuvsMappings[mp];
					fwrite(&Mapping.unicodeValue[0], 1, 1, m_pFontFile);
					fwrite(&Mapping.unicodeValue[1], 1, 1, m_pFontFile);
					fwrite(&Mapping.unicodeValue[2], 1, 1, m_pFontFile);

					usTmp = hy_cdr_int16_to(Mapping.glyphID);
					fwrite(&usTmp, 2, 1, m_pFontFile);
				}
			}
		}

		return HY_NOERROR;

	}	// end of int CHYFontCodec::EncodeCmapFmt14()
#if 0	//emjio 改造临时屏蔽
	//EBLC
	int	CHYFontCodec::DecodeEBLC()
	{
		m_HYCblc.SetDefault();
		if (m_pFontFile) {
			int iEntryIndex = m_HYTbDirectory.FindTableEntry(EBLC_TAG);
			if (iEntryIndex == -1) return FONT_ERR_EBLC_DECODE;

			CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
			fseek(m_pFontFile, tbEntry.offset, SEEK_SET);
			DecodeBmpLC(tbEntry.offset);
		}

		return HY_NOERROR;
	
	}	// end of int CHYFontCodec::DecodeEBLC()

	// CBLC Table
	int	CHYFontCodec::DecodeCBLC()
	{
		m_HYCblc.SetDefault();
		if(m_pFontFile)	{
			int iEntryIndex = m_HYTbDirectory.FindTableEntry(CBLC_TAG);
			if (iEntryIndex == -1) return FONT_ERR_CBLC_DECODE;

			CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
			fseek(m_pFontFile,tbEntry.offset,SEEK_SET);
			DecodeBmpLC(tbEntry.offset);			
		}

		return HY_NOERROR;

	}	// end of int CHYFontCodec::DecodeCBLC()

	void	CHYFontCodec::DecodeBmpLC(unsigned long ulBLCPos)
	{
		// version 
		fread(&m_HYCblc.Header.version.value, 2, 1, m_pFontFile);
		m_HYCblc.Header.version.value = hy_cdr_int16_to(m_HYCblc.Header.version.value);
		fread(&m_HYCblc.Header.version.fract, 2, 1, m_pFontFile);
		m_HYCblc.Header.version.fract = hy_cdr_int16_to(m_HYCblc.Header.version.fract);
		// numsizes
		fread(&m_HYCblc.Header.numsizes, 4, 1, m_pFontFile);
		m_HYCblc.Header.numsizes = hy_cdr_int32_to(m_HYCblc.Header.numsizes);
		// BitmapSize
		for (unsigned long i = 0; i < m_HYCblc.Header.numsizes; i++) {
			BitmapSize bitmapSizeTable;
			//indexSubTableArrayOffset
			fread(&bitmapSizeTable.indexSubTableArrayOffset, 4, 1, m_pFontFile);
			bitmapSizeTable.indexSubTableArrayOffset = hy_cdr_int32_to(bitmapSizeTable.indexSubTableArrayOffset);
			//indexTablesSize
			fread(&bitmapSizeTable.indexTablesSize, 4, 1, m_pFontFile);
			bitmapSizeTable.indexTablesSize = hy_cdr_int32_to(bitmapSizeTable.indexTablesSize);
			//numberofIndexSubTables
			fread(&bitmapSizeTable.numberofIndexSubTables, 4, 1, m_pFontFile);
			bitmapSizeTable.numberofIndexSubTables = hy_cdr_int32_to(bitmapSizeTable.numberofIndexSubTables);
			//colorRef
			fread(&bitmapSizeTable.colorRef, 4, 1, m_pFontFile);
			bitmapSizeTable.colorRef = hy_cdr_int32_to(bitmapSizeTable.colorRef);
			//sbitLineMetrics
			DecodeSbitLineMetrics(bitmapSizeTable.Hori);
			//sbitLineMetrics
			DecodeSbitLineMetrics(bitmapSizeTable.Vert);
			//startGlyphIndex
			fread(&bitmapSizeTable.startGlyphIndex, 2, 1, m_pFontFile);
			bitmapSizeTable.startGlyphIndex = hy_cdr_int16_to(bitmapSizeTable.startGlyphIndex);
			//endGlyphIndex
			fread(&bitmapSizeTable.endGlyphIndex, 2, 1, m_pFontFile);
			bitmapSizeTable.endGlyphIndex = hy_cdr_int16_to(bitmapSizeTable.endGlyphIndex);
			//ppemX
			fread(&bitmapSizeTable.ppemX, 1, 1, m_pFontFile);
			//ppemY
			fread(&bitmapSizeTable.ppemY, 1, 1, m_pFontFile);
			//bitDepth
			fread(&bitmapSizeTable.bitDepth, 1, 1, m_pFontFile);
			//Flags
			fread(&bitmapSizeTable.Flags, 1, 1, m_pFontFile);

			m_HYCblc.vtBitmapSizeTb.push_back(bitmapSizeTable);
		}

		for (unsigned long i = 0; i < m_HYCblc.Header.numsizes; i++){
			BitmapSize& bmpszTb = m_HYCblc.vtBitmapSizeTb[i];
			//解码 indexSubTableArray
			long lCurrentPos = ftell(m_pFontFile);
			fseek(m_pFontFile, ulBLCPos + bmpszTb.indexSubTableArrayOffset, SEEK_SET);

			DecodeIndexSubTableArray(bmpszTb, ulBLCPos + bmpszTb.indexSubTableArrayOffset);			

			fseek(m_pFontFile, lCurrentPos, SEEK_SET);
		}

	}	// end of void	CHYFontCodec::DecodeBmpLC()

	void	CHYFontCodec::DumpCBLC()
	{
	}	// end of void	CHYFontCodec::DumpCBLC()

	void	CHYFontCodec::DecodeSbitLineMetrics(SbitLineMetrics& sbit)
	{
		fread(&sbit.ascender, 1, 1, m_pFontFile);
		fread(&sbit.descender, 1, 1, m_pFontFile);
		fread(&sbit.widthMax, 1, 1, m_pFontFile);
		fread(&sbit.caretSlopeNumerator, 1, 1, m_pFontFile);
		fread(&sbit.caretSlopeDenominator, 1, 1, m_pFontFile);
		fread(&sbit.caretOffset, 1, 1, m_pFontFile);
		fread(&sbit.minOriginSB, 1, 1, m_pFontFile);
		fread(&sbit.minAdvanceSB, 1, 1, m_pFontFile);
		fread(&sbit.maxBeforeBL, 1, 1, m_pFontFile);
		fread(&sbit.minAfterBL, 1, 1, m_pFontFile);
		fread(&sbit.pad1, 1, 1, m_pFontFile);
		fread(&sbit.pad2, 1, 1, m_pFontFile);
	
	}	// end of void	CHYFontCodec::DecodeSbitLineMetrics()

	void	CHYFontCodec::DecodeBigGlyphMetrics(BigGlyphMetrics& bigMtrcs)
	{
		fread(&bigMtrcs.height, 1, 1, m_pFontFile);
		fread(&bigMtrcs.width, 1, 1, m_pFontFile);
		fread(&bigMtrcs.horiBearingX, 1, 1, m_pFontFile);
		fread(&bigMtrcs.horiBearingY, 1, 1, m_pFontFile);
		fread(&bigMtrcs.horiAdvance, 1, 1, m_pFontFile);
		fread(&bigMtrcs.vertBearingX, 1, 1, m_pFontFile);
		fread(&bigMtrcs.vertBearingY, 1, 1, m_pFontFile);
		fread(&bigMtrcs.vertAdvance, 1, 1, m_pFontFile);

	}	// end of void	CHYFontCodec::DecodeBigGlyphMetrics()

	int CHYFontCodec::DecodeIndexSubTableArray(BitmapSize& bitmapSizeTable,long  indexSubTableArrayOffset)
	{
		for (int i = 0; i < bitmapSizeTable.numberofIndexSubTables; i++)
		{
			IndexSubTableArray arryIndxSubTable;

			//startGlyphIndex
			fread(&arryIndxSubTable.firstGlyphIndex, 2, 1, m_pFontFile);
			arryIndxSubTable.firstGlyphIndex = hy_cdr_int16_to(arryIndxSubTable.firstGlyphIndex);
			//endGlyphIndex
			fread(&arryIndxSubTable.lastGlyphIndex, 2, 1, m_pFontFile);
			arryIndxSubTable.lastGlyphIndex = hy_cdr_int16_to(arryIndxSubTable.lastGlyphIndex);
			//additionalOffsetToIndexSubtable
			fread(&arryIndxSubTable.Offset, 4, 1, m_pFontFile);
			arryIndxSubTable.Offset = hy_cdr_int32_to(arryIndxSubTable.Offset);

			long currentPs = ftell(m_pFontFile);

			fseek(m_pFontFile, indexSubTableArrayOffset + arryIndxSubTable.Offset, SEEK_SET);
			IndexSUBTable IndxSubTb;
			DecodeIndexSubTable(IndxSubTb);
			arryIndxSubTable.vtIndxSubTable.push_back(IndxSubTb);

			fseek(m_pFontFile, currentPs, SEEK_SET);

			bitmapSizeTable.vtIndexSubTableArray.push_back(arryIndxSubTable);
		}

		return HY_NOERROR;

	}	// end of int CHYFontCodec::DecodeIndexSubTableArray()

	int CHYFontCodec::DecodeIndexSubTable(IndexSUBTable& IndxSubTb)
	{	
		//IndexSubHeader
		//indexFormat
		fread(&IndxSubTb.Header.indexFormat,2,1,m_pFontFile);
		IndxSubTb.Header.indexFormat = hy_cdr_int16_to(IndxSubTb.Header.indexFormat);
		//imageFormat
		fread(&IndxSubTb.Header.imageFormat,2,1,m_pFontFile);
		IndxSubTb.Header.imageFormat = hy_cdr_int16_to(IndxSubTb.Header.imageFormat);
		//imageDataOffset
		fread(&IndxSubTb.Header.imageDataOffset,4,1,m_pFontFile);
		IndxSubTb.Header.imageDataOffset = hy_cdr_int32_to(IndxSubTb.Header.imageDataOffset);

		switch(IndxSubTb.Header.indexFormat)	{			
			case 1:
				DecdoeIndexSubTable1(IndxSubTb);
				break;			
			case 2:				
				DecdoeIndexSubTable2(IndxSubTb);
				break;			
			case 3:
				DecdoeIndexSubTable3(IndxSubTb);
				break;			
			case 4:				
				DecdoeIndexSubTable4(IndxSubTb);
				break;			
			case 5:
				DecdoeIndexSubTable5(IndxSubTb);
				break;			
		}

		return HY_NOERROR;

	}	// end of int CHYFontCodec::DecodeIndexSubTable()

	void	CHYFontCodec::DecdoeIndexSubTable1(IndexSUBTable& IndxSubTb)
	{
		IndexSubTable1& SubTable1 = IndxSubTb.SubTable1;

		unsigned long ulOffset = 0;
		fread(&ulOffset, 4, 1, m_pFontFile);
		ulOffset = hy_cdr_int32_to(ulOffset);
		SubTable1.vtsbitOffsets.push_back(ulOffset);

	}	// end of int	CHYFontCodec::DecdoeIndexSubTable1()

	void	CHYFontCodec::DecdoeIndexSubTable2(IndexSUBTable& IndxSubTb)
	{
		IndexSubTable2& SubTable2 = IndxSubTb.SubTable2;

		unsigned long ulImageSize = 0;
		fread(&ulImageSize, 4, 1, m_pFontFile);
		SubTable2.ulImageSize = hy_cdr_int32_to(ulImageSize);
		DecodeBigGlyphMetrics(SubTable2.bigMetrics);		

	}	// end of int	CHYFontCodec::DecdoeIndexSubTable2()

	void	CHYFontCodec::DecdoeIndexSubTable3(IndexSUBTable& IndxSubTb)
	{
		IndexSubTable3& SubTable3 = IndxSubTb.SubTable3;

		unsigned short usOffset = 0;
		fread(&usOffset, 2, 1, m_pFontFile);
		usOffset = hy_cdr_int16_to(usOffset);
		SubTable3.vtOffsetArray.push_back(usOffset);

	}	// end of void	CHYFontCodec::DecdoeIndexSubTable3()

	void	CHYFontCodec::DecdoeIndexSubTable4(IndexSUBTable& IndxSubTb)
	{
		IndexSubTable4& SubTable4 = IndxSubTb.SubTable4;

		unsigned long numGlyphs = 0;
		fread(&numGlyphs, 4, 1, m_pFontFile);
		SubTable4.numGlyphs = hy_cdr_int32_to(numGlyphs);

		unsigned short usTmp;
		for (int i = 0; i < numGlyphs; i++){
			GlyphIdOffsetPair 	OffsetSetPair;
			fread(&usTmp, 2, 1, m_pFontFile);
			OffsetSetPair.glyphID = hy_cdr_int16_to(usTmp);
			fread(&usTmp, 2, 1, m_pFontFile);
			OffsetSetPair.sbitOffset = hy_cdr_int16_to(usTmp);

			SubTable4.vtGlyphArray.push_back(OffsetSetPair);
		}

	}	// end of void	CHYFontCodec::DecdoeIndexSubTable4()

	void	CHYFontCodec::DecdoeIndexSubTable5(IndexSUBTable& IndxSubTb)
	{
		unsigned long ulTmp = 0;
		IndexSubTable5& SubTable5 = IndxSubTb.SubTable5;
		fread(&ulTmp, 4, 1, m_pFontFile);
		SubTable5.imageSize = hy_cdr_int32_to(ulTmp);

		DecodeBigGlyphMetrics(SubTable5.bigMetrics);
		
		fread(&ulTmp, 4, 1, m_pFontFile);
		SubTable5.numGlyphs = hy_cdr_int32_to(ulTmp);

		unsigned short usGID = 0;
		for (int i = 0; i < SubTable5.numGlyphs; i++){			
			fread(&usGID, 2, 1, m_pFontFile);
			usGID = hy_cdr_int16_to(usGID);
			SubTable5.vtGlyphIDArray.push_back(usGID);
		}
	
	}	// end of void	CHYFontCodec::DecdoeIndexSubTable5()

	int	CHYFontCodec::EncodeCBLC()
	{
		int iEntryIndex = m_HYTbDirectory.FindTableEntry(CBLC_TAG);
		if (iEntryIndex == -1) return FONT_ERR_CBLC_ENCODE;

		CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
		tbEntry.offset = ftell(m_pFontFile);

		//Table version number
		m_HYCblc.Header.version.value = 3;
		m_HYCblc.Header.version.fract = 0;

		EncodeBLCInfo(m_HYCblc, tbEntry.offset);
		tbEntry.length = ftell(m_pFontFile) - tbEntry.offset;
		::HY_4ByteAlign(m_pFontFile, tbEntry.length);

		return HY_NOERROR;

	}	// end of int	CHYFontCodec::EncodeCBLC()	

	int	CHYFontCodec::EncodeEBLC()
	{
		int iEntryIndex = m_HYTbDirectory.FindTableEntry(EBLC_TAG);
		if (iEntryIndex == -1) return FONT_ERR_EBLC_ENCODE;

		CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
		tbEntry.offset = ftell(m_pFontFile);

		//Table version number
		m_HYCblc.Header.version.value = 2;
		m_HYCblc.Header.version.fract = 0;

		EncodeBLCInfo(m_HYCblc, tbEntry.offset);

		tbEntry.length = ftell(m_pFontFile) - tbEntry.offset;
		::HY_4ByteAlign(m_pFontFile, tbEntry.length);

		return HY_NOERROR;

	}	// end of int	CHYFontCodec::EncodeEBLC()

	void	CHYFontCodec::EncodeBLCInfo(CHYCBLC& HYCblc,unsigned long ulBLCPos)
	{
		unsigned short	usTmp = 0;
		unsigned long	ulTmp = 0;
		unsigned long	TableEnd = 0;
		
		usTmp = hy_cdr_int16_to(m_HYCblc.Header.version.value);
		fwrite(&usTmp, 2, 1, m_pFontFile);
		usTmp = hy_cdr_int16_to(m_HYCblc.Header.version.fract);
		fwrite(&usTmp, 2, 1, m_pFontFile);
		ulTmp = hy_cdr_int32_to(m_HYCblc.Header.numsizes);
		fwrite(&ulTmp, 4, 1, m_pFontFile);
		
		for (unsigned long i = 0; i < m_HYCblc.Header.numsizes; i++) {
			BitmapSize& bitmapSizeTable = m_HYCblc.vtBitmapSizeTb[i];
			bitmapSizeTable.numberofIndexSubTables = (unsigned long)m_HYCblc.vtIndexSubTableArray.size();
			
			ulTmp = 0;			
			fwrite(&ulTmp, 4, 1, m_pFontFile);
			// size部分也先预留空位置。
			fwrite(&ulTmp, 4, 1, m_pFontFile);
			//numberofIndexSubTables
			ulTmp = hy_cdr_int32_to(bitmapSizeTable.numberofIndexSubTables);
			ulTmp = fwrite(&ulTmp, 4, 1, m_pFontFile);
			//colorRef
			ulTmp = hy_cdr_int32_to(bitmapSizeTable.colorRef);
			ulTmp = fwrite(&ulTmp, 4, 1, m_pFontFile);
			//hori
			fwrite(&bitmapSizeTable.Hori.ascender, 1, 1, m_pFontFile);
			fwrite(&bitmapSizeTable.Hori.descender, 1, 1, m_pFontFile);
			fwrite(&bitmapSizeTable.Hori.widthMax, 1, 1, m_pFontFile);
			fwrite(&bitmapSizeTable.Hori.caretSlopeNumerator, 1, 1, m_pFontFile);
			fwrite(&bitmapSizeTable.Hori.caretSlopeDenominator, 1, 1, m_pFontFile);
			fwrite(&bitmapSizeTable.Hori.caretOffset, 1, 1, m_pFontFile);
			fwrite(&bitmapSizeTable.Hori.minOriginSB, 1, 1, m_pFontFile);
			fwrite(&bitmapSizeTable.Hori.minAdvanceSB, 1, 1, m_pFontFile);
			fwrite(&bitmapSizeTable.Hori.maxBeforeBL, 1, 1, m_pFontFile);
			fwrite(&bitmapSizeTable.Hori.minAfterBL, 1, 1, m_pFontFile);
			fwrite(&bitmapSizeTable.Hori.pad1, 1, 1, m_pFontFile);
			fwrite(&bitmapSizeTable.Hori.pad2, 1, 1, m_pFontFile);
			//vert
			fwrite(&bitmapSizeTable.Vert.ascender, 1, 1, m_pFontFile);
			fwrite(&bitmapSizeTable.Vert.descender, 1, 1, m_pFontFile);
			fwrite(&bitmapSizeTable.Vert.widthMax, 1, 1, m_pFontFile);
			fwrite(&bitmapSizeTable.Vert.caretSlopeNumerator, 1, 1, m_pFontFile);
			fwrite(&bitmapSizeTable.Vert.caretSlopeDenominator, 1, 1, m_pFontFile);
			fwrite(&bitmapSizeTable.Vert.caretOffset, 1, 1, m_pFontFile);
			fwrite(&bitmapSizeTable.Vert.minOriginSB, 1, 1, m_pFontFile);
			fwrite(&bitmapSizeTable.Vert.minAdvanceSB, 1, 1, m_pFontFile);
			fwrite(&bitmapSizeTable.Vert.maxBeforeBL, 1, 1, m_pFontFile);
			fwrite(&bitmapSizeTable.Vert.minAfterBL, 1, 1, m_pFontFile);
			fwrite(&bitmapSizeTable.Vert.pad1, 1, 1, m_pFontFile);
			fwrite(&bitmapSizeTable.Vert.pad2, 1, 1, m_pFontFile);
			//startGlyphIndex
			usTmp = hy_cdr_int16_to(bitmapSizeTable.startGlyphIndex);
			fwrite(&usTmp, 2, 1, m_pFontFile);
			//endGlyphIndex
			usTmp = hy_cdr_int16_to(bitmapSizeTable.endGlyphIndex);
			fwrite(&usTmp, 2, 1, m_pFontFile);
			//ppemX	
			fwrite(&bitmapSizeTable.ppemX, 1, 1, m_pFontFile);
			//ppemY
			fwrite(&bitmapSizeTable.ppemY, 1, 1, m_pFontFile);
			//bitDepth
			fwrite(&bitmapSizeTable.bitDepth, 1, 1, m_pFontFile);
			//flags
			fwrite(&bitmapSizeTable.Flags, 1, 1, m_pFontFile);				
		}

		for (unsigned long x = 0; x < m_HYCblc.Header.numsizes; x++) {
			BitmapSize& bitmapSizeTable = m_HYCblc.vtBitmapSizeTb[x];

			unsigned long  posIndexSubTableArrayOffset = ftell(m_pFontFile);
			unsigned long indexSubTableArrayOffset = ftell(m_pFontFile) - ulBLCPos;
			unsigned long indexSubTableArraySize = 0;

			unsigned long AdditionalOffset = 0;
			for (unsigned long y = 0; y < bitmapSizeTable.numberofIndexSubTables; y++) {
				IndexSubTableArray& subtableArray = m_HYCblc.vtIndexSubTableArray[y];
				subtableArray.Offset = AdditionalOffset;
				EncodeIndexSubTableArray(subtableArray, AdditionalOffset);
			}

			unsigned long ultmpPos = ftell(m_pFontFile);
			fseek(m_pFontFile, posIndexSubTableArrayOffset, SEEK_SET);

			// 确定真实的offset 和size的值
			//Offset32 	indexSubTableArrayOffset
			ulTmp = indexSubTableArrayOffset;
			fwrite(&ulTmp, 4, 1, m_pFontFile);
			ulTmp = AdditionalOffset;
			fwrite(&ulTmp, 4, 1, m_pFontFile);

			ultmpPos = ftell(m_pFontFile);
			fseek(m_pFontFile, ultmpPos, SEEK_SET);

		}

	}	// end of int CHYFontCodec::EncodeBLCInfo()

	void CHYFontCodec::EncodeIndexSubTableArray(IndexSubTableArray& subtableArray, int additionalOffset)
	{
		unsigned short usTmp = 0;
		unsigned long ulTmp = 0;

		//IndexSubTableArray
		//firstGlyphIndex
		usTmp = hy_cdr_int16_to(subtableArray.firstGlyphIndex);
		fwrite(&usTmp, 2, 1, m_pFontFile);
		additionalOffset += 2;
		//lastGlyphIndex
		usTmp = hy_cdr_int16_to(subtableArray.lastGlyphIndex);
		fwrite(&usTmp, 2, 1, m_pFontFile);
		additionalOffset += 2;
		//additionalOffset ToIndexSubtable		
		fwrite(&ulTmp, 4, 1, m_pFontFile);
		additionalOffset += 4;
		subtableArray.Offset = additionalOffset;

		//IndexSubHeader
		//indexFormat
		usTmp = hy_cdr_int16_to(subtableArray.IndexSubTb.Header.indexFormat);
		fwrite(&usTmp, 2, 1, m_pFontFile);
		additionalOffset += 2;
		//imageFormat
		usTmp = hy_cdr_int16_to(subtableArray.IndexSubTb.Header.imageFormat);
		fwrite(&usTmp, 2, 1, m_pFontFile);
		additionalOffset += 2;
		//imageDataOffset
		ulTmp = hy_cdr_int32_to(subtableArray.IndexSubTb.Header.imageDataOffset);
		fwrite(&ulTmp, 4, 1, m_pFontFile);
		additionalOffset += 4;

		switch (subtableArray.IndexSubTb.Header.indexFormat)
		{
			case 1:
				EncodeIndexSubTable1(subtableArray, additionalOffset);
				break;
			case 2:
				EncodeIndexSubTable2(subtableArray, additionalOffset);
				break;
			case 3:
				EncodeIndexSubTable3(subtableArray, additionalOffset);
				break;
			case 4:
				EncodeIndexSubTable4(subtableArray, additionalOffset);
				break;
			case 5:
				EncodeIndexSubTable5(subtableArray, additionalOffset);
				break;			
		default:
			break;
		}

		return;

	}	// end of int CHYFontCodec::EncodeIndexSubTableArray()

	void CHYFontCodec::EncodeIndexSubTable1(IndexSubTableArray& IndexArray, unsigned long additionalOffset)
	{
		// +1, it is necessary to store one extra element in the sbitOffsets array pointing just past the end of the range’s image data. 
		int loop = IndexArray.lastGlyphIndex - IndexArray.firstGlyphIndex + 1 + 1;
		unsigned long ulTmp;
		for (int i = 0; i < loop; i++){			
			ulTmp = hy_cdr_int32_to(IndexArray.IndexSubTb.SubTable1.vtsbitOffsets[i]);
			fwrite(&ulTmp, 4, 1, m_pFontFile);
			additionalOffset += 4;
		}
	
	}	// end of int CHYFontCodec::EncodeIndexSubTable1()

	void CHYFontCodec::EncodeIndexSubTable2(IndexSubTableArray& IndexArray, unsigned long additionalOffset)
	{
		unsigned long ulTmp;
		IndexSubTable2& SubTable2 = IndexArray.IndexSubTb.SubTable2;
		ulTmp = hy_cdr_int32_to(SubTable2.ulImageSize);
		fwrite(&ulTmp, 4, 1, m_pFontFile);
		additionalOffset += 4;

		fwrite(&SubTable2.bigMetrics.height, 1, 1, m_pFontFile);
		fwrite(&SubTable2.bigMetrics.width, 1, 1, m_pFontFile);
		fwrite(&SubTable2.bigMetrics.horiBearingX, 1, 1, m_pFontFile);
		fwrite(&SubTable2.bigMetrics.horiBearingY, 1, 1, m_pFontFile);
		fwrite(&SubTable2.bigMetrics.horiAdvance, 1, 1, m_pFontFile);
		fwrite(&SubTable2.bigMetrics.vertBearingX, 1, 1, m_pFontFile);
		fwrite(&SubTable2.bigMetrics.vertBearingY, 1, 1, m_pFontFile);
		fwrite(&SubTable2.bigMetrics.vertAdvance, 1, 1, m_pFontFile);
		additionalOffset += 8;
	
	}	// end of int CHYFontCodec::EncodeIndexSubTable2()

	void CHYFontCodec::EncodeIndexSubTable3(IndexSubTableArray& IndexArray, unsigned long additionalOffset)
	{
		// +1, it is necessary to store one extra element in the sbitOffsets array pointing just past the end of the range’s image data. 
		int loop = IndexArray.lastGlyphIndex - IndexArray.firstGlyphIndex + 1 + 1;
		unsigned short usTmp;
		for (int i = 0; i < loop; i++) {
			usTmp = hy_cdr_int16_to(IndexArray.IndexSubTb.SubTable3.vtOffsetArray[i]);
			fwrite(&usTmp, 2, 1, m_pFontFile);
			additionalOffset += 2;
		}

	}	// end of int CHYFontCodec::EncodeIndexSubTable3()

	void CHYFontCodec::EncodeIndexSubTable4(IndexSubTableArray& IndexArray, unsigned long additionalOffset)
	{	
		unsigned long ulTmp=0;
		unsigned short usTmp = 0;

		IndexSubTable4& SubTable4 = IndexArray.IndexSubTb.SubTable4;
		ulTmp = hy_cdr_int32_to(SubTable4.numGlyphs);
		fwrite(&ulTmp, 4, 1, m_pFontFile);
		additionalOffset += 4;

		for (int i = 0; i <= SubTable4.numGlyphs; i++){
			GlyphIdOffsetPair& Pair = SubTable4.vtGlyphArray[i];
			usTmp = hy_cdr_int16_to(Pair.glyphID);
			fwrite(&usTmp, 2, 1, m_pFontFile);
			usTmp = hy_cdr_int16_to(Pair.sbitOffset);
			fwrite(&usTmp, 2, 1, m_pFontFile);
			additionalOffset += 4;
		}

	}	// end of int CHYFontCodec::EncodeIndexSubTable4()

	void CHYFontCodec::EncodeIndexSubTable5(IndexSubTableArray& IndexArray, unsigned long additionalOffset)
	{
		unsigned long ulTmp;
		IndexSubTable5& SubTable5 = IndexArray.IndexSubTb.SubTable5;
		ulTmp = hy_cdr_int32_to(SubTable5.imageSize);
		fwrite(&ulTmp, 4, 1, m_pFontFile);
		additionalOffset += 4;

		fwrite(&SubTable5.bigMetrics.height, 1, 1, m_pFontFile);
		fwrite(&SubTable5.bigMetrics.width, 1, 1, m_pFontFile);
		fwrite(&SubTable5.bigMetrics.horiBearingX, 1, 1, m_pFontFile);
		fwrite(&SubTable5.bigMetrics.horiBearingY, 1, 1, m_pFontFile);
		fwrite(&SubTable5.bigMetrics.horiAdvance, 1, 1, m_pFontFile);
		fwrite(&SubTable5.bigMetrics.vertBearingX, 1, 1, m_pFontFile);
		fwrite(&SubTable5.bigMetrics.vertBearingY, 1, 1, m_pFontFile);
		fwrite(&SubTable5.bigMetrics.vertAdvance, 1, 1, m_pFontFile);
		additionalOffset += 8;

		ulTmp = hy_cdr_int32_to(SubTable5.numGlyphs);
		fwrite(&ulTmp, 4, 1, m_pFontFile);
		additionalOffset += 4;

		unsigned short usTmp = 0;
		for (int i = 0; i < ulTmp; i++) {
			usTmp = hy_cdr_int16_to(SubTable5.vtGlyphIDArray[i]);
			fwrite(&usTmp, 2, 1, m_pFontFile);
			additionalOffset += 2;
		}

	}	// end of int CHYFontCodec::EncodeIndexSubTable5()

	// EBDT Table
	int	CHYFontCodec::DecodeEBDT()
	{
		if (m_pFontFile) {
			m_HYCBDT.SetDefault();

			int iEntryIndex = m_HYTbDirectory.FindTableEntry(EBDT_TAG);
			if (iEntryIndex == -1) return FONT_ERR_EBDT_DECODE;

			CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
			fseek(m_pFontFile, tbEntry.offset, SEEK_SET);

			// version 
			fread(&m_HYCBDT.majorVersion, 2, 1, m_pFontFile);
			m_HYCBDT.majorVersion = hy_cdr_int16_to(m_HYCBDT.majorVersion);
			fread(&m_HYCBDT.minorVersion, 2, 1, m_pFontFile);
			m_HYCBDT.minorVersion = hy_cdr_int16_to(m_HYCBDT.minorVersion);

			size_t szBitmapSize = m_HYCblc.vtBitmapSizeTb.size();
			for (size_t i = 0; i < szBitmapSize; i++) {
				BitmapSize& bitmapSizeTable = m_HYCblc.vtBitmapSizeTb[i];
				IndexSubTableArray& IndexTableArray = m_HYCblc.vtIndexSubTableArray[i];
				DecodeEBDTData(IndexTableArray, tbEntry.offset);
			}
		}

		return HY_NOERROR;
	
	}	// end of int CHYFontCodec::DecodeEBDT()

	// CBDT Table
	int CHYFontCodec::DecodeCBDT()
	{
		if(m_pFontFile)	{
			m_HYCBDT.SetDefault();

			int iEntryIndex = m_HYTbDirectory.FindTableEntry(CBDT_TAG);
			if (iEntryIndex == -1) return FONT_ERR_CBDT_DECODE;

			CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
			fseek(m_pFontFile,tbEntry.offset,SEEK_SET);

			// version 
			fread(&m_HYCBDT.majorVersion,2,1,m_pFontFile);
			m_HYCBDT.majorVersion = hy_cdr_int16_to(m_HYCBDT.majorVersion);
			fread(&m_HYCBDT.minorVersion,2,1,m_pFontFile);
			m_HYCBDT.minorVersion = hy_cdr_int16_to(m_HYCBDT.minorVersion);

			size_t szBitmapSize = m_HYCblc.vtBitmapSizeTb.size();
			for (size_t i = 0; i < szBitmapSize; i++) {
				BitmapSize& bitmapSizeTable = m_HYCblc.vtBitmapSizeTb[i];
				IndexSubTableArray& IndexTableArray = m_HYCblc.vtIndexSubTableArray[i];
				DecodeEBDTData(IndexTableArray, tbEntry.offset);
			}
		}	

		return HY_NOERROR;

	}	// end of int CHYFontCodec::DecodeCBDT()	

	void CHYFontCodec::DecodeEBDTData(IndexSubTableArray& arrySubtable, unsigned long ulEBDTPos)
	{		
		int iLoop = arrySubtable.lastGlyphIndex - arrySubtable.firstGlyphIndex + 1;
		for (int i = 0; i < iLoop; i++) {
			IndexSUBTable& subTable = arrySubtable.vtIndxSubTable[i];
			unsigned short usImgFormat = subTable.Header.imageFormat;
			unsigned short usindexFormat = subTable.Header.indexFormat;
			unsigned long ulDataoffset = ulEBDTPos + subTable.Header.imageDataOffset;
			
			ulDataoffset += subTable.SubTable1.vtsbitOffsets[i];
			fseek(m_pFontFile, ulDataoffset, SEEK_SET);
			unsigned long dataLen = subTable.SubTable1.vtsbitOffsets[i + 1] - subTable.SubTable1.vtsbitOffsets[i];

			if (usImgFormat == 1){					
				CBDTFormat1& format1 = subTable.dataFrmt1;
				DecodeEBDTFormat1(format1,dataLen);
			}
			if (usImgFormat == 2) {
				CBDTFormat2& format2 = subTable.dataFrmt2;
				DecodeEBDTFormat2(format2, dataLen);
			}
			if (usImgFormat == 5) {
				CBDTFormat5& format5 = subTable.dataFrmt5;
				DecodeEBDTFormat5(format5, dataLen);
			}
			if (usImgFormat == 6) {
				CBDTFormat6& format6 = subTable.dataFrmt6;
				DecodeEBDTFormat6(format6, dataLen);
			}
			if (usImgFormat == 7) {
				CBDTFormat7& format7 = subTable.dataFrmt7;
				DecodeEBDTFormat7(format7, dataLen);					
			}
			if (usImgFormat == 8) {
				CBDTFormat8& format8 = subTable.dataFrmt8;
				DecodeEBDTFormat8(format8);
					
			}
			if (usImgFormat == 9) {
				CBDTFormat9& format9 = subTable.dataFrmt9;
				DecodeEBDTFormat9(format9);					
			}
			if (usImgFormat == 17) {
				CBDTFormat17& format17 = subTable.dataFrmt17;
				DecodeEBDTFormat17(format17);					
			}
			if (usImgFormat == 18) {
				CBDTFormat18& format18 = subTable.dataFrmt18;
				DecodeEBDTFormat18(format18);					
			}
			if (usImgFormat == 19) {
				CBDTFormat19& format19 = subTable.dataFrmt19;
				DecodeEBDTFormat19(format19);					
			}						
		}
	
	}	// end of void CHYFontCodec::DecodeEBDTData()

	void CHYFontCodec::DecodeEBDTFormat1(CBDTFormat1& format1, unsigned long dataLen)
	{
		fread(&format1.SmlGlyphMtcs.height, 1, 1, m_pFontFile);
		fread(&format1.SmlGlyphMtcs.width, 1, 1, m_pFontFile);
		fread(&format1.SmlGlyphMtcs.BearingX, 1, 1, m_pFontFile);
		fread(&format1.SmlGlyphMtcs.BearingY, 1, 1, m_pFontFile);
		fread(&format1.SmlGlyphMtcs.Advance, 1, 1, m_pFontFile);
		for (unsigned long x = 0; x < dataLen; x++) {
			unsigned char cTmp;
			fread(&cTmp, 1, 1, m_pFontFile);
			format1.imagedata.push_back(cTmp);
		}

	}	// end of void CHYFontCodec::DecodeEBDTFormat1()

	void CHYFontCodec::DecodeEBDTFormat2(CBDTFormat2& format2, unsigned long dataLen)
	{
		fread(&format2.SmlGlyphMtcs.height, 1, 1, m_pFontFile);
		fread(&format2.SmlGlyphMtcs.width, 1, 1, m_pFontFile);
		fread(&format2.SmlGlyphMtcs.BearingX, 1, 1, m_pFontFile);
		fread(&format2.SmlGlyphMtcs.BearingY, 1, 1, m_pFontFile);
		fread(&format2.SmlGlyphMtcs.Advance, 1, 1, m_pFontFile);

		for (unsigned long x = 0; x < dataLen; x++) {
			unsigned char cTmp;
			fread(&cTmp, 1, 1, m_pFontFile);
			format2.imagedata.push_back(cTmp);
		}

	}	// end of void CHYFontCodec::DecodeEBDTFormat2()

	void CHYFontCodec::DecodeEBDTFormat5(CBDTFormat5& format5, unsigned long dataLen)
	{
		for (unsigned long x = 0; x < dataLen; x++) {
			unsigned char cTmp;
			fread(&cTmp, 1, 1, m_pFontFile);
			format5.imagedata.push_back(cTmp);
		}
	
	}	// end of void CHYFontCo1dec::DecodeEBDTFormat5()

	void CHYFontCodec::DecodeEBDTFormat6(CBDTFormat6& format6, unsigned long dataLen)
	{	
		fread(&format6.bigGlyphMtcs.height, 1, 1, m_pFontFile);
		fread(&format6.bigGlyphMtcs.width, 1, 1, m_pFontFile);
		fread(&format6.bigGlyphMtcs.horiBearingX, 1, 1, m_pFontFile);
		fread(&format6.bigGlyphMtcs.horiBearingY, 1, 1, m_pFontFile);
		fread(&format6.bigGlyphMtcs.horiAdvance, 1, 1, m_pFontFile);
		fread(&format6.bigGlyphMtcs.vertBearingX, 1, 1, m_pFontFile);
		fread(&format6.bigGlyphMtcs.vertBearingY, 1, 1, m_pFontFile);
		fread(&format6.bigGlyphMtcs.vertAdvance, 1, 1, m_pFontFile);

		for (unsigned long x = 0; x < dataLen; x++) {
			unsigned char cTmp;
			fread(&cTmp, 1, 1, m_pFontFile);
			format6.imagedata.push_back(cTmp);
		}
	
	}	// end of void CHYFontCodec::DecodeEBDTFormat6()

	void CHYFontCodec::DecodeEBDTFormat7(CBDTFormat7& format7, unsigned long dataLen)
	{
		fread(&format7.bigGlyphMtcs.height, 1, 1, m_pFontFile);
		fread(&format7.bigGlyphMtcs.width, 1, 1, m_pFontFile);
		fread(&format7.bigGlyphMtcs.horiBearingX, 1, 1, m_pFontFile);
		fread(&format7.bigGlyphMtcs.horiBearingY, 1, 1, m_pFontFile);
		fread(&format7.bigGlyphMtcs.horiAdvance, 1, 1, m_pFontFile);
		fread(&format7.bigGlyphMtcs.vertBearingX, 1, 1, m_pFontFile);
		fread(&format7.bigGlyphMtcs.vertBearingY, 1, 1, m_pFontFile);
		fread(&format7.bigGlyphMtcs.vertAdvance, 1, 1, m_pFontFile);

		for (unsigned long x = 0; x < dataLen; x++) {
			unsigned char cTmp;
			fread(&cTmp, 1, 1, m_pFontFile);
			format7.imagedata.push_back(cTmp);
		}		
	
	}	// end of void CHYFontCodec::DecodeEBDTFormat7()

	void CHYFontCodec::DecodeEBDTFormat8(CBDTFormat8& format8)
	{
		fread(&format8.smallGlyphMtcs.height, 1, 1, m_pFontFile);
		fread(&format8.smallGlyphMtcs.width, 1, 1, m_pFontFile);
		fread(&format8.smallGlyphMtcs.BearingX, 1, 1, m_pFontFile);
		fread(&format8.smallGlyphMtcs.BearingY, 1, 1, m_pFontFile);
		fread(&format8.smallGlyphMtcs.Advance, 1, 1, m_pFontFile);

		fread(&format8.pad, 1, 1, m_pFontFile);

		fread(&format8.numComponents, 2, 1, m_pFontFile);
		format8.numComponents = hy_cdr_int16_to(format8.numComponents);
		for (int i = 0; i < format8.numComponents; i++){
			CEbdtComponent	EbdtComponent;
			fread(&EbdtComponent.glyphID, 2, 1, m_pFontFile);
			EbdtComponent.glyphID = hy_cdr_int16_to(EbdtComponent.glyphID);
			fread(&EbdtComponent.xOffset, 1, 1, m_pFontFile);
			fread(&EbdtComponent.yOffset, 1, 1, m_pFontFile);

			format8.vtEbdtComponent.push_back(EbdtComponent);
		}

	}	// end of void CHYFontCodec::DecodeEBDTFormat8()

	void CHYFontCodec::DecodeEBDTFormat9(CBDTFormat9& format9)
	{
		fread(&format9.bigGlyphMtcs.height, 1, 1, m_pFontFile);
		fread(&format9.bigGlyphMtcs.width, 1, 1, m_pFontFile);
		fread(&format9.bigGlyphMtcs.horiBearingX, 1, 1, m_pFontFile);
		fread(&format9.bigGlyphMtcs.horiBearingY, 1, 1, m_pFontFile);
		fread(&format9.bigGlyphMtcs.horiAdvance, 1, 1, m_pFontFile);
		fread(&format9.bigGlyphMtcs.vertBearingX, 1, 1, m_pFontFile);
		fread(&format9.bigGlyphMtcs.vertBearingY, 1, 1, m_pFontFile);
		fread(&format9.bigGlyphMtcs.vertAdvance, 1, 1, m_pFontFile);

		fread(&format9.numComponents, 2, 1, m_pFontFile);
		format9.numComponents = hy_cdr_int16_to(format9.numComponents);
		for (int i = 0; i < format9.numComponents; i++) {
			CEbdtComponent	EbdtComponent;
			fread(&EbdtComponent.glyphID, 2, 1, m_pFontFile);
			EbdtComponent.glyphID = hy_cdr_int16_to(EbdtComponent.glyphID);
			fread(&EbdtComponent.xOffset, 1, 1, m_pFontFile);
			fread(&EbdtComponent.yOffset, 1, 1, m_pFontFile);

			format9.vtEbdtComponent.push_back(EbdtComponent);
		}
	
	}	// end of void CHYFontCodec::DecodeEBDTFormat9()

	void	CHYFontCodec::DecodeEBDTFormat17(CBDTFormat17& format17)
	{			
		fread(&format17.smallGlyphMtcs.height, 1, 1, m_pFontFile);
		fread(&format17.smallGlyphMtcs.width, 1, 1, m_pFontFile);
		fread(&format17.smallGlyphMtcs.BearingX, 1, 1, m_pFontFile);
		fread(&format17.smallGlyphMtcs.BearingY, 1, 1, m_pFontFile);
		fread(&format17.smallGlyphMtcs.Advance, 1, 1, m_pFontFile);

		fread(&format17.dataLen, 4, 1, m_pFontFile);
		format17.dataLen = hy_cdr_int32_to(format17.dataLen);
		for (unsigned long x = 0; x < format17.dataLen; x++){
			unsigned char cTmp;
			fread(&cTmp, 1, 1, m_pFontFile);
			format17.data.push_back(cTmp);
		}

	}	// end of void	CHYFontCodec::DecodeEBDTFormat17()

	void	CHYFontCodec::DecodeEBDTFormat18(CBDTFormat18& format18)
	{		
		fread(&format18.bigGlyphMtcs.height, 1, 1, m_pFontFile);
		fread(&format18.bigGlyphMtcs.width, 1, 1, m_pFontFile);
		fread(&format18.bigGlyphMtcs.horiBearingX, 1, 1, m_pFontFile);
		fread(&format18.bigGlyphMtcs.horiBearingY, 1, 1, m_pFontFile);
		fread(&format18.bigGlyphMtcs.horiAdvance, 1, 1, m_pFontFile);
		fread(&format18.bigGlyphMtcs.vertBearingX, 1, 1, m_pFontFile);
		fread(&format18.bigGlyphMtcs.vertBearingY, 1, 1, m_pFontFile);
		fread(&format18.bigGlyphMtcs.vertAdvance, 1, 1, m_pFontFile);

		fread(&format18.dataLen, 4, 1, m_pFontFile);
		format18.dataLen = hy_cdr_int32_to(format18.dataLen);
		for (unsigned long x = 0; x < format18.dataLen; x++)	{
			unsigned char cTmp;
			fread(&cTmp, 1, 1, m_pFontFile);
			format18.data.push_back(cTmp);
		}
	
	}	// end of void	CHYFontCodec::DecodeEBDTFormat18()

	void	CHYFontCodec::DecodeEBDTFormat19(CBDTFormat19& format19)
	{		
		format19.dataLen = hy_cdr_int32_to(format19.dataLen);
		for (unsigned long x = 0; x < format19.dataLen; x++){
			unsigned char cTmp;
			fread(&cTmp, 1, 1, m_pFontFile);
			format19.data.push_back(cTmp);
		}
	
	}	// end of void	CHYFontCodec::DecodeEBDTFormat19()

	int	 CHYFontCodec::EncodeEBDT()
	{
		m_HYCBDT.majorVersion = 2;
		m_HYCBDT.minorVersion = 0;

		int iEntryIndex = m_HYTbDirectory.FindTableEntry(EBDT_TAG);
		if (iEntryIndex == -1) return FONT_ERR_EBDT_ENCODE;

		CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
		tbEntry.offset = ftell(m_pFontFile);
		EncodeEBDTData(tbEntry.offset);
		tbEntry.length = ftell(m_pFontFile) - tbEntry.offset;
		::HY_4ByteAlign(m_pFontFile, tbEntry.length);	

		return HY_NOERROR;
	
	}	//end of int CHYFontCodec::EncodeEBDT()

	int	 CHYFontCodec::EncodeCBDT()
	{
		m_HYCBDT.majorVersion = 3;
		m_HYCBDT.minorVersion = 0;

		int iEntryIndex = m_HYTbDirectory.FindTableEntry(EBDT_TAG);
		if (iEntryIndex == -1) return FONT_ERR_EBDT_ENCODE;

		CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
		tbEntry.offset = ftell(m_pFontFile);
		EncodeEBDTData(tbEntry.offset);

		tbEntry.length = ftell(m_pFontFile) - tbEntry.offset;
		::HY_4ByteAlign(m_pFontFile, tbEntry.length);

		return HY_NOERROR;

	}	//end of int CHYFontCodec::EncodeEBDT()

	int	 CHYFontCodec::EncodeEBDTData(unsigned long uloffsetEBDT)
	{	
		unsigned short	usTmp = 0;
		unsigned long	ulTmp = 0;
		//Table version number	
		usTmp = hy_cdr_int16_to(m_HYCBDT.majorVersion);
		fwrite(&usTmp, 2, 1, m_pFontFile);
		usTmp = hy_cdr_int16_to(m_HYCBDT.minorVersion);
		fwrite(&usTmp, 2, 1, m_pFontFile);
#if 0
		size_t stBTSzTb = m_HYCblc.vtBitmapSizeTb.size();
		for (size_t i = 0; i < stBTSzTb; i++){
			BitmapSize& bitmapSizeTB = m_HYCblc.vtBitmapSizeTb[i];
			size_t tbArraySize = m_HYCblc.vtIndexSubTableArray.size();
			for (size_t j = 0; j < tbArraySize; j++){
				size_t tArySize = 0;
				IndexSubTableArray& SubTableArray = m_HYCblc.vtIndexSubTableArray[j];
				IndexSUBTable& SUBTable = SubTableArray.IndexSubTb;

				switch (SUBTable.Header.imageFormat)
				{
				case 1:
					EncodeEBDTFormat1(SubTableArray, uloffsetEBDT);
					break;
				case 2:
					EncodeEBDTFormat2(SubTableArray, uloffsetEBDT);
					break;
				case 5:
					EncodeEBDTFormat5(SubTableArray, uloffsetEBDT);
					break;
				case 6:
					EncodeEBDTFormat6(SubTableArray, uloffsetEBDT);
					break;
				case 7:
					EncodeEBDTFormat7(SubTableArray, uloffsetEBDT);
					break;
				case 8:
					EncodeEBDTFormat8(SubTableArray, uloffsetEBDT);
					break;
				case 9:
					EncodeEBDTFormat9(SubTableArray, uloffsetEBDT);
					break;
				case 17:
					EncodeEBDTFormat17(SubTableArray, uloffsetEBDT);
					break;
				case 18:
					EncodeEBDTFormat18(SubTableArray, uloffsetEBDT);
					break;
				case 19:
					EncodeEBDTFormat19(SubTableArray, uloffsetEBDT);
					break;
				default:
					break;
				}
			}
		}	
#else 
		size_t stBTSzTb = m_HYCblc.vtBitmapSizeTb.size();
		for (size_t i = 0; i < stBTSzTb; i++) {
			BitmapSize& bitmapSizeTB = m_HYCblc.vtBitmapSizeTb[i];
			IndexSubTableArray& SubTableArray = m_HYCblc.vtIndexSubTableArray[i];
			for (size_t j = 0; j < tbArraySize; j++) {
				size_t tArySize = 0;
				IndexSubTableArray& SubTableArray = m_HYCblc.vtIndexSubTableArray[j];
				IndexSUBTable& SUBTable = SubTableArray.IndexSubTb;

				switch (SUBTable.Header.imageFormat)
				{
				case 1:
					EncodeEBDTFormat1(SubTableArray, uloffsetEBDT);
					break;
				case 2:
					EncodeEBDTFormat2(SubTableArray, uloffsetEBDT);
					break;
				case 5:
					EncodeEBDTFormat5(SubTableArray, uloffsetEBDT);
					break;
				case 6:
					EncodeEBDTFormat6(SubTableArray, uloffsetEBDT);
					break;
				case 7:
					EncodeEBDTFormat7(SubTableArray, uloffsetEBDT);
					break;
				case 8:
					EncodeEBDTFormat8(SubTableArray, uloffsetEBDT);
					break;
				case 9:
					EncodeEBDTFormat9(SubTableArray, uloffsetEBDT);
					break;
				case 17:
					EncodeEBDTFormat17(SubTableArray, uloffsetEBDT);
					break;
				case 18:
					EncodeEBDTFormat18(SubTableArray, uloffsetEBDT);
					break;
				case 19:
					EncodeEBDTFormat19(SubTableArray, uloffsetEBDT);
					break;
				default:
					break;
				}
			}
		}


#endif 
		return true;

	}	// end of int CHYFontCodec::EncodeBDTData()

	void CHYFontCodec::EncodeEBDTFormat1(IndexSubTableArray& SubTableArray, unsigned long uloffsetEBDT)
	{	
		int iseek=0;
		unsigned long ulCurrent = ftell(m_pFontFile);
		SubTableArray.IndexSubTb.Header.imageDataOffset = ulCurrent - uloffsetEBDT;
		unsigned long GlyphOffset = 0;

		for (int i = SubTableArray.firstGlyphIndex; i <= SubTableArray.lastGlyphIndex; i++) {
			CBDTFormat1& Foramt1 = SubTableArray.IndexSubTb.vtDdataForm1[iseek];
			fwrite(&Foramt1.SmlGlyphMtcs.height, 1, 1, m_pFontFile);
			fwrite(&Foramt1.SmlGlyphMtcs.width, 1, 1, m_pFontFile);
			fwrite(&Foramt1.SmlGlyphMtcs.BearingX, 1, 1, m_pFontFile);
			fwrite(&Foramt1.SmlGlyphMtcs.BearingY, 1, 1, m_pFontFile);
			fwrite(&Foramt1.SmlGlyphMtcs.Advance, 1, 1, m_pFontFile);

			size_t stSize = Foramt1.imagedata.size();
			for (size_t y = 0; y < stSize; y++) {
				fwrite(&Foramt1.imagedata[y], 1, 1, m_pFontFile);
			}

			// 计算offset
			GlyphOffset = ftell(m_pFontFile) - ulCurrent;
			if (SubTableArray.IndexSubTb.Header.indexFormat == 1){				
				SubTableArray.IndexSubTb.SubTable1.vtsbitOffsets.push_back(GlyphOffset);
			}			
			if (SubTableArray.IndexSubTb.Header.indexFormat == 3) {				
				SubTableArray.IndexSubTb.SubTable3.vtOffsetArray.push_back((unsigned short)GlyphOffset);
			}
			if (SubTableArray.IndexSubTb.Header.indexFormat == 4) {
				GlyphIdOffsetPair Pair;				
				Pair.glyphID = i;				
				Pair.sbitOffset = (unsigned short)GlyphOffset;
				SubTableArray.IndexSubTb.SubTable4.vtGlyphArray.push_back(Pair);
			}
			iseek++;
		}

		// it is necessary to store one extra element in the sbitOffsets array pointing just past the end of the range’s image data. 
		//This will allow the correct calculation of the image data size for the last glyph in the range.
		GlyphOffset = ftell(m_pFontFile) - ulCurrent;
		if (SubTableArray.IndexSubTb.Header.indexFormat == 1) {
			SubTableArray.IndexSubTb.SubTable1.vtsbitOffsets.push_back(GlyphOffset);
		}
		if (SubTableArray.IndexSubTb.Header.indexFormat == 3) {
			SubTableArray.IndexSubTb.SubTable3.vtOffsetArray.push_back((unsigned short)GlyphOffset);
		}
		if (SubTableArray.IndexSubTb.Header.indexFormat == 4) {
			GlyphIdOffsetPair Pair;
			Pair.glyphID = 0;
			Pair.sbitOffset = (unsigned short)GlyphOffset;
			SubTableArray.IndexSubTb.SubTable4.vtGlyphArray.push_back(Pair);
			SubTableArray.IndexSubTb.SubTable4.numGlyphs = iseek;
		}
	
	}	// end of void CHYFontCodec::EncodeEBDTFormat1()

	void CHYFontCodec::EncodeEBDTFormat2(IndexSubTableArray& SubTableArray, unsigned long uloffsetEBDT)
	{
		int iseek = 0;
		unsigned long ulCurrent = ftell(m_pFontFile);
		SubTableArray.IndexSubTb.Header.imageDataOffset = ulCurrent - uloffsetEBDT;
		unsigned long GlyphOffset = 0;
		for (int i = SubTableArray.firstGlyphIndex; i <= SubTableArray.lastGlyphIndex; i++) {
			CBDTFormat2& Foramt2 = SubTableArray.IndexSubTb.vtDdataForm2[iseek];
			fwrite(&Foramt2.SmlGlyphMtcs.height, 1, 1, m_pFontFile);
			fwrite(&Foramt2.SmlGlyphMtcs.width, 1, 1, m_pFontFile);
			fwrite(&Foramt2.SmlGlyphMtcs.BearingX, 1, 1, m_pFontFile);
			fwrite(&Foramt2.SmlGlyphMtcs.BearingY, 1, 1, m_pFontFile);
			fwrite(&Foramt2.SmlGlyphMtcs.Advance, 1, 1, m_pFontFile);

			size_t stSize = Foramt2.imagedata.size();
			for (size_t y = 0; y < stSize; y++) {
				fwrite(&Foramt2.imagedata[y], 1, 1, m_pFontFile);
			}

			// 计算offset
			GlyphOffset = ftell(m_pFontFile) - ulCurrent;
			if (SubTableArray.IndexSubTb.Header.indexFormat == 1) {				
				SubTableArray.IndexSubTb.SubTable1.vtsbitOffsets.push_back(GlyphOffset);
			}
			if (SubTableArray.IndexSubTb.Header.indexFormat == 3) {				
				SubTableArray.IndexSubTb.SubTable3.vtOffsetArray.push_back((unsigned short)GlyphOffset);
			}
			if (SubTableArray.IndexSubTb.Header.indexFormat == 4) {
				GlyphIdOffsetPair Pair;
				Pair.glyphID = i;
				Pair.sbitOffset = (unsigned short)GlyphOffset;
				SubTableArray.IndexSubTb.SubTable4.vtGlyphArray.push_back(Pair);
			}
			iseek++;
		}		

		// it is necessary to store one extra element in the sbitOffsets array pointing just past the end of the range’s image data. 
		//This will allow the correct calculation of the image data size for the last glyph in the range.
		GlyphOffset = ftell(m_pFontFile) - ulCurrent;
		if (SubTableArray.IndexSubTb.Header.indexFormat == 1) {
			SubTableArray.IndexSubTb.SubTable1.vtsbitOffsets.push_back(GlyphOffset);
		}
		if (SubTableArray.IndexSubTb.Header.indexFormat == 3) {
			SubTableArray.IndexSubTb.SubTable3.vtOffsetArray.push_back((unsigned short)GlyphOffset);
		}
		if (SubTableArray.IndexSubTb.Header.indexFormat == 4) {
			GlyphIdOffsetPair Pair;
			Pair.glyphID = 0;
			Pair.sbitOffset = (unsigned short)GlyphOffset;
			SubTableArray.IndexSubTb.SubTable4.vtGlyphArray.push_back(Pair);
			SubTableArray.IndexSubTb.SubTable4.numGlyphs = iseek;
		}
	
	}	// end of void CHYFontCodec::EncodeEBDTFormat2()

	void CHYFontCodec::EncodeEBDTFormat5(IndexSubTableArray& SubTableArray, unsigned long uloffsetEBDT)
	{
		int iseek = 0;
		unsigned long ulCurrent = ftell(m_pFontFile);
		SubTableArray.IndexSubTb.Header.imageDataOffset = ulCurrent - uloffsetEBDT;

		for (int i = SubTableArray.firstGlyphIndex; i <= SubTableArray.lastGlyphIndex; i++) {
			CBDTFormat5& Foramt5 = SubTableArray.IndexSubTb.vtDdataForm5[iseek];
			size_t stSize = Foramt5.imagedata.size();
			for (size_t y = 0; y < stSize; y++) {
				fwrite(&Foramt5.imagedata[y], 1, 1, m_pFontFile);
			}
			iseek++;
		}
	
	}	// end of void CHYFontCodec::EncodeEBDTFormat5()

	void CHYFontCodec::EncodeEBDTFormat6(IndexSubTableArray& SubTableArray, unsigned long uloffsetEBDT)
	{
		int iseek = 0;
		unsigned long ulCurrent = ftell(m_pFontFile);
		SubTableArray.IndexSubTb.Header.imageDataOffset = ulCurrent - uloffsetEBDT;
		unsigned long GlyphOffset = 0;

		for (int i = SubTableArray.firstGlyphIndex; i <= SubTableArray.lastGlyphIndex; i++) {
			CBDTFormat6& Foramt6 = SubTableArray.IndexSubTb.vtDdataForm6[iseek];
			fwrite(&Foramt6.bigGlyphMtcs.height, 1, 1, m_pFontFile);
			fwrite(&Foramt6.bigGlyphMtcs.width, 1, 1, m_pFontFile);
			fwrite(&Foramt6.bigGlyphMtcs.horiBearingX, 1, 1, m_pFontFile);
			fwrite(&Foramt6.bigGlyphMtcs.horiBearingY, 1, 1, m_pFontFile);
			fwrite(&Foramt6.bigGlyphMtcs.horiAdvance, 1, 1, m_pFontFile);
			fwrite(&Foramt6.bigGlyphMtcs.vertBearingX, 1, 1, m_pFontFile);
			fwrite(&Foramt6.bigGlyphMtcs.vertBearingY, 1, 1, m_pFontFile);
			fwrite(&Foramt6.bigGlyphMtcs.vertAdvance, 1, 1, m_pFontFile);

			size_t stSize = Foramt6.imagedata.size();
			for (size_t y = 0; y < stSize; y++) {
				fwrite(&Foramt6.imagedata[y], 1, 1, m_pFontFile);
			}

			// 计算offset
			GlyphOffset = ftell(m_pFontFile) - ulCurrent;
			if (SubTableArray.IndexSubTb.Header.indexFormat == 1) {
				SubTableArray.IndexSubTb.SubTable1.vtsbitOffsets.push_back(GlyphOffset);
			}
			if (SubTableArray.IndexSubTb.Header.indexFormat == 3) {
				SubTableArray.IndexSubTb.SubTable3.vtOffsetArray.push_back((unsigned short)GlyphOffset);
			}
			if (SubTableArray.IndexSubTb.Header.indexFormat == 4) {
				GlyphIdOffsetPair Pair;
				Pair.glyphID = i;
				Pair.sbitOffset = (unsigned short)GlyphOffset;
				SubTableArray.IndexSubTb.SubTable4.vtGlyphArray.push_back(Pair);
			}
			iseek++;
		}

		// it is necessary to store one extra element in the sbitOffsets array pointing just past the end of the range’s image data. 
		//This will allow the correct calculation of the image data size for the last glyph in the range.
		GlyphOffset = ftell(m_pFontFile) - ulCurrent;
		if (SubTableArray.IndexSubTb.Header.indexFormat == 1) {
			SubTableArray.IndexSubTb.SubTable1.vtsbitOffsets.push_back(GlyphOffset);
		}
		if (SubTableArray.IndexSubTb.Header.indexFormat == 3) {
			SubTableArray.IndexSubTb.SubTable3.vtOffsetArray.push_back((unsigned short)GlyphOffset);
		}
		if (SubTableArray.IndexSubTb.Header.indexFormat == 4) {
			GlyphIdOffsetPair Pair;
			Pair.glyphID = 0;
			Pair.sbitOffset = (unsigned short)GlyphOffset;
			SubTableArray.IndexSubTb.SubTable4.vtGlyphArray.push_back(Pair);
			SubTableArray.IndexSubTb.SubTable4.numGlyphs = iseek;
		}

	}	// end of void CHYFontCodec::EncodeEBDTFormat6()

	void CHYFontCodec::EncodeEBDTFormat7(IndexSubTableArray& SubTableArray, unsigned long uloffsetEBDT)
	{
		int iseek = 0;
		unsigned long ulCurrent = ftell(m_pFontFile);
		SubTableArray.IndexSubTb.Header.imageDataOffset = ulCurrent - uloffsetEBDT;
		unsigned long GlyphOffset = 0;

		for (int i = SubTableArray.firstGlyphIndex; i <= SubTableArray.lastGlyphIndex; i++) {
			CBDTFormat7& Format7 = SubTableArray.IndexSubTb.vtDdataForm7[iseek];
			fwrite(&Format7.bigGlyphMtcs.height, 1, 1, m_pFontFile);
			fwrite(&Format7.bigGlyphMtcs.width, 1, 1, m_pFontFile);
			fwrite(&Format7.bigGlyphMtcs.horiBearingX, 1, 1, m_pFontFile);
			fwrite(&Format7.bigGlyphMtcs.horiBearingY, 1, 1, m_pFontFile);
			fwrite(&Format7.bigGlyphMtcs.horiAdvance, 1, 1, m_pFontFile);
			fwrite(&Format7.bigGlyphMtcs.vertBearingX, 1, 1, m_pFontFile);
			fwrite(&Format7.bigGlyphMtcs.vertBearingY, 1, 1, m_pFontFile);
			fwrite(&Format7.bigGlyphMtcs.vertAdvance, 1, 1, m_pFontFile);

			size_t stSize = Format7.imagedata.size();
			for (size_t y = 0; y < stSize; y++) {
				fwrite(&Format7.imagedata[y], 1, 1, m_pFontFile);
			}

			// 计算offset
			GlyphOffset = ftell(m_pFontFile) - ulCurrent;
			if (SubTableArray.IndexSubTb.Header.indexFormat == 1) {
				SubTableArray.IndexSubTb.SubTable1.vtsbitOffsets.push_back(GlyphOffset);
			}
			if (SubTableArray.IndexSubTb.Header.indexFormat == 3) {
				SubTableArray.IndexSubTb.SubTable3.vtOffsetArray.push_back((unsigned short)GlyphOffset);
			}
			if (SubTableArray.IndexSubTb.Header.indexFormat == 4) {
				GlyphIdOffsetPair Pair;
				Pair.glyphID = i;
				Pair.sbitOffset = (unsigned short)GlyphOffset;
				SubTableArray.IndexSubTb.SubTable4.vtGlyphArray.push_back(Pair);
			}
			iseek++;
		}

		// it is necessary to store one extra element in the sbitOffsets array pointing just past the end of the range’s image data. 
		//This will allow the correct calculation of the image data size for the last glyph in the range.
		GlyphOffset = ftell(m_pFontFile) - ulCurrent;
		if (SubTableArray.IndexSubTb.Header.indexFormat == 1) {
			SubTableArray.IndexSubTb.SubTable1.vtsbitOffsets.push_back(GlyphOffset);
		}
		if (SubTableArray.IndexSubTb.Header.indexFormat == 3) {
			SubTableArray.IndexSubTb.SubTable3.vtOffsetArray.push_back((unsigned short)GlyphOffset);
		}
		if (SubTableArray.IndexSubTb.Header.indexFormat == 4) {
			GlyphIdOffsetPair Pair;
			Pair.glyphID = 0;
			Pair.sbitOffset = (unsigned short)GlyphOffset;
			SubTableArray.IndexSubTb.SubTable4.vtGlyphArray.push_back(Pair);
			SubTableArray.IndexSubTb.SubTable4.numGlyphs = iseek;
		}
	
	}	// end of void CHYFontCodec::EncodeEBDTFormat7()

	void CHYFontCodec::EncodeEBDTFormat8(IndexSubTableArray& SubTableArray, unsigned long uloffsetEBDT)
	{
		int iseek = 0;
		unsigned long ulCurrent = ftell(m_pFontFile);
		SubTableArray.IndexSubTb.Header.imageDataOffset = ulCurrent - uloffsetEBDT;

		char cPad = 0;
		unsigned short usTmp = 0;
		for (int i = SubTableArray.firstGlyphIndex; i <= SubTableArray.lastGlyphIndex; i++) {
			CBDTFormat8& Foramt8 = SubTableArray.IndexSubTb.vtDdataForm8[iseek];
			fwrite(&Foramt8.smallGlyphMtcs.height, 1, 1, m_pFontFile);
			fwrite(&Foramt8.smallGlyphMtcs.width, 1, 1, m_pFontFile);
			fwrite(&Foramt8.smallGlyphMtcs.BearingX, 1, 1, m_pFontFile);
			fwrite(&Foramt8.smallGlyphMtcs.BearingY, 1, 1, m_pFontFile);
			fwrite(&Foramt8.smallGlyphMtcs.Advance, 1, 1, m_pFontFile);

			fwrite(&cPad, 1, 1, m_pFontFile);

			usTmp = hy_cdr_int16_to(Foramt8.numComponents);
			fwrite(&usTmp, 2, 1, m_pFontFile);

			for (int j = 0; j < Foramt8.numComponents; j++){				
				CEbdtComponent&	EbdtCmpnent = Foramt8.vtEbdtComponent[j];
				usTmp = hy_cdr_int16_to(EbdtCmpnent.glyphID);
				fwrite(&usTmp, 2, 1, m_pFontFile);

				fwrite(&EbdtCmpnent.xOffset, 1, 1, m_pFontFile);
				fwrite(&EbdtCmpnent.yOffset, 1, 1, m_pFontFile);
			}
		}
	
	}	// end of void CHYFontCodec::EncodeEBDTFormat8()

	void CHYFontCodec::EncodeEBDTFormat9(IndexSubTableArray& SubTableArray, unsigned long uloffsetEBDT)
	{
		int iseek = 0;
		unsigned long ulCurrent = ftell(m_pFontFile);
		SubTableArray.IndexSubTb.Header.imageDataOffset = ulCurrent - uloffsetEBDT;

		char cPad = 0;
		unsigned short usTmp = 0;
		for (int i = SubTableArray.firstGlyphIndex; i <= SubTableArray.lastGlyphIndex; i++) {
			CBDTFormat9& Foramt9 = SubTableArray.IndexSubTb.vtDdataForm9[iseek];
			fwrite(&Foramt9.bigGlyphMtcs.height, 1, 1, m_pFontFile);
			fwrite(&Foramt9.bigGlyphMtcs.width, 1, 1, m_pFontFile);
			fwrite(&Foramt9.bigGlyphMtcs.horiBearingX, 1, 1, m_pFontFile);
			fwrite(&Foramt9.bigGlyphMtcs.horiBearingY, 1, 1, m_pFontFile);
			fwrite(&Foramt9.bigGlyphMtcs.horiAdvance, 1, 1, m_pFontFile);
			fwrite(&Foramt9.bigGlyphMtcs.vertBearingX, 1, 1, m_pFontFile);
			fwrite(&Foramt9.bigGlyphMtcs.vertBearingY, 1, 1, m_pFontFile);
			fwrite(&Foramt9.bigGlyphMtcs.vertAdvance, 1, 1, m_pFontFile);			

			fwrite(&cPad, 1, 1, m_pFontFile);

			usTmp = hy_cdr_int16_to(Foramt9.numComponents);
			fwrite(&usTmp, 2, 1, m_pFontFile);

			for (int j = 0; j < Foramt9.numComponents; j++) {
				CEbdtComponent& EbdtCmpnent = Foramt9.vtEbdtComponent[j];
				usTmp = hy_cdr_int16_to(EbdtCmpnent.glyphID);
				fwrite(&usTmp, 2, 1, m_pFontFile);

				fwrite(&EbdtCmpnent.xOffset, 1, 1, m_pFontFile);
				fwrite(&EbdtCmpnent.yOffset, 1, 1, m_pFontFile);
			}
		}
	
	}	// end of void CHYFontCodec::EncodeEBDTFormat9()

	void CHYFontCodec::EncodeEBDTFormat17(IndexSubTableArray& SubTableArray, unsigned long uloffsetEBDT)
	{
		int iseek = 0;
		unsigned long ulCurrent = ftell(m_pFontFile);
		SubTableArray.IndexSubTb.Header.imageDataOffset = ulCurrent - uloffsetEBDT;

		char cPad = 0;
		unsigned long ulTmp = 0, GlyphOffset=0;

		for (int i = SubTableArray.firstGlyphIndex; i <= SubTableArray.lastGlyphIndex; i++) {
			CBDTFormat17& Foramt17 = SubTableArray.IndexSubTb.vtDdataForm17[iseek];
			fwrite(&Foramt17.smallGlyphMtcs.height, 1, 1, m_pFontFile);
			fwrite(&Foramt17.smallGlyphMtcs.width, 1, 1, m_pFontFile);
			fwrite(&Foramt17.smallGlyphMtcs.BearingX, 1, 1, m_pFontFile);
			fwrite(&Foramt17.smallGlyphMtcs.BearingY, 1, 1, m_pFontFile);
			fwrite(&Foramt17.smallGlyphMtcs.Advance, 1, 1, m_pFontFile);

			ulTmp = hy_cdr_int32_to(Foramt17.dataLen);
			fwrite(&ulTmp, 4, 1, m_pFontFile);
			for (int y = 0; y < Foramt17.dataLen; y++){
				fwrite(&Foramt17.data[y], 1, 1, m_pFontFile);
			}

			// 计算offset
			GlyphOffset = ftell(m_pFontFile) - ulCurrent;
			if (SubTableArray.IndexSubTb.Header.indexFormat == 1) {
				SubTableArray.IndexSubTb.SubTable1.vtsbitOffsets.push_back(GlyphOffset);
			}
			if (SubTableArray.IndexSubTb.Header.indexFormat == 3) {
				SubTableArray.IndexSubTb.SubTable3.vtOffsetArray.push_back((unsigned short)GlyphOffset);
			}
			if (SubTableArray.IndexSubTb.Header.indexFormat == 4) {
				GlyphIdOffsetPair Pair;
				Pair.glyphID = i;
				Pair.sbitOffset = (unsigned short)GlyphOffset;
				SubTableArray.IndexSubTb.SubTable4.vtGlyphArray.push_back(Pair);
			}
			iseek++;
		}

		// it is necessary to store one extra element in the sbitOffsets array pointing just past the end of the range’s image data. 
		//This will allow the correct calculation of the image data size for the last glyph in the range.
		GlyphOffset = ftell(m_pFontFile) - ulCurrent;
		if (SubTableArray.IndexSubTb.Header.indexFormat == 1) {
			SubTableArray.IndexSubTb.SubTable1.vtsbitOffsets.push_back(GlyphOffset);
		}
		if (SubTableArray.IndexSubTb.Header.indexFormat == 3) {
			SubTableArray.IndexSubTb.SubTable3.vtOffsetArray.push_back((unsigned short)GlyphOffset);
		}
		if (SubTableArray.IndexSubTb.Header.indexFormat == 4) {
			GlyphIdOffsetPair Pair;
			Pair.glyphID = 0;
			Pair.sbitOffset = (unsigned short)GlyphOffset;
			SubTableArray.IndexSubTb.SubTable4.vtGlyphArray.push_back(Pair);
			SubTableArray.IndexSubTb.SubTable4.numGlyphs = iseek;
		}
	
	}	// end of void CHYFontCodec::EncodeEBDTFormat17()

	void CHYFontCodec::EncodeEBDTFormat18(IndexSubTableArray& SubTableArray, unsigned long uloffsetEBDT)
	{
		int iseek = 0;
		unsigned long ulCurrent = ftell(m_pFontFile);
		SubTableArray.IndexSubTb.Header.imageDataOffset = ulCurrent - uloffsetEBDT;
		
		unsigned long ulTmp = 0, GlyphOffset = 0;
		for (int i = SubTableArray.firstGlyphIndex; i <= SubTableArray.lastGlyphIndex; i++) {
			CBDTFormat18& Foramt18 = SubTableArray.IndexSubTb.vtDdataForm18[iseek];
			fwrite(&Foramt18.bigGlyphMtcs.height, 1, 1, m_pFontFile);
			fwrite(&Foramt18.bigGlyphMtcs.width, 1, 1, m_pFontFile);
			fwrite(&Foramt18.bigGlyphMtcs.horiBearingX, 1, 1, m_pFontFile);
			fwrite(&Foramt18.bigGlyphMtcs.horiBearingY, 1, 1, m_pFontFile);
			fwrite(&Foramt18.bigGlyphMtcs.horiAdvance, 1, 1, m_pFontFile);
			fwrite(&Foramt18.bigGlyphMtcs.vertBearingX, 1, 1, m_pFontFile);
			fwrite(&Foramt18.bigGlyphMtcs.vertBearingY, 1, 1, m_pFontFile);
			fwrite(&Foramt18.bigGlyphMtcs.vertAdvance, 1, 1, m_pFontFile);

			ulTmp = hy_cdr_int32_to(Foramt18.dataLen);
			fwrite(&ulTmp, 4, 1, m_pFontFile);
			for (int y = 0; y < Foramt18.dataLen; y++) {
				fwrite(&Foramt18.data[y], 1, 1, m_pFontFile);
			}

			// 计算offset
			GlyphOffset = ftell(m_pFontFile) - ulCurrent;
			if (SubTableArray.IndexSubTb.Header.indexFormat == 1) {
				SubTableArray.IndexSubTb.SubTable1.vtsbitOffsets.push_back(GlyphOffset);
			}
			if (SubTableArray.IndexSubTb.Header.indexFormat == 3) {
				SubTableArray.IndexSubTb.SubTable3.vtOffsetArray.push_back((unsigned short)GlyphOffset);
			}
			if (SubTableArray.IndexSubTb.Header.indexFormat == 4) {
				GlyphIdOffsetPair Pair;
				Pair.glyphID = i;
				Pair.sbitOffset = (unsigned short)GlyphOffset;
				SubTableArray.IndexSubTb.SubTable4.vtGlyphArray.push_back(Pair);
			}
			iseek++;
		}

		// it is necessary to store one extra element in the sbitOffsets array pointing just past the end of the range’s image data. 
		//This will allow the correct calculation of the image data size for the last glyph in the range.
		GlyphOffset = ftell(m_pFontFile) - ulCurrent;
		if (SubTableArray.IndexSubTb.Header.indexFormat == 1) {
			SubTableArray.IndexSubTb.SubTable1.vtsbitOffsets.push_back(GlyphOffset);
		}
		if (SubTableArray.IndexSubTb.Header.indexFormat == 3) {
			SubTableArray.IndexSubTb.SubTable3.vtOffsetArray.push_back((unsigned short)GlyphOffset);
		}
		if (SubTableArray.IndexSubTb.Header.indexFormat == 4) {
			GlyphIdOffsetPair Pair;
			Pair.glyphID = 0;
			Pair.sbitOffset = (unsigned short)GlyphOffset;
			SubTableArray.IndexSubTb.SubTable4.vtGlyphArray.push_back(Pair);
			SubTableArray.IndexSubTb.SubTable4.numGlyphs = iseek;
		}
	
	}	// end of void CHYFontCodec::EncodeEBDTFormat18()

	void CHYFontCodec::EncodeEBDTFormat19(IndexSubTableArray& SubTableArray, unsigned long uloffsetEBDT)
	{
		int iseek = 0;
		unsigned long ulCurrent = ftell(m_pFontFile);
		SubTableArray.IndexSubTb.Header.imageDataOffset = ulCurrent - uloffsetEBDT;
		
		unsigned long ulTmp = 0, GlyphOffset = 0;
		for (int i = SubTableArray.firstGlyphIndex; i <= SubTableArray.lastGlyphIndex; i++) {
			CBDTFormat19& Foramt19 = SubTableArray.IndexSubTb.vtDdataForm19[iseek];
			ulTmp = hy_cdr_int32_to(Foramt19.dataLen);
			fwrite(&ulTmp, 4, 1, m_pFontFile);
			for (int y = 0; y < Foramt19.dataLen; y++) {
				fwrite(&Foramt19.data[y], 1, 1, m_pFontFile);
			}
		}

	}	// end of void CHYFontCodec::EncodeEBDTFormat19()
#endif 

	int	CHYFontCodec::Decodehead()
	{
		m_HYhead.SetDefault();
		if (m_pFontFile)
		{		
			int iEntryIndex = m_HYTbDirectory.FindTableEntry(HEAD_TAG);
			if (iEntryIndex == -1) return FONT_ERR_HEAD_DECODE;

			CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
			fseek(m_pFontFile,tbEntry.offset,SEEK_SET);

			//Table version number
			fread(&m_HYhead.version.value,2,1,m_pFontFile);
			m_HYhead.version.value = hy_cdr_int16_to(m_HYhead.version.value);
			fread(&m_HYhead.version.fract,2,1,m_pFontFile);
			m_HYhead.version.fract = hy_cdr_int16_to(m_HYhead.version.fract);
			//fontRevision
			fread(&m_HYhead.fontRevision.value,2,1,m_pFontFile);
			m_HYhead.fontRevision.value = hy_cdr_int16_to(m_HYhead.fontRevision.value);		
			fread(&m_HYhead.fontRevision.fract,2,1,m_pFontFile);
			m_HYhead.fontRevision.fract = hy_cdr_int16_to(m_HYhead.fontRevision.fract);
			// checkSumAdjustment
			fread(&m_HYhead.checkSumAdjustment,4,1,m_pFontFile);
			m_HYhead.checkSumAdjustment = hy_cdr_int32_to(m_HYhead.checkSumAdjustment);
			// magicNumber
			fread(&m_HYhead.magicNumber,4,1,m_pFontFile);
			m_HYhead.magicNumber = hy_cdr_int32_to(m_HYhead.magicNumber);
			//flags
			fread(&m_HYhead.flags,2,1,m_pFontFile);
			m_HYhead.flags = hy_cdr_int16_to(m_HYhead.flags);
			//unitsPerEm
			fread(&m_HYhead.unitsPerEm,2,1,m_pFontFile);
			m_HYhead.unitsPerEm = hy_cdr_int16_to(m_HYhead.unitsPerEm);
			//created
			fread(m_HYhead.created,8,1,m_pFontFile);
			//modified
			fread(m_HYhead.modified,8,1,m_pFontFile);
			//xMin
			fread(&m_HYhead.xMin,2,1,m_pFontFile);
			m_HYhead.xMin = hy_cdr_int16_to(m_HYhead.xMin);
			// yMin
			fread(&m_HYhead.yMin,2,1,m_pFontFile);
			m_HYhead.yMin = hy_cdr_int16_to(m_HYhead.yMin);
			//xMax
			fread(&m_HYhead.xMax,2,1,m_pFontFile);
			m_HYhead.xMax = hy_cdr_int16_to(m_HYhead.xMax);
			// yMax
			fread(&m_HYhead.yMax,2,1,m_pFontFile);
			m_HYhead.yMax = hy_cdr_int16_to(m_HYhead.yMax);
			// macStyle
			fread(&m_HYhead.macStyle,2,1,m_pFontFile);
			m_HYhead.macStyle = hy_cdr_int16_to(m_HYhead.macStyle);
			//lowestRecPPEM
			fread(&m_HYhead.lowestRecPPEM,2,1,m_pFontFile);
			m_HYhead.lowestRecPPEM = hy_cdr_int16_to(m_HYhead.lowestRecPPEM);
			//fontDirectionHint
			fread(&m_HYhead.fontDirectionHint,2,1,m_pFontFile);
			m_HYhead.fontDirectionHint = hy_cdr_int16_to(m_HYhead.fontDirectionHint);
			//indexToLocFormat
			fread(&m_HYhead.indexToLocFormat,2,1,m_pFontFile);
			m_HYhead.indexToLocFormat = hy_cdr_int16_to(m_HYhead.indexToLocFormat);
			//glyphDataFormat
			fread(&m_HYhead.glyphDataFormat,2,1,m_pFontFile);
			m_HYhead.glyphDataFormat = hy_cdr_int16_to(m_HYhead.glyphDataFormat);
		}

		return HY_NOERROR;

	}	// end of int CHYFontCodec::Decodehead()

	void CHYFontCodec::Dumphead(CMarkup& MK)
	{
	
	
	}	// end of void CHYFontCodec::Dumphead()

	int CHYFontCodec::Encodehead()
	{
		if (m_pFontFile){	
			unsigned short	usTmp = 0;
			unsigned long	ulTmp = 0;

			int iEntryIndex = m_HYTbDirectory.FindTableEntry(HEAD_TAG);
			if (iEntryIndex == -1) return FONT_ERR_HEAD_ENCODE;

			CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
			tbEntry.offset = ftell(m_pFontFile);

			usTmp = hy_cdr_int16_to(m_HYhead.version.value);			
			fwrite(&usTmp,2,1,m_pFontFile);			
			usTmp = hy_cdr_int16_to(m_HYhead.version.fract);
			fwrite(&usTmp,2,1,m_pFontFile);

			//fontRevision			
			usTmp = hy_cdr_int16_to(m_HYhead.fontRevision.value);
			fwrite(&usTmp,2,1,m_pFontFile);			
			usTmp = hy_cdr_int16_to(m_HYhead.fontRevision.fract);
			fwrite(&usTmp,2,1,m_pFontFile);
			// checkSumAdjustment			
			ulTmp = 0;
			fwrite(&ulTmp,4,1,m_pFontFile);
			// magicNumber 标准默认值0x5F0F3CF5			
			ulTmp = hy_cdr_int32_to(m_HYhead.magicNumber);
			fwrite(&ulTmp,4,1,m_pFontFile);
			//flags
			usTmp = hy_cdr_int16_to(m_HYhead.flags);
			fwrite(&usTmp,2,1,m_pFontFile);			
			//unitsPerEm
			usTmp = hy_cdr_int16_to(m_HYhead.unitsPerEm);
			fwrite(&usTmp,2,1,m_pFontFile);			
			//created
			fwrite(m_HYhead.created,8,1,m_pFontFile);
			//modified
			fwrite(m_HYhead.modified,8,1,m_pFontFile);
			//xMin
			usTmp = hy_cdr_int16_to(m_HYhead.xMin);
			fwrite(&usTmp,2,1,m_pFontFile);			
			//yMin
			usTmp = hy_cdr_int16_to(m_HYhead.yMin);
			fwrite(&usTmp,2,1,m_pFontFile);			
			//xMax
			usTmp = hy_cdr_int16_to(m_HYhead.xMax);
			fwrite(&usTmp,2,1,m_pFontFile);
			// yMax
			usTmp = hy_cdr_int16_to(m_HYhead.yMax);
			fwrite(&usTmp,2,1,m_pFontFile);
			// macStyle
			usTmp = hy_cdr_int16_to(m_HYhead.macStyle);
			fwrite(&usTmp,2,1,m_pFontFile);			
			//lowestRecPPEM
			usTmp = hy_cdr_int16_to(m_HYhead.lowestRecPPEM);
			fwrite(&usTmp,2,1,m_pFontFile);			
			//fontDirectionHint
			usTmp = hy_cdr_int16_to(m_HYhead.fontDirectionHint);
			fwrite(&usTmp,2,1,m_pFontFile);			
			//indexToLocFormat			
			usTmp = hy_cdr_int16_to(m_HYhead.indexToLocFormat);
			fwrite(&usTmp,2,1,m_pFontFile);			
			//glyphDataFormat
			m_HYhead.glyphDataFormat = 0;
			usTmp = hy_cdr_int16_to(m_HYhead.glyphDataFormat);
			fwrite(&usTmp,2,1,m_pFontFile);

			tbEntry.length = ftell(m_pFontFile) - tbEntry.offset;
			int iTail = 4-tbEntry.length%4;
			if (tbEntry.length%4>0){
				char c = 0;
				for (int i=0; i<iTail; i++){
					fwrite(&c,1,1,m_pFontFile);
				}				
			}
		}
		
		return HY_NOERROR;

	}	// end of int CHYFontCodec::Encodehead()

	int CHYFontCodec::Decodehhea()
	{
		m_HYHhea.SetDefault();
		if(m_pFontFile)
		{	
			// 解析HHEA表
			int iEntryIndex = m_HYTbDirectory.FindTableEntry(HHEA_TAG);
			if (iEntryIndex == -1) return FONT_ERR_HHEA_DECODE;

			CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
			fseek(m_pFontFile,tbEntry.offset,SEEK_SET);
			// version 
			fread(&m_HYHhea.version.value,2,1,m_pFontFile);
			m_HYHhea.version.value = hy_cdr_int16_to(m_HYHhea.version.value);
			fread(&m_HYHhea.version.fract,2,1,m_pFontFile);
			m_HYHhea.version.fract = hy_cdr_int16_to(m_HYHhea.version.fract);
			// Typographic ascent
			fread(&m_HYHhea.Ascender,2,1,m_pFontFile);
			m_HYHhea.Ascender = hy_cdr_int16_to(m_HYHhea.Ascender);
			// Typographic descent
			fread(&m_HYHhea.Descender,2,1,m_pFontFile);
			m_HYHhea.Descender = hy_cdr_int16_to(m_HYHhea.Descender);
			// Typographic line gap
			fread(&m_HYHhea.LineGap,2,1,m_pFontFile);
			m_HYHhea.LineGap = hy_cdr_int16_to(m_HYHhea.LineGap);
			// Maximum advance width value in hmtx table
			fread(&m_HYHhea.advanceWidthMax,2,1,m_pFontFile);
			m_HYHhea.advanceWidthMax = hy_cdr_int16_to(m_HYHhea.advanceWidthMax);
			// Minimum left sidebearing value
			fread(&m_HYHhea.minLeftSideBearing,2,1,m_pFontFile);
			m_HYHhea.minLeftSideBearing = hy_cdr_int16_to(m_HYHhea.minLeftSideBearing);
			// Minimum right sidebearing value
			fread(&m_HYHhea.minRightSideBearing,2,1,m_pFontFile);
			m_HYHhea.minRightSideBearing = hy_cdr_int16_to(m_HYHhea.minRightSideBearing);		
			// Max(lsb + (xMax - xMin)).
			fread(&m_HYHhea.xMaxExtent,2,1,m_pFontFile);
			m_HYHhea.xMaxExtent = hy_cdr_int16_to(m_HYHhea.xMaxExtent);
			// Used to calculate the slope of the cursor
			fread(&m_HYHhea.caretSlopeRise,2,1,m_pFontFile);
			m_HYHhea.caretSlopeRise = hy_cdr_int16_to(m_HYHhea.caretSlopeRise);	
			// caretSlopeRun
			fread(&m_HYHhea.caretSlopeRun,2,1,m_pFontFile);
			m_HYHhea.caretSlopeRun = hy_cdr_int16_to(m_HYHhea.caretSlopeRun);
			// caretOffset
			fread(&m_HYHhea.caretOffset,2,1,m_pFontFile);
			m_HYHhea.caretOffset = hy_cdr_int16_to(m_HYHhea.caretOffset);
			// reserved1
			fread(&m_HYHhea.reserved1,2,1,m_pFontFile);
			m_HYHhea.reserved1 = hy_cdr_int16_to(m_HYHhea.reserved1);
			// reserved2
			fread(&m_HYHhea.reserved2,2,1,m_pFontFile);
			m_HYHhea.reserved2 = hy_cdr_int16_to(m_HYHhea.reserved2);
			// reserved3
			fread(&m_HYHhea.reserved3,2,1,m_pFontFile);
			m_HYHhea.reserved3 = hy_cdr_int16_to(m_HYHhea.reserved3);
			// reserved4
			fread(&m_HYHhea.reserved4,2,1,m_pFontFile);
			m_HYHhea.reserved4 = hy_cdr_int16_to(m_HYHhea.reserved4);
			// metricDataFormat
			fread(&m_HYHhea.metricDataFormat,2,1,m_pFontFile);
			m_HYHhea.metricDataFormat = hy_cdr_int16_to(m_HYHhea.metricDataFormat);
			// Number of hMetric entries in  hmtx table; may be smaller than the total number of glyphs in the font.
			fread(&m_HYHhea.numberOfHMetrics,2,1,m_pFontFile);
			m_HYHhea.numberOfHMetrics = hy_cdr_int16_to(m_HYHhea.numberOfHMetrics);								
		}

		return HY_NOERROR;

	}	// end of int CHYFontCodec::Decodehhea()

	int	CHYFontCodec::Encodehhea()
	{
		if (m_pFontFile){
			unsigned short	usTmp = 0;
			unsigned long	ulTmp = 0;

			int iEntryIndex = m_HYTbDirectory.FindTableEntry(HHEA_TAG);
			if (iEntryIndex == -1) return FONT_ERR_HHEA_ENCODE;

			CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
			tbEntry.offset = ftell(m_pFontFile);

			// version 			
			usTmp = hy_cdr_int16_to(m_HYHhea.version.value);
			fwrite(&usTmp,2,1,m_pFontFile);			
			usTmp = hy_cdr_int16_to(m_HYHhea.version.fract);
			fwrite(&usTmp,2,1,m_pFontFile);			
			//Ascender
			usTmp = hy_cdr_int16_to(m_HYHhea.Ascender);
			fwrite(&usTmp,2,1,m_pFontFile);
			//Descender
			usTmp = hy_cdr_int16_to(m_HYHhea.Descender);
			fwrite(&usTmp,2,1,m_pFontFile);
			//LineGap			
			usTmp = hy_cdr_int16_to(m_HYHhea.LineGap);
			fwrite(&usTmp,2,1,m_pFontFile);
			//advanceWidthMax			
			usTmp =hy_cdr_int16_to(m_HYHhea.advanceWidthMax);
			fwrite(&usTmp,2,1,m_pFontFile);
			//minLeftSideBearing			
			usTmp =hy_cdr_int16_to(m_HYHhea.minLeftSideBearing);
			fwrite(&usTmp,2,1,m_pFontFile);
			//minRightSideBearing
			usTmp =hy_cdr_int16_to(m_HYHhea.minRightSideBearing);
			fwrite(&usTmp,2,1,m_pFontFile);
			//xMaxExtent			
			usTmp =hy_cdr_int16_to(m_HYHhea.xMaxExtent);
			fwrite(&usTmp,2,1,m_pFontFile);
			//caretSlopeRise		
			usTmp = hy_cdr_int16_to(1);
			fwrite(&usTmp,2,1,m_pFontFile);
			//caretSlopeRun
			usTmp = 0;
			fwrite(&usTmp,2,1,m_pFontFile);
			//caretOffset
			usTmp = 0;
			fwrite(&usTmp,2,1,m_pFontFile);
			usTmp = 0;
			//reserved1			
			fwrite(&usTmp,2,1,m_pFontFile);
			//reserved2			
			fwrite(&usTmp,2,1,m_pFontFile);
			//reserved3			
			fwrite(&usTmp,2,1,m_pFontFile);
			//reserved4			
			fwrite(&usTmp,2,1,m_pFontFile);
			//metricDataFormat
			usTmp = 0;
			fwrite(&usTmp,2,1,m_pFontFile);
			//numberOfHMetrics			
			usTmp = hy_cdr_int16_to(m_HYHhea.numberOfHMetrics);
			fwrite(&usTmp,2,1,m_pFontFile);

			tbEntry.length = ftell(m_pFontFile) - tbEntry.offset;
			int iTail = 4-tbEntry.length%4;
			if (tbEntry.length%4>0)	{
				char c = 0;
				for (int i=0; i<iTail; i++){
					fwrite(&c,1,1,m_pFontFile);
				}				
			}
		}

		return FONT_ERR_HHEA_ENCODE;

	}	// end of int	CHYFontCodec::Encodehhea()

	int CHYFontCodec::Decodehmtx()
	{
		m_HYHmtx.SetDefault();

		if (m_pFontFile)
		{
			int iEntryIndex = m_HYTbDirectory.FindTableEntry(HMTX_TAG);
			if (iEntryIndex == -1) return FONT_ERR_HMTX_DECODE;

			CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
			fseek(m_pFontFile,tbEntry.offset,SEEK_SET);

			unsigned short uHmetricsNum = m_HYHhea.numberOfHMetrics;
			for (unsigned short i=0; i<uHmetricsNum; i++)
			{
				HMTX_LONGHORMERTRIC  Longhormetric;

				fread(&Longhormetric.advanceWidth,2,1,m_pFontFile);
				Longhormetric.advanceWidth = hy_cdr_int16_to(Longhormetric.advanceWidth);

				fread(&Longhormetric.lsb,2,1,m_pFontFile);
				Longhormetric.lsb = hy_cdr_int16_to(Longhormetric.lsb);

				m_HYHmtx.vtLonghormetric.push_back(Longhormetric);
			}

			unsigned short  uLeftSideBearingNum = m_HYMaxp.numGlyphs-uHmetricsNum;
			short sleftSideBearing = 0;
			for (unsigned short  i=0; i<uLeftSideBearingNum; i++)
			{
				fread(&sleftSideBearing,2,1,m_pFontFile);
				sleftSideBearing = hy_cdr_int16_to(sleftSideBearing);
				m_HYHmtx.vtLeftsidebearing.push_back(sleftSideBearing);	
			}		
		}

		return HY_NOERROR;

	}	// end of int CHYFontCodec::Decodehmtx()

	int CHYFontCodec::Encodehmtx()
	{
		if (m_pFontFile)
		{
			unsigned short	usTmp = 0;
			unsigned long	ulTmp = 0;

			int iEntryIndex = m_HYTbDirectory.FindTableEntry(HMTX_TAG);
			if (iEntryIndex == -1) return FONT_ERR_HMTX_ENCODE;

			CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
			tbEntry.offset = ftell(m_pFontFile);
			
			size_t			szGlyphNum			= m_vtHYGlyphs.size();
			unsigned short longhormetricNums	= m_HYHhea.numberOfHMetrics;
			unsigned short lefsidebearNums		= szGlyphNum - m_HYHhea.numberOfHMetrics;

			for (size_t i=0; i<szGlyphNum; i++)
			{					
				if (i<longhormetricNums)
				{
					usTmp = hy_cdr_int16_to(m_vtHYGlyphs[i].advanceWidth);
					fwrite(&usTmp,2,1,m_pFontFile);

					usTmp = hy_cdr_int16_to(m_vtHYGlyphs[i].minX);
					fwrite(&usTmp,2,1,m_pFontFile);					
				}
				else 
				{

					usTmp = hy_cdr_int16_to(m_vtHYGlyphs[i].minX);
					fwrite(&usTmp,2,1,m_pFontFile);
				}
			}

			tbEntry.length = ftell(m_pFontFile) - tbEntry.offset;
			int iTail = 4-tbEntry.length%4;
			if (tbEntry.length%4>0)
			{
				char c = 0;
				for (int i=0; i<iTail; i++)
				{
					fwrite(&c,1,1,m_pFontFile);
				}				
			}
		}
	
		return HY_NOERROR;

	}	// end of int CHYFontCodec::Encodehmtx()

	int CHYFontCodec::Decodemaxp()
	{
		m_HYMaxp.SetDefault();
		if(m_pFontFile)
		{
			int iEntryIndex = m_HYTbDirectory.FindTableEntry(MAXP_TAG);
			if (iEntryIndex == -1) return FONT_ERR_MAXP_DECODE;		
			CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
			fseek(m_pFontFile,tbEntry.offset,SEEK_SET);

			if (m_iFontType == FONTTYPE_TTF)
			{
				fread(&m_HYMaxp.version.value,2,1,m_pFontFile);
				m_HYMaxp.version.value = hy_cdr_int16_to(m_HYMaxp.version.value);

				fread(&m_HYMaxp.version.fract,2,1,m_pFontFile);
				m_HYMaxp.version.fract = hy_cdr_int16_to(m_HYMaxp.version.fract);

				fread(&m_HYMaxp.numGlyphs,2,1,m_pFontFile);
				m_HYMaxp.numGlyphs = hy_cdr_int16_to(m_HYMaxp.numGlyphs);

				fread(&m_HYMaxp.maxPoints,2,1,m_pFontFile);
				m_HYMaxp.maxPoints = hy_cdr_int16_to(m_HYMaxp.maxPoints);

				fread(&m_HYMaxp.maxContours,2,1,m_pFontFile);
				m_HYMaxp.maxContours = hy_cdr_int16_to(m_HYMaxp.maxContours);

				fread(&m_HYMaxp.maxCompositePoints,2,1,m_pFontFile);
				m_HYMaxp.maxCompositePoints = hy_cdr_int16_to(m_HYMaxp.maxCompositePoints);

				fread(&m_HYMaxp.maxCompositeContours,2,1,m_pFontFile);
				m_HYMaxp.maxCompositeContours = hy_cdr_int16_to(m_HYMaxp.maxCompositeContours);

				fread(&m_HYMaxp.maxZones,2,1,m_pFontFile);
				m_HYMaxp.maxZones = hy_cdr_int16_to(m_HYMaxp.maxZones);

				fread(&m_HYMaxp.maxTwilightPoints,2,1,m_pFontFile);
				m_HYMaxp.maxTwilightPoints = hy_cdr_int16_to(m_HYMaxp.maxTwilightPoints);

				fread(&m_HYMaxp.maxStorage,2,1,m_pFontFile);
				m_HYMaxp.maxStorage = hy_cdr_int16_to(m_HYMaxp.maxStorage);		

				fread(&m_HYMaxp.maxFunctionDefs,2,1,m_pFontFile);
				m_HYMaxp.maxFunctionDefs = hy_cdr_int16_to(m_HYMaxp.maxFunctionDefs);

				fread(&m_HYMaxp.maxInstructionDefs,2,1,m_pFontFile);
				m_HYMaxp.maxInstructionDefs = hy_cdr_int16_to(m_HYMaxp.maxInstructionDefs);

				fread(&m_HYMaxp.maxStackElements,2,1,m_pFontFile);
				m_HYMaxp.maxStackElements = hy_cdr_int16_to(m_HYMaxp.maxStackElements);		

				fread(&m_HYMaxp.maxSizeOfInstructions,2,1,m_pFontFile);
				m_HYMaxp.maxSizeOfInstructions = hy_cdr_int16_to(m_HYMaxp.maxSizeOfInstructions);		

				fread(&m_HYMaxp.maxComponentElements,2,1,m_pFontFile);
				m_HYMaxp.maxComponentElements = hy_cdr_int16_to(m_HYMaxp.maxComponentElements);	

				fread(&m_HYMaxp.maxComponentDepth,2,1,m_pFontFile);
				m_HYMaxp.maxComponentDepth = hy_cdr_int16_to(m_HYMaxp.maxComponentDepth);
			}

			if (m_iFontType == FONTTYPE_OTF)
			{
				fread(&m_HYMaxp.version.value,2,1,m_pFontFile);
				m_HYMaxp.version.value = hy_cdr_int16_to(m_HYMaxp.version.value);

				fread(&m_HYMaxp.version.fract,2,1,m_pFontFile);
				m_HYMaxp.version.fract = hy_cdr_int16_to(m_HYMaxp.version.fract);

				fread(&m_HYMaxp.numGlyphs,2,1,m_pFontFile);
				m_HYMaxp.numGlyphs = hy_cdr_int16_to(m_HYMaxp.numGlyphs);
			}
			
		}

		return HY_NOERROR;

	}	// end of int CHYFontCodec::Decodemaxp()

	void CHYFontCodec::Dumpmaxp(CMarkup &Mk)
	{

	}	// end of void CHYFontCodec::Dumpmaxp()

	int	CHYFontCodec::Encodemaxp()
	{
		if (m_pFontFile){
			unsigned short	usTmp = 0;
			unsigned long	ulTmp = 0;

			int iEntryIndex = m_HYTbDirectory.FindTableEntry(MAXP_TAG);
			if (iEntryIndex == -1) return FONT_ERR_HMTX_ENCODE;

			CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
			tbEntry.offset = ftell(m_pFontFile);

			if (m_iFontType == FONTTYPE_TTF) {
				//Table version number	
				m_HYMaxp.version.value = 1;
				usTmp = hy_cdr_int16_to(m_HYMaxp.version.value);
				fwrite(&usTmp, 2, 1, m_pFontFile);
				m_HYMaxp.version.fract = 0;
				usTmp = hy_cdr_int16_to(m_HYMaxp.version.fract);
				fwrite(&m_HYMaxp.version.fract, 2, 1, m_pFontFile);
				//numGlyphs
				usTmp = hy_cdr_int16_to(m_HYMaxp.numGlyphs);
				fwrite(&usTmp, 2, 1, m_pFontFile);
			}

			if (m_iFontType == FONTTYPE_OTF)
			{
				//Table version number	
				m_HYMaxp.version.value = 0;
				usTmp = hy_cdr_int16_to(m_HYMaxp.version.value);
				fwrite(&usTmp, 2, 1, m_pFontFile);
				m_HYMaxp.version.fract = 0x5000;
				usTmp = hy_cdr_int16_to(m_HYMaxp.version.fract);
				fwrite(&usTmp, 2, 1, m_pFontFile);
				//numGlyphs
				usTmp = hy_cdr_int16_to(m_HYMaxp.numGlyphs);
				fwrite(&usTmp, 2, 1, m_pFontFile);
			}

			if (m_iFontType == FONTTYPE_TTF){								
				usTmp = hy_cdr_int16_to(m_HYMaxp.maxPoints);
				fwrite(&usTmp,2,1,m_pFontFile);
				//maxContours				
				usTmp = hy_cdr_int16_to(m_HYMaxp.maxContours);
				fwrite(&usTmp,2,1,m_pFontFile);
				//maxCompositePoints				
				usTmp = hy_cdr_int16_to(m_HYMaxp.maxCompositePoints);
				fwrite(&usTmp,2,1,m_pFontFile);
				//maxCompositeContours
				usTmp = hy_cdr_int16_to(m_HYMaxp.maxCompositeContours);
				fwrite(&usTmp,2,1,m_pFontFile);
				//maxZones
				usTmp = hy_cdr_int16_to(m_HYMaxp.maxZones);
				fwrite(&usTmp,2,1,m_pFontFile);
				//maxTwilightPoints
				usTmp = hy_cdr_int16_to(m_HYMaxp.maxTwilightPoints);
				fwrite(&usTmp,2,1,m_pFontFile);
				//maxStorage
				usTmp = hy_cdr_int16_to(m_HYMaxp.maxStorage);
				fwrite(&usTmp,2,1,m_pFontFile);
				//maxFunctionDefs
				usTmp = hy_cdr_int16_to(m_HYMaxp.maxFunctionDefs);
				fwrite(&usTmp,2,1,m_pFontFile);
				//maxInstructionDefs
				usTmp = hy_cdr_int16_to(m_HYMaxp.maxInstructionDefs);
				fwrite(&usTmp,2,1,m_pFontFile);
				//maxStackElements
				usTmp = hy_cdr_int16_to(m_HYMaxp.maxStackElements);
				fwrite(&usTmp,2,1,m_pFontFile);
				//maxSizeOfInstructions
				usTmp = hy_cdr_int16_to(m_HYMaxp.maxSizeOfInstructions);
				fwrite(&usTmp,2,1,m_pFontFile);
				//maxComponentElements				
				usTmp = hy_cdr_int16_to(m_HYMaxp.maxComponentElements);
				fwrite(&usTmp,2,1,m_pFontFile);
				//maxComponentDepth
				usTmp = hy_cdr_int16_to(m_HYMaxp.maxComponentDepth);
				fwrite(&usTmp,2,1,m_pFontFile);			
			}

			tbEntry.length = ftell(m_pFontFile) - tbEntry.offset;
			int iTail = 4-tbEntry.length%4;
			if (tbEntry.length%4>0){
				char c = 0;
				for (int i=0; i<iTail; i++){
					fwrite(&c,1,1,m_pFontFile);
				}				
			}	
		}

		return HY_NOERROR;

	}	// end of int CHYFontCodec::Encodemaxp()

	int	CHYFontCodec::Decodename()
	{
		m_HYName.SetDefault();
		if (m_pFontFile)
		{
			int iEntryIndex = m_HYTbDirectory.FindTableEntry(NAME_TAG);
			if (iEntryIndex == -1) return FONT_ERR_NAME_DECODE;

			CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
			fseek(m_pFontFile,tbEntry.offset,SEEK_SET);

			unsigned long TbStart = ftell(m_pFontFile);
			fread(&m_HYName.Format,2,1,m_pFontFile);
			m_HYName.Format = hy_cdr_int16_to(m_HYName.Format);
			fread(&m_HYName.count,2,1,m_pFontFile);
			m_HYName.count = hy_cdr_int16_to(m_HYName.count);
			fread(&m_HYName.offset,2,1,m_pFontFile);
			m_HYName.offset = hy_cdr_int16_to(m_HYName.offset);

			for (unsigned short i=0; i<m_HYName.count; i++)
			{
				NAMERECORD	NameRecord;

				fread(&NameRecord.platformID,2,1,m_pFontFile);
				NameRecord.platformID = hy_cdr_int16_to(NameRecord.platformID);
				fread(&NameRecord.encodingID,2,1,m_pFontFile);
				NameRecord.encodingID = hy_cdr_int16_to(NameRecord.encodingID);
				fread(&NameRecord.languageID,2,1,m_pFontFile);
				NameRecord.languageID = hy_cdr_int16_to(NameRecord.languageID);
				fread(&NameRecord.nameID,2,1,m_pFontFile);
				NameRecord.nameID = hy_cdr_int16_to(NameRecord.nameID);
				fread(&NameRecord.length,2,1,m_pFontFile);
				NameRecord.length = hy_cdr_int16_to(NameRecord.length);
				fread(&NameRecord.offset,2,1,m_pFontFile);
				NameRecord.offset = hy_cdr_int16_to(NameRecord.offset);
				int iCurrentPos = ftell(m_pFontFile);
				fseek(m_pFontFile,TbStart+m_HYName.offset+NameRecord.offset,SEEK_SET);

				if (NameRecord.platformID==3 &&NameRecord.encodingID == 1)
				{
					int pwContentLength = NameRecord.length/2+1;
					wchar_t*  pwContent = new wchar_t[pwContentLength];
					memset(pwContent,0,pwContentLength*2);
					unsigned short usTmp;
					for (int j=0; j<NameRecord.length/2; j++)
					{
						fread(&usTmp,2,1,m_pFontFile);
						usTmp = hy_cdr_int16_to(usTmp);
						memcpy(pwContent+j,&usTmp,2);
					}
					int nUsedDefaultChar = 0;				
					int nCharLen = WideCharToMultiByte(CP_ACP,0, pwContent, NameRecord.length/2+1, NULL, 0, NULL, &nUsedDefaultChar);
					if(nCharLen>0)
					{						
						char* pMultiByte = new char[nCharLen];
						ZeroMemory(pMultiByte, nCharLen);
						WideCharToMultiByte(CP_ACP,0, pwContent, NameRecord.length, pMultiByte, nCharLen, NULL, &nUsedDefaultChar);

						NameRecord.strContent = std::string(pMultiByte);

						if (pwContent) delete[] pwContent;
						if (pMultiByte) delete[] pMultiByte;
					}
				}
				else
				{
					char* pContent = new char[NameRecord.length+1];
					ZeroMemory(pContent, NameRecord.length+1);
					fread(pContent,NameRecord.length,1,m_pFontFile);
					NameRecord.strContent = pContent;
					if (pContent) delete[] pContent;
				}
				

				fseek(m_pFontFile,iCurrentPos,SEEK_SET);

				m_HYName.vtNameRecord.push_back(NameRecord);
			}

			if (m_HYName.Format == 1)
			{
				unsigned short langTagCount;
				fread(&langTagCount,2,1,m_pFontFile);
				langTagCount = hy_cdr_int16_to(langTagCount);

				for (unsigned short i=0; i<langTagCount; i++)
				{
					LANGTAGRECORD langTagRecord;

					fread(&langTagRecord.length,2,1,m_pFontFile);
					langTagRecord.length = hy_cdr_int16_to(langTagRecord.length);
					fread(&langTagRecord.offset,2,1,m_pFontFile);
					langTagRecord.offset = hy_cdr_int16_to(langTagRecord.offset);

					int iCurrentPos = ftell(m_pFontFile);
					fseek(m_pFontFile,TbStart+m_HYName.offset+langTagRecord.offset,SEEK_SET);
					fread(&langTagRecord.strContent,1,langTagRecord.length,m_pFontFile);
					fseek(m_pFontFile,iCurrentPos,SEEK_SET);

					m_HYName.vtLangTargeRecord.push_back(langTagRecord);
				}
			}
		}


		return HY_NOERROR;

	}	// end of int CHYFontCodec::Decodename()

	int	CHYFontCodec::Encodename()
	{
		if (m_pFontFile){
			unsigned short	usTmp = 0;
			unsigned long	ulTmp = 0;

			int iEntryIndex = m_HYTbDirectory.FindTableEntry(NAME_TAG);
			if (iEntryIndex == -1) return FONT_ERR_NAME_ENCODE;

			CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
			tbEntry.offset = ftell(m_pFontFile);

			//format
			usTmp = 0;
			fwrite(&usTmp,2,1,m_pFontFile);
			//count
			usTmp = hy_cdr_int16_to((unsigned short)m_HYName.vtNameRecord.size());
			fwrite(&usTmp,2,1,m_pFontFile);
			//stringOffset
			unsigned short usOffset = 6+12*(unsigned short)m_HYName.vtNameRecord.size();
			usTmp = hy_cdr_int16_to(usOffset);
			fwrite(&usTmp,2,1,m_pFontFile);
			//Name Records
			unsigned short usStringSize = 0;
			char* pString = new char[260*4*m_HYName.vtNameRecord.size()];
			
			for(size_t i=0; i<m_HYName.vtNameRecord.size();i++)
			{	
				NAMERECORD& tagNameRecord = m_HYName.vtNameRecord[i];

				if (
					(tagNameRecord.platformID==3&&tagNameRecord.encodingID ==1)||
					(tagNameRecord.platformID==0&&tagNameRecord.encodingID ==3)||
					(tagNameRecord.platformID==0&&tagNameRecord.encodingID ==4)||
					(tagNameRecord.platformID==0&&tagNameRecord.encodingID ==6)						
					)
				{						
					char strRecord[1040] = {0};
					int strLen = 1040;

					MulToWCharBL(strRecord, strLen, tagNameRecord.strContent.c_str(), strlen(tagNameRecord.strContent.c_str()));
					tagNameRecord.length = strLen;

					memcpy(pString+usStringSize,strRecord,strLen);
				}
				else 
				{
					tagNameRecord.length = strlen(tagNameRecord.strContent.c_str());
					memcpy(pString+usStringSize,tagNameRecord.strContent.c_str(),tagNameRecord.length);
				}
				usStringSize+=tagNameRecord.length;
			}
			
			size_t stOffset = 0;
			for(size_t i=0; i<m_HYName.vtNameRecord.size();i++)
			{	
				NAMERECORD& tagNameRecord = m_HYName.vtNameRecord[i];

				//platformID
				usTmp = hy_cdr_int16_to(tagNameRecord.platformID);
				fwrite(&usTmp,2,1,m_pFontFile);

				//encodingID
				usTmp = hy_cdr_int16_to(tagNameRecord.encodingID);
				fwrite(&usTmp,2,1,m_pFontFile);

				//languageID
				usTmp = hy_cdr_int16_to(tagNameRecord.languageID);
				fwrite(&usTmp,2,1,m_pFontFile);

				//nameID
				usTmp = hy_cdr_int16_to(tagNameRecord.nameID);
				fwrite(&usTmp,2,1,m_pFontFile);

				//length				
				usTmp = hy_cdr_int16_to(tagNameRecord.length);
				fwrite(&usTmp,2,1,m_pFontFile);

				//offset				
				usTmp = hy_cdr_int16_to(stOffset);				
				fwrite(&usTmp,2,1,m_pFontFile);
				stOffset += tagNameRecord.length;
			}		
			
			fwrite(pString,usStringSize,1,m_pFontFile);
			if (pString)
				delete[] pString;		

			tbEntry.length = ftell(m_pFontFile) - tbEntry.offset;
			int iTail = 4-tbEntry.length%4;
			if (tbEntry.length%4>0)
			{
				char c = 0;
				for (int i=0; i<iTail; i++)
				{
					fwrite(&c,1,1,m_pFontFile);
				}				
			}
		}

		return HY_NOERROR;

	}	// end of int CHYFontCodec::Encodename()

	int CHYFontCodec::DecodeOS2()
	{
		m_HYOS2.SetDefault();
		if (m_pFontFile)
		{	
			int iEntryIndex = m_HYTbDirectory.FindTableEntry(OS2_TAG);
			if (iEntryIndex == -1) return FONT_ERR_OS2_DECODE;

			CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
			fseek(m_pFontFile,tbEntry.offset,SEEK_SET);
			// version
			fread(&m_HYOS2.version,2,1,m_pFontFile);
			m_HYOS2.version = hy_cdr_int16_to(m_HYOS2.version);
			// Average weighted 
			fread(&m_HYOS2.xAvgCharWidth,2,1,m_pFontFile);
			m_HYOS2.xAvgCharWidth = hy_cdr_int16_to(m_HYOS2.xAvgCharWidth);
			// Indicates the visual weight (degree of blackness or thickness of strokes) of the characters in the font
			fread(&m_HYOS2.usWeightClass,2,1,m_pFontFile);
			m_HYOS2.usWeightClass = hy_cdr_int16_to(m_HYOS2.usWeightClass);
			// Indicates the visual weight (degree of blackness or thickness of strokes) of the characters in the font
			fread(&m_HYOS2.usWidthClass,2,1,m_pFontFile);
			m_HYOS2.usWidthClass = hy_cdr_int16_to(m_HYOS2.usWidthClass);		
			// Indicates font embedding licensing rights for the font. Embeddable fonts may be stored in a document
			fread(&m_HYOS2.fsType,2,1,m_pFontFile);
			m_HYOS2.fsType = hy_cdr_int16_to(m_HYOS2.fsType);
			// Subscript horizontal font size.
			fread(&m_HYOS2.ySubscriptXSize,2,1,m_pFontFile);
			m_HYOS2.ySubscriptXSize = hy_cdr_int16_to(m_HYOS2.ySubscriptXSize);
			// Subscript vertical font size..
			fread(&m_HYOS2.ySubscriptYSize,2,1,m_pFontFile);
			m_HYOS2.ySubscriptYSize = hy_cdr_int16_to(m_HYOS2.ySubscriptYSize);	
			//Subscript x offset
			fread(&m_HYOS2.ySubscriptXOffset,2,1,m_pFontFile);
			m_HYOS2.ySubscriptXOffset = hy_cdr_int16_to(m_HYOS2.ySubscriptXOffset);
			//Subscript y offset
			fread(&m_HYOS2.ySubscriptYOffset,2,1,m_pFontFile);
			m_HYOS2.ySubscriptYOffset = hy_cdr_int16_to(m_HYOS2.ySubscriptYOffset);
			//Subscript y offset
			fread(&m_HYOS2.ySuperscriptXSize,2,1,m_pFontFile);
			m_HYOS2.ySuperscriptXSize = hy_cdr_int16_to(m_HYOS2.ySuperscriptXSize);
			//Superscript vertical font size
			fread(&m_HYOS2.ySuperscriptYSize,2,1,m_pFontFile);
			m_HYOS2.ySuperscriptYSize = hy_cdr_int16_to(m_HYOS2.ySuperscriptYSize);
			//Superscript x offset
			fread(&m_HYOS2.ySuperscriptXOffset,2,1,m_pFontFile);
			m_HYOS2.ySuperscriptXOffset = hy_cdr_int16_to(m_HYOS2.ySuperscriptXOffset);
			//Superscript y offset
			fread(&m_HYOS2.ySuperscriptYOffset,2,1,m_pFontFile);
			m_HYOS2.ySuperscriptYOffset = hy_cdr_int16_to(m_HYOS2.ySuperscriptYOffset);
			//Strikeout size
			fread(&m_HYOS2.yStrikeoutSize,2,1,m_pFontFile);
			m_HYOS2.yStrikeoutSize = hy_cdr_int16_to(m_HYOS2.yStrikeoutSize);
			//Strikeout position.
			fread(&m_HYOS2.yStrikeoutPosition,2,1,m_pFontFile);
			m_HYOS2.yStrikeoutPosition = hy_cdr_int16_to(m_HYOS2.yStrikeoutPosition);		
			//Font-family class and subclass.  Also see section 3.4
			fread(&m_HYOS2.sFamilyClass,2,1,m_pFontFile);
			m_HYOS2.sFamilyClass = hy_cdr_int16_to(m_HYOS2.sFamilyClass);
			//PANOSE classification number		
			fread(&m_HYOS2.panose.FamilyType,1,1,m_pFontFile);
			fread(&m_HYOS2.panose.SerifStyle,1,1,m_pFontFile);
			fread(&m_HYOS2.panose.Weight,1,1,m_pFontFile);
			fread(&m_HYOS2.panose.Proportion,1,1,m_pFontFile);
			fread(&m_HYOS2.panose.Contrast,1,1,m_pFontFile);
			fread(&m_HYOS2.panose.StrokeVariation,1,1,m_pFontFile);
			fread(&m_HYOS2.panose.ArmStyle,1,1,m_pFontFile);
			fread(&m_HYOS2.panose.Letterform,1,1,m_pFontFile);
			fread(&m_HYOS2.panose.Midline,1,1,m_pFontFile);
			fread(&m_HYOS2.panose.XHeight,1,1,m_pFontFile);
			// ulUnicodeRange1
			fread(&m_HYOS2.ulUnicodeRange1,4,1,m_pFontFile);
			m_HYOS2.ulUnicodeRange1 = hy_cdr_int32_to(m_HYOS2.ulUnicodeRange1);
			// ulUnicodeRange2
			fread(&m_HYOS2.ulUnicodeRange2,4,1,m_pFontFile);
			m_HYOS2.ulUnicodeRange2 = hy_cdr_int32_to(m_HYOS2.ulUnicodeRange2);
			// ulUnicodeRange3
			fread(&m_HYOS2.ulUnicodeRange3,4,1,m_pFontFile);
			m_HYOS2.ulUnicodeRange3 = hy_cdr_int32_to(m_HYOS2.ulUnicodeRange3);
			// ulUnicodeRange4
			fread(&m_HYOS2.ulUnicodeRange4,4,1,m_pFontFile);
			m_HYOS2.ulUnicodeRange4 = hy_cdr_int32_to(m_HYOS2.ulUnicodeRange4);		
			// Font Vendor Identification
			char c;
			fread(&c,1,1,m_pFontFile);
			m_HYOS2.vtachVendID.push_back(c);
			fread(&c,1,1,m_pFontFile);
			m_HYOS2.vtachVendID.push_back(c);
			fread(&c,1,1,m_pFontFile);
			m_HYOS2.vtachVendID.push_back(c);
			fread(&c,1,1,m_pFontFile);
			m_HYOS2.vtachVendID.push_back(c);
			// Font selection flags
			fread(&m_HYOS2.fsSelection,2,1,m_pFontFile);
			m_HYOS2.fsSelection = hy_cdr_int16_to(m_HYOS2.fsSelection);
			// The minimum Unicode index (character code) in this font
			fread(&m_HYOS2.usFirstCharIndex,2,1,m_pFontFile);
			m_HYOS2.usFirstCharIndex = hy_cdr_int16_to(m_HYOS2.usFirstCharIndex);
			// The maximum Unicode index (character code) in this font,
			fread(&m_HYOS2.usLastCharIndex,2,1,m_pFontFile);
			m_HYOS2.usLastCharIndex = hy_cdr_int16_to(m_HYOS2.usLastCharIndex);		
			// The typographic ascender for this font. 
			fread(&m_HYOS2.sTypoAscender,2,1,m_pFontFile);
			m_HYOS2.sTypoAscender = hy_cdr_int16_to(m_HYOS2.sTypoAscender);
			// The typographic descender for this font. 
			fread(&m_HYOS2.sTypoDescender,2,1,m_pFontFile);
			m_HYOS2.sTypoDescender = hy_cdr_int16_to(m_HYOS2.sTypoDescender);
			// The typographic line gap for this font
			fread(&m_HYOS2.sTypoLineGap,2,1,m_pFontFile);
			m_HYOS2.sTypoLineGap = hy_cdr_int16_to(m_HYOS2.sTypoLineGap);
			// The ascender metric for Windows
			fread(&m_HYOS2.usWinAscent,2,1,m_pFontFile);
			m_HYOS2.usWinAscent = hy_cdr_int16_to(m_HYOS2.usWinAscent);	
			// The descender metric for Windows
			fread(&m_HYOS2.usWinDescent,2,1,m_pFontFile);
			m_HYOS2.usWinDescent = hy_cdr_int16_to(m_HYOS2.usWinDescent);

			if (m_HYOS2.version>0)
			{
				// ulCodePageRange1
				fread(&m_HYOS2.ulCodePageRange1,4,1,m_pFontFile);
				m_HYOS2.ulCodePageRange1 = hy_cdr_int32_to(m_HYOS2.ulCodePageRange1);
				// ulCodePageRange2
				fread(&m_HYOS2.ulCodePageRange2,4,1,m_pFontFile);
				m_HYOS2.ulCodePageRange2 = hy_cdr_int32_to(m_HYOS2.ulCodePageRange2);
			}		

			if(m_HYOS2.version>1)
			{
				// xHeight
				fread(&m_HYOS2.sxHeight,2,1,m_pFontFile);
				m_HYOS2.sxHeight = hy_cdr_int16_to(m_HYOS2.sxHeight);
				// sCapHeight
				fread(&m_HYOS2.sCapHeight,2,1,m_pFontFile);
				m_HYOS2.sCapHeight = hy_cdr_int16_to(m_HYOS2.sCapHeight);
				// usDefaultChar
				fread(&m_HYOS2.usDefaultChar,2,1,m_pFontFile);
				m_HYOS2.usDefaultChar = hy_cdr_int16_to(m_HYOS2.usDefaultChar);
				// usBreakChar
				fread(&m_HYOS2.usBreakChar,2,1,m_pFontFile);
				m_HYOS2.usBreakChar = hy_cdr_int16_to(m_HYOS2.usBreakChar);
				// usBreakChar
				fread(&m_HYOS2.usMaxContext,2,1,m_pFontFile);
				m_HYOS2.usMaxContext = hy_cdr_int16_to(m_HYOS2.usMaxContext);
			}
		}

		return HY_NOERROR;

	}	// end of int CHYFontCodec::DecodeOS2()  

	int	CHYFontCodec::EncodeOS2()
	{
		if (m_pFontFile){
			unsigned short	usTmp = 0;
			unsigned long	ulTmp = 0;

			int iEntryIndex = m_HYTbDirectory.FindTableEntry(OS2_TAG);
			if (iEntryIndex == -1) return FONT_ERR_OS2_ENCODE;

			CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
			tbEntry.offset = ftell(m_pFontFile);
			//version
			usTmp = hy_cdr_int16_to(m_HYOS2.version);
			fwrite(&usTmp,2,1,m_pFontFile);			
			//xAvgCharWidth			
			usTmp = hy_cdr_int16_to(m_HYOS2.xAvgCharWidth);
			fwrite(&usTmp,2,1,m_pFontFile);
			//usWeightClass
			usTmp = hy_cdr_int16_to(m_HYOS2.usWeightClass);			
			fwrite(&usTmp,2,1,m_pFontFile);
			//usWidthClass			
			usTmp = hy_cdr_int16_to(m_HYOS2.usWidthClass);
			fwrite(&usTmp,2,1,m_pFontFile);
			//fsType
			usTmp = hy_cdr_int16_to(m_HYOS2.fsType);
			fwrite(&usTmp,2,1,m_pFontFile);			
			//ySubscriptXSize
			usTmp = hy_cdr_int16_to(m_HYOS2.ySubscriptXSize);
			fwrite(&usTmp,2,1,m_pFontFile);
			//ySubscriptYSize
			usTmp = hy_cdr_int16_to(m_HYOS2.ySubscriptYSize);
			fwrite(&usTmp,2,1,m_pFontFile);
			//ySubscriptXOffset			
			usTmp = hy_cdr_int16_to(m_HYOS2.ySubscriptXOffset);
			fwrite(&usTmp,2,1,m_pFontFile);
			//ySubscriptYOffset			
			usTmp = hy_cdr_int16_to(m_HYOS2.ySubscriptYOffset);
			fwrite(&usTmp,2,1,m_pFontFile);
			//ySuperscriptXSize
			usTmp = hy_cdr_int16_to(m_HYOS2.ySuperscriptXSize);
			fwrite(&usTmp,2,1,m_pFontFile);
			//ySuperscriptYSize			
			usTmp = hy_cdr_int16_to(m_HYOS2.ySuperscriptYSize);
			fwrite(&usTmp,2,1,m_pFontFile);
			//ySuperscriptXOffset
			usTmp = hy_cdr_int16_to(m_HYOS2.ySuperscriptXOffset);
			fwrite(&usTmp,2,1,m_pFontFile);
			//ySuperscriptYOffset			
			usTmp = hy_cdr_int16_to(m_HYOS2.ySuperscriptYOffset);
			fwrite(&usTmp, 2, 1, m_pFontFile);			
			//yStrikeoutSize			
			usTmp = hy_cdr_int16_to(m_HYOS2.yStrikeoutSize);
			fwrite(&usTmp,2,1,m_pFontFile);
			//yStrikeoutPosition			
			usTmp = hy_cdr_int16_to(m_HYOS2.yStrikeoutPosition);
			fwrite(&usTmp,2,1,m_pFontFile);
			//sFamilyClass
			usTmp = hy_cdr_int16_to(m_HYOS2.sFamilyClass);
			fwrite(&usTmp,2,1,m_pFontFile);
			//Panose			
			fwrite(&m_HYOS2.panose.FamilyType,1,1,m_pFontFile);
			fwrite(&m_HYOS2.panose.SerifStyle,1,1,m_pFontFile);
			fwrite(&m_HYOS2.panose.Weight,1,1,m_pFontFile);
			fwrite(&m_HYOS2.panose.Proportion,1,1,m_pFontFile);
			fwrite(&m_HYOS2.panose.Contrast,1,1,m_pFontFile);
			fwrite(&m_HYOS2.panose.StrokeVariation,1,1,m_pFontFile);
			fwrite(&m_HYOS2.panose.ArmStyle,1,1,m_pFontFile);
			fwrite(&m_HYOS2.panose.Letterform,1,1,m_pFontFile);
			fwrite(&m_HYOS2.panose.Midline,1,1,m_pFontFile);
			fwrite(&m_HYOS2.panose.XHeight,1,1,m_pFontFile);
			//ulUnicodeRange1 
			ulTmp = hy_cdr_int32_to(m_HYOS2.ulUnicodeRange1);
			fwrite(&ulTmp,4,1,m_pFontFile);
			//ulUnicodeRange2 	
			ulTmp = hy_cdr_int32_to(m_HYOS2.ulUnicodeRange2);
			fwrite(&ulTmp,4,1,m_pFontFile);
			//ulUnicodeRange3 	
			ulTmp = hy_cdr_int32_to(m_HYOS2.ulUnicodeRange3);
			fwrite(&ulTmp,4,1,m_pFontFile);
			//ulUnicodeRange4
			ulTmp = hy_cdr_int32_to(m_HYOS2.ulUnicodeRange4);
			fwrite(&ulTmp,4,1,m_pFontFile);			
			//achVendID			
			fwrite(&m_HYOS2.vtachVendID[0], 1, 1, m_pFontFile);
			fwrite(&m_HYOS2.vtachVendID[1], 1, 1, m_pFontFile);
			fwrite(&m_HYOS2.vtachVendID[2], 1, 1, m_pFontFile);
			fwrite(&m_HYOS2.vtachVendID[3], 1, 1, m_pFontFile);		
						
			//fsSelection
			usTmp = hy_cdr_int16_to(m_HYOS2.fsSelection);
			fwrite(&usTmp,2,1,m_pFontFile);
			//usFirstCharIndex		
			usTmp = hy_cdr_int16_to(m_HYOS2.usFirstCharIndex);
			fwrite(&usTmp,2,1,m_pFontFile);
			//usLastCharIndex			
			usTmp = hy_cdr_int16_to(m_HYOS2.usLastCharIndex);
			fwrite(&usTmp,2,1,m_pFontFile);
			//sTypoAscender			
			usTmp = hy_cdr_int16_to(m_HYOS2.sTypoAscender);			
			fwrite(&usTmp,2,1,m_pFontFile);
			//sTypoDescender			
			usTmp = hy_cdr_int16_to(m_HYOS2.sTypoDescender);			
			fwrite(&usTmp,2,1,m_pFontFile);
			//sTypoLineGap			
			usTmp = hy_cdr_int16_to(m_HYOS2.sTypoLineGap);			
			fwrite(&usTmp,2,1,m_pFontFile);
			//usWinAscent
			usTmp = hy_cdr_int16_to(m_HYOS2.usWinAscent);			
			fwrite(&usTmp,2,1,m_pFontFile);
			//usWinDescent
			usTmp = hy_cdr_int16_to(m_HYOS2.usWinDescent);			
			fwrite(&usTmp,2,1,m_pFontFile);
			//ulCodePageRange1
			ulTmp = hy_cdr_int32_to(m_HYOS2.ulCodePageRange1);
			fwrite(&ulTmp,4,1,m_pFontFile);
			//ulCodePageRange2
			ulTmp = hy_cdr_int32_to(m_HYOS2.ulCodePageRange2);
			fwrite(&ulTmp,4,1,m_pFontFile);
			//sxHeight			
			usTmp = hy_cdr_int16_to(m_HYOS2.sxHeight);
			fwrite(&usTmp,2,1,m_pFontFile);
			//sCapHeight			
			usTmp = hy_cdr_int16_to(m_HYOS2.sCapHeight);
			fwrite(&usTmp,2,1,m_pFontFile);
			//usDefaultChar		
			usTmp = hy_cdr_int16_to(m_HYOS2.usDefaultChar);
			fwrite(&usTmp,2,1,m_pFontFile);
			//usBreakChar
			usTmp = hy_cdr_int16_to(m_HYOS2.usBreakChar);
			fwrite(&usTmp,2,1,m_pFontFile);
			//usMaxContext
			usTmp = hy_cdr_int16_to(m_HYOS2.usMaxContext);
			fwrite(&usTmp,2,1,m_pFontFile);

			tbEntry.length = ftell(m_pFontFile) - tbEntry.offset;
			int iTail = 4-tbEntry.length%4;
			if (tbEntry.length%4>0)	{
				char c = 0;
				for (int i=0; i<iTail; i++)	{
					fwrite(&c,1,1,m_pFontFile);
				}				
			}
		}

		return HY_NOERROR;

	}	// end of int CHYFontCodec::EncodeOS2()

	// vesion 2.5 在opentypeV1.3标准后已经被弃用，本程序目前暂不支持编解码。
	int CHYFontCodec::Decodepost()
	{
		m_HYPost.SetDefault();
		if (m_pFontFile){	
			int iEntryIndex = m_HYTbDirectory.FindTableEntry(POST_TAG);
			if (iEntryIndex == -1) return FONT_ERR_OS2_DECODE;
			
			CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
			fseek(m_pFontFile,tbEntry.offset,SEEK_SET);

			unsigned long ulTableLeng = tbEntry.length;
			long lBegPos = ftell(m_pFontFile);
			// Format 
			fread(&m_HYPost.Format.value,2,1,m_pFontFile);
			m_HYPost.Format.value = hy_cdr_int16_to(m_HYPost.Format.value);
			fread(&m_HYPost.Format.fract,2,1,m_pFontFile);
			m_HYPost.Format.fract = hy_cdr_int16_to(m_HYPost.Format.fract);
			//italicAngle
			fread(&m_HYPost.italicAngle.value,2,1,m_pFontFile);
			m_HYPost.italicAngle.value = hy_cdr_int16_to(m_HYPost.italicAngle.value);
			fread(&m_HYPost.italicAngle.fract,2,1,m_pFontFile);
			m_HYPost.italicAngle.fract = hy_cdr_int16_to(m_HYPost.italicAngle.fract);
			//underlinePosition
			fread(&m_HYPost.underlinePosition,2,1,m_pFontFile);
			m_HYPost.underlinePosition = hy_cdr_int16_to(m_HYPost.underlinePosition);
			//underlineThickness
			fread(&m_HYPost.underlineThickness,2,1,m_pFontFile);
			m_HYPost.underlineThickness = hy_cdr_int16_to(m_HYPost.underlineThickness);
			//isFixedPitch
			fread(&m_HYPost.isFixedPitch,4,1,m_pFontFile);
			m_HYPost.isFixedPitch = hy_cdr_int32_to(m_HYPost.isFixedPitch);
			//minMemType42
			fread(&m_HYPost.minMemType42,4,1,m_pFontFile);
			m_HYPost.minMemType42 = hy_cdr_int32_to(m_HYPost.minMemType42);
			//maxMemType42
			fread(&m_HYPost.maxMemType42,4,1,m_pFontFile);
			m_HYPost.maxMemType42 = hy_cdr_int32_to(m_HYPost.maxMemType42);
			//minMemType1
			fread(&m_HYPost.minMemType1,4,1,m_pFontFile);
			m_HYPost.minMemType1 = hy_cdr_int32_to(m_HYPost.minMemType1);
			//maxMemType1
			fread(&m_HYPost.maxMemType1,4,1,m_pFontFile);
			m_HYPost.maxMemType1 = hy_cdr_int32_to(m_HYPost.maxMemType1);

			// format 1  已经预建立			
			// format 2
			if (m_HYPost.Format.value == 2 &&m_HYPost.Format.fract == 0){
				CPostFormat2& pFormat2 = m_HYPost.m_Format2;
				fread(&pFormat2.usNumberOfGlyphs,2,1,m_pFontFile);
				pFormat2.usNumberOfGlyphs = hy_cdr_int16_to(pFormat2.usNumberOfGlyphs);

				unsigned short usTmp	= 0;
				char		cTmp		= 0;				
				for (int i=0; i<pFormat2.usNumberOfGlyphs; i++)	{
					fread(&usTmp,2,1,m_pFontFile);
					usTmp = hy_cdr_int16_to(usTmp);
					pFormat2.vtGlyphNameIndex.push_back(usTmp);
				}

				unsigned long lEndPos = lBegPos+ulTableLeng;
				unsigned long tmp = ftell(m_pFontFile);
				while (ftell(m_pFontFile)<lEndPos){
					fread(&cTmp,1,1,m_pFontFile);
					int iStringLen = cTmp;
					pFormat2.vtNameLength.push_back(cTmp);

					std::string  strTmp;
					for (int j=0; j<iStringLen; j++){
						fread(&cTmp,1,1,m_pFontFile);
						strTmp.push_back(cTmp);
					}
					m_HYPost.m_vtStandString.push_back(strTmp);							
				}
			}			
		}	

		return HY_NOERROR;

	}	// end of int CHYFontCodec::Decodepost()

	int	 CHYFontCodec::Encodepost(HYFIXED& fxVersion)
	{
		if (m_pFontFile){
			unsigned short	usTmp = 0;
			unsigned long	ulTmp = 0;

			int iEntryIndex = m_HYTbDirectory.FindTableEntry(POST_TAG);
			if (iEntryIndex == -1) return FONT_ERR_POST_ENCODE;

			CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
			tbEntry.offset = ftell(m_pFontFile);
			
			//Version
			usTmp = hy_cdr_int16_to(fxVersion.value);
			fwrite(&usTmp,2,1,m_pFontFile);
			usTmp = hy_cdr_int16_to(fxVersion.fract);
			fwrite(&usTmp,2,1,m_pFontFile);
			// italicAngle
			usTmp = hy_cdr_int16_to(m_HYPost.italicAngle.value);
			fwrite(&usTmp,2,1,m_pFontFile);
			usTmp = hy_cdr_int16_to(m_HYPost.italicAngle.fract);
			fwrite(&usTmp,2,1,m_pFontFile);
			//underlinePosition
			usTmp = hy_cdr_int16_to(m_HYPost.underlinePosition);
			fwrite(&usTmp,2,1,m_pFontFile);
			//UnderlineThickness
			usTmp = hy_cdr_int16_to(m_HYPost.underlineThickness);
			fwrite(&usTmp,2,1,m_pFontFile);
			//isFixedPitch
			ulTmp = hy_cdr_int32_to(m_HYPost.isFixedPitch);
			fwrite(&ulTmp,4,1,m_pFontFile);

			ulTmp = 0;
			//minMemType42
			fwrite(&ulTmp,4,1,m_pFontFile);
			//maxMemType42
			fwrite(&ulTmp,4,1,m_pFontFile);
			//minMemType1
			fwrite(&ulTmp,4,1,m_pFontFile);
			//maxMemType1
			fwrite(&ulTmp,4,1,m_pFontFile);

			if (fxVersion.value == 2 && fxVersion.fract == 0){
				// numberOfGlyphs
				usTmp = hy_cdr_int16_to(m_HYMaxp.numGlyphs);
				fwrite(&usTmp,2,1,m_pFontFile);
				//glyphNameIndex
				for (int i = 0; i <m_HYMaxp.numGlyphs; i++){
					unsigned short usPostNameID = m_HYPost.InsertName(m_vtHYGlyphs[i].strPostName);
					usTmp = hy_cdr_int16_to(usPostNameID);
					fwrite(&usTmp,2,1,m_pFontFile);
				}
				//names
				size_t st = m_HYPost.m_vtStandString.size();
				for (size_t i = 258; i <st; i++){
					const char* strData = m_HYPost.m_vtStandString[i].data();
					size_t t = strlen(strData);

					fwrite((byte*)&t,1,1,m_pFontFile);
					fwrite(strData,t,1,m_pFontFile);
				}
			}

			tbEntry.length = ftell(m_pFontFile) - tbEntry.offset;
			int iTail = 4-tbEntry.length%4;
			if (tbEntry.length%4>0)	{
				char c = 0;
				for (int i=0; i<iTail; i++)	{
					fwrite(&c,1,1,m_pFontFile);
				}				
			}
		}

		return HY_NOERROR;

	}	// end of int CHYFontCodec::Encodepost()

	int CHYFontCodec::Decodeprep()
	{
		m_HYPrep.vtPrep.clear();
		if (m_pFontFile)
		{
			int iEntryIndex = m_HYTbDirectory.FindTableEntry(PREP_TAG);
			if (iEntryIndex == -1) return FONT_ERR_PREP_DECODE;

			CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
			fseek(m_pFontFile,tbEntry.offset,SEEK_SET);

			unsigned char cTmp;
			for (unsigned long i=0; i<tbEntry.length; i++){
				fread(&cTmp,1,1,m_pFontFile);
				m_HYPrep.vtPrep.push_back(cTmp);
			}
		}

		return HY_NOERROR;

	}	// end of int CHYFontCodec::Decodeprep()

	int	CHYFontCodec::Encodeprep()
	{
		if (m_pFontFile)
		{
			unsigned long ulTableBegin = ftell(m_pFontFile);
			unsigned long ulGlyphOffset = 0;

			int iEntryIndex = m_HYTbDirectory.FindTableEntry(PREP_TAG);
			if (iEntryIndex ==-1)  return FONT_ERR_PREP_ENCODE;

			CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
			tbEntry.offset = ftell(m_pFontFile);

			unsigned char c=0xb8;
			fwrite(&c,1,1,m_pFontFile);
			c=0x01;
			fwrite(&c,1,1,m_pFontFile);
			c=0xff;
			fwrite(&c,1,1,m_pFontFile);
			c=0x85;
			fwrite(&c,1,1,m_pFontFile);
			c=0xb8;
			fwrite(&c,1,1,m_pFontFile);
			c=0x00;
			fwrite(&c,1,1,m_pFontFile);
			c=0x04;
			fwrite(&c,1,1,m_pFontFile);
			c=0x8d;
			fwrite(&c,1,1,m_pFontFile);

			tbEntry.length = ftell(m_pFontFile) - tbEntry.offset;
			int iTail = 4-tbEntry.length%4;
			if (tbEntry.length%4>0)
			{
				char c = 0;
				for (int i=0; i<iTail; i++)
				{
					fwrite(&c,1,1,m_pFontFile);
				}				
			}
		}

		return HY_NOERROR;

	}	// end of int CHYFontCodec::Encodeprep()

	int	CHYFontCodec::Decodecvt()
	{
		m_HYCvt.vtCvt.clear();
		if (m_pFontFile)
		{		
			int iEntryIndex = m_HYTbDirectory.FindTableEntry(CVT_TAG);
			if (iEntryIndex == -1) return FONT_ERR_CVT_DECODE;

			CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
			fseek(m_pFontFile,tbEntry.offset,SEEK_SET);

			short sTmp=0;
			int iCVTNum = tbEntry.length>>1;
			for (int i=0; i<iCVTNum; i++)
			{
				fread(&sTmp,2,1,m_pFontFile);
				sTmp = hy_cdr_int16_to(sTmp);
				m_HYCvt.vtCvt.push_back(sTmp);
			}
		}

		return HY_NOERROR;

	}	// end of int	CHYFontCodec::Decodecvt()

	int	CHYFontCodec::Decodefgpm()
	{
		m_HYFpgm.vtfgpm.clear();
		if (m_pFontFile)
		{
			int iEntryIndex = m_HYTbDirectory.FindTableEntry(FPGM_TAG);
			if (iEntryIndex == -1) return FONT_ERR_FPGM_DECODE;

			CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
			fseek(m_pFontFile,tbEntry.offset,SEEK_SET);

			unsigned char cTmp;
			for (unsigned long i=0; i<tbEntry.length; i++)
			{
				fread(&cTmp,1,1,m_pFontFile);
				m_HYFpgm.vtfgpm.push_back(cTmp);
			}
		}

		return HY_NOERROR;

	}	// end of int	CHYFontCodec::Decodefgpm()

	int	CHYFontCodec::Decodeglyf()
	{
		int iTableStartPoint = ftell(m_pFontFile);

		if (m_HYloca.vtLoca.size()==0)
			return FONT_ERR_GLYF_DECODE;

		unsigned long ulGlyphOffset = 0;
		short sContourNums=0;		
		for (unsigned short i=0; i <m_HYMaxp.numGlyphs; i++){			
			CHYGlyph& glyph = m_vtHYGlyphs[i];
			glyph.fontFlag = FONTTYPE_TTF;

			glyph.advanceWidth = GetGlyfAdvancWidth(i);
			VMTX_LONGVRTMETRIC vmtx;
			GetGlyfVrtmetric(i,vmtx);
			glyph.advanceHeight = vmtx.advanceHeight;
			glyph.topSB = vmtx.tsb;

			if (m_HYPost.FindNameIDByGID(i)!= -1) {
				glyph.strPostName = m_HYPost.FindNameByGID(i);
			}		
			else {
				char  buffer[64] = { 0 };
				_stprintf(buffer, _T("CID%d"), i);
				glyph.strPostName = buffer;
			}

			if(m_HYloca.vtLoca[i]==m_HYloca.vtLoca[i+1]) 
				continue;

			fseek(m_pFontFile,iTableStartPoint+m_HYloca.vtLoca[i],SEEK_SET);

			// contour numbers
			fread(&sContourNums,2,1,m_pFontFile);
			sContourNums = hy_cdr_int16_to(sContourNums);
			//Minimum x for coordinate data 
			fread(&glyph.minX,2,1,m_pFontFile);
			glyph.minX = hy_cdr_int16_to(glyph.minX);
			//Minimum y for coordinate data 
			fread(&glyph.minY,2,1,m_pFontFile);
			glyph.minY = hy_cdr_int16_to(glyph.minY);
			//Maximum x for coordinate data 
			fread(&glyph.maxX,2,1,m_pFontFile);
			glyph.maxX = hy_cdr_int16_to(glyph.maxX);
			//Maximum y for coordinate data 
			fread(&glyph.maxY,2,1,m_pFontFile);
			glyph.maxY = hy_cdr_int16_to(glyph.maxY);
			//simple glyph
			if (sContourNums>-1){			
				DecodeGLYF_SIMPLE(glyph, sContourNums);
			}
			else {					
				DecodeGLYF_COMPOS(glyph);
			}

			m_iStepNo = (i+1)*100/m_HYMaxp.numGlyphs;	
		}

		return HY_NOERROR;

	}	// end of int	CHYFontCodec::Decodeglyf()

	void CHYFontCodec::DecodeGLYF_SIMPLE(CHYGlyph& Glyph, unsigned short usContours)
	{	
		Glyph.glyfType = GLYF_TYPE_SIMPLE;

		unsigned short 	i = 0, nIns = 0, nPts = 0, nTmp=0;
		short			nCnts = usContours;
		std::vector<unsigned short> endptsofContours;

		//endPtsOfContours
		for (i=0; i<nCnts; i++){
			fread(&nTmp,2,1,m_pFontFile);
			nTmp = hy_cdr_int16_to(nTmp);	
			endptsofContours.push_back(nTmp);
		}

		// instructionLength
		fread(&nTmp,2,1,m_pFontFile);
		nTmp = hy_cdr_int16_to(nTmp);	
		unsigned char ctmp=0;
#if 0
		//instructions
		for (i=0;i<nTmp;i++)
		{
			fread(&ctmp,1,1,m_pFontFile);
			Glyph.vtInstruntions.push_back(ctmp);
		}
#else 

		fseek(m_pFontFile,nTmp,SEEK_CUR);
#endif 

		// 获取共有多少个点
		std::vector<unsigned char>	vtflags;
		std::vector<short>	xCoordinates;
		std::vector<short>	yCoordinates;

		if (nCnts!=0){
			nPts = endptsofContours[nCnts-1]+1;
		}

		unsigned char j, c;	
		// 获取标记	
		for (i=0;i<nPts;i++){	
			fread(&c,1,1,m_pFontFile);
			vtflags.push_back(c);		
			if (c&GLYF_SMPL_REPEAT){ 			
				fread(&j,1,1,m_pFontFile);
				while (j--){
					i++;				
					vtflags.push_back(c);
				}
			}
		}

		// 获取X坐标
		short stmp = 0;
		short Ptx=0, Pty=0;
		for (i=0; i<nPts;i++){
			unsigned char& flag = vtflags[i];
			if (flag&GLYF_SMPL_X_VECTOR){
				fread(&c,1,1,m_pFontFile);
				if (flag&GLYF_SMPL_X_SAME_FLG){
					// 正值
					Ptx+= c;
					xCoordinates.push_back(Ptx);
				}
				else {
					// 负值
					Ptx+= -c;
					xCoordinates.push_back(Ptx);
				}
			}
			else {
				// 无偏移
				if (flag&GLYF_SMPL_X_SAME_FLG){
					xCoordinates.push_back(Ptx);
				}
				else{	
					fread(&stmp,2,1,m_pFontFile);
					Ptx += hy_cdr_int16_to(stmp);
					xCoordinates.push_back(Ptx);				
				}
			}
		}

		int CntIndex=0;
		CHYPoint hypt;
		CHYContour cntur;
		// 获取Y轴坐标	
		for (i=0; i<nPts;i++){
			unsigned char& flag = vtflags[i];
			if (flag&GLYF_SMPL_Y_VECTOR){		
				fread(&c,1,1,m_pFontFile);
				if (flag&GLYF_SMPL_Y_SAME_FLG){
					// 正值
					Pty+=c;
				}
				else {
					// 负值
					Pty+=-c;					
				}
			}
			else {
				// 无偏移
				if (!(flag&GLYF_SMPL_Y_SAME_FLG)){	
					// SHORT 类型偏移
					fread(&stmp,2,1,m_pFontFile);
					Pty += hy_cdr_int16_to(stmp);					
				}
			}

			if (vtflags[i]&GLYF_SMPL_ONCURVE)
				hypt.flag = POINT_FLG_ANCHOR;
			else 
				hypt.flag = POINT_FLG_CONTROL;

			hypt.y = Pty;		
			hypt.x = xCoordinates[i];
			cntur.vtHYPoints.push_back(hypt);

			if  (i==endptsofContours[CntIndex])	{			
				Glyph.vtContour.push_back(cntur);				
				cntur.vtHYPoints.clear();
				CntIndex++;
			}
		}

	}	// end of void  CHYFontCodec::DecodeGLYF_SIMPLE()

	void CHYFontCodec::DecodeGLYF_COMPOS(CHYGlyph& Glyph)
	{
		Glyph.glyfType = GLYF_TYPE_COMPOSITE;
		USHORT	nIns=0,	flags=0;

		int		iCompIndex  =0;
		CHYCmpst cmpst;
		do {		
			cmpst.SetDefault();

			fread(&cmpst.flags,2,1,m_pFontFile);
			cmpst.flags = hy_cdr_int16_to(cmpst.flags);	

			fread(&cmpst.glyphIndex,2,1,m_pFontFile);
			cmpst.glyphIndex = hy_cdr_int16_to(cmpst.glyphIndex);
			//flags&GLYF_CMPST_ARGS_ARE_XY_VALUES;		
			//flags&GLYF_CMPST_ROUND_XY_TO_GRID;
			//flags&GLYF_CMPST_USE_MY_METRICS;			

			if (cmpst.flags&GLYF_CMPST_ARG_1_AND_2_ARE_WORDS)
			{
				fread(&cmpst.args[0],2,1,m_pFontFile);
				cmpst.args[0] = hy_cdr_int16_to(cmpst.args[0]);
				fread(&cmpst.args[1],2,1,m_pFontFile);
				cmpst.args[1] = hy_cdr_int16_to(cmpst.args[1]);			
			}
			else
			{
				USHORT uArg1and2 = 0; 
				fread(&uArg1and2,2,1,m_pFontFile);
				uArg1and2 = hy_cdr_int16_to(uArg1and2);
				
				cmpst.args[0] = (char)(uArg1and2 >> 8);
				cmpst.args[1] = (char)(uArg1and2 & 0x00FF);
			}

			if (cmpst.flags&GLYF_CMPST_WE_HAVE_A_SCALE)
			{				
				fread(&cmpst.scale,2,1,m_pFontFile);
				cmpst.scale = hy_cdr_int16_to(cmpst.scale);
			}
			else if (cmpst.flags&GLYF_CMPST_WE_HAVE_AN_X_AND_Y_SCALE)
			{
				fread(&cmpst.vctr.xscale,2,1,m_pFontFile);
				cmpst.vctr.xscale = hy_cdr_int16_to(cmpst.vctr.xscale);
				fread(&cmpst.vctr.yscale,2,1,m_pFontFile);
				cmpst.vctr.yscale = hy_cdr_int16_to(cmpst.vctr.yscale);				
			}
			else if (cmpst.flags&GLYF_CMPST_WE_HAVE_A_TWO_BY_TWO)
			{
				fread(&cmpst.tensor.xscale,2,1,m_pFontFile);
				cmpst.tensor.xscale = hy_cdr_int16_to(cmpst.tensor.xscale);
				fread(&cmpst.tensor.scale01,2,1,m_pFontFile);
				cmpst.tensor.scale01 = hy_cdr_int16_to(cmpst.tensor.scale01);
				fread(&cmpst.tensor.scale10,2,1,m_pFontFile);
				cmpst.tensor.scale10 = hy_cdr_int16_to(cmpst.tensor.scale10);
				fread(&cmpst.tensor.yscale,2,1,m_pFontFile);
				cmpst.tensor.yscale = hy_cdr_int16_to(cmpst.tensor.yscale);					 
			}

			Glyph.vtComponents.push_back(cmpst);		

		} while (cmpst.flags&GLYF_CMPST_MORE_COMPONENT); 	

		if (cmpst.flags&GLYF_CMPST_WE_HAVE_INSTRUCTIONS)
		{
			unsigned short  instructionLength = 0;
			fread(&instructionLength,2,1,m_pFontFile);
#if 0
			unsigned char cTmp;
			for (unsigned short i=0; i<instructionLength; i++)
			{
				fread(&cTmp,1,1,m_pFontFile);
				Glyph.vtInstruntions.push_back(cTmp);
			}		
#else 
			fseek(m_pFontFile,instructionLength,SEEK_CUR);
#endif 
		}	

	}	// end of void CHYFontCodec::DecodeGLYF_COMPOS()

	int CHYFontCodec::EncodeGlyph()
	{
		unsigned long	ulTmp = 0;
		unsigned short  usTmp = 0;

		if (m_pFontFile)
		{
			unsigned long ulTableBegin = ftell(m_pFontFile);
			unsigned long ulGlyphOffset = 0;

			int iEntryIndex = m_HYTbDirectory.FindTableEntry(GLYF_TAG);
			if (iEntryIndex ==-1)  return FONT_ERR_GLYF_ENCODE;
			CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
			tbEntry.offset = ftell(m_pFontFile);

			m_HYloca.vtLoca.clear();
			for (size_t i=0; i<m_vtHYGlyphs.size(); i++)
			{
				m_HYloca.vtLoca.push_back(ulGlyphOffset);

				CHYGlyph& glyph = m_vtHYGlyphs[i];			
				if (glyph.vtComponents.size()==0 &&glyph.vtContour.size()==0)
				{
					continue;
				}

				size_t szCnturSize = glyph.vtContour.size();
				if (glyph.vtContour.size()>0)	// 轮廓字形
				{
					//numberOfContours
					usTmp = szCnturSize;
					usTmp =hy_cdr_int16_to(usTmp);
					fwrite(&usTmp,2,1,m_pFontFile);					
					//xMin
					usTmp = hy_cdr_int16_to(glyph.minX);
					fwrite(&usTmp,2,1,m_pFontFile);					
					//yMin
					usTmp = hy_cdr_int16_to(glyph.minY);
					fwrite(&usTmp,2,1,m_pFontFile);					
					//xMax
					usTmp = hy_cdr_int16_to(glyph.maxX);
					fwrite(&usTmp,2,1,m_pFontFile);					
					//yMax
					usTmp = hy_cdr_int16_to(glyph.maxY);
					fwrite(&usTmp,2,1,m_pFontFile);

					bool			bXIsBYTE = true, bYIsBYTE = true;
					CHYPoint		prePoint;
					std::vector<short> vtXCoordinat;
					std::vector<short> vtYCoordinat;
					std::vector<unsigned char>	vtflage;
					std::vector<unsigned short> vtendPtsOfContour;
					short	XOffset=0, YOffset=0;

					size_t  TatlPntNum = 0;
					for (size_t j=0; j<glyph.vtContour.size(); j++)
					{						
						const CHYContour& cntur = glyph.vtContour[j];
						size_t stCutPntnum = cntur.vtHYPoints.size();
						for (size_t x=0; x<stCutPntnum; x++)
						{
							const CHYPoint& pt = cntur.vtHYPoints[x];
							unsigned char	flage = 0;

							if (pt.flag==POINT_FLG_ANCHOR)
							{
								flage |= GLYF_SMPL_ONCURVE;
							}

							// 计算X轴坐标点偏移量
							XOffset = pt.x - prePoint.x;
							if ( XOffset == 0 )
							{
								// 当前点与上一点x位置相同
								flage |= GLYF_SMPL_X_SAME_FLG;
							}
							else 
							{
								// 如果偏移量的绝对值小于255，按单字节保存
								if (abs(XOffset)<=255)
								{
									// x-Short Vector位设置为1
									flage |= GLYF_SMPL_X_VECTOR;
									// 如果offset为正,This x is same表示偏移方向 1为正， 0为负
									if (XOffset>0) flage |= GLYF_SMPL_X_SAME_FLG;

									vtXCoordinat.push_back((short)abs(XOffset));									
								}
								else	// 按双字节保存
								{									
									vtXCoordinat.push_back(XOffset);
								}
							}

							// 计算Y轴坐标点偏移量				
							YOffset = pt.y - prePoint.y;
							if (YOffset==0)
							{
								// 当前点与上一点偏移量相同
								flage |= GLYF_SMPL_Y_SAME_FLG;
							}
							else 
							{
								// 如果偏移量的绝对值小于255，按单字节保存
								if (abs(YOffset) <= 255)
								{
									flage |= GLYF_SMPL_Y_VECTOR;

									// 如果offset为正,This x is same表示偏移方向 1为正， 0为负
									if (YOffset>0) flage |= GLYF_SMPL_Y_SAME_FLG;
									vtYCoordinat.push_back((short)abs(YOffset));
								}
								else	// 按双字节保存
								{
									vtYCoordinat.push_back(YOffset);
								}
							}
							prePoint = pt;
							vtflage.push_back(flage);
						}

						TatlPntNum+=stCutPntnum;
						vtendPtsOfContour.push_back(TatlPntNum-1);
					}

					//endPtsOfContours
					size_t stEndPtsCnturNum = vtendPtsOfContour.size();
					for (size_t y =0; y<stEndPtsCnturNum; y++)
					{
						usTmp = hy_cdr_int16_to(vtendPtsOfContour[y]);
						fwrite(&usTmp,2,1,m_pFontFile);
					}

					//instructionLength					
					usTmp = hy_cdr_int16_to((unsigned short)glyph.vtInstruntions.size());
					fwrite(&usTmp,2,1,m_pFontFile);

					//instructions[n]
					size_t stInstrSize = glyph.vtInstruntions.size();
					for (size_t y=0; y<stInstrSize; y++)
					{
						fwrite(&glyph.vtInstruntions[y],1,1,m_pFontFile);
					}

					// 对Flag做行程压缩
					int				iRepeat=0;					
					unsigned char	btFlag=0, btNextFlag=0;					
					std::vector<unsigned char>	vtRle;										
					size_t	stflagsize = vtflage.size();					

					size_t y=0,z=0;
					for (y=0; y<stflagsize; y++)
					{
						btFlag	= vtflage[y];
						iRepeat = 0;
						for ( z=y+1; z<stflagsize; z++)
						{
							if (iRepeat>255) break;
							btNextFlag = vtflage[z];

							if (btNextFlag!=btFlag) break;
							iRepeat++;
						}

						if (iRepeat>0)
						{
							y = z-1;
							btFlag|=GLYF_SMPL_REPEAT;
							vtRle.push_back(btFlag);
							vtRle.push_back((unsigned char)iRepeat);						
						}
						else 
						{
							vtRle.push_back(btFlag);
						}
					}

					//flags
					size_t stRlesz = vtRle.size();
					for (size_t y=0; y<stRlesz; y++)
					{
						fwrite(&vtRle[y],1,1,m_pFontFile);
					}

					unsigned char ucTmp=0;
					//xCoordinates
					size_t stcrdts = vtflage.size();					
					size_t stAryIndex = 0;
					for (size_t y=0; y<stcrdts; y++)
					{
						if (vtflage[y]&GLYF_SMPL_X_VECTOR)
						{
							ucTmp = (unsigned char)vtXCoordinat[stAryIndex];
							fwrite(&ucTmp,1,1,m_pFontFile);
						}
						else 
						{
							if (vtflage[y]&GLYF_SMPL_X_SAME_FLG)
								continue;
							usTmp = hy_cdr_int16_to(vtXCoordinat[stAryIndex]);
							fwrite(&usTmp,2,1,m_pFontFile);
						}		
						stAryIndex++;
					}

					stAryIndex = 0;
					//yCoordinates
					stcrdts = vtflage.size();					
					for (size_t y=0; y<stcrdts; y++)
					{
						if (vtflage[y]&GLYF_SMPL_Y_VECTOR)
						{
							ucTmp = (unsigned char)vtYCoordinat[stAryIndex];
							fwrite(&ucTmp,1,1,m_pFontFile);
						}
						else 
						{

							if (vtflage[y]&GLYF_SMPL_Y_SAME_FLG)
								continue;

							usTmp = hy_cdr_int16_to(vtYCoordinat[stAryIndex]);
							fwrite(&usTmp,2,1,m_pFontFile);
						}
						stAryIndex++;
					}
				}
				else // 组件字形
				{
					//numberOfContours					
					usTmp =0xffff;
					fwrite(&usTmp,2,1,m_pFontFile);					
					//xMin
					usTmp = hy_cdr_int16_to(glyph.minX);
					fwrite(&usTmp,2,1,m_pFontFile);					
					//yMin
					usTmp = hy_cdr_int16_to(glyph.minY);
					fwrite(&usTmp,2,1,m_pFontFile);					
					//xMax
					usTmp = hy_cdr_int16_to(glyph.maxX);
					fwrite(&usTmp,2,1,m_pFontFile);					
					//yMax
					usTmp = hy_cdr_int16_to(glyph.maxY);
					fwrite(&usTmp,2,1,m_pFontFile);

					size_t stCmpsz = glyph.vtComponents.size();
					for (size_t x=0; x<stCmpsz; x++)
					{
						//flags
						const CHYCmpst&  Cmps = glyph.vtComponents[x];
						usTmp = hy_cdr_int16_to(Cmps.flags);
						fwrite(&usTmp,2,1,m_pFontFile);

						//glyphIndex
						usTmp = hy_cdr_int16_to(Cmps.glyphIndex);
						fwrite(&usTmp,2,1,m_pFontFile);

						//
						if (Cmps.flags&GLYF_CMPST_ARG_1_AND_2_ARE_WORDS)
						{
							usTmp = hy_cdr_int16_to(Cmps.args[0]);
							fwrite(&usTmp,2,1,m_pFontFile);							

							usTmp = hy_cdr_int16_to(Cmps.args[1]);
							fwrite(&usTmp,2,1,m_pFontFile);						
						}
						else 
						{ 
							usTmp = Cmps.args[0]<<8 | (Cmps.args[1]&0xFF);
							usTmp = hy_cdr_int16_to(usTmp);
							fwrite(&usTmp,2,1,m_pFontFile);							
						}

						if (Cmps.flags&GLYF_CMPST_WE_HAVE_A_SCALE)
						{
							usTmp = hy_cdr_int16_to(Cmps.scale);
							fwrite(&usTmp,2,1,m_pFontFile);
						}
						else if (Cmps.flags & GLYF_CMPST_WE_HAVE_AN_X_AND_Y_SCALE)
						{
							usTmp = hy_cdr_int16_to(Cmps.vctr.xscale);
							fwrite(&usTmp,2,1,m_pFontFile);

							usTmp = hy_cdr_int16_to(Cmps.vctr.yscale);
							fwrite(&usTmp,2,1,m_pFontFile);
						}
						else if (Cmps.flags & GLYF_CMPST_WE_HAVE_A_TWO_BY_TWO)
						{
							usTmp = hy_cdr_int16_to(Cmps.tensor.xscale);
							fwrite(&usTmp,2,1,m_pFontFile);

							usTmp = hy_cdr_int16_to(Cmps.tensor.scale01);
							fwrite(&usTmp,2,1,m_pFontFile);

							usTmp = hy_cdr_int16_to(Cmps.tensor.scale10);
							fwrite(&usTmp,2,1,m_pFontFile);

							usTmp = hy_cdr_int16_to(Cmps.tensor.yscale);
							fwrite(&usTmp,2,1,m_pFontFile);
						}
					}					

					if (glyph.vtInstruntions.size() > 0)
					{
						size_t stInstSize = glyph.vtInstruntions.size();
						for (size_t x = 0;x<stInstSize;x++)
						{
							fwrite(&glyph.vtInstruntions[x],1,1,m_pFontFile);
						}						
					}
				}
				ulGlyphOffset = ftell(m_pFontFile) - ulTableBegin;

				int iTail = 4-ulGlyphOffset%4;
				if (ulGlyphOffset%4>0)
				{
					char c = 0;
					for (int i=0; i<iTail; i++)
					{
						fwrite(&c,1,1,m_pFontFile);
					}			
					ulGlyphOffset += iTail;
				}

			}

			// loca number = glyph+1;
			m_HYloca.vtLoca.push_back(ulGlyphOffset);

			tbEntry.length = ftell(m_pFontFile) - tbEntry.offset;
			int iTail = 4-tbEntry.length%4;
			if (tbEntry.length%4>0)
			{
				char c = 0;
				for (int i=0; i<iTail; i++)
				{
					fwrite(&c,1,1,m_pFontFile);
				}				
			}

		}
		return 0;

	}	// end of int CHYFontCodec::EncodeGlyph()

	int	CHYFontCodec::Decodeloca()
	{
		m_HYloca.SetDefault();
		if (m_pFontFile){		
			int iEntryIndex = m_HYTbDirectory.FindTableEntry(LOCA_TAG);
			if (iEntryIndex == -1) return FONT_ERR_LOCA_DECODE;
			
			CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
			fseek(m_pFontFile,tbEntry.offset,SEEK_SET);

			short sIndexToLocFormat = m_HYhead.indexToLocFormat;
			unsigned int usArraySize = m_HYMaxp.numGlyphs+1;//>65535 ? 65535 : m_HYMaxp.numGlyphs+1;
			if (sIndexToLocFormat == LOCA_INDEXTOLOCFORMAT_SHORT)
			{
				unsigned short sTmp; 		
				for (unsigned int i = 0; i<usArraySize; i++){
					fread(&sTmp,2,1,m_pFontFile);
					sTmp = hy_cdr_int16_to(sTmp);
					m_HYloca.vtLoca.push_back(sTmp<<1);
				}				
			}	

			if (sIndexToLocFormat == LOCA_INDEXTOLOCFORMAT_LONG){		
				unsigned long ulTmp; 		
				for (unsigned int i = 0; i<usArraySize; i++){			
					fread(&ulTmp,4,1,m_pFontFile);
					ulTmp = hy_cdr_int32_to(ulTmp);
					m_HYloca.vtLoca.push_back(ulTmp);
				}
			}
		}

		return HY_NOERROR;

	}	// end of int CHYFontCodec::Decodeloca()

	int CHYFontCodec::Encodeloca()
	{
		if (m_pFontFile){
			unsigned long ulTableBegin = ftell(m_pFontFile);
			unsigned long ulGlyphOffset = 0;

			int iEntryIndex = m_HYTbDirectory.FindTableEntry(LOCA_TAG);
			if (iEntryIndex ==-1)  return FONT_ERR_LOCA_ENCODE;
			CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
			tbEntry.offset = ftell(m_pFontFile);

			unsigned long ulTmp = 0;
			unsigned short usTmp = 0;
			size_t stLocalNum = m_HYloca.vtLoca.size();
			if (m_HYhead.indexToLocFormat == 0)	{
				for (size_t i=0; i<stLocalNum; i++)
				{
					usTmp = (unsigned short)m_HYloca.vtLoca[i];
					usTmp = hy_cdr_int16_to(usTmp>>1);
					fwrite(&usTmp,2,1,m_pFontFile);
				}
			}

			if (m_HYhead.indexToLocFormat == 1){
				for (size_t i=0; i<stLocalNum; i++)	{					
					ulTmp = hy_cdr_int32_to(m_HYloca.vtLoca[i]);
					fwrite(&ulTmp,4,1,m_pFontFile);
				}
			}	

			tbEntry.length = ftell(m_pFontFile) - tbEntry.offset;
			if(tbEntry.length%4!=0)
			{
				int iTail = 4-tbEntry.length%4;
				char c = 0;
				for (int i=0; i<iTail; i++)
				{
					fwrite(&c,1,1,m_pFontFile);
				}				
			}
			
		}

		return HY_NOERROR;

	}	// end of int CHYFontCodec::Encodeloca()	

	int	CHYFontCodec::DecodeCFF()
	{	
		m_HYCFFInfo.SetDefault();
		if (m_pFontFile){
			int iEntryIndex = m_HYTbDirectory.FindTableEntry(CFF_TAG);
			if (iEntryIndex == -1) return FONT_ERR_CFF_DECODE;

			CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
			fseek(m_pFontFile,tbEntry.offset,SEEK_SET);		

			// head
 			fread(&m_HYCFFInfo.Header.major,1,1,m_pFontFile);
			fread(&m_HYCFFInfo.Header.minor,1,1,m_pFontFile);
			fread(&m_HYCFFInfo.Header.hdrSize,1,1,m_pFontFile);
			fread(&m_HYCFFInfo.Header.offSize,1,1,m_pFontFile);
			
			// name string 
			DecodeNameString();
			// TopDict
			DecodeTopDict(m_HYCFFInfo.TopDICT);
			// string index
			DecodeStringIndex();
			// global sub Index
			DecodeGlobalSubIndex();
			DecodeCharSets(tbEntry.offset+m_HYCFFInfo.TopDICT.charsetOffset);

			if (m_HYCFFInfo.TopDICT.FDArryIndexOffset>0){			
				unsigned long ulFDArrayOffset = tbEntry.offset+m_HYCFFInfo.TopDICT.FDArryIndexOffset;
				DecodeFDArray(ulFDArrayOffset,tbEntry.offset);
				unsigned long ulFDSelectOffset = tbEntry.offset+m_HYCFFInfo.TopDICT.FDSelectOffset;
				DecodeFDSelect(ulFDSelectOffset);
			}
			else {
				DecodePrivteDict(m_HYCFFInfo.PrivteDict,m_HYCFFInfo.TopDICT.PrivteOffset+tbEntry.offset,m_HYCFFInfo.TopDICT.PrivteDictSize);
			}

			unsigned long ulCharStringOffset = tbEntry.offset+m_HYCFFInfo.TopDICT.charStringOffset;
			DecodeCharString(ulCharStringOffset);
		}

		return HY_NOERROR;

	}	// end of int	CHYFontCodec::DecodeCFF()

	void CHYFontCodec::DecodeNameString()
	{
		if (m_pFontFile)
		{		
			CHYCFFIndex nmInxd;
			DecodeCFFIndex(nmInxd);
			unsigned char ch;
			unsigned long ulSize=0;
			for(unsigned short i=0; i<nmInxd.Count;i++)
			{
				std::string str;
				ulSize = nmInxd.vtOffset[i+1]-nmInxd.vtOffset[i];
				for (unsigned long j=0; j<ulSize;j++)
				{
					fread(&ch,1,1,m_pFontFile);
					str.push_back(ch);
				}
				m_HYCFFInfo.vtFontName.push_back(str);
			}
		}

	}	// end of void CHYFontCodec::DecodeNameString()

	void CHYFontCodec::DecodeTopDict(CHYCFFTopDict& TopDict)
	{
		if (m_pFontFile){		
			CHYCFFIndex TopDictIndx;
			DecodeCFFIndex(TopDictIndx);

			unsigned long					iBPos = ftell(m_pFontFile);
			unsigned long					iCPos=0;
			long							loperator	= 0 ;	
			unsigned long		ulTopDictSize=TopDictIndx.vtOffset[1]-TopDictIndx.vtOffset[0];

			std::vector<double>	vtOperand;
			unsigned char		byFirst;
			unsigned char		bySecond;
			while(iCPos<ulTopDictSize){			 
				 fread(&byFirst,1,1,m_pFontFile);
				 loperator			= 0;
				 if (byFirst>=0 && byFirst<=21) {			
					 if (byFirst==12) {					 
						fread(&bySecond,1,1,m_pFontFile);
						loperator = byFirst<<8|bySecond;					
					 }
					 else 
						 loperator = byFirst;		 

					 switch(loperator) {
						case CFF_DICT_OPERATOR_VERSION:					 
							TopDict.iVersionSID=(int)vtOperand[0];					 
							break;
						case CFF_DICT_OPERATOR_NOTICE:					 
							TopDict.iNoticeSID=(int)vtOperand[0];
							break;
						case CFF_DICT_OPERATOR_COPYRIGHT:					 
							TopDict.iCopyrghtSID=(int)vtOperand[0];					 
							break;
						case CFF_DICT_OPERATOR_FULLNAME:
							TopDict.iFullNameSID=(int)vtOperand[0];					 
							break;
						case CFF_DICT_OPERATOR_FAMILYNAME:
							TopDict.iFamilyNameSID=(int)vtOperand[0];				 
							break;
						case CFF_DICT_OPERATOR_WEIGHT:					 
							TopDict.iWeightSID=(int)vtOperand[0];					 
							break;
						case CFF_DICT_OPERATOR_ISFIXEDPITCH:					 
							if (vtOperand.size()>0)						
								TopDict.isFixedPitch = (long)vtOperand[0];
							break;
						case CFF_DICT_OPERATOR_ITALICANGLE:					 
							if (vtOperand.size()>0)						
								TopDict.ItalicAngle = vtOperand[0];						
							break;
						case CFF_DICT_OPERATOR_UNDERLINEPOSITION:					 
							if (vtOperand.size()>0)						 
								TopDict.UnderlinePosition = vtOperand[0];						 					 
							break;
						case CFF_DICT_OPERATOR_UNDERLINETHICKNESS:					 
							if (vtOperand.size()>0)						
								TopDict.UnderlineThickness = vtOperand[0];						
							break;
						case CFF_DICT_OPERATOR_PAINTTYPE:					 
							if (vtOperand.size()>0)						
								TopDict.PaintType= (long)vtOperand[0];											 
							break;
						case CFF_DICT_OPERATOR_CHARSTRINGTYPE:					 
							if (vtOperand.size()>0)
								TopDict.PaintType= (long)vtOperand[0];
							break;
						case CFF_DICT_OPERATOR_FONTMATRIX:					
							TopDict.vtFontMatrix=vtOperand;												
							break;
						case CFF_DICT_OPERATOR_UNIQUEID:					
							TopDict.UniqueID = (long)vtOperand[0];				 
							break;
						case CFF_DICT_OPERATOR_FONTBBOX:						
							for (size_t s=0; s<vtOperand.size(); s++){
								TopDict.vtFontBOX.push_back((int)vtOperand[s]);
							}
							break;
						case CFF_DICT_OPERATOR_STROKEWIDTH:					 
							if (vtOperand.size()>0)						
								TopDict.strokeWidth = vtOperand[0];											 
							break;
						case CFF_DICT_OPERATOR_XUID:						 
							for (size_t s=0; s<vtOperand.size(); s++){
								TopDict.vtXUID.push_back((int)vtOperand[s]);
							}
							break;
						case CFF_DICT_OPERATOR_CHARSET:					 
							if (vtOperand.size()>0)						
								TopDict.charsetOffset = (long)vtOperand[0];
							break;
						case CFF_DICT_OPERATOR_ENCODING:					 
							if (vtOperand.size()>0)						
								TopDict.encodingOffset = (long)vtOperand[0];						
							break;
						case CFF_DICT_OPERATOR_CHARSTRINGS:						
								TopDict.charStringOffset= (long)vtOperand[0];						
							break;
						case CFF_DICT_OPERATOR_PRIVATE:					 						 						 
							TopDict.PrivteDictSize = (long)vtOperand[0];
							TopDict.PrivteOffset = (long)vtOperand[1];
							break;
						case CFF_DICT_OPERATOR_SYNTHETICBASE:
							TopDict.SyntheticBaseIndex = (long)vtOperand[0];				 
							break;
						case CFF_DICT_OPERATOR_POSTSCRIPT:					
							TopDict.iPostSriptSID = (int)vtOperand[0];
							break;
						case CFF_DICT_OPERATOR_BASEFONTNAME:					
							TopDict.iBaseFontNameSID = (int)vtOperand[0];
							break;
						case CFF_DICT_OPERATOR_BASEFONTBLEND:
							for(size_t s=0; s<vtOperand.size(); s++){
								TopDict.vtBaseFontBlend.push_back(vtOperand[s]);
							}
							break;
						case CFF_DICT_OPERATOR_ROS:	
							//Registry
							TopDict.Ros.RegistrySID=(int)vtOperand[0];
							//Ordering
							TopDict.Ros.OrderingSID=(int)vtOperand[1];
							//Supplement
							TopDict.Ros.Supplement=(int)vtOperand[2];
							TopDict.IsCIDFont = true;
							break;
						case CFF_DICT_OPERATOR_CIDFONTVERSION:				 
							if (vtOperand.size()>0)						 
								TopDict.CIDFontVersion = vtOperand[0];
							break;
						case CFF_DICT_OPERATOR_CIDFONTREVISION:					 
							if (vtOperand.size()>0)
								TopDict.CIDFontRevision= vtOperand[0];					 
							break;
						case CFF_DICT_OPERATOR_CIDFONTTYPE:				 
							if (vtOperand.size()>0)
								TopDict.CIDFontType = vtOperand[0];
							break;
						case CFF_DICT_OPERATOR_CIDCOUNT:					 
							if (vtOperand.size()>0)						
								TopDict.CIDCount = (long)vtOperand[0];
							break;
						case CFF_DICT_OPERATOR_UIDBASE:					
							TopDict.UIDBase = (long)vtOperand[0];
							break;
						case CFF_DICT_OPERATOR_FDARRAY:					 
							TopDict.FDArryIndexOffset =  (long)vtOperand[0];
							break;
						case CFF_DICT_OPERATOR_FDSELECT:				
							TopDict.FDSelectOffset = (long)vtOperand[0];
							break;
						case CFF_DICT_OPERATOR_FONTNAME:					 
							TopDict.CIDFontNameSID=(int)vtOperand[0];
							break;
						default:
							break;							
					 }
					 vtOperand.clear();			
				}
				else 
				{	
					double f;
					if (byFirst == 0x1e)
					{				 
						f=DecodeDICTReal();
						vtOperand.push_back(f);
					}
					else 
					{
						fseek(m_pFontFile,-1,SEEK_CUR);
						f=DecodeDICTInteger();
						vtOperand.push_back((double)f);
					}
				}

				iCPos = ftell(m_pFontFile)- iBPos;
			}
		}

	}	// end of void CHYFontCodec::DecodeTopDict()

	void CHYFontCodec::DecodeGlobalSubIndex()
	{
		// Global Sub Index;
		DecodeCFFIndex(m_HYCFFInfo.globalSubIndex);	

		unsigned long ulSize = 0;
		for(unsigned short i=0; i<m_HYCFFInfo.globalSubIndex.Count;i++)
		{			
			ulSize += m_HYCFFInfo.globalSubIndex.vtOffset[i+1]-m_HYCFFInfo.globalSubIndex.vtOffset[i];		
		}		
		unsigned char ch;
		for (unsigned long i=0; i<ulSize; i++)
		{
			fread(&ch,1,1,m_pFontFile);
			m_HYCFFInfo.globalSubIndex.vtData.push_back(ch);
		}		

	}	// end of void CHYFontCodec::DecodeGlobalSubIndex()

	void CHYFontCodec::DecodeFDArray(unsigned long ulOffset, unsigned long ulCFFBegPos)
	{	
		if (m_pFontFile)
		{	
			fseek(m_pFontFile,ulOffset,SEEK_SET);

			CHYCFFIndex		PdictIndex;
			DecodeCFFIndex(PdictIndex);

			for(unsigned short i=0; i<PdictIndex.Count;i++)
			{				
				unsigned long	iDataSize = PdictIndex.vtOffset[i+1]-PdictIndex.vtOffset[i];

				CHYCFFPrivteDict				PrvtDict;
				std::vector<double>				vtOperand;
				long							loperator	= 0;			
				unsigned long					iBPos = ftell(m_pFontFile);
				unsigned long					iCPos = 0;
				unsigned char					byFirst, bySecond;

				while (iCPos<iDataSize)
				{			
					fread(&byFirst,1,1,m_pFontFile);
					// operator 
					if (byFirst>=0 && byFirst<=21)
					{			
						if (byFirst==12)
						{					
							fread(&bySecond,1,1,m_pFontFile);
							loperator = byFirst<<8|bySecond;					
						}
						else 			
							loperator = byFirst;				
						
						if (loperator == 0x0c26)
						{
							PrvtDict.iFontNameID = (int)vtOperand[0];
						}
						else 
						{
							unsigned long unCurnPin = ftell(m_pFontFile);							
							if (vtOperand.size()>1)
							{
								DecodePrivteDict(PrvtDict, (unsigned long)vtOperand[1] + ulCFFBegPos, (unsigned long)vtOperand[0]);
								fseek(m_pFontFile, unCurnPin, SEEK_SET);
								m_HYCFFInfo.vtFDArry.push_back(PrvtDict);
							}
						}
						vtOperand.clear();
					}
					else  
					{
						double f;
						if ( byFirst == 0x1e )
						{						
							f = DecodeDICTReal();
							vtOperand.push_back(f);
						}
						else 
						{
							fseek(m_pFontFile,-1,SEEK_CUR);
							f = (double)DecodeDICTInteger();
							vtOperand.push_back(f);
						}
					}

					iCPos = ftell(m_pFontFile)-iBPos;
				}
			}
		}	

	}	// end of void CHYFontCodec::DecodeFDArray()

	void CHYFontCodec::DecodePrivteDict(CHYCFFPrivteDict& PrvtDict, unsigned long  ulOffset,unsigned long ulDataSize)
	{
		if (m_pFontFile)
		{
			fseek(m_pFontFile,ulOffset,SEEK_SET);
			std::vector<double>				vtOperand;
			long							loperator	= 0;			
			unsigned long					iBPos = ftell(m_pFontFile);
			unsigned long					iCPos = 0;
			unsigned char					byFirst, bySecond;
			
			while (iCPos<ulDataSize)
			{			
				fread(&byFirst,1,1,m_pFontFile);

				// operator 
				if (byFirst>=0 && byFirst<=21)
				{			
					if (byFirst == 12)
					{
						fread(&bySecond,1,1,m_pFontFile);
						loperator = byFirst<<8|bySecond;					
					}
					else 				
						loperator = byFirst;				
					
					switch(loperator)
					{
						case CFF_DICT_OPERATOR_BLUEVALUES:						
							PrvtDict.vtBlueValues=vtOperand;
							break;
						case CFF_DICT_OPERATOR_OTHERBLUES:
							PrvtDict.vtOtherBlues=vtOperand;
							break;
						case CFF_DICT_OPERATOR_FAMILYBLUES:
							PrvtDict.vtFamilyBlues=vtOperand;
							break;
						case CFF_DICT_OPERATOR_FAMILYOTHERBLUES:
							PrvtDict.vtFamilyOtherBlues=vtOperand;
							break;
						case CFF_DICT_OPERATOR_BLUESCALE:						
							if (vtOperand.size()>0)							
								PrvtDict.fBlueScale=vtOperand[0];						
							break;
						case CFF_DICT_OPERATOR_BLUESHIFT:						
							if (vtOperand.size()>0)							
								PrvtDict.fBlueShift=vtOperand[0];
							break;
						case CFF_DICT_OPERATOR_BLUEFUZZ:
							if (vtOperand.size()>0)							
								PrvtDict.fBlueFuzz=vtOperand[0];
							break;
						case CFF_DICT_OPERATOR_STDHW:						
							PrvtDict.fStdHW=vtOperand[0];
							break;
						case CFF_DICT_OPERATOR_STDVW:							
							PrvtDict.fStdVW=vtOperand[0];
							break;
						case CFF_DICT_OPERATOR_STEMSNAPH:
							PrvtDict.vtStemSnapH=vtOperand;
							break;
						case CFF_DICT_OPERATOR_STEMSNAPV:
							PrvtDict.vtStemSnapV=vtOperand;
							break;
						case CFF_DICT_OPERATOR_FORCEBOLD:
							if (vtOperand.size()>0)						
								PrvtDict.lForceBold=(long)vtOperand[0];						
							break;
						case CFF_DICT_OPERATOR_LANGUAGEGROUP:
							if (vtOperand.size()>0)						
								PrvtDict.lLanguageGroup=(long)vtOperand[0];						
							break;
						case CFF_DICT_OPERATOR_EXPANSIONFACTOR:						
							if (vtOperand.size()>0)						
								PrvtDict.fExpansionFactor=vtOperand[0];												
							break;
						case CFF_DICT_OPERATOR_INITIALRANDOMSEED:						
							if (vtOperand.size()>0)						
								PrvtDict.finitialRandomSeed=vtOperand[0];												
							break;
						case CFF_DICT_OPERATOR_SUBRS:
							{
								unsigned long ulSubOffset = ulOffset+(unsigned long)vtOperand[0];								

								unsigned long ulCurPos = ftell(m_pFontFile);
								fseek(m_pFontFile,ulSubOffset,SEEK_SET);
								
								DecodeCFFIndex(PrvtDict.SubIndex);		
								unsigned long ulSize = 0;
								for(unsigned short i=0; i<PrvtDict.SubIndex.Count;i++)
								{			
									ulSize += PrvtDict.SubIndex.vtOffset[i+1]-PrvtDict.SubIndex.vtOffset[i];		
								}
								
								unsigned char ch;
								for (unsigned long i=0; i<ulSize; i++)
								{
									fread(&ch,1,1,m_pFontFile);
									PrvtDict.SubIndex.vtData.push_back(ch);
								}
								fseek(m_pFontFile,ulCurPos,SEEK_SET);
							}	
							break;
						case CFF_DICT_OPERATOR_DEFAULTWIDTHX:						
							if (vtOperand.size()>0)
								PrvtDict.ldefaultWidthX=(long)vtOperand[0];
							break;
						case CFF_DICT_OPERATOR_NOMINALWIDTHX:						
							if (vtOperand.size()>0)						
								PrvtDict.lnominalWidthX=(long)vtOperand[0];												
							break;
						default:
							break;
					}				
					vtOperand.clear();
				}
				else 
				{
					double f;
					if (byFirst==0x1e)
					{
						f=DecodeDICTReal();					
						vtOperand.push_back(f);
					}
					else 
					{
						fseek(m_pFontFile,-1,SEEK_CUR);
						f=(double)DecodeDICTInteger();
						vtOperand.push_back(f);
					}			
				}
				iCPos = ftell(m_pFontFile)-iBPos;
			}		
		}	
		
	}	// end of void CHYFontCodec::DecodePrivteDict()

	void CHYFontCodec::DecodeFDSelect(unsigned long  ulOffset)
	{
		if (m_pFontFile)
		{
			fseek(m_pFontFile,ulOffset,SEEK_SET);
			unsigned char uch;
			fread(&uch,1,1,m_pFontFile);

			int				i = 0;
			unsigned short	nTmp = 0;
			BYTE			btmp = 0;

			m_HYCFFInfo.FDSelect.iFormat = uch;
			if (uch == 0)
			{
				for (long i=0; i<m_HYCFFInfo.TopDICT.CIDCount; i++)
				{
					fread(&uch,1,1,m_pFontFile);
					m_HYCFFInfo.FDSelect.format0.push_back(uch);
				}
			}		

			if (uch==3)
			{
				nTmp = 0;
				fread(&uch,1,1,m_pFontFile);
				nTmp |= uch<<8;
				fread(&uch,1,1,m_pFontFile);	
				nTmp |= uch;				

				for(i=0; i<nTmp; i++)
				{
					CCFFFDSRang3	FDRang3;

					fread(&uch,1,1,m_pFontFile);
					FDRang3.first |= uch<<8;
					fread(&uch,1,1,m_pFontFile);
					FDRang3.first |= uch;

					fread(&FDRang3.fdIndex,1,1,m_pFontFile);
					m_HYCFFInfo.FDSelect.format3.vtRang3.push_back(FDRang3);
				}
				nTmp = 0;
				fread(&uch,1,1,m_pFontFile);
				nTmp |= uch<<8;
				fread(&uch,1,1,m_pFontFile);	
				nTmp |= uch;
				m_HYCFFInfo.FDSelect.format3.sentinel= nTmp;
			}
		}

	}	// end of void CHYFontCodec::DecodeFDSelect()
	
	int	 CHYFontCodec::GetFDIndex(CHYCFFFDSelect& FDSelect, int iGID)
	{
		int FDIndex = -1;
		if (FDSelect.iFormat==0)
		{
			if (FDSelect.format0.size() >= (size_t)iGID)
			{
				FDIndex = FDSelect.format0[iGID];
			}
		}

		if (FDSelect.iFormat == 3)
		{
			int iRange3Size = FDSelect.format3.vtRang3.size();

			for (int i=0; i<iRange3Size; i++)
			{
				if (i==iRange3Size-1)
				{
					if (iGID>=FDSelect.format3.vtRang3[i].first && iGID<=FDSelect.format3.sentinel)
					{
						FDIndex = FDSelect.format3.vtRang3[i].fdIndex;
					}
				}
				else if ((iGID>=FDSelect.format3.vtRang3[i].first) && (iGID<FDSelect.format3.vtRang3[i+1].first))
				{
					FDIndex = FDSelect.format3.vtRang3[i].fdIndex;
					break;
				}
			}
		}

		return FDIndex;

	}	// end of int	 CHYFontCodec::GetFDIndex()

	void CHYFontCodec::DecodeCharString(unsigned long  ulOffset)
	{
		if (m_pFontFile){			
			fseek(m_pFontFile,ulOffset,SEEK_SET);
			CHYCFFIndex			CharStringIndx;
			DecodeCFFIndex(CharStringIndx);

			int iStringlen = 0;	
			for (int i=0; i<CharStringIndx.Count; i++){				
				CHYGlyph&	HYGryph = m_vtHYGlyphs[i];
				double		AdbWidht = -32768.0;
				int			iDept = 0;				

				CCharStringParam	CharParam;
				CHYContour			HYContour;

				unsigned long ulCharDataSize = CharStringIndx.vtOffset[i+1] -CharStringIndx.vtOffset[i];				
				if (ulCharDataSize<0x80000)	{
					unsigned char charData[0x80000] ={0};
					fread(charData,ulCharDataSize,1,m_pFontFile);
					DecodeCharData(charData,ulCharDataSize, HYGryph,i,AdbWidht,(void*)&CharParam, iDept,HYContour);				

					HYGryph.fontFlag=FONTTYPE_OTF;
					if (HYGryph.GetTotalPointsNumber() > 0){
						HYGryph.CountBoundBox();
					}

					if (!m_HYCFFInfo.TopDICT.IsCIDFont) {
						HYGryph.strPostName = m_HYCFFInfo.FindStringbyGlyphID(i);
					}
					else {
						char  buffer[64] = { 0 };
						_stprintf(buffer, _T("CID%d"), i);
						HYGryph.strPostName = buffer;
					}
				}
				m_iStepNo = (i+1)*100/m_HYMaxp.numGlyphs;	
			}
		}

	}	// end of void CHYFontCodec::DecodeCharString()

	void CHYFontCodec::DecodeCharData(unsigned char* pData, unsigned long ulDataSize,CHYGlyph& HYGryph, int iGID, double& AdvWidth,void* pCharParam,int iDepth,CHYContour& HYContour)
	{
		unsigned char const *		bufend = pData + ulDataSize;
		CCharStringParam*  pCharStringPara = (CCharStringParam*)pCharParam;
		if (iDepth > 10) return;
		if (iDepth == 0) pCharStringPara->bufStart = pData;

		BYTE		B0 = 0, B1 = 0, B2 = 0, B3 = 0, B4 = 0;		
		CHYPoint	hypoint;
		int			iStemNumber = 0;

		while (pData < bufend)
		{
			unsigned char const*	opStart = pData;
			B0=*pData++;

			// b0 range 32 - 246
			if (B0>31 && B0<247)
			{
				pCharStringPara->arguments.push(static_cast <double> (static_cast <SHORT> (static_cast <USHORT> (B0))-139));
				continue;
			}

			// b0 rang 247 - 250
			if (B0>246 && B0<251)
			{				
				B1=*pData++;
				pCharStringPara->arguments.push(static_cast <double> ((B0-247)* 256+B1+108));
				continue;
			}

			// b0 rang 251 - 254
			if (B0>250 && B0<255)
			{						
				B1=*pData++;

				pCharStringPara->arguments.push(static_cast <double> (-(B0-251)* 256-B1-108));
				continue;
			}

			// b0 rang 28
			if (B0 == TYPE2_CHARSTING_OPERATOR_SHORTINT )
			{
				B1=*pData++;
				B2=*pData++;

				pCharStringPara->arguments.push(static_cast <double> (static_cast <short>(B1<<8|B2)));
				continue;
			}

			// b0 rang 255
			if (B0 == 255 )
			{
				B1=*pData++;
				B2=*pData++;
				B3=*pData++;
				B4=*pData++;

				pCharStringPara->arguments.push(static_cast <double> (static_cast <long>(B1<<24|B2<<16|B3<<8|B4))/65536.0);
				continue;
			}

			switch(B0)
			{
			case TYPE2_CHARSTING_OPERATOR_HSTEM:
				{
					size_t		acount = 2*(pCharStringPara->arguments.size()/2);

					if (!pCharStringPara->hstem && iDepth == 0)	// We won't modify a subroutine
					{
						pCharStringPara->hstemPos = static_cast <size_t>(opStart-pCharStringPara->bufStart);
						pCharStringPara->hstemArgs = acount;
					}

					pCharStringPara->TakeWidth(AdvWidth,acount);
					pCharStringPara->hintCount += static_cast <USHORT> (acount/2);
					pCharStringPara->arguments.clear();
					pCharStringPara->hstem = true;
				}
				break;
			case TYPE2_CHARSTING_OPERATOR_VSTEM:
				{
					// We don't care about hints
					size_t		acount = 2*(pCharStringPara->arguments.size()/2);					
					pCharStringPara->TakeWidth(AdvWidth,acount);
					pCharStringPara->hintCount += static_cast <USHORT>(acount/2);
					pCharStringPara->arguments.clear();
					pCharStringPara->vstem = true;
				}
				break;			
			case TYPE2_CHARSTING_OPERATOR_HSTEMHM:
				{
					size_t		acount = 2*(pCharStringPara->arguments.size()/2);

					if (!pCharStringPara->hstem && iDepth==0)	// We won't modify a subroutine
					{
						pCharStringPara->hstemPos = static_cast <size_t>(opStart-pCharStringPara->bufStart);
						pCharStringPara->hstemArgs = acount;
					}					
					pCharStringPara->TakeWidth(AdvWidth,acount);
					pCharStringPara->hintCount += static_cast<USHORT>(acount/2);
					pCharStringPara->arguments.clear();
					pCharStringPara->hstem = true;
				}
				break;

			case TYPE2_CHARSTING_OPERATOR_VSTEMHM:
				{
					size_t		acount = 2*(pCharStringPara->arguments.size()/2);					
					pCharStringPara->TakeWidth(AdvWidth,acount);
					pCharStringPara->hintCount += static_cast <USHORT>(acount/2);
					pCharStringPara->arguments.clear();
					pCharStringPara->vstem = true;
				}
				break;			
			case TYPE2_CHARSTING_OPERATOR_HINTMASK:
			case TYPE2_CHARSTING_OPERATOR_CNTRMASK:
				{
					if (pCharStringPara->hstem && !pCharStringPara->vstem && (pCharStringPara->lastOp==1||pCharStringPara->lastOp==18))
					{
						size_t		acount = 2*(pCharStringPara->arguments.size()/2);
						pCharStringPara->hintCount += static_cast <USHORT> (acount/2);
						pCharStringPara->vstem = true;
						pCharStringPara->arguments.popN(acount);						
						pCharStringPara->TakeWidth(AdvWidth,acount);
					}
					else
					{
						size_t		acount = 2*(pCharStringPara->arguments.size()/2);
						pCharStringPara->hintCount += static_cast <USHORT> (acount/2);						
						pCharStringPara->TakeWidth(AdvWidth,acount);
					}

					size_t		count = HintBytes (pCharStringPara->hintCount);
					
					if (pData+count > bufend) return;
					pData += count;
					
					pCharStringPara->arguments.clear();
				}
				break;
			case TYPE2_CHARSTING_OPERATOR_RMOVETO:
				{
					if (HYContour.vtHYPoints.size()>0)
					{
						HYGryph.vtContour.push_back(HYContour);					
						HYContour.SetDefault();
					}

					SetMovePath (pCharStringPara->movePath, iDepth, pCharStringPara->injectPos, opStart, pCharStringPara->bufStart);					
					int		ix = pCharStringPara->TakeWidth(AdvWidth,2);
					pCharStringPara->position.x += pCharStringPara->arguments[ix++];
					pCharStringPara->position.y += pCharStringPara->arguments[ix++];

					hypoint.x		= HY_RealRount(pCharStringPara->position.x);
					hypoint.y		= HY_RealRount(pCharStringPara->position.y);

					hypoint.flag	= POINT_FLG_ANCHOR;

					HYContour.vtHYPoints.push_back(hypoint);
					pCharStringPara->arguments.clear();
				}
				break;		
			case TYPE2_CHARSTING_OPERATOR_HMOVETO:
				{
					if (HYContour.vtHYPoints.size()>0)
					{
						HYGryph.vtContour.push_back(HYContour);	
						HYContour.SetDefault();
					}

					SetMovePath (pCharStringPara->movePath, iDepth, pCharStringPara->injectPos, opStart, pCharStringPara->bufStart);					
					int		ix = pCharStringPara->TakeWidth(AdvWidth,1);

					pCharStringPara->position.x += pCharStringPara->arguments[ix];

					hypoint.x = HY_RealRount(pCharStringPara->position.x);
					hypoint.y = HY_RealRount(pCharStringPara->position.y);
					hypoint.flag = POINT_FLG_ANCHOR;

					HYContour.vtHYPoints.push_back(hypoint);
					pCharStringPara->arguments.clear();
				}
				break;
			case TYPE2_CHARSTING_OPERATOR_VMOVETO:
				{
					if (HYContour.vtHYPoints.size()>0)
					{
						HYGryph.vtContour.push_back(HYContour);		
						HYContour.SetDefault();
					}

					SetMovePath (pCharStringPara->movePath, iDepth, pCharStringPara->injectPos, opStart, pCharStringPara->bufStart);
					int		ix = pCharStringPara->TakeWidth(AdvWidth,1);

					pCharStringPara->position.y += pCharStringPara->arguments[ix];

					hypoint.x = HY_RealRount(pCharStringPara->position.x);
					hypoint.y = HY_RealRount(pCharStringPara->position.y);
					hypoint.flag = POINT_FLG_ANCHOR;

					HYContour.vtHYPoints.push_back(hypoint);
					pCharStringPara->arguments.clear();
				}
				break;
			case TYPE2_CHARSTING_OPERATOR_RLINETO:
				{
					size_t		pairCount = pCharStringPara->arguments.size()/2;
					for (size_t ix = 0; ix != pairCount; ++ix)
					{
						double		dx = pCharStringPara->arguments[2*ix];
						double		dy = pCharStringPara->arguments[2*ix+1];

						pCharStringPara->position.x += dx;
						pCharStringPara->position.y += dy;

						hypoint.x = HY_RealRount(pCharStringPara->position.x);
						hypoint.y = HY_RealRount(pCharStringPara->position.y);

						hypoint.flag = POINT_FLG_ANCHOR;					
						HYContour.vtHYPoints.push_back(hypoint);
					}

					pCharStringPara->arguments.clear();
				}
				break;
			case TYPE2_CHARSTING_OPERATOR_HLINETO:
				{
					size_t		count = pCharStringPara->arguments.size();
					size_t		ix = 0;
					bool		h = true;
					while (ix!=count)
					{
						double		d = pCharStringPara->arguments[ix];
						if (h)
						{						
							pCharStringPara->position.x += d;
							h = false;
						}
						else
						{						
							pCharStringPara->position.y += d;
							h = true;
						}

						hypoint.x = HY_RealRount(pCharStringPara->position.x);
						hypoint.y = HY_RealRount(pCharStringPara->position.y);
						hypoint.flag = POINT_FLG_ANCHOR;

						HYContour.vtHYPoints.push_back(hypoint);
						++ix;
					}
					pCharStringPara->arguments.clear();
				}
				break;		
			case TYPE2_CHARSTING_OPERATOR_VLINETO:
				{
					size_t		count = pCharStringPara->arguments.size();
					size_t		ix = 0;
					bool		v = true;

					while (ix!=count)
					{
						double d = pCharStringPara->arguments[ix];
						if (v)
						{						
							pCharStringPara->position.y +=d;
							v = false;
						}
						else
						{
							pCharStringPara->position.x +=d;						
							v = true;
						}
						++ix;

						hypoint.x = HY_RealRount(pCharStringPara->position.x);
						hypoint.y = HY_RealRount(pCharStringPara->position.y);
						hypoint.flag = POINT_FLG_ANCHOR;

						HYContour.vtHYPoints.push_back(hypoint);
					}
					pCharStringPara->arguments.clear();
				}
				break;
			case TYPE2_CHARSTING_OPERATOR_RRCURVETO:
				{
					int		iStackIndex		= 0; 
					int		iStackDeep		= static_cast<int>(pCharStringPara->arguments.size());

					while( iStackIndex < iStackDeep )
					{
						pCharStringPara->position.x += pCharStringPara->arguments[iStackIndex++];
						pCharStringPara->position.y += pCharStringPara->arguments[iStackIndex++];
						hypoint.x = HY_RealRount(pCharStringPara->position.x);
						hypoint.y = HY_RealRount(pCharStringPara->position.y);
						hypoint.flag = POINT_FLG_CONTROL;
						HYContour.vtHYPoints.push_back(hypoint);

						pCharStringPara->position.x += pCharStringPara->arguments[iStackIndex++];
						pCharStringPara->position.y += pCharStringPara->arguments[iStackIndex++];
						hypoint.x = HY_RealRount(pCharStringPara->position.x);
						hypoint.y = HY_RealRount(pCharStringPara->position.y);
						hypoint.flag = POINT_FLG_CONTROL;
						HYContour.vtHYPoints.push_back(hypoint);

						pCharStringPara->position.x += pCharStringPara->arguments[iStackIndex++];
						pCharStringPara->position.y += pCharStringPara->arguments[iStackIndex++];
						hypoint.x = HY_RealRount(pCharStringPara->position.x);
						hypoint.y = HY_RealRount(pCharStringPara->position.y);
						hypoint.flag = POINT_FLG_ANCHOR;
						HYContour.vtHYPoints.push_back(hypoint);					
					}
					pCharStringPara->arguments.clear();
				}
				break;
			case TYPE2_CHARSTING_OPERATOR_HHCURVETO:
				{
					int iStackIndex = 0;
					int iStackDeep = static_cast<int>(pCharStringPara->arguments.size());

					if ((iStackDeep%4)>1) 
					{
						pCharStringPara->arguments.clear();
						break;
					}

					double		dya = 0.0;
					if( (iStackDeep%4)!= 0 )
					{
						dya += pCharStringPara->arguments[iStackIndex++];
					}

					while (iStackIndex<iStackDeep)
					{
						double		dxa = pCharStringPara->arguments[iStackIndex++];
						double		dxb = pCharStringPara->arguments[iStackIndex++];
						double		dyb = pCharStringPara->arguments[iStackIndex++];
						double		dxc = pCharStringPara->arguments[iStackIndex++];
						Point		da (dxa, dya);
						Point		db (dxb, dyb);
						Point		curve [4];

						curve [0] = pCharStringPara->position;
						curve [1] = curve [0] + da;
						curve [2] = curve [1] + db;
						curve [3].x = curve [2].x+dxc;
						curve [3].y = curve [2].y;

						hypoint.x = HY_RealRount(curve[1].x);
						hypoint.y = HY_RealRount(curve[1].y);
						hypoint.flag = POINT_FLG_CONTROL;
						HYContour.vtHYPoints.push_back(hypoint);

						hypoint.x = HY_RealRount(curve [2].x);
						hypoint.y = HY_RealRount(curve [2].y);
						hypoint.flag = POINT_FLG_CONTROL;
						HYContour.vtHYPoints.push_back(hypoint);

						hypoint.x = HY_RealRount(curve[3].x);
						hypoint.y = HY_RealRount(curve[3].y);
						hypoint.flag = POINT_FLG_ANCHOR;
						HYContour.vtHYPoints.push_back(hypoint);

						pCharStringPara->position = curve[3];

						dya = 0.0;
					}
					pCharStringPara->arguments.clear();
				}
				break;
			case TYPE2_CHARSTING_OPERATOR_HVCURVETO:
				{
					BOOL bHorizontal = TRUE;
					int iStackIndex = 0; 
					int iStackDeep = static_cast<int>(pCharStringPara->arguments.size());

					while(iStackIndex<iStackDeep)
					{
						if (bHorizontal)
						{
							pCharStringPara->position.x += pCharStringPara->arguments[iStackIndex++];
							hypoint.x = HY_RealRount(pCharStringPara->position.x);
							hypoint.y = HY_RealRount(pCharStringPara->position.y);
							hypoint.flag = POINT_FLG_CONTROL;
							HYContour.vtHYPoints.push_back(hypoint);

							pCharStringPara->position.x += pCharStringPara->arguments[iStackIndex++];
							pCharStringPara->position.y += pCharStringPara->arguments[iStackIndex++];
							hypoint.x = HY_RealRount(pCharStringPara->position.x);
							hypoint.y = HY_RealRount(pCharStringPara->position.y);
							hypoint.flag = POINT_FLG_CONTROL;
							HYContour.vtHYPoints.push_back(hypoint);


							if (iStackDeep%4!=0)
							{
								if (iStackIndex == (iStackDeep-2))
								{
									pCharStringPara->position.y += pCharStringPara->arguments[iStackIndex++];
									pCharStringPara->position.x += pCharStringPara->arguments[iStackIndex++];

									hypoint.x = HY_RealRount(pCharStringPara->position.x);
									hypoint.y = HY_RealRount(pCharStringPara->position.y);
									hypoint.flag = POINT_FLG_ANCHOR;
									HYContour.vtHYPoints.push_back(hypoint);									
								}
								else
								{
									pCharStringPara->position.y += pCharStringPara->arguments[iStackIndex++];

									hypoint.x = HY_RealRount(pCharStringPara->position.x);
									hypoint.y = HY_RealRount(pCharStringPara->position.y);
									hypoint.flag = POINT_FLG_ANCHOR;
									HYContour.vtHYPoints.push_back(hypoint);
								}
							}
							else 
							{
								pCharStringPara->position.y += pCharStringPara->arguments[iStackIndex++];

								hypoint.x = HY_RealRount(pCharStringPara->position.x);
								hypoint.y = HY_RealRount(pCharStringPara->position.y);
								hypoint.flag = POINT_FLG_ANCHOR;
								HYContour.vtHYPoints.push_back(hypoint);
							}
						}
						else 
						{
							pCharStringPara->position.y += pCharStringPara->arguments[iStackIndex++];

							hypoint.x = HY_RealRount(pCharStringPara->position.x);
							hypoint.y = HY_RealRount(pCharStringPara->position.y);
							hypoint.flag = POINT_FLG_CONTROL;
							HYContour.vtHYPoints.push_back(hypoint);							

							pCharStringPara->position.x += pCharStringPara->arguments[iStackIndex++];
							pCharStringPara->position.y += pCharStringPara->arguments[iStackIndex++];

							hypoint.x = HY_RealRount(pCharStringPara->position.x);
							hypoint.y = HY_RealRount(pCharStringPara->position.y);
							hypoint.flag = POINT_FLG_CONTROL;
							HYContour.vtHYPoints.push_back(hypoint);		

							if (iStackDeep%4 != 0)
							{
								if (iStackIndex == (iStackDeep-2))
								{
									pCharStringPara->position.x += pCharStringPara->arguments[iStackIndex++];
									pCharStringPara->position.y += pCharStringPara->arguments[iStackIndex++];

									hypoint.x = HY_RealRount(pCharStringPara->position.x);
									hypoint.y = HY_RealRount(pCharStringPara->position.y);
									hypoint.flag = POINT_FLG_ANCHOR;
									HYContour.vtHYPoints.push_back(hypoint);	
								}
								else
								{
									pCharStringPara->position.x += pCharStringPara->arguments[iStackIndex++];

									hypoint.x = HY_RealRount(pCharStringPara->position.x);
									hypoint.y = HY_RealRount(pCharStringPara->position.y);
									hypoint.flag = POINT_FLG_ANCHOR;
									HYContour.vtHYPoints.push_back(hypoint);
								}
							}
							else 
							{
								pCharStringPara->position.x += pCharStringPara->arguments[iStackIndex++];

								hypoint.x = HY_RealRount(pCharStringPara->position.x);
								hypoint.y = HY_RealRount(pCharStringPara->position.y);
								hypoint.flag = POINT_FLG_ANCHOR;
								HYContour.vtHYPoints.push_back(hypoint);								
							}
						}
						bHorizontal = !bHorizontal;
					}
					pCharStringPara->arguments.clear();
				}
				break;
			case TYPE2_CHARSTING_OPERATOR_RCURVELINE:
				{
					int iStackIndex = 0; 
					int iStackDeep = static_cast<int>(pCharStringPara->arguments.size());

					while(iStackIndex < iStackDeep-2)
					{
						pCharStringPara->position.x += pCharStringPara->arguments[iStackIndex++];
						pCharStringPara->position.y += pCharStringPara->arguments[iStackIndex++];

						hypoint.x = HY_RealRount(pCharStringPara->position.x);
						hypoint.y = HY_RealRount(pCharStringPara->position.y);
						hypoint.flag = POINT_FLG_CONTROL;
						HYContour.vtHYPoints.push_back(hypoint);					

						pCharStringPara->position.x += pCharStringPara->arguments[iStackIndex++];
						pCharStringPara->position.y += pCharStringPara->arguments[iStackIndex++];

						hypoint.x = HY_RealRount(pCharStringPara->position.x);
						hypoint.y = HY_RealRount(pCharStringPara->position.y);
						hypoint.flag = POINT_FLG_CONTROL;
						HYContour.vtHYPoints.push_back(hypoint);

						pCharStringPara->position.x += pCharStringPara->arguments[iStackIndex++];
						pCharStringPara->position.y += pCharStringPara->arguments[iStackIndex++];

						hypoint.x = HY_RealRount(pCharStringPara->position.x);
						hypoint.y = HY_RealRount(pCharStringPara->position.y);
						hypoint.flag = POINT_FLG_ANCHOR;
						HYContour.vtHYPoints.push_back(hypoint);
					}

					pCharStringPara->position.x += pCharStringPara->arguments[iStackIndex++];
					pCharStringPara->position.y += pCharStringPara->arguments[iStackIndex++];

					hypoint.x = HY_RealRount(pCharStringPara->position.x);
					hypoint.y = HY_RealRount(pCharStringPara->position.y);
					hypoint.flag = POINT_FLG_ANCHOR;
					HYContour.vtHYPoints.push_back(hypoint);

					pCharStringPara->arguments.clear();
				}
				break;
			case TYPE2_CHARSTING_OPERATOR_RLINECURVE:
				{
					int iStackIndex = 0; 
					int iStackDeep = static_cast<int>(pCharStringPara->arguments.size());

					while(iStackIndex < iStackDeep-6)
					{
						pCharStringPara->position.x += pCharStringPara->arguments[iStackIndex++];
						pCharStringPara->position.y += pCharStringPara->arguments[iStackIndex++];

						hypoint.x = HY_RealRount(pCharStringPara->position.x);
						hypoint.y = HY_RealRount(pCharStringPara->position.y);
						hypoint.flag = POINT_FLG_ANCHOR;
						HYContour.vtHYPoints.push_back(hypoint);						
					}

					pCharStringPara->position.x += pCharStringPara->arguments[iStackIndex++];
					pCharStringPara->position.y += pCharStringPara->arguments[iStackIndex++];

					hypoint.x = HY_RealRount(pCharStringPara->position.x);
					hypoint.y = HY_RealRount(pCharStringPara->position.y);
					hypoint.flag = POINT_FLG_CONTROL;
					HYContour.vtHYPoints.push_back(hypoint);					

					pCharStringPara->position.x += pCharStringPara->arguments[iStackIndex++];
					pCharStringPara->position.y += pCharStringPara->arguments[iStackIndex++];

					hypoint.x = HY_RealRount(pCharStringPara->position.x);
					hypoint.y = HY_RealRount(pCharStringPara->position.y);
					hypoint.flag = POINT_FLG_CONTROL;
					HYContour.vtHYPoints.push_back(hypoint);

					pCharStringPara->position.x += pCharStringPara->arguments[iStackIndex++];
					pCharStringPara->position.y += pCharStringPara->arguments[iStackIndex++];

					hypoint.x = HY_RealRount(pCharStringPara->position.x);
					hypoint.y = HY_RealRount(pCharStringPara->position.y);
					hypoint.flag = POINT_FLG_ANCHOR;
					HYContour.vtHYPoints.push_back(hypoint);

					pCharStringPara->arguments.clear();
				}
				break;
			case TYPE2_CHARSTING_OPERATOR_VHCURVETO:
				{
					BOOL bVertical = TRUE;
					int iStackIndex = 0; 
					int iStackDeep = static_cast<int>(pCharStringPara->arguments.size());

					while(iStackIndex < iStackDeep)
					{
						if (bVertical)
						{
							pCharStringPara->position.y += pCharStringPara->arguments[iStackIndex++];

							hypoint.x = HY_RealRount(pCharStringPara->position.x);
							hypoint.y = HY_RealRount(pCharStringPara->position.y);
							hypoint.flag = POINT_FLG_CONTROL;
							HYContour.vtHYPoints.push_back(hypoint);

							pCharStringPara->position.x += pCharStringPara->arguments[iStackIndex++];
							pCharStringPara->position.y += pCharStringPara->arguments[iStackIndex++];

							hypoint.x = HY_RealRount(pCharStringPara->position.x);
							hypoint.y = HY_RealRount(pCharStringPara->position.y);
							hypoint.flag = POINT_FLG_CONTROL;
							HYContour.vtHYPoints.push_back(hypoint);

							if (iStackDeep%4 != 0)
							{
								if ( iStackIndex == (iStackDeep - 2) )
								{
									pCharStringPara->position.x += pCharStringPara->arguments[iStackIndex++];
									pCharStringPara->position.y += pCharStringPara->arguments[iStackIndex++];

									hypoint.x = HY_RealRount(pCharStringPara->position.x);
									hypoint.y = HY_RealRount(pCharStringPara->position.y);
									hypoint.flag = POINT_FLG_ANCHOR;
									HYContour.vtHYPoints.push_back(hypoint);
								}
								else 
								{
									pCharStringPara->position.x += pCharStringPara->arguments[iStackIndex++];

									hypoint.x = HY_RealRount(pCharStringPara->position.x);
									hypoint.y = HY_RealRount(pCharStringPara->position.y);
									hypoint.flag = POINT_FLG_ANCHOR;
									HYContour.vtHYPoints.push_back(hypoint);
								}
							}
							else 
							{
								pCharStringPara->position.x += pCharStringPara->arguments[iStackIndex++];

								hypoint.x = HY_RealRount(pCharStringPara->position.x);
								hypoint.y = HY_RealRount(pCharStringPara->position.y);
								hypoint.flag = POINT_FLG_ANCHOR;
								HYContour.vtHYPoints.push_back(hypoint);
							}
						}
						else 
						{
							pCharStringPara->position.x += pCharStringPara->arguments[iStackIndex++];

							hypoint.x = HY_RealRount(pCharStringPara->position.x);
							hypoint.y = HY_RealRount(pCharStringPara->position.y);
							hypoint.flag = POINT_FLG_CONTROL;
							HYContour.vtHYPoints.push_back(hypoint);

							pCharStringPara->position.x += pCharStringPara->arguments[iStackIndex++];
							pCharStringPara->position.y += pCharStringPara->arguments[iStackIndex++];

							hypoint.x = HY_RealRount(pCharStringPara->position.x);
							hypoint.y = HY_RealRount(pCharStringPara->position.y);
							hypoint.flag = POINT_FLG_CONTROL;
							HYContour.vtHYPoints.push_back(hypoint);

							if (iStackDeep%4 != 0)
							{
								if (iStackIndex == (iStackDeep-2))
								{
									pCharStringPara->position.y += pCharStringPara->arguments[iStackIndex++];
									pCharStringPara->position.x += pCharStringPara->arguments[iStackIndex++];

									hypoint.x = HY_RealRount(pCharStringPara->position.x);
									hypoint.y = HY_RealRount(pCharStringPara->position.y);
									hypoint.flag = POINT_FLG_ANCHOR;
									HYContour.vtHYPoints.push_back(hypoint);
								}
								else
								{
									pCharStringPara->position.y += pCharStringPara->arguments[iStackIndex++];

									hypoint.x = HY_RealRount(pCharStringPara->position.x);
									hypoint.y = HY_RealRount(pCharStringPara->position.y);
									hypoint.flag = POINT_FLG_ANCHOR;
									HYContour.vtHYPoints.push_back(hypoint);
								}
							}
							else 
							{
								pCharStringPara->position.y += pCharStringPara->arguments[iStackIndex++];

								hypoint.x = HY_RealRount(pCharStringPara->position.x);
								hypoint.y = HY_RealRount(pCharStringPara->position.y);
								hypoint.flag = POINT_FLG_ANCHOR;
								HYContour.vtHYPoints.push_back(hypoint);
							}
						}
						bVertical = !bVertical;
					}
					pCharStringPara->arguments.clear();
				}
				break;
			case TYPE2_CHARSTING_OPERATOR_VVCURVETO:
				{
					int iStackIndex = 0; 
					int iStackDeep = static_cast<int>(pCharStringPara->arguments.size());

					if ((pCharStringPara->arguments.size()%4)>1) break;

					double		dxa = 0.0;
					size_t		ix = 0;
					if ((iStackDeep%4) != 0)
					{
						dxa = pCharStringPara->arguments[ix++];
					}

					while (ix<pCharStringPara->arguments.size())
					{
						double		dya = pCharStringPara->arguments[ix++];
						double		dxb = pCharStringPara->arguments[ix++];
						double		dyb = pCharStringPara->arguments[ix++];
						double		dyc = pCharStringPara->arguments[ix++];
						Point		da (dxa, dya);
						Point		db (dxb, dyb);
						Point		curve [4];

						curve [0] = pCharStringPara->position;
						curve [1] = curve [0]+da;
						curve [2] = curve [1]+db;
						curve [3].x = curve [2].x;
						curve [3].y = curve [2].y+dyc;						

						hypoint.x = HY_RealRount(curve[1].x);
						hypoint.y = HY_RealRount(curve[1].y);
						hypoint.flag = POINT_FLG_CONTROL;
						HYContour.vtHYPoints.push_back(hypoint);

						hypoint.x = HY_RealRount(curve[2].x);
						hypoint.y = HY_RealRount(curve[2].y);
						hypoint.flag = POINT_FLG_CONTROL;
						HYContour.vtHYPoints.push_back(hypoint);

						hypoint.x = HY_RealRount(curve[3].x);
						hypoint.y = HY_RealRount(curve[3].y);
						hypoint.flag = POINT_FLG_ANCHOR;
						HYContour.vtHYPoints.push_back(hypoint);	

						pCharStringPara->position = curve[3];
						dxa = 0.0;
					}
					pCharStringPara->arguments.clear();
				}
				break;			
			case TYPE2_CHARSTING_OPERATOR_ENDCHAR:
				{
					int		ix = pCharStringPara->TakeWidth(AdvWidth,pCharStringPara->arguments.size()>1?4:0);

					pCharStringPara->arguments.clear();
					pCharStringPara->lastOp = B0;					

					if (HYContour.vtHYPoints.size()>0)
					{
						HYGryph.vtContour.push_back(HYContour);
						HYContour.SetDefault();
					}

					if (pCharStringPara->haveWidth)
					{
						if (m_HYCFFInfo.TopDICT.Ros.RegistrySID|m_HYCFFInfo.TopDICT.Ros.OrderingSID|m_HYCFFInfo.TopDICT.Ros.Supplement)
						{
							int iFDIndex = GetFDIndex(m_HYCFFInfo.FDSelect, iGID);
							if ( iFDIndex > -1 )
							{
								if (AdvWidth == -32768.0 )
								{
									AdvWidth = m_HYCFFInfo.vtFDArry[iFDIndex].ldefaultWidthX;
								}
								else 
								{
									AdvWidth = m_HYCFFInfo.vtFDArry[iFDIndex].lnominalWidthX+AdvWidth;
								}
							}
						}
						else
						{
							if (AdvWidth == -32768.0 )
							{
								AdvWidth = m_HYCFFInfo.PrivteDict.ldefaultWidthX;
							}
							else 
							{
								AdvWidth = m_HYCFFInfo.PrivteDict.lnominalWidthX+AdvWidth;
							}
						}
					}
					else 
					{
						AdvWidth = m_HYCFFInfo.PrivteDict.ldefaultWidthX;
					}

					HYGryph.advanceWidth = HY_RealRount(AdvWidth);
				}
				break;
			case TYPE2_CHARSTING_OPERATOR_CALLSUBR:
				{
					int FDIndex = GetFDIndex(m_HYCFFInfo.FDSelect, iGID);
					unsigned char data[10240]	= {0};
					int ipos					= 0;
					unsigned long ulSubSize		= 0;
					unsigned long ulStart		= 0;
					unsigned long ulEnd			= 0;

					if (FDIndex != -1) 
					{
						CHYCFFIndex& CFFIndex = m_HYCFFInfo.vtFDArry[FDIndex].SubIndex;
						int					ix = make_int32_t (pCharStringPara->arguments.pop());
						size_t				count = (size_t)CFFIndex.Count;

						if (count < 1240)			ix += 107;
						else if (count < 33900)		ix += 1131;
						else						ix += 32768;

						if (static_cast <size_t>(ix) < (size_t)CFFIndex.Count)
						{
							ulSubSize =CFFIndex.vtOffset[ix+1]-CFFIndex.vtOffset[ix];
							ulStart = CFFIndex.vtOffset[ix]-1;
							ulEnd = CFFIndex.vtOffset[ix+1]-1;
							
							for (unsigned long i= ulStart; i!=ulEnd;i++)
							{
								data[ipos++] = CFFIndex.vtData[i];
							}

							DecodeCharData(data, ulSubSize,HYGryph, iGID, AdvWidth, (void*)pCharStringPara,iDepth,HYContour);
						}
					}		
					else 
					{
						CHYCFFIndex& CFFIndex	= m_HYCFFInfo.PrivteDict.SubIndex;
						size_t				count = (size_t)CFFIndex.Count;

						int							ix = make_int32_t (pCharStringPara->arguments.pop());
						if (count < 1240)			ix += 107;
						else if (count < 33900)		ix += 1131;
						else						ix += 32768;

						if (static_cast <size_t> (ix) < (size_t)CFFIndex.Count)
						{
							ulSubSize =CFFIndex.vtOffset[ix+1]-CFFIndex.vtOffset[ix];
							ulStart = CFFIndex.vtOffset[ix]-1;
							ulEnd = CFFIndex.vtOffset[ix+1]-1;

							for (unsigned long  i= ulStart; i!=ulEnd;i++)
							{
								data[ipos++] = CFFIndex.vtData[i];
							}
							DecodeCharData(data, ulSubSize,HYGryph, iGID, AdvWidth, (void*)pCharStringPara,iDepth,HYContour);
						}					
					}				
				}
				break;
			case TYPE2_CHARSTING_OPERATOR_CALLGSUBR:
				{
					CHYCFFIndex& CFFIndex = m_HYCFFInfo.globalSubIndex;
					size_t				count = CFFIndex.Count;
					int							ix = make_int32_t (pCharStringPara->arguments.pop());
					if (count < 1240)			ix += 107;
					else if (count < 33900)		ix += 1131;
					else						ix += 32768;

					if (static_cast <size_t> (ix) < count)
					{						
						unsigned long ulSubSize =CFFIndex.vtOffset[ix+1]-CFFIndex.vtOffset[ix];
						unsigned char data[10240] = {0};

						unsigned long ulStart = CFFIndex.vtOffset[ix]-1;
						unsigned long ulEnd = CFFIndex.vtOffset[ix+1]-1;

						int ipos = 0;
						for (unsigned long  i=ulStart; i!=ulEnd;i++)
						{
							data[ipos++] = CFFIndex.vtData[i];
						}

						DecodeCharData(data, ulSubSize,HYGryph, iGID, AdvWidth, (void*)pCharStringPara,iDepth,HYContour);

					}					
				}
				break;
			case TYPE2_CHARSTING_OPERATOR_RETURN:
				pCharStringPara->lastOp = B0;
				return;
			case TYPE2_CHARSTING_OPERATOR_ESCAPE:
				{					
					//fread(&B0,1,1,m_pFontFile);

					B0 = *pData++;
					USHORT US = 0x0C00 | B0;
					switch (US)
					{					
					case TYPE2_CHARSTING_OPERATOR_RESERVED0:		// dotsection (deprecated)
					case TYPE2_CHARSTING_OPERATOR_RESERVED1:
					case TYPE2_CHARSTING_OPERATOR_RESERVED2:
						pCharStringPara->arguments.clear();
						break;
					case TYPE2_CHARSTING_OPERATOR_AND:		// and
						pCharStringPara->arguments.push (pCharStringPara->arguments.pop() && pCharStringPara->arguments.pop()? 1.0: 0.0);
						break;
					case TYPE2_CHARSTING_OPERATOR_OR:		// or
						pCharStringPara->arguments.push (pCharStringPara->arguments.pop() || pCharStringPara->arguments.pop()? 1.0: 0.0);
						break;
					case TYPE2_CHARSTING_OPERATOR_NOT:		// not
						pCharStringPara->arguments.push (pCharStringPara->arguments.pop()? 0.0: 1.0);
						break;
					case TYPE2_CHARSTING_OPERATOR_ABS:		// abs
						{
							double		x = pCharStringPara->arguments.pop();
							pCharStringPara->arguments.push (x < 0.0? -x: x);
						}

						break;
					case TYPE2_CHARSTING_OPERATOR_ADD:		// add
						pCharStringPara->arguments.push (pCharStringPara->arguments.pop() + pCharStringPara->arguments.pop());
						break;
					case TYPE2_CHARSTING_OPERATOR_SUB:		// sub
						{
							double		y = pCharStringPara->arguments.pop();
							double		x = pCharStringPara->arguments.pop();

							pCharStringPara->arguments.push (x - y);
						}
						break;
					case TYPE2_CHARSTING_OPERATOR_DIV:		// div
						{
							double		y = pCharStringPara->arguments.pop();
							double		x = pCharStringPara->arguments.pop();

							pCharStringPara->arguments.push (x / y);
						}

						break;
					case TYPE2_CHARSTING_OPERATOR_NEG:		// neg
						pCharStringPara->arguments.push (-pCharStringPara->arguments.pop());

						break;
					case TYPE2_CHARSTING_OPERATOR_EG:		// eq
						pCharStringPara->arguments.push (pCharStringPara->arguments.pop() == pCharStringPara->arguments.pop()? 1.0: 0.0);

						break;
					case TYPE2_CHARSTING_OPERATOR_DROP:		// drop
						pCharStringPara->arguments.drop();

						break;
					case TYPE2_CHARSTING_OPERATOR_PUT:		// put
						{
							size_t		ix = make_size_t (pCharStringPara->arguments.pop());

							if (ix >= 32)
							{
								pCharStringPara->arguments.clear();
								break;
							}

							pCharStringPara->transient [ix] = pCharStringPara->arguments.pop();
						}

						break;
					case TYPE2_CHARSTING_OPERATOR_GET:		// get
						{
							size_t		ix = make_size_t (pCharStringPara->arguments.pop());

							if (ix >= 32)
							{
								pCharStringPara->arguments.clear();
								break;
							}

							pCharStringPara->arguments.push (pCharStringPara->transient [ix]);
						}

						break;
					case TYPE2_CHARSTING_OPERATOR_IFELSE:		// ifelse
						{
							double		v2 = pCharStringPara->arguments.pop();
							double		v1 = pCharStringPara->arguments.pop();
							double		s2 = pCharStringPara->arguments.pop();
							double		s1 = pCharStringPara->arguments.pop();

							pCharStringPara->arguments.push (v1 <= v2? s1: s2);
						}

						break;
					case TYPE2_CHARSTING_OPERATOR_RANDOM:		// random
						{
							double dbRand = static_cast<double>(rand());
							pCharStringPara->arguments.push (dbRand);
						}
						break;
					case TYPE2_CHARSTING_OPERATOR_MUL:		// mul
						{
							pCharStringPara->arguments.push (pCharStringPara->arguments.pop() * pCharStringPara->arguments.pop());
						}
						break;
					case TYPE2_CHARSTING_OPERATOR_SQRT:		// sqrt
						pCharStringPara->arguments.push (sqrt (pCharStringPara->arguments.pop()));
						break;
					case TYPE2_CHARSTING_OPERATOR_DUP:		// dup
						pCharStringPara->arguments.dup();
						break;
					case TYPE2_CHARSTING_OPERATOR_EXCH:		// exch
						pCharStringPara->arguments.exch();
						break;
					case TYPE2_CHARSTING_OPERATOR_INDEX:	// index
						pCharStringPara->arguments.index();
						break;
					case TYPE2_CHARSTING_OPERATOR_ROLL:		// roll
						pCharStringPara->arguments.roll(); 
						break;
					case TYPE2_CHARSTING_OPERATOR_HFLEX:	// hflex
						{
							if (pCharStringPara->arguments.size() < 7)
							{
								pCharStringPara->arguments.clear();
								break;
							}

							double		y = pCharStringPara->position.y;
							double		jointy = y + pCharStringPara->arguments [2];

							Point		curve [4];

							curve [0].x = pCharStringPara->position.x;
							curve [0].y = pCharStringPara->position.y;

							curve [1] = Point (curve [0].x + pCharStringPara->arguments [0], y);
							curve [2] = Point (curve [1].x + pCharStringPara->arguments [1], jointy);
							curve [3] = Point (curve [2].x + pCharStringPara->arguments [3], jointy);

							pCharStringPara->position.x = curve [3].x;
							pCharStringPara->position.y = curve [3].y;

							curve [0].x = pCharStringPara->position.x;
							curve [0].y = pCharStringPara->position.y;
							curve [1] = Point (curve [0].x + pCharStringPara->arguments [4], jointy);
							curve [2] = Point (curve [1].x +pCharStringPara->arguments [5], y);
							curve [3] = Point (curve [2].x + pCharStringPara->arguments [6], y);

							pCharStringPara->position.x = curve [3].x;
							pCharStringPara->position.y = curve [3].y;

							pCharStringPara->arguments.clear();
							break;
						}
						break;
					case TYPE2_CHARSTING_OPERATOR_FLEX:		// flex
						{
							if (pCharStringPara->arguments.size() < 13)
							{
								pCharStringPara->arguments.clear();
								break;
							}

							for (size_t ix = 0; ix != 1; ++ix)
							{
								Point		da (pCharStringPara->arguments [6 * ix + 0], pCharStringPara->arguments [6 * ix + 1]);
								Point		db (pCharStringPara->arguments [6 * ix + 2], pCharStringPara->arguments [6 * ix + 3]);
								Point		dc (pCharStringPara->arguments [6 * ix + 4], pCharStringPara->arguments [6 * ix + 5]);
								Point		curve [4];

								curve[0].x = pCharStringPara->position.x;
								curve[0].y = pCharStringPara->position.y;
								curve[1] = curve [0] + da;
								curve[2] = curve [1] + db;
								curve[3] = curve [2] + dc;

								pCharStringPara->position.x = curve [3].x;
								pCharStringPara->position.y = curve [3].y;
							}

							pCharStringPara->arguments.clear();
							break;

						}
						break;
					case TYPE2_CHARSTING_OPERATOR_HFLEX1:		// hflex1
						{
							if (pCharStringPara->arguments.size() < 9)
							{
								pCharStringPara->arguments.clear();
								break;
							}

							double		y = pCharStringPara->position.y;
							double		jointy = y + pCharStringPara->arguments[3];

							Point		curve [4];

							curve [0].x = pCharStringPara->position.x;
							curve [0].y = pCharStringPara->position.y;

							curve [1] = Point (curve [0].x + pCharStringPara->arguments [0], curve [0].y + pCharStringPara->arguments [1]);
							curve [2] = Point (curve [1].x + pCharStringPara->arguments [2], curve [1].y + pCharStringPara->arguments [3]);
							curve [3] = Point (curve [2].x + pCharStringPara->arguments [4], jointy);

							curve [0].x = pCharStringPara->position.x;
							curve [0].y = pCharStringPara->position.y;
							curve [1] = Point (curve [0].x + pCharStringPara->arguments [5], jointy);
							curve [2] = Point (curve [1].x + pCharStringPara->arguments [6], curve [1].y + pCharStringPara->arguments [7]);
							curve [3] = Point (curve [2].x + pCharStringPara->arguments [8], y);


							pCharStringPara->position.x = curve [3].x;
							pCharStringPara->position.y = curve [3].y;

						}
						pCharStringPara->arguments.clear();
						break;
					case TYPE2_CHARSTING_OPERATOR_FLEX1:		// flex1
						{

							if (pCharStringPara->arguments.size() != 11)
							{
								pCharStringPara->arguments.clear();
								break;
							}

							double		d6 = pCharStringPara->arguments.pop();
							double		x = pCharStringPara->position.x;
							double		y = pCharStringPara->position.y;

							Point		da (pCharStringPara->arguments[0], pCharStringPara->arguments[1]);
							Point		db (pCharStringPara->arguments[2], pCharStringPara->arguments[3]);
							Point		dc (pCharStringPara->arguments[4], pCharStringPara->arguments[5]);
							Point		curve [4];

							curve [0].x = pCharStringPara->position.x;
							curve [0].y = pCharStringPara->position.y;
							curve [1] = curve [0] + da;
							curve [2] = curve [1] + db;
							curve [3] = curve [2] + dc;

							pCharStringPara->position.x = curve [3].x;
							pCharStringPara->position.y = curve [3].y;


							double		dx = da.x + db.x + dc.x;
							double		dy = da.y + db.y + dc.y;

							da = Point (pCharStringPara->arguments [6], pCharStringPara->arguments [7]);
							db = Point (pCharStringPara->arguments [8], pCharStringPara->arguments [9]);

							dx += da.x + db.x;
							dy += da.y + db.y;

							curve [0].x = pCharStringPara->position.x;
							curve [0].y = pCharStringPara->position.y;
							curve [1] = curve [0] + da;
							curve [2] = curve [1] + db;

							if (abs (dx) > abs (dy))
							{
								curve [3].x = curve [2].x + d6;
								curve [3].y = y;
							}
							else
							{
								curve [3].x = x;
								curve [3].y = curve [2].y + d6;
							}

							pCharStringPara->arguments.clear();
							break;
						}
					default:
						break;
					}
					pCharStringPara->lastOp = US;
					break;
				}			
			default:
				break;
			}
			pCharStringPara->lastOp = B0;	
		}

	}	// end of void CHYFontCodec::DecodeCharData()

	void CHYFontCodec::SetMovePath (bool &movePath, size_t depth, size_t &injectPos, unsigned char const *opstart, unsigned char const * bufstart)
	{
		if (!movePath)
		{
			movePath = true;
			if (depth == 0)
				injectPos = static_cast <size_t> (opstart - bufstart);
		}

	}

	int CHYFontCodec::HintBytes(unsigned hintCount)
	{
		return (hintCount+7)/8;

	}	// end of int CHYFontCodec::HintBytes()

	std::string CHYFontCodec::DecodeCFFString(int SID, CHYCFFIndex& strIndex)
	{
		std::string			str="";
		CHYCFFStandString	Standstr;

		unsigned long iOffset			= 0;
		unsigned long iDataLen		= 0;	

		int iStandStrMaxInx = Standstr.m_szStandString.size()-1;
		if (SID>iStandStrMaxInx)
		{
			iOffset = strIndex.vtOffset[SID-iStandStrMaxInx]-1;
			iDataLen = strIndex.vtOffset[SID-iStandStrMaxInx+1] - strIndex.vtOffset[SID-iStandStrMaxInx];		
			
			for (unsigned long i=0; i<iDataLen;i++)
			{
				str.push_back(strIndex.vtData[iOffset+i]);
			}		
		}
		else 
		{
			str = Standstr.m_szStandString[SID];	
		}

		return str;

	}	// end of void	CHYFontCodec::DecodeCFFString()

	void CHYFontCodec::DecodeCFFIndex(CHYCFFIndex& CffIndex)
	{	
		if (m_pFontFile)
		{
			unsigned long	i=0;
			unsigned char	uchar=0;
			unsigned long	loffset = 0;
			unsigned long	DataSize = 0;

			fread(&CffIndex.Count,2,1,m_pFontFile);
			CffIndex.Count = hy_cdr_int16_to(CffIndex.Count);
			if(CffIndex.Count>0)
			{
				fread(&CffIndex.OffSize,1,1,m_pFontFile);
				for (i=0; i<CffIndex.Count; i++)
				{				
					loffset = DecodeCFFInteger(CffIndex.OffSize);					
					CffIndex.vtOffset.push_back(loffset);				
				}

				loffset = DecodeCFFInteger(CffIndex.OffSize);					
				CffIndex.vtOffset.push_back(loffset);		
			}
		}

	}	// end of void CHYFontCodec::DecodeCFFIndex()

	long CHYFontCodec::DecodeCFFInteger(int iDataSize)
	{
		long	Integer = 0xffffffff;
		if (m_pFontFile)
		{
			BYTE	B0=0, B1=0, B2=0, B3=0;
			for (int i=0; i<iDataSize; i++)
			{
				switch(i)
				{
					case 0:
						fread(&B0,1,1,m_pFontFile);			
						break;
					case 1:
						fread(&B1,1,1,m_pFontFile);			
						break;
					case 2:
						fread(&B2,1,1,m_pFontFile);			
						break;
					case 3:
						fread(&B3,1,1,m_pFontFile);			
						break;
					default:
						break;
				}
			}

			switch(iDataSize)
			{
				case 1:
					Integer = B0;
					break;
				case 2:
					Integer = B0<<8|B1;
					break;
				case 3:
					Integer = B0<<16|B1<<8|B2;
					break;
				case 4:
					Integer = B0<<24|B1<<16|B2<<8|B3;
					break;
				default:
					break;
			}
		}	

		return Integer;

	}	//end of long CHYFontCodec::DecodeCFFInteger()

	long CHYFontCodec::DecodeDICTInteger()
	{
		long	Integer = 0xffffffff;
		if (m_pFontFile)
		{
			BYTE	B0=0, B1=0, B2=0, B3=0, B4=0;		
			fread(&B0,1,1,m_pFontFile);

			// b0 range 32 - 246
			if (B0>31 && B0<247)
			{			
				return Integer = B0-139;
			}

			// b0 rang 247 - 250
			if (B0>246 && B0<251)
			{			
				fread(&B1,1,1,m_pFontFile);		
				return Integer = (B0-247)* 256+B1+108;
			}

			// b0 rang 251 - 254
			if (B0>250 && B0<255)
			{
				fread(&B1,1,1,m_pFontFile);			
				return Integer = -(B0 - 251)* 256-B1-108;
			}

			// b0 rang 28
			if (B0 == 28 )
			{
				fread(&B1,1,1,m_pFontFile);
				fread(&B2,1,1,m_pFontFile);			
				return Integer = B1<<8|B2;
			}

			// b0 rang 29
			if (B0 == 29 )
			{	
				fread(&B1,1,1,m_pFontFile);
				fread(&B2,1,1,m_pFontFile);
				fread(&B3,1,1,m_pFontFile);
				fread(&B4,1,1,m_pFontFile);

				return Integer = B1<<24|B2<<16|B3<<8|B4;
			}
		}

		return Integer;

	}	// end of int CHYFontCodec::DecodeDICTInteger()

	double CHYFontCodec::DecodeDICTReal()
	{
		double		result = 0.0f;
		if (m_pFontFile)
		{
			bool				going = true;
			bool				started = false;
			bool				inFraction = false;
			bool				inExponent = false;
			unsigned			ix = 0;
			char				bcd [23];
			
			while (going)
			{
				unsigned char		a;
				fread(&a,1,1,m_pFontFile);
				for (unsigned i=0; i<2; ++i)
				{
					unsigned char		nybble= i?a&0xf:a>>4;
					switch (nybble)
					{
						case 0:
						case 1:
						case 2:
						case 3:
						case 4:
						case 5:
						case 6:
						case 7:
						case 8:
						case 9:
							bcd[ix++] = '0'+nybble;
							started = true;
							break;
						case 0xa:
							if (inFraction)					
								return 0.0f;

							if (inExponent)					
								return 0.0f;					

							inFraction = true;
							bcd [ix++] = '.';
							started = true;
							break;
						case 0xb:
						case 0xc:
							if (inExponent)										
								return 0.0f;					

							inExponent = true;
							bcd [ix++] = 'e';

							if (nybble == 0xc)
								bcd [ix++] = '-';

							started = true;
							break;
						case 0xd:					
							return 0.0f;
						case 0xe:
							if (started)					
								return 0.0f;

							bcd [ix++] = '-';
							started = true;
							break;
						case 0xf:
							going = false;
					}

					if (ix>20)							
						return 0.0f;				
				}
			}

			bcd [ix++] = 0;	
			sscanf (bcd, "%lf", &result);
		}	

		return result;

	}	// end of double CHYFontCodec::DecodeDICTReal()

	int	CHYFontCodec::EncodeCFF()
	{
		if (m_pFontFile){
			unsigned long ulTableBegin = ftell(m_pFontFile);
			unsigned long ulGlyphOffset = 0;
			int iEntryIndex = m_HYTbDirectory.FindTableEntry(CFF_TAG);
			if (iEntryIndex ==-1)  return FONT_ERR_CFF_ENCODE;

			CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
			tbEntry.offset = ftell(m_pFontFile);
			
			PrepareCFFInfo();			
			std::vector<char> vtHead;
			std::vector<char> vtName;
			std::vector<char> vtTopDict;
			std::vector<char> vtString;	
			std::vector<char> vtGolbIndex;
			std::vector<char> vtCharset;
			std::vector<char> vtFDSelect;
			std::vector<char> vtFArrayData;
			std::vector<char> vtPrivateData;
			std::vector<char> vtCharStrings;

			// string 
			EncodeStringIndex(vtString);

			EncodeCFFHead(vtHead);			
			EncodeNameIndex(vtName);
			EncodeTopDictIndex(vtTopDict);
			EncodeGlobalIndex(vtGolbIndex);			
			EncodeCharSets(vtCharset);
			EncodeCharStrings(vtCharStrings);		

			unsigned int ulOffset = 0;
			ulOffset = vtHead.size()+vtName.size()+vtTopDict.size()+vtString.size()+vtGolbIndex.size();
			// charset
			m_HYCFFInfo.TopDICT.charsetOffset = ulOffset;		
			ulOffset += vtCharset.size();

			if (m_HYCFFInfo.TopDICT.IsCIDFont){
				// fdselect				
				m_HYCFFInfo.TopDICT.FDSelectOffset = ulOffset;
				EncodeFDSelect(vtFDSelect);
				ulOffset+=vtFDSelect.size();
			}

			m_HYCFFInfo.TopDICT.charStringOffset = ulOffset;
			ulOffset += vtCharStrings.size();

			if (m_HYCFFInfo.TopDICT.IsCIDFont){
				//fdarray
				m_HYCFFInfo.TopDICT.FDArryIndexOffset= ulOffset;				
				EncodeFDArray(vtFArrayData,ulOffset);
				ulOffset+=vtFArrayData.size();
			}		
			else{
				//private Dict
				m_HYCFFInfo.TopDICT.PrivteOffset=ulOffset;
				EncodePrivateDICTData(vtPrivateData,m_HYCFFInfo.PrivteDict);
				m_HYCFFInfo.TopDICT.PrivteDictSize=vtPrivateData.size();
				ulOffset+= vtPrivateData.size();
			}
			
			vtTopDict.clear();
			EncodeTopDictIndex(vtTopDict);
			
			//write file
			// Head
			size_t st =vtHead.size(); 
			for(size_t i=0; i<st; i++){
				fwrite(&vtHead[i],1,1,m_pFontFile);
			}
			// Name
			st = vtName.size();
			for(size_t i=0; i<st; i++){
				fwrite(&vtName[i],1,1,m_pFontFile);
			}
			// TopDict
			st = vtTopDict.size();
			for (size_t i=0; i<st; i++){
				fwrite(&vtTopDict[i],1,1,m_pFontFile);
			}
			// string
			st = vtString.size();
			for (size_t i=0; i<st; i++){
				fwrite(&vtString[i],1,1,m_pFontFile);
			}
			// Global
			st = vtGolbIndex.size();
			for (size_t i=0; i<st; i++){
				fwrite(&vtGolbIndex[i],1,1,m_pFontFile);
			}
			// charset
			st = vtCharset.size();
			for (size_t i=0; i<st; i++){
				fwrite(&vtCharset[i],1,1,m_pFontFile);
			}

			if (m_HYCFFInfo.TopDICT.IsCIDFont){
				// FDSelect
				st = vtFDSelect.size();
				for (size_t i=0; i<st; i++){
					fwrite(&vtFDSelect[i],1,1,m_pFontFile);
				}
			}

			st = vtCharStrings.size();
			for (size_t i=0; i<st; i++){
				fwrite(&vtCharStrings[i],1,1,m_pFontFile);
			}

			if (m_HYCFFInfo.TopDICT.IsCIDFont){
				//fdarray				
				st = vtFArrayData.size();
				for (size_t i=0; i<st; i++){
					fwrite(&vtFArrayData[i],1,1,m_pFontFile);
				}
			}		
			else{
				//private Dict
				st = vtPrivateData.size();
				for (size_t i=0; i<st; i++)	{
					fwrite(&vtPrivateData[i],1,1,m_pFontFile);
				}			
			}			
			
			tbEntry.length = ftell(m_pFontFile) - tbEntry.offset;
			int iTail = 4-tbEntry.length%4;
			if (tbEntry.length%4>0){
				char c = 0;
				for (int i=0; i<iTail; i++){
					fwrite(&c,1,1,m_pFontFile);
				}				
			}
		}	

		return HY_NOERROR;

	}	// end of int	CHYFontCodec::EncodeCFF()

	void CHYFontCodec::PrepareCFFInfo()
	{
		// top dict data
		for (size_t i=0; i<m_HYName.vtNameRecord.size(); i++)
		{
			NAMERECORD& tagNameRecord = m_HYName.vtNameRecord[i];
			// notice
			if (tagNameRecord.platformID==1 && tagNameRecord.encodingID == 0 &&	tagNameRecord.nameID==0)				
			{				
				m_HYCFFInfo.TopDICT.strNotice = tagNameRecord.strContent;
			}
			// FullName
			if (tagNameRecord.platformID==1 && tagNameRecord.encodingID==0 &&tagNameRecord.nameID==4)
			{				
				m_HYCFFInfo.TopDICT.strFullName = tagNameRecord.strContent;
			}
			// FamilyName
			if (tagNameRecord.platformID==1 && tagNameRecord.encodingID==0 &&tagNameRecord.nameID==1)
			{
				m_HYCFFInfo.TopDICT.strFamilyName = tagNameRecord.strContent;
			}
			// CIDFontName
			if (tagNameRecord.platformID==1 && tagNameRecord.encodingID==0 &&tagNameRecord.nameID==6)
			{
				m_HYCFFInfo.TopDICT.strCIDFontName = tagNameRecord.strContent;
			}
			// weight
			if (tagNameRecord.platformID==1 && tagNameRecord.encodingID==0 &&tagNameRecord.nameID==2)
			{
				m_HYCFFInfo.TopDICT.strWeight = tagNameRecord.strContent;
			}
		} 

		m_HYCFFInfo.TopDICT.vtFontBOX.clear();
		m_HYCFFInfo.TopDICT.vtFontBOX.push_back(m_HYhead.xMin);
		m_HYCFFInfo.TopDICT.vtFontBOX.push_back(m_HYhead.yMin);
		m_HYCFFInfo.TopDICT.vtFontBOX.push_back(m_HYhead.xMax);
		m_HYCFFInfo.TopDICT.vtFontBOX.push_back(m_HYhead.yMax);

		m_HYCFFInfo.vtFDArry.clear();
		CHYCFFPrivteDict	privteDict;
		privteDict.vtBlueValues.push_back(-250);
		privteDict.vtBlueValues.push_back(0);
		privteDict.vtBlueValues.push_back(1350);
		privteDict.vtBlueValues.push_back(0);			
		privteDict.fBlueScale = 0.039625;
		privteDict.fStdHW = 25;
		privteDict.fStdVW = 66;		
		privteDict.ldefaultWidthX = 1000;
		privteDict.lnominalWidthX = 607;	
		if (m_HYCFFInfo.TopDICT.IsCIDFont==1)
		{				
			m_HYCFFInfo.TopDICT.CIDCount=(long)m_vtHYGlyphs.size();
			if (m_HYCFFInfo.TopDICT.vtXUID.size() > 2) {
				m_HYCFFInfo.TopDICT.UIDBase = m_HYCFFInfo.TopDICT.vtXUID[2];
			}
			else {
				m_HYCFFInfo.TopDICT.UIDBase = -1;
			}

			m_HYCFFInfo.TopDICT.CIDFontVersion = 1.0;
			//FDSelect
			m_HYCFFInfo.FDSelect.iFormat = 3;		
			CCFFFDSRang3	Rang3;
			Rang3.first		= 0;
			Rang3.fdIndex	= 0;
			m_HYCFFInfo.FDSelect.format3.vtRang3.push_back(Rang3);
			m_HYCFFInfo.FDSelect.format3.sentinel = m_vtHYGlyphs.size();
			//FDArray			
			privteDict.strFontName = m_HYCFFInfo.TopDICT.strCIDFontName;			
			m_HYCFFInfo.vtFDArry.push_back(privteDict);
		}
		else{
			m_HYCFFInfo.PrivteDict = privteDict;
		}

		if (m_HYCFFInfo.TopDICT.IsCIDFont){
			//charset
			m_HYCFFInfo.Charset.Setdefault();
			m_HYCFFInfo.Charset.format = 2;
			CCFFCSFormatRang	formatRang;		
			formatRang.first = 1;
			//-2 是因为第一位 （0位是.notdef的位置，不能计入Charset中，另外，nleft 并不包括first
			formatRang.left =	m_vtHYGlyphs.size()-2;
			m_HYCFFInfo.Charset.format1and2.vtRang.push_back(formatRang);
		}
		else {
			m_HYCFFInfo.Charset.Setdefault();
			m_HYCFFInfo.Charset.format = 1;
			CCFFCSFormatRang	formatRang;
			bool				bIsContinue = false;
			unsigned short		usCounter=0, usPreValue;

			size_t stGlyphNum = m_vtHYGlyphs.size();
			for (size_t i=0; i<stGlyphNum;i++){
				CHYGlyph& Glyph = m_vtHYGlyphs[i];
				if (Glyph.strPostName == ""){
					char  buffer[200] = {0};
					if (Glyph.vtUnicode.size()>0)				
						_stprintf(buffer, "uni%X",Glyph.vtUnicode[0]);
					else 
						_stprintf(buffer, "CID%d",i);

					Glyph.strPostName  =  buffer;
					m_HYCFFInfo.stnStrings.m_szStandString.push_back(Glyph.strPostName);
				}
			}
			
			for (size_t i=1; i<stGlyphNum;)	{
				CHYGlyph& Glyph = m_vtHYGlyphs[i++];
				unsigned short usPostNameID = m_HYCFFInfo.stnStrings.FindNumberByString(Glyph.strPostName);
				if (i==stGlyphNum)
				{
					formatRang.left = ++usCounter;
					if (formatRang.left > 255)
					{
						m_HYCFFInfo.Charset.format = 2;
					}
					m_HYCFFInfo.Charset.format1and2.vtRang.push_back(formatRang);
					formatRang.SetDefault();
				}

				if (!bIsContinue){				
					usPreValue = formatRang.first  = usPostNameID;
					bIsContinue = true;				
				}
				else{
					if (++usPreValue == usPostNameID){
						usCounter++;
					}
					else {				
						formatRang.left = usCounter;					
						m_HYCFFInfo.Charset.format1and2.vtRang.push_back(formatRang);
						formatRang.SetDefault();
						i--;
						usCounter=0;
						bIsContinue = false;
					}
				}
			}
		}

	}	// end of void CHYFontCodec::PrepareCFFInfo()

	int CHYFontCodec::EncodeCFFHead(std::vector<char>& vtHead)
	{	
		//major
		m_HYCFFInfo.Header.major = 1;//(unsigned char)m_HYhead.fontRevision.value;
		vtHead.push_back(static_cast<char>(m_HYCFFInfo.Header.major));
		//minor
		m_HYCFFInfo.Header.minor= 0;//(unsigned char)m_HYhead.fontRevision.fract;
		vtHead.push_back(static_cast<char>(m_HYCFFInfo.Header.minor));
		//hdrSize
		m_HYCFFInfo.Header.hdrSize= 4;
		vtHead.push_back(static_cast<char>(m_HYCFFInfo.Header.hdrSize));
		//offSize
		m_HYCFFInfo.Header.offSize= 4;
		vtHead.push_back(static_cast<char>(m_HYCFFInfo.Header.offSize));

		return HY_NOERROR;

	}	// end of int CHYFontCodec::EncodeCFFHead()

	int CHYFontCodec::EncodeNameIndex(std::vector<char>& vtName)
	{
		std::vector<char>							vtNm;
		std::vector<unsigned int>					vtOffset;
		
		for (size_t i=0; i<m_HYName.vtNameRecord.size(); i++)			
		{
			NAMERECORD& tagNamRecord = m_HYName.vtNameRecord[i];
			if (tagNamRecord.platformID==1 && tagNamRecord.encodingID==0 && tagNamRecord.nameID==6)
			{
				int istrlen = strlen(tagNamRecord.strContent.c_str());
				for (int j=0; j<istrlen; j++)
				{
					vtNm.push_back(tagNamRecord.strContent[j]);
				}

				vtOffset.push_back(1);
				vtOffset.push_back(istrlen+1);
				break;
			}
		}

		EncodeIndexData(vtName,vtNm,vtOffset);

		return HY_NOERROR;

	}	// end of int CHYFontCodec::EncodeNameIndex()

	void CHYFontCodec::DecodeStringIndex()
	{
		if (m_pFontFile)	
		{		
			CHYCFFIndex StringIndx;
			DecodeCFFIndex(StringIndx);

			unsigned long ulDataBegin = ftell(m_pFontFile);
			unsigned char ch;
			unsigned long ulSize=0;
			for(unsigned short i=0; i<StringIndx.Count;i++)
			{
				std::string str;
				ulSize = StringIndx.vtOffset[i+1]-StringIndx.vtOffset[i];		
				for (unsigned long j=0; j<ulSize;j++)
				{
					fread(&ch,1,1,m_pFontFile);
					str.push_back(ch);
				}
				m_HYCFFInfo.stnStrings.m_szStandString.push_back(str);
			}
		}

	}	// end of void CHYFontCodec::DecodeStringIndex()

	int	CHYFontCodec::EncodeStringIndex(std::vector<char>& vtString)
	{
		std::vector<char>			vtStringTmp;
		std::vector<unsigned int>	vtOffset;
		unsigned int				uiStrSize = 0;	
		unsigned int				uiSID = 391;

		size_t stNewString = m_HYCFFInfo.stnStrings.m_szStandString.size();
		if(m_HYCFFInfo.TopDICT.IsCIDFont==1){
			//Registry
			m_HYCFFInfo.TopDICT.Ros.RegistrySID = m_HYCFFInfo.stnStrings.InsertString(m_HYCFFInfo.TopDICT.Ros.strRegistry);
			//Ordering
			m_HYCFFInfo.TopDICT.Ros.OrderingSID = m_HYCFFInfo.stnStrings.InsertString(m_HYCFFInfo.TopDICT.Ros.strOrdering);
		}
		//version
		if (m_HYCFFInfo.TopDICT.strVersion.size() != 0){
			m_HYCFFInfo.TopDICT.iVersionSID = m_HYCFFInfo.stnStrings.InsertString(m_HYCFFInfo.TopDICT.strVersion);				
		}		
		// Notice
		if (m_HYCFFInfo.TopDICT.strNotice.size() != 0){
			m_HYCFFInfo.TopDICT.iNoticeSID = m_HYCFFInfo.stnStrings.InsertString(m_HYCFFInfo.TopDICT.strNotice);
		}		
		//copyright
		if (m_HYCFFInfo.TopDICT.strCopyright.size() != 0){
			m_HYCFFInfo.TopDICT.iCopyrghtSID = m_HYCFFInfo.stnStrings.InsertString(m_HYCFFInfo.TopDICT.strCopyright);
		}
		//FullName
		if (m_HYCFFInfo.TopDICT.strFullName.size() != 0){
			m_HYCFFInfo.TopDICT.iFullNameSID = m_HYCFFInfo.stnStrings.InsertString(m_HYCFFInfo.TopDICT.strFullName);
		}		
		//FamilyName
		if (m_HYCFFInfo.TopDICT.strFamilyName.size() != 0){
			m_HYCFFInfo.TopDICT.iFamilyNameSID = m_HYCFFInfo.stnStrings.InsertString(m_HYCFFInfo.TopDICT.strFamilyName);
		}		
		//weight
		if (m_HYCFFInfo.TopDICT.strWeight.size() != 0){
			m_HYCFFInfo.TopDICT.iWeightSID = m_HYCFFInfo.stnStrings.InsertString(m_HYCFFInfo.TopDICT.strWeight);
		}		
		//postscript
		if (m_HYCFFInfo.TopDICT.strPostSript.size() != 0){
			m_HYCFFInfo.TopDICT.iPostSriptSID = m_HYCFFInfo.stnStrings.InsertString(m_HYCFFInfo.TopDICT.strPostSript);
		}
		//BaseFontName
		if (m_HYCFFInfo.TopDICT.strBaseFontName.size() != 0){
			m_HYCFFInfo.TopDICT.iBaseFontNameSID = m_HYCFFInfo.stnStrings.InsertString(m_HYCFFInfo.TopDICT.strBaseFontName);
		}		
		//FDARRAY
		for (size_t i=0; i<m_HYCFFInfo.vtFDArry.size(); i++){			
			if (m_HYCFFInfo.vtFDArry[i].strFontName.size()!= 0){
				m_HYCFFInfo.vtFDArry[i].iFontNameID =  m_HYCFFInfo.stnStrings.InsertString(m_HYCFFInfo.vtFDArry[i].strFontName);		
			}			
		}

		vtOffset.push_back(1);
		for (size_t i=391; i<m_HYCFFInfo.stnStrings.m_szStandString.size(); i++){
			uiStrSize = PutStringToVectorChar(vtStringTmp,m_HYCFFInfo.stnStrings.m_szStandString[i]);
			if (uiStrSize>0){
				vtOffset.push_back(vtStringTmp.size()+1);				
			}
		}
		EncodeIndexData(vtString,vtStringTmp,vtOffset);

		return HY_NOERROR;

	}	// end of int	CHYFontCodec::EncodeStringIndex()

	int	CHYFontCodec::EncodeGlobalIndex(std::vector<char>& vtGlobal)
	{
		char cTmp = 0;
		CHYCFFIndex& Global = m_HYCFFInfo.globalSubIndex;

		//count
		vtGlobal.push_back(static_cast<char>(Global.Count>>8));
		vtGlobal.push_back(static_cast<char>(Global.Count));
		if (Global.Count>0)
		{
			//OffSize
			vtGlobal.push_back(Global.OffSize);
			size_t st = Global.vtOffset.size();
			for (size_t i=0; i<st; i++)
			{
				unsigned long& ulTmp = Global.vtOffset[i];
				if(Global.OffSize==1)
				{
					cTmp  = ulTmp&0x000000ff;				
					vtGlobal.push_back(cTmp);
				}
				if (Global.OffSize==2)
				{
					cTmp = (ulTmp&0x0000ff00)>>8;
					vtGlobal.push_back(cTmp);
					cTmp = (ulTmp&0x000000ff);				
					vtGlobal.push_back(cTmp);
				}
				if (Global.OffSize==3)
				{
					cTmp = (ulTmp&0x00ff0000)>>16;				
					vtGlobal.push_back(cTmp);
					cTmp = (ulTmp&0x0000ff00)>>8;				
					vtGlobal.push_back(cTmp);
					cTmp = (ulTmp&0x000000ff);				
					vtGlobal.push_back(cTmp);
				}
				if (Global.OffSize==4)
				{
					cTmp = ulTmp>>24;
					vtGlobal.push_back(cTmp);
					cTmp = (ulTmp&0x00ff0000)>>16;
					vtGlobal.push_back(cTmp);
					cTmp = (ulTmp&0x0000ff00)>>8;
					vtGlobal.push_back(cTmp);
					cTmp = (ulTmp&0x000000ff);
					vtGlobal.push_back(cTmp);
				}
			}
			size_t szInData = Global.vtData.size();
			for (size_t i=0; i<szInData; i++)
			{
				vtGlobal.push_back(Global.vtData[i]);
			}
		}		

		return HY_NOERROR;

	}	// end of int	CHYFontCodec::EncodeGlobalIndex()

	int CHYFontCodec::EncodeTopDictIndex(std::vector<char>& vtTopDict)
	{			
		std::vector<char> vtTmpData;
		CHYCFFTopDict& TopDict = m_HYCFFInfo.TopDICT;
		std::vector<unsigned int>	vtOffset;
		vtOffset.push_back(1);

		if (TopDict.IsCIDFont==1)
		{
			//ROS
			EncodeDICTInteger(TopDict.Ros.RegistrySID,vtTmpData);
			EncodeDICTInteger(TopDict.Ros.OrderingSID,vtTmpData);
			EncodeDICTInteger(TopDict.Ros.Supplement,vtTmpData);
			vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_ROS>>8));
			vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_ROS));
		}

		//version
		if (TopDict.iVersionSID!=0)
		{
			EncodeDICTInteger(TopDict.iVersionSID,vtTmpData);
			vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_VERSION));
		}
		//Notice
		if (TopDict.iNoticeSID!=0)
		{
			EncodeDICTInteger(TopDict.iNoticeSID,vtTmpData);
			vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_NOTICE));
		}
		//Copyright
		if (TopDict.iCopyrghtSID!=0)
		{
			EncodeDICTInteger(TopDict.iCopyrghtSID,vtTmpData);
			vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_COPYRIGHT>>8));
			vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_COPYRIGHT));
		}
		//FullName
		if (TopDict.iFullNameSID!=0)
		{
			EncodeDICTInteger(TopDict.iFullNameSID,vtTmpData);			
			vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_FULLNAME));
		}
		//FamilyName
		if (TopDict.iFamilyNameSID!=0)
		{
			EncodeDICTInteger(TopDict.iFamilyNameSID,vtTmpData);			
			vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_FAMILYNAME));
		}
		//Weight	
		EncodeDICTInteger(TopDict.iWeightSID,vtTmpData);			
		vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_WEIGHT));
		
		//isFixedPitch		
		if (TopDict.isFixedPitch!=0)
		{
			EncodeDICTInteger(TopDict.isFixedPitch,vtTmpData);			
			vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_ISFIXEDPITCH>>8));	
			vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_ISFIXEDPITCH));		
		}		
		//ItalicAngle		
		if (TopDict.ItalicAngle!=0)
		{
			EncodeDICTInteger(static_cast<long>(TopDict.ItalicAngle),vtTmpData);			
			vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_ITALICANGLE>>8));
			vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_ITALICANGLE));
		}
		//UnderlinePosition
		if (TopDict.UnderlinePosition != -100.0f)
		{
			EncodeDICTInteger(static_cast<long>(TopDict.UnderlinePosition),vtTmpData);
			vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_UNDERLINEPOSITION>>8));
			vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_UNDERLINEPOSITION));
		}		
		//UnderlineThickness
		if (TopDict.UnderlineThickness != 50.0f)
		{	
			EncodeDICTInteger(static_cast<long>(TopDict.UnderlineThickness),vtTmpData);
			vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_UNDERLINETHICKNESS>>8));
			vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_UNDERLINETHICKNESS));
		}	

		//PaintType
		if (TopDict.PaintType!=0)
		{
			EncodeDICTInteger(TopDict.PaintType,vtTmpData);
			vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_PAINTTYPE>>8));
			vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_PAINTTYPE));
		}
		// charstringtype
		if (TopDict.CharStringType != 2)
		{
			EncodeDICTInteger(static_cast<long>(TopDict.CharStringType),vtTmpData);
			vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_CHARSTRINGTYPE>>8));
			vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_CHARSTRINGTYPE));
		}
		//FontMatrix
		size_t st = TopDict.vtFontMatrix.size();
		if (st>0)
		{
			for (size_t i=0; i<st; i++)
			{
				EncodeDICTReal(TopDict.vtFontMatrix[i],vtTmpData);
			}			
			vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_FONTMATRIX>>8));
			vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_FONTMATRIX));	
		}

		//UniqueID
		if(TopDict.UniqueID!=-1)
		{
			EncodeDICTInteger(TopDict.UniqueID,vtTmpData);
			vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_UNIQUEID));
		}

		//FontBBox
		st = TopDict.vtFontBOX.size(); 
		if (st>0)
		{
			for (size_t i=0; i<st; i++)
			{
				EncodeDICTInteger(static_cast<long>(TopDict.vtFontBOX[i]),vtTmpData);
			}			
			vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_FONTBBOX));		
		}
		//StrokeWidth
		if (TopDict.strokeWidth>0)
		{
			EncodeDICTInteger(static_cast<long>(TopDict.strokeWidth),vtTmpData);
			vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_STROKEWIDTH>>8));		
			vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_STROKEWIDTH));
		}
		//XUID
		st=TopDict.vtXUID.size();
		if (st>0){
			for (size_t i=0;i<st;i++){
				EncodeDICTInteger(TopDict.vtXUID[i],vtTmpData);
			}						
			vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_XUID));
		}
		//charset
		EncodeDICTMaxInteger(TopDict.charsetOffset,vtTmpData);
		vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_CHARSET));			
		
		//Encoding	
		//EncodeDICTMaxInteger(TopDict.encodingOffset,vtTmpData);
		//vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_ENCODING));
		
		//CharStrings		
		EncodeDICTMaxInteger(TopDict.charStringOffset,vtTmpData);
		vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_CHARSTRINGS));
		
		if (TopDict.IsCIDFont==0){
			EncodeDICTMaxInteger(TopDict.PrivteDictSize,vtTmpData);	
			EncodeDICTMaxInteger(TopDict.PrivteOffset,vtTmpData);
			vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_PRIVATE));
		}
		//SyntheticBase
		if (TopDict.SyntheticBaseIndex!=0){
			EncodeDICTInteger(TopDict.SyntheticBaseIndex,vtTmpData);
			vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_SYNTHETICBASE>>8));
			vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_SYNTHETICBASE));
		}
		//PostScript
		if (TopDict.iPostSriptSID!=0){
			EncodeDICTInteger(TopDict.iPostSriptSID,vtTmpData);
			vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_POSTSCRIPT>>8));
			vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_POSTSCRIPT));
		}
		//BaseFontName
		if (TopDict.iBaseFontNameSID!=0){
			EncodeDICTInteger(TopDict.iPostSriptSID,vtTmpData);
			vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_BASEFONTNAME>>8));
			vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_BASEFONTNAME));
		}		
		//BaseFontBlend	
		st=TopDict.vtBaseFontBlend.size();
		if (st>0){
			for(size_t i=0; i<st; i++){
				EncodeDICTReal(TopDict.vtBaseFontBlend[i],vtTmpData);
			}
			vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_BASEFONTBLEND>>8));
			vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_BASEFONTBLEND));
		}

		if (TopDict.IsCIDFont==1){
			//ROS
//			EncodeDICTInteger(TopDict.Ros.RegistrySID,vtTmpData);
//			EncodeDICTInteger(TopDict.Ros.OrderingSID,vtTmpData);
//			EncodeDICTInteger(TopDict.Ros.Supplement,vtTmpData);
//			vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_ROS>>8));
//			vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_ROS));
			//CIDFontVersion
			if (TopDict.CIDFontVersion!=0)
			{
				EncodeDICTReal(TopDict.CIDFontVersion,vtTmpData);
				vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_CIDFONTVERSION>>8));
				vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_CIDFONTVERSION));
			}
			//CIDFontVersion
			if (TopDict.CIDFontRevision!=0)
			{
				EncodeDICTInteger(TopDict.CIDFontRevision,vtTmpData);
				vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_CIDFONTREVISION>>8));
				vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_CIDFONTREVISION));
			}
			//CIDFontType
			if (TopDict.CIDFontType!=0)
			{
				EncodeDICTInteger(TopDict.CIDFontType,vtTmpData);
				vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_CIDFONTTYPE>>8));
				vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_CIDFONTTYPE));
			}
			//CIDCount
			if (TopDict.CIDCount!=8720)
			{
				EncodeDICTInteger(TopDict.CIDCount,vtTmpData);
				vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_CIDCOUNT>>8));
				vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_CIDCOUNT));
			}

			EncodeDICTMaxInteger(TopDict.FDArryIndexOffset,vtTmpData);
			vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_FDARRAY>>8));
			vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_FDARRAY));
			
			EncodeDICTMaxInteger(TopDict.FDSelectOffset,vtTmpData);
			vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_FDSELECT>>8));
			vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_FDSELECT));

			//FontName
			if (TopDict.CIDFontNameSID!=0)
			{
				EncodeDICTInteger(TopDict.CIDFontNameSID,vtTmpData);
				vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_FONTNAME>>8));
				vtTmpData.push_back(static_cast<char>(CFF_DICT_OPERATOR_FONTNAME));
			}
		}

		vtOffset.push_back(vtTmpData.size()+1);
		EncodeIndexData(vtTopDict,vtTmpData,vtOffset);

		return HY_NOERROR;

	}	// end of int CHYFontCodec::EncodeTopDictIndex()

	int CHYFontCodec::EncodeIndexData(std::vector<char>& vtOutData,std::vector<char>& vtInData, std::vector<unsigned int>& vtInOffset)
	{
		unsigned short usTmp = 0;
		unsigned long ulTmp = 0;
		char cTmp = 0;
		
		//count
		usTmp = (unsigned short)vtInOffset.size();
		if (usTmp==0) return FONT_ERR_CFF_ENCODE;

		usTmp-=1;
		vtOutData.push_back(static_cast<char>(usTmp>>8));
		vtOutData.push_back(static_cast<char>(usTmp));

		if(usTmp>0){
			char offSize=0;
			ulTmp = vtInOffset[vtInOffset.size()-1];
			if (ulTmp &0x000000ff) offSize = 1;
			if (ulTmp &0x0000ff00) offSize = 2;
			if (ulTmp &0x00ff0000) offSize = 3;
			if (ulTmp &0xff000000) offSize = 4;
			//offSize		
			vtOutData.push_back(offSize);
			//offset
			for (size_t i=0; i<vtInOffset.size(); i++){
				unsigned int ulTmp = vtInOffset[i];
				if(offSize==1){
					cTmp  = ulTmp&0x000000ff;				
					vtOutData.push_back(cTmp);
				}
				if (offSize==2){
					cTmp = (ulTmp&0x0000ff00)>>8;
					vtOutData.push_back(cTmp);
					cTmp = (ulTmp&0x000000ff);				
					vtOutData.push_back(cTmp);
				}
				if (offSize==3){
					cTmp = (ulTmp&0x00ff0000)>>16;				
					vtOutData.push_back(cTmp);
					cTmp = (ulTmp&0x0000ff00)>>8;				
					vtOutData.push_back(cTmp);
					cTmp = (ulTmp&0x000000ff);				
					vtOutData.push_back(cTmp);
				}
				if (offSize==4){
					cTmp = ulTmp>>24;
					vtOutData.push_back(cTmp);
					cTmp = (ulTmp&0x00ff0000)>>16;
					vtOutData.push_back(cTmp);
					cTmp = (ulTmp&0x0000ff00)>>8;
					vtOutData.push_back(cTmp);
					cTmp = (ulTmp&0x000000ff);
					vtOutData.push_back(cTmp);
				}
			}

			size_t szInData = vtInData.size();
			for (size_t i=0; i<szInData; i++){
				vtOutData.push_back(vtInData[i]);
			}
		}

		return HY_NOERROR;

	}	// end of int CHYFontCodec::EncodeIndexData()

	int CHYFontCodec::EncodeFDArray(std::vector<char>& vtFDArrayData, size_t stOffset)
	{
		size_t stPrivateDICT = m_HYCFFInfo.vtFDArry.size();
		std::vector<unsigned int>		vtOffset;
		std::vector<char>				vtTmpData;
		
		vtOffset.push_back(0);		
		for (size_t i=0; i<stPrivateDICT; i++)
		{
			CHYCFFPrivteDict& cffPrivteDict = m_HYCFFInfo.vtFDArry[i];
			EncodePrivateDICTData(vtTmpData,cffPrivteDict);			
			vtOffset.push_back(vtTmpData.size());
		}
		
		std::vector<unsigned int>		vtFDArryOffset;
		std::vector<char>				vtFDArrayDataTmp;		
		vtFDArryOffset.push_back(1);
		for (size_t i=0; i<stPrivateDICT;i++)
		{		
			CHYCFFPrivteDict& cffPrivteDict = m_HYCFFInfo.vtFDArry[i];
			// FontSet Name ID
			EncodeDICTInteger(cffPrivteDict.iFontNameID,vtFDArrayDataTmp);
			vtFDArrayDataTmp.push_back(static_cast<char>(CFF_DICT_OPERATOR_FONTNAME>>8));
			vtFDArrayDataTmp.push_back(static_cast<char>(CFF_DICT_OPERATOR_FONTNAME));

			// PriuvateDict	size			
			unsigned long iSize = vtOffset[i+1]-vtOffset[i];			
			//EncodeDICTMaxInteger(iSize,vtFDArrayDataTmp);
			EncodeDICTInteger(iSize,vtFDArrayDataTmp);
			// PriuvateDict	offset
			EncodeDICTMaxInteger(vtOffset[i],vtFDArrayDataTmp);
			vtFDArrayDataTmp.push_back(static_cast<char>(CFF_DICT_OPERATOR_PRIVATE));
			vtFDArryOffset.push_back(vtFDArrayDataTmp.size()+1);
		}
		EncodeIndexData(vtFDArrayData,vtFDArrayDataTmp,vtFDArryOffset);

		size_t stFDOffset = stOffset+vtFDArrayData.size();
		vtFDArrayDataTmp.clear();
		vtFDArryOffset.clear();
		vtFDArrayData.clear();
		vtFDArryOffset.push_back(1);
		for (size_t i=0; i<stPrivateDICT;i++)
		{		
			CHYCFFPrivteDict& cffPrivteDict = m_HYCFFInfo.vtFDArry[i];
			// FontSet Name ID
			EncodeDICTInteger(cffPrivteDict.iFontNameID,vtFDArrayDataTmp);
			vtFDArrayDataTmp.push_back(static_cast<char>(CFF_DICT_OPERATOR_FONTNAME>>8));
			vtFDArrayDataTmp.push_back(static_cast<char>(CFF_DICT_OPERATOR_FONTNAME));

			// PriuvateDict	size			
			unsigned long iSize = vtOffset[i+1]-vtOffset[i];			
			EncodeDICTInteger(iSize,vtFDArrayDataTmp);
			// PriuvateDict	offset
			EncodeDICTMaxInteger(vtOffset[i]+stFDOffset,vtFDArrayDataTmp);
			vtFDArrayDataTmp.push_back(static_cast<char>(CFF_DICT_OPERATOR_PRIVATE));
			vtFDArryOffset.push_back(vtFDArrayDataTmp.size()+1);
		}
		EncodeIndexData(vtFDArrayData,vtFDArrayDataTmp,vtFDArryOffset);

		size_t szTmpData = vtTmpData.size();
		for (size_t i=0; i<szTmpData;i++)
		{
			vtFDArrayData.push_back(vtTmpData[i]);
		}
		return HY_NOERROR;

	}	// end of int CHYFontCodec::EncodeFDArray()

	void CHYFontCodec::DecodeCharSets(unsigned long  ulOffset)
	{
		if (m_pFontFile)
		{
			fseek(m_pFontFile,ulOffset,SEEK_SET);
			unsigned char uch;
			fread(&uch,1,1,m_pFontFile);

			size_t			i = 0;
			unsigned short	nTmp = 0;
			BYTE			btmp = 0;

			m_HYCFFInfo.Charset.format = uch;
			if (uch == 0)
			{
				size_t st = m_vtHYGlyphs.size()-1;
				for (i=0; i<st; i++)
				{
					fread(&nTmp,2,1,m_pFontFile);
					nTmp = hy_cdr_int16_to(nTmp);
					m_HYCFFInfo.Charset.vtFoamat0SID.push_back(nTmp);
				}				
			}

			if (uch == 1)
			{
				size_t stGlyphNum = m_vtHYGlyphs.size()-1;
				size_t CharSetNum = 0;				
				while (CharSetNum<stGlyphNum)
				{
					CCFFCSFormatRang csr1;
					fread(&nTmp,2,1,m_pFontFile);
					nTmp = hy_cdr_int16_to(nTmp);					
					fread(&btmp,1,1,m_pFontFile);
					csr1.first = nTmp;
					csr1.left = btmp;
					m_HYCFFInfo.Charset.format1and2.vtRang.push_back(csr1);
					CharSetNum+=btmp+1;
				}
			}

			if (uch==2)
			{
				size_t stGlyphNum = m_vtHYGlyphs.size()-1;
				size_t CharSetNum = 0;				
				while (CharSetNum<stGlyphNum)
				{
					CCFFCSFormatRang csr2;
					fread(&nTmp,2,1,m_pFontFile);
					nTmp = hy_cdr_int16_to(nTmp);
					csr2.first = nTmp;					
					fread(&nTmp,2,1,m_pFontFile);
					nTmp = hy_cdr_int16_to(nTmp);					
					csr2.left = nTmp;
					m_HYCFFInfo.Charset.format1and2.vtRang.push_back(csr2);
					CharSetNum+=nTmp+1;
				}	
			}
		}

	}	// end of void CHYFontCodec::DecodeCharSets()

	int	CHYFontCodec::EncodeCharSets(std::vector<char>& vtCharSetData)
	{			
		size_t sz = 0;
		// format
		vtCharSetData.push_back(m_HYCFFInfo.Charset.format);

		// format 0
		if (m_HYCFFInfo.Charset.format==0)
		{
			sz = m_HYCFFInfo.Charset.vtFoamat0SID.size();
			for (size_t i=0; i<sz; i++)
			{
				unsigned short unTmp = m_HYCFFInfo.Charset.vtFoamat0SID[i];
				vtCharSetData.push_back(static_cast<char>(unTmp>>8));
				vtCharSetData.push_back(static_cast<char>(unTmp));
			}
		}

		// format 1
		if (m_HYCFFInfo.Charset.format==1)
		{
			sz = m_HYCFFInfo.Charset.format1and2.vtRang.size();
			for (size_t i=0; i<sz; i++)
			{
				unsigned short unTmp = m_HYCFFInfo.Charset.format1and2.vtRang[i].first;
				vtCharSetData.push_back(static_cast<char>(unTmp>>8));
				vtCharSetData.push_back(static_cast<char>(unTmp));

				vtCharSetData.push_back(static_cast<char>(m_HYCFFInfo.Charset.format1and2.vtRang[i].left));
			}
		}

		// format 2
		if (m_HYCFFInfo.Charset.format==2)
		{
			sz = m_HYCFFInfo.Charset.format1and2.vtRang.size();
			for (size_t i=0; i<sz; i++)
			{
				unsigned short unTmp = m_HYCFFInfo.Charset.format1and2.vtRang[i].first;
				vtCharSetData.push_back(static_cast<char>(unTmp>>8));
				vtCharSetData.push_back(static_cast<char>(unTmp));

				unTmp = m_HYCFFInfo.Charset.format1and2.vtRang[i].left;
				vtCharSetData.push_back(static_cast<char>(unTmp>>8));
				vtCharSetData.push_back(static_cast<char>(unTmp));				
			}
		}

		return HY_NOERROR;

	}	// end of int	CHYFontCodec::EncodeCharSets()

	int CHYFontCodec::EncodeCharStrings(std::vector<char>& vtCharString)
	{
		std::vector<char>	cCharStringTmp;
		std::vector<size_t>	cCharStringOffset;
		size_t st=m_vtHYGlyphs.size();

		cCharStringOffset.push_back(1);
		for (size_t i=0; i<st; i++)
		{	
			CHYGlyph& Glyph = m_vtHYGlyphs[i];

			/*
			if (Glyph.vtUnicode.size()>0)
			{
				if(Glyph.vtUnicode[0] == 0x0024)
				{
					int ijinjunTest =0;
				}
			}
			*/

			if (m_HYCFFInfo.TopDICT.IsCIDFont)
			{
				int iFDIndex = GetFDIndex(i);
				if (iFDIndex!=-1)
				{
					CHYCFFPrivteDict& CFFPrivteDict = m_HYCFFInfo.vtFDArry[iFDIndex];
					EncodeCharString(cCharStringTmp,CFFPrivteDict,Glyph);
					cCharStringOffset.push_back(cCharStringTmp.size()+1);
				}
			}
			else 
			{
				CHYCFFPrivteDict& CFFPrivteDict = m_HYCFFInfo.PrivteDict;
				EncodeCharString(cCharStringTmp,CFFPrivteDict,Glyph);
				cCharStringOffset.push_back(cCharStringTmp.size()+1);
			}
		}
		EncodeIndexData(vtCharString,cCharStringTmp,cCharStringOffset);

		return HY_NOERROR;

	}	// end of int CHYFontCodec::EncodeCharStrings()

	int	CHYFontCodec::EncodeCharString(std::vector<char>& vtCharSData,CHYCFFPrivteDict& PrivteDict, CHYGlyph& Glyph)
	{
		long lTmp = 0;
		if (Glyph.advanceWidth!=PrivteDict.ldefaultWidthX)
		{
			lTmp=Glyph.advanceWidth-PrivteDict.lnominalWidthX;
			EncodeDICTInteger(lTmp,vtCharSData);
		}

		CHYPoint			PrevHYPoint;
		size_t				szCntuslst=Glyph.vtContour.size();		
		int					iDirection=TYPE2_LINE_DIRECTION_0;
		int					iOperatorFlag=0;
		CCharStringParam	CharParam;

		for (size_t i=0; i<szCntuslst; i++)
		{
			CHYContour& cntu = Glyph.vtContour[i];
			size_t szPoints = cntu.vtHYPoints.size();
			size_t x=0, ArgumentStackSize;
			short sX = 0, sY = 0;
			while(x<szPoints)
			{
				ArgumentStackSize = 0;
				iDirection = TYPE2_CURVE_DIRECTION_0;
				iOperatorFlag = TYPE2_CHARSTING_OPERATOR_UNKNOW;

				CHYPoint& HyPnt	= cntu.vtHYPoints[x];
				if (x==0)
				{
					sX = HyPnt.x-PrevHYPoint.x;
					sY = HyPnt.y-PrevHYPoint.y;

					if (sX!=0 && sY!=0)
					{
						// x					
						EncodeDICTInteger(sX,vtCharSData);
						// y					
						EncodeDICTInteger(sY,vtCharSData);						
						// operator
						vtCharSData.push_back(TYPE2_CHARSTING_OPERATOR_RMOVETO);
					}
					else if (sX == 0 )
					{
						// y						
						EncodeDICTInteger(sY,vtCharSData);						
						// operator						
						vtCharSData.push_back(TYPE2_CHARSTING_OPERATOR_VMOVETO);
					}
					else if (sY == 0)
					{
						// X					
						EncodeDICTInteger(sX,vtCharSData);						
						// operator
						vtCharSData.push_back(TYPE2_CHARSTING_OPERATOR_HMOVETO);						
					}

					PrevHYPoint.x		= HyPnt.x;
					PrevHYPoint.y		= HyPnt.y;
					PrevHYPoint.flag	= HyPnt.flag;

					x++;
				}
				else if(HyPnt.flag == POINT_FLG_ANCHOR)
				{
					EncodeLineToData(vtCharSData,PrevHYPoint,cntu.vtHYPoints,x,iOperatorFlag,iDirection,ArgumentStackSize);					
					vtCharSData.push_back(static_cast<char>(iOperatorFlag));
				}
				else if (HyPnt.flag == POINT_FLG_CONTROL)
				{
					EncodeCurveToData(vtCharSData,PrevHYPoint,cntu.vtHYPoints,x,iOperatorFlag,iDirection,ArgumentStackSize);
					vtCharSData.push_back(static_cast<char>(iOperatorFlag));					
				}
				else // 防止出现 flag 随机值的问题出现
				{
					x++;
				}				
			}
		}
		// operator		
		vtCharSData.push_back(TYPE2_CHARSTING_OPERATOR_ENDCHAR);

		return HY_NOERROR;

	}	// end of int	CHYFontCodec::EncodeCharString()

	int	 CHYFontCodec::EncodeLineToData(std::vector<char>& vtData, CHYPoint& PrevHYPoint, std::vector<CHYPoint>& vtHYPoints, size_t& iPointIndex, int& iOperatorFlag, int& iDirection,size_t& iArgumentStackSize)
	{
		short	sX = 0, sY = 0;
		long	lTmp = 0;

		size_t stPnts = vtHYPoints.size();
		if (stPnts<=iPointIndex ) return HY_NOERROR;
		
		CHYPoint& HyPnt = vtHYPoints[iPointIndex];
		sX = HyPnt.x-PrevHYPoint.x;
		sY = HyPnt.y-PrevHYPoint.y;	

		if (HyPnt.flag == POINT_FLG_CONTROL)
		{
			if (iOperatorFlag == TYPE2_CHARSTING_OPERATOR_RLINETO)
			{
				if (iArgumentStackSize+6 > TYPE2_ARGUMENT_STACK)	return HY_NOERROR;
				if(iPointIndex>=stPnts-3)							return HY_NOERROR;

				// 第一个点
				sX = HyPnt.x - PrevHYPoint.x;
				sY = HyPnt.y - PrevHYPoint.y;
				// x			
				EncodeDICTInteger(sX,vtData);				
				// y			
				EncodeDICTInteger(sY,vtData);				
				iPointIndex++;
				// 第二个点
				CHYPoint& HyPnt2 = vtHYPoints[iPointIndex];
				sX = HyPnt2.x - HyPnt.x;
				sY = HyPnt2.y - HyPnt.y;
				// x				
				EncodeDICTInteger(sX, vtData);
				// y				
				EncodeDICTInteger(sY, vtData);				
				iPointIndex++;
				// 第三个点
				CHYPoint& HyPnt3 = vtHYPoints[iPointIndex];
				sX = HyPnt3.x-HyPnt2.x;
				sY = HyPnt3.y-HyPnt2.y;
				// x			
				EncodeDICTInteger(sX,vtData);				
				// y			
				EncodeDICTInteger(sY,vtData);				
				iPointIndex++;
				// operator
				iOperatorFlag =	TYPE2_CHARSTING_OPERATOR_RLINECURVE;

				PrevHYPoint.x=HyPnt3.x;
				PrevHYPoint.y=HyPnt3.y;
				PrevHYPoint.flag=HyPnt3.flag;

				return TYPE2_LINE_END;
			}
			return HY_NOERROR;
		}
		if (iOperatorFlag == TYPE2_CHARSTING_OPERATOR_UNKNOW)
		{
			// RLINTTO
			if (sX!=0 && sY!=0 ) 
			{
				if (iArgumentStackSize+2 > TYPE2_ARGUMENT_STACK)	return HY_NOERROR;

				// x			
				EncodeDICTInteger(sX, vtData);				
				iArgumentStackSize++;
				// y			
				EncodeDICTInteger(sY, vtData);				
				PrevHYPoint.x = HyPnt.x;
				PrevHYPoint.y = HyPnt.y;
				PrevHYPoint.flag = HyPnt.flag;
				iArgumentStackSize++;
				iOperatorFlag = TYPE2_CHARSTING_OPERATOR_RLINETO;
				iDirection = TYPE2_LINE_DIRECTION_0;

				EncodeLineToData(vtData,PrevHYPoint,vtHYPoints,++iPointIndex,iOperatorFlag,iDirection,iArgumentStackSize);
			}
			else if (sX == 0) // x轴相同, Y轴方向移动
			{
				if (iArgumentStackSize+1 > TYPE2_ARGUMENT_STACK)	return HY_NOERROR;			
				
				EncodeDICTInteger(sY, vtData);
				PrevHYPoint.x = HyPnt.x;
				PrevHYPoint.y = HyPnt.y;
				PrevHYPoint.flag = HyPnt.flag;
				iOperatorFlag = TYPE2_CHARSTING_OPERATOR_VLINETO;
				iDirection = TYPE2_LINE_DIRECTION_V;
				iArgumentStackSize++;

				EncodeLineToData(vtData,PrevHYPoint,vtHYPoints,++iPointIndex,iOperatorFlag,iDirection,iArgumentStackSize);
			}
			else if (sY == 0) // Y轴相同,X轴方向移动
			{
				if (iArgumentStackSize+1 > TYPE2_ARGUMENT_STACK)	return HY_NOERROR;
				
				EncodeDICTInteger(sX,vtData);		
				PrevHYPoint.x = HyPnt.x;
				PrevHYPoint.y = HyPnt.y;
				PrevHYPoint.flag = HyPnt.flag;							
				iOperatorFlag =TYPE2_CHARSTING_OPERATOR_HLINETO;
				iDirection = TYPE2_LINE_DIRECTION_H;
				iArgumentStackSize++;

				EncodeLineToData(vtData,PrevHYPoint,vtHYPoints, ++iPointIndex, iOperatorFlag, iDirection,iArgumentStackSize);
			}
		}
		else if (iOperatorFlag == TYPE2_CHARSTING_OPERATOR_RLINETO)
		{
			if (sX==0 || sY==0 ) return HY_NOERROR;
			if (iArgumentStackSize+2 > TYPE2_ARGUMENT_STACK)	return HY_NOERROR;

			// x			
			EncodeDICTInteger(sX,vtData);			
			iArgumentStackSize++;
			// y			
			EncodeDICTInteger(sY,vtData);			
			PrevHYPoint.x = HyPnt.x;
			PrevHYPoint.y = HyPnt.y;
			PrevHYPoint.flag = HyPnt.flag;
			iArgumentStackSize++;
			iOperatorFlag =	TYPE2_CHARSTING_OPERATOR_RLINETO;
			iDirection = TYPE2_LINE_DIRECTION_0;

			EncodeLineToData(vtData,PrevHYPoint,vtHYPoints, ++iPointIndex, iOperatorFlag, iDirection,iArgumentStackSize); 
		}
		else if (iOperatorFlag == TYPE2_CHARSTING_OPERATOR_HLINETO)
		{	
			if (iArgumentStackSize+1 > TYPE2_ARGUMENT_STACK)	return HY_NOERROR;
			if (iDirection == TYPE2_LINE_DIRECTION_H)
			{
				if (sX != 0) return HY_NOERROR;
				EncodeDICTInteger(sY, vtData);
				PrevHYPoint.x = HyPnt.x;
				PrevHYPoint.y = HyPnt.y;
				PrevHYPoint.flag = HyPnt.flag;
				iDirection = TYPE2_LINE_DIRECTION_V;
				iArgumentStackSize++;

				EncodeLineToData(vtData,PrevHYPoint,vtHYPoints,++iPointIndex, iOperatorFlag, iDirection,iArgumentStackSize);
			}
			else if (iDirection == TYPE2_LINE_DIRECTION_V) 
			{
				if (sY != 0 ) return HY_NOERROR;

				EncodeDICTInteger(sX, vtData);				
				PrevHYPoint.x = HyPnt.x;
				PrevHYPoint.y = HyPnt.y;
				PrevHYPoint.flag = HyPnt.flag;
				iDirection = TYPE2_LINE_DIRECTION_H;
				iArgumentStackSize++;

				EncodeLineToData(vtData,PrevHYPoint,vtHYPoints,++iPointIndex,iOperatorFlag,iDirection,iArgumentStackSize);
			}
		}
		else if (iOperatorFlag == TYPE2_CHARSTING_OPERATOR_VLINETO)
		{
			if (iArgumentStackSize+1 > TYPE2_ARGUMENT_STACK)	return HY_NOERROR;
			if (iDirection == TYPE2_LINE_DIRECTION_V) 
			{
				if (sY != 0 ) return HY_NOERROR;

				EncodeDICTInteger(sX,vtData);
				PrevHYPoint.x = HyPnt.x;
				PrevHYPoint.y = HyPnt.y;
				PrevHYPoint.flag = HyPnt.flag;				
				iDirection = TYPE2_LINE_DIRECTION_H;
				iArgumentStackSize++;

				EncodeLineToData(vtData,PrevHYPoint,vtHYPoints,++iPointIndex,iOperatorFlag,iDirection,iArgumentStackSize);
			}
			else if (iDirection == TYPE2_LINE_DIRECTION_H)
			{
				if (sX != 0) return HY_NOERROR;

				EncodeDICTInteger(sY,vtData);
				PrevHYPoint.x = HyPnt.x;
				PrevHYPoint.y = HyPnt.y;
				PrevHYPoint.flag = HyPnt.flag;
				iDirection = TYPE2_LINE_DIRECTION_V;
				iArgumentStackSize++;

				EncodeLineToData(vtData,PrevHYPoint,vtHYPoints,++iPointIndex,iOperatorFlag,iDirection,iArgumentStackSize);
			}
		}

		return HY_NOERROR;

	}	// end of int COTFFontCodec::EncodeLineToData()

	int	 CHYFontCodec::EncodeCurveToData(std::vector<char>& vtData, CHYPoint& PrevHYPoint, std::vector<CHYPoint>& vtHYPoints, size_t& iPointIndex, int& iOperatorFlag, int& iDirection,size_t& iArgumentStackSize)
	{
		short sX = 0, sY = 0;
		long lTmp = 0;

		size_t szPnts = vtHYPoints.size();
		if (szPnts<=iPointIndex) return HY_NOERROR;

		CHYPoint&	HyPnt1 = vtHYPoints[iPointIndex];
		if (HyPnt1.flag == POINT_FLG_ANCHOR)
		{
			if (iOperatorFlag == TYPE2_CHARSTING_OPERATOR_RRCURVETO)
			{
				if (iArgumentStackSize+2 > TYPE2_ARGUMENT_STACK) return HY_NOERROR;
				// 如果是控制点代表下边将是一条直线 rcurveline
				sX = HyPnt1.x - PrevHYPoint.x;
				sY = HyPnt1.y - PrevHYPoint.y;
				// x				
				EncodeDICTInteger(sX,vtData);
				iArgumentStackSize++;				
				// y				
				EncodeDICTInteger(sY,vtData);
				iArgumentStackSize++;				
				PrevHYPoint.x = HyPnt1.x;
				PrevHYPoint.y = HyPnt1.y;
				PrevHYPoint.flag = HyPnt1.flag;
				iPointIndex++;
				// operator
				iOperatorFlag =	 TYPE2_CHARSTING_OPERATOR_RCURVELINE;
				iDirection =   TYPE2_CURVE_DIRECTION_0;

				return TYPE2_CURVE_END;
			}
			return HY_NOERROR; 
		}

		//取后两个点
		CHYPoint	HyPnt2;
		CHYPoint	HyPnt3;
		if (szPnts <= iPointIndex + 2)
		{
			CHYPoint&	HyPnt2 = vtHYPoints[iPointIndex + 1];
			CHYPoint&	HyPnt3 = vtHYPoints[0];
		}
		else
		{
			HyPnt2 = vtHYPoints[iPointIndex + 1];
			HyPnt3 = vtHYPoints[iPointIndex + 2];
		}
				

		// 压缩的起点
		if(iOperatorFlag == TYPE2_CHARSTING_OPERATOR_UNKNOW)
		{
			// hvcurveto
			if (((HyPnt1.y-PrevHYPoint.y)==0) && ((HyPnt3.x-HyPnt2.x)==0))
			{
				if (iArgumentStackSize+4 > TYPE2_ARGUMENT_STACK) return HY_NOERROR;

				sX = HyPnt1.x - PrevHYPoint.x;				
				EncodeDICTInteger(sX,vtData);				
				iArgumentStackSize++;
				// dxb
				sX = HyPnt2.x - HyPnt1.x;				
				EncodeDICTInteger(sX,vtData);				
				iArgumentStackSize++;
				// dyb
				sY = HyPnt2.y - HyPnt1.y;			
				EncodeDICTInteger(sY,vtData);				
				iArgumentStackSize++;
				// dyc
				sY = HyPnt3.y - HyPnt2.y;			
				EncodeDICTInteger(sY,vtData);				
				iArgumentStackSize++;
				
				PrevHYPoint.x = HyPnt3.x;
				PrevHYPoint.y = HyPnt3.y;
				PrevHYPoint.flag = HyPnt3.flag;
				iDirection = TYPE2_CURVE_DIRECTION_HV;
				iOperatorFlag = TYPE2_CHARSTING_OPERATOR_HVCURVETO;

				iPointIndex += 3;
				EncodeCurveToData(vtData,PrevHYPoint,vtHYPoints,iPointIndex,iOperatorFlag,iDirection,iArgumentStackSize);
			}  
			else if (((HyPnt1.x - PrevHYPoint.x)==0) && ((HyPnt3.y - HyPnt2.y) == 0))	// vhcurveto
			{
				if (iArgumentStackSize+4 > TYPE2_ARGUMENT_STACK) return HY_NOERROR;

				sY = HyPnt1.y - PrevHYPoint.y;				
				EncodeDICTInteger(sY,vtData);			
				iArgumentStackSize++;
				// dxb
				sX = HyPnt2.x - HyPnt1.x;
				EncodeDICTInteger(sX,vtData);				
				iArgumentStackSize++;
				// dyb
				sY = HyPnt2.y - HyPnt1.y;			
				EncodeDICTInteger(sY,vtData);
				iArgumentStackSize++;
				// dyc
				sX = HyPnt3.x - HyPnt2.x;
				EncodeDICTInteger(sX,vtData);				
				iArgumentStackSize++;			

				PrevHYPoint.x = HyPnt3.x;
				PrevHYPoint.y = HyPnt3.y;
				PrevHYPoint.flag = HyPnt3.flag;
				iDirection = TYPE2_CURVE_DIRECTION_VH;
				iOperatorFlag = TYPE2_CHARSTING_OPERATOR_VHCURVETO;
				iPointIndex += 3;

				EncodeCurveToData(vtData,PrevHYPoint,vtHYPoints,iPointIndex,iOperatorFlag,iDirection,iArgumentStackSize);
			}
			else if((HyPnt3.y - HyPnt2.y) == 0)		// hhcurveto
			{
				if (iArgumentStackSize+4 > TYPE2_ARGUMENT_STACK) return HY_NOERROR;

				// |-dy1? {dxa dxb dyb dxc} + hhcurveto|-
				if ((HyPnt1.y - PrevHYPoint.y) != 0)
				{
					if (iArgumentStackSize+5 > TYPE2_ARGUMENT_STACK) return HY_NOERROR;

					// dy1
					sY = HyPnt1.y - PrevHYPoint.y;					
					EncodeDICTInteger(sY,vtData);					
					iArgumentStackSize++;					
				}
				// |-{dxa dxb dyb dxc} + hhcurveto|-
				// dxa
				sX = HyPnt1.x - PrevHYPoint.x;				
				EncodeDICTInteger(sX,vtData);				
				iArgumentStackSize++;
				// dxb
				sX = HyPnt2.x - HyPnt1.x;			
				EncodeDICTInteger(sX,vtData);				
				iArgumentStackSize++;
				// dyb
				sY = HyPnt2.y - HyPnt1.y;				
				EncodeDICTInteger(sY,vtData);				
				iArgumentStackSize++;
				// dxc
				sX = HyPnt3.x - HyPnt2.x;				
				EncodeDICTInteger(sX,vtData);				
				iArgumentStackSize++;

				PrevHYPoint.x = HyPnt3.x;
				PrevHYPoint.y = HyPnt3.y;
				PrevHYPoint.flag = HyPnt3.flag;
				iDirection = TYPE2_CURVE_DIRECTION_H;
				iOperatorFlag = TYPE2_CHARSTING_OPERATOR_HHCURVETO;
				iPointIndex += 3;

				EncodeCurveToData(vtData,PrevHYPoint,vtHYPoints,iPointIndex,iOperatorFlag,iDirection,iArgumentStackSize);
			}
			// vvcuveto
			else if((HyPnt3.x - HyPnt2.x) == 0)
			{
				if (iArgumentStackSize+4 > TYPE2_ARGUMENT_STACK) return HY_NOERROR;

				// |-dx1? {dya dxb dyb dyc} + hhcurveto|-
				if ((HyPnt1.x - PrevHYPoint.x) != 0 )
				{
					if (iArgumentStackSize+5 > TYPE2_ARGUMENT_STACK) return HY_NOERROR;

					// dx1
					sX = HyPnt1.x - PrevHYPoint.x;					
					EncodeDICTInteger(sX,vtData);					
					iArgumentStackSize++;					
				}
				// |-{dya dxb dyb dyc} + hhcurveto|-
				// dya
				sY = HyPnt1.y - PrevHYPoint.y;				
				EncodeDICTInteger(sY,vtData);				
				iArgumentStackSize++;
				// dxb
				sX = HyPnt2.x - HyPnt1.x;
				EncodeDICTInteger(sX,vtData);				
				iArgumentStackSize++;
				// dyb
				sY = HyPnt2.y - HyPnt1.y;
				EncodeDICTInteger(sY,vtData);
				iArgumentStackSize++;
				// dyc
				sY = HyPnt3.y - HyPnt2.y;
				EncodeDICTInteger(sY,vtData);				
				iArgumentStackSize++;

				PrevHYPoint.x = HyPnt3.x;
				PrevHYPoint.y = HyPnt3.y;
				PrevHYPoint.flag = HyPnt3.flag;
				iDirection = TYPE2_CURVE_DIRECTION_V;
				iOperatorFlag = TYPE2_CHARSTING_OPERATOR_VVCURVETO;
				iPointIndex += 3;

				EncodeCurveToData(vtData,PrevHYPoint,vtHYPoints,iPointIndex,iOperatorFlag,iDirection,iArgumentStackSize);
			}
			else 
			{
				if (iArgumentStackSize+6 > TYPE2_ARGUMENT_STACK) return HY_NOERROR;

				// 第一个点
				sX = HyPnt1.x - PrevHYPoint.x;
				sY = HyPnt1.y - PrevHYPoint.y;
				// x				
				EncodeDICTInteger(sX,vtData);				
				iArgumentStackSize++;				
				// y
				EncodeDICTInteger(sY,vtData);				
				iArgumentStackSize++;
				// 第二个点
				sX = HyPnt2.x - HyPnt1.x;
				sY = HyPnt2.y - HyPnt1.y;
				// x				
				EncodeDICTInteger(sX,vtData);				
				iArgumentStackSize++;				
				// y			
				EncodeDICTInteger(sY,vtData);				
				iArgumentStackSize++;
				// 第三个点
				sX = HyPnt3.x - HyPnt2.x;
				sY = HyPnt3.y - HyPnt2.y;
				// x
				EncodeDICTInteger(sX,vtData);				
				iArgumentStackSize++;				
				// y			
				EncodeDICTInteger(sY,vtData);				
				iArgumentStackSize++;			

				PrevHYPoint.x = HyPnt3.x;
				PrevHYPoint.y = HyPnt3.y;
				PrevHYPoint.flag = HyPnt3.flag;
				// operator
				iOperatorFlag =	 TYPE2_CHARSTING_OPERATOR_RRCURVETO;
				iDirection =   TYPE2_CURVE_DIRECTION_0;
				iPointIndex+=3;

				EncodeCurveToData(vtData,PrevHYPoint,vtHYPoints,iPointIndex,iOperatorFlag,iDirection,iArgumentStackSize);
			}
		}
		else if (iOperatorFlag == TYPE2_CHARSTING_OPERATOR_RRCURVETO)
		{
			if (iArgumentStackSize+6 > TYPE2_ARGUMENT_STACK) return HY_NOERROR;
			if ((HyPnt1.y - PrevHYPoint.y) == 0 )	return HY_NOERROR;

			// 第一个点
			sX = HyPnt1.x - PrevHYPoint.x;
			sY = HyPnt1.y - PrevHYPoint.y;
			// x		
			EncodeDICTInteger(sX,vtData);			
			iArgumentStackSize++;			
			// y			
			EncodeDICTInteger(sY,vtData);			
			iArgumentStackSize++;			
			// 第二个点
			sX = HyPnt2.x - HyPnt1.x;
			sY = HyPnt2.y - HyPnt1.y;
			// x			
			EncodeDICTInteger(sX,vtData);			
			iArgumentStackSize++;			
			// y			
			EncodeDICTInteger(sY,vtData);			
			iArgumentStackSize++;			
			// 第三个点
			sX = HyPnt3.x - HyPnt2.x;
			sY = HyPnt3.y - HyPnt2.y;
			// x
			EncodeDICTInteger(sX,vtData);			
			iArgumentStackSize++;			
			// y			
			EncodeDICTInteger(sY,vtData);			
			iArgumentStackSize++;	

			PrevHYPoint.x = HyPnt3.x;
			PrevHYPoint.y = HyPnt3.y;
			PrevHYPoint.flag = HyPnt3.flag;
			// operator
			iOperatorFlag =	 TYPE2_CHARSTING_OPERATOR_RRCURVETO;
			iDirection =   TYPE2_CURVE_DIRECTION_0;
			iPointIndex+=3;

			EncodeCurveToData(vtData,PrevHYPoint,vtHYPoints,iPointIndex,iOperatorFlag,iDirection,iArgumentStackSize);
		}
		else if (iOperatorFlag == TYPE2_CHARSTING_OPERATOR_HHCURVETO)
		{
			if (iArgumentStackSize+4 > TYPE2_ARGUMENT_STACK) return HY_NOERROR;
			if ((HyPnt1.y - PrevHYPoint.y) != 0 ) return HY_NOERROR;
			if ((HyPnt3.y - HyPnt2.y) != 0 ) return HY_NOERROR;		

			// |-{dxa dxb dyb dxc} + hhcurveto|-
			// dxa
			sX = HyPnt1.x-PrevHYPoint.x;			
			EncodeDICTInteger(sX,vtData);			
			iArgumentStackSize++;
			// dxb
			sX = HyPnt2.x-HyPnt1.x;		
			EncodeDICTInteger(sX,vtData);			
			iArgumentStackSize++;
			// dyb
			sY = HyPnt2.y-HyPnt1.y;			
			EncodeDICTInteger(sY,vtData);			
			iArgumentStackSize++;
			// dxc
			sX = HyPnt3.x-HyPnt2.x;			
			EncodeDICTInteger(sX,vtData);			
			iArgumentStackSize++;		

			PrevHYPoint.x = HyPnt3.x;
			PrevHYPoint.y = HyPnt3.y;
			PrevHYPoint.flag = HyPnt3.flag;
			iDirection = TYPE2_CURVE_DIRECTION_H;
			iOperatorFlag = TYPE2_CHARSTING_OPERATOR_HHCURVETO;
			iPointIndex += 3;

			EncodeCurveToData(vtData,PrevHYPoint,vtHYPoints,iPointIndex,iOperatorFlag,iDirection,iArgumentStackSize);
		}
		else if (iOperatorFlag == TYPE2_CHARSTING_OPERATOR_VVCURVETO)
		{
			if (iArgumentStackSize+4 > TYPE2_ARGUMENT_STACK) return HY_NOERROR;
			if ((HyPnt1.x - PrevHYPoint.x) != 0 ) return HY_NOERROR;
			if ((HyPnt3.x - HyPnt2.x) != 0 ) return HY_NOERROR;

			// |-{dya dxb dyb dyc} + hhcurveto|-
			// dya
			sY = HyPnt1.y-PrevHYPoint.y;			
			EncodeDICTInteger(sY,vtData);			
			iArgumentStackSize++;
			// dxb
			sX = HyPnt2.x-HyPnt1.x;			
			EncodeDICTInteger(sX,vtData);			
			iArgumentStackSize++;
			// dyb
			sY = HyPnt2.y-HyPnt1.y;			
			EncodeDICTInteger(sY,vtData);			
			iArgumentStackSize++;
			// dyc
			sY = HyPnt3.y-HyPnt2.y;			
			EncodeDICTInteger(sY,vtData);			
			iArgumentStackSize++;

			PrevHYPoint.x = HyPnt3.x;
			PrevHYPoint.y = HyPnt3.y;
			PrevHYPoint.flag = HyPnt3.flag;

			iDirection = TYPE2_CURVE_DIRECTION_H;
			iOperatorFlag = TYPE2_CHARSTING_OPERATOR_VVCURVETO;
			iPointIndex += 3;

			EncodeCurveToData(vtData,PrevHYPoint,vtHYPoints,iPointIndex,iOperatorFlag,iDirection,iArgumentStackSize);

		}
		else if (iOperatorFlag == TYPE2_CHARSTING_OPERATOR_HVCURVETO)
		{			
			if (iArgumentStackSize+5 > TYPE2_ARGUMENT_STACK) return HY_NOERROR;
			if (iDirection == TYPE2_CURVE_DIRECTION_HV)
			{
				if ((HyPnt1.x-PrevHYPoint.x) != 0 ) return HY_NOERROR;

				// dyd
				sY = HyPnt1.y-PrevHYPoint.y;				
				EncodeDICTInteger(sY,vtData);			
				iArgumentStackSize++;
				// dxe
				sX = HyPnt2.x-HyPnt1.x;				
				EncodeDICTInteger(sX,vtData);				
				iArgumentStackSize++;
				// dye
				sY = HyPnt2.y-HyPnt1.y;				
				EncodeDICTInteger(sY,vtData);				
				iArgumentStackSize++;
				// dxf
				sX = HyPnt3.x-HyPnt2.x;				
				EncodeDICTInteger(sX,vtData);				
				iArgumentStackSize++;

				PrevHYPoint.x = HyPnt3.x;
				PrevHYPoint.y = HyPnt3.y;
				PrevHYPoint.flag = HyPnt3.flag;

				iDirection = TYPE2_CURVE_DIRECTION_VH;
				iOperatorFlag = TYPE2_CHARSTING_OPERATOR_HVCURVETO;
				iPointIndex += 3;

				//|-{dxa dxb dyb dyc dyd dxe dye dxf} + hvcurveto(31)|-
				if ((HyPnt3.y-HyPnt2.y) == 0)
				{
					EncodeCurveToData(vtData,PrevHYPoint,vtHYPoints,iPointIndex,iOperatorFlag,iDirection,iArgumentStackSize);
				}
				else	//|-{dxa dxb dyb dyc dyd dxe dye dxf} + dyf? hvcurveto(31)|-
				{
					// dyf
					sY=HyPnt3.y-HyPnt2.y;					
					EncodeDICTInteger(sY,vtData);					
					iArgumentStackSize++;
					return HY_NOERROR;
				}			
			}
			else if (iDirection == TYPE2_CURVE_DIRECTION_VH)
			{				
				if ((HyPnt1.y-PrevHYPoint.y) != 0 ) return HY_NOERROR;

				// dyd
				sX = HyPnt1.x-PrevHYPoint.x;				
				EncodeDICTInteger(sX,vtData);				
				iArgumentStackSize++;
				// dxe
				sX = HyPnt2.x-HyPnt1.x;				
				EncodeDICTInteger(sX,vtData);				
				iArgumentStackSize++;
				// dye
				sY = HyPnt2.y-HyPnt1.y;				
				EncodeDICTInteger(sY,vtData);				
				iArgumentStackSize++;
				// dyf
				sY = HyPnt3.y-HyPnt2.y;				
				EncodeDICTInteger(sY,vtData);				
				iArgumentStackSize++;

				PrevHYPoint.x = HyPnt3.x;
				PrevHYPoint.y = HyPnt3.y;
				PrevHYPoint.flag = HyPnt3.flag;

				iDirection = TYPE2_CURVE_DIRECTION_HV;
				iOperatorFlag = TYPE2_CHARSTING_OPERATOR_HVCURVETO;
				iPointIndex += 3;

				//|-{dxa dxb dyb dyc dyd dxe dye dxf} + hvcurveto(31)|-
				if ((HyPnt3.x-HyPnt2.x)==0)
				{
					EncodeCurveToData(vtData,PrevHYPoint,vtHYPoints,iPointIndex,iOperatorFlag,iDirection,iArgumentStackSize);
				}
				else	//|-{dxa dxb dyb dyc dyd dxe dye dxf} + dyf? hvcurveto(31)|-
				{
					// dxf
					sX = HyPnt3.x-HyPnt2.x;					
					EncodeDICTInteger(sX,vtData);					
					iArgumentStackSize++;			

					return HY_NOERROR;
				}
			}
			
		}
		else if (iOperatorFlag == TYPE2_CHARSTING_OPERATOR_VHCURVETO)
		{			
			if (iArgumentStackSize+5 > TYPE2_ARGUMENT_STACK) return HY_NOERROR;
			if (iDirection == TYPE2_CURVE_DIRECTION_VH)
			{
				if ((HyPnt1.y-PrevHYPoint.y) != 0 ) return HY_NOERROR;
				// dxd
				sX = HyPnt1.x - PrevHYPoint.x;				
				EncodeDICTInteger(sX,vtData);				
				iArgumentStackSize++;
				// dxe
				sX = HyPnt2.x - HyPnt1.x;				
				EncodeDICTInteger(sX,vtData);				
				iArgumentStackSize++;
				// dye
				sY = HyPnt2.y-HyPnt1.y;				
				EncodeDICTInteger(sY,vtData);				
				iArgumentStackSize++;
				// dyf
				sY = HyPnt3.y-HyPnt2.y;				
				EncodeDICTInteger(sY,vtData);				
				iArgumentStackSize++;
				
				PrevHYPoint.x = HyPnt3.x;
				PrevHYPoint.y = HyPnt3.y;
				PrevHYPoint.flag = HyPnt3.flag;

				iDirection = TYPE2_CURVE_DIRECTION_HV;
				iOperatorFlag = TYPE2_CHARSTING_OPERATOR_VHCURVETO;
				iPointIndex += 3;

				//|-{dya dxb dyb dxc dxd dxe dye dyf} + vhcurveto(30)|-
				if ((HyPnt3.x-HyPnt2.x)==0)
				{
					EncodeCurveToData(vtData,PrevHYPoint,vtHYPoints,iPointIndex,iOperatorFlag,iDirection,iArgumentStackSize);
				}
				else	//|-{dya dxb dyb dxc dxd dxe dye dyf} + dxf? vhcurveto(30)|-
				{
					// dxf
					sX = HyPnt3.x-HyPnt2.x;					
					EncodeDICTInteger(sX,vtData);					
					iArgumentStackSize++;			

					return HY_NOERROR;
				}
			}
			else if (iDirection == TYPE2_CURVE_DIRECTION_HV)
			{
				if ((HyPnt1.x-PrevHYPoint.x) != 0 ) return HY_NOERROR;

				// dyd
				sY = HyPnt1.y-PrevHYPoint.y;				
				EncodeDICTInteger(sY,vtData);			
				iArgumentStackSize++;
				// dxe
				sX = HyPnt2.x-HyPnt1.x;				
				EncodeDICTInteger(sX,vtData);				
				iArgumentStackSize++;
				// dye
				sY = HyPnt2.y-HyPnt1.y;				
				EncodeDICTInteger(sY,vtData);				
				iArgumentStackSize++;
				// dxf
				sX = HyPnt3.x-HyPnt2.x;				
				EncodeDICTInteger(sX,vtData);				
				iArgumentStackSize++;			

				PrevHYPoint.x = HyPnt3.x;
				PrevHYPoint.y = HyPnt3.y;
				PrevHYPoint.flag = HyPnt3.flag;

				iDirection = TYPE2_CURVE_DIRECTION_VH;
				iOperatorFlag = TYPE2_CHARSTING_OPERATOR_VHCURVETO;
				iPointIndex += 3;

				if ((HyPnt3.y-HyPnt2.y) == 0 )
				{
					EncodeCurveToData(vtData,PrevHYPoint,vtHYPoints,iPointIndex,iOperatorFlag,iDirection,iArgumentStackSize);
				}
				else	//|-{dya dxb dyb dxc dxd dxe dye dyf} + dxf? vhcurveto(30)|-
				{
					// dyf
					sY = HyPnt3.y-HyPnt2.y;					
					EncodeDICTInteger(sY,vtData);					
					iArgumentStackSize++;					
					return HY_NOERROR;
				}
			}			
		}

		return HY_NOERROR;

	}	//end of int CHYFontCodec::EncodeCurveToData()

	int CHYFontCodec::EncodePrivateDICTData(std::vector<char>& vtData,CHYCFFPrivteDict& PrivtDict)
	{		
		//BlueValues;
		size_t szBlV = PrivtDict.vtBlueValues.size();
		if (szBlV>0)
		{
			for (size_t i=0; i<szBlV;i++)
			{
				int iInteger = static_cast<int>(PrivtDict.vtBlueValues[i]);
				EncodeDICTInteger(iInteger,vtData);				
			}
			vtData.push_back(CFF_DICT_OPERATOR_BLUEVALUES);
		}		

		//OtherBlueValues;
		size_t szOB = PrivtDict.vtOtherBlues.size();
		if (szOB>0)
		{
			for (size_t i=0; i<szOB; i++)
			{
				int iInteger = static_cast<int>(PrivtDict.vtOtherBlues[i]);
				EncodeDICTInteger(iInteger,vtData);				
			}
			vtData.push_back(CFF_DICT_OPERATOR_OTHERBLUES);
		}		

		//FamliyBlues;
		size_t szFBLU = PrivtDict.vtFamilyBlues.size();
		if (szFBLU>0)
		{
			for (size_t i=0; i<szFBLU; i++)
			{
				int iInteger = static_cast<int>(PrivtDict.vtFamilyBlues[i]);
				EncodeDICTInteger(iInteger,vtData);				
			}
			vtData.push_back(CFF_DICT_OPERATOR_FAMILYBLUES);
		}
		
		//FamliyOtherBulues:
		size_t szFOBLU = PrivtDict.vtFamilyOtherBlues.size();
		if (szFOBLU>0)
		{
			for (size_t i=0; i<szFOBLU; i++)
			{
				int iInteger = static_cast<int>(PrivtDict.vtFamilyOtherBlues[i]);
				EncodeDICTInteger(iInteger,vtData);				
			}
			vtData.push_back(CFF_DICT_OPERATOR_FAMILYOTHERBLUES);
		}

		//BlueScale:
		if (PrivtDict.fBlueScale != 0.039625000)
		{
			EncodeDICTReal(PrivtDict.fBlueScale,vtData);
			vtData.push_back(static_cast<char>(CFF_DICT_OPERATOR_BLUESCALE>>8));
			vtData.push_back(static_cast<char>(CFF_DICT_OPERATOR_BLUESCALE));
		}		

		//BlueShift
		if (PrivtDict.fBlueShift != 7.0)
		{
			EncodeDICTInteger(static_cast<long>(PrivtDict.fBlueShift),vtData);
			vtData.push_back(static_cast<char>(CFF_DICT_OPERATOR_BLUESHIFT>>8));
			vtData.push_back(static_cast<char>(CFF_DICT_OPERATOR_BLUESHIFT));
		}
		//BlueFuzz
		if (PrivtDict.fBlueFuzz != 1.0)
		{
			EncodeDICTInteger(static_cast<long>(PrivtDict.fBlueFuzz),vtData);
			vtData.push_back(static_cast<char>(CFF_DICT_OPERATOR_BLUEFUZZ>>8));
			vtData.push_back(static_cast<char>(CFF_DICT_OPERATOR_BLUEFUZZ));
		}
		//StdHW		
		if (PrivtDict.fStdHW != -1.0)
		{
			EncodeDICTInteger(static_cast<long>(PrivtDict.fStdHW),vtData);
			vtData.push_back(static_cast<char>(CFF_DICT_OPERATOR_STDHW));
		}
		//StdVW
		if (PrivtDict.fStdHW != -1.0)
		{
			EncodeDICTInteger(static_cast<long>(PrivtDict.fStdVW), vtData);
			vtData.push_back(static_cast<char>(CFF_DICT_OPERATOR_STDVW));
		}
		//stemsnapH		
		size_t sztmsnpH = PrivtDict.vtStemSnapH.size();
		if (sztmsnpH>0)
		{
			for (size_t i=0; i<sztmsnpH; i++)
			{
				int iInteger = static_cast<int>(PrivtDict.vtStemSnapH[i]);
				EncodeDICTInteger(iInteger,vtData);					
			}
			vtData.push_back(static_cast<char>(CFF_DICT_OPERATOR_STEMSNAPH>>8));
			vtData.push_back(static_cast<char>(CFF_DICT_OPERATOR_STEMSNAPH));
		}
		//stemsnapV
		size_t sztmsnpV = PrivtDict.vtStemSnapV.size();
		if (sztmsnpV>0)
		{
			for (size_t i=0; i<sztmsnpV; i++)
			{
				int iInteger = static_cast<int>(PrivtDict.vtStemSnapV[i]);
				EncodeDICTInteger(iInteger,vtData);			
			}
			vtData.push_back(static_cast<char>(CFF_DICT_OPERATOR_STEMSNAPV>>8));
			vtData.push_back(static_cast<char>(CFF_DICT_OPERATOR_STEMSNAPV));
		}		
		// forcebold
		if (PrivtDict.lForceBold!=0)
		{
			EncodeDICTInteger(PrivtDict.lForceBold,vtData);
			vtData.push_back(static_cast<char>(CFF_DICT_OPERATOR_FORCEBOLD>>8));
			vtData.push_back(static_cast<char>(CFF_DICT_OPERATOR_FORCEBOLD));
		}
		//languageGroup		
		if (PrivtDict.lLanguageGroup!=0)
		{
			EncodeDICTInteger(PrivtDict.lLanguageGroup, vtData);
			vtData.push_back(static_cast<char>(CFF_DICT_OPERATOR_LANGUAGEGROUP>>8));
			vtData.push_back(static_cast<char>(CFF_DICT_OPERATOR_LANGUAGEGROUP));
		}
		//expansionfactor
		if (PrivtDict.fExpansionFactor<0.059999||PrivtDict.fExpansionFactor>0.06)
		{
			EncodeDICTReal(PrivtDict.fExpansionFactor, vtData);
			vtData.push_back(static_cast<char>(CFF_DICT_OPERATOR_EXPANSIONFACTOR>>8));
			vtData.push_back(static_cast<char>(CFF_DICT_OPERATOR_EXPANSIONFACTOR));
		}		
		//initialRandomSeed
		if(PrivtDict.finitialRandomSeed!=0.0)
		{
			EncodeDICTReal(PrivtDict.finitialRandomSeed, vtData);
			vtData.push_back(static_cast<char>(CFF_DICT_OPERATOR_INITIALRANDOMSEED>>8));
			vtData.push_back(static_cast<char>(CFF_DICT_OPERATOR_INITIALRANDOMSEED));	
		}
		

#if 0	//暂时不财通subrs的组件服用方式
		// subrs()
		lSubrPostion = 0;
		EncodeDICTInteger(PrivtDict.lSubrsOffset, vtData);
		vtData.push_back(static_cast<char>(CFF_DICT_OPERATOR_SUBRS));
#endif 

		if (PrivtDict.ldefaultWidthX !=0) 
		{
			//defaulwidthx
			EncodeDICTInteger(PrivtDict.ldefaultWidthX,vtData);
			vtData.push_back(static_cast<char>(CFF_DICT_OPERATOR_DEFAULTWIDTHX));
		}

		if (PrivtDict.lnominalWidthX!=0)
		{
			//nominalwidthx		
			EncodeDICTInteger(PrivtDict.lnominalWidthX, vtData);
			vtData.push_back(static_cast<char>(CFF_DICT_OPERATOR_NOMINALWIDTHX));
		}	

		return HY_NOERROR;

	}	// end of int CHYFontCodec::EncodePrivateDICTData()

	int	CHYFontCodec::EncodeFDSelect(std::vector<char>& vtData)
	{
		CHYCFFFDSelect& fdSelect = m_HYCFFInfo.FDSelect;
		
		// format
		vtData.push_back(static_cast<char>(fdSelect.iFormat));
		if (fdSelect.iFormat == 0)
		{
			size_t szFormat0 = fdSelect.format0.size();
			for (size_t i=0; i<szFormat0 ; i++)
			{
				vtData.push_back(fdSelect.format0[i]);
			}
		}

		if (fdSelect.iFormat == 3)
		{
			// nRanges
			unsigned short stRang = fdSelect.format3.vtRang3.size();
			vtData.push_back(static_cast<char>(stRang>>8));
			vtData.push_back(static_cast<char>(stRang));

			// Rang3
			for (unsigned short i=0; i<stRang; i++)
			{
				vtData.push_back(static_cast<char>(fdSelect.format3.vtRang3[i].first>>8));
				vtData.push_back(static_cast<char>(fdSelect.format3.vtRang3[i].first));

				vtData.push_back(fdSelect.format3.vtRang3[i].fdIndex);
			}

			//sentinel
			vtData.push_back(static_cast<char>(fdSelect.format3.sentinel>>8));
			vtData.push_back(static_cast<char>(fdSelect.format3.sentinel));
		}

		return HY_NOERROR;

	}	// end of int COTFFontCodec::BuildFDSelect()

	void CHYFontCodec::EncodeDICTMaxInteger(long lsrc, std::vector<char>& vtData)
	{
		char cTmp = 0;
		vtData.push_back(29);
		cTmp = (char)((lsrc&0xff000000)>>24);
		vtData.push_back(cTmp);
		cTmp = (char)((lsrc&0x00ff0000)>>16);
		vtData.push_back(cTmp);
		cTmp = (char)((lsrc&0x0000ff00)>>8);
		vtData.push_back(cTmp);
		vtData.push_back((char)lsrc);

	}	// end of void COTFFontCodec::EncodeDICTOffset()

	void CHYFontCodec::EncodeDICTInteger(long lsrc, std::vector<char>& vtData)
	{	
		char cTmp = 0;
		short usValue = 0;

		if (lsrc>=-107 && lsrc<=107)
		{			
			vtData.push_back((char)(lsrc+139));
		}
		else if (lsrc>=108 && lsrc<=1131)
		{
			usValue = lsrc-108;

			cTmp = (char)((usValue>>8)+247);
			vtData.push_back(cTmp);
			cTmp = (char)(usValue&0x00ff);
			vtData.push_back(cTmp);			

		}
		else if (lsrc>=-1131 && lsrc<=-108)
		{
			usValue = -lsrc-108;

			cTmp = (char)((usValue>>8)+251);
			vtData.push_back(cTmp);
			cTmp = (char)(usValue&0x00ff);
			vtData.push_back(cTmp);			
		}
		else if (lsrc >= -32768 &&  lsrc <= 32767)
		{
			usValue=lsrc;

			vtData.push_back(28);
			cTmp = (char)(usValue>>8);
			vtData.push_back(cTmp);
			cTmp = (char)(usValue&0x00ff);
			vtData.push_back(cTmp);			
			
		}
		else if (lsrc<-32768 ||  lsrc>32767)
		{
			vtData.push_back(29);
			cTmp = (char)((lsrc&0xff000000)>>24);
			vtData.push_back(cTmp);
			cTmp = (char)((lsrc&0x00ff0000)>>16);
			vtData.push_back(cTmp);
			cTmp = (char)((lsrc&0xff000000)>>8);
			vtData.push_back(cTmp);		
			vtData.push_back((char)lsrc);
		}

	}	// end of void CHYFontCodec::EncodeDICTInteger()

	void CHYFontCodec::EncodeDICTReal(double dbsrc, std::vector<char>& vtData)
	{
		char			bcd [256] = {0};		
		sprintf (bcd, "%.6g", dbsrc);

		unsigned int	ix			= 0;
		BYTE			ch;
		BYTE			a			= 0;
		bool			started		= false;
		bool			inFraction	= false;
		bool			inExponent	= false;
		bool			even		= true;
		
		vtData.push_back((char)30);
		while ((ch = bcd [ix++]) != 0)
		{
			a = a << 4;
			if (ch>='0' && ch<='9')
			{
				a |= static_cast<BYTE>(ch) - '0';
				started = true;
			} else if (ch == '-')
			{
				if (started)			
					return;				
				a |= 0xe;
				started = true;

			} else if (ch == 'E')
			{
				if (inExponent)				
					return;

				if (bcd [ix] == '-')
				{
					a |= 0xc;
					++ix;
				}
				else
				{
					a |= 0xb;
				}

				inExponent	= true;
				started		= true;

			} else if (ch == '.')
			{
				if (inFraction)										
					return;
				if (inExponent)
					return;
				a |= 0xa;
				inFraction = true;
				started = true;
			}
			even = !even;
			if (even) 				
				vtData.push_back(a);
		}

		a = (a<<4)|0xf;
		if (even)
			a = (a<<4)|0xf;

		vtData.push_back(a);

	}	// end of void COTFFontCodec::EncodeDICTReal()	

	unsigned int CHYFontCodec::PutStringToVectorChar(std::vector<char>& vtData, std::string str)
	{
		size_t stStr = str.size();
		for(size_t i=0; i<stStr;i++)
		{
			vtData.push_back(str[i]);
		}
		return stStr;

	}	// end of int	CHYFontCodec::PutStringToVectorChar()

	int CHYFontCodec::GetFDIndex(unsigned int iGID)
	{
		CHYCFFFDSelect& FDSelect = m_HYCFFInfo.FDSelect;
		int FDIndex = -1;
		if (FDSelect.iFormat==0)
		{
			if (FDSelect.format0.size()>iGID)
			{
				FDIndex = static_cast<int>(FDSelect.format0[iGID]);
			}
		}

		if (FDSelect.iFormat==3)
		{
			size_t sz=FDSelect.format3.vtRang3.size();
			for (size_t i=0; i<sz; i++)
			{
				if (i==sz-1)
				{
					if (iGID>=FDSelect.format3.vtRang3[i].first && iGID<=FDSelect.format3.sentinel)
					{
						FDIndex = FDSelect.format3.vtRang3[i].fdIndex;
					}
				}
				else if ((iGID>=FDSelect.format3.vtRang3[i].first) && (iGID<FDSelect.format3.vtRang3[i+1].first))
				{
					FDIndex = FDSelect.format3.vtRang3[i].fdIndex;
					break;
				}
			}
		}

		return FDIndex;

	}	// end of int CHYFontCodec::GetFDIndex()			

	int CHYFontCodec::DecodeBASE()
	{
		if (m_pFontFile)
		{
			int iEntryIndex = m_HYTbDirectory.FindTableEntry(BASE_TAG);
			if (iEntryIndex == -1) return FONT_ERR_BASE_DECODE;

			CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
			fseek(m_pFontFile, tbEntry.offset, SEEK_SET);

			TableData tbData;
			tbData.iTableFlg = BASE_TAG;

			char chData;
			for (int i = 0; i < tbEntry.length; i++) {
				fread(&chData, 1, 1, m_pFontFile);
				tbData.vtData.push_back(chData);
			}
			tbData.ulDataSize = tbEntry.length;

			m_mulpTableData.vtTableData.push_back(tbData);

			return HY_NOERROR;
		}

		return FONT_ERR_BASE_DECODE;

	}	// end of int CHYFontCodec::DecodeBASE()

	int	CHYFontCodec::DecodeGDEF()
	{
		if (m_pFontFile)
		{
			int iEntryIndex = m_HYTbDirectory.FindTableEntry(GDEF_TAG);
			if (iEntryIndex == -1) return FONT_ERR_GDEF_DECODE;

			CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
			fseek(m_pFontFile, tbEntry.offset, SEEK_SET);

			TableData tbData;
			tbData.iTableFlg = GDEF_TAG;

			char chData;
			for (int i = 0; i < tbEntry.length; i++) {
				fread(&chData, 1, 1, m_pFontFile);
				tbData.vtData.push_back(chData);
			}
			tbData.ulDataSize = tbEntry.length;

			m_mulpTableData.vtTableData.push_back(tbData);

			return HY_NOERROR;
		}

		return FONT_ERR_GDEF_DECODE;

	}	// end of int CHYFontCodec::DecodeGDEF()

	int	CHYFontCodec::DecodeGPOS()
	{
		if (m_pFontFile)
		{
			int iEntryIndex = m_HYTbDirectory.FindTableEntry(GPOS_TAG);
			if (iEntryIndex == -1) return FONT_ERR_GPOS_DECODE;

			CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
			fseek(m_pFontFile, tbEntry.offset, SEEK_SET);

			TableData tbData;
			tbData.iTableFlg = GPOS_TAG;

			char chData;
			for (int i = 0; i < tbEntry.length; i++) {
				fread(&chData, 1, 1, m_pFontFile);
				tbData.vtData.push_back(chData);
			}
			tbData.ulDataSize = tbEntry.length;

			m_mulpTableData.vtTableData.push_back(tbData);

			return HY_NOERROR;
		}

		return FONT_ERR_GPOS_DECODE;

	}	// end of int CHYFontCodec::DecodeGPOS()

	int CHYFontCodec::DecodeGSUB()
	{	
#if 0
		if (m_pFontFile)
		{
			int iEntryIndex = m_HYTbDirectory.FindTableEntry(GSUB_TAG);
			if (iEntryIndex == -1) return FONT_ERR_GSUB_DECODE;

			CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
			fseek(m_pFontFile,tbEntry.offset,SEEK_SET);

			//Table version number
			fread(&m_HYhead.version.value,2,1,m_pFontFile);
			m_HYGsub.version.value = hy_cdr_int16_to(m_HYhead.version.value);
			fread(&m_HYhead.version.fract,2,1,m_pFontFile);
			m_HYGsub.version.fract = hy_cdr_int16_to(m_HYhead.version.fract);

			//ScriptList offset
			unsigned short ScriptlistOffset = 0;
			fread(&ScriptlistOffset,2,1,m_pFontFile);
			ScriptlistOffset = hy_cdr_int16_to(ScriptlistOffset);

			//FeatureList Offset
			unsigned short FeatureListOffset = 0;
			fread(&FeatureListOffset,2,1,m_pFontFile);
			FeatureListOffset = hy_cdr_int16_to(FeatureListOffset);

			//LookupList offset
			unsigned short LookupListOffset = 0;
			fread(&LookupListOffset,2,1,m_pFontFile);
			LookupListOffset = hy_cdr_int16_to(LookupListOffset);

			DecodeScriptList(tbEntry.offset+ScriptlistOffset);
			DecodeFeaturetList(tbEntry.offset+FeatureListOffset);
			DecodeLookupList(tbEntry.offset+LookupListOffset);

			return HY_NOERROR;	
		}
#else 
		if (m_pFontFile)
		{
			int iEntryIndex = m_HYTbDirectory.FindTableEntry(GSUB_TAG);
			if (iEntryIndex == -1) return FONT_ERR_GSUB_DECODE;

			CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
			fseek(m_pFontFile, tbEntry.offset, SEEK_SET);
			
			TableData tbData;
			tbData.iTableFlg = GSUB_TAG;

			char chData;
			for (int i = 0; i < tbEntry.length; i++) {
				fread(&chData, 1, 1, m_pFontFile);
				tbData.vtData.push_back(chData);
			}
			tbData.ulDataSize = tbEntry.length;
			m_mulpTableData.vtTableData.push_back(tbData);

			return HY_NOERROR;
		}
#endif 

		return FONT_ERR_GSUB_DECODE;		

	}	// end of int CHYFontCodec::DecodeGSUB()

	bool CHYFontCodec::DecodeScriptList(unsigned long ulOffset)
	{
		unsigned short usTmp = 0;

		fseek(m_pFontFile,ulOffset,SEEK_SET);

		//ScriptCount
		unsigned short usScriptCount=0;
		fread(&usScriptCount,2,1,m_pFontFile);
		usScriptCount = hy_cdr_int16_to(usScriptCount);

		for (unsigned short i=0; i<usScriptCount; i++)
		{
			CHYScriptRecord  ScriptRecord;

			//ScriptTag
			fread(&ScriptRecord.Tag,4,1,m_pFontFile);

			// Script offset
			unsigned short usScriptOffset = 0;
			fread(&usScriptOffset,2,1,m_pFontFile);
			usScriptOffset = hy_cdr_int16_to(usScriptOffset);

			long ScriptPos = ftell(m_pFontFile);

			fseek(m_pFontFile,ulOffset+usScriptOffset,SEEK_SET);
			//DefaultLangSys
			unsigned short usDefaultLangSysOffset=0;
			fread(&usDefaultLangSysOffset,2,1,m_pFontFile);
			usDefaultLangSysOffset = hy_cdr_int16_to(usDefaultLangSysOffset);

			if (usDefaultLangSysOffset>0)
			{
				long DefaultPos = ftell(m_pFontFile);
				fseek(m_pFontFile,ulOffset+usScriptOffset+usDefaultLangSysOffset,SEEK_SET);
				//Offset	reserved  0	
				fread(&usTmp,2,1,m_pFontFile);
				//ReqFeatureIndex
				fread(&ScriptRecord.Script.DefaultLangSys.ReqFeatureIndex,2,1,m_pFontFile);
				ScriptRecord.Script.DefaultLangSys.ReqFeatureIndex = hy_cdr_int16_to(ScriptRecord.Script.DefaultLangSys.ReqFeatureIndex);
				//FeatureCount
				fread(&ScriptRecord.Script.DefaultLangSys.FeatureCount,2,1,m_pFontFile);
				ScriptRecord.Script.DefaultLangSys.FeatureCount = hy_cdr_int16_to(ScriptRecord.Script.DefaultLangSys.FeatureCount);
				for (unsigned short x=0; x<ScriptRecord.Script.DefaultLangSys.FeatureCount; x++)
				{
					//FeatureIndex					
					fread(&usTmp,2,1,m_pFontFile);
					usTmp = hy_cdr_int16_to(usTmp);
					ScriptRecord.Script.DefaultLangSys.vtFeatureIndex.push_back(usTmp);
				}			
				fseek(m_pFontFile,DefaultPos,SEEK_SET);
			}		

			//LangSysCount
			unsigned short usLangSysCount=0;
			fread(&usLangSysCount,2,1,m_pFontFile);		
			usLangSysCount = hy_cdr_int16_to(usLangSysCount);

			// LangSysRecord
			for (unsigned short j=0; j<usLangSysCount; j++)
			{
				CHYLangSysRecord	LangSysRecord;
				//LangSysTag
				fread(&LangSysRecord.SysTag,4,1,m_pFontFile);
				//LangSys offset
				unsigned short usLangSysOffset=0 ; 
				fread(&usLangSysOffset,2,1,m_pFontFile);
				usLangSysOffset = hy_cdr_int16_to(usLangSysOffset);

				long LangSysPos = ftell(m_pFontFile);

				fseek(m_pFontFile,ulOffset+usScriptOffset+usLangSysOffset,SEEK_SET);
				//Offset	reserved  0	
				fread(&usTmp,2,1,m_pFontFile);
				//ReqFeatureIndex
				fread(&LangSysRecord.LangSys.ReqFeatureIndex,2,1,m_pFontFile);
				LangSysRecord.LangSys.ReqFeatureIndex = hy_cdr_int16_to(LangSysRecord.LangSys.ReqFeatureIndex);
				//FeatureCount
				fread(&LangSysRecord.LangSys.FeatureCount,2,1,m_pFontFile);
				LangSysRecord.LangSys.FeatureCount = hy_cdr_int16_to(LangSysRecord.LangSys.FeatureCount);
				for (unsigned short x=0; x<LangSysRecord.LangSys.FeatureCount; x++)
				{
					//FeatureIndex					
					fread(&usTmp,2,1,m_pFontFile);
					usTmp = hy_cdr_int16_to(usTmp);
					LangSysRecord.LangSys.vtFeatureIndex.push_back(usTmp);
				}

				ScriptRecord.Script.vtLangSysRecord.push_back(LangSysRecord);
				fseek(m_pFontFile,LangSysPos,SEEK_SET);
			}

			m_HYGsub.vtScriptsList.push_back(ScriptRecord);
			fseek(m_pFontFile,ScriptPos,SEEK_SET);
		}

		return true;

	}	// end of int CHYFontCodec::DecodeScriptList()

	bool CHYFontCodec::DecodeFeaturetList(unsigned long ulOffset)
	{
		unsigned short usTmp = 0;
		fseek(m_pFontFile,ulOffset,SEEK_SET);

		//FeatureCount
		unsigned short usFeatureCount=0;
		fread(&usFeatureCount,2,1,m_pFontFile);
		usFeatureCount = hy_cdr_int16_to(usFeatureCount);

		for (unsigned short i=0; i<usFeatureCount; i++)
		{
			CHYFeatureRecord  FeatureRecord;

			//FeatureTag
			fread(&FeatureRecord.Tag,4,1,m_pFontFile);

			unsigned short usFeatueoffset=0;
			fread(&usFeatueoffset,2,1,m_pFontFile);
			usFeatueoffset = hy_cdr_int16_to(usFeatueoffset);

			long lFeaturePos = ftell(m_pFontFile);

			fseek(m_pFontFile,ulOffset+usFeatueoffset,SEEK_SET);
			//FeatureParams reserved 0
			fread(&usTmp,2,1,m_pFontFile);

			//LookupCount
			unsigned short LookupCount = 0;
			fread(&LookupCount,2,1,m_pFontFile);
			LookupCount = hy_cdr_int16_to(LookupCount);
			for (unsigned short j=0; j<LookupCount; j++)
			{		
				//LookupListIndex
				fread(&usTmp,2,1,m_pFontFile);
				usTmp = hy_cdr_int16_to(usTmp);
				FeatureRecord.FeatureTable.vtLookupListIndex.push_back(usTmp);
			}

			m_HYGsub.vtFeaturesList.push_back(FeatureRecord);
			fseek(m_pFontFile,lFeaturePos,SEEK_SET);
		}

		return true;

	}	// end of bool CHYFontCodec::DecodeScriptList()

	bool CHYFontCodec::DecodeLookupList(unsigned long ulOffset)
	{
		unsigned short usTmp = 0;
		fseek(m_pFontFile,ulOffset,SEEK_SET);

		//LookupCount
		unsigned short usLookupCount=0;
		fread(&usLookupCount,2,1,m_pFontFile);
		usLookupCount = hy_cdr_int16_to(usLookupCount);

		for (unsigned short i=0; i<usLookupCount; i++)
		{
			unsigned short usLookupLstOffset=0;
			fread(&usLookupLstOffset,2,1,m_pFontFile);
			usLookupLstOffset = hy_cdr_int16_to(usLookupLstOffset);

			long LookupPos = ftell(m_pFontFile);

			fseek(m_pFontFile,ulOffset+usLookupLstOffset,SEEK_SET);		
			CLookUp lookup;
			//LookupType
			fread(&lookup.LookUpType,2,1,m_pFontFile);
			lookup.LookUpType = hy_cdr_int16_to(lookup.LookUpType);
			//LookupFlag
			fread(&lookup.LookUpFlag,2,1,m_pFontFile);
			lookup.LookUpFlag = hy_cdr_int16_to(lookup.LookUpFlag);
			//SubTableCount			
			fread(&lookup.SubTableCount,2,1,m_pFontFile);
			lookup.SubTableCount = hy_cdr_int16_to(lookup.SubTableCount);

			//SubTable Array of offsets 
			for (unsigned short j=0; j<lookup.SubTableCount;j++)
			{
				unsigned short SubTableoffset=0;
				fread(&SubTableoffset,2,1,m_pFontFile);
				SubTableoffset = hy_cdr_int16_to(SubTableoffset);
				
				if (lookup.LookUpType==1)
				{					
					CLookUpType1		LookupType1;

					long SubTablePost = ftell(m_pFontFile);
					fseek(m_pFontFile,ulOffset+usLookupLstOffset+SubTableoffset,SEEK_SET);

					//SubstFormat					
					fread(&LookupType1.SubFormat,2,1,m_pFontFile);
					LookupType1.SubFormat = hy_cdr_int16_to(LookupType1.SubFormat);
					if (LookupType1.SubFormat==2)
					{
						//Coverage
						unsigned short usCoverageOffset = 0;
						fread(&usCoverageOffset,2,1,m_pFontFile);
						usCoverageOffset = hy_cdr_int16_to(usCoverageOffset);

						long CoveragePos = ftell(m_pFontFile);
						fseek(m_pFontFile,ulOffset+usLookupLstOffset+SubTableoffset+usCoverageOffset,SEEK_SET);

						fread(&LookupType1.Substitution2.Coverage.CoverageFormat,2,1,m_pFontFile);
						LookupType1.Substitution2.Coverage.CoverageFormat = hy_cdr_int16_to(LookupType1.Substitution2.Coverage.CoverageFormat);
						if (LookupType1.Substitution2.Coverage.CoverageFormat==1)
						{
							fread(&LookupType1.Substitution2.Coverage.GlyphCount,2,1,m_pFontFile);
							LookupType1.Substitution2.Coverage.GlyphCount = hy_cdr_int16_to(LookupType1.Substitution2.Coverage.GlyphCount);

							for (int x=0; x<LookupType1.Substitution2.Coverage.GlyphCount; x++)
							{
								fread(&usTmp,2,1,m_pFontFile);
								usTmp = hy_cdr_int16_to(usTmp);
								LookupType1.Substitution2.Coverage.vtGlyphID.push_back(usTmp);
							}
						}
						else if (LookupType1.Substitution2.Coverage.CoverageFormat==2)
						{
							fread(&LookupType1.Substitution2.Coverage.RangeCount,2,1,m_pFontFile);
							LookupType1.Substitution2.Coverage.RangeCount = hy_cdr_int16_to(LookupType1.Substitution2.Coverage.RangeCount);

							for (int x=0; x<LookupType1.Substitution2.Coverage.RangeCount; x++)
							{
								CRangeRecord rangrecord;
								//Start
								fread(&rangrecord.Start,2,1,m_pFontFile);
								rangrecord.Start = hy_cdr_int16_to(rangrecord.Start);
								//End
								fread(&rangrecord.End,2,1,m_pFontFile);
								rangrecord.End = hy_cdr_int16_to(rangrecord.End);
								//StartCoverageIndex
								fread(&rangrecord.StartCoverageIndex,2,1,m_pFontFile);
								rangrecord.StartCoverageIndex = hy_cdr_int16_to(rangrecord.StartCoverageIndex);

								LookupType1.Substitution2.Coverage.vtRangeRecord.push_back(rangrecord);
							}
						}

						
						fseek(m_pFontFile,CoveragePos,SEEK_SET);

						fread(&LookupType1.Substitution2.GlyphCount,2,1,m_pFontFile);
						LookupType1.Substitution2.GlyphCount = hy_cdr_int16_to(LookupType1.Substitution2.GlyphCount);
						for (int y=0; y<LookupType1.Substitution2.GlyphCount; y++)
						{
							fread(&usTmp,2,1,m_pFontFile);
							usTmp = hy_cdr_int16_to(usTmp);
							LookupType1.Substitution2.vtGlyphID.push_back(usTmp);
						}
					}

					fseek(m_pFontFile,SubTablePost,SEEK_SET);
					lookup.vtLookupType1.push_back(LookupType1);
				}
			}
			
			m_HYGsub.vtLookupList.push_back(lookup);
			fseek(m_pFontFile,LookupPos,SEEK_SET);	
		}

		return true;

	}	// end of bool CHYFontCodec::DecodeLookupList()

	long g_GsubEndPos = 0;
	long g_GsubBeginPos = 0;
	int	CHYFontCodec::EncodeGSUB()
	{
		if (m_pFontFile) {
			if (this->m_tagOption.bCmplLayout) {
				BulidGSUB();
			}
			else {
				int iEntryIndex = m_HYTbDirectory.FindTableEntry(GSUB_TAG);
				if (iEntryIndex == -1) return FONT_ERR_GSUB_ENCODE;
				CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
				tbEntry.offset = ftell(m_pFontFile);

				TableData* pGSUBData = m_mulpTableData.FindTableData(GSUB_TAG);
				if (pGSUBData) {					
					for (int i = 0; i < pGSUBData->ulDataSize; i++) {
						fwrite(&pGSUBData->vtData[i], 1, 1, m_pFontFile);
					}	
				}

				tbEntry.length = ftell(m_pFontFile) - tbEntry.offset;
				::HY_4ByteAlign(m_pFontFile, tbEntry.length);
			}
		}

		return HY_NOERROR;

	}	// end of int	CHYFontCodec::EncodeGSUB()

	int CHYFontCodec::BulidGSUB()
	{
		unsigned short	usTmp = 0;
		int iEntryIndex = m_HYTbDirectory.FindTableEntry(GSUB_TAG);
		if (iEntryIndex == -1) return FONT_ERR_GSUB_ENCODE;

		CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
		tbEntry.offset = ftell(m_pFontFile);
		g_GsubBeginPos = tbEntry.offset;

		//Table version number
		m_HYGsub.version.value = 1;
		m_HYGsub.version.fract = 0;
		usTmp = hy_cdr_int16_to(m_HYhead.version.value);
		fwrite(&usTmp, 2, 1, m_pFontFile);
		usTmp = hy_cdr_int16_to(m_HYhead.version.fract);
		fwrite(&usTmp, 2, 1, m_pFontFile);

		usTmp = 0;
		//ScriptList offset			
		fwrite(&usTmp, 2, 1, m_pFontFile);
		//FeatureList offset
		fwrite(&usTmp, 2, 1, m_pFontFile);
		//LookupList offset
		fwrite(&usTmp, 2, 1, m_pFontFile);

		EncodeScriptList();
		EncodeFeatureList();
		EncodeLookupList();

		tbEntry.length = ftell(m_pFontFile) - tbEntry.offset;
		::HY_4ByteAlign(m_pFontFile, tbEntry.length);

		return HY_NOERROR;

	}	// end of int CHYFontCodec::BulidGSUB()

	int CHYFontCodec::EncodeScriptList()
	{
		if (m_pFontFile)
		{	
			unsigned short				usTmp				= 0;

			unsigned long ulScriptLstBeginPos = ftell(m_pFontFile);
			//write ScriptList offset
			fseek(m_pFontFile,g_GsubBeginPos+4,SEEK_SET);
			usTmp = hy_cdr_int16_to(ulScriptLstBeginPos-g_GsubBeginPos);
			fwrite(&usTmp,2,1,m_pFontFile);
			fseek(m_pFontFile,ulScriptLstBeginPos,SEEK_SET);

			size_t stScriptsList = m_HYGsub.vtScriptsList.size();
			//ScriptCount			
			usTmp = hy_cdr_int16_to(static_cast<unsigned short>(stScriptsList));
			fwrite(&usTmp,2,1,m_pFontFile);

			std::vector<long> vtScriptOffset;
			for (size_t i=0; i<stScriptsList; i++)
			{
				CHYScriptRecord&  ScriptRecord = m_HYGsub.vtScriptsList[i];
				//ScriptTag
				fwrite(ScriptRecord.Tag,4,1,m_pFontFile);				
				//Script offset;
				usTmp = 0;		
				fwrite(&usTmp,2,1,m_pFontFile);
			}

			for (size_t i=0; i<stScriptsList; i++)
			{
				// save Script Offset
				long ulScriptBegin = ftell(m_pFontFile);
				vtScriptOffset.push_back(ulScriptBegin-ulScriptLstBeginPos);

				CHYScriptRecord&  ScriptRecord = m_HYGsub.vtScriptsList[i];
				//DefaultLangSys offset;				
				usTmp = 0;
				fwrite(&usTmp,2,1,m_pFontFile);
				//LangSysCount
				size_t  stLangSysRecord = ScriptRecord.Script.vtLangSysRecord.size();
				usTmp = hy_cdr_int16_to(static_cast<unsigned short>(stLangSysRecord));
				fwrite(&usTmp,2,1,m_pFontFile);

				//LangSysRecord
				for (size_t j=0; j<stLangSysRecord; j++)
				{
					CHYLangSysRecord& LangSysRecord = ScriptRecord.Script.vtLangSysRecord[j];
					//LangSysRecord Tag
					fwrite(LangSysRecord.SysTag,4,1,m_pFontFile);
					//LangSys Offset
					usTmp = 0;
					fwrite(&usTmp,2,1,m_pFontFile);
				}

				std::vector<long> vtLangSysOffset;
				for (size_t j=0; j<stLangSysRecord; j++)
				{
					long ulLangSysBegin = ftell(m_pFontFile);
					vtLangSysOffset.push_back(ulLangSysBegin-ulScriptBegin);
					
					CHYLangSysRecord& LangSysRecord = ScriptRecord.Script.vtLangSysRecord[j];
					//LangSys table
					//LookupOrder reserved 0
					usTmp = 0;
					fwrite(&usTmp,2,1,m_pFontFile);
					//ReqFeatureIndex
					usTmp = hy_cdr_int16_to(LangSysRecord.LangSys.ReqFeatureIndex);
					fwrite(&usTmp,2,1,m_pFontFile);
					//FeatureCount
					size_t stFeatureCount = LangSysRecord.LangSys.vtFeatureIndex.size();					
					usTmp = hy_cdr_int16_to(static_cast<unsigned short>(stFeatureCount));
					fwrite(&usTmp,2,1,m_pFontFile);
					for (size_t k=0; k<stFeatureCount; k++)
					{
						//FeatureIndex
						usTmp = hy_cdr_int16_to(LangSysRecord.LangSys.vtFeatureIndex[k]);
						fwrite(&usTmp,2,1,m_pFontFile);
					}
				}
				
				g_GsubEndPos = ftell(m_pFontFile);

				// write LangSys offset
				fseek(m_pFontFile,ulScriptBegin+4,SEEK_SET);
				//LangSysRecord offset
				for (size_t j=0; j<stLangSysRecord; j++)
				{
					//LangSysTag
					CHYLangSysRecord& LangSysRecord = ScriptRecord.Script.vtLangSysRecord[j];
					//LangSysRecord Tag
					fwrite(LangSysRecord.SysTag,4,1,m_pFontFile);

					//offset
					usTmp = hy_cdr_int16_to(vtLangSysOffset[j]);
					fwrite(&usTmp,2,1,m_pFontFile);
				}
				fseek(m_pFontFile,g_GsubEndPos,SEEK_SET);

				// write DefaultLangSys offset
				ScriptRecord.Script.DefaultLangSys.FeatureCount = ScriptRecord.Script.DefaultLangSys.vtFeatureIndex.size();
				if (ScriptRecord.Script.DefaultLangSys.FeatureCount>0)
				{						
					fseek(m_pFontFile,ulScriptBegin,SEEK_SET);
					usTmp = hy_cdr_int16_to(g_GsubEndPos-ulScriptBegin);
					fwrite(&usTmp,2,1,m_pFontFile);
					fseek(m_pFontFile,g_GsubEndPos,SEEK_SET);

					//LookupOrder eserved for an offset to a reordering table
					usTmp=0;
					fwrite(&usTmp,2,1,m_pFontFile);
					//ReqFeatureIndex
					usTmp = hy_cdr_int16_to(ScriptRecord.Script.DefaultLangSys.ReqFeatureIndex);
					fwrite(&usTmp,2,1,m_pFontFile);
					//FeatureCount
					size_t stFeatureCount = ScriptRecord.Script.DefaultLangSys.vtFeatureIndex.size();					
					usTmp = hy_cdr_int16_to(static_cast<unsigned short>(stFeatureCount));
					fwrite(&usTmp,2,1,m_pFontFile);
					for (size_t k=0; k<stFeatureCount; k++)
					{
						//FeatureIndex
						usTmp = hy_cdr_int16_to(ScriptRecord.Script.DefaultLangSys.vtFeatureIndex[k]);
						fwrite(&usTmp,2,1,m_pFontFile);
					}
					g_GsubEndPos = ftell(m_pFontFile);
				}
			}						

			// write scriptlist  offset
			fseek(m_pFontFile,ulScriptLstBeginPos+2,SEEK_SET);			
			for (size_t j=0; j<stScriptsList; j++)
			{
				CHYScriptRecord&  ScriptRecord = m_HYGsub.vtScriptsList[j];
				//ScriptTag
				fwrite(ScriptRecord.Tag,4,1,m_pFontFile);
				// offset
				usTmp = hy_cdr_int16_to(vtScriptOffset[j]);
				fwrite(&usTmp,2,1,m_pFontFile);
			}
			fseek(m_pFontFile,g_GsubEndPos,SEEK_SET);
		}

		return HY_NOERROR;

	}	// end of int CHYFontCodec::EncodeScriptList()

	int	CHYFontCodec::EncodeFeatureList()
	{
		if (m_pFontFile)
		{
			unsigned short usTmp = 0;
			size_t stFeatureList = m_HYGsub.vtFeaturesList.size();
			unsigned long ulFeatureLstBeginPos = ftell(m_pFontFile);
			//write FeatureList offset
			fseek(m_pFontFile,g_GsubBeginPos+6,SEEK_SET);
			usTmp = hy_cdr_int16_to(ulFeatureLstBeginPos-g_GsubBeginPos);
			fwrite(&usTmp,2,1,m_pFontFile);
			fseek(m_pFontFile,ulFeatureLstBeginPos,SEEK_SET);

			//FeatureCount			
			usTmp = hy_cdr_int16_to(static_cast<unsigned short>(stFeatureList));
			fwrite(&usTmp,2,1,m_pFontFile);
			
			std::vector<long> vtFeatureOffset;
			//FeatureRecord
			usTmp = 0;
			for (size_t i=0; i<stFeatureList; i++)
			{
				CHYFeatureRecord& FeaturTable = m_HYGsub.vtFeaturesList[i];
				//FeatureTag
				fwrite(FeaturTable.Tag,4,1,m_pFontFile);

				//Feature  0ffset = 0;
				fwrite(&usTmp,2,1,m_pFontFile);
			}

			for (size_t i=0; i<stFeatureList; i++)
			{
				CHYFeatureRecord& FeaturTable = m_HYGsub.vtFeaturesList[i];

				//  Feature offset
				vtFeatureOffset.push_back(ftell(m_pFontFile)-ulFeatureLstBeginPos);

				//FeatureParams 
				usTmp = 0;				
				fwrite(&usTmp,2,1,m_pFontFile);

				//LookupCount
				usTmp = FeaturTable.FeatureTable.vtLookupListIndex.size();
				usTmp = hy_cdr_int16_to(usTmp);
				fwrite(&usTmp,2,1,m_pFontFile);
				
				for (size_t j=0; j<FeaturTable.FeatureTable.vtLookupListIndex.size(); j++)
				{
					usTmp = FeaturTable.FeatureTable.vtLookupListIndex[j];
					usTmp = hy_cdr_int16_to(usTmp);
					fwrite(&usTmp,2,1,m_pFontFile);
				}
			}

			g_GsubEndPos = ftell(m_pFontFile);

			// go to struct	FeatureRecord[FeatureCount] postion
			unsigned long ltmp = ulFeatureLstBeginPos+2;//+2 uint16	FeatureCount
			fseek(m_pFontFile,ltmp,SEEK_SET);
			for (size_t i=0; i<stFeatureList; i++)
			{
				CHYFeatureRecord& FeaturTable = m_HYGsub.vtFeaturesList[i];				
				//FeatureTag
				fwrite(FeaturTable.Tag,4,1,m_pFontFile);

				usTmp = hy_cdr_int16_to(vtFeatureOffset[i]);
				fwrite(&usTmp,2,1,m_pFontFile);

			}
			fseek(m_pFontFile,g_GsubEndPos,SEEK_SET);	
		}

		return HY_NOERROR;

	}	// end of int	CHYFontCodec::EncodeFeatureList()

	int	CHYFontCodec::EncodeLookupList()
	{
		if (m_pFontFile)
		{			
			unsigned short usTmp = 0;
			size_t szLookUpList = m_HYGsub.vtLookupList.size();

			std::vector<long> vtLookupOffset;
			long lLookupLstBegin =  ftell(m_pFontFile);			
			//write FeatureList offset
			fseek(m_pFontFile,g_GsubBeginPos+8,SEEK_SET);
			usTmp = hy_cdr_int16_to(lLookupLstBegin-g_GsubBeginPos);
			fwrite(&usTmp,2,1,m_pFontFile);
			fseek(m_pFontFile,lLookupLstBegin,SEEK_SET);

			//LookupCount
			usTmp = hy_cdr_int16_to(static_cast<unsigned short>(szLookUpList));
			fwrite(&usTmp,2,1,m_pFontFile);
			
			//Offset	Lookup[LookupCount]			
			for(size_t i=0; i<szLookUpList; i++)
			{
				usTmp = 0;
				fwrite(&usTmp,2,1,m_pFontFile);
			}			
			
			std::vector<long> vtLookUpBegin;
			for (size_t i=0; i<szLookUpList; i++)
			{	
				// look up begin position
				long lLookUpBegin =  ftell(m_pFontFile);
				vtLookUpBegin.push_back(lLookUpBegin);
				// save  SubTables offset from beginning of Lookup table 
				vtLookupOffset.push_back(lLookUpBegin-lLookupLstBegin);

				CLookUp& lookup = m_HYGsub.vtLookupList[i];
				//LookupType
				usTmp = hy_cdr_int16_to(static_cast<unsigned short>(lookup.LookUpType));
				fwrite(&usTmp,2,1,m_pFontFile);
				//LookupFlag
				usTmp = hy_cdr_int16_to(static_cast<unsigned short>(lookup.LookUpFlag));
				fwrite(&usTmp,2,1,m_pFontFile);
				//SubTableCount
				usTmp = hy_cdr_int16_to(static_cast<unsigned short>(lookup.SubTableCount));
				fwrite(&usTmp,2,1,m_pFontFile);
				
				//SubTable Array of offsets to SubTables-from beginning of Lookup table
				usTmp = 0;					
				for (size_t j=0; j<lookup.SubTableCount;j++)
				{
					fwrite(&usTmp,2,1,m_pFontFile);						
				}
								
				//MarkFilteringSet 0
				usTmp = 0;
				fwrite(&usTmp,2,1,m_pFontFile);
			}
			
			for (size_t i=0; i<szLookUpList; i++)
			{
				CLookUp& lookup = m_HYGsub.vtLookupList[i];
				//offset Array of offsets to SubTables-from beginning of Lookup table
				std::vector<long> vtSubTableOffset;
				//SubTable Single Substitution 
				if (lookup.LookUpType==1)
				{					
					for (size_t j=0; j<lookup.SubTableCount;j++)
					{
						CLookUpType1& LookUpType1 = lookup.vtLookupType1[j];
						//Single Substitution begin position
						long lSubstitutionBegin =  ftell(m_pFontFile);
						// save  SubTables offset from beginning of Lookup table 
						vtSubTableOffset.push_back(lSubstitutionBegin-vtLookUpBegin[i]);

						//SubstFormat
						usTmp = hy_cdr_int16_to(LookUpType1.SubFormat);
						fwrite(&usTmp,2,1,m_pFontFile);
						//Coverage	Offset to Coverage table-from beginning of Substitution table
						usTmp = 0;
						fwrite(&usTmp,2,1,m_pFontFile);

						if (LookUpType1.SubFormat==2)
						{
							CLookUpSingleSubstitution2&  Substitution2 = LookUpType1.Substitution2;							
							//GlyphCount
							usTmp = hy_cdr_int16_to(Substitution2.GlyphCount);
							fwrite(&usTmp,2,1,m_pFontFile);

							//Substitute Array of substitute GlyphIDs-ordered by Coverage Index
							for (unsigned short x=0; x<Substitution2.GlyphCount; x++)
							{
								usTmp = hy_cdr_int16_to(Substitution2.vtGlyphID[x]);
								fwrite(&usTmp,2,1,m_pFontFile);
							}

							//write Coverage offset
							g_GsubEndPos = ftell(m_pFontFile);
							unsigned short sSubstitutionOffset =  (unsigned short)(g_GsubEndPos - lSubstitutionBegin);
							fseek(m_pFontFile,lSubstitutionBegin+2,SEEK_SET);
							usTmp = hy_cdr_int16_to(sSubstitutionOffset);
							fwrite(&usTmp,2,1,m_pFontFile);
							// seek to file end 
							fseek(m_pFontFile,g_GsubEndPos,SEEK_SET);

							//Coverage
							usTmp = hy_cdr_int16_to(Substitution2.Coverage.CoverageFormat);
							fwrite(&usTmp,2,1,m_pFontFile);

							if (Substitution2.Coverage.CoverageFormat == 1)
							{								
								usTmp = hy_cdr_int16_to(Substitution2.Coverage.GlyphCount);
								fwrite(&usTmp,2,1,m_pFontFile);
								for (unsigned short x=0; x<Substitution2.Coverage.GlyphCount;x++)
								{
									usTmp = hy_cdr_int16_to(Substitution2.Coverage.vtGlyphID[x]);
									fwrite(&usTmp,2,1,m_pFontFile);
								}
							}
							g_GsubEndPos = ftell(m_pFontFile);
						}
					}
				}

				// write  SubTables offset from beginning of Lookup table 
				fseek(m_pFontFile,vtLookUpBegin[i]+6,SEEK_SET);
				for (unsigned short us =0; us<lookup.SubTableCount; us++)
				{
					usTmp = hy_cdr_int16_to(vtSubTableOffset[us]);					
					fwrite(&usTmp,2,1,m_pFontFile);
				}

				fseek(m_pFontFile,g_GsubEndPos,SEEK_SET);

			}

			// write  Lookup  offset from beginning of LookupList -zero based 
			fseek(m_pFontFile,lLookupLstBegin+2,SEEK_SET);
			for (size_t us =0; us<szLookUpList; us++)
			{
				usTmp = hy_cdr_int16_to(vtLookupOffset[us]);					
				fwrite(&usTmp,2,1,m_pFontFile);
			}

			fseek(m_pFontFile,g_GsubEndPos,SEEK_SET);
			
		}

		return HY_NOERROR;

	}	// end of int	CHYFontCodec::EncodeLookupList()

	int CHYFontCodec::EncodeGPOS() 
	{
		if (m_pFontFile) {			
			int iEntryIndex = m_HYTbDirectory.FindTableEntry(GPOS_TAG);
			if (iEntryIndex == -1) return FONT_ERR_GPOS_ENCODE;
			CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
			tbEntry.offset = ftell(m_pFontFile);

			TableData* pGSUBData = m_mulpTableData.FindTableData(GPOS_TAG);
			if (pGSUBData) {
				for (int i = 0; i < pGSUBData->ulDataSize; i++) {
					fwrite(&pGSUBData->vtData[i], 1, 1, m_pFontFile);
				}
			}

			tbEntry.length = ftell(m_pFontFile) - tbEntry.offset;
			::HY_4ByteAlign(m_pFontFile,tbEntry.length);
		}

	}	// end of int CHYFontCodec::EncodeGPOS() 

	int	CHYFontCodec::DecodeJSTF()
	{

		return HY_NOERROR;

	}	// end of int	CHYFontCodec::DecodeJSTF()	

	int CHYFontCodec::MakeGasp_ClearType()
	{
		m_HYGasp.SetDefault();

		m_HYGasp.Head.version = 1;
		m_HYGasp.Head.numRanges = 3;

		CHYGasp_RangeRecord RangeRecord;
		RangeRecord.rangeMaxPPEM = 8;
		RangeRecord.rangeGaspBehavior = 0x000e;
		m_HYGasp.vtRangeRecord.push_back(RangeRecord);

		RangeRecord.rangeMaxPPEM = 16;
		RangeRecord.rangeGaspBehavior = 0x0007;
		m_HYGasp.vtRangeRecord.push_back(RangeRecord);

		RangeRecord.rangeMaxPPEM = 65535;
		RangeRecord.rangeGaspBehavior = 0x000f;

		m_HYGasp.vtRangeRecord.push_back(RangeRecord);

		return HY_NOERROR;

	}	// end of int CHYFontCodec::MakeGasp_ClearType()

	int	CHYFontCodec::Decodegasp()
	{
		m_HYGasp.SetDefault();

		if (m_pFontFile)
		{
			int iEntryIndex = m_HYTbDirectory.FindTableEntry(GASP_TAG);
			if (iEntryIndex == -1) return FONT_ERR_GASP_DECODE;

			CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
			fseek(m_pFontFile,tbEntry.offset,SEEK_SET);

			fread(&m_HYGasp.Head.version,2,1,m_pFontFile);
			m_HYGasp.Head.version = hy_cdr_int16_to(m_HYGasp.Head.version);
			fread(&m_HYGasp.Head.numRanges,2,1,m_pFontFile);
			m_HYGasp.Head.numRanges = hy_cdr_int16_to(m_HYGasp.Head.numRanges);

			for (size_t i=0; i<m_HYGasp.Head.numRanges; i++)
			{
				CHYGasp_RangeRecord	RangeRecord;

				fread(&RangeRecord.rangeMaxPPEM,2,1,m_pFontFile);
				RangeRecord.rangeMaxPPEM = hy_cdr_int16_to(RangeRecord.rangeMaxPPEM);

				fread(&RangeRecord.rangeGaspBehavior,2,1,m_pFontFile);
				RangeRecord.rangeGaspBehavior = hy_cdr_int16_to(RangeRecord.rangeGaspBehavior);

				m_HYGasp.vtRangeRecord.push_back(RangeRecord);
			}
		}

		return HY_NOERROR;

	}	// end of int CHYFontCodec::Decodegasp()

	int	CHYFontCodec::Encodegasp()
	{
		if (m_pFontFile)
		{
			unsigned short	usTmp = 0;
			unsigned long	ulTmp = 0;

			int iEntryIndex = m_HYTbDirectory.FindTableEntry(GASP_TAG);
			if (iEntryIndex == -1) return FONT_ERR_GASP_ENCODE;

			CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
			tbEntry.offset = ftell(m_pFontFile);

			//Table version number
			m_HYVhea.version.value = 1;
			m_HYVhea.version.fract = 0;
			usTmp = hy_cdr_int16_to(m_HYGasp.Head.version);
			fwrite(&usTmp,2,1,m_pFontFile);
			usTmp = hy_cdr_int16_to(m_HYGasp.Head.numRanges);
			fwrite(&usTmp,2,1,m_pFontFile);
			
			for (size_t i=0; i<m_HYGasp.vtRangeRecord.size(); i++)
			{
				CHYGasp_RangeRecord& RangeRecord = m_HYGasp.vtRangeRecord[i];
				usTmp = hy_cdr_int16_to(RangeRecord.rangeMaxPPEM);
				fwrite(&usTmp,2,1,m_pFontFile);
				usTmp = hy_cdr_int16_to(RangeRecord.rangeGaspBehavior);
				fwrite(&usTmp,2,1,m_pFontFile);
			}

			tbEntry.length = ftell(m_pFontFile) - tbEntry.offset;
			int iTail = 4-tbEntry.length%4;
			if (tbEntry.length%4>0)
			{
				char c = 0;
				for (int i=0; i<iTail; i++)
				{
					fwrite(&c,1,1,m_pFontFile);
				}				
			}
		}

		return  HY_NOERROR;

	}	// end of int CHYFontCodec::Encodegasp()

	int	CHYFontCodec::Decodehdmx()
	{
		return HY_NOERROR;

	}	// end of int CHYFontCodec::Decodehdmx()

	int CHYFontCodec::Decodekern()
	{

		return HY_NOERROR;

	}	// end of int CHYFontCodec::Decodekern()

	int CHYFontCodec::DecodeLTSH()
	{

		return HY_NOERROR;

	}	// end of int CHYFontCodec::DecodeLTSH()

	int	CHYFontCodec::DecodePCLT()
	{

		return HY_NOERROR;

	}	// end of int CHYFontCodec::DecodePCLT()

	int	CHYFontCodec::DecodeVDMX()
	{
		return HY_NOERROR;

	}	// end of int CHYFontCodec::DecodeVDMX()

	int	CHYFontCodec::Decodevhea()
	{
		m_HYVhea.SetDefault();
		if (m_pFontFile)
		{
			int iEntryIndex = m_HYTbDirectory.FindTableEntry(VHEA_TAG);
			if (iEntryIndex == -1) return FONT_ERR_VHEA_DECODE;

			CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
			fseek(m_pFontFile,tbEntry.offset,SEEK_SET);

			fread(&m_HYVhea.version.value,2,1,m_pFontFile);
			m_HYVhea.version.value = hy_cdr_int16_to(m_HYVhea.version.value);
			fread(&m_HYVhea.version.fract,2,1,m_pFontFile);
			m_HYVhea.version.fract = hy_cdr_int16_to(m_HYVhea.version.fract);

			fread(&m_HYVhea.ascent,2,1,m_pFontFile);
			m_HYVhea.ascent = hy_cdr_int16_to(m_HYVhea.ascent);
			fread(&m_HYVhea.descent,2,1,m_pFontFile);
			m_HYVhea.descent = hy_cdr_int16_to(m_HYVhea.descent);
			fread(&m_HYVhea.lineGap,2,1,m_pFontFile);
			m_HYVhea.lineGap = hy_cdr_int16_to(m_HYVhea.lineGap);
			fread(&m_HYVhea.advanceHeightMax,2,1,m_pFontFile);
			m_HYVhea.advanceHeightMax = hy_cdr_int16_to(m_HYVhea.advanceHeightMax);
			fread(&m_HYVhea.minTop,2,1,m_pFontFile);
			m_HYVhea.minTop = hy_cdr_int16_to(m_HYVhea.minTop);
			fread(&m_HYVhea.minBottom,2,1,m_pFontFile);
			m_HYVhea.minBottom = hy_cdr_int16_to(m_HYVhea.minBottom);
			fread(&m_HYVhea.yMaxExtent,2,1,m_pFontFile);
			m_HYVhea.yMaxExtent = hy_cdr_int16_to(m_HYVhea.yMaxExtent);
			fread(&m_HYVhea.caretSlopeRise,2,1,m_pFontFile);
			m_HYVhea.caretSlopeRise = hy_cdr_int16_to(m_HYVhea.caretSlopeRise);
			fread(&m_HYVhea.caretSlopeRun,2,1,m_pFontFile);
			m_HYVhea.caretSlopeRun = hy_cdr_int16_to(m_HYVhea.caretSlopeRun);
			fread(&m_HYVhea.caretOffset,2,1,m_pFontFile);
			m_HYVhea.caretOffset = hy_cdr_int16_to(m_HYVhea.caretOffset);

			fread(&m_HYVhea.reserved1,2,1,m_pFontFile);
			m_HYVhea.reserved1 = hy_cdr_int16_to(m_HYVhea.reserved1);
			fread(&m_HYVhea.reserved2,2,1,m_pFontFile);
			m_HYVhea.reserved2 = hy_cdr_int16_to(m_HYVhea.reserved2);
			fread(&m_HYVhea.reserved3,2,1,m_pFontFile);
			m_HYVhea.reserved3 = hy_cdr_int16_to(m_HYVhea.reserved3);
			fread(&m_HYVhea.reserved4,2,1,m_pFontFile);
			m_HYVhea.reserved4 = hy_cdr_int16_to(m_HYVhea.reserved4);

			fread(&m_HYVhea.metricDataFormat,2,1,m_pFontFile);
			m_HYVhea.metricDataFormat = hy_cdr_int16_to(m_HYVhea.metricDataFormat);

			fread(&m_HYVhea.numOfLongVerMetrics,2,1,m_pFontFile);
			m_HYVhea.numOfLongVerMetrics = hy_cdr_int16_to(m_HYVhea.numOfLongVerMetrics);		
		}

		return HY_NOERROR;

	}	// end of int CHYFontCodec::Decodevhea()

	int CHYFontCodec::Encodevhea()
	{			
		if (m_pFontFile){	
			unsigned short	usTmp = 0;
			unsigned long	ulTmp = 0;

			int iEntryIndex = m_HYTbDirectory.FindTableEntry(VHEA_TAG);
			if (iEntryIndex == -1) return FONT_ERR_VHEA_ENCODE;

			CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
			tbEntry.offset = ftell(m_pFontFile);

			//Table version number
			m_HYVhea.version.value = 1;
			m_HYVhea.version.fract = 0;
			usTmp = hy_cdr_int16_to(m_HYVhea.version.value);
			fwrite(&usTmp,2,1,m_pFontFile);
			usTmp = hy_cdr_int16_to(m_HYVhea.version.fract);
			fwrite(&usTmp,2,1,m_pFontFile);
			// ascent
			usTmp = hy_cdr_int16_to(m_HYVhea.ascent);
			fwrite(&usTmp,2,1,m_pFontFile);
			// descent
			usTmp = hy_cdr_int16_to(m_HYVhea.descent);
			fwrite(&usTmp,2,1,m_pFontFile);
			// lineGap
			usTmp = hy_cdr_int16_to(m_HYVhea.lineGap);
			fwrite(&usTmp,2,1,m_pFontFile);
			// advanceHeightMax
			usTmp = hy_cdr_int16_to(m_HYVhea.advanceHeightMax);
			fwrite(&usTmp,2,1,m_pFontFile);
			// minTopSideBearing
			usTmp = hy_cdr_int16_to(m_HYVhea.minTop);
			fwrite(&usTmp,2,1,m_pFontFile);
			//minBottomSideBearing
			usTmp = hy_cdr_int16_to(m_HYVhea.minBottom);
			fwrite(&usTmp,2,1,m_pFontFile);
			//yMaxExtent
			usTmp = hy_cdr_int16_to(m_HYVhea.yMaxExtent);
			fwrite(&usTmp,2,1,m_pFontFile);
			//caretSlopeRise
			usTmp = hy_cdr_int16_to(m_HYVhea.caretSlopeRise);
			fwrite(&usTmp,2,1,m_pFontFile);
			// caretSlopeRun
			usTmp = hy_cdr_int16_to(m_HYVhea.caretSlopeRun);
			fwrite(&usTmp,2,1,m_pFontFile);
			//caretOffset
			usTmp = hy_cdr_int16_to(m_HYVhea.caretOffset);
			fwrite(&usTmp,2,1,m_pFontFile);
			//reserved
			usTmp = hy_cdr_int16_to(m_HYVhea.reserved1);
			fwrite(&usTmp,2,1,m_pFontFile);
			//reserved
			usTmp = hy_cdr_int16_to(m_HYVhea.reserved2);
			fwrite(&usTmp,2,1,m_pFontFile);
			//reserved
			usTmp = hy_cdr_int16_to(m_HYVhea.reserved3);
			fwrite(&usTmp,2,1,m_pFontFile);
			//reserved
			usTmp = hy_cdr_int16_to(m_HYVhea.reserved4);
			fwrite(&usTmp,2,1,m_pFontFile);
			//metricDataFormat
			usTmp = hy_cdr_int16_to(m_HYVhea.metricDataFormat);
			fwrite(&usTmp,2,1,m_pFontFile);
			//numOfLongVerMetrics
			usTmp = hy_cdr_int16_to(m_HYVhea.numOfLongVerMetrics);
			fwrite(&usTmp,2,1,m_pFontFile);

			tbEntry.length = ftell(m_pFontFile) - tbEntry.offset;
			int iTail = 4-tbEntry.length%4;
			if (tbEntry.length%4>0)
			{
				char c = 0;
				for (int i=0; i<iTail; i++)
				{
					fwrite(&c,1,1,m_pFontFile);
				}				
			}
		}

		return HY_NOERROR;

	}	// end of int CHYFontCodec::Encodevhea()

	int CHYFontCodec::Decodevmtx()
	{
		m_HYVmtx.SetDefault();
		if (m_pFontFile)
		{
			int iEntryIndex = m_HYTbDirectory.FindTableEntry(VMTX_TAG);
			if (iEntryIndex == -1) return FONT_ERR_VMTX_DECODE;

			CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
			fseek(m_pFontFile,tbEntry.offset,SEEK_SET);

			unsigned short uVmetricsNum = m_HYVhea.numOfLongVerMetrics;
			for (unsigned short i=0; i<uVmetricsNum; i++)
			{
				VMTX_LONGVRTMETRIC  Longhormetric;

				fread(&Longhormetric.advanceHeight,2,1,m_pFontFile);
				Longhormetric.advanceHeight = hy_cdr_int16_to(Longhormetric.advanceHeight);

				fread(&Longhormetric.tsb,2,1,m_pFontFile);
				Longhormetric.tsb = hy_cdr_int16_to(Longhormetric.tsb);

				m_HYVmtx.vtLongVrtmetric.push_back(Longhormetric);
			}

			unsigned short  uTopSideBearingNum = m_HYMaxp.numGlyphs-uVmetricsNum;
			short sTopSideBearing = 0;
			for (unsigned short  i=0; i<uTopSideBearingNum; i++)
			{
				fread(&sTopSideBearing,2,1,m_pFontFile);
				sTopSideBearing = hy_cdr_int16_to(sTopSideBearing);
				m_HYVmtx.vtTopsidebearing.push_back(sTopSideBearing);
			}
		}

		return HY_NOERROR;

	}	// end of int CHYFontCodec::Decodevmtx()

	int CHYFontCodec::Encodevmtx()
	{
		if (m_pFontFile)
		{
			unsigned short	usTmp = 0;
			unsigned long	ulTmp = 0;

			int iEntryIndex = m_HYTbDirectory.FindTableEntry(VMTX_TAG);
			if (iEntryIndex == -1) return FONT_ERR_VMTX_ENCODE;

			CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
			tbEntry.offset = ftell(m_pFontFile);

			for (unsigned short i = 0; i<m_HYVhea.numOfLongVerMetrics; i++){
				usTmp = hy_cdr_int16_to(m_HYVmtx.vtLongVrtmetric[i].advanceHeight);
				fwrite(&usTmp,2,1,m_pFontFile);
				usTmp = hy_cdr_int16_to(m_HYVmtx.vtLongVrtmetric[i].tsb);
				fwrite(&usTmp,2,1,m_pFontFile);			
			}
			size_t stTopsidebearNum = m_HYVmtx.vtTopsidebearing.size();
			for (size_t i = 0; i < stTopsidebearNum; i++ ){			 
				usTmp = hy_cdr_int16_to(m_HYVmtx.vtTopsidebearing[i]);
				fwrite(&usTmp,2,1,m_pFontFile);
			}
			tbEntry.length = ftell(m_pFontFile) - tbEntry.offset;
			int iTail = 4-tbEntry.length%4;
			if (tbEntry.length%4>0)	{
				char c = 0;
				for (int i=0; i<iTail; i++)	{
					fwrite(&c,1,1,m_pFontFile);
				}				
			}
		}

		return HY_NOERROR;

	}	// end of int CHYFontCodec::Encodevmtx()

	int CHYFontCodec::Decodesbix()
	{
		m_HYsbix.setdefault();
		if (m_pFontFile)
		{
			int iEntryIndex = m_HYTbDirectory.FindTableEntry(SBIX_TAG);
			if (iEntryIndex == -1) return FONT_ERR_SBIX_DECODE;

			CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
			fseek(m_pFontFile,tbEntry.offset,SEEK_SET);

			fread(&m_HYsbix.version,2,1,m_pFontFile);
			m_HYsbix.version = hy_cdr_int16_to(m_HYsbix.version);
			fread(&m_HYsbix.flags,2,1,m_pFontFile);
			m_HYsbix.flags = hy_cdr_int16_to(m_HYsbix.flags);
			fread(&m_HYsbix.numStrikes,4,1,m_pFontFile);
			m_HYsbix.numStrikes = hy_cdr_int32_to(m_HYsbix.numStrikes);

			unsigned long Strikesoffset = 0;
			for (unsigned int i=0; i<m_HYsbix.numStrikes; i++)
			{				
				fread(&Strikesoffset,4,1,m_pFontFile);
				Strikesoffset = hy_cdr_int32_to(Strikesoffset);
				m_HYsbix.vtstrikeOffset.push_back(Strikesoffset);			
			}

			for (unsigned int i=0; i<m_HYsbix.numStrikes; i++)
			{				
				Strikesoffset = m_HYsbix.vtstrikeOffset[i];
				fseek(m_pFontFile,tbEntry.offset+Strikesoffset,SEEK_SET);

				CSbixStrikes	SbixStrikes;
				fread(&SbixStrikes.ppem,2,1,m_pFontFile);
				SbixStrikes.ppem = hy_cdr_int16_to(SbixStrikes.ppem);

				fread(&SbixStrikes.resolution,2,1,m_pFontFile);
				SbixStrikes.resolution = hy_cdr_int16_to(SbixStrikes.resolution);

				unsigned long glyphDataOffset = 0;
				fread(&glyphDataOffset,4,1,m_pFontFile);
				glyphDataOffset = hy_cdr_int32_to(glyphDataOffset);
				SbixStrikes.vtglyphDataOffset.push_back(glyphDataOffset);

				for (unsigned int j=0; j<m_HYMaxp.numGlyphs; j++)
				{
					glyphDataOffset = 0;	
					fread(&glyphDataOffset,4,1,m_pFontFile);
					glyphDataOffset = hy_cdr_int32_to(glyphDataOffset);
					SbixStrikes.vtglyphDataOffset.push_back(glyphDataOffset);
				}

				unsigned long psStrike = ftell(m_pFontFile);
				for (unsigned int j=0; j<m_HYMaxp.numGlyphs; j++)
				{	
					unsigned long ulDataLeng = SbixStrikes.vtglyphDataOffset[j+1] - SbixStrikes.vtglyphDataOffset[j];
					if(ulDataLeng>0)
					{
						CStrikesDatarecord		Datarecord;
						fseek(m_pFontFile,tbEntry.offset+Strikesoffset+SbixStrikes.vtglyphDataOffset[j],SEEK_SET);

						fread(&Datarecord.originOffsetX,2,1,m_pFontFile);
						Datarecord.originOffsetX = hy_cdr_int16_to(Datarecord.originOffsetX);

						fread(&Datarecord.originOffsetY,2,1,m_pFontFile);
						Datarecord.originOffsetY= hy_cdr_int16_to(Datarecord.originOffsetY);

						fread(&Datarecord.graphicType[0],1,1,m_pFontFile);
						fread(&Datarecord.graphicType[1],1,1,m_pFontFile);
						fread(&Datarecord.graphicType[2],1,1,m_pFontFile);
						fread(&Datarecord.graphicType[3],1,1,m_pFontFile);
						
						char strGType[5] = {0};
						memcpy_s(strGType,5,Datarecord.graphicType,4);
						if (strGType=="dupe")
						{
							int iReserve = 0;
						}
						else if (strGType=="mask")
						{
							int iReserve = 0;
						}
						else 
						{
							unsigned int realLength =  ulDataLeng - 8;
							for (unsigned int x=0; x<realLength; x++)
							{
								unsigned char Data;
								fread(&Data,1,1,m_pFontFile);
								Datarecord.vtdata.push_back(Data);
							}
						}

						Datarecord.gid = j;
						SbixStrikes.vtStrikesDatarecord.push_back(Datarecord);				
					}					
				}
				fseek(m_pFontFile,psStrike,SEEK_SET);
				m_HYsbix.vtSbixStrikes.push_back(SbixStrikes);

			}
		}
		
		return HY_NOERROR;

	}	// end of int CHYFontCodec::Decodesbix()

	int CHYFontCodec::Encodesbix()
	{
		if (m_pFontFile)
		{
			unsigned short	usTmp = 0;
			unsigned long	ulTmp = 0;

			int iEntryIndex = m_HYTbDirectory.FindTableEntry(SBIX_TAG);
			if (iEntryIndex == -1) return FONT_ERR_SBIX_ENCODE;

			CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
			tbEntry.offset = ftell(m_pFontFile);

			usTmp = hy_cdr_int16_to(m_HYsbix.version);
			fwrite(&usTmp,2,1,m_pFontFile);
			usTmp = hy_cdr_int16_to(m_HYsbix.flags);
			fwrite(&usTmp,2,1,m_pFontFile);
			ulTmp = hy_cdr_int32_to(m_HYsbix.numStrikes);
			fwrite(&ulTmp,4,1,m_pFontFile);

			ulTmp=0;
			for (unsigned long i=0; i<m_HYsbix.numStrikes; i++)
			{
				fwrite(&ulTmp,4,1,m_pFontFile);
			}

			std::vector<long> vtstrikeOffset;			
			unsigned long currentPos;
			for (unsigned long i=0; i<m_HYsbix.numStrikes; i++)
			{
				currentPos = ftell(m_pFontFile);
				unsigned long offset = currentPos - tbEntry.offset;
				vtstrikeOffset.push_back(offset);

				CSbixStrikes&  sbixstrikes = m_HYsbix.vtSbixStrikes[i];
				usTmp = hy_cdr_int16_to(sbixstrikes.ppem);
				fwrite(&usTmp,2,1,m_pFontFile);
				usTmp = hy_cdr_int16_to(sbixstrikes.resolution);
				fwrite(&usTmp,2,1,m_pFontFile);

				ulTmp = 0;
				unsigned long glyphDataOffsetPos = ftell(m_pFontFile);
				for (unsigned long j=0; j<m_HYMaxp.numGlyphs;j++)
				{
					fwrite(&ulTmp,4,1,m_pFontFile);
				}			
				
				std::vector<long> vtGlyphOffset;
				for (unsigned long j=0; j<m_HYMaxp.numGlyphs;j++)
				{
					vtGlyphOffset.push_back(currentPos-ftell(m_pFontFile));
					CStrikesDatarecord& Datarecord = sbixstrikes.vtStrikesDatarecord[j];
					if (Datarecord.gid==j) 
					{
						usTmp = hy_cdr_int16_to(Datarecord.originOffsetX);
						fwrite(&usTmp,2,1,m_pFontFile);
						usTmp = hy_cdr_int16_to(Datarecord.originOffsetY);
						fwrite(&usTmp,2,1,m_pFontFile);

						fwrite(&Datarecord.graphicType[0],1,1,m_pFontFile);
						fwrite(&Datarecord.graphicType[1],1,1,m_pFontFile);
						fwrite(&Datarecord.graphicType[2],1,1,m_pFontFile);
						fwrite(&Datarecord.graphicType[3],1,1,m_pFontFile);

						size_t stDatasize = Datarecord.vtdata.size();
						for (size_t x=0; x<stDatasize; x++)
						{
							fwrite(&Datarecord.graphicType[0],1,1,m_pFontFile);
						}
					}
				}

				currentPos = ftell(m_pFontFile);
				fseek(m_pFontFile,glyphDataOffsetPos,SEEK_SET);
				for (unsigned long j=0; j<m_HYMaxp.numGlyphs;j++)
				{
					ulTmp = vtGlyphOffset[j];
					fwrite(&ulTmp,4,1,m_pFontFile);
				}

				fseek(m_pFontFile,currentPos,SEEK_SET);
			}

			currentPos = ftell(m_pFontFile);
			fseek(m_pFontFile,tbEntry.offset+8,SEEK_SET);
			for (unsigned long i=0; i<m_HYsbix.numStrikes; i++)
			{
				ulTmp = vtstrikeOffset[i];
				fwrite(&ulTmp,4,1,m_pFontFile);
			}
			fseek(m_pFontFile,currentPos,SEEK_SET);
		}

		return HY_NOERROR;

	}	// end of int CHYFontCodec::Encodesbix()

	int CHYFontCodec::DecodeCVT()
	{
		m_HYCvt.vtCvt.clear();

		int iEntryIndex = m_HYTbDirectory.FindTableEntry(CVT_TAG);
		if (iEntryIndex == -1) return FONT_ERR_CVT_DECODE;

		CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
		fseek(m_pFontFile,tbEntry.offset,SEEK_SET);		

		int i=0;
		while (i<tbEntry.length)
		{
			short sTmp=0;
			fread(&sTmp,2,1,m_pFontFile);
			sTmp = hy_cdr_int16_to(sTmp);
			i+=2;

			m_HYCvt.vtCvt.push_back(sTmp);
		}

		return HY_NOERROR;

	}	// end of int CHYFontCodec::DecodeCVT()

	int	CHYFontCodec::EncodeCVT()
	{
		if (m_pFontFile)
		{

			//冗余 
			m_HYCvt.vtCvt.push_back(16);
			// end 冗余

			short	sTmp = 0;	

			int iEntryIndex = m_HYTbDirectory.FindTableEntry(CVT_TAG);
			if (iEntryIndex == -1) return FONT_ERR_CVT_ENCODE;

			CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
			tbEntry.offset = ftell(m_pFontFile);
			
			for (size_t i=0; i<m_HYCvt.vtCvt.size(); i++)
			{
				sTmp = hy_cdr_int16_to(m_HYCvt.vtCvt[i]);
				fwrite(&sTmp,2,1,m_pFontFile);
			}


			tbEntry.length = ftell(m_pFontFile) - tbEntry.offset;
			int iTail = 4-tbEntry.length%4;
			if (tbEntry.length%4>0)
			{
				char c = 0;
				for (int i=0; i<iTail; i++)
				{
					fwrite(&c,1,1,m_pFontFile);
				}				
			}
		}

		return HY_NOERROR;

	}	// end of int	CHYFontCodec::EncodeCVT()

	int CHYFontCodec::Decodefpgm()
	{
		m_HYfpgm.clear();

		int iEntryIndex = m_HYTbDirectory.FindTableEntry(FPGM_TAG);
		if (iEntryIndex == -1) return FONT_ERR_FPGM_DECODE;

		CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
		fseek(m_pFontFile,tbEntry.offset,SEEK_SET);		

		int i=0;
		while (i<tbEntry.length)
		{
			byte bTmp=0;
			fread(&bTmp,1,1,m_pFontFile);			
			i++;

			m_HYfpgm.push_back(bTmp);
		}

		return HY_NOERROR;

	}	// end of int CHYFontCodec::Decodefpgm()

	int	CHYFontCodec::Encodefpgm()
	{
		if (m_pFontFile)
		{
			// 冗余
			m_HYfpgm.clear();
			m_HYfpgm.push_back(0xb8);
			m_HYfpgm.push_back(0x00);
			m_HYfpgm.push_back(0x09);
			// end 冗余

			int iEntryIndex = m_HYTbDirectory.FindTableEntry(FPGM_TAG);
			if (iEntryIndex == -1) return FONT_ERR_FPGM_ENCODE;

			CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
			tbEntry.offset = ftell(m_pFontFile);

			for (size_t i=0; i<m_HYfpgm.size(); i++)
			{				
				fwrite(&m_HYfpgm[i],1,1,m_pFontFile);
			}

			tbEntry.length = ftell(m_pFontFile) - tbEntry.offset;
			int iTail = 4-tbEntry.length%4;
			if (tbEntry.length%4>0)
			{
				char c = 0;
				for (int i=0; i<iTail; i++)
				{
					fwrite(&c,1,1,m_pFontFile);
				}				
			}
		}

		return HY_NOERROR;

	}	// end of int	CHYFontCodec::Encodefpgm()

	int CHYFontCodec::DecodeVORG()
	{
		int iEntryIndex = m_HYTbDirectory.FindTableEntry(VORG_TAG);		
		if (iEntryIndex == -1) return FONT_ERR_FPGM_DECODE;

		m_HYVORG.SetDefault();

		CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
		fseek(m_pFontFile,tbEntry.offset,SEEK_SET);		
		
		short	sTmp = 0;
		fread(&sTmp,2,1,m_pFontFile);
		m_HYVORG.majorVersion = hy_cdr_int16_to(sTmp);
		fread(&sTmp,2,1,m_pFontFile);
		m_HYVORG.minorVersion = hy_cdr_int16_to(sTmp);
		fread(&sTmp,2,1,m_pFontFile);
		m_HYVORG.defaultVertOriginY = hy_cdr_int16_to(sTmp);
		fread(&sTmp,2,1,m_pFontFile);
		m_HYVORG.numVertOriginYMetrics = hy_cdr_int16_to(sTmp);
	
		for (unsigned short i=0; i<m_HYVORG.numVertOriginYMetrics; i++)
		{
			CHYVertOriginYMetrics	vrtOrgnYMtrcs;
			fread(&sTmp,2,1,m_pFontFile);
			vrtOrgnYMtrcs.glyphIndex = hy_cdr_int16_to(sTmp);
			fread(&sTmp,2,1,m_pFontFile);
			vrtOrgnYMtrcs.vertOriginY = hy_cdr_int16_to(sTmp);

			m_HYVORG.vtVertOriginYMetrics.push_back(vrtOrgnYMtrcs);
		}

		return HY_NOERROR;

	}	// end of int CHYFontCodec::DecodeVORG()

	int	CHYFontCodec::EncodeVORG()
	{
		CountVORG();

		int iEntryIndex = m_HYTbDirectory.FindTableEntry(VORG_TAG);
		if (iEntryIndex == -1) return FONT_ERR_VORG_ENCODE;

		short sTmp=0;
		CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
		tbEntry.offset = ftell(m_pFontFile);

		sTmp = hy_cdr_int16_to(m_HYVORG.majorVersion);
		fwrite(&sTmp,2,1,m_pFontFile);
		sTmp = hy_cdr_int16_to(m_HYVORG.minorVersion);
		fwrite(&sTmp,2,1,m_pFontFile);
		sTmp = hy_cdr_int16_to(m_HYVORG.defaultVertOriginY);
		fwrite(&sTmp,2,1,m_pFontFile);
		sTmp = hy_cdr_int16_to(m_HYVORG.numVertOriginYMetrics);
		fwrite(&sTmp,2,1,m_pFontFile);

		for (unsigned short i=0; i<m_HYVORG.numVertOriginYMetrics; i++)
		{
			CHYVertOriginYMetrics& OrgnYMtrics = m_HYVORG.vtVertOriginYMetrics[i];

			sTmp = hy_cdr_int16_to(OrgnYMtrics.glyphIndex);
			fwrite(&sTmp,2,1,m_pFontFile);
			sTmp = hy_cdr_int16_to(OrgnYMtrics.vertOriginY);
			fwrite(&sTmp,2,1,m_pFontFile);
		}

		tbEntry.length = ftell(m_pFontFile) - tbEntry.offset;
		int iTail = 4-tbEntry.length%4;
		if (tbEntry.length%4>0)
		{
			char c = 0;
			for (int i=0; i<iTail; i++)
			{
				fwrite(&c,1,1,m_pFontFile);
			}				
		}

		return HY_NOERROR;

	}	// end of int	CHYFontCodec::EncodeVORG()

	void CHYFontCodec::CountVORG()
	{
		m_HYVORG.majorVersion = 1;
		m_HYVORG.minorVersion = 0;
		m_HYVORG.defaultVertOriginY = m_HYOS2.sTypoAscender;
		m_HYVORG.numVertOriginYMetrics = 0;

		size_t stGlyphNum = m_vtHYGlyphs.size();
		m_HYVORG.vtVertOriginYMetrics.clear();
		
		VMTX_LONGVRTMETRIC vrtMetoic;
		for (size_t i=0; i<stGlyphNum; i++)
		{
			m_HYVmtx.GetVrtMetric(i,vrtMetoic);

			CHYVertOriginYMetrics  OrgnYMtrics;
			CHYGlyph& glyph = m_vtHYGlyphs[i];

			short sYMetrics = glyph.maxY+vrtMetoic.tsb;
			if (sYMetrics != m_HYVORG.defaultVertOriginY)
			{
				OrgnYMtrics.glyphIndex = i;
				OrgnYMtrics.vertOriginY = sYMetrics;

				m_HYVORG.vtVertOriginYMetrics.push_back(OrgnYMtrics);
				m_HYVORG.numVertOriginYMetrics++;
			}
		}		

	}	// end of void CHYFontCodec::CountVORG()


	int CHYFontCodec::DecodeDSIG()
	{
		m_HYDSIG.SetDefault();

		int iEntryIndex = m_HYTbDirectory.FindTableEntry(DSIG_TAG);
		if (iEntryIndex == -1) return FONT_ERR_DSIG_DECODE;

		CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
		fseek(m_pFontFile,tbEntry.offset,SEEK_SET);

		unsigned short usTmp=0;
		unsigned long ulTmp=0;

		fread(&ulTmp,4,1,m_pFontFile);
		m_HYDSIG.ulVersion = hy_cdr_int32_to(ulTmp);

		fread(&usTmp,2,1,m_pFontFile);
		m_HYDSIG.usNumSigs = hy_cdr_int16_to(usTmp);

		fread(&usTmp,2,1,m_pFontFile);
		m_HYDSIG.usFlag = hy_cdr_int16_to(usTmp);		

		return HY_NOERROR;

	}	// end of int CHYFontCodec::DecodeDSIG()

	int	CHYFontCodec::EncodeDSIG()
	{
		if (m_pFontFile)
		{
			unsigned short usTmp=0;
			unsigned long ulTmp=0;

			int iEntryIndex = m_HYTbDirectory.FindTableEntry(DSIG_TAG);
			if (iEntryIndex == -1) return FONT_ERR_DSIG_ENCODE;

			CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
			tbEntry.offset = ftell(m_pFontFile);

			ulTmp = hy_cdr_int32_to(m_HYDSIG.ulVersion);
			fwrite(&ulTmp,4,1,m_pFontFile);

			usTmp = hy_cdr_int16_to(m_HYDSIG.usNumSigs);
			fwrite(&usTmp,2,1,m_pFontFile);

			usTmp = hy_cdr_int16_to(m_HYDSIG.usFlag);
			fwrite(&usTmp,2,1,m_pFontFile);

			tbEntry.length = ftell(m_pFontFile) - tbEntry.offset;
			int iTail = 4-tbEntry.length%4;
			if (tbEntry.length%4>0)	{
				char c = 0;
				for (int i=0; i<iTail; i++)	{
					fwrite(&c,1,1,m_pFontFile);
				}				
			}
		}

		return HY_NOERROR;

	}	// end of int	CHYFontCodec::EncodeDSIG()

	int CHYFontCodec::DecodeCOLR()
	{
		m_HYCOLR.SetDefault();

		int iEntryIndex = m_HYTbDirectory.FindTableEntry(COLR_TAG);
		if (iEntryIndex == -1) return FONT_ERR_COLR_DECODE;

		CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
		fseek(m_pFontFile,tbEntry.offset,SEEK_SET);

		unsigned short usTmp=0;
		unsigned long ulTmp=0;

		fread(&usTmp,2,1,m_pFontFile);
		m_HYCOLR.version = hy_cdr_int16_to(usTmp);
		fread(&usTmp,2,1,m_pFontFile);
		m_HYCOLR.numBaseGlyphRecords = hy_cdr_int16_to(usTmp);
		fread(&ulTmp,4,1,m_pFontFile);
		m_HYCOLR.baseGlyphRecordsOffset = hy_cdr_int32_to(ulTmp);
		fread(&ulTmp,4,1,m_pFontFile);
		m_HYCOLR.layerRecordsOffset = hy_cdr_int32_to(ulTmp);
		fread(&usTmp,2,1,m_pFontFile);
		m_HYCOLR.numLayerRecords= hy_cdr_int16_to(usTmp);		

		fseek(m_pFontFile,tbEntry.offset+m_HYCOLR.baseGlyphRecordsOffset,SEEK_SET);
		for (long i=0; i<m_HYCOLR.numBaseGlyphRecords; i++)
		{
			CBaseGlyphRecord BaseGlyphRecord;

			fread(&usTmp,2,1,m_pFontFile);
			BaseGlyphRecord.GID= hy_cdr_int16_to(usTmp);
			fread(&usTmp,2,1,m_pFontFile);
			BaseGlyphRecord.firstLayerIndex= hy_cdr_int16_to(usTmp);
			fread(&usTmp,2,1,m_pFontFile);
			BaseGlyphRecord.numLayers = hy_cdr_int16_to(usTmp);

			m_HYCOLR.vtBaseGlyphRecord.push_back(BaseGlyphRecord);
		}

		fseek(m_pFontFile,tbEntry.offset+m_HYCOLR.layerRecordsOffset,SEEK_SET);
		for (long i=0; i<m_HYCOLR.numLayerRecords; i++)
		{
			CLayerRecord LayerRecord;

			fread(&usTmp,2,1,m_pFontFile);
			LayerRecord.GID= hy_cdr_int16_to(usTmp);
			fread(&usTmp,2,1,m_pFontFile);
			LayerRecord.paletteIndex= hy_cdr_int16_to(usTmp);			

			m_HYCOLR.vtLayerRecord.push_back(LayerRecord);
		}

		return HY_NOERROR;

	}	// end of int CHYFontCodec::DecodeCOLR()

	int CHYFontCodec::EncodeCOLR()
	{
		if (m_pFontFile){

			unsigned short usTmp=0;
			unsigned long ulTmp=0;

			int iEntryIndex = m_HYTbDirectory.FindTableEntry(COLR_TAG);
			if (iEntryIndex == -1) return FONT_ERR_COLR_DECODE;

			CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
			tbEntry.offset = ftell(m_pFontFile);
			
			usTmp = hy_cdr_int16_to(m_HYCOLR.version);
			fwrite(&usTmp,2,1,m_pFontFile);
			usTmp = hy_cdr_int16_to(m_HYCOLR.numBaseGlyphRecords);
			fwrite(&usTmp,2,1,m_pFontFile);
			long bGROffset = ftell(m_pFontFile);
			ulTmp = hy_cdr_int32_to(m_HYCOLR.baseGlyphRecordsOffset);
			fwrite(&ulTmp,4,1,m_pFontFile);
			long lROffset = ftell(m_pFontFile);
			ulTmp = hy_cdr_int32_to(m_HYCOLR.layerRecordsOffset);
			fwrite(&ulTmp,4,1,m_pFontFile);
			usTmp = hy_cdr_int16_to(m_HYCOLR.numLayerRecords);
			fwrite(&usTmp,2,1,m_pFontFile);
			 
			long BaseGlyphlth = 0;
			for (int i=0; i<m_HYCOLR.numBaseGlyphRecords; i++){
				const CBaseGlyphRecord& BaseGlyphRecord = m_HYCOLR.vtBaseGlyphRecord[i];
				usTmp = hy_cdr_int16_to(BaseGlyphRecord.GID);
				fwrite(&usTmp,2,1,m_pFontFile);
				usTmp = hy_cdr_int16_to(BaseGlyphRecord.firstLayerIndex);
				fwrite(&usTmp,2,1,m_pFontFile);
				usTmp = hy_cdr_int16_to(BaseGlyphRecord.numLayers );
				fwrite(&usTmp,2,1,m_pFontFile);

				BaseGlyphlth+=6;
			}

			for (int i=0; i<m_HYCOLR.numLayerRecords; i++){
				const CLayerRecord& LayerRecord = m_HYCOLR.vtLayerRecord[i];

				usTmp = hy_cdr_int16_to(LayerRecord.GID);
				fwrite(&usTmp,2,1,m_pFontFile);
				usTmp = hy_cdr_int16_to(LayerRecord.paletteIndex);
				fwrite(&usTmp,2,1,m_pFontFile);				
			}

			int iTMP = ftell(m_pFontFile);

			fseek(m_pFontFile, bGROffset,SEEK_SET);
			m_HYCOLR.baseGlyphRecordsOffset = 16;
			ulTmp = hy_cdr_int32_to(m_HYCOLR.baseGlyphRecordsOffset);
			fwrite(&ulTmp,4,1,m_pFontFile);
			m_HYCOLR.layerRecordsOffset = 16 + BaseGlyphlth;
			ulTmp = hy_cdr_int32_to(m_HYCOLR.layerRecordsOffset);
			fwrite(&ulTmp,4,1,m_pFontFile);

			fseek(m_pFontFile, iTMP,SEEK_SET);

			tbEntry.length = ftell(m_pFontFile) - tbEntry.offset;
			int iTail = 4-tbEntry.length%4;
			if (tbEntry.length%4>0)	{
				char c = 0;
				for (int i=0; i<iTail; i++)	{
					fwrite(&c,1,1,m_pFontFile);
				}				
			}
		}

		return HY_NOERROR;

	}	// end of int CHYFontCodec::EncodeCOLR()

	int	CHYFontCodec::DecodeSVG()
	{
		m_HYSVG.SetDefault();

		int iEntryIndex = m_HYTbDirectory.FindTableEntry(SVG_TAG);
		if (iEntryIndex == -1) return FONT_ERR_SVG_DECODE;

		CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
		fseek(m_pFontFile, tbEntry.offset, SEEK_SET);

		unsigned short usTmp = 0;
		unsigned long ulTmp = 0;

		fread(&usTmp, 2, 1, m_pFontFile);
		m_HYSVG.Header.version = hy_cdr_int16_to(usTmp);
		fread(&ulTmp, 4, 1, m_pFontFile);
		m_HYSVG.Header.svgDocumentListOffset = hy_cdr_int32_to(ulTmp);
		fread(&ulTmp, 4, 1, m_pFontFile);
		m_HYSVG.Header.reserved = hy_cdr_int32_to(ulTmp);

		unsigned long  ulDocLstPos = ftell(m_pFontFile);
		fread(&usTmp, 2, 1, m_pFontFile);
		m_HYSVG.DocumentList.numEntries = hy_cdr_int16_to(usTmp);
		unsigned long  ulCurPos = 0;
		for (int i = 0; i < m_HYSVG.DocumentList.numEntries; i++)
		{
			SVGDocumentRecord Record;
			fread(&usTmp, 2, 1, m_pFontFile);
			Record.startGlyphID = hy_cdr_int16_to(usTmp);
			fread(&usTmp, 2, 1, m_pFontFile);
			Record.endGlyphID = hy_cdr_int16_to(usTmp);
			fread(&ulTmp, 4, 1, m_pFontFile);
			Record.svgDocOffset = hy_cdr_int32_to(ulTmp);
			fread(&ulTmp, 4, 1, m_pFontFile);
			Record.svgDocLength = hy_cdr_int32_to(ulTmp);

			ulCurPos = ftell(m_pFontFile);
			fseek(m_pFontFile, ulDocLstPos + Record.svgDocOffset, SEEK_SET);
			for (int j = 0; j < Record.svgDocLength; j++){
				unsigned char  uChar = 0;
				fread(&uChar, 1, 1, m_pFontFile);
				Record.vtDocData.push_back(uChar);
			}
			fseek(m_pFontFile, ulCurPos, SEEK_SET);

			m_HYSVG.DocumentList.vtDocumentRecord.push_back(Record);
		}

		return HY_NOERROR;

	}	// end of int CHYFontCodec::DecodeSVG()

	void CHYFontCodec::DumpSVG()
	{

	}	// end of void CHYFontCodec::DumpSVG()

	int	CHYFontCodec::EncodeSVG()
	{
		int iEntryIndex = m_HYTbDirectory.FindTableEntry(SVG_TAG);
		if (iEntryIndex == -1)  return FONT_ERR_SVG_ENCODE;

		CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
		tbEntry.offset = ftell(m_pFontFile);

		unsigned short usTmp = 0;
		unsigned long ulTmp = 0;
		
		usTmp = hy_cdr_int16_to(m_HYSVG.Header.version);
		fwrite(&usTmp, 2, 1, m_pFontFile);
		m_HYSVG.Header.svgDocumentListOffset = 10;//version+svgDocumentListOffset+reserved
		ulTmp = hy_cdr_int32_to(m_HYSVG.Header.svgDocumentListOffset);
		fwrite(&ulTmp, 4, 1, m_pFontFile);
		ulTmp = hy_cdr_int32_to(m_HYSVG.Header.reserved);
		fwrite(&ulTmp, 4, 1, m_pFontFile);

		long lDocLstLength = m_HYSVG.DocumentList.numEntries * 12;
		usTmp = hy_cdr_int16_to(m_HYSVG.DocumentList.numEntries);
		fwrite(&usTmp, 2, 1, m_pFontFile);
		unsigned long ulOffset = lDocLstLength;
		for (int i = 0; i < m_HYSVG.DocumentList.numEntries; i++)
		{
			SVGDocumentRecord& Record = m_HYSVG.DocumentList.vtDocumentRecord[i];

			usTmp = hy_cdr_int16_to(Record.startGlyphID);
			fwrite(&usTmp, 2, 1, m_pFontFile);
			usTmp = hy_cdr_int16_to(Record.endGlyphID);
			fwrite(&usTmp, 2, 1, m_pFontFile);

			Record.svgDocOffset = lDocLstLength;
			ulTmp = hy_cdr_int32_to(Record.svgDocOffset);
			fwrite(&ulTmp, 4, 1, m_pFontFile);
			ulTmp = hy_cdr_int32_to(Record.svgDocLength);
			fwrite(&ulTmp, 4, 1, m_pFontFile);

			lDocLstLength += Record.svgDocLength;
		}

		for (int i = 0; i < m_HYSVG.DocumentList.numEntries; i++)
		{
			SVGDocumentRecord& Record = m_HYSVG.DocumentList.vtDocumentRecord[i];
			for (int j = 0; j < Record.svgDocLength; j++){
				fwrite(&Record.vtDocData[j], 1, 1, m_pFontFile);
			}
		}

		tbEntry.length = ftell(m_pFontFile) - tbEntry.offset;
		::HY_4ByteAlign(m_pFontFile, tbEntry.length);

		return HY_NOERROR;

	}	// end of int	CHYFontCodec::EncodeSVG()

	int CHYFontCodec::Decodeavar()
	{
		m_HYavar.SetDefault();

		int iEntryIndex = m_HYTbDirectory.FindTableEntry(AVAR_TAG);
		if (iEntryIndex == -1) return FONT_ERR_AVAR_DECODE;

		CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
		fseek(m_pFontFile, tbEntry.offset, SEEK_SET);

		unsigned short usTmp = 0;
		unsigned long ulTmp = 0;

		fread(&usTmp, 2, 1, m_pFontFile);
		m_HYavar.majorVersion = hy_cdr_int16_to(usTmp);

		fread(&usTmp, 2, 1, m_pFontFile);
		m_HYavar.minorVersion = hy_cdr_int16_to(usTmp);

		fread(&usTmp, 2, 1, m_pFontFile);
		m_HYavar.reserved = hy_cdr_int16_to(usTmp);

		//The number of variation axes for this font. 
		//This must be the same number as axisCount in the 'fvar' table.
		fread(&usTmp, 2, 1, m_pFontFile);
		m_HYavar.axisCount = hy_cdr_int16_to(usTmp);

		CHYSegmentMaps SegmentMap;
		for (int i = 0; i < m_HYavar.axisCount; i++){
			fread(&usTmp, 2, 1, m_pFontFile);
			SegmentMap.positionMapCount = hy_cdr_int16_to(usTmp);

			for (int j = 0; j < SegmentMap.positionMapCount; j++)	{
				CHYAxisValueMap	AxMap;
				fread(&usTmp, 2, 1, m_pFontFile);
				AxMap.fromCoordinate = hy_cdr_int16_to(usTmp);
				fread(&usTmp, 2, 1, m_pFontFile);
				AxMap.toCoordinate = hy_cdr_int16_to(usTmp);

				SegmentMap.vtAxisValueMaps.push_back(AxMap);
			}		

			m_HYavar.vtSegmenMaps.push_back(SegmentMap);
		}

		return HY_NOERROR;

	}	// end of int CHYFontCodec::Decodeavar()

	void CHYFontCodec::Dumpavar()
	{


	}	// end of void CHYFontCodec::Dumpavar()

	int CHYFontCodec::Encodeavar()
	{
		int iEntryIndex = m_HYTbDirectory.FindTableEntry(AVAR_TAG);
		if (iEntryIndex == -1)  return FONT_ERR_AVAR_ENCODE;

		CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
		tbEntry.offset = ftell(m_pFontFile);

		unsigned short usTmp = 0;
		unsigned long ulTmp = 0;

		//Major version number of the font variations table 
		usTmp = hy_cdr_int16_to(m_HYavar.majorVersion);
		fwrite(&usTmp, 2, 1, m_pFontFile);
		//Major version number of the font variations table 
		usTmp = hy_cdr_int16_to(m_HYavar.minorVersion);
		fwrite(&usTmp, 2, 1, m_pFontFile);
		//Permanently reserved; set to zero.
		usTmp = hy_cdr_int16_to(m_HYavar.reserved);
		fwrite(&usTmp, 2, 1, m_pFontFile);
		//The number of variation axes for this font.
		//This must be the same number as axisCount in the 'fvar' table.
		usTmp = hy_cdr_int16_to(m_HYavar.axisCount);
		fwrite(&usTmp, 2, 1, m_pFontFile);

		//The segment maps array — one segment map for each axis, 
		//in the order of axes specified in the 'fvar' table.
		for (int i = 0; i < m_HYavar.axisCount; i++){
			CHYSegmentMaps& SegmentMap = m_HYavar.vtSegmenMaps[i];
			usTmp = hy_cdr_int16_to(SegmentMap.positionMapCount);
			fwrite(&usTmp, 2, 1, m_pFontFile);

			for (size_t j = 0; j < SegmentMap.vtAxisValueMaps.size(); j++){
				CHYAxisValueMap& ValueMap = SegmentMap.vtAxisValueMaps[j];
				usTmp = hy_cdr_int16_to(ValueMap.fromCoordinate);
				fwrite(&usTmp, 2, 1, m_pFontFile);
				usTmp = hy_cdr_int16_to(ValueMap.toCoordinate);
				fwrite(&usTmp, 2, 1, m_pFontFile);
			}
		}

		tbEntry.length = ftell(m_pFontFile) - tbEntry.offset;
		::HY_4ByteAlign(m_pFontFile, tbEntry.length);

		return HY_NOERROR;

	} // end of int CHYFontCodec::Encodeavar()

	int CHYFontCodec::Decodefvar()
	{
		m_HYfvar.setDefault();

		int iEntryIndex = m_HYTbDirectory.FindTableEntry(FVAR_TAG);
		if (iEntryIndex == -1) return FONT_ERR_FVAR_DECODE;

		CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
		fseek(m_pFontFile,tbEntry.offset,SEEK_SET);

		unsigned short usTmp=0;
		unsigned long ulTmp=0;

		fread(&usTmp,2,1,m_pFontFile);
		m_HYfvar.majorVersion = hy_cdr_int16_to(usTmp);
		fread(&usTmp,2,1,m_pFontFile);
		m_HYfvar.minorVersion = hy_cdr_int16_to(usTmp);
		fread(&usTmp,2,1,m_pFontFile);
		m_HYfvar.offsetToAxesArray = hy_cdr_int16_to(usTmp);
		fread(&usTmp,2,1,m_pFontFile);
		m_HYfvar.reserved = hy_cdr_int16_to(usTmp);
		fread(&usTmp,2,1,m_pFontFile);
		m_HYfvar.axisCount = hy_cdr_int16_to(usTmp);
		fread(&usTmp,2,1,m_pFontFile);
		m_HYfvar.axisSize = hy_cdr_int16_to(usTmp);
		fread(&usTmp,2,1,m_pFontFile);
		m_HYfvar.instanceCount = hy_cdr_int16_to(usTmp);
		fread(&usTmp,2,1,m_pFontFile);
		m_HYfvar.instanceSize = hy_cdr_int16_to(usTmp);
		
		for (unsigned short i=0; i<m_HYfvar.axisCount; i++ )
		{
			CHYFvarAxisRecord  vAxisRecord;

			fread(&ulTmp,4,1,m_pFontFile);
			vAxisRecord.Tag= hy_cdr_int32_to(ulTmp);

			fread(&usTmp,2,1,m_pFontFile);
			vAxisRecord.minValue.value = hy_cdr_int16_to(usTmp);
			fread(&usTmp,2,1,m_pFontFile);
			vAxisRecord.minValue.fract = hy_cdr_int16_to(usTmp);

			fread(&usTmp,2,1,m_pFontFile);
			vAxisRecord.defaultValue.value = hy_cdr_int16_to(usTmp);
			fread(&usTmp,2,1,m_pFontFile);
			vAxisRecord.defaultValue.fract =  hy_cdr_int16_to(usTmp);

			fread(&usTmp,2,1,m_pFontFile);
			vAxisRecord.maxValue.value =  hy_cdr_int16_to(usTmp);
			fread(&usTmp,2,1,m_pFontFile);
			vAxisRecord.maxValue.fract=  hy_cdr_int16_to(usTmp);

			fread(&usTmp,2,1,m_pFontFile);
			vAxisRecord.flags=  hy_cdr_int16_to(usTmp);
			fread(&usTmp,2,1,m_pFontFile);
			vAxisRecord.axisNameID = hy_cdr_int16_to(usTmp);

			m_HYfvar.vtaxes.push_back(vAxisRecord);
		}

		for (unsigned short i=0; i<m_HYfvar.instanceCount; i++)
		{
			CHYFvarInstncRecord	InstanceRecord;

			fread(&usTmp,2,1,m_pFontFile);
			InstanceRecord.subfamilyNameID = hy_cdr_int16_to(usTmp);

			fread(&usTmp,2,1,m_pFontFile);
			InstanceRecord.flags = hy_cdr_int16_to(usTmp);

			for (unsigned short j=0; j<m_HYfvar.axisCount; j++)
			{
				HYFIXED  fixed;

				fread(&usTmp,2,1,m_pFontFile);
				fixed.value = hy_cdr_int16_to(usTmp);
				fread(&usTmp,2,1,m_pFontFile);
				fixed.fract= hy_cdr_int16_to(usTmp);
				InstanceRecord.coordinates.push_back(fixed);
			}

			if (m_HYfvar.instanceSize== m_HYfvar.axisCount * sizeof(HYFIXED) + 6)
			{
				fread(&usTmp,2,1,m_pFontFile);
				InstanceRecord.postScriptNameID = hy_cdr_int16_to(usTmp);
			}		

			m_HYfvar.vtinstances.push_back(InstanceRecord);
		}

		return HY_NOERROR;

	}	// end of int CHYFontCodec::Decodefvar()

	void CHYFontCodec::Dumpfvar(CMarkup& MK)
	{
		char strTmp[MAX_PATH] = { 0 };

		MK.AddElem("fvar");
		MK.IntoElem();

		std::vector<string> vtAxisTag;
		for (int i = 0; i < m_HYfvar.axisCount; i++)
		{
			MK.AddElem("Axis");
			MK.IntoElem();

			memset(strTmp, 0, MAX_PATH);
			sprintf_s(strTmp, "%x", m_HYfvar.vtaxes[i].Tag);			
			MK.AddElem("AxisTag", HY_Hex_to_string(strTmp));
			vtAxisTag.push_back(HY_Hex_to_string(strTmp));

			MK.AddElem("minValue", HY_HYFIXED_to_float(m_HYfvar.vtaxes[i].minValue));
			MK.AddElem("defaultValue", HY_HYFIXED_to_float(m_HYfvar.vtaxes[i].defaultValue));
			MK.AddElem("maxValue", HY_HYFIXED_to_float(m_HYfvar.vtaxes[i].maxValue));

			MK.OutOfElem();
		}

		for (int i = 0; i < m_HYfvar.instanceCount; i++)
		{
			memset(strTmp, 0, MAX_PATH);
			sprintf_s(strTmp, "%d", m_HYfvar.vtinstances[i].subfamilyNameID);

			MK.AddElem("NamedInstance");
			MK.AddAttrib("subfamilyNameID", strTmp);

			if (m_HYfvar.vtinstances[i].postScriptNameID != 0xFFFF)
			{
				memset(strTmp, 0, MAX_PATH);
				sprintf_s(strTmp, "%d", m_HYfvar.vtinstances[i].postScriptNameID);
				MK.AddAttrib("postScriptNameID", strTmp);
			}

			MK.IntoElem();

			for (size_t j = 0; j < m_HYfvar.vtinstances[i].coordinates.size(); j++)
			{
				MK.AddElem("coord");
				MK.AddAttrib("axis", vtAxisTag[j]);

				
				memset(strTmp, 0, MAX_PATH);
				sprintf_s(strTmp, "%03f", HY_HYFIXED_to_float(m_HYfvar.vtinstances[i].coordinates[j]));
				MK.AddAttrib("value",strTmp);
			}
			MK.OutOfElem();
		}
		MK.OutOfElem();

	}	// end of void CHYFontCodec::Dumpfvar(CMarkup& MK)

	int	CHYFontCodec::Encodefvar()
	{
		unsigned long ulTableBegin = ftell(m_pFontFile);
		unsigned long ulGlyphOffset = 0;

		int iEntryIndex = m_HYTbDirectory.FindTableEntry(FVAR_TAG);
		if (iEntryIndex == -1)  return FONT_ERR_FVAR_ENCODE;

		CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
		tbEntry.offset = ftell(m_pFontFile);
		 
		unsigned short usTmp = 0;
		unsigned long ulTmp = 0;
		
		//Major version number of the font variations table 
		usTmp = hy_cdr_int16_to(m_HYfvar.majorVersion);
		fwrite(&usTmp, 2, 1, m_pFontFile);
		//Major version number of the font variations table 
		usTmp = hy_cdr_int16_to(m_HYfvar.minorVersion);
		fwrite(&usTmp, 2, 1, m_pFontFile);

		//Offset in bytes from the beginning of the table to the start of the VariationAxisRecord array.
		m_HYfvar.offsetToAxesArray = ftell(m_pFontFile)- tbEntry.offset;
		usTmp = hy_cdr_int16_to(m_HYfvar.offsetToAxesArray);
		fwrite(&usTmp, 2, 1, m_pFontFile);

		//This field is permanently reserved. Set to 2
		m_HYfvar.reserved = 2;
		usTmp = hy_cdr_int16_to(m_HYfvar.reserved);
		fwrite(&usTmp, 2, 1, m_pFontFile);

		//The number of variation axes in the font (the number of records in the axes array).
		usTmp = hy_cdr_int16_to(m_HYfvar.axisCount);
		fwrite(&usTmp, 2, 1, m_pFontFile);

		//he size in bytes of each VariationAxisRecord — set to 20 (0x0014) for this version
		usTmp = hy_cdr_int16_to(m_HYfvar.axisSize);
		fwrite(&usTmp, 2, 1, m_pFontFile);

		//The number of named instances defined in the font (the number of records in the instances array).
		usTmp = hy_cdr_int16_to(m_HYfvar.instanceCount);
		fwrite(&usTmp, 2, 1, m_pFontFile);

		//The size in bytes of each InstanceRecord — set to either axisCount * sizeof(Fixed) + 4, 
		//or to axisCount * sizeof(Fixed) + 6.
		//目前不考虑postScriptNameID项
		m_HYfvar.instanceSize = m_HYfvar.axisCount * 4 + 4;
		usTmp = hy_cdr_int16_to(m_HYfvar.instanceSize);
		fwrite(&usTmp, 2, 1, m_pFontFile);

		// refill offsetToAxesArray			
		unsigned short usOffst = ftell(m_pFontFile) - tbEntry.offset;
		fseek(m_pFontFile, tbEntry.offset + m_HYfvar.offsetToAxesArray, SEEK_SET);

		usTmp = hy_cdr_int16_to(usOffst);
		fwrite(&usTmp, 2, 1, m_pFontFile);

		// VariationAxisRecord
		fseek(m_pFontFile, tbEntry.offset + usOffst, SEEK_SET);
		for (int i = 0; i < m_HYfvar.axisCount; i++){
			CHYFvarAxisRecord&  AxisRcrd = m_HYfvar.vtaxes[i];

			//Tag identifying the design variation for the axis.
			ulTmp = hy_cdr_int32_to(AxisRcrd.Tag);
			fwrite(&ulTmp, 4, 1, m_pFontFile);
			//Fixed	minValue The minimum coordinate value for the axis.
			usTmp = hy_cdr_int16_to(AxisRcrd.minValue.value);
			fwrite(&usTmp, 2, 1, m_pFontFile);
			usTmp = hy_cdr_int16_to(AxisRcrd.minValue.fract);
			fwrite(&usTmp, 2, 1, m_pFontFile);
			//The default coordinate value for the axis.
			usTmp = hy_cdr_int16_to(AxisRcrd.defaultValue.value);
			fwrite(&usTmp, 2, 1, m_pFontFile);
			usTmp = hy_cdr_int16_to(AxisRcrd.defaultValue.fract);
			fwrite(&usTmp, 2, 1, m_pFontFile);
			//The maximum coordinate value for the axis.
			usTmp = hy_cdr_int16_to(AxisRcrd.maxValue.value);
			fwrite(&usTmp, 2, 1, m_pFontFile);
			usTmp = hy_cdr_int16_to(AxisRcrd.maxValue.fract);
			fwrite(&usTmp, 2, 1, m_pFontFile);
			//Axis qualifiers
			usTmp = hy_cdr_int16_to(AxisRcrd.flags);
			fwrite(&usTmp, 2, 1, m_pFontFile);
			//The name ID for entries in the 'name' table that provide a display name for this axis.
			usTmp = hy_cdr_int16_to(AxisRcrd.axisNameID);
			fwrite(&usTmp, 2, 1, m_pFontFile);
		}

		for (int i = 0; i < m_HYfvar.instanceCount; i++){
			CHYFvarInstncRecord&  InstncRcrd = m_HYfvar.vtinstances[i];
			//The name ID for entries in the 'name' table that provide subfamily names for this instance.
			usTmp = hy_cdr_int16_to(InstncRcrd.subfamilyNameID);
			fwrite(&usTmp, 2, 1, m_pFontFile);
			//Reserved for future use
			usTmp = hy_cdr_int16_to(InstncRcrd.flags);
			fwrite(&usTmp, 2, 1, m_pFontFile);

			//The coordinates array for this instance.			
			for (int j = 0; j < m_HYfvar.axisCount; j++){
				HYFIXED& fxd = InstncRcrd.coordinates[j];
				usTmp = hy_cdr_int16_to(fxd.value);
				fwrite(&usTmp, 2, 1, m_pFontFile);				
				usTmp = hy_cdr_int16_to(fxd.fract);
				fwrite(&usTmp, 2, 1, m_pFontFile);
			}
		}

		tbEntry.length = ftell(m_pFontFile) - tbEntry.offset;
		::HY_4ByteAlign(m_pFontFile, tbEntry.length);

		return HY_NOERROR;

	}	// end of int	CHYFontCodec::Endocdefvar()

	void CHYFontCodec::Dumpgvar(CMarkup& MK)
	{
		char strTmp[MAX_PATH] = { 0 };

		MK.AddElem("gvar");
		MK.IntoElem();

		MK.AddElem("version");
		sprintf_s(strTmp, "%d", m_HYgvar.majorVersion);
		MK.AddAttrib("value", strTmp);

		MK.AddElem("reserved");
		memset(strTmp, 0, MAX_PATH);
		sprintf_s(strTmp, "%d", m_HYgvar.minorVersion);
		MK.AddAttrib("value", strTmp);

		/*
		MK.AddElem("sharedTuple");
		MK.IntoElem();
		for (int i=0; i<m_HYgvar.sharedTupleCount;i++)
		{
		CHYTuple& tuple = m_HYgvar.sharedTuples[i];
		MK.AddElem("Tuple");
		MK.IntoElem();
		for (size_t j = 0; j < tuple.coordinates.size(); j++)
		{
		float  fValue = HY_F2DOT14_to_float(tuple.coordinates[j]);

		memset(strTmp, 0, MAX_PATH);
		sprintf_s(strTmp, "%f", fValue);
		MK.AddElem("AxisIndex", strTmp);
		memset(strTmp, 0, MAX_PATH);
		sprintf_s(strTmp, "%d",j);
		MK.AddAttrib("Index", strTmp);
		}
		MK.OutOfElem();
		}
		MK.OutOfElem();
		*/

		for (int i = 0; i < m_HYgvar.glyphCount; i++)
		{
			glyphVariationData& gVartnData = m_HYgvar.vtGlyphVariationData[i];
			CHYGlyph& glyph = m_vtHYGlyphs[i];

			MK.AddElem("glyphVariations");
			MK.AddAttrib("glyph", glyph.strPostName);
			MK.IntoElem();

			for (int j = 0; j < gVartnData.tupleVariationCount; j++)
			{
				TupleVariationHeader& tplHeader = gVartnData.vttupleVariationHeaders[j];
				MK.AddElem("tuple");
				MK.IntoElem();

				CHYTuple& shrTuple = m_HYgvar.sharedTuples[tplHeader.iSharedTupleInx];
				for (int x = 0; x < m_HYfvar.axisCount; x++)
				{
					float flt = HY_F2DOT14_to_float(shrTuple.coordinates[x]);

					MK.AddElem("coord");

					memset(strTmp, 0, MAX_PATH);
					sprintf_s(strTmp, "%x", m_HYfvar.vtaxes[x].Tag);
					MK.AddAttrib("axis", HY_Hex_to_string(strTmp));

					memset(strTmp, 0, MAX_PATH);
					sprintf_s(strTmp, "%.1f", flt);
					MK.AddAttrib("value", strTmp);
				}

				for (int x = 0; x < tplHeader.prvtpointNumbers.usPointNumer; x++)
				{
					MK.AddElem("delta");

					memset(strTmp, 0, MAX_PATH);
					sprintf_s(strTmp, "%d", x);
					MK.AddAttrib("pt", strTmp);

					memset(strTmp, 0, MAX_PATH);
					sprintf_s(strTmp, "%d", tplHeader.vtVariationPoint_X[x]);
					MK.AddAttrib("x", strTmp);

					memset(strTmp, 0, MAX_PATH);
					sprintf_s(strTmp, "%d", tplHeader.vtVariationPoint_Y[x]);
					MK.AddAttrib("y", strTmp);
				}
				MK.OutOfElem();
			}
			MK.OutOfElem();
		}
		MK.OutOfElem();

	}	// end of void CHYFontCodec::Dumpgvar()
	 
	int	CHYFontCodec::Decodegvar() 
	{		
		m_HYgvar.setDefault();

		int iEntryIndex = m_HYTbDirectory.FindTableEntry(GVAR_TAG);
		if (iEntryIndex == -1) return FONT_ERR_GVAR_DECODE;

		CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
		fseek(m_pFontFile,tbEntry.offset,SEEK_SET);

		unsigned short usTmp=0;
		unsigned long ulTmp=0;
		fread(&usTmp,2,1,m_pFontFile);
		m_HYgvar.majorVersion = hy_cdr_int16_to(usTmp);
		fread(&usTmp,2,1,m_pFontFile);
		m_HYgvar.minorVersion = hy_cdr_int16_to(usTmp);
		fread(&usTmp,2,1,m_pFontFile);
		m_HYgvar.axisCount= hy_cdr_int16_to(usTmp);
		fread(&usTmp,2,1,m_pFontFile);
		m_HYgvar.sharedTupleCount= hy_cdr_int16_to(usTmp);
		fread(&ulTmp,4,1,m_pFontFile);
		m_HYgvar.offsetToSharedTuples= hy_cdr_int32_to(ulTmp);
		fread(&usTmp,2,1,m_pFontFile);
		m_HYgvar.glyphCount= hy_cdr_int16_to(usTmp);
		fread(&usTmp,2,1,m_pFontFile);
		m_HYgvar.flags= hy_cdr_int16_to(usTmp);
		fread(&ulTmp,4,1,m_pFontFile);
		m_HYgvar.offsetToGlyphVariationData= hy_cdr_int32_to(ulTmp);

		//get glyphVariationDataOffsets[glyphCount + 1]
		for (unsigned short i = 0; i<m_HYgvar.glyphCount+1; i++){
			if (m_HYgvar.flags & 0x0001){
				fread(&ulTmp, 4, 1, m_pFontFile);
				m_HYgvar.vtOffsets.push_back(hy_cdr_int32_to(ulTmp));
			}
			else{
				fread(&usTmp, 2, 1, m_pFontFile);
				usTmp = hy_cdr_int16_to(usTmp);
				m_HYgvar.vtOffsets.push_back(usTmp << 1);
			}
		}
		//get shared tuples	
		fseek(m_pFontFile, tbEntry.offset + m_HYgvar.offsetToSharedTuples, SEEK_SET);
		for (int i = 0; i<m_HYgvar.sharedTupleCount; i++){
			CHYTuple tuple;
			float test = 0.0f;
			for (int j = 0; j<m_HYfvar.axisCount; j++){
				fread(&usTmp, 2, 1, m_pFontFile);
				usTmp = hy_cdr_int16_to(usTmp);
				tuple.coordinates.push_back(usTmp);
				test = HY_F2DOT14_to_float(usTmp);				
			}
			m_HYgvar.sharedTuples.push_back(tuple);
		}

		//Offset from the start of this table to the array of glyphVariationData tables.
		unsigned long offsetGlyphVartnDataAry = tbEntry.offset+m_HYgvar.offsetToGlyphVariationData;
		int iResult = DecodeGlyphVariationData(offsetGlyphVartnDataAry);
		if (iResult != HY_NOERROR) return iResult;		

		return HY_NOERROR;

	}	// end of int	CHYFontCodec::Decodegvar()

	int CHYFontCodec::DecodeGlyphVariationData(unsigned long uloffsetGlyphVartnDataAry)
	{
		unsigned short usTmp = 0;			
		for (int i=0; i<m_HYgvar.glyphCount; i++)		
		{
			if (i == 4)
			{
				int ijinjuntest = 0;
			}
			glyphVariationData  VGlyphData;
			if (m_HYgvar.vtOffsets[i] == m_HYgvar.vtOffsets[i + 1])
			{
				m_HYgvar.vtGlyphVariationData.push_back(VGlyphData);
				continue;
			}
			//Offsets from the start of the glyphVariationData array to each glyphVariationData table. 
			unsigned long offsetGlyphVartnData  = uloffsetGlyphVartnDataAry+m_HYgvar.vtOffsets[i];
			fseek(m_pFontFile,offsetGlyphVartnData,SEEK_SET);
			
			CHYGlyph& curglyph = m_vtHYGlyphs[i];
			DecocdeGlyphVariationDataHeader(VGlyphData, curglyph);

			// offset Serialized Data
			unsigned long offsetSerialized  = offsetGlyphVartnData+VGlyphData.offsetToData;
			fseek(m_pFontFile,offsetSerialized,SEEK_SET);
			Decocdeserializeddata(VGlyphData,curglyph);

			m_HYgvar.vtGlyphVariationData.push_back(VGlyphData);
		}

		return TRUE;

	}	// end of int CHYFontCodec::DecodeGlyphVariationData()

	int CHYFontCodec::DecocdeGlyphVariationDataHeader(glyphVariationData&  VGlyphData,CHYGlyph& curglyph)
	{
		unsigned short usTmp = 0;

		//A packed field. The high 4 bits are flags, and the low 12 bits are the number of tuple variation tables for this glyph. 
		//The number of tuple variation tables can be any number between 1 and 4095.
		fread(&usTmp,2,1,m_pFontFile);
		VGlyphData.tupleVariationCount = hy_cdr_int16_to(usTmp);

		// 	Offset from the start of the GlyphVariationData table to the serialized data
		fread(&usTmp,2,1,m_pFontFile);
		VGlyphData.offsetToData = hy_cdr_int16_to(usTmp);

		//tupleVariationHeader
		int shrindx =-1;
		int VariationCount = VGlyphData.tupleVariationCount&0x0FFF;
		for (int j=0; j<VariationCount; j++)
		{
			TupleVariationHeader tuplevHeader;

			// The size in bytes of the serialized data for this tuple variation table
			fread(&usTmp,2,1,m_pFontFile);
			tuplevHeader.variationDataSize = hy_cdr_int16_to(usTmp);

			//A packed field. The high 4 bits are flags 
			//The low 12 bits are an index into a shared tuple records array.
			fread(&usTmp,2,1,m_pFontFile);
			tuplevHeader.tupleIndex = hy_cdr_int16_to(usTmp);

			if (tuplevHeader.tupleIndex&0x8000)	//embeddedPeakTuple
			{					
				for (int x=0; x<m_HYfvar.axisCount; x++)
				{
					fread(&usTmp,2,1,m_pFontFile);
					usTmp = hy_cdr_int16_to(usTmp);
					tuplevHeader.peakTuple.coordinates.push_back(usTmp);
				}
			}
			//intermediateRegion			
			if (tuplevHeader.tupleIndex&0x4000){
				for (int x=0; x<m_HYfvar.axisCount; x++){
					fread(&usTmp,2,1,m_pFontFile);
					usTmp = hy_cdr_int16_to(usTmp);
					float test = HY_F2DOT14_to_float(usTmp);
					tuplevHeader.intermediateStartTuple.coordinates.push_back(usTmp);					
				}
				for (int x=0; x<m_HYfvar.axisCount; x++){
					fread(&usTmp,2,1,m_pFontFile);
					usTmp = hy_cdr_int16_to(usTmp);
					float test = HY_F2DOT14_to_float(usTmp);
					tuplevHeader.intermediateEndTuple.coordinates.push_back(usTmp);
				}
			}
			// 	TUPLE_INDEX_MASK Mask for the low 12 bits to give the shared tuple records index.
			tuplevHeader.iSharedTupleInx = tuplevHeader.tupleIndex&0x0FFF;
			VGlyphData.vttupleVariationHeaders.push_back(tuplevHeader);
		}

		return true;

	}	// end of int CHYFontCodec::DecocdeGlyphVariationDataHeader()

	int CHYFontCodec::Decocdeserializeddata(glyphVariationData&  VGlyphData,CHYGlyph& curglyph)
	{		
		unsigned char ucTmp=0,ucTmp1=0;
		unsigned short usTmp=0;

		//get shared"Point"numbers
		if (VGlyphData.tupleVariationCount& SHARED_POINT_NUMBERS) {
			DecocdepointNumbers(m_pFontFile,VGlyphData.SharepointNumers);
		}

		int tupleCount = VGlyphData.tupleVariationCount&0x0FFF;
		for (int j=0; j<tupleCount; j++)
		{
			int Dataoffset = 0;
			TupleVariationHeader& tuplevHeader = VGlyphData.vttupleVariationHeaders[j];
			//privatePointNumbers
			//Flag indicating that the serialized data for this tuple variation table includes packed "point" number data. 
			//If set, this tuple variation table uses that number data; 
			//if clear, this tuple variation table uses shared number data found at the start of the serialized data for this glyph variation data or 'cvar' table.			
			if (tuplevHeader.tupleIndex&0x2000)	{
				unsigned long ulSize = DecocdepointNumbers(m_pFontFile, tuplevHeader.prvtpointNumbers);
				Dataoffset += ulSize;
			}

			//Packed Deltas
			std::vector<short> vttmp;
			while (Dataoffset<tuplevHeader.variationDataSize){
				fread(&ucTmp, 1, 1, m_pFontFile);
				Dataoffset++;
				if (ucTmp&DELTAS_ARE_ZERO) {
					int iTmp = (ucTmp&DELTA_RUN_COUNT_MASK)+1;
					for (int x = 0; x < iTmp; x++){
						vttmp.push_back(0);
					}
				}
				else if (ucTmp&DELTAS_ARE_WORDS) {
					int iTmp = (ucTmp&DELTA_RUN_COUNT_MASK) + 1;
					for (int x = 0; x < iTmp; x++) {
						fread(&usTmp, 2, 1, m_pFontFile);
						usTmp = hy_cdr_int16_to(usTmp);
						Dataoffset += 2;
						vttmp.push_back((short)usTmp);
					}
				}
				else {
					int iTmp = (ucTmp&DELTA_RUN_COUNT_MASK) + 1;					
					for (int i = 0; i < iTmp; i++) {
						fread(&ucTmp, 1, 1, m_pFontFile);
						Dataoffset++;
						vttmp.push_back((short)(char)ucTmp);
					}
				}
			}

			int iSize = (int)vttmp.size();
			int iSizeHF = iSize >> 1;
			for (int x = 0; x < iSize; x++)	{
				if (x < iSizeHF)
					tuplevHeader.vtVariationPoint_X.push_back(vttmp[x]);
				else 
					tuplevHeader.vtVariationPoint_Y.push_back(vttmp[x]);
			}
		}
		
		return true;

	}	// end of int CHYFontCodec::Decocdeserializeddata()

	unsigned long CHYFontCodec::DecocdepointNumbers(FILE* pFile,pointNumbers& pntNumbers)
	{
		unsigned long Dataoffset = 0;
		unsigned char cTmp = 0, cTmp1 = 0;

		//得到点数量
		fread(&cTmp, 1, 1, m_pFontFile);
		Dataoffset++;
		if (cTmp == 0) {
			pntNumbers.usPointNumer = 0;
		}
		else if (cTmp&POINTS_ARE_WORDS) {
			fread(&cTmp1, 1, 1, m_pFontFile);
			Dataoffset++;
			pntNumbers.usPointNumer = (unsigned short)((cTmp&POINT_RUN_COUNT_MASK << 8)|cTmp1);
		}
		else {
			pntNumbers.usPointNumer = (unsigned short)(cTmp&POINT_RUN_COUNT_MASK);
		}
		
		if (pntNumbers.usPointNumer > 0)
		{
			// 得到点数据
			unsigned short uselement = 0;
			unsigned short usTmp = 0;
			fread(&cTmp, 1, 1, m_pFontFile);
			Dataoffset++;
			uselement = (cTmp&POINT_RUN_COUNT_MASK) + 1;
			pntNumbers.vtPointIndex.clear();
			if (cTmp&POINTS_ARE_WORDS) {
				for (int j = 0; j < uselement; j++) {
					fread(&usTmp, 2, 1, m_pFontFile);
					Dataoffset += 2;
					usTmp = hy_cdr_int16_to(usTmp);
					pntNumbers.vtPointIndex.push_back(usTmp);
				}
			}
			else {
				for (int j = 0; j < uselement; j++) {
					fread(&cTmp, 1, 1, m_pFontFile);
					Dataoffset++;
					pntNumbers.vtPointIndex.push_back(cTmp);
				}
			}
		}
		
		return Dataoffset;

	}	// end of void CHYFontCodec::DecocdepointNumbers()
	
	int	CHYFontCodec::Encodegvar()
	{
		unsigned long ulTableBegin = ftell(m_pFontFile);
		unsigned long ulGlyphOffset = 0;

		int iEntryIndex = m_HYTbDirectory.FindTableEntry(GVAR_TAG);
		if (iEntryIndex == -1)  return FONT_ERR_GVAR_ENCODE;

		std::vector<unsigned char> vtShrdtpls;
		std::vector<unsigned char> vtGlyphVariationData;

		CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
		tbEntry.offset = ftell(m_pFontFile);

		unsigned short usTmp = 0;
		unsigned long ulTmp = 0;
		unsigned long tboffset = 0;
		//Major version number of the style attributes table — set to 1.
		usTmp = hy_cdr_int16_to(m_HYgvar.majorVersion);
		fwrite(&usTmp, 2, 1, m_pFontFile);
		tboffset += 2;

		//Minor version number of the glyph variations table 
		usTmp = hy_cdr_int16_to(m_HYgvar.minorVersion);
		fwrite(&usTmp, 2, 1, m_pFontFile);
		tboffset += 2;

		//The number of variation axes for this font. 
		//This must be the same number as axisCount in the 'fvar' table.
		usTmp = hy_cdr_int16_to(m_HYgvar.axisCount);
		fwrite(&usTmp, 2, 1, m_pFontFile);
		tboffset += 2;

		//The number of shared tuple records.
		//Shared tuple records can be referenced within glyph variation data tables for multiple glyphs, as opposed to other tuple records stored directly within a glyph variation data table.
		usTmp = hy_cdr_int16_to(m_HYgvar.sharedTupleCount);
		fwrite(&usTmp, 2, 1, m_pFontFile);
		tboffset += 2;

		EndocdeSharedtuples(vtShrdtpls);
		//Offset from the start of this table to the shared tuple records.		
		m_HYgvar.offsetToSharedTuples = ftell(m_pFontFile);
		ulTmp = 0;
		fwrite(&ulTmp, 4, 1, m_pFontFile);
		tboffset += 2;

		//The number of glyphs in this font.
		//This must match the number of glyphs stored elsewhere in the font.
		usTmp = hy_cdr_int16_to(m_HYgvar.glyphCount);
		fwrite(&usTmp, 2, 1, m_pFontFile);
		tboffset += 2;

		//cacl VariationDataOffsets
		std::vector<unsigned long> vtVariationDataOffsets;
		unsigned long offset = 0;
		for (int i = 0; i < m_HYgvar.glyphCount; i++) {				
			vtVariationDataOffsets.push_back(offset);
			glyphVariationData& glyphVrtDt = m_HYgvar.vtGlyphVariationData[i];
			EndocdeGlyphVariationData(glyphVrtDt);
			offset += (unsigned long)glyphVrtDt.vtGlyphVariationData.size();
		}

		if (m_HYgvar.glyphCount > 0)
			vtVariationDataOffsets.push_back(offset);
		////////////////////////////

		m_HYgvar.flags = 0;
		for (int i = 0; i < m_HYgvar.glyphCount + 1; i++) {
			if (vtVariationDataOffsets[i] > 131070) {
				m_HYgvar.flags = 0x0001;
				break;
			}
		}
		//Bit-field that gives the format of the offset array that follows.
		//If bit 0 is clear, the offsets are uint16; if bit 0 is set, the offsets are uint32.
		usTmp = hy_cdr_int16_to(m_HYgvar.flags);
		fwrite(&usTmp, 2, 1, m_pFontFile);
		tboffset += 2;		
		
		m_HYgvar.offsetToGlyphVariationData = ftell(m_pFontFile);
		ulTmp = 0;
		fwrite(&ulTmp, 4, 1, m_pFontFile);
		
		if (m_HYgvar.flags)	{			
			for (int i = 0; i < m_HYgvar.glyphCount + 1; i++) {
				ulTmp = vtVariationDataOffsets[i];
				ulTmp = hy_cdr_int32_to(ulTmp);
				fwrite(&ulTmp, 4, 1, m_pFontFile);
			}
		}
		else{
			
			for (int i = 0; i < m_HYgvar.glyphCount + 1; i++) {
				usTmp = vtVariationDataOffsets[i]>>1;
				usTmp = hy_cdr_int16_to(usTmp);
				fwrite(&usTmp, 2, 1, m_pFontFile);
			}
		}

		//set sharedTuplesOffset
		unsigned long ulPos = ftell(m_pFontFile);
		ulTmp = ulPos - tbEntry.offset;
		fseek(m_pFontFile, m_HYgvar.offsetToSharedTuples, SEEK_SET);
		ulTmp = hy_cdr_int32_to(ulTmp);
		fwrite(&ulTmp, 4, 1, m_pFontFile);
		fseek(m_pFontFile, ulPos, SEEK_SET);

		size_t vtsize = vtShrdtpls.size();
		for (int i = 0; i < vtsize; i++) {
			fwrite(&vtShrdtpls[i], 1, 1, m_pFontFile);
		}		

		//set glyphVariationDataArrayOffset
		ulPos = ftell(m_pFontFile);
		ulTmp = ulPos - tbEntry.offset;
		fseek(m_pFontFile, m_HYgvar.offsetToGlyphVariationData,SEEK_SET);
		ulTmp = hy_cdr_int32_to(ulTmp);
		fwrite(&ulTmp, 4, 1, m_pFontFile);
		fseek(m_pFontFile, ulPos, SEEK_SET);

		//glyphVariationData
		for (int i = 0; i < m_HYgvar.glyphCount; i++) {
			glyphVariationData& vData = m_HYgvar.vtGlyphVariationData[i];
			size_t stSize = vData.vtGlyphVariationData.size();
			for (int j = 0; j < stSize; j++){
				fwrite(&vData.vtGlyphVariationData[j], 1, 1, m_pFontFile);
			}
		}

		tbEntry.length = ftell(m_pFontFile) - tbEntry.offset;
		::HY_4ByteAlign(m_pFontFile, tbEntry.length);

		return HY_NOERROR;

	}	// end of int	CHYFontCodec::Endocdegvar()

	void CHYFontCodec::EndocdeSharedtuples(std::vector<unsigned char>& vtShrdtpls)
	{	 
		unsigned short usTmp = 0;

		for (int i = 0; i<m_HYgvar.sharedTupleCount; i++)
		{
			CHYTuple& tuple = m_HYgvar.sharedTuples[i];			
			for (int j = 0; j<m_HYgvar.axisCount; j++){				
				usTmp = (unsigned short)tuple.coordinates[j];
				vtShrdtpls.push_back(usTmp>>8);
				vtShrdtpls.push_back(usTmp&0x00FF);				
			}
		}

	}	// end of void CHYFontCodec::EndocdeSharedtuples()

	void CHYFontCodec::EndocdeGlyphVariationData(glyphVariationData& glyphVrtDt)
	{
		unsigned short usTmp = 0;
		unsigned long ulTmp = 0;

		unsigned long uloffset = 0;
		std::vector<unsigned char>&	vtGlyphVariationData = glyphVrtDt.vtGlyphVariationData;

		if (!glyphVrtDt.IsVariate())
		{
			vtGlyphVariationData.clear();
			return;
		}

		//A packed field. 
		//The high 4 bits are flags, 
		//and the low 12 bits are the number of tuple variation tables for this glyph. The number of tuple variation tables can be any number between 1 and 4095.
		usTmp = glyphVrtDt.tupleVariationCount;
		vtGlyphVariationData.push_back(usTmp >> 8);
		vtGlyphVariationData.push_back(usTmp & 0x00FF);
		uloffset += 2;

		//Offset from the start of the GlyphVariationData table to the serialized data
		//临时占位			
		usTmp = 0;
		unsigned long ulOffsetPos = uloffset;
		vtGlyphVariationData.push_back(usTmp >> 8);
		vtGlyphVariationData.push_back(usTmp & 0x00FF);
		uloffset += 2;

		//glyphVrtDt.tupleVariationCount			
		std::vector<unsigned short> vtVrtnDtSz;
		std::vector<unsigned long> vtVrtnDtSzPos;
		int tupleCount = glyphVrtDt.tupleVariationCount & 0x0FFF;
		for (int i = 0; i < tupleCount; i++) {
			TupleVariationHeader& Header = glyphVrtDt.vttupleVariationHeaders[i];
			EncodeTupleVariationHeader(Header, vtGlyphVariationData, vtVrtnDtSzPos, uloffset);
		}

		usTmp = (unsigned short)uloffset;
		// set GlyphVariationData offset
		vtGlyphVariationData[ulOffsetPos]=(signed char)(usTmp >> 8);
		vtGlyphVariationData[ulOffsetPos+1]= (signed char)(usTmp & 0x00FF);

		EndocdeSerializedData(glyphVrtDt,vtVrtnDtSz);
		
		int iSize = vtGlyphVariationData.size();
		if (iSize / 2 != 0)
			vtGlyphVariationData.push_back(0);		

		// set variationDataSize
		for (int i = 0; i < tupleCount; i++) {
			unsigned short VrtnDtSz = vtVrtnDtSz[i];
			unsigned long VrtnDtSzPos = vtVrtnDtSzPos[i];

			vtGlyphVariationData[VrtnDtSzPos]=(unsigned char)(VrtnDtSz >> 8);
			vtGlyphVariationData[VrtnDtSzPos+1]= (unsigned char)(VrtnDtSz & 0x00FF);
		}		

	}	// end of void CHYFontCodec::EndocdeGlyphVariationData()

	void CHYFontCodec::EncodeTupleVariationHeader(TupleVariationHeader& Header, std::vector<unsigned char>& vtGlyphVariationData,std::vector<unsigned long>& vtVrtnDtSzPos, unsigned long& ulOffset)
	{
		unsigned short usTmp = 0;
		//The size in bytes of the serialized data for this tuple variation table.
		//临时设置为0
		vtVrtnDtSzPos.push_back(ulOffset);
		vtGlyphVariationData.push_back(usTmp >> 8);
		vtGlyphVariationData.push_back(usTmp & 0x00FF);
		ulOffset += 2;

		//tupleIndex
		//A packed field. The high 4 bits are flags (see below).
		//The low 12 bits are an index into a shared tuple records array.				
		usTmp = Header.tupleIndex;
		vtGlyphVariationData.push_back(usTmp >> 8);
		vtGlyphVariationData.push_back(usTmp & 0x00FF);
		ulOffset += 2;

		//peakTuple
		if (usTmp & EMBEDDED_PEAK_TUPLE) {
			for (size_t j = 0; j<Header.peakTuple.coordinates.size(); j++) {
				usTmp = Header.peakTuple.coordinates[j];
				vtGlyphVariationData.push_back(usTmp >> 8);
				vtGlyphVariationData.push_back(usTmp & 0x00FF);
				ulOffset += 2;
			}
		}

		if (usTmp & INTERMEDIATE_REGION) {
			//Intermediate start tuple record for this tuple variation table — optional, determined by flags in the tupleIndex value.
			for (size_t j = 0; j<Header.intermediateStartTuple.coordinates.size(); j++) {
				usTmp = Header.intermediateStartTuple.coordinates[j];
				vtGlyphVariationData.push_back(usTmp >> 8);
				vtGlyphVariationData.push_back(usTmp & 0x00FF);
				ulOffset += 2;
			}

			//Intermediate end tuple record for this tuple variation table — optional, determined by flags in the tupleIndex value.
			for (size_t j = 0; j<Header.intermediateEndTuple.coordinates.size(); j++) {
				usTmp = Header.intermediateEndTuple.coordinates[j];
				vtGlyphVariationData.push_back(usTmp >> 8);
				vtGlyphVariationData.push_back(usTmp & 0x00FF);
				ulOffset += 2;
			}
		}	

	}	// end of void CHYFontCodec::EncodeTupleVariationHeader()
	
	void CHYFontCodec::EndocdeSerializedData(glyphVariationData& glyphVrtDt, std::vector<unsigned short>& vtVrtnDtSz)
	{	
		unsigned char ucTmp= 0;
		unsigned short usTmp=0;
		unsigned short usOffset = 0;
		
		std::vector<unsigned char>&	vtGVData = glyphVrtDt.vtGlyphVariationData;
		// “point”Number		
		EndocdepointNumber(glyphVrtDt.SharepointNumers, vtGVData);

		size_t stGlyphVrDataHead = glyphVrtDt.vttupleVariationHeaders.size();
		for (size_t i = 0; i < stGlyphVrDataHead; i++){			
			unsigned short usSize = 0;
			TupleVariationHeader& Header = glyphVrtDt.vttupleVariationHeaders[i];
			//private point numbers
			//usSize = EndocdepointNumber(Header.prvtpointNumbers, vtGVData);
			
			usSize += EndocdePackedDeltas(Header.vtVariationPoint_X, vtGVData);
			usSize += EndocdePackedDeltas(Header.vtVariationPoint_Y, vtGVData);
						
			vtVrtnDtSz.push_back((unsigned short)(usSize));
		}

	}	// void CHYFontCodec::EndocdeSerializedData()

	int CHYFontCodec::EndocdepointNumber(pointNumbers& ptNumber, std::vector<unsigned char>& vtGlyphVariationData)
	{
		unsigned char ucTmp = 0;
		unsigned short usTmp = 0;

		int iDataSize = 0;
		if (ptNumber.usPointNumer > 0) {
			if (ptNumber.usPointNumer<128) {
				vtGlyphVariationData.push_back((unsigned char)ptNumber.usPointNumer);
				iDataSize++;
			}
			else {
				ucTmp = unsigned char((ptNumber.usPointNumer >> 8) | POINTS_ARE_WORDS);
				vtGlyphVariationData.push_back(ucTmp);
				vtGlyphVariationData.push_back(ptNumber.usPointNumer & 0x00FF);				
				iDataSize += 2;
			}

			for (int i = 0; i < ptNumber.usPointNumer; i++) {
				usTmp = ptNumber.vtPointIndex[i];
				usTmp--;
				if (usTmp>127) {
					ucTmp = unsigned char((usTmp >> 8) | POINTS_ARE_WORDS);
					vtGlyphVariationData.push_back(ucTmp);
					vtGlyphVariationData.push_back((unsigned char)(usTmp & 0x00FF));					
					iDataSize += 2;
				}
				else {
					vtGlyphVariationData.push_back((unsigned char)(usTmp & 0x00FF));					
					iDataSize++;
				}
			}
		}
		else {
			vtGlyphVariationData.push_back((unsigned char)0);			
			iDataSize++;
		}
		return iDataSize;

	}	// end of void CHYFontCodec::EndocdepointNumber()

	int CHYFontCodec::EndocdePackedDeltas(std::vector<short>& vtVariationPoint, std::vector<unsigned char>& vtGVData)
	{
		int l1 = (int)vtGVData.size();
		int iPointNumber = (int)vtVariationPoint.size();
		int iIndex = 0;
		short s1 = 0, s2 = 0;		
		unsigned short usControlbytePos = 0; 
		while (iIndex < iPointNumber) 
		{
			unsigned char ucControlbyte = 0;			
			s1 = vtVariationPoint[iIndex++];
			if (s1 == 0) 
			{
				ucControlbyte = DELTAS_ARE_ZERO;
				vtGVData.push_back(ucControlbyte);

				usControlbytePos = vtGVData.size() - 1;
			}
			else if (s1 > 127 || s1 < -128)
			{
				ucControlbyte = DELTAS_ARE_WORDS;
				vtGVData.push_back(ucControlbyte);
				usControlbytePos = vtGVData.size() - 1;

				vtGVData.push_back((char)(s1 >> 8));
				vtGVData.push_back((char)s1);
			}
			else
			{
				vtGVData.push_back(ucControlbyte);
				usControlbytePos = vtGVData.size() - 1;

				vtGVData.push_back((char)s1);
			}	

			unsigned short usSegmentNumber = 0;
			while (iIndex < iPointNumber){
				if (usSegmentNumber == 64)				
					break;				

				s2 = vtVariationPoint[iIndex];
				if (s2 == 0)
				{
					if (ucControlbyte & DELTAS_ARE_ZERO)
					{
						usSegmentNumber++;
						iIndex++;
						continue;
					}
					else 
					{
						if (iIndex + 1 != iPointNumber)// than one zero
						{
							if (vtVariationPoint[iIndex + 1] != 0)
							{
								vtGVData.push_back((char)s2);
								usSegmentNumber++;
								iIndex++;
								continue;
							}
						}
						break;
					}
				}
				else if (s2 > 127 || s2 < -128)
				{
					if (ucControlbyte & DELTAS_ARE_WORDS)
					{
						vtGVData.push_back((char)(s2 >> 8));
						vtGVData.push_back((char)s2);
						usSegmentNumber++;
						iIndex++;
						continue;
					}
					break;					
				}
				else
				{
					if (ucControlbyte & DELTAS_ARE_ZERO) break;
					if (ucControlbyte & DELTAS_ARE_WORDS) break;

					vtGVData.push_back((char)s2);
					usSegmentNumber++;
					iIndex++;
					continue;
				}
			}
			
			ucControlbyte |= (char)usSegmentNumber;
			vtGVData[usControlbytePos] = ucControlbyte;
		}

		return  (int)vtGVData.size() - l1;

	}	// end of int CHYFontCodec::EndocdePackedDeltas()	

	int CHYFontCodec::DecodeSTAT()
	{
		m_HYStat.SetDefault();

		int iEntryIndex = m_HYTbDirectory.FindTableEntry(STAT_TAG);
		if (iEntryIndex == -1) return FONT_ERR_STAT_DECODE;

		CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
		fseek(m_pFontFile,tbEntry.offset,SEEK_SET);

		unsigned short usTmp=0;
		unsigned long ulTmp=0;
		fread(&usTmp,2,1,m_pFontFile);
		m_HYStat.majorVersion = hy_cdr_int16_to(usTmp);
		fread(&usTmp,2,1,m_pFontFile);
		m_HYStat.minorVersion = hy_cdr_int16_to(usTmp);
		fread(&usTmp,2,1,m_pFontFile);
		m_HYStat.designAxisSize= hy_cdr_int16_to(usTmp);
		fread(&usTmp,2,1,m_pFontFile);
		m_HYStat.designAxisCount= hy_cdr_int16_to(usTmp);
		fread(&ulTmp,4,1,m_pFontFile);
		m_HYStat.designAxesOffset= hy_cdr_int32_to(ulTmp);
		fread(&usTmp,2,1,m_pFontFile);
		m_HYStat.axisValueCount= hy_cdr_int16_to(usTmp);
		fread(&ulTmp,4,1,m_pFontFile);
		m_HYStat.offsetToAxisValueOffsets= hy_cdr_int32_to(ulTmp);

		if (m_HYStat.majorVersion == 1 && m_HYStat.minorVersion>0)
		{
			fread(&usTmp,2,1,m_pFontFile);
			m_HYStat.elidedFallbackNameID= hy_cdr_int16_to(usTmp);		
		}

		if (m_HYStat.designAxisCount >0)
		{
			DecodeDesignAxes(tbEntry.offset+m_HYStat.designAxesOffset,m_HYStat);
			DecodeAxisValue(tbEntry.offset+m_HYStat.offsetToAxisValueOffsets,m_HYStat);
		}

		return HY_NOERROR;

	}	// end of int CHYFontCodec::DecodeSTAT()	

	void CHYFontCodec::DecodeDesignAxes(unsigned long offstTable,CHYSTAT& HYStat)
	{
		fseek(m_pFontFile,offstTable,SEEK_SET);
		
		unsigned long ulTmp=0;
		unsigned short usTmp=0;
		for (int i=0; i<HYStat.designAxisCount; i++)
		{	
			CHYAxisRecord	AxisRecord;
			
			fread(&ulTmp,4,1,m_pFontFile);
			AxisRecord.axisTag = hy_cdr_int32_to(ulTmp);
			fread(&usTmp,2,1,m_pFontFile);
			AxisRecord.axisNameID = hy_cdr_int16_to(usTmp);
			fread(&usTmp,2,1,m_pFontFile);
			AxisRecord.axisOrdering= hy_cdr_int16_to(usTmp);

			HYStat.vtdesignAxes.push_back(AxisRecord);
		}		
		
		for (int i=0; i<HYStat.axisValueCount; i++)
		{
			fread(&usTmp,2,1,m_pFontFile);
			usTmp = hy_cdr_int16_to(usTmp);
			HYStat.axisValueOffsets.push_back(usTmp);
		}

	}	// end of void CHYFontCodec::DecodeDesignAxes()

	void CHYFontCodec::DecodeAxisValue(unsigned long offstTable,CHYSTAT& HYStat)
	{
		unsigned long ulTmp=0;
		unsigned short usTmp=0;

		for (size_t i=0; i<HYStat.axisValueOffsets.size(); i++)
		{
			fseek(m_pFontFile,offstTable+HYStat.axisValueOffsets[i],SEEK_SET);
			fread(&usTmp,2,1,m_pFontFile);
			usTmp = hy_cdr_int16_to(usTmp);

			CHYAxisvalueFormat AxsVulFormat;
			if (usTmp==1)
			{
				AxsVulFormat.format = 1;

				fread(&usTmp,2,1,m_pFontFile);
				AxsVulFormat.axisIndex = hy_cdr_int16_to(usTmp);
				fread(&usTmp,2,1,m_pFontFile);
				AxsVulFormat.flags = hy_cdr_int16_to(usTmp);
				fread(&usTmp,2,1,m_pFontFile);
				AxsVulFormat.valueNameID = hy_cdr_int16_to(usTmp);
				fread(&usTmp,2,1,m_pFontFile);
				AxsVulFormat.fxValue.value = hy_cdr_int16_to(usTmp);
				fread(&usTmp,2,1,m_pFontFile);
				AxsVulFormat.fxValue.fract = hy_cdr_int16_to(usTmp);

				HYStat.vtAxisvalueFormat.push_back(AxsVulFormat);
			}

			if (usTmp==2)
			{	
				AxsVulFormat.format = 2;

				fread(&usTmp,2,1,m_pFontFile);
				AxsVulFormat.axisIndex = hy_cdr_int16_to(usTmp);
				fread(&usTmp,2,1,m_pFontFile);
				AxsVulFormat.flags = hy_cdr_int16_to(usTmp);
				fread(&usTmp,2,1,m_pFontFile);
				AxsVulFormat.valueNameID = hy_cdr_int16_to(usTmp);
				fread(&usTmp,2,1,m_pFontFile);
				AxsVulFormat.nominalValue.value = hy_cdr_int16_to(usTmp);
				fread(&usTmp,2,1,m_pFontFile);
				AxsVulFormat.nominalValue.fract = hy_cdr_int16_to(usTmp);
				fread(&usTmp,2,1,m_pFontFile);
				AxsVulFormat.rangeMinValue.value = hy_cdr_int16_to(usTmp);
				fread(&usTmp,2,1,m_pFontFile);
				AxsVulFormat.rangeMinValue.fract = hy_cdr_int16_to(usTmp);
				fread(&usTmp,2,1,m_pFontFile);
				AxsVulFormat.rangeMaxValue.value = hy_cdr_int16_to(usTmp);
				fread(&usTmp,2,1,m_pFontFile);
				AxsVulFormat.rangeMaxValue.fract = hy_cdr_int16_to(usTmp);

				HYStat.vtAxisvalueFormat.push_back(AxsVulFormat);
			}

			if (usTmp==3)
			{	
				AxsVulFormat.format = 3;

				fread(&usTmp,2,1,m_pFontFile);
				AxsVulFormat.axisIndex = hy_cdr_int16_to(usTmp);
				fread(&usTmp,2,1,m_pFontFile);
				AxsVulFormat.flags = hy_cdr_int16_to(usTmp);
				fread(&usTmp,2,1,m_pFontFile);
				AxsVulFormat.valueNameID = hy_cdr_int16_to(usTmp);
				fread(&usTmp,2,1,m_pFontFile);
				AxsVulFormat.fxValue.value = hy_cdr_int16_to(usTmp);
				fread(&usTmp,2,1,m_pFontFile);
				AxsVulFormat.fxValue.fract = hy_cdr_int16_to(usTmp);
				fread(&usTmp,2,1,m_pFontFile);
				AxsVulFormat.linkedValue.value = hy_cdr_int16_to(usTmp);
				fread(&usTmp,2,1,m_pFontFile);
				AxsVulFormat.linkedValue.fract = hy_cdr_int16_to(usTmp);

				HYStat.vtAxisvalueFormat.push_back(AxsVulFormat);
			}

			if (usTmp==4)
			{	
				AxsVulFormat.format = 4;

				fread(&usTmp,2,1,m_pFontFile);
				AxsVulFormat.axisCount = hy_cdr_int16_to(usTmp);
				fread(&usTmp,2,1,m_pFontFile);
				AxsVulFormat.flags = hy_cdr_int16_to(usTmp);
				fread(&usTmp,2,1,m_pFontFile);
				AxsVulFormat.valueNameID = hy_cdr_int16_to(usTmp);

				for (int j=0; j<AxsVulFormat.axisCount; j++)
				{
					CHYAxisValues		AxisValues;
					fread(&usTmp,2,1,m_pFontFile);
					AxisValues.axisIndex = hy_cdr_int16_to(usTmp);
					fread(&usTmp,2,1,m_pFontFile);
					AxisValues.fxValue.value = hy_cdr_int16_to(usTmp);
					fread(&usTmp,2,1,m_pFontFile);
					AxisValues.fxValue.fract = hy_cdr_int16_to(usTmp);
				}
				HYStat.vtAxisvalueFormat.push_back(AxsVulFormat);
			}
		}

	}	// end of void CHYFontCodec::DecodeAxisValue()

	void CHYFontCodec::DumpSTAT(CMarkup& MK)
	{
		char strTmp[MAX_PATH] = {0};

		MK.AddElem("STAT");
		MK.IntoElem();

		unsigned long Version = m_HYStat.majorVersion<<16;
		Version |= m_HYStat.minorVersion;
	
		MK.AddElem("Version");
		sprintf_s(strTmp,"0x%0*x",8,Version);
		MK.AddAttrib("value", strTmp);

		MK.AddElem("DesignAxisRecordSize");
		memset(strTmp,0,MAX_PATH);
		sprintf_s(strTmp, "%d", m_HYStat.designAxisSize);
		MK.AddAttrib("value", strTmp);

		MK.AddElem("DesignAxisRecord");
		MK.IntoElem();
		for (int i = 0; i < m_HYStat.designAxisCount; i++)
		{
			CHYAxisRecord& AxisRecord = m_HYStat.vtdesignAxes[0];
			MK.AddElem("Axis");
			memset(strTmp,0,MAX_PATH);
			sprintf_s(strTmp, "%d", i);
			MK.AddAttrib("index", strTmp);
			MK.IntoElem();

			MK.AddElem("AxisTag");
			memset(strTmp,0,MAX_PATH);
			sprintf_s(strTmp, "%x", AxisRecord.axisTag);			
			MK.AddAttrib("value", HY_Hex_to_string(strTmp));

			MK.AddElem("AxisNameID");			
			memset(strTmp,0,MAX_PATH);
			sprintf_s(strTmp, "%d", AxisRecord.axisNameID);
			MK.AddAttrib("value", strTmp);

			MK.AddElem("AxisOrdering");
			memset(strTmp,0,MAX_PATH);
			sprintf_s(strTmp, "%d", AxisRecord.axisOrdering);
			MK.AddAttrib("value", strTmp);

			MK.OutOfElem();
		}
		MK.OutOfElem(); // end of DesignAxisRecord

		MK.AddElem("AxisValueArray");
		MK.IntoElem();
		for (int i = 0; i < m_HYStat.axisValueCount; i++)
		{
			CHYAxisvalueFormat& AxFmt = m_HYStat.vtAxisvalueFormat[i];
			MK.AddElem("AxisValue");
			memset(strTmp,0,MAX_PATH);
			sprintf_s(strTmp, "%d", i);
			MK.AddAttrib("index", strTmp);
			memset(strTmp,0,MAX_PATH);
			sprintf_s(strTmp, "%d", AxFmt.format);
			MK.AddAttrib("Format", strTmp);
			MK.IntoElem();

			if (AxFmt.format == 1)
			{
				MK.AddElem("AxisIndex");
				memset(strTmp,0,MAX_PATH);
				sprintf_s(strTmp, "%d", AxFmt.axisIndex);
				MK.AddAttrib("value", strTmp);

				MK.AddElem("flags");
				memset(strTmp,0,MAX_PATH);
				sprintf_s(strTmp, "%d", AxFmt.flags);
				MK.AddAttrib("value", strTmp);

				MK.AddElem("ValueNameID");
				memset(strTmp,0,MAX_PATH);
				sprintf_s(strTmp, "%d", AxFmt.valueNameID);
				MK.AddAttrib("value", strTmp);

				MK.AddElem("Value");
				memset(strTmp,0,MAX_PATH);
				sprintf_s(strTmp, "%f", HY_HYFIXED_to_float(AxFmt.fxValue));
				MK.AddAttrib("value", strTmp);
			}

			if (AxFmt.format == 2)
			{
				MK.AddElem("AxisIndex");
				memset(strTmp,0,MAX_PATH);
				sprintf_s(strTmp, "%d", AxFmt.axisIndex);
				MK.AddAttrib("value", strTmp);

				MK.AddElem("flags");
				memset(strTmp, 0, MAX_PATH);
				sprintf_s(strTmp, "%d", AxFmt.flags);
				MK.AddAttrib("value", strTmp);

				MK.AddElem("ValueNameID");
				memset(strTmp, 0, MAX_PATH);
				sprintf_s(strTmp, "%d", AxFmt.valueNameID);
				MK.AddAttrib("value", strTmp);

				MK.AddElem("nominalValue");
				memset(strTmp, 0, MAX_PATH);
				sprintf_s(strTmp, "%f", HY_HYFIXED_to_float(AxFmt.nominalValue));
				MK.AddAttrib("value", strTmp);

				MK.AddElem("rangeMinValue");
				memset(strTmp, 0, MAX_PATH);
				sprintf_s(strTmp, "%f", HY_HYFIXED_to_float(AxFmt.rangeMinValue));
				MK.AddAttrib("value", strTmp);

				MK.AddElem("rangeMaxValue");
				memset(strTmp, 0, MAX_PATH);
				sprintf_s(strTmp, "%f", HY_HYFIXED_to_float(AxFmt.rangeMaxValue));
				MK.AddAttrib("value", strTmp);
			}

			if (AxFmt.format == 3)
			{
				MK.AddElem("AxisIndex");
				memset(strTmp, 0, MAX_PATH);
				sprintf_s(strTmp, "%d", AxFmt.axisIndex);
				MK.AddAttrib("value", strTmp);

				MK.AddElem("flags");
				memset(strTmp, 0, MAX_PATH);
				sprintf_s(strTmp, "%d", AxFmt.flags);
				MK.AddAttrib("value", strTmp);

				MK.AddElem("ValueNameID");
				memset(strTmp, 0, MAX_PATH);
				sprintf_s(strTmp, "%d", AxFmt.valueNameID);
				MK.AddAttrib("value", strTmp);

				MK.AddElem("value");
				memset(strTmp, 0, MAX_PATH);
				sprintf_s(strTmp, "%f", HY_HYFIXED_to_float(AxFmt.fxValue));
				MK.AddAttrib("value", strTmp);

				MK.AddElem("linkedValue");
				memset(strTmp, 0, MAX_PATH);
				sprintf_s(strTmp, "%f", HY_HYFIXED_to_float(AxFmt.linkedValue));
				MK.AddAttrib("value", strTmp);
			}

			if (AxFmt.format == 4)
			{
				MK.AddElem("axisCount");
				memset(strTmp, 0, MAX_PATH);
				sprintf_s(strTmp, "%d", AxFmt.axisCount);
				MK.AddAttrib("value", strTmp);

				MK.AddElem("flags");
				memset(strTmp, 0, MAX_PATH);
				sprintf_s(strTmp, "%d", AxFmt.flags);
				MK.AddAttrib("value", strTmp);

				MK.AddElem("ValueNameID");
				memset(strTmp, 0, MAX_PATH);
				sprintf_s(strTmp, "%d", AxFmt.valueNameID);
				MK.AddAttrib("value", strTmp);

				for (int j = 0; j < AxFmt.axisCount; j++)
				{
					MK.AddElem("AxisValue Record");
					MK.IntoElem();

					MK.AddElem("axisIndex");
					memset(strTmp, 0, MAX_PATH);
					sprintf_s(strTmp, "%d", AxFmt.vtAxisValues[j].axisIndex);
					MK.AddAttrib("value",strTmp);

					MK.AddElem("value");
					memset(strTmp, 0, MAX_PATH);
					sprintf_s(strTmp, "%f", HY_HYFIXED_to_float(AxFmt.vtAxisValues[j].fxValue));
					MK.AddAttrib("value", strTmp);

					MK.OutOfElem();
				}
			}
			MK.OutOfElem();
		}
		MK.OutOfElem();
		MK.OutOfElem();

	}	// end of void CHYFontCodec::DumpSTAT()

	int CHYFontCodec::EncodeSTAT()
	{
		unsigned long ulTableBegin = ftell(m_pFontFile);
		unsigned long ulGlyphOffset = 0;

		int iEntryIndex = m_HYTbDirectory.FindTableEntry(STAT_TAG);
		if (iEntryIndex == -1)  return FONT_ERR_STAT_ENCODE;

		CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
		tbEntry.offset = ftell(m_pFontFile);

		unsigned short usTmp = 0;
		unsigned long ulTmp = 0;

		//Major version number of the style attributes table — set to 1.
		usTmp = hy_cdr_int16_to(m_HYStat.majorVersion);
		fwrite(&usTmp, 2, 1, m_pFontFile);
		//Minor version number of the style attributes table — set to 2.
		usTmp = hy_cdr_int16_to(m_HYStat.minorVersion);
		fwrite(&usTmp, 2, 1, m_pFontFile);

		//The size in bytes of each axis record.
		// 8 bytes Tag 	axisTag + uint16 axisNameID + uint16 axisOrdering
		m_HYStat.designAxisSize = 8;
		usTmp = hy_cdr_int16_to(m_HYStat.designAxisSize);
		fwrite(&usTmp, 2, 1, m_pFontFile);

		//The number of design axis records. 
		//In a font with an 'fvar' table, this value must be greater than or equal to the axisCount value in the 'fvar' table.
		//In all fonts, must be greater than zero if axisValueCount is greater than zero.		
		usTmp = hy_cdr_int16_to(m_HYStat.designAxisCount);
		fwrite(&usTmp, 2, 1, m_pFontFile);

		//Offset in bytes from the beginning of the STAT table to the start of the design axes array. 
		//If designAxisCount is zero, set to zero; 
		//if designAxisCount is greater than zero, must be greater than zero.
		unsigned long ulDesignAxesOffsetPOS = ftell(m_pFontFile);
		ulTmp = 0;
		fwrite(&ulTmp, 4, 1, m_pFontFile);

		// The number of axis value tables.		
		usTmp = hy_cdr_int16_to(m_HYStat.axisValueCount);
		fwrite(&usTmp, 2, 1, m_pFontFile);

		//Offset in bytes from the beginning of the STAT table to the start of the design axes value offsets array. 
		//If axisValueCount is zero, set to zero; 
		//if axisValueCount is greater than zero, must be greater than zero.
		unsigned long offsetToAxisValueOffsets = ftell(m_pFontFile);
		ulTmp = 0;
		fwrite(&ulTmp, 4, 1, m_pFontFile);

		// Name ID used as fallback when projection of names into a particular font model produces a subfamily name containing only elidable elements.
		// In version 1.0 of the style attributes table, the elidedFallbackNameId field was not included.
		if (m_HYStat.majorVersion + m_HYStat.minorVersion > 1)	{
			usTmp = hy_cdr_int16_to(m_HYStat.elidedFallbackNameID);
			fwrite(&usTmp, 2, 1, m_pFontFile);
		}

		// set DesignAxesOffset;
		unsigned long DesignAxesOffset = ftell(m_pFontFile) - tbEntry.offset;
		fseek(m_pFontFile, ulDesignAxesOffsetPOS,SEEK_SET);
		ulTmp = hy_cdr_int32_to(DesignAxesOffset);
		fwrite(&ulTmp, 4, 1, m_pFontFile);
		fseek(m_pFontFile, tbEntry.offset+ DesignAxesOffset, SEEK_SET);

		for (int i=0; i <m_HYStat.designAxisCount; i++)	{
			CHYAxisRecord&  AxisRcrd = m_HYStat.vtdesignAxes[i];

			//A tag identifying the axis of design variation
			ulTmp = hy_cdr_int32_to(AxisRcrd.axisTag);
			fwrite(&ulTmp, 4, 1, m_pFontFile);

			//A tag identifying the axis of design variation
			usTmp = hy_cdr_int16_to(AxisRcrd.axisNameID);
			fwrite(&usTmp, 2, 1, m_pFontFile);

			//uint16 	axisOrdering 	A value that applications can use to determine primary sorting of face names, 
			//or for ordering of descriptors when composing family or face names.
			usTmp = hy_cdr_int16_to(AxisRcrd.axisOrdering);
			fwrite(&usTmp, 2, 1, m_pFontFile);
		}		

		
		// set offsetToAxisValueOffsets
		unsigned long ulAxisValueOffset = ftell(m_pFontFile) - tbEntry.offset;
		fseek(m_pFontFile, offsetToAxisValueOffsets, SEEK_SET);
		ulTmp = hy_cdr_int32_to(ulAxisValueOffset);
		fwrite(&ulTmp, 4, 1, m_pFontFile);
		fseek(m_pFontFile, tbEntry.offset + ulAxisValueOffset, SEEK_SET);

		unsigned short axisValueOffset = 0;		
		//offset占位 Offset16
		usTmp = 0;
		for (int i = 0; i < m_HYStat.axisValueCount; i++) {
			fwrite(&usTmp, 2, 1, m_pFontFile);
			axisValueOffset+=2;
		}			
		
		for (int i = 0; i < m_HYStat.axisValueCount; i++){
			m_HYStat.axisValueOffsets.push_back(axisValueOffset);
			CHYAxisvalueFormat& valueFormat = m_HYStat.vtAxisvalueFormat[i];

			usTmp = hy_cdr_int16_to(valueFormat.format);
			fwrite(&usTmp, 2, 1, m_pFontFile);
			axisValueOffset += 2;
			if (valueFormat.format == 1) {
				//Zero-base index into the axis record array identifying the axis of design variation to which the axis value record applies.
				//Must be less than designAxisCount.
				usTmp = hy_cdr_int16_to(valueFormat.axisIndex);
				fwrite(&usTmp, 2, 1, m_pFontFile);
				axisValueOffset += 2;
				//Flags 
				usTmp = hy_cdr_int16_to(valueFormat.flags);
				fwrite(&usTmp, 2, 1, m_pFontFile);
				axisValueOffset += 2;
				//The name ID for entries in the 'name' table that 
				//provide a display string for this attribute value
				usTmp = hy_cdr_int16_to(valueFormat.valueNameID);
				fwrite(&usTmp, 2, 1, m_pFontFile);
				axisValueOffset += 2;
				//A numeric value for this attribute value.
				usTmp = hy_cdr_int16_to(valueFormat.fxValue.value);
				fwrite(&usTmp, 2, 1, m_pFontFile);
				axisValueOffset += 2;
				usTmp = hy_cdr_int16_to(valueFormat.fxValue.fract);
				fwrite(&usTmp, 2, 1, m_pFontFile);
				axisValueOffset += 2;
			}

			if (valueFormat.format == 2) {
				//Zero-base index into the axis record array identifying the axis of design variation to which the axis value record applies.
				//Must be less than designAxisCount.
				usTmp = hy_cdr_int16_to(valueFormat.axisIndex);
				fwrite(&usTmp, 2, 1, m_pFontFile);
				axisValueOffset += 2;
				//Flags 
				usTmp = hy_cdr_int16_to(valueFormat.flags);
				fwrite(&usTmp, 2, 1, m_pFontFile);
				axisValueOffset += 2;
				//The name ID for entries in the 'name' table that 
				//provide a display string for this attribute value. 
				usTmp = hy_cdr_int16_to(valueFormat.valueNameID);
				fwrite(&usTmp, 2, 1, m_pFontFile);
				axisValueOffset += 2;
				//A nominal numeric value for this attribute value.
				usTmp = hy_cdr_int16_to(valueFormat.nominalValue.value);
				fwrite(&usTmp, 2, 1, m_pFontFile);
				axisValueOffset += 2;
				usTmp = hy_cdr_int16_to(valueFormat.nominalValue.fract);
				fwrite(&usTmp, 2, 1, m_pFontFile);
				axisValueOffset += 2;
				//The minimum value for a range associated with the specified name ID.
				usTmp = hy_cdr_int16_to(valueFormat.rangeMinValue.value);
				fwrite(&usTmp, 2, 1, m_pFontFile);
				axisValueOffset += 2;
				usTmp = hy_cdr_int16_to(valueFormat.rangeMinValue.fract);
				fwrite(&usTmp, 2, 1, m_pFontFile);
				axisValueOffset += 2;
				//The maximum value for a range associated with the specified name ID.
				usTmp = hy_cdr_int16_to(valueFormat.rangeMaxValue.value);
				fwrite(&usTmp, 2, 1, m_pFontFile);
				axisValueOffset += 2;
				usTmp = hy_cdr_int16_to(valueFormat.rangeMaxValue.fract);
				fwrite(&usTmp, 2, 1, m_pFontFile);
				axisValueOffset += 2;
			}

			if (valueFormat.format == 3) {
				//Zero-base index into the axis record array identifying the axis of design variation to which the axis value record applies.
				//Must be less than designAxisCount.
				usTmp = hy_cdr_int16_to(valueFormat.axisIndex);
				fwrite(&usTmp, 2, 1, m_pFontFile);
				axisValueOffset += 2;
				//Flags 
				usTmp = hy_cdr_int16_to(valueFormat.flags);
				fwrite(&usTmp, 2, 1, m_pFontFile);
				axisValueOffset += 2;
				//The name ID for entries in the 'name' table that 
				//provide a display string for this attribute value. 
				usTmp = hy_cdr_int16_to(valueFormat.valueNameID);
				fwrite(&usTmp, 2, 1, m_pFontFile);
				axisValueOffset += 2;
				//A numeric value for this attribute value.
				usTmp = hy_cdr_int16_to(valueFormat.fxValue.value);
				fwrite(&usTmp, 2, 1, m_pFontFile);
				axisValueOffset += 2;
				usTmp = hy_cdr_int16_to(valueFormat.fxValue.fract);
				fwrite(&usTmp, 2, 1, m_pFontFile);
				axisValueOffset += 2;
				//The numeric value for a style-linked mapping from this value.
				usTmp = hy_cdr_int16_to(valueFormat.linkedValue.value);
				fwrite(&usTmp, 2, 1, m_pFontFile);
				axisValueOffset += 2;
				usTmp = hy_cdr_int16_to(valueFormat.linkedValue.fract);
				fwrite(&usTmp, 2, 1, m_pFontFile);
				axisValueOffset += 2;
			}

			if (valueFormat.format == 4) {
				//The total number of axes contributing to this axis-values combination.				
				usTmp = hy_cdr_int16_to(valueFormat.axisCount);
				fwrite(&usTmp, 2, 1, m_pFontFile);
				axisValueOffset += 2;
				//Flags 
				usTmp = hy_cdr_int16_to(valueFormat.flags);
				fwrite(&usTmp, 2, 1, m_pFontFile);
				axisValueOffset += 2;
				//The name ID for entries in the 'name' table that 
				//provide a display string for this attribute value. 
				usTmp = hy_cdr_int16_to(valueFormat.valueNameID);
				fwrite(&usTmp, 2, 1, m_pFontFile);
				axisValueOffset += 2;

				for (int j = 0; j < valueFormat.axisCount; j++)	{
					CHYAxisValues& AxsVlus = valueFormat.vtAxisValues[j];

					//Zero-base index into the axis record array identifying the axis to which this value applies. 
					//Must be less than designAxisCount.
					usTmp = hy_cdr_int16_to(AxsVlus.axisIndex);
					fwrite(&usTmp, 2, 1, m_pFontFile);
					axisValueOffset += 2;

					//A numeric value for this attribute value.					
					usTmp = hy_cdr_int16_to(AxsVlus.fxValue.value);
					fwrite(&usTmp, 2, 1, m_pFontFile);
					axisValueOffset += 2;
					usTmp = hy_cdr_int16_to(AxsVlus.fxValue.fract);
					fwrite(&usTmp, 2, 1, m_pFontFile);
					axisValueOffset += 2;
				}
			}
		}

		unsigned long ulEndPos =  ftell(m_pFontFile);
		fseek(m_pFontFile, tbEntry.offset + ulAxisValueOffset,SEEK_SET);
		for (int i = 0; i < m_HYStat.axisValueCount; i++) {			
			usTmp = hy_cdr_int16_to(m_HYStat.axisValueOffsets[i]);
			fwrite(&usTmp, 2, 1, m_pFontFile);
		}

		fseek(m_pFontFile, ulEndPos, SEEK_SET);

		tbEntry.length = ftell(m_pFontFile) - tbEntry.offset;
		::HY_4ByteAlign(m_pFontFile, tbEntry.length);

		return HY_NOERROR;

	}	// end of int CHYFontCodec::EncodeSTAT()

	int CHYFontCodec::DecodeVariationRegionList(long Offset, CHYItemVariationStore*	pitmVratnStr)
	{
		unsigned short usTmp = 0;
		unsigned long ulTmp = 0;

		fseek(m_pFontFile, Offset, SEEK_SET);

		VariationRegionList& VrtnRgnLst = pitmVratnStr->variationRglst;
		//axisCount
		fread(&usTmp, 2, 1, m_pFontFile);
		VrtnRgnLst.axisCount = hy_cdr_int16_to(usTmp);

		//regionCount
		fread(&usTmp, 2, 1, m_pFontFile);
		VrtnRgnLst.regionCount = hy_cdr_int16_to(usTmp);

		//Array of variation regions.
		for (int i = 0; i < VrtnRgnLst.regionCount; i++){
			VariationRegion		vrtnRgn;
			for (int j = 0; j < VrtnRgnLst.axisCount; j++){
				RegionAxisCoordinates 	regionAxes;
				//startCoord
				fread(&usTmp, 2, 1, m_pFontFile);
				regionAxes.startCoord = hy_cdr_int16_to(usTmp);
				//peakCoord
				fread(&usTmp, 2, 1, m_pFontFile);
				regionAxes.peakCoord = hy_cdr_int16_to(usTmp);
				//endCoord
				fread(&usTmp, 2, 1, m_pFontFile);
				regionAxes.endCoord = hy_cdr_int16_to(usTmp);

				vrtnRgn.vtRegionAxes.push_back(regionAxes);
			}

			vrtnRgn.axisCount = VrtnRgnLst.axisCount;
			VrtnRgnLst.vtVariationRegion.push_back(vrtnRgn);
		}

		return HY_NOERROR;

	}	// end of int CHYFontCodec::DecodeVariationRegionList()

	int CHYFontCodec::DecodeItemVariationData(long Offset,CHYItemVariationData& itmVrtndt)
	{
		unsigned short usTmp = 0;
		unsigned long ulTmp = 0;

		fseek(m_pFontFile, Offset, SEEK_SET);

		//itemCount
		fread(&usTmp, 2, 1, m_pFontFile);
		itmVrtndt.ItemCount = hy_cdr_int16_to(usTmp);

		//shortDeltaCount
		fread(&usTmp, 2, 1, m_pFontFile);
		itmVrtndt.ShortDeltaCount = hy_cdr_int16_to(usTmp);

		//regionIndexCount
		fread(&usTmp, 2, 1, m_pFontFile);
		itmVrtndt.RegionIndexCount = hy_cdr_int16_to(usTmp);

		for (int i = 0; i < itmVrtndt.RegionIndexCount; i++)
		{
			//regionIndexes
			fread(&usTmp, 2, 1, m_pFontFile);
			usTmp = hy_cdr_int16_to(usTmp);

			itmVrtndt.vtRegionIndexes.push_back(usTmp);
		}

		for (int i = 0; i < itmVrtndt.ItemCount; i++)
		{
			CHYDeltaSet	dltSt;

			for (int j = 0; j < itmVrtndt.ShortDeltaCount; j++)
			{
				fread(&usTmp, 2, 1, m_pFontFile);
				usTmp = hy_cdr_int16_to(usTmp);
				dltSt.vtDeltaData.push_back((short)usTmp);
			}			

			int iDff = itmVrtndt.RegionIndexCount - itmVrtndt.ShortDeltaCount;
			for (int j = 0; j < iDff; j++)
			{
				char ch=0;
				fread(&ch, 1, 1, m_pFontFile);
				dltSt.vtDeltaData.push_back((short)ch);
			}
			itmVrtndt.vtdeltaSets.push_back(dltSt);
		}

		return HY_NOERROR;

	}	// end of int CHYFontCodec::DecodeItemVariationData()	

	int CHYFontCodec::DecodeDeltaSetIndexMap(long Offset, DeltaSetIndexMap&  dltstInxmp)
	{
		unsigned short usTmp = 0;
		unsigned long ulTmp = 0;

		fseek(m_pFontFile, Offset, SEEK_SET);

		//entryFormat
		fread(&usTmp, 2, 1, m_pFontFile);
		dltstInxmp.entryFormat = hy_cdr_int16_to(usTmp);
		//mapCount
		fread(&usTmp, 2, 1, m_pFontFile);
		dltstInxmp.mapCount = hy_cdr_int16_to(usTmp);
		int iTest = dltstInxmp.entryFormat&MAP_ENTRY_SIZE_MASK;
		dltstInxmp.oneEntrySize = ((dltstInxmp.entryFormat&MAP_ENTRY_SIZE_MASK) >> 4) + 1;
		int mapDataSize = dltstInxmp.oneEntrySize * dltstInxmp.mapCount;
		
		char ch=0;
		for (int i = 0; i < mapDataSize; i++)
		{	
			fread(&ch, 1, 1, m_pFontFile);
			dltstInxmp.vtmapData.push_back(ch);
		}

		return HY_NOERROR;

	}	// end of int CHYFontCodec::DecodeitemVariationStore()

	int CHYFontCodec::DecodeitemVariationStore(long Offset, long tbFlag)
	{
		unsigned short usTmp = 0;
		unsigned long ulTmp = 0;
		
		fseek(m_pFontFile, Offset, SEEK_SET);

		CHYItemVariationStore*	pitmVratnStr = NULL;
		if (tbFlag == VVAR_TAG)
		{
			pitmVratnStr = &m_HYVVAR.itemVariationStore;
		}
		else if (tbFlag == HVAR_TAG)
		{
			pitmVratnStr = &m_HYHVAR.itemVariationStore;
		}

		//format
		fread(&usTmp, 2, 1, m_pFontFile);
		pitmVratnStr->format= hy_cdr_int16_to(usTmp);
		//variationRegionListOffset
		fread(&ulTmp, 4, 1, m_pFontFile);
		pitmVratnStr->variationRegionListOffset = hy_cdr_int32_to(ulTmp);
		//itemVariationDataCount
		fread(&usTmp, 2, 1, m_pFontFile);
		pitmVratnStr->itemVariationDataCount = hy_cdr_int16_to(usTmp);

		//itemVariationDataOffsets[itemVariationDataCount]
		for (int i = 0; i < pitmVratnStr->itemVariationDataCount; i++)
		{
			fread(&ulTmp, 4, 1, m_pFontFile);
			ulTmp = hy_cdr_int32_to(ulTmp);
			pitmVratnStr->vtitemVariationDataOffsets.push_back(ulTmp);
		}

		//VariationRegionList		
		DecodeVariationRegionList(Offset + pitmVratnStr->variationRegionListOffset, pitmVratnStr);

		//itemVariationData
		for (int i = 0; i < pitmVratnStr->itemVariationDataCount; i++)
		{
			CHYItemVariationData itmVrtndt;
			long Offst = Offset + pitmVratnStr->vtitemVariationDataOffsets[i];
			DecodeItemVariationData(Offst, itmVrtndt);
			pitmVratnStr->vtItemVariationData.push_back(itmVrtndt);
		}

		return HY_NOERROR;

	}	// end of int CHYFontCodec::DecodeitemVariationStore()

	int	CHYFontCodec::DecodeHVAR()
	{
		m_HYHVAR.SetDefault();

		int iEntryIndex = m_HYTbDirectory.FindTableEntry(HVAR_TAG);
		if (iEntryIndex == -1) return FONT_ERR_HVAR_DECODE;

		CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
		fseek(m_pFontFile, tbEntry.offset, SEEK_SET);

		unsigned short usTmp = 0;
		unsigned long ulTmp = 0;
		fread(&usTmp, 2, 1, m_pFontFile);
		m_HYHVAR.majorVersion = hy_cdr_int16_to(usTmp);
		fread(&usTmp, 2, 1, m_pFontFile);
		m_HYHVAR.minorVersion = hy_cdr_int16_to(usTmp);

		fread(&ulTmp, 4, 1, m_pFontFile);
		m_HYHVAR.itemVariationStoreOffset = hy_cdr_int32_to(ulTmp);
		fread(&ulTmp, 4, 1, m_pFontFile);
		m_HYHVAR.advanceWidthMappingOffset = hy_cdr_int32_to(ulTmp);
		fread(&ulTmp, 4, 1, m_pFontFile);
		m_HYHVAR.lsbMappingOffset = hy_cdr_int32_to(ulTmp);
		fread(&ulTmp, 4, 1, m_pFontFile);
		m_HYHVAR.rsbMappingOffset = hy_cdr_int32_to(ulTmp);

		//itemVariationStore
		DecodeitemVariationStore(tbEntry.offset + m_HYHVAR.itemVariationStoreOffset, HVAR_TAG);

		if (m_HYHVAR.advanceWidthMappingOffset != NULL)
		{
			long Offst = tbEntry.offset + m_HYHVAR.advanceWidthMappingOffset;
			DecodeDeltaSetIndexMap(Offst, m_HYHVAR.advanceWidthMapping);
		}

		if (m_HYHVAR.lsbMappingOffset != NULL)
		{
			long Offst = tbEntry.offset + m_HYHVAR.lsbMappingOffset;
			DecodeDeltaSetIndexMap(Offst, m_HYHVAR.lsbMapping);
		}

		if (m_HYHVAR.rsbMappingOffset != NULL)
		{
			long Offst = tbEntry.offset + m_HYHVAR.rsbMappingOffset;
			DecodeDeltaSetIndexMap(Offst, m_HYHVAR.rsbMapping);
		}

		return HY_NOERROR;

	}	// end of int CHYFontCodec::DecodeHVAR()

	void CHYFontCodec::DumpHVAR(CMarkup& MK)
	{
		char strTmp[MAX_PATH] = { 0 };

		MK.AddElem("HVAR");
		{
			MK.IntoElem();

			unsigned long Version = m_HYHVAR.majorVersion << 16;
			Version |= m_HYHVAR.minorVersion;
			MK.AddElem("Version");
			sprintf_s(strTmp, "0x%0*x",8,Version);
			MK.AddAttrib("value", strTmp);

			MK.AddElem("VarStore");
			CHYItemVariationStore& VrtnStr = m_HYHVAR.itemVariationStore;
			memset(strTmp,0, MAX_PATH);
			sprintf_s(strTmp, "%d", VrtnStr.format);
			MK.AddAttrib("Format", strTmp);
			{
				MK.IntoElem();
				MK.AddElem("Format");
				MK.AddAttrib("value", strTmp);
				MK.AddElem("VarRegionList");
				{
					MK.IntoElem();
					VariationRegionList&  vrtnrgnlst = VrtnStr.variationRglst;
					for (int i = 0; i < vrtnrgnlst.regionCount; i++)
					{
						VariationRegion&  vrgn = vrtnrgnlst.vtVariationRegion[i];
						MK.AddElem("Region");
						sprintf_s(strTmp, "%d", i);
						MK.AddAttrib("index", strTmp);
						{
							MK.IntoElem();
							for (int j = 0; j < vrgn.axisCount; j++)
							{
								RegionAxisCoordinates& rgnxcdnts = vrgn.vtRegionAxes[j];
								MK.AddElem("VarRegionAxis");
								sprintf_s(strTmp, "%d", j);
								MK.AddAttrib("index", strTmp);
								{
									MK.IntoElem();
									MK.AddElem("StartCoord");
									sprintf_s(strTmp, "%.1f", HY_F2DOT14_to_float(rgnxcdnts.startCoord));
									MK.AddAttrib("value", strTmp);

									MK.AddElem("PeakCoord");
									sprintf_s(strTmp, "%.1f", HY_F2DOT14_to_float(rgnxcdnts.peakCoord));
									MK.AddAttrib("value", strTmp);

									MK.AddElem("EndCoord");
									sprintf_s(strTmp, "%.1f", HY_F2DOT14_to_float(rgnxcdnts.endCoord));
									MK.AddAttrib("value", strTmp);
									MK.OutOfElem();
								}
							}
							MK.OutOfElem();
						}
					}
					MK.OutOfElem();
				}	// end of VarRegionList

				for (int i = 0; i<VrtnStr.itemVariationDataCount; i++)
				{
					CHYItemVariationData&  VData = VrtnStr.vtItemVariationData[i];
					MK.AddElem("VarData");
					{
						MK.IntoElem();
						{
							MK.AddElem("NumShorts");
							sprintf_s(strTmp, "%d", VData.ShortDeltaCount);
							MK.AddAttrib("value", strTmp);

							for (int j = 0; j < VData.RegionIndexCount; j++)
							{
								MK.AddElem("VarRegionIndex");
								sprintf_s(strTmp, "%d", j);
								MK.AddAttrib("index", strTmp);
								sprintf_s(strTmp, "%d", VData.vtRegionIndexes[j]);
								MK.AddAttrib("value", strTmp);
							}

							for (int j = 0; j < VData.ItemCount; j++)
							{
								CHYDeltaSet& DltSt = VData.vtdeltaSets[j];

								MK.AddElem("Item");
								sprintf_s(strTmp, "%d", j);
								MK.AddAttrib("index", strTmp);

								size_t st = DltSt.vtDeltaData.size();
								ostringstream ostrStrm;
								ostrStrm << "[";
								for (size_t x = 0; x < st; x++)
								{									
									ostrStrm << DltSt.vtDeltaData[x];
									//strDltValue += ostrStrm.str();
									if (x!=st-1)
										ostrStrm <<",  ";
								}
								ostrStrm << "]";

								std::string strDltValue = ostrStrm.str();
								MK.AddAttrib("value", strDltValue);
							}
						}
						MK.OutOfElem();
					}	// end of VarData
				}
				MK.OutOfElem();
			}// end of VarStore

			if (m_HYHVAR.advanceWidthMappingOffset != 0)
			{
				MK.AddElem("AdvWidthMap");
				{
					MK.IntoElem();
					DeltaSetIndexMap& AdvWidthMap = m_HYHVAR.advanceWidthMapping;
					for (int i = 0; i < AdvWidthMap.mapCount; i++)
					{	
						MK.AddElem("Map");
						sprintf_s(strTmp, "%d",i);
						MK.AddAttrib("index", strTmp);

						unsigned long Entry = 0;						
						for (int j = 0; j < AdvWidthMap.oneEntrySize; j++)
						{
							int iStep = i * AdvWidthMap.oneEntrySize + j;
							int iData =  AdvWidthMap.vtmapData[iStep];

							Entry |= iData << j;
						}
						int outerIndex = Entry >> ((AdvWidthMap.entryFormat & INNER_INDEX_BIT_COUNT_MASK) + 1);
						int innerIndex = Entry & ((1 << ((AdvWidthMap.entryFormat & INNER_INDEX_BIT_COUNT_MASK) + 1)) - 1);
						sprintf_s(strTmp, "%d", outerIndex);
						MK.AddAttrib("outer", strTmp);
						sprintf_s(strTmp, "%d", innerIndex);
						MK.AddAttrib("inner", strTmp);
					}
					MK.OutOfElem();
				}	//end of AdvWidthMap
			}

			if (m_HYHVAR.lsbMappingOffset != 0)
			{
				MK.AddElem("lsbMap");
				{
					MK.IntoElem();
					DeltaSetIndexMap& lsbMap = m_HYHVAR.lsbMapping;
					for (int i = 0; i < lsbMap.mapCount; i++)
					{
						MK.AddElem("Map");
						sprintf_s(strTmp, "%d", i);
						MK.AddAttrib("index", strTmp);

						unsigned long Entry = 0;
						for (int j = 0; j < lsbMap.oneEntrySize; j++)
						{
							int iStep = i * lsbMap.oneEntrySize + j;
							int iData = lsbMap.vtmapData[iStep];

							Entry |= iData << j;
						}
						int outerIndex = Entry >> ((lsbMap.entryFormat & INNER_INDEX_BIT_COUNT_MASK) + 1);
						int innerIndex = Entry & ((1 << ((lsbMap.entryFormat & INNER_INDEX_BIT_COUNT_MASK) + 1)) - 1);
						sprintf_s(strTmp, "%d", outerIndex);
						MK.AddAttrib("outer", strTmp);
						sprintf_s(strTmp, "%d", innerIndex);
						MK.AddAttrib("inner", strTmp);
					}
					MK.OutOfElem();
				}	//end of lsbMap
			}

			if (m_HYHVAR.rsbMappingOffset != 0)
			{
				MK.AddElem("rsbMap");
				{
					MK.IntoElem();
					DeltaSetIndexMap& rsbMap = m_HYHVAR.rsbMapping;
					for (int i = 0; i < rsbMap.mapCount; i++)
					{
						MK.AddElem("Map");
						sprintf_s(strTmp, "%d", i);
						MK.AddAttrib("index", strTmp);

						unsigned long Entry = 0;
						for (int j = 0; j < rsbMap.oneEntrySize; j++)
						{
							int iStep = i * rsbMap.oneEntrySize + j;
							int iData = rsbMap.vtmapData[iStep];

							Entry |= iData << j;
						}
						int outerIndex = Entry >> ((rsbMap.entryFormat & INNER_INDEX_BIT_COUNT_MASK) + 1);
						int innerIndex = Entry & ((1 << ((rsbMap.entryFormat & INNER_INDEX_BIT_COUNT_MASK) + 1)) - 1);
						sprintf_s(strTmp, "%d", outerIndex);
						MK.AddAttrib("outer", strTmp);
						sprintf_s(strTmp, "%d", innerIndex);
						MK.AddAttrib("inner", strTmp);
					}
					MK.OutOfElem();
				}	//end of rsbMap
			}

			MK.OutOfElem();
		}	// end of HVAR 

	}	// end of void CHYFontCodec::DumpHVAR()

	int CHYFontCodec::EnCodeHVAR()
	{
		unsigned long ulTableBegin = ftell(m_pFontFile);
		unsigned long ulGlyphOffset = 0;

		int iEntryIndex = m_HYTbDirectory.FindTableEntry(HVAR_TAG);
		if (iEntryIndex == -1)  return FONT_ERR_HVAR_ENCODE;

		CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
		tbEntry.offset = ftell(m_pFontFile);

		unsigned short usTmp = 0;
		unsigned long ulTmp = 0;

		//Major version number of the horizontal metrics variations table
		usTmp = hy_cdr_int16_to(m_HYHVAR.majorVersion);
		fwrite(&usTmp, 2, 1, m_pFontFile);
		//Minor version number of the horizontal metrics variations table  
		usTmp = hy_cdr_int16_to(m_HYHVAR.minorVersion);
		fwrite(&usTmp, 2, 1, m_pFontFile);

		unsigned long VrtnStoreOffsetPos = ftell(m_pFontFile);		
		fwrite(&ulTmp, 4, 1, m_pFontFile);

		unsigned long AdwOffsetPos=0,LsbOffsetPos=0,RsbOffsetPos=0;
		if (m_HYHVAR.IsadvanceWidthMapping) {
			AdwOffsetPos = ftell(m_pFontFile);
			fwrite(&ulTmp, 4, 1, m_pFontFile);
		}
		if (m_HYHVAR.IslsbMapping) {
			LsbOffsetPos = ftell(m_pFontFile);
			fwrite(&ulTmp, 4, 1, m_pFontFile);
		}
		if (m_HYHVAR.IsrsbMapping){
			RsbOffsetPos = ftell(m_pFontFile);
			fwrite(&ulTmp, 4, 1, m_pFontFile);
		}

		// itemVariationStore
		// Offset in bytes from the 
		// start of this table to the item variation store table.
		unsigned long VrtnStoreOffset = tbEntry.offset - ftell(m_pFontFile);
		fseek(m_pFontFile, VrtnStoreOffsetPos,SEEK_SET);
		ulTmp = hy_cdr_int16_to(VrtnStoreOffset);
		fwrite(&ulTmp, 4, 1, m_pFontFile);
		fseek(m_pFontFile, tbEntry.offset+VrtnStoreOffset, SEEK_SET);

		//itemVariationStore
		CHYItemVariationStore& itemVariationStore = m_HYHVAR.itemVariationStore;
		EnCodeItemVariationStore(m_pFontFile, itemVariationStore);

		//advanceWidthMappingOffset
		if (m_HYHVAR.IsadvanceWidthMapping) {
			//Offset in bytes from the start of this table to the delta-set index mapping for advance widths (may be NULL).		
			unsigned long adwMappingOffset = tbEntry.offset - ftell(m_pFontFile);
			fseek(m_pFontFile, AdwOffsetPos, SEEK_SET);
			ulTmp = hy_cdr_int16_to(adwMappingOffset);
			fwrite(&ulTmp, 4, 1, m_pFontFile);
			fseek(m_pFontFile, tbEntry.offset + adwMappingOffset, SEEK_SET);
			EncodeDeltaSetIndexMap(m_pFontFile, m_HYHVAR.advanceWidthMapping);
		}

		//lsbMappingOffset
		if (m_HYHVAR.IslsbMapping) {
			//Offset in bytes from the start of this table to the delta-set index mapping for advance widths (may be NULL).		
			unsigned long lsbMappingOffset = tbEntry.offset - ftell(m_pFontFile);
			fseek(m_pFontFile, LsbOffsetPos, SEEK_SET);
			ulTmp = hy_cdr_int16_to(lsbMappingOffset);
			fwrite(&ulTmp, 4, 1, m_pFontFile);
			fseek(m_pFontFile, tbEntry.offset + lsbMappingOffset, SEEK_SET);
			EncodeDeltaSetIndexMap(m_pFontFile, m_HYHVAR.lsbMapping);
		}

		//rsbMappingOffset
		if (m_HYHVAR.IsrsbMapping) {
			//Offset in bytes from the start of this table to the delta-set index mapping for advance widths (may be NULL).		
			unsigned long RsbMappingOffset = tbEntry.offset - ftell(m_pFontFile);
			fseek(m_pFontFile, RsbOffsetPos, SEEK_SET);
			ulTmp = hy_cdr_int16_to(RsbMappingOffset);
			fwrite(&ulTmp, 4, 1, m_pFontFile);
			fseek(m_pFontFile, tbEntry.offset + RsbMappingOffset, SEEK_SET);
			EncodeDeltaSetIndexMap(m_pFontFile, m_HYHVAR.rsbMapping);
		}

		return HY_NOERROR;

	}	// end of int CHYFontCodec::EnCodeHVAR()

	void CHYFontCodec::EncodeDeltaSetIndexMap(FILE* pFlie,DeltaSetIndexMap& DtIndxMp)
	{
		unsigned short usTmp = 0;
		unsigned long ulTmp = 0;

		//entryFormat
		usTmp = hy_cdr_int16_to(DtIndxMp.entryFormat);
		fwrite(&usTmp, 2, 1, pFlie);

		//The number of mapping entries.
		usTmp = hy_cdr_int16_to(DtIndxMp.mapCount);
		fwrite(&usTmp, 2, 1, pFlie);

		// 目前这个函数暂时未完成
		DtIndxMp.MapIndex2MapData();
		//The dela - set index mapping data.See details below.
		for each (unsigned char dt in DtIndxMp.vtmapData){
			fwrite(&dt, 1, 1, pFlie);
		}

	}	// end of void CHYFontCodec::EncodeDeltaSetIndexMap()

	void CHYFontCodec::EnCodeItemVariationStore(FILE* pFl, CHYItemVariationStore& itemVariationStore)
	{
		unsigned short usTmp = 0;
		unsigned long ulTmp = 0;
		unsigned long ulStrBeginPos = ftell(pFl);

		//format
		usTmp = hy_cdr_int16_to(itemVariationStore.format);
		fwrite(&usTmp, 2, 1, pFl);

		unsigned long RegnLstOffstPos = ftell(pFl);
		//Offset in bytes from the start of the item variation store to the variation region list.
		fwrite(&ulTmp, 4, 1, pFl);

		//The number of item variation data subtables.
		usTmp = hy_cdr_int16_to(itemVariationStore.itemVariationDataCount);		
		fwrite(&usTmp, 2, 1, pFl);

		//Offsets in bytes from the start of the item variation store 
		//to each item variation data subtable.
		unsigned long itmVrtnDataOffsetsPos = ftell(pFl);
		for (int i = 0; i < itemVariationStore.itemVariationDataCount; i++)	{
			fwrite(&ulTmp, 4, 1, pFl);
		}

		unsigned long ulCurrntPos = ftell(pFl);
		fseek(pFl, RegnLstOffstPos,SEEK_SET);
		itemVariationStore.variationRegionListOffset = ulCurrntPos - ulStrBeginPos;
		ulTmp = hy_cdr_int32_to(itemVariationStore.variationRegionListOffset);
		fwrite(&ulTmp, 4, 1, pFl);
		fseek(pFl, ulCurrntPos, SEEK_SET);

		//VariationRegionList
		usTmp = hy_cdr_int16_to(itemVariationStore.variationRglst.axisCount);
		fwrite(&usTmp, 2, 1, pFl);

		usTmp = hy_cdr_int16_to(itemVariationStore.variationRglst.regionCount);
		fwrite(&usTmp, 2, 1, pFl);

		for (int i = 0; i < itemVariationStore.variationRglst.regionCount; i++)	{
			VariationRegion& vRgn = itemVariationStore.variationRglst.vtVariationRegion[i];
			for (int j = 0; j < vRgn.axisCount; j++){
				RegionAxisCoordinates& Coordinates = vRgn.vtRegionAxes[j];
				usTmp = hy_cdr_int16_to(Coordinates.startCoord);
				fwrite(&usTmp, 2, 1, pFl);
				usTmp = hy_cdr_int16_to(Coordinates.peakCoord);
				fwrite(&usTmp, 2, 1, pFl);
				usTmp = hy_cdr_int16_to(Coordinates.endCoord);
				fwrite(&usTmp, 2, 1, pFl);
			}
		}

		//ItemVariationData subtable:
		for (int i = 0; i < itemVariationStore.itemVariationDataCount; i++)
		{
			ulCurrntPos = ftell(pFl)- ulStrBeginPos;
			itemVariationStore.vtitemVariationDataOffsets.push_back(ulCurrntPos);

			CHYItemVariationData& itemVariationData =  itemVariationStore.vtItemVariationData[i];
			usTmp = hy_cdr_int16_to(itemVariationData.ItemCount);
			fwrite(&usTmp, 2, 1, pFl);
			usTmp = hy_cdr_int16_to(itemVariationData.ShortDeltaCount);
			fwrite(&usTmp, 2, 1, pFl);
			usTmp = hy_cdr_int16_to(itemVariationData.RegionIndexCount);
			fwrite(&usTmp, 2, 1, pFl);

			for (int j = 0; j < itemVariationData.RegionIndexCount; j++){
				usTmp = hy_cdr_int16_to(itemVariationData.vtRegionIndexes[j]);
				fwrite(&usTmp, 2, 1, pFl);
			}

			//deltaSets[itemCount]
			for (int j = 0; j < itemVariationData.ItemCount; j++) {
				CHYDeltaSet& DeltaSet = itemVariationData.vtdeltaSets[j];
				for (size_t x = 0; x < DeltaSet.vtDeltaData.size(); x++){
					if (x == 0)					{						
						usTmp = hy_cdr_int16_to(DeltaSet.vtDeltaData[0]);
						fwrite(&usTmp, 2, 1, pFl);
					}
					else {
						byte bytmp = DeltaSet.vtDeltaData[x];
						fwrite(&bytmp, 1, 1, pFl);
					}
				}
			}
		}

		ulCurrntPos = ftell(pFl);
		fseek(pFl, itmVrtnDataOffsetsPos, SEEK_SET);
		
		for (int i = 0; i < itemVariationStore.itemVariationDataCount; i++)	{
			ulTmp = hy_cdr_int32_to(itemVariationStore.vtitemVariationDataOffsets[i]);
			fwrite(&ulTmp, 4, 1, pFl);
		}
		fseek(pFl, ulCurrntPos, SEEK_SET);

	}	// end of void CHYFontCodec::EnCodeItemVariationStore()

	int	CHYFontCodec::DecodeVVAR()
	{
		m_HYVVAR.SetDefault();

		int iEntryIndex = m_HYTbDirectory.FindTableEntry(VVAR_TAG);
		if (iEntryIndex == -1) return FONT_ERR_VVAR_DECODE;

		CHYTableEntry& tbEntry = m_HYTbDirectory.vtTableEntry[iEntryIndex];
		fseek(m_pFontFile, tbEntry.offset, SEEK_SET);

		unsigned short usTmp = 0;
		unsigned long ulTmp = 0;
		fread(&usTmp, 2, 1, m_pFontFile);
		m_HYVVAR.majorVersion = hy_cdr_int16_to(usTmp);
		fread(&usTmp, 2, 1, m_pFontFile);
		m_HYVVAR.minorVersion = hy_cdr_int16_to(usTmp);

		fread(&ulTmp, 4, 1, m_pFontFile);
		m_HYVVAR.itemVariationStoreOffset = hy_cdr_int32_to(ulTmp);
		fread(&ulTmp, 4, 1, m_pFontFile);
		m_HYVVAR.advanceHeightMappingOffset= hy_cdr_int32_to(ulTmp);
		fread(&ulTmp, 4, 1, m_pFontFile);
		m_HYVVAR.tsbMappingOffset = hy_cdr_int32_to(ulTmp);
		fread(&ulTmp, 4, 1, m_pFontFile);
		m_HYVVAR.bsbMappingOffset = hy_cdr_int32_to(ulTmp);
		fread(&ulTmp, 4, 1, m_pFontFile);
		m_HYVVAR.vOrgMappingOffset = hy_cdr_int32_to(ulTmp);

		//itemVariationStore
		DecodeitemVariationStore(tbEntry.offset + m_HYVVAR.itemVariationStoreOffset, VVAR_TAG);

		if (m_HYVVAR.advanceHeightMappingOffset != NULL)
		{
			long Offst = tbEntry.offset + m_HYVVAR.advanceHeightMappingOffset;
			DecodeDeltaSetIndexMap(Offst, m_HYVVAR.advanceHeightMapping);
		}

		if (m_HYVVAR.tsbMappingOffset != NULL)
		{
			long Offst = tbEntry.offset + m_HYVVAR.tsbMappingOffset;
			DecodeDeltaSetIndexMap(Offst, m_HYVVAR.tsbMapping);
		}

		if (m_HYVVAR.bsbMappingOffset != NULL)
		{
			long Offst = tbEntry.offset + m_HYVVAR.bsbMappingOffset;
			DecodeDeltaSetIndexMap(Offst, m_HYVVAR.bsbMapping);
		}

		return HY_NOERROR;

	}	// end of int	CHYFontCodec::DecodeVVAR()

	void CHYFontCodec::DumpVVAR(CMarkup& MK)
	{
		char strTmp[MAX_PATH] = { 0 };

		MK.AddElem("VVAR");
		{
			MK.IntoElem();

			unsigned long Version = m_HYVVAR.majorVersion << 16;
			Version |= m_HYVVAR.minorVersion;
			MK.AddElem("Version");
			sprintf_s(strTmp, "0x%0*x", 8, Version);
			MK.AddAttrib("value", strTmp);

			MK.AddElem("VarStore");
			CHYItemVariationStore& VrtnStr = m_HYVVAR.itemVariationStore;
			memset(strTmp, 0, MAX_PATH);
			sprintf_s(strTmp, "%d", VrtnStr.format);
			MK.AddAttrib("Format", strTmp);
			{
				MK.IntoElem();
				MK.AddElem("Format");
				MK.AddAttrib("value", strTmp);
				MK.AddElem("VarRegionList");
				{
					MK.IntoElem();
					VariationRegionList&  vrtnrgnlst = VrtnStr.variationRglst;
					for (int i = 0; i < vrtnrgnlst.regionCount; i++)
					{
						VariationRegion&  vrgn = vrtnrgnlst.vtVariationRegion[i];
						MK.AddElem("Region");
						sprintf_s(strTmp, "%d", i);
						MK.AddAttrib("index", strTmp);
						{
							MK.IntoElem();
							for (int j = 0; j < vrgn.axisCount; j++)
							{
								RegionAxisCoordinates& rgnxcdnts = vrgn.vtRegionAxes[j];
								MK.AddElem("VarRegionAxis");
								sprintf_s(strTmp, "%d", j);
								MK.AddAttrib("index", strTmp);
								{
									MK.IntoElem();
									MK.AddElem("StartCoord");
									sprintf_s(strTmp, "%.1f", HY_F2DOT14_to_float(rgnxcdnts.startCoord));
									MK.AddAttrib("value", strTmp);

									MK.AddElem("PeakCoord");
									sprintf_s(strTmp, "%.1f", HY_F2DOT14_to_float(rgnxcdnts.peakCoord));
									MK.AddAttrib("value", strTmp);

									MK.AddElem("EndCoord");
									sprintf_s(strTmp, "%.1f", HY_F2DOT14_to_float(rgnxcdnts.endCoord));
									MK.AddAttrib("value", strTmp);
									MK.OutOfElem();
								}
							}
							MK.OutOfElem();
						}
					}
					MK.OutOfElem();
				}	// end of VarRegionList

				for (int i = 0; i<VrtnStr.itemVariationDataCount; i++)
				{
					CHYItemVariationData&  VData = VrtnStr.vtItemVariationData[i];
					MK.AddElem("VarData");
					{
						MK.IntoElem();
						{
							MK.AddElem("NumShorts");
							sprintf_s(strTmp, "%d", VData.ShortDeltaCount);
							MK.AddAttrib("value", strTmp);

							for (int j = 0; j < VData.RegionIndexCount; j++)
							{
								MK.AddElem("VarRegionIndex");
								sprintf_s(strTmp, "%d", j);
								MK.AddAttrib("index", strTmp);
								sprintf_s(strTmp, "%d", VData.vtRegionIndexes[j]);
								MK.AddAttrib("value", strTmp);
							}

							for (int j = 0; j < VData.ItemCount; j++)
							{
								CHYDeltaSet& DltSt = VData.vtdeltaSets[j];

								MK.AddElem("Item");
								sprintf_s(strTmp, "%d", j);
								MK.AddAttrib("index", strTmp);

								size_t st = DltSt.vtDeltaData.size();
								ostringstream ostrStrm;
								ostrStrm << "[";
								for (size_t x = 0; x < st; x++)
								{
									ostrStrm << DltSt.vtDeltaData[x];
									//strDltValue += ostrStrm.str();
									if (x != st - 1)
										ostrStrm << ",  ";
								}
								ostrStrm << "]";

								std::string strDltValue = ostrStrm.str();
								MK.AddAttrib("value", strDltValue);
							}
						}
						MK.OutOfElem();
					}	// end of VarData
				}
				MK.OutOfElem();
			}// end of VarStore

			if (m_HYVVAR.advanceHeightMappingOffset != 0)
			{
				MK.AddElem("AdvHeightMap");
				{
					MK.IntoElem();
					DeltaSetIndexMap& AdvHeightMap = m_HYVVAR.advanceHeightMapping;
					for (int i = 0; i < AdvHeightMap.mapCount; i++)
					{
						MK.AddElem("Map");
						sprintf_s(strTmp, "%d", i);
						MK.AddAttrib("index", strTmp);

						unsigned long Entry = 0;
						for (int j = 0; j < AdvHeightMap.oneEntrySize; j++)
						{
							int iStep = i * AdvHeightMap.oneEntrySize + j;
							int iData = AdvHeightMap.vtmapData[iStep];

							Entry |= iData << j;
						}
						int outerIndex = Entry >> ((AdvHeightMap.entryFormat & INNER_INDEX_BIT_COUNT_MASK) + 1);
						int innerIndex = Entry & ((1 << ((AdvHeightMap.entryFormat & INNER_INDEX_BIT_COUNT_MASK) + 1)) - 1);
						sprintf_s(strTmp, "%d", outerIndex);
						MK.AddAttrib("outer", strTmp);
						sprintf_s(strTmp, "%d", innerIndex);
						MK.AddAttrib("inner", strTmp);
					}
					MK.OutOfElem();
				}	//end of AdvWidthMap
			}

			if (m_HYVVAR.tsbMappingOffset != 0)
			{
				MK.AddElem("tsbMap");
				{
					MK.IntoElem();
					DeltaSetIndexMap& tsbMap = m_HYVVAR.tsbMapping;
					for (int i = 0; i < tsbMap.mapCount; i++)
					{
						MK.AddElem("Map");
						sprintf_s(strTmp, "%d", i);
						MK.AddAttrib("index", strTmp);

						unsigned long Entry = 0;
						for (int j = 0; j < tsbMap.oneEntrySize; j++)
						{
							int iStep = i * tsbMap.oneEntrySize + j;
							int iData = tsbMap.vtmapData[iStep];

							Entry |= iData << j;
						}
						int outerIndex = Entry >> ((tsbMap.entryFormat & INNER_INDEX_BIT_COUNT_MASK) + 1);
						int innerIndex = Entry & ((1 << ((tsbMap.entryFormat & INNER_INDEX_BIT_COUNT_MASK) + 1)) - 1);
						sprintf_s(strTmp, "%d", outerIndex);
						MK.AddAttrib("outer", strTmp);
						sprintf_s(strTmp, "%d", innerIndex);
						MK.AddAttrib("inner", strTmp);
					}
					MK.OutOfElem();
				}	//end of lsbMap
			}

			if (m_HYVVAR.bsbMappingOffset != 0)
			{
				MK.AddElem("bsbMap");
				{
					MK.IntoElem();
					DeltaSetIndexMap& bsbMap = m_HYVVAR.bsbMapping;
					for (int i = 0; i < bsbMap.mapCount; i++)
					{
						MK.AddElem("Map");
						sprintf_s(strTmp, "%d", i);
						MK.AddAttrib("index", strTmp);

						unsigned long Entry = 0;
						for (int j = 0; j < bsbMap.oneEntrySize; j++)
						{
							int iStep = i * bsbMap.oneEntrySize + j;
							int iData = bsbMap.vtmapData[iStep];

							Entry |= iData << j;
						}
						int outerIndex = Entry >> ((bsbMap.entryFormat & INNER_INDEX_BIT_COUNT_MASK) + 1);
						int innerIndex = Entry & ((1 << ((bsbMap.entryFormat & INNER_INDEX_BIT_COUNT_MASK) + 1)) - 1);
						sprintf_s(strTmp, "%d", outerIndex);
						MK.AddAttrib("outer", strTmp);
						sprintf_s(strTmp, "%d", innerIndex);
						MK.AddAttrib("inner", strTmp);
					}
					MK.OutOfElem();
				}	//end of rsbMap
			}

			if (m_HYVVAR.vOrgMappingOffset != 0)
			{
				MK.AddElem("vOrgMap");
				{
					MK.IntoElem();
					DeltaSetIndexMap& vOrgMap = m_HYVVAR.vOrgMapping;
					for (int i = 0; i < vOrgMap.mapCount; i++)
					{
						MK.AddElem("Map");
						sprintf_s(strTmp, "%d", i);
						MK.AddAttrib("index", strTmp);

						unsigned long Entry = 0;
						for (int j = 0; j < vOrgMap.oneEntrySize; j++)
						{
							int iStep = i * vOrgMap.oneEntrySize + j;
							int iData = vOrgMap.vtmapData[iStep];

							Entry |= iData << j;
						}
						int outerIndex = Entry >> ((vOrgMap.entryFormat & INNER_INDEX_BIT_COUNT_MASK) + 1);
						int innerIndex = Entry & ((1 << ((vOrgMap.entryFormat & INNER_INDEX_BIT_COUNT_MASK) + 1)) - 1);

						sprintf_s(strTmp, "%d", outerIndex);
						MK.AddAttrib("outer", strTmp);
						sprintf_s(strTmp, "%d", innerIndex);
						MK.AddAttrib("inner", strTmp);
					}
					MK.OutOfElem();
				}	//end of vOrgMap
			}

			MK.OutOfElem();
		}	// end of VVAR 

	}	// end of void CHYFontCodec::DumpVVAR()

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 通过Unicode编码获取Gryph Index
	int	CHYFontCodec::FindGryphIndexByUnciode(unsigned long ulUnicode)
	{		
		int iGlyphIndex = -1;
		unsigned long ulloop = 0;
		int iEntryIndex = m_HYCmap.FindSpecificEntryIndex(CMAP_ENCODE_FT_12);
		if (iEntryIndex!=-1)
		{
			CMAP_TABLE_ENTRY& TbEntry = m_HYCmap.vtCamp_tb_entry[iEntryIndex];

			ulloop = TbEntry.Format12.nGroups;
			for (unsigned long i=0; i<ulloop; i++)
			{
				CMAP_ENCODE_FORMAT_12_GROUP&			tagF12_GROUP = TbEntry.Format12.vtGroup[i];
				if (ulUnicode>=tagF12_GROUP.startCharCode && ulUnicode<= tagF12_GROUP.endCharCode)
				{
					iGlyphIndex = tagF12_GROUP.startGlyphID + (ulUnicode-tagF12_GROUP.startCharCode);
				}
			}
		}
		else 
		{
			iEntryIndex = m_HYCmap.FindSpecificEntryIndex(CMAP_ENCODE_FT_4);
			if (iEntryIndex!=-1)
			{
				CMAP_TABLE_ENTRY& TbEntry =  m_HYCmap.vtCamp_tb_entry[iEntryIndex];
				CMAP_ENCODE_FORMAT_4& Format4 = TbEntry.Format4;

				ulloop = Format4.segCountX2>>1;
				for (unsigned short i=0; i<ulloop; i++)
				{
					// 确定这个unicode是处于哪个segment中。
					if (ulUnicode>=Format4.vtstartCount[i] && ulUnicode<=Format4.vtEndCount[i])
					{
						const int &iRang		= ulUnicode-Format4.vtstartCount[i];
						if (Format4.vtidRangeOffset[i] != 0)
						{
							const int &iID = (Format4.vtidRangeOffset[i]-Format4.segCountX2)/2+i+iRang;
							iGlyphIndex = Format4.vtglyphIdArray[iID];
						}
						else 
						{
							iGlyphIndex = (Format4.vtstartCount[i]+iRang+(Format4.vtidDelta[i]))%65536;
						}
					}
				}
			}
		}

		return iGlyphIndex;

	}	// end of int	CHYFontCodec::FindGryphIndexByUnciode()

	int	CHYFontCodec::FindGryphIndexByUnciodeEx(unsigned long ulUnicode)
	{
		for (size_t i=0; i<m_vtHYGlyphs.size();i++)	{
			CHYGlyph& glyph = m_vtHYGlyphs[i];
			for (size_t j=0; j<glyph.vtUnicode.size(); j++)	{
				if (ulUnicode == glyph.vtUnicode[j])
					return i;
			}
		}
		return -1;

	}	// end of int CHYFontCodec::FindGryphIndexByUnciodeEx()

	int	CHYFontCodec::FindGryphIndexByGlyName(string strGName)
	{
		for (size_t i = 0; i < m_vtHYGlyphs.size(); i++) {
			CHYGlyph& glyph = m_vtHYGlyphs[i];			
			if (strGName == glyph.strPostName) {
				return i;
			}
		}
		return -1;

	}	// end of int CHYFontCodec::FindGryphIndexByGlyName()

	// 通过Gryph Index 获取 Unicode,一个GryphIndex 可能会对应多个Uncoide 编码	
	void CHYFontCodec::FindGryphUncidoByIndex(unsigned long ulGlyphIndex, std::vector<unsigned long>& szUnicode)
	{
		szUnicode.clear();

		std::vector<unsigned long> szTmpUnicode;
		unsigned long ulloop = 0;
		int iEntryIndex = m_HYCmap.FindSpecificEntryIndex(CMAP_ENCODE_FT_12);
		if (iEntryIndex!=-1)
		{
			CMAP_TABLE_ENTRY& TbEntry = m_HYCmap.vtCamp_tb_entry[iEntryIndex];
			CMAP_ENCODE_FORMAT_12& Format12 = TbEntry.Format12;
			ulloop = Format12.nGroups;
			for (unsigned long i=0; i<ulloop; i++)
			{
				CMAP_ENCODE_FORMAT_12_GROUP&  tagF12Group = Format12.vtGroup[i];
				if (ulGlyphIndex >= tagF12Group.startGlyphID)
				{
					if (tagF12Group.startCharCode+(ulGlyphIndex-tagF12Group.startGlyphID) <= tagF12Group.endCharCode)
					{
						//szTmpUnicode.push_back(tagF12Group.startCharCode+(ulGlyphIndex-tagF12Group.startGlyphID));
						szUnicode.push_back(tagF12Group.startCharCode+(ulGlyphIndex-tagF12Group.startGlyphID));
					}
				}			
			}
		}
		else 
		{
			iEntryIndex = m_HYCmap.FindSpecificEntryIndex(CMAP_ENCODE_FT_4);
			if (iEntryIndex!=-1)
			{
				CMAP_TABLE_ENTRY& TbEntry = m_HYCmap.vtCamp_tb_entry[iEntryIndex];
				CMAP_ENCODE_FORMAT_4& Format4 = TbEntry.Format4;
				ulloop = Format4.segCountX2>>1;

				for (unsigned short i=0; i<ulloop;i++)
				{
					int iRang		= Format4.vtEndCount[i]-Format4.vtstartCount[i];
					if (Format4.vtidRangeOffset[i]%65535 !=0)
					{
						for (int x=0; x<=iRang; x++)
						{
							int iID = (Format4.vtidRangeOffset[i] - Format4.segCountX2)/2+i+x;
							if(ulGlyphIndex == Format4.vtglyphIdArray[iID])
							{
								//szTmpUnicode.push_back(Format4.vtstartCount[i]+x);
								szUnicode.push_back(Format4.vtstartCount[i]+x);
							}
						}
					}
					else 
					{	
						for (int x=0; x<=iRang; x++)
						{
							if (ulGlyphIndex == (Format4.vtstartCount[i]+x+(Format4.vtidDelta[i]))%65536)
							{
								//szTmpUnicode.push_back(Format4.vtstartCount[i]+x);
								szUnicode.push_back(Format4.vtstartCount[i]+x);
							}
						}
					}
				}
			}
		}

		/*
		BOOL bRepeat = FALSE;
		for (size_t i=0; i<szTmpUnicode.size(); i++)
		{
			bRepeat = FALSE;
			for (size_t j=0; j<szUnicode.size(); j++)
			{
				if (szTmpUnicode[i]== szUnicode[j]) 
					bRepeat = TRUE;
			}

			if (!bRepeat)
			{
				szUnicode.push_back(szTmpUnicode[i]);			
			}
		}
		*/

	}	// end of int CHYFontCodec::FindGryphUncidoByIndex()

	int CHYFontCodec::FindHeadXminGID()
	{
		if (m_vtHYGlyphs.size() > 0)
		{
			int iGID = 0;
			int iXmin = m_vtHYGlyphs[0].minX;		
			for(size_t i=1; i<m_vtHYGlyphs.size(); i++)
			{
				if (m_vtHYGlyphs[i].minX<iXmin)
				{
					iXmin = m_vtHYGlyphs[i].minX;
					iGID = i;
				}
			}

			return iGID;
		}
	
		return -1;

	}	// end of int CHYFontCodec::FindHeadXminGID()

	int	CHYFontCodec::FindHeadXmaxGID()
	{
		if (m_vtHYGlyphs.size() > 0)
		{
			int iGID = 0;
			int iXmax = m_vtHYGlyphs[0].maxX;		
			for(size_t i=1; i<m_vtHYGlyphs.size(); i++)
			{
				if (m_vtHYGlyphs[i].maxX>iXmax)
				{
					iXmax = m_vtHYGlyphs[i].maxX;
					iGID = i;
				}
			}

			return iGID;
		}

	
		return -1;

	}	// end of int CHYFontCodec::FindHeadXmaxGID()

	int	CHYFontCodec::FindHeadYminGID()
	{
		if (m_vtHYGlyphs.size() > 0)
		{
			int iGID = 0;
			int iYmin = m_vtHYGlyphs[0].minY;		
			for(size_t i=1; i<m_vtHYGlyphs.size(); i++)
			{
				if (m_vtHYGlyphs[i].minY<iYmin)
				{
					iYmin = m_vtHYGlyphs[i].minY;
					iGID = i;
				}
			}

			return iGID;
		}

		return -1;

	}	// end of int CHYFontCodec::FindHeadYminGID()

	int	CHYFontCodec::FindHeadYmaxGID()
	{
		if (m_vtHYGlyphs.size() > 0)
		{
			int iGID = 0;
			int iYmax = m_vtHYGlyphs[0].maxY;		
			for(size_t i=1; i<m_vtHYGlyphs.size(); i++)
			{
				if (m_vtHYGlyphs[i].maxY>iYmax)
				{
					iYmax = m_vtHYGlyphs[i].maxY;
					iGID = i;
				}
			}

			return iGID;
		}
	
		return -1;

	}	// end of int CHYFontCodec::FindHeadYmaxGID()

	int	CHYFontCodec::FindVertMinTop()
	{	
		int minTop = 0;
		size_t stVerMetricsNum = m_HYVhea.numOfLongVerMetrics;		
		if (stVerMetricsNum>0)
		{
			minTop = m_HYVmtx.vtLongVrtmetric[0].tsb;
			for (size_t i=1; i<stVerMetricsNum; i++)
			{					
				if (m_HYVmtx.vtLongVrtmetric[i].tsb<minTop)  
				{
					minTop = m_HYVmtx.vtLongVrtmetric[i].tsb;	
				}	
			}

			size_t stTopsidebearNum = m_HYVmtx.vtTopsidebearing.size();		
			for (size_t i=0; i<stTopsidebearNum; i++)
			{	
				if (m_HYVmtx.vtTopsidebearing[i]<minTop)  
				{
					minTop = m_HYVmtx.vtTopsidebearing[i];					
				}	
			}
		}

		return minTop;

	}	// end of int	CHYFontCodec::FindVertMinTop()

	int	CHYFontCodec::FindVertMinBottom()
	{
		short minBottom = 0;
		size_t stGlyphNum = m_vtHYGlyphs.size();
		if (stGlyphNum>0)
		{			
			//minBottom = m_vtHYGlyphs[0].minY-m_HYhead.yMin;
			minBottom = m_vtHYGlyphs[0].minY-m_HYOS2.sTypoDescender;
			for (size_t i=0; i<stGlyphNum; i++)
			{	
				CHYGlyph& Gryph = m_vtHYGlyphs[i];				
				//if (Gryph.minY-m_HYhead.yMin<minBottom)  
				if (Gryph.minY-m_HYOS2.sTypoDescender<minBottom)  
				{	
					//minBottom = Gryph.minY-m_HYhead.yMin;
					minBottom = Gryph.minY-m_HYOS2.sTypoDescender;
				}
			}
		}

		return minBottom;

	}	// end of int	CHYFontCodec::FindVertMinBottom()


	int	CHYFontCodec::FindVertTopSideBearGID()
	{
		int iGID = -1;
		size_t stVerMetricsNum = m_HYVhea.numOfLongVerMetrics;		
		if (stVerMetricsNum>0)
		{
			short minTop = m_HYVmtx.vtLongVrtmetric[0].tsb;
			for (size_t i=1; i<stVerMetricsNum; i++)
			{					
				if (m_HYVmtx.vtLongVrtmetric[i].tsb<minTop)  
				{
					minTop = m_HYVmtx.vtLongVrtmetric[i].tsb;
					iGID = i;
				}	
			}

			size_t stTopsidebearNum = m_HYVmtx.vtTopsidebearing.size();		
			for (size_t i=0; i<stTopsidebearNum; i++)
			{					
				if (m_HYVmtx.vtTopsidebearing[i]<minTop)  
				{
					minTop = m_HYVmtx.vtTopsidebearing[i];
					iGID = i;
				}	
			}
		}

		return iGID;

	}	// end of int	CHYFontCodec::FindVertTopSideBearGID()

	int	CHYFontCodec::FindVertBottomSideBearGID()
	{
		int iGID = -1;
		size_t stGlyphNum = m_vtHYGlyphs.size();
		if (stGlyphNum>0)
		{			
			short minBottom = m_vtHYGlyphs[0].minY-m_HYHhea.Descender;
			for (size_t i=0; i<stGlyphNum; i++)
			{			
				CHYGlyph& Gryph = m_vtHYGlyphs[i];				
				if (Gryph.minY-m_HYHhea.Descender<minBottom)  
				{					
					minBottom = Gryph.minY - m_HYHhea.Descender;			
					iGID = i;
				}
			}
		}

		return iGID;

	}	// end of int CHYFontCodec::FindVertBottomSideBearGID()

	int CHYFontCodec::FindMinRightSideBearing()
	{		
		size_t stGlyphNum = m_vtHYGlyphs.size();
		if (stGlyphNum == 0) return -1;					
		short minRSB = m_vtHYGlyphs[0].advanceWidth - m_vtHYGlyphs[0].minX-(m_vtHYGlyphs[0].maxX-m_vtHYGlyphs[0].minX);
		for (size_t i=0; i<stGlyphNum; i++)
		{			
			CHYGlyph& Gryph = m_vtHYGlyphs[i];
			short tmp = Gryph.advanceWidth - Gryph.minX-(Gryph.maxX-Gryph.minX);
			if (tmp<minRSB)  
			{					
				minRSB = tmp;								
			}
		}
		
		return minRSB;

	}	// end of int CHYFontCodec::FindMinRightSideBearing()

	unsigned short 	 CHYFontCodec::GetAdvancMaxWidth()
	{
		size_t sz = m_vtHYGlyphs.size();
		unsigned short iAVDW=-1;
		if (sz>0)
		{
			iAVDW = m_vtHYGlyphs[0].advanceWidth;
			for (size_t i=1; i<sz; i++)
			{
				if (m_vtHYGlyphs[i].advanceWidth>iAVDW)
				{
					iAVDW = m_vtHYGlyphs[i].advanceWidth;
				}				
			}
		}

		return iAVDW;

	}	// end of int CHYFontCodec::GetAdvancMaxWidth();

	unsigned short	CHYFontCodec::GetAdvancMaxHeight()
	{
		size_t sz = m_vtHYGlyphs.size();
		unsigned short iAVHT=-1;
		if (sz>0)
		{
			iAVHT = m_vtHYGlyphs[0].advanceHeight;
			for (size_t i=1; i<sz; i++)
			{
				if (m_vtHYGlyphs[i].advanceHeight>iAVHT)
				{
					iAVHT = m_vtHYGlyphs[i].advanceHeight;
				}				
			}
		}

		return iAVHT;

	}	// end of unsigned short CHYFontCodec::GetAdvancMaxHeight()

	int CHYFontCodec::GetGlyfAdvancWidth(int iGlyphID)
	{
		int iLonghormetricSize = m_HYHmtx.vtLonghormetric.size();
		if (iLonghormetricSize== 0) return 0;

		if (iGlyphID<iLonghormetricSize)
		{
			return  m_HYHmtx.vtLonghormetric[iGlyphID].advanceWidth;
		}
		else 
		{
			return  m_HYHmtx.vtLonghormetric[iLonghormetricSize-1].advanceWidth;
		}

	}	// end of void CHYFontCodec::GetGlyfAdvancWidth()	

	int CHYFontCodec::GetGlyfAdvancHeight(int iGlyphID)
	{
		int iLonghormetricSize = m_HYVmtx.vtLongVrtmetric.size();
		if (iLonghormetricSize== 0) return 0;

		if (iGlyphID<iLonghormetricSize){
			return  m_HYVmtx.vtLongVrtmetric[iGlyphID].advanceHeight;
		}
		else {
			return  m_HYVmtx.vtLongVrtmetric[iLonghormetricSize-1].advanceHeight;
		}

		return -1;

	}	// end of void CHYFontCodec::GetGlyfAdvancWidth()

	void CHYFontCodec::GetGlyfVrtmetric(int iGlyphID, VMTX_LONGVRTMETRIC& Vrtmetric)
	{
		int iLonghormetricSize = m_HYVmtx.vtLongVrtmetric.size();
		if (iLonghormetricSize == 0) return;

		if (iGlyphID < iLonghormetricSize) {
			Vrtmetric.advanceHeight = m_HYVmtx.vtLongVrtmetric[iGlyphID].advanceHeight;
			Vrtmetric.tsb = m_HYVmtx.vtLongVrtmetric[iGlyphID].tsb;
		}
		else {
			Vrtmetric.advanceHeight = m_HYVmtx.vtLongVrtmetric[iLonghormetricSize - 1].advanceHeight;
			Vrtmetric.tsb = m_HYVmtx.vtTopsidebearing[iGlyphID- iLonghormetricSize];
		}

	}	// end of int CHYFontCodec::GetGlyfVrtmetric()
	
	void CHYFontCodec::CountFontBound()
	{
		if (m_vtHYGlyphs.size() > 0)
		{
			m_HYhead.xMin = m_vtHYGlyphs[0].minX != 0 ? m_vtHYGlyphs[0].minX:32767;
			m_HYhead.yMin = m_vtHYGlyphs[0].minY != 0 ? m_vtHYGlyphs[0].minY:32767;
			m_HYhead.xMax = m_vtHYGlyphs[0].maxX != 0 ? m_vtHYGlyphs[0].maxX:-32768;
			m_HYhead.yMax = m_vtHYGlyphs[0].maxY != 0 ? m_vtHYGlyphs[0].maxY:-32768;

			for(size_t i=1; i<m_vtHYGlyphs.size(); i++)
			{
				if (m_vtHYGlyphs[i].vtContour.size()==0)
					continue;

				if (m_vtHYGlyphs[i].minX<m_HYhead.xMin)
					m_HYhead.xMin = m_vtHYGlyphs[i].minX;

				if (m_vtHYGlyphs[i].minY<m_HYhead.yMin)
					m_HYhead.yMin = m_vtHYGlyphs[i].minY;

				if (m_vtHYGlyphs[i].maxX>m_HYhead.xMax)
					m_HYhead.xMax = m_vtHYGlyphs[i].maxX;

				if (m_vtHYGlyphs[i].maxY>m_HYhead.yMax)
					m_HYhead.yMax = m_vtHYGlyphs[i].maxY;
			}
		}

	}	// end of int CHYFontCodec::CountFontBound()	

	bool CHYFontCodec::MulToWCharBL(char* pDest, int& iDestlen,const char* pSrc, int iSrclen)
	{
		if ( pDest == NULL ) return false;
		if ( pSrc == NULL )	return false;

		WCHAR	wpString[260]	= {0};
		int		iWStringLen			= 0;	

		iWStringLen = MultiByteToWideChar(CP_ACP,0, pSrc, -1, NULL, 0 );
		if (iWStringLen == 0 )					return false;		
		if (iWStringLen >= MAX_PATH / 2 )		return false ;	

		MultiByteToWideChar(CP_ACP, 0, pSrc, strlen(pSrc), wpString, iWStringLen);

		iWStringLen = wcslen(wpString);

		iDestlen = 2*iWStringLen;

		// 大端模式
		short sTmp = 0;
		for ( int x = 0; x < iWStringLen ; x++ )
		{			
			sTmp = hy_cdr_int16_to(wpString[x]);			
			memcpy(pDest+x*2, &sTmp,2);
		}
		return true;
	}	// end of void MulToWCharBL()

	bool CHYFontCodec::MulToWChar(char* pDest, int& iDestlen,const char* pSrc, int iSrclen)
	{
		if ( pDest == NULL ) return false;
		if ( pSrc == NULL )	return false;

		WCHAR	wpString[260]	= {0};
		int		iWStringLen			= 0;	

		iWStringLen = MultiByteToWideChar(CP_ACP,0, pSrc, -1, NULL, 0 );
		if (iWStringLen == 0 )					return false;		
		if (iWStringLen >= MAX_PATH / 2 )		return false ;	

		MultiByteToWideChar(CP_ACP, 0, pSrc, strlen(pSrc), wpString, iWStringLen);

		iWStringLen = wcslen(wpString);

		iDestlen = 2*iWStringLen;		
		for ( int x = 0; x < iWStringLen ; x++ )
		{						
			memcpy(pDest+x*2, &wpString[x],2);
		}
		return true;
	}	// end of void MToWChar()	

	int		CHYFontCodec::FindGlyph(unsigned long ulUni, std::vector<CHYGlyph>& vtHYGlyphs)
	{
		size_t szGlyphNums = vtHYGlyphs.size();
		for (size_t i=0; i<szGlyphNums; i++){
			CHYGlyph& glyph = vtHYGlyphs[i];
			size_t szUniNums = glyph.vtUnicode.size();
			for (size_t j=0; j<szUniNums; j++){
				unsigned long ulTmp = glyph.vtUnicode[j];
				if (ulTmp == ulUni){
					return i;
				}		
			}
		}

		return -1;

	}	// end of int CHYFontCodec::FindGlyph()

	int CHYFontCodec::FindLastCharIndex(std::vector<CHYGlyph>& vtHYGlyphs)
	{
		unsigned long ulMaxUni = 0x00;
		int iIndx = 0;
		size_t szGlyphNums = vtHYGlyphs.size();
		for (size_t i = 1; i < szGlyphNums; i++) {
			CHYGlyph& glyph = vtHYGlyphs[i];
			size_t szUniNums = glyph.vtUnicode.size();
			for (size_t j = 0; j < szUniNums; j++) {
				unsigned long ulTmp = glyph.vtUnicode[j];
				if (ulTmp > ulMaxUni) {
					ulMaxUni = ulTmp;
					iIndx = i;
				}
			}
		}
		if (ulMaxUni > 0xFFFF) return 0xffff;
		return  ulMaxUni;

	}	// end of int CHYFontCodec::FindLastCharIndex()

}

