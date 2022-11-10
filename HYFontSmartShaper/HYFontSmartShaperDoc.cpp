
// HYFontSmartShaperDoc.cpp : CHYFontSmartShaperDoc 类的实现
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


// CHYFontSmartShaperDoc 构造/析构

CHYFontSmartShaperDoc::CHYFontSmartShaperDoc()
{
	// TODO: 在此添加一次性构造代码

}

CHYFontSmartShaperDoc::~CHYFontSmartShaperDoc()
{
}

BOOL CHYFontSmartShaperDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CHYFontSmartShaperDoc 序列化

void CHYFontSmartShaperDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}


// CHYFontSmartShaperDoc 诊断

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


// CHYFontSmartShaperDoc 命令
