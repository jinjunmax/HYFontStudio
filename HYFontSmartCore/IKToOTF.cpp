#include "StdAfx.h"
#include "IKToOTF.h"

CIKToOTF::CIKToOTF(void)
{

}	// end of CIKToOTF::CIKToOTF()

CIKToOTF::~CIKToOTF(void)
{

}	// end of CIKToOTF::~CIKToOTF()

int CIKToOTF::IKTest(PIK_HEAD pIKHead_In, PIK_CHARACTER pIKCharacter_In, HY_POINT* pDstHYP, int& iDsttPointsSize, TTF_CONVTER_PARAM* pTTFParam)
{
	int iEM = pIKHead_In->tagIKFontInf.usBodySize/2;
	pDstHYP[0].flag = OTF_POINT_FLG_START;
	pDstHYP[0].x	= (pIKHead_In->tagIKFontInf.usBodySize - iEM)/2;
	pDstHYP[0].y	= (pIKHead_In->tagIKFontInf.usBodySize - iEM)/2;

	pDstHYP[1].flag	= OTF_POINT_FLG_ANCHOR;
	pDstHYP[1].x	= pDstHYP[0].x;
	pDstHYP[1].y	= pDstHYP[0].y + iEM;


	pDstHYP[2].flag	= OTF_POINT_FLG_ANCHOR;
	pDstHYP[2].x	= pDstHYP[1].x + iEM;
	pDstHYP[2].y	= pDstHYP[1].y;

	pDstHYP[3].flag	= OTF_POINT_FLG_ANCHOR;
	pDstHYP[3].x	= pDstHYP[2].x;
	pDstHYP[3].y	= pDstHYP[2].y - iEM;

	pDstHYP[4].flag	= OTF_POINT_FLG_END;
	pDstHYP[4].x	= pDstHYP[0].x;
	pDstHYP[4].y	= pDstHYP[0].y;

	iDsttPointsSize = 5;

	return HY_NOERROR;
}	// end of int CIKToOTF::IKTest()