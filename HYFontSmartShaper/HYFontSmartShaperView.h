
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
	afx_msg void		OnEnChangeMnAdwEdt();
	afx_msg void		OnEnChangeMnAdhEdt();
	afx_msg void		OnMnCmptxt();
	afx_msg void		OnMnChkerttf();
	afx_msg void		OnMnChkerotf();
	// ����Emoji�ֿ�
	afx_msg void OnEmojiExport();
	// ����Emoji�ֿ�	
	afx_msg void OnEmojiMk();
	// �������
	afx_msg void OnMnCodemap();


	virtual BOOL		PreTranslateMessage(MSG* pMsg);

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
	void				cu2qu(std::vector<CHYPoint>& vtotfPT, std::vector<CHYBezierCurver>& vtTTFCurvers);
	bool				cubic_approx_spline(std::vector<CHYPoint>& vtotfPT, std::vector<CHYPoint>& vtttfPT, int n, double tolerance, double _2_3 = 0.666666666666666666666666667);
	void				cubic_approx_control(double t, std::complex<double>& p0, std::complex<double>& p1, std::complex<double>& p2, std::complex<double>& p3, std::complex<double>& outControl);
	bool				cubic_farthest_fit_inside(std::complex<double>& p0, std::complex<double>& p1, std::complex<double>& p2, std::complex<double>& p3, double tolerance, double _2_3 = 0.666666666666666666666666667);
	bool				cubic_approx_quadratic(std::vector<CHYPoint>& vtotfPT, std::vector<CHYPoint>& vtttfPT, double tolerance, double _2_3 = 0.666666666666666666666666667);
	bool				calc_intersect(std::complex<double>& a, std::complex<double>& b, std::complex<double>& c, std::complex<double>& d, std::complex<double>& intr);
	float				dot(std::complex<double>& v1, std::complex<double>& v2);
	void				split_cubic_into_n_iter(CHYPoint& a, CHYPoint& b, CHYPoint& c, CHYPoint& d, int n, std::vector<CHYBezierCurver>& vtOTFCurvers);
	void				makeCFFCurver(std::complex<double>& p1, std::complex<double>& p2, std::complex<double>& p3, std::complex<double>& p4, CHYBezierCurver& otfCurver);
	void				makeTTFCurver(std::complex<double>& p1, std::complex<double>& p2, std::complex<double>& p3, CHYBezierCurver& ttfCurver);
	void				split_cubic_into_two(CHYPoint& a, CHYPoint& b, CHYPoint& c, CHYPoint& d, std::vector<CHYBezierCurver>& vtOTFCurvers);
	void				split_cubic_into_three(CHYPoint& a, CHYPoint& b, CHYPoint& c, CHYPoint& d, std::vector<CHYBezierCurver>& votfFCurvers, float _27 = 0.03703703703703703703703703703704);
	void				split_cubic_into_four(CHYPoint& a, CHYPoint& b, CHYPoint& c, CHYPoint& d, std::vector<CHYBezierCurver>& vtOTFCurvers);
	void				split_cubic_into_six(CHYPoint& a, CHYPoint& b, CHYPoint& c, CHYPoint& d, std::vector<CHYBezierCurver>& vtOTFCurvers, float _27 = 0.03703703703703703703703703703704);
	void				_split_cubic_into_n_gen(CHYPoint& a1, CHYPoint& b1, CHYPoint& c1, CHYPoint& d1, int n, std::vector<CHYBezierCurver>& vtOTFCurvers);
	void				calc_cubic_parameters(std::complex<double>& p0, std::complex<double>& p1, std::complex<double>& p2, std::complex<double>& p3, std::complex<double>& a, std::complex<double>& b, std::complex<double>& c, std::complex<double>& d);
	void				calc_cubic_points(std::complex<double>& a, std::complex<double>& b, std::complex<double>& c, std::complex<double>& d, std::complex<double>& _1, std::complex<double>& _2, std::complex<double>& _3, std::complex<double>& _4);

	BOOL				NameTranscodeToWCHAR(CHYName& name);
	void				MakeFontSaveInfo();
	void				ScaleGlyph(CHYGlyph& Gryph, float fScale);
	void				SetAdWidth(UINT adw, CHYGlyph& Glyphs);
	void				SetAdHeight(UINT adh, CHYGlyph& Glyphs);

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
	UINT							m_adsAdw;	// ����ָ�����ο��
	UINT							m_adsAdh;	// ����ָ�����θ߶�
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
	
};