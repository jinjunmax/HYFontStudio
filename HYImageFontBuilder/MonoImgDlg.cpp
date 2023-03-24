// MonoImgDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "HYImageFontBuilder.h"
#include "MonoImgDlg.h"
#include "afxdialogex.h"


// CMonoImgDlg �Ի���

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
				  // �쳣: OCX ����ҳӦ���� FALSE

}	// end of BOOL CMonoImgDlg::OnInitDialog()

// CMonoImgDlg ��Ϣ�������
void CMonoImgDlg::OnBnClickedSrcpathBtn()
{	
	BROWSEINFO bi;
	TCHAR szPath[MAX_PATH] = {0};
	TCHAR szDirPath[MAX_PATH] = {0};

	bi.hwndOwner = NULL;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = szPath; //�˲�����ΪNULL������ʾ�Ի���
	bi.lpszTitle = "ѡ��Ŀ¼";
	bi.ulFlags = BIF_RETURNONLYFSDIRS;
	bi.lpfn = NULL;
	bi.iImage = 0;   //��ʼ����ڲ���bi����
	LPITEMIDLIST pIDList = SHBrowseForFolder(&bi);//������ʾѡ��Ի���
	if (pIDList)
	{
		SHGetPathFromIDList(pIDList, szDirPath);
		//ȡ���ļ���·����path��
		m_strSrcPath = szDirPath;//��·��������һ��CString�����m_strPathNameΪ�༭��󶨵�һ��ֵ��						   
	}

	UpdateData(FALSE);    //��ѡ����ļ���·����ʾ�ڱ༭����

}	// end of void CMonoImgDlg::OnBnClickedSrcpathBtn()

void CMonoImgDlg::OnBnClickedDstpathBtn()
{	
	BROWSEINFO bi;
	TCHAR szPath[MAX_PATH] = { 0 };
	TCHAR szDirPath[MAX_PATH] = { 0 };

	bi.hwndOwner = NULL;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = szPath; //�˲�����ΪNULL������ʾ�Ի���
	bi.lpszTitle = "ѡ��Ŀ¼";
	bi.ulFlags = BIF_RETURNONLYFSDIRS;
	bi.lpfn = NULL;
	bi.iImage = 0;   //��ʼ����ڲ���bi����
	LPITEMIDLIST pIDList = SHBrowseForFolder(&bi);//������ʾѡ��Ի���
	if (pIDList)
	{
		SHGetPathFromIDList(pIDList, szDirPath);
		//ȡ���ļ���·����path��
		m_strDstPath = szDirPath;//��·��������һ��CString�����m_strPathNameΪ�༭��󶨵�һ��ֵ��
	}

	UpdateData(FALSE);    //��ѡ����ļ���·����ʾ�ڱ༭����

}	// end of void CMonoImgDlg::OnBnClickedDstpathBtn()

void CMonoImgDlg::OnBnClickedCvntBtn()
{
	UpdateData(TRUE);

	if (m_strSrcPath.IsEmpty())	{
		AfxMessageBox("������Ч�Ķ�ȡĿ¼");
		return;
	}
	if (m_strDstPath.IsEmpty())	{
		AfxMessageBox("������Ч�ı���Ŀ¼");
		return;
	}	
	vtSrcFile.clear();
	::HY_GetFilesFromDir(std::string(m_strSrcPath), vtSrcFile);
	if (vtSrcFile.size() == 0)	{
		AfxMessageBox("��ȡĿ¼Ϊ��");
		return;
	}
	if (m_unMonoWidth == 0)	{
		AfxMessageBox("���Ϊ��");
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