// TXTCmpDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "HYFontSmartShaper.h"
#include "TXTCmpDlg.h"
#include "afxdialogex.h"


// CTXTCmpDlg 对话框

IMPLEMENT_DYNAMIC(CTXTCmpDlg, CDialogEx)
CTXTCmpDlg::CTXTCmpDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TXTCMP_DLG, pParent)
	, m_strFile1(_T(""))
	, m_strFile2(_T(""))
{

}	// end of CTXTCmpDlg::CTXTCmpDlg()

CTXTCmpDlg::~CTXTCmpDlg()
{

}	// end of CTXTCmpDlg::~CTXTCmpDlg()

void CTXTCmpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_TCMPFL1_RD, m_strFile1);
	DDX_Text(pDX, IDC_TCMPFL2_RD, m_strFile2);
}	// end of void CTXTCmpDlg::DoDataExchange()

BEGIN_MESSAGE_MAP(CTXTCmpDlg, CDialogEx)
	ON_BN_CLICKED(IDC_TCMPFL1_BTN, &CTXTCmpDlg::OnBnClickedTcmpfl1Btn)
	ON_BN_CLICKED(IDC_TCMPFL2_BTN, &CTXTCmpDlg::OnBnClickedTcmpfl2Btn)
	ON_BN_CLICKED(IDC_TCMPDST_BTN, &CTXTCmpDlg::OnBnClickedTcmpdstBtn)
END_MESSAGE_MAP()

// CTXTCmpDlg 消息处理程序
BOOL CTXTCmpDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CheckRadioButton(IDC_TCMPZB_RD, IDC_TCMPNB_RD, IDC_TCMPZB_RD);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE

}	// end of BOOL CTXTCmpDlg::OnInitDialog()

void CTXTCmpDlg::OnBnClickedTcmpfl1Btn()
{
	TCHAR	szFilters[] = _T("TXT(*.txt)|*.txt||");
	CFileDialog  openFileDlg(TRUE, _T(""), _T(""), OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilters);
	if (openFileDlg.DoModal() != IDOK)	return;

	m_strFile1 = openFileDlg.GetPathName();

	UpdateData(FALSE);

}	// end of void CTXTCmpDlg::OnBnClickedTcmpfl1Btn()

void CTXTCmpDlg::OnBnClickedTcmpfl2Btn()
{
	TCHAR	szFilters[] = _T("TXT(*.txt)|*.txt||");
	CFileDialog  openFileDlg(TRUE, _T(""), _T(""), OFN_LONGNAMES | OFN_FILEMUSTEXIST, szFilters);
	if (openFileDlg.DoModal() != IDOK)	return;

	m_strFile2 = openFileDlg.GetPathName();

	UpdateData(FALSE);

}	// end of void CTXTCmpDlg::OnBnClickedTcmpfl2Btn()

void CTXTCmpDlg::OnBnClickedTcmpdstBtn()
{
	UpdateData();

	int CtrlID = GetCheckedRadioButton(IDC_TCMPZB_RD, IDC_TCMPNB_RD);
	string diffFile = HY_GetDirFromPath(string(m_strFile1))
		+ HY_GetFileNameFromPath(string(m_strFile1))+"_diff.txt";

	string sameFile = HY_GetDirFromPath(string(m_strFile1))
		+ HY_GetFileNameFromPath(string(m_strFile1)) + "_same.txt";

	if (m_strFile1 == "") return;
	if (m_strFile2 == "") return;

	std::vector<string> diff;
	std::vector<string> same;

	switch (CtrlID)
	{
		case IDC_TCMPZB_RD:
			CmprCharFile(m_strFile1, m_strFile2, diff, same);
			break;
		case IDC_TCMPMB_RD:
			CmprCodeFile(m_strFile1, m_strFile2, diff, same);
			break;
		case IDC_TCMPNB_RD:
			CmprCodeFile(m_strFile1, m_strFile2, diff, same);
			break;
		default:
			break;
	}

	DeleteFile(diffFile.c_str());
	DeleteFile(sameFile.c_str());

	ofstream diffout(diffFile);
	if (!diffout.bad())
	{
		for (size_t st = 0; st < diff.size(); st++)
		{
			diffout<<diff[st]<<endl;
		}
		diffout.close();
	}

	ofstream sameout(sameFile);
	if (!sameout.bad())
	{
		for (size_t st = 0; st < diff.size(); st++)
		{
			sameout << same[st]<<endl;
		}
		sameout.close();
	}
	
	AfxMessageBox("处理完成");

}	// end of void CTXTCmpDlg::OnBnClickedTcmpdstBtn()

void CTXTCmpDlg::CmprCodeFile(CString strFL1, CString strFL2, std::vector<string>& vtDiff , std::vector<string>& vtSame)
{
	vtDiff.clear();
	vtSame.clear();

	ifstream in1(strFL1);
	string line1;
	vector<string> vtText1;	
	if (in1) {
		while (getline(in1, line1)) { // line中不包括每行的换行符
			if (line1.length() == 0) continue;
			line1 = HY_trim(line1);			
			vtText1.push_back(line1);
		}
	}

	ifstream in2(strFL2);
	string line2;
	vector<string> vtText2;
	if (in2) {
		while (getline(in2, line2)) { // line中不包括每行的换行符
			if (line2.length() == 0) continue;
			line2 = HY_trim(line2);
			vtText2.push_back(line2);
		}
	}	
	
	int iIndex1 = 0;
	int iIndex2 = 0;
	string strdiff="";
	while (true) {
		string str1, str2;
		if (iIndex1 < vtText1.size()&& iIndex2 < vtText2.size()){
			str1 = vtText1[iIndex1];
			str2 = vtText2[iIndex2];

			if (str1 != str2){
				strdiff = str1 + ";" + str2;
				vtDiff.push_back(strdiff);
			}
			else{
				vtSame.push_back(str1);
			}
		}
		else if (iIndex1 >= vtText1.size() && iIndex2 < vtText2.size()){
			strdiff = ";" + str2;
			vtDiff.push_back(strdiff);
		}
		else if (iIndex1 < vtText1.size() && iIndex2 >= vtText2.size()){
			strdiff =str1+";";
			vtDiff.push_back(strdiff);
		}
		else if (iIndex1 >= vtText1.size() && iIndex2 >= vtText2.size()){
			break;
		}

		iIndex1++;
		iIndex2++;
	}

}	// end of void CTXTCmpDlg::CmprCodeFile()

void CTXTCmpDlg::CmprCharFile(CString strFL1, CString strFL2, std::vector<string>& vtDiff, std::vector<string>& vtSame)
{
	
	

}	// end of void CTXTCmpDlg::CmprChar()
