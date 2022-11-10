#pragma once

#include "FontExtract.h"

struct GroupRecord
{
	unsigned long		startCode;
	unsigned long		endCode;
	int 				startGID;
	bool				contiguous;
};

// CHYMakeJFFontDlg 对话框
class CHYMakeJFFontDlg : public CDialog
{
	DECLARE_DYNAMIC(CHYMakeJFFontDlg)

public:
	CHYMakeJFFontDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CHYMakeJFFontDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FNT_MKJF_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedMjfOpenBtn();

public:
	bool	DecodeW(CString strWFontFile, CHYFontCodec& FontWDecode);	
	void	MakeJF(CString strWFontFile, CString strJPath, CString strFPath);
	void	MakeJ(CHYFontCodec& FontWDecode, 
				  CString strJPath,
				  std::vector<GlyfData>& vtJ,
				  std::vector<GlyfData>& vtXiWen);
	void	MakeF(CHYFontCodec& FontWDecode, 
					CString strFPath,
					std::vector<GlyfData>& vtF, 
					std::vector<GlyfData>& vtXiWen);

	void	MakeName(CHYName& inName, CHYName& outName, int iJFFlag);
	void	BulidFont(CHYFontCodec& FontDecodec,
					  char* strNewTTFFile,
					  std::vector<GlyfData>& vtGlyfData);

	int		SetCheckSumAdjustment(const char* pFontFile);
	void	DecodeTableDirectory(CHYFontCodec& FntDecodec,FILE* pFntFile);

	void	WriteTableEntry(FILE* pFile);
	void	BulidGlyph(FILE* pCreatFile, std::vector<GlyfData>& vtGlyfData);
	void	BulidLOCAL(FILE* pCreatFile, std::vector<GlyfData>& vtGlyfData);
	void	BulidHead(CHYFontCodec& FontCodec, FILE* pCreatFile, std::vector<GlyfData>& vtGlyfData);
	void	BulidMAXP(CHYFontCodec& FontCodec, FILE* pCreatFile, std::vector<GlyfData>& vtGlyfData);
	void	BulidCMAP(FILE* pCreatFile, std::vector<GlyfData>& vtGlyfData);
	void	EncodeCmapFmt0(FILE* pFontFile, CMAP_TABLE_ENTRY& entry, std::vector<char>& vtCmap);
	void	EncodeCmapFmt4(FILE* pFontFile, CMAP_TABLE_ENTRY& entry, std::vector<char>& vtCmap);
	int		NextGroup(int ix, int eov, GroupRecord& group);
	void	EncodeCmapFmt12(FILE* pFontFile, CMAP_TABLE_ENTRY& entry, std::vector<char>& vtCmap);
	void	MakeHYCodeMap(std::vector<GlyfData>& vtGlyfData);
	void	BulidHHEA(CHYFontCodec& FontCodec, FILE* pCreatFile, std::vector<GlyfData>& vtGlyfData);

	int		FindXmin(std::vector<GlyfData>& vtGlyfData);
	int		FindXmax(std::vector<GlyfData>& vtGlyfData);
	int		FindYmin(std::vector<GlyfData>& vtGlyfData);
	int		FindYmax(std::vector<GlyfData>& vtGlyfData);
	int		FindMinLeftSideBearing(std::vector<GlyfData>& vtGlyfData);
	int		FindMinRightSideBearing(std::vector<GlyfData>& vtGlyfData);
	unsigned short 	 GetAdvancMaxWidth(std::vector<GlyfData>& vtGlyfData);

	void	BulidHMTX(FILE* pCreatFile, std::vector<GlyfData>& vtGlyfData);
	void	Bulidname(FILE* pCreatFile);
	void	BulidVHEA(CHYFontCodec& FontCodec, FILE* pCreatFile, std::vector<GlyfData>& vtGlyfData);
	void	BulidVMTX(FILE* pCreatFile, std::vector<GlyfData>& vtGlyfData);
	void	BulidPOST(CHYFontCodec& FontCodec, FILE* pCreatFile, std::vector<GlyfData>& vtGlyfData);
	

	bool	CheckGid(std::vector<int>& vtgid, int iGid);

	void	CountFontBound(short& xMin,
							short& yMin,
							short& xMax,
							short& yMax,
							std::vector<GlyfData>& vtglfData);

	bool	Extraglyf(	HYFONTCODEC::CHYFontCodec& FontWDecode,
					std::vector<GlyfData>& vtGlyfData,
					std::vector<unsigned long>&	vtUni);

	void	RemoveHan(HYFONTCODEC::CHYFontCodec& FontWDecode,
					std::vector<GlyfData>& vtXiWen
					);

protected:
	std::vector<unsigned long>	m_vtFUni; // 繁体
	std::vector<unsigned long>	m_vtJUni; // 简体
	std::vector<unsigned long>	m_vtWUni; // W
	CHYCodeMap				m_HYCodeMap;
	CHYCmap					m_HYNewCmap;
	CHYTableDirectory		m_HYTbDirectory;
	CHYName					m_HYNewName;
};
