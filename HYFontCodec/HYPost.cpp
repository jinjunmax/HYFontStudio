#include "StdAfx.h"
#include "HYPost.h"

namespace HYFONTCODEC
{
	CPostFormat2::CPostFormat2()
	{
		usNumberOfGlyphs = 0;


	}	// end of CPostFormat2::CPostFormat2()

	CPostFormat2::~CPostFormat2()
	{

	}	// end of CPostFormat2::~CPostFormat2()

	void CPostFormat2::SetDefault()
	{
		usNumberOfGlyphs = 0;
		vtGlyphNameIndex.clear();
		vtNameLength.clear();
		vtNameStrings.clear();

	}	// end of void CPostFormat2::SetDefault()

	CPostFormat2& CPostFormat2::operator=(const CPostFormat2& other)
	{
		if (this == &other)
		{
			return *this;
		}
		else 
		{
			usNumberOfGlyphs = other.usNumberOfGlyphs;
			vtGlyphNameIndex = other.vtGlyphNameIndex;
			vtNameLength = other.vtNameLength;
			vtNameStrings = other.vtNameStrings;
		}

		return *this;

	}	// end of CPostFormat2& CPostFormat2::operator=()

	CPostFormat25::CPostFormat25()
	{
		usNumberOfGlyphs = 0;

	}	// end of CPostFormat25::CPostFormat25()
		
	CPostFormat25::~CPostFormat25()
	{

	}	// end of CPostFormat25::~CPostFormat25()

	void CPostFormat25::SetDefault()
	{
		usNumberOfGlyphs = 0;
		vtNameOffset.clear();
		vtNameStrings.clear();

	}	// end of void CPostFormat25::SetDefault()

	CPostFormat25& CPostFormat25::operator=(const CPostFormat25& other)
	{
		if (this == &other)
		{
			return *this;
		}
		else 
		{
			usNumberOfGlyphs = other.usNumberOfGlyphs;
			vtNameOffset = other.vtNameOffset;
			vtNameStrings = other.vtNameStrings;
		}

		return *this;

	}	// end of CPostFormat25& CPostFormat25::operator=()

	CHYPost::CHYPost(void)
	{
		Format.fract		= 0;
		Format.value		= 0;
		italicAngle.fract	= 0;
		italicAngle.value	= 0;
		underlinePosition	= 0;
		underlineThickness	= 0;
		isFixedPitch		= 0;
		minMemType42		= 0;
		maxMemType42		= 0;
		minMemType1			= 0;
		maxMemType1			= 0;

		m_vtStandString.push_back(".notdef");
		m_vtStandString.push_back(".null");
		m_vtStandString.push_back("nonmarkingreturn");
		m_vtStandString.push_back("space");		
		m_vtStandString.push_back("exclam");
		m_vtStandString.push_back("quotedbl");
		m_vtStandString.push_back("numbersign");
		m_vtStandString.push_back("dollar");
		m_vtStandString.push_back("percent");
		m_vtStandString.push_back("ampersand");
		m_vtStandString.push_back("quotesingle");
		m_vtStandString.push_back("parenleft");
		m_vtStandString.push_back("parenright");
		m_vtStandString.push_back("asterisk");
		m_vtStandString.push_back("plus");
		m_vtStandString.push_back("comma");
		m_vtStandString.push_back("hyphen");
		m_vtStandString.push_back("period");
		m_vtStandString.push_back("slash");
		m_vtStandString.push_back("zero");
		m_vtStandString.push_back("one");
		m_vtStandString.push_back("two");
		m_vtStandString.push_back("three");
		m_vtStandString.push_back("four");
		m_vtStandString.push_back("five");
		m_vtStandString.push_back("six");
		m_vtStandString.push_back("seven");
		m_vtStandString.push_back("eight");
		m_vtStandString.push_back("nine");
		m_vtStandString.push_back("colon");
		m_vtStandString.push_back("semicolon");
		m_vtStandString.push_back("less");
		m_vtStandString.push_back("equal");
		m_vtStandString.push_back("greater");
		m_vtStandString.push_back("question");
		m_vtStandString.push_back("at");
		m_vtStandString.push_back("A");
		m_vtStandString.push_back("B");
		m_vtStandString.push_back("C");
		m_vtStandString.push_back("D");
		m_vtStandString.push_back("E");
		m_vtStandString.push_back("F");
		m_vtStandString.push_back("G");
		m_vtStandString.push_back("H");
		m_vtStandString.push_back("I");
		m_vtStandString.push_back("J");
		m_vtStandString.push_back("K");
		m_vtStandString.push_back("L");
		m_vtStandString.push_back("M");
		m_vtStandString.push_back("N");
		m_vtStandString.push_back("O");
		m_vtStandString.push_back("P");
		m_vtStandString.push_back("Q");
		m_vtStandString.push_back("R");
		m_vtStandString.push_back("S");
		m_vtStandString.push_back("T");
		m_vtStandString.push_back("U");
		m_vtStandString.push_back("V");
		m_vtStandString.push_back("W");
		m_vtStandString.push_back("X");
		m_vtStandString.push_back("Y");
		m_vtStandString.push_back("Z");
		m_vtStandString.push_back("bracketleft");
		m_vtStandString.push_back("backslash");
		m_vtStandString.push_back("bracketright");
		m_vtStandString.push_back("asciicircum");
		m_vtStandString.push_back("underscore");
		m_vtStandString.push_back("grave");
		m_vtStandString.push_back("a");
		m_vtStandString.push_back("b");
		m_vtStandString.push_back("c");
		m_vtStandString.push_back("d");
		m_vtStandString.push_back("e");
		m_vtStandString.push_back("f");
		m_vtStandString.push_back("g");
		m_vtStandString.push_back("h");
		m_vtStandString.push_back("i");
		m_vtStandString.push_back("j");
		m_vtStandString.push_back("k");
		m_vtStandString.push_back("l");
		m_vtStandString.push_back("m");
		m_vtStandString.push_back("n");
		m_vtStandString.push_back("o");
		m_vtStandString.push_back("p");
		m_vtStandString.push_back("q");
		m_vtStandString.push_back("r");
		m_vtStandString.push_back("s");
		m_vtStandString.push_back("t");
		m_vtStandString.push_back("u");
		m_vtStandString.push_back("v");
		m_vtStandString.push_back("w");
		m_vtStandString.push_back("x");
		m_vtStandString.push_back("y");
		m_vtStandString.push_back("z");
		m_vtStandString.push_back("braceleft");
		m_vtStandString.push_back("bar");
		m_vtStandString.push_back("braceright");
		m_vtStandString.push_back("asciitilde");
		m_vtStandString.push_back("Adieresis");
		m_vtStandString.push_back("Aring");
		m_vtStandString.push_back("Ccedilla");
		m_vtStandString.push_back("Eacute");
		m_vtStandString.push_back("Ntilde");
		m_vtStandString.push_back("Odieresis");
		m_vtStandString.push_back("Udieresis");
		m_vtStandString.push_back("aacute");
		m_vtStandString.push_back("agrave");
		m_vtStandString.push_back("acircumflex");
		m_vtStandString.push_back("adieresis");
		m_vtStandString.push_back("atilde");
		m_vtStandString.push_back("aring");
		m_vtStandString.push_back("ccedilla");
		m_vtStandString.push_back("eacute");
		m_vtStandString.push_back("egrave");
		m_vtStandString.push_back("ecircumflex");
		m_vtStandString.push_back("edieresis");
		m_vtStandString.push_back("iacute");
		m_vtStandString.push_back("igrave");
		m_vtStandString.push_back("icircumflex");
		m_vtStandString.push_back("idieresis");
		m_vtStandString.push_back("ntilde");
		m_vtStandString.push_back("oacute");
		m_vtStandString.push_back("ograve");
		m_vtStandString.push_back("ocircumflex");
		m_vtStandString.push_back("odieresis");
		m_vtStandString.push_back("otilde");
		m_vtStandString.push_back("uacute");
		m_vtStandString.push_back("ugrave");
		m_vtStandString.push_back("ucircumflex");
		m_vtStandString.push_back("udieresis");
		m_vtStandString.push_back("dagger");
		m_vtStandString.push_back("degree");
		m_vtStandString.push_back("cent");
		m_vtStandString.push_back("sterling");
		m_vtStandString.push_back("section");
		m_vtStandString.push_back("bullet");
		m_vtStandString.push_back("paragraph");
		m_vtStandString.push_back("germandbls");
		m_vtStandString.push_back("registered");
		m_vtStandString.push_back("copyright");
		m_vtStandString.push_back("trademark");
		m_vtStandString.push_back("acute");
		m_vtStandString.push_back("dieresis");
		m_vtStandString.push_back("notequal");
		m_vtStandString.push_back("AE");
		m_vtStandString.push_back("Oslash");
		m_vtStandString.push_back("infinity");
		m_vtStandString.push_back("plusminus");
		m_vtStandString.push_back("lessequal");
		m_vtStandString.push_back("greaterequal");
		m_vtStandString.push_back("yen");
		m_vtStandString.push_back("mu");
		m_vtStandString.push_back("partialdiff");
		m_vtStandString.push_back("summation");
		m_vtStandString.push_back("product");
		m_vtStandString.push_back("pi");
		m_vtStandString.push_back("integral");
		m_vtStandString.push_back("ordfeminine");
		m_vtStandString.push_back("ordmasculine");
		m_vtStandString.push_back("Omega");
		m_vtStandString.push_back("ae");
		m_vtStandString.push_back("oslash");
		m_vtStandString.push_back("questiondown");
		m_vtStandString.push_back("exclamdown");
		m_vtStandString.push_back("logicalnot");
		m_vtStandString.push_back("radical");
		m_vtStandString.push_back("florin");
		m_vtStandString.push_back("approxequal");
		m_vtStandString.push_back("Delta");
		m_vtStandString.push_back("guillemotleft");
		m_vtStandString.push_back("guillemotright");
		m_vtStandString.push_back("ellipsis");
		m_vtStandString.push_back("nonbreakingspace");
		m_vtStandString.push_back("Agrave");
		m_vtStandString.push_back("Atilde");
		m_vtStandString.push_back("Otilde");
		m_vtStandString.push_back("OE");
		m_vtStandString.push_back("oe");
		m_vtStandString.push_back("endash");
		m_vtStandString.push_back("emdash");
		m_vtStandString.push_back("quotedblleft");
		m_vtStandString.push_back("quotedblright");
		m_vtStandString.push_back("quoteleft");
		m_vtStandString.push_back("quoteright");
		m_vtStandString.push_back("divide");
		m_vtStandString.push_back("lozenge");
		m_vtStandString.push_back("ydieresis");
		m_vtStandString.push_back("Ydieresis");
		m_vtStandString.push_back("fraction");
		m_vtStandString.push_back("currency");
		m_vtStandString.push_back("guilsinglleft");
		m_vtStandString.push_back("guilsinglright");
		m_vtStandString.push_back("fi");
		m_vtStandString.push_back("fl");
		m_vtStandString.push_back("daggerdbl");
		m_vtStandString.push_back("periodcentered");
		m_vtStandString.push_back("quotesinglbase");
		m_vtStandString.push_back("quotedblbase");
		m_vtStandString.push_back("perthousand");
		m_vtStandString.push_back("Acircumflex");
		m_vtStandString.push_back("Ecircumflex");
		m_vtStandString.push_back("Aacute");
		m_vtStandString.push_back("Edieresis");
		m_vtStandString.push_back("Egrave");
		m_vtStandString.push_back("Iacute");
		m_vtStandString.push_back("Icircumflex");
		m_vtStandString.push_back("Idieresis");
		m_vtStandString.push_back("Igrave");
		m_vtStandString.push_back("Oacute");
		m_vtStandString.push_back("Ocircumflex");
		m_vtStandString.push_back("apple");
		m_vtStandString.push_back("Ograve");
		m_vtStandString.push_back("Uacute");
		m_vtStandString.push_back("Ucircumflex");
		m_vtStandString.push_back("Ugrave");
		m_vtStandString.push_back("dotlessi");
		m_vtStandString.push_back("circumflex");
		m_vtStandString.push_back("tilde");
		m_vtStandString.push_back("macron");
		m_vtStandString.push_back("breve");
		m_vtStandString.push_back("dotaccent");
		m_vtStandString.push_back("ring");
		m_vtStandString.push_back("cedilla");	
		m_vtStandString.push_back("hungarumlaut");	
		m_vtStandString.push_back("ogonek");
		m_vtStandString.push_back("caron");
		m_vtStandString.push_back("Lslash");
		m_vtStandString.push_back("lslash");
		m_vtStandString.push_back("Scaron");
		m_vtStandString.push_back("scaron");
		m_vtStandString.push_back("Zcaron");
		m_vtStandString.push_back("zcaron");
		m_vtStandString.push_back("brokenbar");
		m_vtStandString.push_back("Eth");
		m_vtStandString.push_back("eth");
		m_vtStandString.push_back("Yacute");
		m_vtStandString.push_back("yacute");
		m_vtStandString.push_back("Thorn");
		m_vtStandString.push_back("thorn");
		m_vtStandString.push_back("minus");
		m_vtStandString.push_back("multiply");
		m_vtStandString.push_back("onesuperior");
		m_vtStandString.push_back("twosuperior");
		m_vtStandString.push_back("threesuperior");
		m_vtStandString.push_back("onehalf");
		m_vtStandString.push_back("onequarter");
		m_vtStandString.push_back("threequarters");
		m_vtStandString.push_back("franc");
		m_vtStandString.push_back("Gbreve");
		m_vtStandString.push_back("gbreve");
		m_vtStandString.push_back("Idotaccent");
		m_vtStandString.push_back("Scedilla");
		m_vtStandString.push_back("scedilla");
		m_vtStandString.push_back("Cacute");
		m_vtStandString.push_back("cacute");
		m_vtStandString.push_back("Ccaron");
		m_vtStandString.push_back("ccaron");
		m_vtStandString.push_back("dcroat");

	}	// end of CHYPost::CHYPost()

	CHYPost::~CHYPost(void)
	{

	}	// end of CHYPost::~CHYPost()	

	void CHYPost::SetDefault()
	{
		Format.fract		= 0;
		Format.value		= 0;
		italicAngle.fract	= 0;
		italicAngle.value	= 0;
		underlinePosition	= 0;
		underlineThickness	= 0;
		isFixedPitch		= 0;
		minMemType42		= 0;
		maxMemType42		= 0;
		minMemType1			= 0;
		maxMemType1			= 0;

		m_Format2.SetDefault();
		m_Format25.SetDefault();

		m_vtStandString.clear();
		m_vtStandString.push_back(".notdef");
		m_vtStandString.push_back(".null");
		m_vtStandString.push_back("nonmarkingreturn");
		m_vtStandString.push_back("space");
		m_vtStandString.push_back("exclam");
		m_vtStandString.push_back("quotedbl");
		m_vtStandString.push_back("numbersign");
		m_vtStandString.push_back("dollar");
		m_vtStandString.push_back("percent");
		m_vtStandString.push_back("ampersand");
		m_vtStandString.push_back("quotesingle");
		m_vtStandString.push_back("parenleft");
		m_vtStandString.push_back("parenright");
		m_vtStandString.push_back("asterisk");
		m_vtStandString.push_back("plus");
		m_vtStandString.push_back("comma");
		m_vtStandString.push_back("hyphen");
		m_vtStandString.push_back("period");
		m_vtStandString.push_back("slash");
		m_vtStandString.push_back("zero");
		m_vtStandString.push_back("one");
		m_vtStandString.push_back("two");
		m_vtStandString.push_back("three");
		m_vtStandString.push_back("four");
		m_vtStandString.push_back("five");
		m_vtStandString.push_back("six");
		m_vtStandString.push_back("seven");
		m_vtStandString.push_back("eight");
		m_vtStandString.push_back("nine");
		m_vtStandString.push_back("colon");
		m_vtStandString.push_back("semicolon");
		m_vtStandString.push_back("less");
		m_vtStandString.push_back("equal");
		m_vtStandString.push_back("greater");
		m_vtStandString.push_back("question");
		m_vtStandString.push_back("at");
		m_vtStandString.push_back("A");
		m_vtStandString.push_back("B");
		m_vtStandString.push_back("C");
		m_vtStandString.push_back("D");
		m_vtStandString.push_back("E");
		m_vtStandString.push_back("F");
		m_vtStandString.push_back("G");
		m_vtStandString.push_back("H");
		m_vtStandString.push_back("I");
		m_vtStandString.push_back("J");
		m_vtStandString.push_back("K");
		m_vtStandString.push_back("L");
		m_vtStandString.push_back("M");
		m_vtStandString.push_back("N");
		m_vtStandString.push_back("O");
		m_vtStandString.push_back("P");
		m_vtStandString.push_back("Q");
		m_vtStandString.push_back("R");
		m_vtStandString.push_back("S");
		m_vtStandString.push_back("T");
		m_vtStandString.push_back("U");
		m_vtStandString.push_back("V");
		m_vtStandString.push_back("W");
		m_vtStandString.push_back("X");
		m_vtStandString.push_back("Y");
		m_vtStandString.push_back("Z");
		m_vtStandString.push_back("bracketleft");
		m_vtStandString.push_back("backslash");
		m_vtStandString.push_back("bracketright");
		m_vtStandString.push_back("asciicircum");
		m_vtStandString.push_back("underscore");
		m_vtStandString.push_back("grave");
		m_vtStandString.push_back("a");
		m_vtStandString.push_back("b");
		m_vtStandString.push_back("c");
		m_vtStandString.push_back("d");
		m_vtStandString.push_back("e");
		m_vtStandString.push_back("f");
		m_vtStandString.push_back("g");
		m_vtStandString.push_back("h");
		m_vtStandString.push_back("i");
		m_vtStandString.push_back("j");
		m_vtStandString.push_back("k");
		m_vtStandString.push_back("l");
		m_vtStandString.push_back("m");
		m_vtStandString.push_back("n");
		m_vtStandString.push_back("o");
		m_vtStandString.push_back("p");
		m_vtStandString.push_back("q");
		m_vtStandString.push_back("r");
		m_vtStandString.push_back("s");
		m_vtStandString.push_back("t");
		m_vtStandString.push_back("u");
		m_vtStandString.push_back("v");
		m_vtStandString.push_back("w");
		m_vtStandString.push_back("x");
		m_vtStandString.push_back("y");
		m_vtStandString.push_back("z");
		m_vtStandString.push_back("braceleft");
		m_vtStandString.push_back("bar");
		m_vtStandString.push_back("braceright");
		m_vtStandString.push_back("asciitilde");
		m_vtStandString.push_back("Adieresis");
		m_vtStandString.push_back("Aring");
		m_vtStandString.push_back("Ccedilla");
		m_vtStandString.push_back("Eacute");
		m_vtStandString.push_back("Ntilde");
		m_vtStandString.push_back("Odieresis");
		m_vtStandString.push_back("Udieresis");
		m_vtStandString.push_back("aacute");
		m_vtStandString.push_back("agrave");
		m_vtStandString.push_back("acircumflex");
		m_vtStandString.push_back("adieresis");
		m_vtStandString.push_back("atilde");
		m_vtStandString.push_back("aring");
		m_vtStandString.push_back("ccedilla");
		m_vtStandString.push_back("eacute");
		m_vtStandString.push_back("egrave");
		m_vtStandString.push_back("ecircumflex");
		m_vtStandString.push_back("edieresis");
		m_vtStandString.push_back("iacute");
		m_vtStandString.push_back("igrave");
		m_vtStandString.push_back("icircumflex");
		m_vtStandString.push_back("idieresis");
		m_vtStandString.push_back("ntilde");
		m_vtStandString.push_back("oacute");
		m_vtStandString.push_back("ograve");
		m_vtStandString.push_back("ocircumflex");
		m_vtStandString.push_back("odieresis");
		m_vtStandString.push_back("otilde");
		m_vtStandString.push_back("uacute");
		m_vtStandString.push_back("ugrave");
		m_vtStandString.push_back("ucircumflex");
		m_vtStandString.push_back("udieresis");
		m_vtStandString.push_back("dagger");
		m_vtStandString.push_back("degree");
		m_vtStandString.push_back("cent");
		m_vtStandString.push_back("sterling");
		m_vtStandString.push_back("section");
		m_vtStandString.push_back("bullet");
		m_vtStandString.push_back("paragraph");
		m_vtStandString.push_back("germandbls");
		m_vtStandString.push_back("registered");
		m_vtStandString.push_back("copyright");
		m_vtStandString.push_back("trademark");
		m_vtStandString.push_back("acute");
		m_vtStandString.push_back("dieresis");
		m_vtStandString.push_back("notequal");
		m_vtStandString.push_back("AE");
		m_vtStandString.push_back("Oslash");
		m_vtStandString.push_back("infinity");
		m_vtStandString.push_back("plusminus");
		m_vtStandString.push_back("lessequal");
		m_vtStandString.push_back("greaterequal");
		m_vtStandString.push_back("yen");
		m_vtStandString.push_back("mu");
		m_vtStandString.push_back("partialdiff");
		m_vtStandString.push_back("summation");
		m_vtStandString.push_back("product");
		m_vtStandString.push_back("pi");
		m_vtStandString.push_back("integral");
		m_vtStandString.push_back("ordfeminine");
		m_vtStandString.push_back("ordmasculine");
		m_vtStandString.push_back("Omega");
		m_vtStandString.push_back("ae");
		m_vtStandString.push_back("oslash");
		m_vtStandString.push_back("questiondown");
		m_vtStandString.push_back("exclamdown");
		m_vtStandString.push_back("logicalnot");
		m_vtStandString.push_back("radical");
		m_vtStandString.push_back("florin");
		m_vtStandString.push_back("approxequal");
		m_vtStandString.push_back("Delta");
		m_vtStandString.push_back("guillemotleft");
		m_vtStandString.push_back("guillemotright");
		m_vtStandString.push_back("ellipsis");
		m_vtStandString.push_back("nonbreakingspace");
		m_vtStandString.push_back("Agrave");
		m_vtStandString.push_back("Atilde");
		m_vtStandString.push_back("Otilde");
		m_vtStandString.push_back("OE");
		m_vtStandString.push_back("oe");
		m_vtStandString.push_back("endash");
		m_vtStandString.push_back("emdash");
		m_vtStandString.push_back("quotedblleft");
		m_vtStandString.push_back("quotedblright");
		m_vtStandString.push_back("quoteleft");
		m_vtStandString.push_back("quoteright");
		m_vtStandString.push_back("divide");
		m_vtStandString.push_back("lozenge");
		m_vtStandString.push_back("ydieresis");
		m_vtStandString.push_back("Ydieresis");
		m_vtStandString.push_back("fraction");
		m_vtStandString.push_back("currency");
		m_vtStandString.push_back("guilsinglleft");
		m_vtStandString.push_back("guilsinglright");
		m_vtStandString.push_back("fi");
		m_vtStandString.push_back("fl");
		m_vtStandString.push_back("daggerdbl");
		m_vtStandString.push_back("periodcentered");
		m_vtStandString.push_back("quotesinglbase");
		m_vtStandString.push_back("quotedblbase");
		m_vtStandString.push_back("perthousand");
		m_vtStandString.push_back("Acircumflex");
		m_vtStandString.push_back("Ecircumflex");
		m_vtStandString.push_back("Aacute");
		m_vtStandString.push_back("Edieresis");
		m_vtStandString.push_back("Egrave");
		m_vtStandString.push_back("Iacute");
		m_vtStandString.push_back("Icircumflex");
		m_vtStandString.push_back("Idieresis");
		m_vtStandString.push_back("Igrave");
		m_vtStandString.push_back("Oacute");
		m_vtStandString.push_back("Ocircumflex");
		m_vtStandString.push_back("apple");
		m_vtStandString.push_back("Ograve");
		m_vtStandString.push_back("Uacute");
		m_vtStandString.push_back("Ucircumflex");
		m_vtStandString.push_back("Ugrave");
		m_vtStandString.push_back("dotlessi");
		m_vtStandString.push_back("circumflex");
		m_vtStandString.push_back("tilde");
		m_vtStandString.push_back("macron");
		m_vtStandString.push_back("breve");
		m_vtStandString.push_back("dotaccent");
		m_vtStandString.push_back("ring");
		m_vtStandString.push_back("cedilla");
		m_vtStandString.push_back("hungarumlaut");
		m_vtStandString.push_back("ogonek");
		m_vtStandString.push_back("caron");
		m_vtStandString.push_back("Lslash");
		m_vtStandString.push_back("lslash");
		m_vtStandString.push_back("Scaron");
		m_vtStandString.push_back("scaron");
		m_vtStandString.push_back("Zcaron");
		m_vtStandString.push_back("zcaron");
		m_vtStandString.push_back("brokenbar");
		m_vtStandString.push_back("Eth");
		m_vtStandString.push_back("eth");
		m_vtStandString.push_back("Yacute");
		m_vtStandString.push_back("yacute");
		m_vtStandString.push_back("Thorn");
		m_vtStandString.push_back("thorn");
		m_vtStandString.push_back("minus");
		m_vtStandString.push_back("multiply");
		m_vtStandString.push_back("onesuperior");
		m_vtStandString.push_back("twosuperior");
		m_vtStandString.push_back("threesuperior");
		m_vtStandString.push_back("onehalf");
		m_vtStandString.push_back("onequarter");
		m_vtStandString.push_back("threequarters");
		m_vtStandString.push_back("franc");
		m_vtStandString.push_back("Gbreve");
		m_vtStandString.push_back("gbreve");
		m_vtStandString.push_back("Idotaccent");
		m_vtStandString.push_back("Scedilla");
		m_vtStandString.push_back("scedilla");
		m_vtStandString.push_back("Cacute");
		m_vtStandString.push_back("cacute");
		m_vtStandString.push_back("Ccaron");
		m_vtStandString.push_back("ccaron");
		m_vtStandString.push_back("dcroat");

	}	// end of void CHYPost::SetDefault()

	CHYPost& CHYPost::operator=(const CHYPost& other)
	{
		if (this == &other)
		{
			return *this;
		}
		else 
		{
			Format.value		= other.Format.value;
			Format.fract		= other.Format.fract;		
			italicAngle.value	= other.italicAngle.value;
			italicAngle.fract	= other.italicAngle.fract;		
			underlinePosition	= other.underlinePosition;
			underlineThickness	= other.underlineThickness;
			isFixedPitch		= other.isFixedPitch;
			minMemType42		= other.minMemType42;
			maxMemType42		= other.maxMemType42;
			minMemType1			= other.minMemType1;			
			maxMemType1			= other.maxMemType1;
			m_vtStandString		= other.m_vtStandString;
			m_Format2			= other.m_Format2;
			m_Format25			= other.m_Format25;
		}		

		return *this;

	}	// end of CHYPost& CHYPost::operator=()

	std::string  CHYPost::FindNameByGID(unsigned long ulID)
	{
		if (Format.value == 1 && Format.fract == 0) {
			if (m_vtStandString.size() > ulID) {
				return m_vtStandString[ulID];
			}	
		}

		if (Format.value == 2 && Format.fract == 0){
			size_t st = m_Format2.vtGlyphNameIndex.size();
			if (ulID < m_Format2.vtGlyphNameIndex.size()){
				unsigned short usNameIndex = m_Format2.vtGlyphNameIndex[ulID];
				return m_vtStandString[usNameIndex];
			}
		}

		return "";			

	}	// end of std::string  CHYPost::FindNameByID()

	unsigned short CHYPost::FindNameIDByGID(unsigned long ulID)
	{
		if (Format.value == 1 && Format.fract == 0){
			if (ulID<258){
				return ulID;
			}			
		}

		if (Format.value == 2 && Format.fract == 0){
			size_t st = m_Format2.vtGlyphNameIndex.size();
			if (ulID < st){
				return m_Format2.vtGlyphNameIndex[ulID];				
			}
		}
		
		return -1;

	}	// end of unsigned short CHYPost::FindNameIDByGID()

	int CHYPost::FindGIDbyName(std::string  strPSName)
	{
		if (Format.value == 1 && Format.fract == 0) 
		{
			for (int i =0; i<258; i++)
			{
				if (strPSName == m_vtStandString[i])
				{
					return i;
				}
			}		
		}

		if (Format.value == 2 && Format.fract == 0) 
		{		
			for (size_t i=0; i<m_Format2.vtGlyphNameIndex.size(); i++)
			{

				if (strPSName == m_vtStandString[m_Format2.vtGlyphNameIndex[i]])
				{
					return (int)i;
				}
			}
		}

		return -1;

	}	// end of unsigned short CHYPost::FindGIDbyName()

	int CHYPost::InsertName(std::string strName)
	{			
		int iIndex = -1;
		size_t st = m_vtStandString.size();

		for (size_t i=0; i<st; i++)	{
			std::string strTmp = m_vtStandString[i];
			if (strTmp == strName){
				return iIndex = i;				
			}
		}

		if (iIndex == -1){
			m_vtStandString.push_back(strName);
			return m_vtStandString.size() -1;
		}

		return 0;

	}	// end of int CHYPost::InsertName()	

}

