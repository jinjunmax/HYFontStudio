
// HYFontSmartShaper.h : HYFontSmartShaper Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// CHYFontSmartShaperApp:
// �йش����ʵ�֣������ HYFontSmartShaper.cpp
//

class CHYFontSmartShaperApp : public CWinApp
{
public:
	CHYFontSmartShaperApp();
	virtual ~CHYFontSmartShaperApp();

// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// ʵ��
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()

public:
	ULONG_PTR			m_gdiplusToken;	

};

extern CHYFontSmartShaperApp theApp;
