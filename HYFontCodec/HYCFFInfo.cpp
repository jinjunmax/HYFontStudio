#include "StdAfx.h"
#include "HYCFFInfo.h"

namespace HYFONTCODEC
{
	CHYCFFStandString::CHYCFFStandString()
	{
		SetDefault();

	}	// end of CHYCFFStandString::CHYCFFStandString()

	CHYCFFStandString::~CHYCFFStandString()
	{

	}	// end of CHYCFFStandString::~CHYCFFStandString()

	void CHYCFFStandString::SetDefault()
	{
		m_szStandString.clear();
		m_szStandString.push_back(".notdef");
		m_szStandString.push_back("space");
		m_szStandString.push_back("exclam");
		m_szStandString.push_back("quotedbl");
		m_szStandString.push_back("numbersign");
		m_szStandString.push_back("dollar");
		m_szStandString.push_back("percent");
		m_szStandString.push_back("ampersand");
		m_szStandString.push_back("quoteright");
		m_szStandString.push_back("parenleft");
		m_szStandString.push_back("parenright");
		m_szStandString.push_back("asterisk");
		m_szStandString.push_back("plus");
		m_szStandString.push_back("comma");
		m_szStandString.push_back("hyphen");
		m_szStandString.push_back("period");
		m_szStandString.push_back("slash");
		m_szStandString.push_back("zero");
		m_szStandString.push_back("one");
		m_szStandString.push_back("two");
		m_szStandString.push_back("three");
		m_szStandString.push_back("four");
		m_szStandString.push_back("five");
		m_szStandString.push_back("six");
		m_szStandString.push_back("seven");
		m_szStandString.push_back("eight");
		m_szStandString.push_back("nine");
		m_szStandString.push_back("colon");
		m_szStandString.push_back("semicolon");
		m_szStandString.push_back("less");
		m_szStandString.push_back("equal");
		m_szStandString.push_back("greater");
		m_szStandString.push_back("question");
		m_szStandString.push_back("at");
		m_szStandString.push_back("A");
		m_szStandString.push_back("B");
		m_szStandString.push_back("C");
		m_szStandString.push_back("D");
		m_szStandString.push_back("E");
		m_szStandString.push_back("F");
		m_szStandString.push_back("G");
		m_szStandString.push_back("H");
		m_szStandString.push_back("I");
		m_szStandString.push_back("J");
		m_szStandString.push_back("K");
		m_szStandString.push_back("L");
		m_szStandString.push_back("M");
		m_szStandString.push_back("N");
		m_szStandString.push_back("O");
		m_szStandString.push_back("P");
		m_szStandString.push_back("Q");
		m_szStandString.push_back("R");
		m_szStandString.push_back("S");
		m_szStandString.push_back("T");
		m_szStandString.push_back("U");
		m_szStandString.push_back("V");
		m_szStandString.push_back("W");
		m_szStandString.push_back("X");
		m_szStandString.push_back("Y");
		m_szStandString.push_back("Z");
		m_szStandString.push_back("bracketleft");
		m_szStandString.push_back("backslash");
		m_szStandString.push_back("bracketright");
		m_szStandString.push_back("asciicircum");
		m_szStandString.push_back("underscore");
		m_szStandString.push_back("quoteleft");
		m_szStandString.push_back("a");
		m_szStandString.push_back("b");
		m_szStandString.push_back("c");
		m_szStandString.push_back("d");
		m_szStandString.push_back("e");
		m_szStandString.push_back("f");
		m_szStandString.push_back("g");
		m_szStandString.push_back("h");
		m_szStandString.push_back("i");
		m_szStandString.push_back("j");
		m_szStandString.push_back("k");
		m_szStandString.push_back("l");
		m_szStandString.push_back("m");
		m_szStandString.push_back("n");
		m_szStandString.push_back("o");
		m_szStandString.push_back("p");
		m_szStandString.push_back("q");
		m_szStandString.push_back("r");
		m_szStandString.push_back("s");
		m_szStandString.push_back("t");
		m_szStandString.push_back("u");
		m_szStandString.push_back("v");
		m_szStandString.push_back("w");
		m_szStandString.push_back("x");
		m_szStandString.push_back("y");
		m_szStandString.push_back("z");
		m_szStandString.push_back("braceleft");
		m_szStandString.push_back("bar");
		m_szStandString.push_back("braceright");
		m_szStandString.push_back("asciitilde");
		m_szStandString.push_back("exclamdown");
		m_szStandString.push_back("cent");
		m_szStandString.push_back("sterling");
		m_szStandString.push_back("fraction");
		m_szStandString.push_back("yen");
		m_szStandString.push_back("florin");
		m_szStandString.push_back("section");
		m_szStandString.push_back("currency");
		m_szStandString.push_back("quotesingle");
		m_szStandString.push_back("quotedblleft");
		m_szStandString.push_back("guillemotleft");
		m_szStandString.push_back("guilsinglleft");
		m_szStandString.push_back("guilsinglright");
		m_szStandString.push_back("fi");
		m_szStandString.push_back("fl");
		m_szStandString.push_back("endash");
		m_szStandString.push_back("dagger");
		m_szStandString.push_back("daggerdbl");
		m_szStandString.push_back("periodcentered");
		m_szStandString.push_back("paragraph");
		m_szStandString.push_back("bullet");
		m_szStandString.push_back("quotesinglbase");
		m_szStandString.push_back("quotedblbase");
		m_szStandString.push_back("quotedblright");
		m_szStandString.push_back("guillemotright");
		m_szStandString.push_back("ellipsis");
		m_szStandString.push_back("perthousand");
		m_szStandString.push_back("questiondown");
		m_szStandString.push_back("grave");
		m_szStandString.push_back("acute");
		m_szStandString.push_back("circumflex");
		m_szStandString.push_back("tilde");
		m_szStandString.push_back("macron");
		m_szStandString.push_back("breve");
		m_szStandString.push_back("dotaccent");
		m_szStandString.push_back("dieresis");
		m_szStandString.push_back("ring");
		m_szStandString.push_back("cedilla");
		m_szStandString.push_back("hungarumlaut");
		m_szStandString.push_back("ogonek");
		m_szStandString.push_back("caron");
		m_szStandString.push_back("emdash");
		m_szStandString.push_back("AE");
		m_szStandString.push_back("ordfeminine");
		m_szStandString.push_back("Lslash");
		m_szStandString.push_back("Oslash");
		m_szStandString.push_back("OE");
		m_szStandString.push_back("ordmasculine");
		m_szStandString.push_back("ae");
		m_szStandString.push_back("dotlessi");
		m_szStandString.push_back("lslash");
		m_szStandString.push_back("oslash");
		m_szStandString.push_back("oe");
		m_szStandString.push_back("germandbls");
		m_szStandString.push_back("onesuperior");
		m_szStandString.push_back("logicalnot");
		m_szStandString.push_back("mu");
		m_szStandString.push_back("trademark");
		m_szStandString.push_back("Eth");
		m_szStandString.push_back("onehalf");
		m_szStandString.push_back("plusminus");
		m_szStandString.push_back("Thorn");
		m_szStandString.push_back("onequarter");
		m_szStandString.push_back("divide");
		m_szStandString.push_back("brokenbar");
		m_szStandString.push_back("degree");
		m_szStandString.push_back("thorn");
		m_szStandString.push_back("threequarters");
		m_szStandString.push_back("twosuperior");
		m_szStandString.push_back("registered");
		m_szStandString.push_back("minus");
		m_szStandString.push_back("eth");
		m_szStandString.push_back("multiply");
		m_szStandString.push_back("threesuperior");
		m_szStandString.push_back("copyright");
		m_szStandString.push_back("Aacute");
		m_szStandString.push_back("Acircumflex");
		m_szStandString.push_back("Adieresis");
		m_szStandString.push_back("Agrave");
		m_szStandString.push_back("Aring");
		m_szStandString.push_back("Atilde");
		m_szStandString.push_back("Ccedilla");
		m_szStandString.push_back("Eacute");
		m_szStandString.push_back("Ecircumflex");
		m_szStandString.push_back("Edieresis");
		m_szStandString.push_back("Egrave");
		m_szStandString.push_back("Iacute");
		m_szStandString.push_back("Icircumflex");
		m_szStandString.push_back("Idieresis");
		m_szStandString.push_back("Igrave");
		m_szStandString.push_back("Ntilde");
		m_szStandString.push_back("Oacute");
		m_szStandString.push_back("Ocircumflex");
		m_szStandString.push_back("Odieresis");
		m_szStandString.push_back("Ograve");
		m_szStandString.push_back("Otilde");
		m_szStandString.push_back("Scaron");
		m_szStandString.push_back("Uacute");
		m_szStandString.push_back("Ucircumflex");
		m_szStandString.push_back("Udieresis");
		m_szStandString.push_back("Ugrave");
		m_szStandString.push_back("Yacute");
		m_szStandString.push_back("Ydieresis");
		m_szStandString.push_back("Zcaron");
		m_szStandString.push_back("aacute");
		m_szStandString.push_back("acircumflex");
		m_szStandString.push_back("adieresis");
		m_szStandString.push_back("agrave");
		m_szStandString.push_back("aring");
		m_szStandString.push_back("atilde");
		m_szStandString.push_back("ccedilla");
		m_szStandString.push_back("eacute");
		m_szStandString.push_back("ecircumflex");
		m_szStandString.push_back("edieresis");
		m_szStandString.push_back("egrave");
		m_szStandString.push_back("iacute");
		m_szStandString.push_back("icircumflex");
		m_szStandString.push_back("idieresis");
		m_szStandString.push_back("igrave");
		m_szStandString.push_back("ntilde");
		m_szStandString.push_back("oacute");
		m_szStandString.push_back("ocircumflex");
		m_szStandString.push_back("odieresis");
		m_szStandString.push_back("ograve");
		m_szStandString.push_back("otilde");
		m_szStandString.push_back("scaron");
		m_szStandString.push_back("uacute");
		m_szStandString.push_back("ucircumflex");
		m_szStandString.push_back("udieresis");
		m_szStandString.push_back("ugrave");
		m_szStandString.push_back("yacute");
		m_szStandString.push_back("ydieresis");
		m_szStandString.push_back("zcaron");
		m_szStandString.push_back("exclamsmall");
		m_szStandString.push_back("Hungarumlautsmall");
		m_szStandString.push_back("dollaroldstyle");
		m_szStandString.push_back("dollarsuperior");
		m_szStandString.push_back("ampersandsmall");
		m_szStandString.push_back("Acutesmall");
		m_szStandString.push_back("parenleftsuperior");
		m_szStandString.push_back("parenrightsuperior");
		m_szStandString.push_back("twodotenleader");
		m_szStandString.push_back("onedotenleader");
		m_szStandString.push_back("zerooldstyle");
		m_szStandString.push_back("oneoldstyle");
		m_szStandString.push_back("twooldstyle");
		m_szStandString.push_back("threeoldstyle");
		m_szStandString.push_back("fouroldstyle");
		m_szStandString.push_back("fiveoldstyle");
		m_szStandString.push_back("sixoldstyle");
		m_szStandString.push_back("sevenoldstyle");
		m_szStandString.push_back("eightoldstyle");
		m_szStandString.push_back("nineoldstyle");
		m_szStandString.push_back("commasuperior");
		m_szStandString.push_back("threequartersemdash");
		m_szStandString.push_back("periodsuperior");
		m_szStandString.push_back("questionsma");
		m_szStandString.push_back("asuperior");
		m_szStandString.push_back("bsuperior");
		m_szStandString.push_back("centsuperior");
		m_szStandString.push_back("dsuperior");
		m_szStandString.push_back("esuperior");
		m_szStandString.push_back("isuperior");
		m_szStandString.push_back("lsuperior");
		m_szStandString.push_back("msuperior");
		m_szStandString.push_back("nsuperior");
		m_szStandString.push_back("osuperior");
		m_szStandString.push_back("rsuperior");
		m_szStandString.push_back("ssuperior");
		m_szStandString.push_back("tsuperior");
		m_szStandString.push_back("ff");
		m_szStandString.push_back("ffi");
		m_szStandString.push_back("ffl");
		m_szStandString.push_back("parenleftinferior");
		m_szStandString.push_back("parenrightinferior");
		m_szStandString.push_back("Circumflexsmall");
		m_szStandString.push_back("hyphensuperior");
		m_szStandString.push_back("Gravesmall");
		m_szStandString.push_back("Asmall");
		m_szStandString.push_back("Bsmall");
		m_szStandString.push_back("Csmall");
		m_szStandString.push_back("Dsmall");
		m_szStandString.push_back("Esmall");
		m_szStandString.push_back("Fsmall");
		m_szStandString.push_back("Gsmall");
		m_szStandString.push_back("Hsmall");
		m_szStandString.push_back("Ismall");
		m_szStandString.push_back("Jsmall");
		m_szStandString.push_back("Ksmall");
		m_szStandString.push_back("Lsmall");
		m_szStandString.push_back("Msmall");
		m_szStandString.push_back("Nsmall");
		m_szStandString.push_back("Osmall");
		m_szStandString.push_back("Psmall");
		m_szStandString.push_back("Qsmall");
		m_szStandString.push_back("Rsmall");
		m_szStandString.push_back("Ssmall");
		m_szStandString.push_back("Tsmall");
		m_szStandString.push_back("Usmall");
		m_szStandString.push_back("Vsmall");
		m_szStandString.push_back("Wsmall");
		m_szStandString.push_back("Xsmall");
		m_szStandString.push_back("Ysmall");
		m_szStandString.push_back("Zsmall");
		m_szStandString.push_back("colonmonetary");
		m_szStandString.push_back("onefitted");
		m_szStandString.push_back("rupiah");
		m_szStandString.push_back("Tildesmall");
		m_szStandString.push_back("exclamdownsmall");
		m_szStandString.push_back("centoldstyle");
		m_szStandString.push_back("Lslashsmall");
		m_szStandString.push_back("Scaronsmall");
		m_szStandString.push_back("Zcaronsmall");
		m_szStandString.push_back("Dieresissmall");
		m_szStandString.push_back("Brevesmall");
		m_szStandString.push_back("Caronsmall");
		m_szStandString.push_back("Dotaccentsmall");
		m_szStandString.push_back("Macronsmall");
		m_szStandString.push_back("figuredash");
		m_szStandString.push_back("hypheninferior");
		m_szStandString.push_back("Ogoneksmall");
		m_szStandString.push_back("Ringsmall");
		m_szStandString.push_back("Cedillasmall");
		m_szStandString.push_back("questiondownsmall");
		m_szStandString.push_back("oneeighth");
		m_szStandString.push_back("threeeighths");
		m_szStandString.push_back("fiveeighths");
		m_szStandString.push_back("seveneighths");
		m_szStandString.push_back("onethird");
		m_szStandString.push_back("twothirds");
		m_szStandString.push_back("zerosuperior");
		m_szStandString.push_back("foursuperior");
		m_szStandString.push_back("fivesuperior");
		m_szStandString.push_back("sixsuperior");
		m_szStandString.push_back("sevensuperior");
		m_szStandString.push_back("eightsuperior");
		m_szStandString.push_back("ninesuperior");
		m_szStandString.push_back("zeroinferior");
		m_szStandString.push_back("oneinferior");
		m_szStandString.push_back("twoinferior");
		m_szStandString.push_back("threeinferior");
		m_szStandString.push_back("fourinferior");
		m_szStandString.push_back("fiveinferior");
		m_szStandString.push_back("sixinferior");
		m_szStandString.push_back("seveninferior");
		m_szStandString.push_back("eightinferior");
		m_szStandString.push_back("nineinferior");
		m_szStandString.push_back("centinferior");
		m_szStandString.push_back("dollarinferior");
		m_szStandString.push_back("periodinferior");
		m_szStandString.push_back("commainferior");
		m_szStandString.push_back("Agravesmall");
		m_szStandString.push_back("Aacutesmall");
		m_szStandString.push_back("Acircumflexsmall");
		m_szStandString.push_back("Atildesmall");
		m_szStandString.push_back("Adieresissmall");
		m_szStandString.push_back("Aringsmall");
		m_szStandString.push_back("AEsmall");
		m_szStandString.push_back("Ccedillasmall");
		m_szStandString.push_back("Egravesmall");
		m_szStandString.push_back("Eacutesmall");
		m_szStandString.push_back("Ecircumflexsmall");
		m_szStandString.push_back("Edieresissmall");
		m_szStandString.push_back("Igravesmall");
		m_szStandString.push_back("Iacutesmall");
		m_szStandString.push_back("Icircumflexsmall");
		m_szStandString.push_back("Idieresissmall");
		m_szStandString.push_back("Ethsmall");
		m_szStandString.push_back("Ntildesmall");
		m_szStandString.push_back("Ogravesmall");
		m_szStandString.push_back("Oacutesmall");
		m_szStandString.push_back("Ocircumflexsmall");
		m_szStandString.push_back("Otildesmall");
		m_szStandString.push_back("Odieresissmall");
		m_szStandString.push_back("OEsmall");
		m_szStandString.push_back("Oslashsmall");
		m_szStandString.push_back("Ugravesmall");
		m_szStandString.push_back("Uacutesmall");
		m_szStandString.push_back("Ucircumflexsmall");
		m_szStandString.push_back("Udieresissmall");
		m_szStandString.push_back("Yacutesmall");
		m_szStandString.push_back("Thornsmall");
		m_szStandString.push_back("Ydieresissmall");
		m_szStandString.push_back("001.000");
		m_szStandString.push_back("001.001");
		m_szStandString.push_back("001.002");
		m_szStandString.push_back("001.003");
		m_szStandString.push_back("Black");
		m_szStandString.push_back("Bold");
		m_szStandString.push_back("Book");
		m_szStandString.push_back("Light");
		m_szStandString.push_back("Medium");
		m_szStandString.push_back("Regular");
		m_szStandString.push_back("Roman");
		m_szStandString.push_back("Semibold");

	}	// end of void CHYCFFStandString::SetDefault()
	
	int CHYCFFStandString::FindNumberByString(std::string str)
	{
		size_t szSize = m_szStandString.size();
		for(size_t i=0; i!=szSize; i++)
		{	
			if (m_szStandString[i]==str)
				return  i;
		}

		return -1;

	}	// end of int CHYCFFStandString::FindNumberByString()
	
	int CHYCFFStandString::InsertString(std::string strString)
	{
		if (strString.size() == 0) 
			return -1;

		int iIndex = -1;
		size_t st = m_szStandString.size();		

		for (size_t i=0; i<st; i++)
		{
			std::string strTmp = m_szStandString[i];

			if (strTmp == strString)
			{
				return iIndex = i;				
			}
		}

		if (iIndex == -1)
		{
			m_szStandString.push_back(strString);
			return m_szStandString.size() -1;
		}

		return 0;

	}	// end of int CHYCFFInfo::InsertString()


	CHYCFFIndex::CHYCFFIndex()
	{
		Count	= 0;
		OffSize	= 0;

	}	// end of CHYCFFIndex::CHYCFFIndex()

	CHYCFFIndex::~CHYCFFIndex()
	{

	}	// end of CHYCFFIndex::~CHYCFFIndex()

	void CHYCFFIndex::Setdefault()
	{
		Count = 0 ;
		OffSize = 0;
		vtOffset.clear();
		vtData.clear();

	}	// end of void CHYCFFIndex::Setdefault()

	CHYCFFROS::CHYCFFROS()
	{
		RegistrySID = 0;
		OrderingSID = 0;
		Supplement	= 0;	

	}	// end of CHYCFFROS::CHYCFFROS()

	CHYCFFROS::~CHYCFFROS()
	{

	}	// end of CHYCFFROS::~CHYCFFROS()

	CHYCFFPrivteDict::CHYCFFPrivteDict()
	{
		strFontName.empty();	
		iFontNameID			=0;	
		fBlueScale			= 0.039625f;
		fBlueShift			= 7.0f;
		fBlueFuzz			= 1.0f;
		fStdHW				= 0.0f;
		fStdVW				= 0.0f;
		lForceBold			= 0;
		lLanguageGroup		= 0;
		fExpansionFactor	= 0.06f;
		finitialRandomSeed	= 0;
		lSubrsOffset		= 0;
		ldefaultWidthX		= 0;
		lnominalWidthX		= 0;

	}	// end of CHYCFFPrivteDict::CHYCFFPrivteDict()

	CHYCFFPrivteDict::~CHYCFFPrivteDict()
	{

	}	// end of CHYCFFPrivteDict::~CHYCFFPrivteDict()

	void CHYCFFPrivteDict::SetDefault()
	{
		strFontName.empty();	
		iFontNameID=0;
		vtBlueValues.clear();
		vtOtherBlues.clear();
		vtFamilyBlues.clear();
		vtFamilyOtherBlues.clear();		
		fBlueScale = 0.039625f;
		fBlueShift = 7.0f;
		fBlueFuzz = 1.0f;
		fStdHW=0.0f;
		fStdVW=0.0f;
		vtStemSnapH.clear();	
		vtStemSnapV.clear();
		lForceBold = 0;
		lLanguageGroup = 0;
		fExpansionFactor = 0.06f;
		finitialRandomSeed = 0;
		lSubrsOffset = 0;
		ldefaultWidthX = 0;
		lnominalWidthX = 0;
		SubIndex.Setdefault();

	}	// end of void CHYCFFPrivteDict::SetDefault()

	CCFFFDSRang3::CCFFFDSRang3()
	{
		first	= 0;
		fdIndex	= 0;

	}	// end of CCFFFDSRang3::CCFFFDSRang3()

	CCFFFDSRang3::~CCFFFDSRang3()
	{

	}	// end of CCFFFDSRang3::~CCFFFDSRang3()

	CCFFFDSFormat3::CCFFFDSFormat3()
	{
		sentinel = 0;

	}	// end of CCFFFDSFormat3::CCFFFDSFormat3()

	CCFFFDSFormat3::~CCFFFDSFormat3()
	{

	}	// end of CCFFFDSFormat3::~CCFFFDSFormat3()

	void CCFFFDSFormat3::Setdefault()
	{
		vtRang3.clear();
		sentinel = 0;

	}	// end of void CCFFFDSFormat3::Setdefault()

	CHYCFFFDSelect::CHYCFFFDSelect()
	{
		iFormat = -1;

	}	// end of CHYCFFFDSelect::CHYCFFFDSelect()

	CHYCFFFDSelect::~CHYCFFFDSelect()
	{

	}	// end of CHYCFFFDSelect::~CHYCFFFDSelect()

	void CHYCFFFDSelect::Setdefault()
	{
		iFormat = -1;
		format0.clear();
		format3.Setdefault();	

	}	// end of void CHYCFFFDSelect::Setdefault()
	
	CCFFCSFormatRang::CCFFCSFormatRang()
	{
		first=0;
		left=0;

	}	// end of CCFFCSFormatRang2::CCFFCSFormatRang2()

	CCFFCSFormatRang::~CCFFCSFormatRang()
	{

	}	// end of CCFFCSFormatRang2::~CCFFCSFormatRang2()

	void CCFFCSFormatRang::SetDefault()
	{
		first=0;
		left=0;

	}	// end of void CCFFCSFormatRang2::SetDefault()
	
	CCFFCSFormat::CCFFCSFormat()
	{

	}	// end of CCFFCSFormat2::CCFFCSFormat2()

	CCFFCSFormat::~CCFFCSFormat()
	{

	}	// end of CCFFCSFormat2::~CCFFCSFormat2()

	void CCFFCSFormat::Setdefault()
	{
		vtRang.clear();

	}	// end of void CCFFCSFormat2::Setdefault()

	CHYCFFCharset::CHYCFFCharset()
	{
		format=0xff;

	}	// end of CHYCFFCharset::CHYCFFCharset()

	CHYCFFCharset::~CHYCFFCharset()
	{

	}	// end of CHYCFFCharset::~CHYCFFCharset()

	void CHYCFFCharset::Setdefault()
	{		
		vtFoamat0SID.clear();
		format1and2.Setdefault();	

	}	// end of void CHYCFFCharset::Setdefault()

	CHYCFFTopDict::CHYCFFTopDict(void)
	{
		strVersion.empty();
		iVersionSID				= 0;
		strNotice.empty();
		iNoticeSID				= 0;
		strCopyright.empty();
		iCopyrghtSID			= 0;
		strFullName.empty();	
		iFullNameSID			= 0;
		strFamilyName.empty();	
		iFamilyNameSID			= 0;
		strWeight.empty();	
		iWeightSID				= 0;
		isFixedPitch			= 0;
		ItalicAngle				= 0.0f;
		UnderlinePosition		= -100.0f;
		UnderlineThickness		= 50.0f;
		PaintType				= 0;
		CharStringType			= 2;
		UniqueID				= -1;
		strokeWidth				= 0;	
		charsetOffset			= 0;
		encodingOffset			= 0;
		charStringOffset		= 0;
		PrivteDictSize			= 0;
		PrivteOffset			= 0;
		SyntheticBaseIndex		= 0;
		strPostSript.empty();	

		iPostSriptSID			= 0;
		strBaseFontName.empty();	
		iBaseFontNameSID		= 0;
		// cid font 特有
		Ros.OrderingSID = 0;
		Ros.strOrdering.empty();
		Ros.RegistrySID = 0;
		Ros.strRegistry.empty();
		Ros.Supplement = 0;

		CIDFontVersion = 0;
		CIDFontRevision = 0;
		CIDFontType = 0;
		CIDCount = 8720;
		UIDBase = 0;
		FDArryIndexOffset = 0;
		FDSelectOffset = 0;
		strCIDFontName.empty();
		CIDFontNameSID = 0;	
		IsCIDFont = 0;

	}	// end of CTopDictData::CTopDictData(void)

	CHYCFFTopDict::~CHYCFFTopDict(void)
	{

	}	// end of CTopDictData::~CTopDictData(void)

	void CHYCFFTopDict::SetDefault()
	{
		strVersion.empty();
		iVersionSID				= 0;
		strNotice.empty();
		iNoticeSID				= 0;
		strCopyright.empty();
		iCopyrghtSID			= 0;
		strFullName.empty();	
		iFullNameSID			= 0;
		strFamilyName.empty();	
		iFamilyNameSID			= 0;
		strWeight.empty();	
		iWeightSID				= 0;
		isFixedPitch			= 0;
		ItalicAngle				= 0.0f;
		UnderlinePosition		= -100.0f;
		UnderlineThickness		= 50;
		PaintType				= 0;
		CharStringType			= 2;
		vtFontMatrix.clear();	
		UniqueID				= -1;
		strokeWidth				= 0;
		vtXUID.clear();
		charsetOffset			= 0;
		encodingOffset			= 0;
		charStringOffset		= 0;
		PrivteDictSize			= 0;
		PrivteOffset			= 0;
		SyntheticBaseIndex		= 0;
		strPostSript.empty();
		iPostSriptSID			= 0;
		strBaseFontName.empty();	
		iBaseFontNameSID		= 0;
		vtBaseFontBlend.clear();
		// cid font 特有
		Ros.OrderingSID			= 0;
		Ros.strOrdering.empty();
		Ros.RegistrySID			= 0;
		Ros.strRegistry.empty();
		Ros.Supplement			= 0;
		CIDFontVersion			= 0;
		CIDFontRevision			= 0;
		CIDFontType				= 0;
		CIDCount				= 8720;
		UIDBase					= 0;
		FDArryIndexOffset		= 0;
		FDSelectOffset			= 0;
		strCIDFontName.empty();
		CIDFontNameSID			= 0;	
		IsCIDFont				= 0;

	}	// end of void CHYCFFTopDict::SetDefault()

	CHYCFFHeader::CHYCFFHeader()
	{
		major		= 0;
		minor		= 0;
		hdrSize		= 0;
		offSize		= 0;

	}	// end of CHYCFFHeader::CHYCFFHeader()

	CHYCFFHeader::~CHYCFFHeader()
	{

	}	// end of CHYCFFHeader::~CHYCFFHeader()

	CHYCFFInfo::CHYCFFInfo(void)
	{

	}	// end of CHYCFFInfo::CHYCFFInfo()

	CHYCFFInfo::~CHYCFFInfo(void)
	{

	}	// end of CHYCFFInfo::~CHYCFFInfo()

	void CHYCFFInfo::SetDefault()
	{
		Header.major = 0;	
		Header.hdrSize = 0;
		Header.minor = 0;
		Header.offSize = 0;

		TopDICT.SetDefault();
		vtFDArry.clear();
		vtFontName.clear();
		stnStrings.SetDefault();
		globalSubIndex.Setdefault();
		PrivteDict.SetDefault();
		FDSelect.Setdefault();

	}	// end of void CHYCFFInfo::SetDefault()	

	unsigned short	CHYCFFInfo::FindSIDbyGlyphID(unsigned usGID)
	{
		if (usGID==0) return 0;
		if (Charset.format == 0){
			return Charset.vtFoamat0SID[usGID-1];
		}

		if (Charset.format == 1 || Charset.format == 2){
			size_t t = Charset.format1and2.vtRang.size();
			int iGIDIndex = 1;
			for (size_t i=0; i<t; i++) 	{				
				CCFFCSFormatRang& FormatRang = Charset.format1and2.vtRang[i];
				if (usGID == iGIDIndex){
					return FormatRang.first;
				}

				if (usGID>iGIDIndex && usGID<=iGIDIndex+FormatRang.left){					
					return FormatRang.first+usGID-iGIDIndex;
				}
				else{
					iGIDIndex += FormatRang.left+1;
				}
			}
		}		
		return 0;

	}	// end of unsigned short CHYCFFInfo::FindSIDbyGlyph()

	std::string	CHYCFFInfo::FindStringbyGlyphID(unsigned usGID)
	{
		unsigned short usSID = FindSIDbyGlyphID(usGID);
		return stnStrings.m_szStandString[usSID];

	}	// end of unsigned short CHYCFFInfo::FindNamebyGlyphID()

}