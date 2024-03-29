
// HYFontVerifyDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "HYFontVerify.h"
#include "HYFontVerifyDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define COLOR_DEFAULT 0 //默认颜色
#define COLOR_RED 1		//红色
#define COLOR_YELLOW 2	//蓝色
#define COLOR_WHITE 3	//蓝色

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{

}	// end of CAboutDlg::CAboutDlg() : CDialogEx()

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

}	// end of void CAboutDlg::DoDataExchange()

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// CHYFontVerifyDlg 对话框
CHYFontVerifyDlg::CHYFontVerifyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_HYFONTVERIFY_DIALOG, pParent)
	, m_bError(FALSE)
	, m_bWarn(FALSE)
	, m_bAllInf(TRUE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_TimerID = 0;

}	//end of CHYFontVerifyDlg::CHYFontVerifyDlg()

void CHYFontVerifyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LST_TRACE, m_lstBxTraceInf);
	DDX_Control(pDX, IDC_LST_FILE, m_lstCntrlFontResult);
	DDX_Control(pDX, IDC_LST_FNTINF, m_lstCntrlFntInf);
	DDX_Check(pDX, IDC_HCK_ERR, m_bError);
	DDX_Check(pDX, IDC_HCK_WRN, m_bWarn);
	DDX_Check(pDX, IDC_HCK_INF, m_bAllInf);
}	// end of void CHYFontVerifyDlg::DoDataExchange()

BEGIN_MESSAGE_MAP(CHYFontVerifyDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(ID_MN_OPENFILE, &CHYFontVerifyDlg::OnMnOpenfile)
	ON_MESSAGE(VM_UPDATEALLVIEWS, &CHYFontVerifyDlg::OnUpdateAllViews)	
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LST_FILE, &CHYFontVerifyDlg::OnNMCustomdrawLstFile)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LST_FNTINF, &CHYFontVerifyDlg::OnNMCustomdrawLstFntinf)
	ON_BN_CLICKED(IDC_HCK_ERR, &CHYFontVerifyDlg::OnBnClickedHckErr)
	ON_BN_CLICKED(IDC_HCK_WRN, &CHYFontVerifyDlg::OnBnClickedHckWrn)
	ON_NOTIFY(NM_SETFOCUS, IDC_LST_FILE, &CHYFontVerifyDlg::OnNMSetfocusLstFile)
	ON_NOTIFY(NM_CLICK, IDC_LST_FILE, &CHYFontVerifyDlg::OnNMClickLstFile)
	ON_COMMAND(ID_FNTFIX_MN, &CHYFontVerifyDlg::OnFntfixMn)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_HCK_INF, &CHYFontVerifyDlg::OnBnClickedHckInf)
END_MESSAGE_MAP()

// CHYFontVerifyDlg 消息处理程序
BOOL CHYFontVerifyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。
	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_MainMn.LoadMenuA(IDR_MAIN_MN);
	SetMenu(&m_MainMn);

	DWORD dwStyle = m_lstCntrlFontResult.GetExtendedStyle();

	dwStyle |= LVS_EX_FULLROWSELECT;
	dwStyle |= LVS_EX_GRIDLINES;	// 网格线   	

	m_lstCntrlFontResult.SetExtendedStyle(dwStyle);	
	CRect rect;
	m_lstCntrlFontResult.GetClientRect(&rect);
	m_lstCntrlFontResult.InsertColumn(0, _T("结果"), LVCFMT_CENTER, rect.Width()-2);	

	dwStyle = m_lstCntrlFntInf.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;
	dwStyle |= LVS_EX_GRIDLINES;	// 网格线   
	m_lstCntrlFntInf.SetExtendedStyle(dwStyle);
	m_lstCntrlFntInf.GetClientRect(&rect);
	m_lstCntrlFntInf.InsertColumn(0, _T("信息"), LVCFMT_CENTER, rect.Width() - 2);
	
	m_vtFntVrfResult.clear();

	m_TimerID = SetTimer(1001,500,NULL);
	UpdateData(FALSE);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE

}	// end of BOOL CHYFontVerifyDlg::OnInitDialog()

void CHYFontVerifyDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}

}	// end of void CHYFontVerifyDlg::OnSysCommand()

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。
void CHYFontVerifyDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}

}	// end of void CHYFontVerifyDlg::OnPaint()

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CHYFontVerifyDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);

}	// end of HCURSOR CHYFontVerifyDlg::OnQueryDragIcon()

void CHYFontVerifyDlg::OnMnOpenfile()
{
	TCHAR	szFilters[] = _T("TrueType 文件(*.ttf)|*.ttf|OpenType 文件(*.otf)|*.otf||");
	DWORD	MAXFILE = 2048;
	TCHAR*	pc = new TCHAR[MAXFILE*(MAX_PATH + 1) + 1];

	CFileDialog  openFileDlg(TRUE, _T(""), _T(""), OFN_ALLOWMULTISELECT | OFN_READONLY | OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilters);
	if (openFileDlg.DoModal() != IDOK)
	{
		if (pc) delete[] pc;
		pc = NULL;
		return;
	}

	m_vtFntVrfResult.clear();
	m_lstBxTraceInf.ResetContent();
	m_lstCntrlFontResult.DeleteAllItems();
	m_vtReusltIndex.clear();
	m_lstCntrlFntInf.DeleteAllItems();	

	int nCount = 0;
	int iTotalNum = 0, iErrNum = 0, iWarnNum = 0;
	POSITION pos = openFileDlg.GetStartPosition();
	while (pos != NULL)
	{
		CString szPathName = openFileDlg.GetNextPathName(pos);		

		CFontVerifyResult Result;
		fontCodec.VerifyFont((LPTSTR)(LPCTSTR)(szPathName), Result);
		
		m_vtFntVrfResult.push_back(Result);
		iTotalNum++;
		if (Result.IsError() > 0){
			iErrNum++;
		}
		else if(Result.IsWarn()>0){
			iWarnNum++;
		}

		if (m_vtFntVrfResult.size() > 0)
		{
			::XSysproxy().UpdateAllViews(MSG_UPDATETRACE, m_vtFntVrfResult.size() - 1);
		}
	}	

	CString strInf;
	strInf.Format("共检测%d个文件，错误文件%d个，问题文件%d个", iTotalNum, iErrNum, iWarnNum);

	m_lstBxTraceInf.AddString(strInf);
	m_lstBxTraceInf.SelectString(-1,strInf);

	UpdateData(TRUE);

	UpdateFontFileLst();

	delete[] pc;
	pc = NULL;

}	// end of void CHYFontVerifyDlg::OnMnOpenfile()

LRESULT  CHYFontVerifyDlg::OnUpdateAllViews(WPARAM waram, LPARAM lparam)
{
	switch (waram)
	{
		case MSG_UPDATETRACE:
		{
			CString str = m_vtFntVrfResult[(int)lparam].strFontFile.c_str();
			m_lstBxTraceInf.AddString(str);
			break;
		}
		default:
			break;
	}

	return 0L;

}	// end of LRESULT  CHYFontVerifyDlg::OnUpdateAllViews()

void CHYFontVerifyDlg::OnNMCustomdrawLstFile(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVCUSTOMDRAW pNMCD = reinterpret_cast<LPNMTVCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	NMCUSTOMDRAW nmCustomDraw = pNMCD->nmcd;
	switch (nmCustomDraw.dwDrawStage)
	{
	case CDDS_ITEMPREPAINT:
	{
		if (COLOR_WHITE == nmCustomDraw.lItemlParam)
		{
			pNMCD->clrTextBk = RGB(255, 255, 255);
			pNMCD->clrText = RGB(0, 0, 0);
		}
		else if (COLOR_RED == nmCustomDraw.lItemlParam)
		{
			pNMCD->clrTextBk = RGB(255, 0, 0);		//背景颜色
			pNMCD->clrText = RGB(255, 255, 255);		//文字颜色
		}
		else if (COLOR_YELLOW == nmCustomDraw.lItemlParam)
		{
			pNMCD->clrTextBk = RGB(255, 255, 128);
			pNMCD->clrText = RGB(0, 0, 0);
		}
		else
		{
			pNMCD->clrTextBk = RGB(255, 255, 255);
			pNMCD->clrText = RGB(0, 0, 0);
		}
		break;
	}
	default:
	{
		break;
	}
	}

	*pResult = 0;
	*pResult |= CDRF_NOTIFYPOSTPAINT;		//必须有，不然就没有效果
	*pResult |= CDRF_NOTIFYITEMDRAW;		//必须有，不然就没有效果
	return;

}	// end of void CHYFontVerifyDlg::OnNMCustomdrawLstFile()

void CHYFontVerifyDlg::OnNMCustomdrawLstFntinf(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVCUSTOMDRAW pNMCD = reinterpret_cast<LPNMTVCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	NMCUSTOMDRAW nmCustomDraw = pNMCD->nmcd;
	switch (nmCustomDraw.dwDrawStage)
	{
	case CDDS_ITEMPREPAINT:
	{
		if (COLOR_WHITE == nmCustomDraw.lItemlParam)
		{
			pNMCD->clrTextBk = RGB(255, 255, 255);
			pNMCD->clrText = RGB(0, 0, 0);
		}
		else if (COLOR_RED == nmCustomDraw.lItemlParam)
		{
			pNMCD->clrTextBk = RGB(255, 0, 0);		//背景颜色
			pNMCD->clrText = RGB(255, 255, 255);		//文字颜色
		}
		else if (COLOR_YELLOW == nmCustomDraw.lItemlParam)
		{
			pNMCD->clrTextBk = RGB(255, 255, 128);
			pNMCD->clrText = RGB(0, 0, 0);
		}
		else
		{
			pNMCD->clrTextBk = RGB(255, 255, 255);
			pNMCD->clrText = RGB(0, 0, 0);
		}
		break;
	}
	default:
	{
		break;
	}
	}

	*pResult = 0;
	*pResult |= CDRF_NOTIFYPOSTPAINT;		//必须有，不然就没有效果
	*pResult |= CDRF_NOTIFYITEMDRAW;		//必须有，不然就没有效果

}	//end of void CHYFontVerifyDlg::OnNMCustomdrawLstFntinf()

void CHYFontVerifyDlg::OnBnClickedHckErr()
{	
	UpdateData(TRUE);
	if (m_bError) {
		m_bAllInf = false;
	}

	UpdateFontFileLst();

}	// end of void CHYFontVerifyDlg::OnBnClickedHckErr()

void CHYFontVerifyDlg::OnBnClickedHckWrn()
{
	UpdateData(TRUE);
	if (m_bWarn) {
		m_bAllInf = false;
	}

	UpdateFontFileLst();

}	// end of void CHYFontVerifyDlg::OnBnClickedHckWrn()

void CHYFontVerifyDlg::OnNMSetfocusLstFile(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	
	int iIndex = m_lstCntrlFontResult.GetNextItem(-1, LVNI_FOCUSED);
	if (iIndex > -1)
	{
		UpdateFontInofLst(m_vtReusltIndex[iIndex]);
	}
	
	*pResult = 0;

}	// end of void CHYFontVerifyDlg::OnNMSetfocusLstFile()

void CHYFontVerifyDlg::OnNMClickLstFile(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int iIndex = m_lstCntrlFontResult.GetNextItem(-1, LVNI_SELECTED);
	if (iIndex > -1)
	{
		UpdateFontInofLst(m_vtReusltIndex[iIndex]);
	}

	*pResult = 0;

}	// end of void CHYFontVerifyDlg::OnNMClickLstFile()

void CHYFontVerifyDlg::UpdateFontFileLst()
{
	m_lstCntrlFontResult.DeleteAllItems();
	m_lstCntrlFntInf.DeleteAllItems();
	m_vtReusltIndex.clear();
	
	if (m_bAllInf)
	{
		m_bWarn = FALSE;
		m_bError = FALSE;

		size_t st = m_vtFntVrfResult.size();
		for (size_t i = 0; i != st; i++)
		{
			m_lstCntrlFontResult.InsertItem(i, ::HY_GetFullNameFromPath(m_vtFntVrfResult[i].strFontFile).c_str());
			m_vtReusltIndex.push_back(i);

			if (m_vtFntVrfResult[i].IsError()){				
				m_lstCntrlFontResult.SetItemData(i, COLOR_RED);
			}
			else if (m_vtFntVrfResult[i].IsWarn()){				
				m_lstCntrlFontResult.SetItemData(i, COLOR_YELLOW);
			}
			else{				
				m_lstCntrlFontResult.SetItemData(i, COLOR_WHITE);
			}
		}
	}
	else
	{
		m_bAllInf = FALSE;		

		if (m_bWarn&&m_bError)
		{
			size_t st = m_vtFntVrfResult.size();
			for (size_t i = 0; i != st; i++)
			{
				if (m_vtFntVrfResult[i].IsError())	{
					m_lstCntrlFontResult.InsertItem(i, ::HY_GetFullNameFromPath(m_vtFntVrfResult[i].strFontFile).c_str());
					m_vtReusltIndex.push_back(i);		
					m_lstCntrlFontResult.SetItemData(i, COLOR_RED);
				}
				else if (m_vtFntVrfResult[i].IsWarn()){
					m_lstCntrlFontResult.InsertItem(i, ::HY_GetFullNameFromPath(m_vtFntVrfResult[i].strFontFile).c_str());
					m_vtReusltIndex.push_back(i);					
					m_lstCntrlFontResult.SetItemData(i, COLOR_YELLOW);
				}
			}
		}

		if (m_bWarn && !m_bError)
		{
			size_t st = m_vtFntVrfResult.size();
			for (size_t i = 0; i != st; i++){
				if (m_vtFntVrfResult[i].IsWarn() && (m_vtFntVrfResult[i].IsError() == 0)){
					m_lstCntrlFontResult.InsertItem(i, ::HY_GetFullNameFromPath(m_vtFntVrfResult[i].strFontFile).c_str());
					m_vtReusltIndex.push_back(i);
					m_lstCntrlFontResult.SetItemData(i, COLOR_YELLOW);
				}
			}
		}

		if (!m_bWarn && m_bError)
		{
			size_t st = m_vtFntVrfResult.size();
			for (size_t i = 0; i != st; i++)
			{
				if (m_vtFntVrfResult[i].IsError())	{
					m_lstCntrlFontResult.InsertItem(i, ::HY_GetFullNameFromPath(m_vtFntVrfResult[i].strFontFile).c_str());
					m_vtReusltIndex.push_back(i);
					m_lstCntrlFontResult.SetItemData(i, COLOR_RED);
				}
			}
		}
	}

	if (m_lstCntrlFontResult.GetItemCount() > 0)
	{
		m_lstCntrlFontResult.SetFocus();
		m_lstCntrlFontResult.SetItemState(0, LVNI_FOCUSED | LVIS_SELECTED, LVNI_FOCUSED | LVIS_SELECTED);

		NMLISTVIEW  nmListView;
		nmListView.iItem = 0; //告诉响应函数选中的是第几行，如果为-1则为不选中
		nmListView.hdr.code = NM_CLICK;
		nmListView.hdr.idFrom = IDC_LST_FILE;
		nmListView.hdr.hwndFrom = m_lstCntrlFontResult.m_hWnd;
		SendMessage(WM_NOTIFY,0, (LPARAM)&nmListView);
	}

	UpdateData(FALSE);

}	//end of void CHYFontVerifyDlg::UpdateFontFileLst()

void CHYFontVerifyDlg::UpdateFontInofLst(int iResultIndex)
{
	m_lstCntrlFntInf.DeleteAllItems();

	if (iResultIndex == -1) return;	
	if (iResultIndex >= (int)m_vtFntVrfResult.size()) return;

	CFontVerifyResult&  VerifyResult = m_vtFntVrfResult[iResultIndex];
	size_t st = VerifyResult.vtVerifyInf.size();

	for (size_t i = 0; i != st; i++)
	{
		m_lstCntrlFntInf.InsertItem((int)i, VerifyResult.vtVerifyInf[i].strInf.c_str());
		if (VerifyResult.vtVerifyInf[i].iWarnLevel == WARNLEVEL_WARN)	m_lstCntrlFntInf.SetItemData(i, COLOR_YELLOW);
		else if (VerifyResult.vtVerifyInf[i].iWarnLevel == WARNLEVEL_ERR)	m_lstCntrlFntInf.SetItemData(i, COLOR_RED);
		else if (VerifyResult.vtVerifyInf[i].iWarnLevel == WARNLEVEL_NORMAL) m_lstCntrlFntInf.SetItemData(i, COLOR_WHITE);
	}	

}	// end of void CHYFontVerifyDlg::UpdateFontInofLst()

void CHYFontVerifyDlg::OnFntfixMn()
{
	TCHAR	szFilters[] = _T("TrueType 文件(*.ttf)|*.ttf|OpenType 文件(*.otf)|*.otf||");
	DWORD	MAXFILE = 2048;
	TCHAR*	pc = new TCHAR[MAXFILE*(MAX_PATH + 1) + 1];

	CFileDialog  openFileDlg(TRUE, _T(""), _T(""), OFN_ALLOWMULTISELECT | OFN_READONLY | OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilters);
	if (openFileDlg.DoModal() != IDOK)
	{
		if (pc) delete[] pc;
		pc = NULL;
		return;
	}

	CWaitCursor wc; 

	m_bAllInf = TRUE;

	m_vtFntVrfResult.clear();
	m_lstBxTraceInf.ResetContent();
	m_lstCntrlFontResult.DeleteAllItems();
	m_vtReusltIndex.clear();
	m_lstCntrlFntInf.DeleteAllItems();

	int nCount = 0;
	int iTotalNum = 0, iErrNum = 0, iWarnNum = 0;
	POSITION pos = openFileDlg.GetStartPosition();
	while (pos != NULL)
	{	
		CFontVerifyResult Result;		
		CString stroldPathName = openFileDlg.GetNextPathName(pos);
		CString strnewPathName;
		strnewPathName.Format("%s\\%s_rmHinting.ttf", HY_GetDirFromPath((LPTSTR)(LPCTSTR)stroldPathName).data(), HY_GetFileNameFromPath((LPTSTR)(LPCTSTR)(stroldPathName)).data());

		Result.strFontFile = stroldPathName;
		if (::HY_FileExist(stroldPathName) > 0)
		{			
			CVerifyInformation info;			
			FixFont(stroldPathName, strnewPathName);				
			info.iWarnLevel = WARNLEVEL_NORMAL;
			info.strInf = "文件处理完成,新文件" + HY_GetFileNameFromPath((LPTSTR)(LPCTSTR)strnewPathName) + ".ttf";
			Result.vtVerifyInf.push_back(info);
		}
		else
		{
			CVerifyInformation info;
			info.iWarnLevel = WARNLEVEL_ERR;
			info.strInf = _T("文件不存在");
			Result.vtVerifyInf.push_back(info);
		}		
		m_vtFntVrfResult.push_back(Result);


		if (m_vtFntVrfResult.size() > 0)
		{			
			::XSysproxy().UpdateAllViews(MSG_UPDATETRACE, m_vtFntVrfResult.size() - 1);
		}

	}

	UpdateData(TRUE);
	UpdateFontFileLst();

	delete[] pc;
	pc = NULL;

	
	UpdateData(FALSE);

}	// end of void CHYFontVerifyDlg::OnFntfixMn()

void CHYFontVerifyDlg::FixFont(const char* strFontFile, const char* strNewFontFile)
{
	CHYFontCodec Fontcodec;

	::XSysproxy().PrepareFont(strFontFile, Fontcodec);
	::XSysproxy().AdjustLsb(Fontcodec);
	::XSysproxy().BulidFont(Fontcodec,strNewFontFile);

}	// end of void CHYFontVerifyDlg::FixFont()

void CHYFontVerifyDlg::OnTimer(UINT_PTR nIDEvent)
{	
	switch (nIDEvent)
	{
	case 1001:		
		//m_lstBxTraceInf.AddString(m_strTaceInf);
		break;
	default:
		break;
	}

	CDialog::OnTimer(nIDEvent);

}	// end of void CHYFontVerifyDlg::OnTimer()

void CHYFontVerifyDlg::OnDestroy()
{
	CDialog::OnDestroy();

	if (m_TimerID != 0)	{
		KillTimer(m_TimerID);
	}
	
}	// end of void CHYFontVerifyDlg::OnDestroy()

void CHYFontVerifyDlg::OnBnClickedHckInf()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	if (m_bAllInf) {
		m_bWarn = false;
		m_bError = false;
	}

	UpdateFontFileLst();

}	// end of void CHYFontVerifyDlg::OnBnClickedHckInf()
