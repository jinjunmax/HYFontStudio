
// HYFontSmartShaperView.h : CHYFontSmartShaperView ��Ľӿ�
//
#pragma once
#include "afxwin.h"
#include "afxcmn.h"

class CHYBezierCurver
{
public:
	CHYBezierCurver() {};
	~CHYBezierCurver(){};
public:
	std::vector<CHYPoint> vtCurverPt;
	int iCurverType;	// 0 ���α����� 1 ���α�����
};
class CHYFontSmartShaperView : public CFormView
{
public: // �������л�����
	CHYFontSmartShaperView();
	virtual ~CHYFontSmartShaperView();
	DECLARE_DYNCREATE(CHYFontSmartShaperView)

public:
	enum { IDD = IDD_HYFONTSMARTSHAPER_FORM };

	// ����
public:

	// ��д
public:
	virtual BOOL		PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void		DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual void		OnInitialUpdate(); // ������һ�ε���
	afx_msg void		OnDestroy();
	afx_msg void		OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL		OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg LRESULT		OnThumSelet(WPARAM waram, LPARAM lparam);
	afx_msg LRESULT		OnPopWnd(WPARAM waram, LPARAM lparam);
	afx_msg	LRESULT 	OnUpdateAllViews(WPARAM waram, LPARAM lparam);
	afx_msg void		GetDrawParam(UINT iID);
	afx_msg void		OnBnClickedMnSeekBtn();

	// ���ֿ�
	afx_msg void		OnFileOpen();
	// �����ֿ�
	afx_msg void		OnFileSave();
	// ��������
	afx_msg void		OnFntEncodeParam();
	afx_msg void		OnBnClickedMnFntstBtn();
	afx_msg void		OnBnClickedMnCvtsBtn();
	// ȫ��ת��
	afx_msg void		OnBnClickedMnCvtmBtn();
	afx_msg void		OnIkMerge();
	afx_msg void		OnIkSplit();
	afx_msg void		OnCbnSelchangeMnVmtypCmb();
	afx_msg void		OnFntCvtpramMu();	
	afx_msg void		OnBnClickedMnCvtTtfRd();
	afx_msg void		OnBnClickedMnCvtOtfRd();
	// �Ͽ�
	afx_msg void		OnFntMerge();
	afx_msg void		OnBnSetfocusMnCvtTtfRd();
	afx_msg void		OnBnSetfocusMnCvtOtfRd();

	afx_msg void		OnSvgCvtMu();
	afx_msg void		OnSvgEotMu();
	afx_msg void		OnToWoffMu();
	afx_msg void		OnDropFiles(HDROP hDropInfo);
	afx_msg void		OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void		OnChaConvert();
	afx_msg void		OnFlnm2cdnm();
	afx_msg void		OnCdnm2flnm();
	afx_msg void		OnBnClickedMnCutmBtn();
	afx_msg void		OnMnVfntInf();
	afx_msg void		OnMnCha();
	afx_msg void		OnMnFntinf();
	// ��������ö��ŷָ�
	afx_msg void		OnMnCodedot();
	// ���ɼ��ֿ�
	afx_msg void		OnMnMkjf();
	// ��������ֿ�
	afx_msg void		OnFntVadh();
	afx_msg void		OnMnAduni();
	afx_msg void		OnMkvntMn();	
	afx_msg void		OnMnJszCodetchar();
	// �������һЩ���Ի����ı���Ϣ
	afx_msg void		OnChnvert();
	afx_msg void		OnVerifyvntMn();
	afx_msg void		OnTtcTtctofont();
	afx_msg void		OnTtcFonttottc();
	afx_msg void		OnCharsCompareMn();
	// �����ֱ���ȡsubsetfont
	afx_msg void		OnFntExtrchr();
	// ���������ȡsubsetfont
	afx_msg void		OnFntExtrcd();
	// ��������������ȡsubsetfont
	afx_msg void		OnFntExtrname();
	afx_msg void		OnFntRpNodef();
	afx_msg void		OnFntRpTwfnt();
	afx_msg void		OnNoboxMnu();
	//�����ֿ�postscript name
	afx_msg void		OnFntRsetname();
	//�����ֿ�postscript name
	afx_msg void		OnMnExptname();
	// ���ڵ��������ַ���AdvanceWidth	
	afx_msg void		OnFlnm2numnm();
	
	
	afx_msg void		OnMnCmptxt();
	afx_msg void		OnMnChkerttf();
	afx_msg void		OnMnChkerotf();
	// ����Emoji�ֿ�
	afx_msg void		OnEmojiExport();
	// ����Emoji�ֿ�	
	afx_msg void		OnEmojiMk();
	// �������
	afx_msg void		OnMnCodemap();
	// ���ñ���
	afx_msg void		OnFntResetcode();
	virtual BOOL		PreTranslateMessage(MSG* pMsg);
	// ������otf -> CIDOTF
	afx_msg void		OnFntBudcidotf();
	// �޸�Glyphs3�汾�ɱ��ֿ������������OVERLAP_SIMPLE��־
	afx_msg void		OnG3fxMn();
	// ���ֿ���ַ������м�飬��Ҫ�Ǽ��
	afx_msg void		OnMnChkcharset();

	// ʵ��
public:
#ifdef _DEBUG
	CHYFontSmartShaperDoc* GetDocument() const;
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	void				InitCtrl();
	void				UnInitCtrl();
	void				InitWindows();
	void				UnitWindows();
	void				InitScrollBar();
	void				SetDefault();

	void				InitHYShowListWnd();
	void				UninitHYShowListWnd();
	void				UpdateHYThumWnds(int iStart);
	void				FlashHYThumWnds();
	void				SetSelectHYThumWnds(int iSltIndex);

	void				SetLayout();
	void 				LoadFontThreadFun();
	void 				LoadFontStepThreadFun();

	void				ShowSrcWnd(int iGid);
	void				ShowDstWnd(int iGid);

	void				SetDrawParam();

	BOOL				TTFConvert(CHYGlyph& SrcGryph, CHYGlyph& DstGryph, CHYHead& SrcHead);
	BOOL				OTFConvert(CHYGlyph& SrcGryph, CHYGlyph& DstGryph, CHYHead& SrcHead);
	void				OTF2TTF(CHYGlyph& otfGryph, CHYGlyph& ttfGryph);

	BOOL				NameTranscodeToWCHAR(CHYName& name);
	void				MakeFontSaveInfo();
	void				ScaleGlyph(CHYGlyph& Gryph, float fScale);
	void				SetAdWidth(UINT adw, CHYGlyph& Glyphs);
	// ��ȡ�Ӽ�����
	void				GetFntSubset(char* strSrcFnt, std::vector<unsigned long> vtUni, const char* strDstFnt);
	void				GetOTFSawtooth(std::vector<CHYPoint>& vtInPnts, std::vector<CHYPoint>& vtOutPnts);
	void				Smooth(CHYGlyph& inglyph, CHYGlyph& Outglyph);
	void				fitting(std::vector<CHYPoint>& vtInPnts, std::vector<CHYPoint>& vtOutPnts);
	void				SmoothAndfitting(std::vector<CHYPoint>& InvtPnt, std::vector<CHYPoint>& OutvtPnt);

	// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()

public:
	CFontShowWnd*					m_pFntSrcWnd;
	CFontShowWnd*					m_pFntDstWnd;
	CCompareFontWnd*				m_pCompareFontWnd;
	CFontZoomWnd*					m_pFntSrcZoomWnd;
	CFontZoomWnd*					m_pFntDstZoomWnd;
	CFontCmprZoomWnd*				m_pCmprZoomWnd;

	std::vector<CThumFontWnd*>		m_vtThumFnontWnd;
	int								m_iThumClumnNums;
	int								m_iThumRowNums;
	CScrollBar						m_srblThum;
	CProgressCtrl					m_prssProgress;
	int								m_iSelectGID;
	CString							m_strFontFileName;
	HANDLE							m_hdLoadFontThread;
	HANDLE							m_hdLoadFontStepThread;	
	HYFONTCODEC::CHYFontCodec		m_FontDeCodec;
	HYFONTCODEC::CHYFontCodec		m_FontEnCodec;
	HANDLE							m_hLoadFontEvent;
	CComboBox						m_cmbSeek;
	CString							m_strSeek;
	
	
#ifndef _DEBUG  // HYFontSmartShaperView.cpp �еĵ��԰汾
	inline CHYFontSmartShaperDoc* CHYFontSmartShaperView::GetDocument() const
	{
		return reinterpret_cast<CHYFontSmartShaperDoc*>(m_pDocument);
	}
#endif	
	
	afx_msg void OnMnExpname();
};