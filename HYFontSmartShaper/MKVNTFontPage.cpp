// MKVNTFontPage.cpp : 实现文件
//

#include "stdafx.h"
#include "HYFontSmartShaper.h"
#include "MKVNTFontPage.h"
#include "afxdialogex.h"


// CMKVNTFontPage 对话框

IMPLEMENT_DYNAMIC(CMKVNTFontPage, CPropertyPage)

CMKVNTFontPage::CMKVNTFontPage()
	: CPropertyPage(IDD_FNT_MKVNT_FNT_PAGE)
{
#ifdef MYDEBUG
	m_strENGFamilyName = "HYTest";
	m_strCHSFamilyName = "汉仪测试"; 
	m_strPostScriptName = "HYTest";
	m_strENGCopyright = "Hanyi";
	m_strCHSCopyright = "Hanyi";
	m_strVersion = "0.001";
#else 
	m_strENGFamilyName = "";
	m_strCHSFamilyName = "";
	m_strPostScriptName = "";
	m_strENGCopyright = "";
	m_strCHSCopyright = "";
	m_strVersion = "";
#endif 

}	//end of CMKVNTFontPage::CMKVNTFontPage() 

CMKVNTFontPage::~CMKVNTFontPage()
{

}	// end of CMKVNTFontPage::~CMKVNTFontPage()

void CMKVNTFontPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_MKV_FNT_ENGFAMILY_EDT, m_strENGFamilyName);
	DDX_Text(pDX, IDC_MKV_FNT_CHSNAME_EDT, m_strCHSFamilyName);
	DDX_Text(pDX, IDC_MKV_FNT_PSTSCT_EDT, m_strPostScriptName);
	DDX_Text(pDX, IDC_MKV_FNT_ENGCPYGHT_EDT, m_strENGCopyright);
	DDX_Text(pDX, IDC_MKV_FNT_CHSCPYGHT_EDT, m_strCHSCopyright);
	DDX_Text(pDX, IDC_MKV_FNT_VRSN_EDT, m_strVersion);

}	// end of void CMKVNTFontPage::DoDataExchange()

BEGIN_MESSAGE_MAP(CMKVNTFontPage, CPropertyPage)
	ON_BN_CLICKED(IDC_MKV_FNT_MKNAME_EDT, &CMKVNTFontPage::OnBnClickedMkvFntMknameEdt)
END_MESSAGE_MAP()

// CMKVNTFontPage 消息处理程序
void CMKVNTFontPage::OnBnClickedMkvFntMknameEdt()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();

	::XSysproxy().VrFntName.m_strENGFamilyName = m_strENGFamilyName;
	::XSysproxy().VrFntName.m_strCHSFamilyName = m_strCHSFamilyName;
	::XSysproxy().VrFntName.m_strPostScriptName = m_strPostScriptName;
	::XSysproxy().VrFntName.m_strENGCopyright = m_strENGCopyright;
	::XSysproxy().VrFntName.m_strCHSCopyright = m_strCHSCopyright;
	::XSysproxy().VrFntName.m_strVersion = m_strVersion;

}	// end of void CMKVNTFontPage::OnBnClickedMkvFntMknameEdt()

BOOL CMKVNTFontPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_strENGFamilyName		= ::XSysproxy().VrFntName.m_strENGFamilyName;
	m_strCHSFamilyName		= ::XSysproxy().VrFntName.m_strCHSFamilyName;
	m_strPostScriptName		= ::XSysproxy().VrFntName.m_strPostScriptName;
	m_strENGCopyright		= ::XSysproxy().VrFntName.m_strENGCopyright;
	m_strCHSCopyright		= ::XSysproxy().VrFntName.m_strCHSCopyright;
	m_strVersion			= ::XSysproxy().VrFntName.m_strVersion;

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE

}	// end of BOOL CMKVNTFontPage::OnInitDialog()
