
// HYFontSmartShaperDoc.h : CHYFontSmartShaperDoc ��Ľӿ�
//


#pragma once


class CHYFontSmartShaperDoc : public CDocument
{
protected: // �������л�����
	CHYFontSmartShaperDoc();
	DECLARE_DYNCREATE(CHYFontSmartShaperDoc)

// ����
public:

// ����
public:

// ��д
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// ʵ��
public:
	virtual ~CHYFontSmartShaperDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
};


