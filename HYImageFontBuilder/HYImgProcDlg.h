#pragma once
#include "afxwin.h"

#include "CHYPreviewWnd.h"
#include "afxcmn.h"
// CHYImgProcDlg 对话框
#define WM_SCRLMSG  WM_USER+201
#define WM_STPMSG  WM_USER+202

class CHYImgProcDlg : public CDialog
{
	DECLARE_DYNAMIC(CHYImgProcDlg)

public:
	CHYImgProcDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CHYImgProcDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_IMGPROC_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedImgpbatch();
	afx_msg void OnImgprdOpenfileMu();
	afx_msg void OnBnClickedImgpInvertChk();		
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg LRESULT OnSCRLMsgHandler(WPARAM, LPARAM);
	afx_msg LRESULT OnStepMsgHandler(WPARAM wm, LPARAM lm);
	afx_msg void OnBnClickedImgpBwChk();
	afx_msg void OnRadioButton(UINT nID);

	void MakeBGImage(cv::String strBGImage, cv::Mat& BGimg);
	void MergeBackground(cv::String strFGimg, cv::Mat& BGimg);
	void MergeBackground(cv::Mat& FGimg, cv::Mat& BGimg);
	void AdjustImage(cv::Mat& InImg, cv::Mat& OutImg);
	void SetContrastLight(cv::Mat& inImg, cv::Mat& outImg, int iContrast, int iLight);
	void SetBKThres(cv::Mat& inImg, cv::Mat& outImg, int ivalue);
	void SetInverse(cv::Mat& inImg, cv::Mat& outImg);

	CMenu			m_Menu;
	cv::Mat			m_BGimg;
	cv::Mat			m_Mergeimg;	
	CHYPreviewWnd	m_prwWnd;
	CSliderCtrl		m_sldContrast;
	CSliderCtrl		m_sldLight;
	CSliderCtrl		m_sldBWThres;
	BOOL			m_bInvert;
	BOOL			m_bMono;
	CString			m_strBGImageName;
	CString			m_strFGImageName;
	int				m_iLight;
	int				m_iContrast;
	int				m_iBWThres;	
	int				m_iTimerID;	
	CProgressCtrl	m_psctrlImgpress;
	string			m_strCurFile;
	cv::Mat			m_OutImg;
	afx_msg void OnImgprdSavefileMu();
};
