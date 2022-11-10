// HYImageConvertFont.h : main header file for the HYImageConvertFont DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CHYImageConvertFontApp
// See HYImageConvertFont.cpp for the implementation of this class
//

class CHYImageConvertFontApp : public CWinApp
{
public:
	CHYImageConvertFontApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
