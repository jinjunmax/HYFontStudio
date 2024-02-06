
// HYFontSmartShaperView.h : CHYFontSmartShaperView 类的接口
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
	int iCurverType;	// 0 两次贝塞尔 1 三次贝塞尔
};
class CHYFontSmartShaperView : public CFormView
{
public: // 仅从序列化创建
	CHYFontSmartShaperView();
	virtual ~CHYFontSmartShaperView();
	DECLARE_DYNCREATE(CHYFontSmartShaperView)

public:
	enum { IDD = IDD_HYFONTSMARTSHAPER_FORM };

	// 属性
public:

	// 重写
public:
	virtual BOOL		PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void		DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void		OnInitialUpdate(); // 构造后第一次调用
	afx_msg void		OnDestroy();
	afx_msg void		OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL		OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg LRESULT		OnThumSelet(WPARAM waram, LPARAM lparam);
	afx_msg LRESULT		OnPopWnd(WPARAM waram, LPARAM lparam);
	afx_msg	LRESULT 	OnUpdateAllViews(WPARAM waram, LPARAM lparam);
	afx_msg void		GetDrawParam(UINT iID);
	afx_msg void		OnBnClickedMnSeekBtn();

	// 打开字库
	afx_msg void		OnFileOpen();
	// 生成字库
	afx_msg void		OnFileSave();
	// 生成设置
	afx_msg void		OnFntEncodeParam();
	afx_msg void		OnBnClickedMnFntstBtn();
	afx_msg void		OnBnClickedMnCvtsBtn();
	// 全部转换
	afx_msg void		OnBnClickedMnCvtmBtn();
	afx_msg void		OnIkMerge();
	afx_msg void		OnIkSplit();
	afx_msg void		OnCbnSelchangeMnVmtypCmb();
	afx_msg void		OnFntCvtpramMu();	
	afx_msg void		OnBnClickedMnCvtTtfRd();
	afx_msg void		OnBnClickedMnCvtOtfRd();
	// 合库
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
	// 多行码表用逗号分割
	afx_msg void		OnMnCodedot();
	// 生成简繁字库
	afx_msg void		OnMnMkjf();
	// 生产变高字库
	afx_msg void		OnFntVadh();
	afx_msg void		OnMnAduni();
	afx_msg void		OnMkvntMn();	
	afx_msg void		OnMnJszCodetchar();
	// 用于输出一些个性化的文本信息
	afx_msg void		OnChnvert();
	afx_msg void		OnVerifyvntMn();
	afx_msg void		OnTtcTtctofont();
	afx_msg void		OnTtcFonttottc();
	afx_msg void		OnCharsCompareMn();
	// 根据字表提取subsetfont
	afx_msg void		OnFntExtrchr();
	// 根据码表提取subsetfont
	afx_msg void		OnFntExtrcd();
	// 根据字形名称提取subsetfont
	afx_msg void		OnFntExtrname();
	afx_msg void		OnFntRpNodef();
	afx_msg void		OnFntRpTwfnt();
	afx_msg void		OnNoboxMnu();
	//重置字库postscript name
	afx_msg void		OnFntRsetname();
	//导出字库postscript name
	afx_msg void		OnMnExptname();
	// 用于调整中文字符的AdvanceWidth	
	afx_msg void		OnFlnm2numnm();
	
	
	afx_msg void		OnMnCmptxt();
	afx_msg void		OnMnChkerttf();
	afx_msg void		OnMnChkerotf();
	// 导出Emoji字库
	afx_msg void		OnEmojiExport();
	// 生成Emoji字库	
	afx_msg void		OnEmojiMk();
	// 导入码表
	afx_msg void		OnMnCodemap();
	// 重置编码
	afx_msg void		OnFntResetcode();
	virtual BOOL		PreTranslateMessage(MSG* pMsg);
	// 批处理otf -> CIDOTF
	afx_msg void		OnFntBudcidotf();
	// 修复Glyphs3版本可变字库轮廓点会增加OVERLAP_SIMPLE标志
	afx_msg void		OnG3fxMn();
	// 对字库的字符集进行检查，主要是检查
	afx_msg void		OnMnChkcharset();

	// 实现
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
	// 提取子集操作
	void				GetFntSubset(char* strSrcFnt, std::vector<unsigned long> vtUni, const char* strDstFnt);
	void				GetOTFSawtooth(std::vector<CHYPoint>& vtInPnts, std::vector<CHYPoint>& vtOutPnts);
	void				Smooth(CHYGlyph& inglyph, CHYGlyph& Outglyph);
	void				fitting(std::vector<CHYPoint>& vtInPnts, std::vector<CHYPoint>& vtOutPnts);
	void				SmoothAndfitting(std::vector<CHYPoint>& InvtPnt, std::vector<CHYPoint>& OutvtPnt);

	// 生成的消息映射函数
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
	
	
#ifndef _DEBUG  // HYFontSmartShaperView.cpp 中的调试版本
	inline CHYFontSmartShaperDoc* CHYFontSmartShaperView::GetDocument() const
	{
		return reinterpret_cast<CHYFontSmartShaperDoc*>(m_pDocument);
	}
#endif	
	
	afx_msg void OnMnExpname();
};