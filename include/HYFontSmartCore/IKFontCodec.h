#pragma once

#include "IKFont.h"
class AFX_EXT_CLASS CIKFontCodec
{
public:
	CIKFontCodec(void);
	virtual ~CIKFontCodec(void);

	// 新的读取码表的方式
	int						MakeUnicode();	
	int 					EncodeIK(CString strFileName, CIKFont* pIKFont);

protected:
	void  					Build_IK(CIKFont* pIKFont);	
	unsigned long 			BuildCharBuffer(PIK_CHARACTER pCharacter, short sBaseLine);

public:
	CIKFont* 				DecodeIK(const CString& strFileName);
protected:
	int						AnalyseDataFormat(unsigned char* pBuf);
	int						ConvtertBigTOLittle(unsigned char* pBuf, DWORD dwBufLeng);
	int						Analyse_HEAD(unsigned char* pBuf, DWORD& dwOffset, PIK_FONT pIK);
	int						Analyse_CHARACTER(unsigned char* pBuf, DWORD& dwOffset, PIK_FONT pIK);
public:
	CIKFont*				GetIKFont()						{ return &m_IKFont;}
	CHYMapList*				GetHYMapList()					{ return &m_lstHYMaplst;}


protected:
	void			InitParam();
	void			UninitParam();

	void			InitIKBuf();
	void			UinitIKBuf();

	void			InitVectorCharBuffer();
	void			UinitVectorCharBuffer();

	void			InitUnicodeLst(int iLstLen);
	void			UninitUnicodeLst();

public:
	PDWORD						m_pUnicodeLst;
	DWORD						m_dwUnicodLstLen;

protected:
	CIKFont						m_IKFont;
	PBYTE 						m_pIKBuf;
	DWORD						m_dwIKBufLen;
	DWORD						m_dwIKOffset;
	CHYMapList					m_lstHYMaplst;		//读取码表方式改变，此种方式将淘汰

	std::vector<PBYTE>			m_vtBuildCharBuffer;
	std::vector<unsigned long>	m_vtCharSize;
	std::vector<unsigned long>	m_vtCharNum;

	PBYTE						m_pHeadBuf;
	DWORD						m_dwHeadBufSize;

	
};	
	