#pragma once

#include "OTFToTTF.h"
class AFX_EXT_CLASS CIKToOTF : public COTFToTTF
{
public:
	CIKToOTF(void);
	virtual ~CIKToOTF(void);

	int  IKTest(PIK_HEAD pIKHead_In, PIK_CHARACTER pIKCharacter_In, HY_POINT* pDstHYP, int& iDsttPointsSize, TTF_CONVTER_PARAM* pTTFParam);
};
