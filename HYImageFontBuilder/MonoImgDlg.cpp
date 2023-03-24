// MonoImgDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "HYImageFontBuilder.h"
#include "MonoImgDlg.h"
#include "afxdialogex.h"


// CMonoImgDlg 对话框

IMPLEMENT_DYNAMIC(CMonoImgDlg, CDialog)

CMonoImgDlg::CMonoImgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_MONO_DLG, pParent)
	, m_unMonoWidth(0)
	, m_strSrcPath(_T(""))
	, m_strDstPath(_T(""))
{
}	// end of CMonoImgDlg::CMonoImgDlg()

CMonoImgDlg::~CMonoImgDlg()
{
}	// end of CMonoImgDlg::~CMonoImgDlg()

void CMonoImgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_MONOSPACE_EDT, m_unMonoWidth);

	DDX_Control(pDX, IDC_CVNT_PRGRS, m_prgsConvert);
	DDX_Text(pDX, IDC_SRCPATH_EDT, m_strSrcPath);
	DDX_Text(pDX, IDC_DSTPATH_EDT, m_strDstPath);
}	//end of void CMonoImgDlg::DoDataExchange()

BEGIN_MESSAGE_MAP(CMonoImgDlg, CDialog)
	ON_BN_CLICKED(IDC_SRCPATH_BTN, &CMonoImgDlg::OnBnClickedSrcpathBtn)
	ON_BN_CLICKED(IDC_DSTPATH_BTN, &CMonoImgDlg::OnBnClickedDstpathBtn)
	ON_BN_CLICKED(IDC_CVNT_BTN, &CMonoImgDlg::OnBnClickedCvntBtn)
END_MESSAGE_MAP()

BOOL CMonoImgDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	m_prgsConvert.SetPos(0);
	m_prgsConvert.SetRange(0,100);
	vtSrcFile.clear();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE

}	// end of BOOL CMonoImgDlg::OnInitDialog()

// CMonoImgDlg 消息处理程序
void CMonoImgDlg::OnBnClickedSrcpathBtn()
{	
	BROWSEINFO bi;
	TCHAR szPath[MAX_PATH] = {0};
	TCHAR szDirPath[MAX_PATH] = {0};

	bi.hwndOwner = NULL;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = szPath; //此参数如为NULL则不能显示对话框
	bi.lpszTitle = "选择目录";
	bi.ulFlags = BIF_RETURNONLYFSDIRS;
	bi.lpfn = NULL;
	bi.iImage = 0;   //初始化入口参数bi结束
	LPITEMIDLIST pIDList = SHBrowseForFolder(&bi);//调用显示选择对话框
	if (pIDList)
	{
		SHGetPathFromIDList(pIDList, szDirPath);
		//取得文件夹路径到path里
		m_strSrcPath = szDirPath;//将路径保存在一个CString对象里，m_strPathName为编辑框绑定的一个值类						   
	}

	UpdateData(FALSE);    //将选择的文件夹路径显示在编辑框中

}	// end of void CMonoImgDlg::OnBnClickedSrcpathBtn()

void CMonoImgDlg::OnBnClickedDstpathBtn()
{	
	BROWSEINFO bi;
	TCHAR szPath[MAX_PATH] = { 0 };
	TCHAR szDirPath[MAX_PATH] = { 0 };

	bi.hwndOwner = NULL;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = szPath; //此参数如为NULL则不能显示对话框
	bi.lpszTitle = "选择目录";
	bi.ulFlags = BIF_RETURNONLYFSDIRS;
	bi.lpfn = NULL;
	bi.iImage = 0;   //初始化入口参数bi结束
	LPITEMIDLIST pIDList = SHBrowseForFolder(&bi);//调用显示选择对话框
	if (pIDList)
	{
		SHGetPathFromIDList(pIDList, szDirPath);
		//取得文件夹路径到path里
		m_strDstPath = szDirPath;//将路径保存在一个CString对象里，m_strPathName为编辑框绑定的一个值类
	}

	UpdateData(FALSE);    //将选择的文件夹路径显示在编辑框中

}	// end of void CMonoImgDlg::OnBnClickedDstpathBtn()

void CMonoImgDlg::OnBnClickedCvntBtn()
{
	UpdateData(TRUE);

	if (m_strSrcPath.IsEmpty())	{
		AfxMessageBox("不是有效的读取目录");
		return;
	}
	if (m_strDstPath.IsEmpty())	{
		AfxMessageBox("不是有效的保存目录");
		return;
	}	
	vtSrcFile.clear();
	::HY_GetFilesFromDir(std::string(m_strSrcPath), vtSrcFile);
	if (vtSrcFile.size() == 0)	{
		AfxMessageBox("读取目录为空");
		return;
	}
	if (m_unMonoWidth == 0)	{
		AfxMessageBox("宽度为零");
		return;
	}
	m_prgsConvert.SetPos(0);

	int iThres = ::XSysproxy().m_tagScanPrm.iGrayThre;
	int iSmoothParam = ::XSysproxy().m_tagScanPrm.iSmoothParam;

	size_t st = vtSrcFile.size();
	for (size_t i = 0; i < st; i++)
	{
		std::string& strSrcFile = vtSrcFile[i];
		std::string strSrcName = HY_GetFileNameFromPath(strSrcFile);
		std::string DstName = HY_StringFormat("%s\\%s", m_strDstPath, strSrcName.c_str());

		bool b = ::XSysproxy().SquareImage(strSrcFile.c_str(), iThres, DstName.c_str(), m_unMonoWidth, iSmoothParam);

		m_prgsConvert.SetPos((float)i/(st-1) * 100);
	}

}	// end of void CMonoImgDlg::OnBnClickedCvntBtn()