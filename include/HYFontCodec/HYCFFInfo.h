#pragma once

#include <vector>
#include <string>

namespace HYFONTCODEC
{
	class HY_DLL_CLASS CHYCFFStandString
	{
	public:
		CHYCFFStandString();
		~CHYCFFStandString();

		void SetDefault();

	public:	

		int							FindNumberByString(std::string str);
		//std::string					FindNameByGID(unsigned long ulID);
		//unsigned short				FindNameIDByGID(unsigned long ulID);
		// 返回NameID 如果存在同名Name返回同名Name的ID，否则返回新加的ID
		int							InsertString(std::string strString);
		std::vector<std::string>	m_szStandString;

	};

	class HY_DLL_CLASS CHYCFFIndex
	{
	public:
		CHYCFFIndex();
		~CHYCFFIndex();

		void Setdefault();

	public:
		unsigned short 				Count;
		unsigned char				OffSize;
		std::vector<unsigned long>	vtOffset;
		std::vector<unsigned char>	vtData;
	};

	class HY_DLL_CLASS	CHYCFFROS
	{
	public:
		CHYCFFROS();
		~CHYCFFROS();
	public:
		std::string		strRegistry;	
		int				RegistrySID;
		std::string		strOrdering;	
		int				OrderingSID;
		long			Supplement;
	};

	class HY_DLL_CLASS	CHYCFFPrivteDict
	{
	public:
		CHYCFFPrivteDict();
		~CHYCFFPrivteDict();
		void SetDefault();

	public:
		std::string				strFontName;	
		int						iFontNameID;
		std::vector<double>		vtBlueValues;
		std::vector<double>		vtOtherBlues;
		std::vector<double>		vtFamilyBlues;
		std::vector<double>		vtFamilyOtherBlues;		
		double					fBlueScale;
		double 					fBlueShift;
		double					fBlueFuzz;
		double					fStdHW;
		double					fStdVW;

		std::vector<double>		vtStemSnapH;	
		std::vector<double>		vtStemSnapV;
		long 					lForceBold;
		long 					lLanguageGroup;
		double					fExpansionFactor;
		double					finitialRandomSeed;
		long 					lSubrsOffset;
		long 					ldefaultWidthX;
		long 					lnominalWidthX;
		CHYCFFIndex				SubIndex;	
	};

	class HY_DLL_CLASS CCFFFDSRang3
	{
	public:
		CCFFFDSRang3();
		~CCFFFDSRang3();

	public:
		short				first;
		unsigned char		fdIndex;
	};

	class HY_DLL_CLASS CCFFFDSFormat3
	{
	public:
		CCFFFDSFormat3();
		~CCFFFDSFormat3();

		void Setdefault();

	public:
		std::vector<CCFFFDSRang3>	vtRang3;
		unsigned short				sentinel;
	};

	class HY_DLL_CLASS CHYCFFFDSelect
	{
	public:
		CHYCFFFDSelect();
		~CHYCFFFDSelect();	

		void Setdefault();

	public:
		int							iFormat;
		std::vector<unsigned char>	format0;
		CCFFFDSFormat3				format3;
	};	

	class HY_DLL_CLASS CCFFCSFormatRang
	{
	public:
		CCFFCSFormatRang();
		~CCFFCSFormatRang();

		void SetDefault();

	public:
		unsigned short  first;
		unsigned short	left;
	};

	class HY_DLL_CLASS CCFFCSFormat
	{
	public:
		CCFFCSFormat();
		~CCFFCSFormat();

		void Setdefault();

	public:
		std::vector<CCFFCSFormatRang>	vtRang;
	};	

	class HY_DLL_CLASS CHYCFFCharset
	{
	public:
		CHYCFFCharset();
		~CHYCFFCharset();
		void Setdefault();

		std::vector<unsigned short> vtFoamat0SID;	
		CCFFCSFormat		format1and2;

	public:	
		unsigned char  format;
	};


	class HY_DLL_CLASS CHYCFFTopDict
	{
	public:
		CHYCFFTopDict(void);
		~CHYCFFTopDict(void);

		void	SetDefault();

	public:
		std::string						strVersion;
		int								iVersionSID;
		std::string						strNotice;
		int								iNoticeSID;
		std::string						strCopyright;
		int								iCopyrghtSID;
		std::string 					strFullName;	
		int								iFullNameSID;
		std::string						strFamilyName;	
		int								iFamilyNameSID;
		std::string 					strWeight;	
		int								iWeightSID;
		long							isFixedPitch;
		double							ItalicAngle;
		double							UnderlinePosition;
		double							UnderlineThickness;
		long							PaintType;
		long							CharStringType;
		std::vector<double> 			vtFontMatrix;
		long							UniqueID;	
		std::vector<int>				vtFontBOX;
		double							strokeWidth;
		std::vector<int>				vtXUID;
		long							charsetOffset;
		long							encodingOffset;
		long							charStringOffset;
		long							PrivteDictSize;
		long							PrivteOffset;
		long							SyntheticBaseIndex;
		std::string						strPostSript;	
		int								iPostSriptSID;
		std::string						strBaseFontName;	
		int								iBaseFontNameSID;
		std::vector<double>				vtBaseFontBlend;

		int								IsCIDFont;
		// cid font 特有
		CHYCFFROS						Ros;
		double							CIDFontVersion;
		double							CIDFontRevision;
		double							CIDFontType;
		long 							CIDCount;
		long							UIDBase;
		long							FDArryIndexOffset;
		long							FDSelectOffset;

		std::string						strCIDFontName;
		int								CIDFontNameSID;	
	};

	class HY_DLL_CLASS CHYCFFHeader
	{
	public:
		CHYCFFHeader();
		~CHYCFFHeader();

	public:
		unsigned char major;
		unsigned char minor;
		unsigned char hdrSize;
		unsigned char offSize;
	};

	class HY_DLL_CLASS CHYCFFInfo
	{
	public:
		CHYCFFInfo(void);
		~CHYCFFInfo(void);

		void				SetDefault();	
		unsigned short 		FindSIDbyGlyphID(unsigned usGID);
		std::string			FindStringbyGlyphID(unsigned usGID);

	public:
		CHYCFFHeader					Header;	
		CHYCFFTopDict					TopDICT;
		std::vector<CHYCFFPrivteDict>	vtFDArry;
		std::vector<std::string>		vtFontName;
		CHYCFFStandString				stnStrings;
		CHYCFFIndex						globalSubIndex;	
		CHYCFFPrivteDict				PrivteDict;
		CHYCFFFDSelect					FDSelect;
		CHYCFFCharset					Charset;
	};
};
