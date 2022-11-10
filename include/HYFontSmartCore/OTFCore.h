#pragma once

#include "IKToOTF.h"



class AFX_EXT_CLASS COTFCore : public CIKToOTF
{
public:
	COTFCore(void);
	~COTFCore(void);

	int			OTFTOOTF(HY_POINT* pSrcHYP, int iSrctPointsSize, HY_POINT* pDstHYP, int& iDsttPointsSize, HEAD_TABLE* pSrcTTFHead, TTF_CONVTER_PARAM* pTTFParam);
	int			OTFTOTTF(HY_POINT* pSrcHYP, int iSrctPointsSize, HY_POINT* pDstHYP, int& iDsttPointsSize, HEAD_TABLE* pSrcTTFHead, TTF_CONVTER_PARAM* pTTFParam);
	int			TTFTOOTF(HY_POINT* pSrcHYP, int iSrctPointsSize, HY_POINT* pDstHYP, int& iDsttPointsSize, HEAD_TABLE* pSrcTTFHead, TTF_CONVTER_PARAM* pTTFParam);
	int			IKTOOTF(PIK_HEAD pIKHead_In, PIK_CHARACTER pIKCharacter_In, HY_POINT* pDstHYP, int& iDsttPointsSize, TTF_CONVTER_PARAM* pTTFParam);

};
