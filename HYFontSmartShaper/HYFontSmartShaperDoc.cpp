
// HYFontSmartShaperDoc.cpp : CHYFontSmartShaperDoc ���ʵ��
//

#include "stdafx.h"
#include "HYFontSmartShaper.h"

#include "HYFontSmartShaperDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CHYFontSmartShaperDoc

IMPLEMENT_DYNCREATE(CHYFontSmartShaperDoc, CDocument)

BEGIN_MESSAGE_MAP(CHYFontSmartShaperDoc, CDocument)
END_MESSAGE_MAP()


// CHYFontSmartShaperDoc ����/����

CHYFontSmartShaperDoc::CHYFontSmartShaperDoc()
{
	// TODO: �ڴ����һ���Թ������

}

CHYFontSmartShaperDoc::~CHYFontSmartShaperDoc()
{
}

BOOL CHYFontSmartShaperDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: �ڴ�������³�ʼ������
	// (SDI �ĵ������ø��ĵ�)

	return TRUE;
}




// CHYFontSmartShaperDoc ���л�

void CHYFontSmartShaperDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: �ڴ���Ӵ洢����
	}
	else
	{
		// TODO: �ڴ���Ӽ��ش���
	}
}


// CHYFontSmartShaperDoc ���

#ifdef _DEBUG
void CHYFontSmartShaperDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CHYFontSmartShaperDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CHYFontSmartShaperDoc ����
