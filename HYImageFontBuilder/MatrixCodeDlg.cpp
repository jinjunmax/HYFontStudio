// MatrixCodeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "HYImageFontBuilder.h"
#include "MatrixCodeDlg.h"
#include "afxdialogex.h"


// CMatrixCodeDlg 对话框

IMPLEMENT_DYNAMIC(CMatrixCodeDlg, CDialog)

CMatrixCodeDlg::CMatrixCodeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_RCCODE_DLG, pParent)
{

}	//end of CMatrixCodeDlg::CMatrixCodeDlg()

CMatrixCodeDlg::~CMatrixCodeDlg()
{

}	// end of CMatrixCodeDlg::~CMatrixCodeDlg()

void CMatrixCodeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RED_CDEINF, m_rdcCodeInfo);

}	// end of void CMatrixCodeDlg::DoDataExchange()

void CMatrixCodeDlg::SetMatrixCode(CStringArray& szMatrixCode)
{
	m_rdcCodeInfo.SetSel(0, -1);
	m_rdcCodeInfo.Clear();

	CString strText;
	for (size_t i = 0; i < szMatrixCode.GetCount(); i++)
	{
		strText += szMatrixCode[i];
	}

	m_rdcCodeInfo.SetWindowText(strText);

}	// end of void CMatrixCodeDlg::SetMatrixCode()


BEGIN_MESSAGE_MAP(CMatrixCodeDlg, CDialog)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// CMatrixCodeDlg 消息处理程序

BOOL CMatrixCodeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}	// end of BOOL CMatrixCodeDlg::OnInitDialog()

void CMatrixCodeDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	m_rdcCodeInfo.SetSel(0, -1);
	m_rdcCodeInfo.Clear();

}	// end of void CMatrixCodeDlg::OnDestroy()
