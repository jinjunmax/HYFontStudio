#pragma once

//#include "TTFFontCodec.h"
#define	  MAX_GLYFBUF	4608
class AFX_EXT_CLASS CTTFExt_GLFYITEM
{
public:
	CTTFExt_GLFYITEM();
	virtual ~CTTFExt_GLFYITEM();

	void	SetDefault();
	void	operator=(const CTTFExt_GLFYITEM& other);
	
	void	SetGlyfBuf(PBYTE pGlyfbuf, int iBufSize);

public:
	ULONG	m_ulUNicode;
	int		m_iLeftside;
	int		m_iAdvanceWidth;
	BYTE	m_pGlyfBuf[MAX_GLYFBUF];
	int		m_iGlyfBufSize;
};

typedef std::vector<CTTFExt_GLFYITEM> CTTFExt_GLFYITEMS;
class AFX_EXT_CLASS CTTFExt_GLFYList : public CTTFExt_GLFYITEMS
{
public:
	CTTFExt_GLFYList(void);
	virtual ~CTTFExt_GLFYList(void);

public:
	void				SetDefault();
	void				operator=(const CTTFExt_GLFYList& other);

public:
	CTTFExt_GLFYITEM*		FindItem(int iFocusedItem);
	CTTFExt_GLFYITEM*		FindItembyUncoide(unsigned long lUnicode);
	int						FindIndexbyUncoide(unsigned long lUnicode);

	int					InsertItem(CTTFExt_GLFYITEM& info, int iFocusedItem = -1);
	int					AddLast(CTTFExt_GLFYITEM& info);
	int					RemoveItem(int iFocusedItem);
	int					SetItem(CTTFExt_GLFYITEM& info, int iFocusedItem);

public:
	int					NumItems()							{ return size(); }
	BOOL				IsNull()							{ return empty(); }
	void				QuickSortbyUnicode();

protected:
	BOOL				IsItemValid(int iFocusedItem);	
};
////////////////////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CTTFExtract
{
public:
	CTTFExtract(void);
	virtual ~CTTFExtract(void);

	BOOL		ExtractFont(CString strSrcFileName, CString strDstFileName, ULONG* pUinCode, UINT uiUincodeLen);
	BOOL		MakeMorisawaTTF(CString strSrc1FontName, CString strSrc2FontName, CString strDestFontName);
	BOOL		MergeFont(CString strSrc1FontName, DWORD dwSrc1TableFlag, CString strSrc2FontName, DWORD dwSrc2TableFlag, CString strDestFontName);
	BOOL		ImportGSUB(CString strFontName, CString strGSUBFile);
	int			LoadOTF_GSUBData(CString strGSUBTableFileName);
protected:
	void		SetDefault();

	// 萃取字库工程，必须解析的表LOCAL,CMAP,HMTX,HEAD,HEEA,MAXP
	// 萃取字库工程，必须读取的表GLYH,POST,OS/2,NAME
	// 萃取字库工程，必须生成的表LOCAL,CMAP,HMTX,GLYH,HEAD,HEEA,POST,OS/2,NAME,MAXP

	// 1 根据传进来的UNICODE，在CMAP表中确定GID。
	// 2 根据GID在Local表确定Glyp的数据
	// 3 将GLYH数据拷贝到CTTFExt_GLFYList数组中
	// 4 hmtx；
	// 5 生成各个表
	// 6 生成抽取后的字库

	int 		AnalyseTABLE_DIRECTORY( PBYTE pBuf, CTTFFont* pTTF);

	int 		AnalyseTTF_TABLE( PBYTE pBuf, CTTFFont* pTTF );	
	int			AnalyseHEAD_TABLE( PBYTE pBuf, ULONG uLength, CTTFFont* pTTF );
	int			AnalyseHHEA_TABLE( PBYTE pBuf, ULONG uLength, CTTFFont* pTTF );
	int			AnalyseMAXP_TABLE( PBYTE pBuf, ULONG uLength, CTTFFont* pTTF );
	int			AnalyseLOCAL_TABLE( PBYTE pBuf, ULONG uLength, CTTFFont* pTTF);
	int			AnalyseCMAP_TABLE( PBYTE pBuf, ULONG uLength, CTTFFont* pTTF );
	int			AnalyseHMTX_TABLE( PBYTE pBuf, ULONG uLength, CTTFFont* pTTF );
	int			AnalysePOST_TABLE( PBYTE pBuf, ULONG uLength, CTTFFont* pTTF );

protected:
	BOOL		Extract(ULONG* pUinCode, UINT uiUincodeLen);
	void		SeparateTableData(PBYTE pSrcFontDate, DWORD dwTableFlag);

protected:
	int			MakeTTFTable();
	int			MakeMAXP_TABLE( CTTFFont* pTTF );
	int			MakeCMAP_TABLE( CTTFFont* pTTF );
	int			MakeHHEA_TABLE( CTTFFont* pTTF );
	int			MakeHMTX_TABLE( CTTFFont* pTTF );
	int			MakePOST_Table( CTTFFont* pTTF );
	int			MakeRomaCmapTable(CMAP_TB_ENTRY* pCMAPEntry);
	int			MakeUnicodeFormat4CmapTable(CMAP_TB_ENTRY* pCMAPEntry, BOOL& bIsNeedFormat12);
	int			MakeUnicodeFormat12CmapTable(CMAP_TB_ENTRY* pCMAPEntry);

protected:	
	int			BuildTablieDirecory(PBYTE pDirecBuf, DWORD dwDirecBufOffset, PBYTE tag, ULONG checkSum, ULONG offset, ULONG length);
	int			BuildMAXPTable(MAXP_TB* pMaxp);
	int			BuildCMAPTable(CMAP_TB* pCmapTB);
	int			BuildHHEATable(HHEA_TB* pHheaTB);
	int			BuildHmtxTable(HMTX_TB* pHmtxTB);
	//直接将Glyh数据写入文件，因为中文字数过大无法一次写入内存
	int			BuildGlyhTable(CTTFFont* pTTFont);
	int			BuildPostTable(POST_TB* pPostTB);

	void		BulidFontBaseForExtract(CString strPath);
	void		BulidFontBaseForImport(CString strPath);
	void		BulidFontBaseForMorisawaFont(CString strPath);
	void		BulidMergeFontBase(CString strPath);

	ULONG		CalcTableChecksum(ULONG *Table, ULONG Length);
	int			BuildHeadTable(HEAD_TB* pHeadTB);
	int			BuildGaspTable(GASP_TB* pGasp);
	int			BuildGSUBTable(COTF_ScriptList* pScriptList,	COTF_FeaturList* pFeaturlst, COTF_LookUpList* pLookUpLst);
	int			BuildScriptList(COTF_ScriptList* pScriptList,	PBYTE		&pScriptLstBuf, unsigned short& usScriptlstBufLen);
	int			BuildFeaturList(COTF_FeaturList* pFeaturtList,	PBYTE		&pFearturLstBuf, unsigned short& usFeaturlstBufLen);
	int			BuildLookuoList(COTF_LookUpList* pLookupList,	PBYTE		&pLookupLstBuf, unsigned short& usLookuplstBufLen);
protected:
	CTTFFont			m_srcTTF;
	CTTFExt_GLFYList	m_szGlyfLst;

	PBYTE				m_pHeadBuf;
	DWORD 				m_dwHeadlen;
	PBYTE				m_pHheaBuf;
	DWORD 				m_dwHhealen;
	PBYTE				m_pHmtxBuf;
	DWORD 				m_dwHmtxLen;
	PBYTE				m_pCmapBuf;
	DWORD				m_dwCmapLen;
	PBYTE				m_pGlyfBuf;
	DWORD				m_dwGlyfLen;
	PBYTE				m_pLocaBuf;
	DWORD				m_dwLocaLen;
	PBYTE				m_pNameBuf;
	DWORD				m_dwNameLen;
	PBYTE				m_pPostBuf;
	DWORD				m_dwPostLen;
	PBYTE				m_pOS2Buf;
	DWORD				m_dwOS2Len;
	PBYTE				m_pMAXPBuf;
	DWORD				m_dwMaxpLen;
	PBYTE				m_pCVTBuf;
	DWORD				m_dwCVTLeng;
	PBYTE				m_pFPGMBuf;
	DWORD				m_dwFPGMLeng;
	PBYTE				m_pGaspBuf;
	DWORD				m_dwGaspLeng;
	PBYTE				m_pPREPBuf;
	DWORD				m_dwPREPLeng;
	PBYTE				m_pGSUB;
	DWORD				m_dwGSUBLeng;
	PBYTE				m_pVHEA;
	DWORD				m_dwVHEALeng;
	PBYTE				m_pVMTX;
	DWORD				m_dwVMTXLeng;
	PBYTE				m_pBASE;
	DWORD				m_dwBASELeng;
	PBYTE				m_pDirecoryBuf;
	DWORD				m_dwDirectoryLen;
	PBYTE				m_pCFF;
	DWORD				m_dwCFFLeng;
	PBYTE				m_pVORG;
	DWORD				m_dwVORGLeng;

	DWORD				m_dwTableFlag;
	COTF_ScriptList		m_szGSUBScriptList;
	COTF_FeaturList		m_szGSUBFeatureList;
	COTF_LookUpList		m_szGSUBLookupList;
};
