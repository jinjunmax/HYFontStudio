#pragma once

#include "MKVNTSheet.h"
// CMKVNTDlg 对话框

class CMKVNTDlg : public CDialog
{
	DECLARE_DYNAMIC(CMKVNTDlg)

public:
	CMKVNTDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMKVNTDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FNT_MKVNT_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	void	InitParam();
	void	UninitParam();

public:	
	HYFONTCODEC::CHYFontCodec	m_MKVFntCodec;
	CMKVNTSheet					m_sheet;	

public:
	virtual						BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedFntMkvntMkBtn();

public:	
	void		DecodeMastersFont();
	bool		CheckMaster();
	void		makefvar(CHYfvar& fvr);
	void		CountAxesValue(CString strAxisTag, HYFIXED& outminValue, HYFIXED& outdefaultValue, HYFIXED& outmaxValue);

	void		makeSTAT(CHYSTAT& Stat);

	void		makegvar(CHYgvar& gvar);
	void		MakeGlyphVariationData(glyphVariationData& gvartnData, CVrMasters& Masters, int iGlyphID);
	void		MakeTupleVariationHeader(glyphVariationData& gvartnData, CVrMaster& Master, short sSharTuple, int iGlyphID);
	void		MakeSerializeddata(glyphVariationData& gvartnData, CVrMasters& Masters, int iGlyphID);
	void		PackedDeltas(std::vector<unsigned char>& outData, std::vector<short>& invtOffset);
	bool		MakeMasterDeltas(CVrMasters& Masters);
	void		MakeFinalGlpyhDelta(CVrMaster& defaultMaster, CVrMaster& inMaster);
	void		MakeGlphysDelta(CVrMaster& defaultMaster, CVrMaster& inMaster);
	void		MakeGlphyDelta(CHYGlyph& defultGlyph, CHYGlyph &inGlyph, CVrGlyph& outVrGlyph);	
	void		AddPhantomPoint(CHYGlyph& defultGlyph, CHYGlyph& inGlyph, CVrGlyph& outVrGlyph);

	void		MakeSharedTuple(std::vector<CHYTuple>& sharedTuples, CVrMasters& Masters);
	void		Normalizedcoordinate(CVrMasters& Masters);
	float		Normalize(float fDefault, float fMax, float fMin, float fInValue);

	void		makeFontName();
	void		makeEnglishName(unsigned short	platformID, unsigned short	encodingID, unsigned short	languageID, std::vector<NAMERECORD>	& vtNameRecord);
	void		makeChineseName(unsigned short	platformID, unsigned short	encodingID, unsigned short	languageID, std::vector<NAMERECORD>	& vtNameRecord);
	void		makeVrName(unsigned short	platformID, unsigned short	encodingID, unsigned short	languageID, std::vector<NAMERECORD>	& vtNameRecord);
	
};
