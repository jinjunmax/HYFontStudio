// MakePrgrssDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "HYImageFontBuilder.h"
#include "MakePrgrssDlg.h"
#include "afxdialogex.h"


// CMakePrgrssDlg �Ի���

IMPLEMENT_DYNAMIC(CMakePrgrssDlg, CDialogEx)

CMakePrgrssDlg::CMakePrgrssDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_PRGRESS_DLG, pParent)
{

}	// end of CMakePrgrssDlg::CMakePrgrssDlg()

CMakePrgrssDlg::~CMakePrgrssDlg()
{

}	// end of CMakePrgrssDlg::~CMakePrgrssDlg()

void CMakePrgrssDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MAIN_PRS, m_MainPrgrss);
	DDX_Control(pDX, IDC_SUB_PRS, m_SubPrgrss);
	

}	// end of void CMakePrgrssDlg::DoDataExchange()

BEGIN_MESSAGE_MAP(CMakePrgrssDlg, CDialogEx)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_MESSAGE(MSG_PRS_MAIN, &CMakePrgrssDlg::OnMainPrsView)
	ON_MESSAGE(MSG_PRS_SUB, &CMakePrgrssDlg::OnSubPrsView)
	ON_WM_PAINT()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

// CMakePrgrssDlg ��Ϣ�������
BOOL CMakePrgrssDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	m_MainPrgrss.SetRange(1,100);
	m_SubPrgrss.SetRange(1,100);

	m_MainPrgrss.SetPos(1);
	m_SubPrgrss.SetPos(1);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE

}	// end of BOOL CMakePrgrssDlg::OnInitDialog()

void CMakePrgrssDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: �ڴ˴������Ϣ����������

}	// end of void CMakePrgrssDlg::OnDestroy()

void CMakePrgrssDlg::OnTimer(UINT_PTR nIDEvent)
{	


	CDialogEx::OnTimer(nIDEvent);

}	// end of void CMakePrgrssDlg::OnTimer()

LRESULT CMakePrgrssDlg::OnMainPrsView(WPARAM waram, LPARAM lparam)
{
	m_MainPrgrss.SetPos((int)lparam);

	return 0L;

}	// end of LRESULT CMakePrgrssDlg::OnMainPrsView()

LRESULT CMakePrgrssDlg::OnSubPrsView(WPARAM waram, LPARAM lparam)
{
	m_SubPrgrss.SetPos((int)lparam);

	return 0L;

}	// end of LRESULT CMakePrgrssDlg::OnSubPrsView()

void CMakePrgrssDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: �ڴ˴������Ϣ����������
					   // ��Ϊ��ͼ��Ϣ���� CDialogEx::OnPaint()

}	// end of void CMakePrgrssDlg::OnPaint()

void CMakePrgrssDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: �ڴ˴������Ϣ����������
	if (bShow)
	{
		this->UpdateWindow();
	}

}	// end of void CMakePrgrssDlg::OnShowWindow()
