// IKSplitDlg.cpp : implementation file
//

#include "stdafx.h"
#include "HYFontSmartShaper.h"
#include "IKSplitDlg.h"

#define HY_EXTORDER_S	0	// 7533���������ȡ
#define HY_EXTORDER_P	1	// 7533ƫ�������ȡ
#define HY_EXTORDER_C	2	// �Զ��������ȡ

// CIKSplitDlg dialog

IMPLEMENT_DYNAMIC(CIKSplitDlg, CDialog)

CIKSplitDlg::CIKSplitDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CIKSplitDlg::IDD, pParent)	
	, m_strSrcIK(_T(""))
	, m_strSrcTXT(_T(""))
	, m_strCustom(_T(""))
{

}	// end of CIKSplitDlg::CIKSplitDlg()

CIKSplitDlg::~CIKSplitDlg()
{

}	// end of CIKSplitDlg::~CIKSplitDlg()

void CIKSplitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);	
	DDX_Control(pDX, IDD_IK_SPLT_INF_LST, m_lstbMergerInfo);
	DDX_Control(pDX, IDD_IK_SPORDER_CMB, m_cmbExtractOrder);
	DDX_Text(pDX, IDD_IK_SPLT_IKF_EDT, m_strSrcIK);
	DDX_Text(pDX, IDD_IK_SPLT_TXTF_EDT, m_strSrcTXT);
	DDX_Text(pDX, IDD_IK_SPLT_CTXT_EDT, m_strCustom);
	DDX_Control(pDX, IDD_IK_SPLT_LST, m_lstIkIFiles);
}	// end of void CIKSplitDlg::DoDataExchange()

BEGIN_MESSAGE_MAP(CIKSplitDlg, CDialog)	
	ON_BN_CLICKED(IDD_IK_SPLT_BTN, &CIKSplitDlg::OnBnClickedIkSpltBtn)
	ON_WM_DESTROY()	
	ON_BN_CLICKED(IDD_IK_SPLT_CTXT_BTN, &CIKSplitDlg::OnBnClickedIkSpltCtxtBtn)
	ON_BN_CLICKED(IDD_IK_SPLT_IKF_BTN, &CIKSplitDlg::OnBnClickedIkSpltIkfBtn)
	ON_BN_CLICKED(IDD_IK_SPLT_TXTF_BTN, &CIKSplitDlg::OnBnClickedIkSpltTxtfBtn)
	ON_NOTIFY(LVN_ITEMCHANGED, IDD_IK_SPLT_LST, &CIKSplitDlg::OnLvnItemchangedIkSpltLst)
	ON_BN_CLICKED(IDD_IK_SPLT_ADD_BTN, &CIKSplitDlg::OnBnClickedIkSpltAddBtn)
	ON_BN_CLICKED(IDD_IK_SPLT_DEL_BTN, &CIKSplitDlg::OnBnClickedIkSpltDelBtn)
	ON_CBN_SELCHANGE(IDD_IK_SPORDER_CMB, &CIKSplitDlg::OnCbnSelchangeIkSporderCmb)
END_MESSAGE_MAP()

void CIKSplitDlg::SetDefault()
{
	m_ikf27533.SetDefault();

}	// end of void CIKSplitDlg::SetDefault()

void CIKSplitDlg::InitParam()
{



}	// end of void CIKSplitDlg::InitParam()

void CIKSplitDlg::UninitParam()
{


}	// end of void CIKSplitDlg::UninitParam()

void CIKSplitDlg::IniCtrl()
{
	LVCOLUMN		tagColumn;
	ZeroMemory(&tagColumn, sizeof(LVCOLUMN));
	tagColumn.mask = LVCF_WIDTH|LVCF_TEXT;
	tagColumn.cx = 100;
	tagColumn.pszText = _T("IK�ļ�");	
	m_lstIkIFiles.InsertColumn(0, &tagColumn);
	tagColumn.cx = 100;
	tagColumn.pszText = _T("Txt�ļ�");
	m_lstIkIFiles.InsertColumn(1, &tagColumn);
	m_lstIkIFiles.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	m_cmbExtractOrder.SetCurSel(HY_EXTORDER_S);
	GetDlgItem(IDD_IK_SPLT_CTXT_EDT)->EnableWindow(FALSE);
	GetDlgItem(IDD_IK_SPLT_CTXT_BTN)->EnableWindow(FALSE);

}	// end of void CIKSplitDlg::IniCtrl()

void CIKSplitDlg::UpdateCtrl()
{

	if (m_cmbExtractOrder.GetCurSel()==HY_EXTORDER_C)
	{
		GetDlgItem(IDD_IK_SPLT_CTXT_EDT)->EnableWindow(TRUE);
		GetDlgItem(IDD_IK_SPLT_CTXT_BTN)->EnableWindow(TRUE);
	}
	else 
	{
		GetDlgItem(IDD_IK_SPLT_CTXT_EDT)->EnableWindow(FALSE);
		GetDlgItem(IDD_IK_SPLT_CTXT_BTN)->EnableWindow(FALSE);
	}

}	// end of void CIKSplitDlg::UpdateCtrl()

int	CIKSplitDlg::GetListCtrlSelectItem()
{
	int nItem = -1;
	POSITION pos = m_lstIkIFiles.GetFirstSelectedItemPosition();	
	if (pos)
	{
		nItem = m_lstIkIFiles.GetNextSelectedItem(pos);
	}
	return nItem;

}	// end of int CIKSplitDlg::GetListCtrlSelectItem()

void CIKSplitDlg::UninitCtrl()
{


}	// end of void CIKSplitDlg::UninitCtrl()

void CIKSplitDlg::UpdateListCtrl()
{
	m_lstIkIFiles.DeleteAllItems();
	for (INT_PTR i=0; i<m_aryIKFiles.GetSize(); i++)
	{
		LVITEM	tagLvItem;
		ZeroMemory(&tagLvItem, sizeof(LVITEM));

		tagLvItem.mask			= LVIF_TEXT;
		tagLvItem.iItem			= i;
		tagLvItem.iSubItem		= 0;		
		tagLvItem.pszText		= new char[MAX_PATH];
		tagLvItem.cchTextMax	= MAX_PATH;

		ZeroMemory(tagLvItem.pszText, MAX_PATH);
		strcpy_s(tagLvItem.pszText, MAX_PATH, ::HY_GetFullNameFromPath(std::string(m_aryIKFiles[i])).data());
		m_lstIkIFiles.InsertItem(&tagLvItem);
		tagLvItem.iSubItem		= 1;		
		ZeroMemory(tagLvItem.pszText, MAX_PATH);
		strcpy_s(tagLvItem.pszText, MAX_PATH, ::HY_GetFullNameFromPath(std::string(m_aryTxtFiles[i])).data());
		m_lstIkIFiles.SetItem(&tagLvItem);

		delete[] tagLvItem.pszText;
		tagLvItem.pszText = NULL;
	}

}	// end of void CIKSplitDlg::UpdateListCtrl()

void CIKSplitDlg::PrintMergeInfo(CString strInfo)
{	
	m_lstbMergerInfo.AddString(strInfo);	
	m_lstbMergerInfo.UpdateWindow();	

}	// end of void	CIKMergeDlg::PrintMergeInfo()

void CIKSplitDlg::ResetPrintMergeInfo()
{
	m_lstbMergerInfo.ResetContent();

}	// end of void	CIKMergeDlg::ResetPrintMergeInfo()

void CIKSplitDlg::Prepare27533()
{
	m_ikf27533.SetDefault();

	HYFONTCODEC::CIKCharacter	ikChar;
	CString& strAPP = ::XSysproxy().m_strAppDir;

	// cjk 20902
	CString strCJKFile;
	strCJKFile.Format(_T("%s%s\\%s"),strAPP,"data","cjk.txt");
	std::vector<unsigned long> vtUnicode;	 
	HY_TXTFileToUniArray((LPTSTR)(LPCTSTR)strCJKFile,vtUnicode);
	size_t szUni = vtUnicode.size();	
	for (size_t i=0; i<szUni; i++)
	{		
		ikChar.SetDefault();
		ikChar.MapCode = vtUnicode[i];
		m_ikf27533.vtCharacter.push_back(ikChar);
	}

	// cjka 6530
	vtUnicode.clear();
	CString strCJKAFile;
	strCJKAFile.Format(_T("%s%s\\%s"),strAPP,"data","cjkA.txt");	
	HY_TXTFileToUniArray((LPTSTR)(LPCTSTR)strCJKAFile,vtUnicode);
	szUni = vtUnicode.size();
	for (size_t i=0; i<szUni; i++)
	{
		ikChar.SetDefault();
		ikChar.MapCode = vtUnicode[i];
		m_ikf27533.vtCharacter.push_back(ikChar);
	}

	// cjk 101
	vtUnicode.clear();
	CString strCJK101File;
	strCJK101File.Format(_T("%s%s\\%s"),strAPP,"data","cjk101.txt");	
	HY_TXTFileToUniArray((LPTSTR)(LPCTSTR)strCJK101File,vtUnicode);
	szUni = vtUnicode.size();
	for (size_t i=0; i<szUni; i++)
	{
		ikChar.SetDefault();
		ikChar.MapCode = vtUnicode[i];
		m_ikf27533.vtCharacter.push_back(ikChar);
	}

}	// end of void	CIKSplitDlg::Prepare27533();

void CIKSplitDlg::IkMerge()
{	
	CString strInfo;
	INT_PTR iFileSz = m_aryIKFiles.GetSize();
	if (iFileSz==0) return;

	Prepare27533();
	for (INT_PTR i=0; i<iFileSz; i++)
	{
		if (i==0)
		{			
			IkSmlTo27533(m_aryIKFiles[i],m_aryTxtFiles[i],TRUE);
		}
		else 
		{
			IkSmlTo27533(m_aryIKFiles[i],m_aryTxtFiles[i],FALSE);
		}		
	}

}	// end of int		CIKSplitDlg::IkMerge()

void	CIKSplitDlg::IkSmlTo27533(CString strSIKFile, CString strSTXT,BOOL bHead)
{	
	HYFONTCODEC::CHYIkCodec IkCodec;
	if (HY_NOERROR!=IkCodec.Decode((LPTSTR)(LPCTSTR)strSIKFile)) 
		return;

	if (bHead)
	{
		m_ikf27533.Header=IkCodec.m_IKFont.Header;
		m_ikf27533.Header.CharIndexInf.SetDefault();
	}

	std::vector<unsigned long>	vtUnicode;
	HY_TXTFileToUniArray((LPTSTR)(LPCTSTR)strSTXT,vtUnicode);	
	size_t	szUni	= vtUnicode.size();
	int		iRt		= HY_NOERROR;
	CString strInf;
	for (size_t i=0; i<szUni; i++)
	{	
		HYFONTCODEC::CIKCharacter&	IkChar		= IkCodec.m_IKFont.vtCharacter[i];
		unsigned long&				ulUnicode	= vtUnicode[i];		

		size_t stIKIndex = m_ikf27533.IkCharExistByMapCode(ulUnicode);		
		if (stIKIndex!=-1)
		{	
			IkChar.MapCode = ulUnicode;
			if (m_ikf27533.vtCharacter[stIKIndex].CharNameInf.sCharNumber!=0)
			{
				strInf.Format(_T("unicode 0x%X�ظ�,�Ѹ���"),ulUnicode);
				PrintMergeInfo(strInf);
			}
			m_ikf27533.SetIkChar(stIKIndex,IkChar);
		}
		else
		{
			strInf.Format(_T("UniCode=%d ������27533����,IKCharNum=%d,IKFileName=%s"),
				ulUnicode, IkChar.CharNameInf.sCharNumber, 
				HY_GetFullNameFromPath(std::string(strSIKFile)).data());
			PrintMergeInfo(strInf);			
		}	
	}	

}	// end of void	IkSmlTo27533();

void	CIKSplitDlg::Extract(CString strDstIKFile, int iExtractOrder)
{
	CString						strInf;
	HYFONTCODEC::CHYIkCodec		IKCodec;
	std::vector<unsigned long>	vtMDUni;
	HYFONTCODEC::CIKFont		IKSFont;

	IKSFont.Header = m_ikf27533.Header;
	strcpy_s(IKSFont.Header.Name.FileName,IK_FILENAME_LENG,HY_GetFullNameFromPath(std::string(strDstIKFile)).data());

	size_t sz27533Char = m_ikf27533.vtCharacter.size();
	if (iExtractOrder==HY_EXTORDER_S)
	{
		size_t szSUni = m_vtS27533.size();
		for(size_t i=0; i<szSUni; i++)
		{	
			unsigned long ulSUni = m_vtS27533[i];
			for (size_t j=0; j<sz27533Char; j++)
			{	
				HYFONTCODEC::CIKCharacter&	ikBChar = m_ikf27533.vtCharacter[j];				
				if (ikBChar.MapCode==ulSUni)
				{
					ikBChar.CharNameInf.sCharNumber = (short)(i+1);
					size_t iCharIndex = IKSFont.IkCharExistByMapCode(ulSUni);
					if (iCharIndex !=-1 )
					{					
						IKSFont.SetIkChar(iCharIndex,ikBChar);						
						break;
					}
					else 
					{				
						if (ikBChar.ContourInfo.vtContourIndex.size()>0)
							IKSFont.vtCharacter.push_back(ikBChar);
						break;
					}
				}		
			}		
		}
	}

	if (iExtractOrder==HY_EXTORDER_P)
	{
		size_t szPUni = m_vtP27533.size();
		for(size_t i=0; i<szPUni; i++)
		{	
			unsigned long ulPUni = m_vtP27533[i];
			for (size_t j=0; j<sz27533Char; j++)
			{	
				HYFONTCODEC::CIKCharacter&	ikBChar = m_ikf27533.vtCharacter[j];				
				if (ikBChar.MapCode==ulPUni)
				{
					ikBChar.CharNameInf.sCharNumber = (short)(i+1);
					size_t iCharIndex = IKSFont.IkCharExistByMapCode(ulPUni);
					if (iCharIndex !=-1 )
					{				
						IKSFont.SetIkChar(iCharIndex,ikBChar);
						break;
					}
					else 
					{				
						if (ikBChar.ContourInfo.vtContourIndex.size()>0)
							IKSFont.vtCharacter.push_back(ikBChar);
						break;
					}
				}		
			}		
		}
	}

	if (iExtractOrder==HY_EXTORDER_C)
	{
		size_t szCUni = m_vtCustom.size();
		for(size_t i=0; i<szCUni; i++)
		{	
			unsigned long ulCUni = m_vtCustom[i];
			for (size_t j=0; j<sz27533Char; j++)
			{	
				HYFONTCODEC::CIKCharacter&	ikBChar = m_ikf27533.vtCharacter[j];				
				if (ikBChar.MapCode==ulCUni)
				{
					ikBChar.CharNameInf.sCharNumber = (unsigned short)(i+1);
					size_t iCharIndex = IKSFont.IkCharExistByMapCode(szCUni);
					if (iCharIndex !=-1 )
					{
						IKSFont.SetIkChar(iCharIndex,ikBChar);
						break;
					}
					else 
					{				
						if (ikBChar.ContourInfo.vtContourIndex.size()>0)
							IKSFont.vtCharacter.push_back(ikBChar);
						break;
					}
				}		
			}		
		}		
	}

	//Ϊ�˱�֤��TXT����һ�������ﲻ��IKSFont��������
	size_t sz = IKSFont.vtCharacter.size();
	for(size_t i=0; i<sz; i++)
	{
		CIKCharacter&		ikChar = IKSFont.vtCharacter[i];
		CIKCharIndexInf&	CharIndexInf = IKSFont.Header.CharIndexInf;
		CIKCharIndex		CharIndex;
		CharIndex.sCharNumber = ikChar.CharNameInf.sCharNumber;
		CharIndexInf.vtCharIndex.push_back(CharIndex);		
	}

	HYFONTCODEC::CHYIkCodec IKCdec;
	if (IKCdec.Encode((LPTSTR)(LPCTSTR)strDstIKFile,IKSFont)!=HY_NOERROR)
	{
		strInf.Format(_T("%s�ļ�����ʧ�ܡ�"), HY_GetFileNameFromPath(std::string(strDstIKFile)).data());
		PrintMergeInfo(strInf);
	}

	PrintMergeInfo(_T("IK�ļ����ɳɹ�"));	

}	// end of void	CIKSplitDlg::Extract()

// CIKSplitDlg message handlers
void CIKSplitDlg::OnBnClickedIkSpltBtn()
{		
	TCHAR szFilters[]= _T("IK �ļ�(*.ik)|*.ik||");
	CFileDialog  OpenFileDlg(FALSE, _T(""), _T(""), OFN_READONLY|OFN_LONGNAMES|OFN_FILEMUSTEXIST,szFilters);
	if (OpenFileDlg.DoModal() != IDOK) return;
	ResetPrintMergeInfo();
	CString strSplit = OpenFileDlg.GetPathName();

	if (HY_FileExist((LPTSTR)(LPCTSTR)strSplit)>0)
	{
		if ( AfxMessageBox(_T("�ļ��Ѵ��ڣ������Ƿ񸲸ǣ�"),MB_YESNO) == IDNO) 
			return;		
	}

	INT_PTR iFileSz = m_aryIKFiles.GetSize();
	if(iFileSz==0)
	{
		PrintMergeInfo(_T("������Ҫһ��IKԴ�ļ�"));
		return;
	}

	CWaitCursor wc;
	IkMerge();

	int iExtractOrder = m_cmbExtractOrder.GetCurSel();
	Extract(strSplit,iExtractOrder);

	if(m_lstbMergerInfo.GetCount()>0)
		m_lstbMergerInfo.SetCurSel(m_lstbMergerInfo.GetCount()-1);

}	// end of void CIKSplitDlg::OnBnClickedIkSpltBtn()

BOOL CIKSplitDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	IniCtrl();
	UpdateCtrl();

	CString str27533P;
	str27533P.Format(_T("%s\\data\\27533P.txt"),::XSysproxy().m_strAppDir);
	CString str27533S;
	str27533S.Format(_T("%s\\data\\27533S.txt"),::XSysproxy().m_strAppDir);

	if (HY_TXTFileToUniArray((LPTSTR)(LPCTSTR)str27533P,m_vtP27533) != HY_NOERROR)
	{
		AfxMessageBox(_T("27533ƫ�������ı��ļ�������!"));
	}

	if (HY_TXTFileToUniArray((LPTSTR)(LPCTSTR)str27533S,m_vtS27533) != HY_NOERROR)
	{
		AfxMessageBox(_T("27533���������ı��ļ�������!"));
	}

	return TRUE;

}	// end of BOOL CIKSplitDlg::OnInitDialog()

void CIKSplitDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: Add your message handler code here

}	// end of void CIKSplitDlg::OnDestroy()

void CIKSplitDlg::OnBnClickedIkSpltCtxtBtn()
{
	TCHAR szFilters[]= _T("txt �ļ�(*.txt)|*.txt||");
	CFileDialog  OpenFileDlg(TRUE, _T(""), _T(""), OFN_READONLY|OFN_LONGNAMES|OFN_FILEMUSTEXIST,szFilters);	
	if (OpenFileDlg.DoModal() != IDOK) return;

	m_strCustom = OpenFileDlg.GetPathName();
	UpdateData(FALSE);

}	// end of void CIKSplitDlg::OnBnClickedIkSpltCtxtBtn()

void CIKSplitDlg::OnBnClickedIkSpltIkfBtn()
{
	TCHAR szFilters[]= _T("IK �ļ�(*.ik)|*.ik||");
	CFileDialog  OpenFileDlg(TRUE, _T(""), _T(""), OFN_READONLY|OFN_LONGNAMES|OFN_FILEMUSTEXIST,szFilters);	
	if (OpenFileDlg.DoModal() != IDOK) return;

	m_strSrcIK = OpenFileDlg.GetPathName();
	UpdateData(FALSE);

}	// end of void CIKSplitDlg::OnBnClickedIkSpltIkfBtn()

void CIKSplitDlg::OnBnClickedIkSpltTxtfBtn()
{
	TCHAR szFilters[]= _T("txt �ļ�(*.txt)|*.txt||");
	CFileDialog  OpenFileDlg(TRUE, _T(""), _T(""), OFN_READONLY|OFN_LONGNAMES|OFN_FILEMUSTEXIST,szFilters);	
	if (OpenFileDlg.DoModal() != IDOK) return;

	m_strSrcTXT = OpenFileDlg.GetPathName();
	UpdateData(FALSE);
	
}	// end of void CIKSplitDlg::OnBnClickedIkSpltTxtfBtn()

void CIKSplitDlg::OnLvnItemchangedIkSpltLst(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here


	*pResult = 0;

}	// end of void CIKSplitDlg::OnLvnItemchangedIkSpltLst()

void CIKSplitDlg::OnBnClickedIkSpltAddBtn()
{
	UpdateData();

	if(m_strSrcTXT.IsEmpty()) 
	{
		AfxMessageBox(_T("Txt�ļ���Ϊ��"));
		return; 
	}

	if(m_strSrcIK.IsEmpty()) 
	{
		AfxMessageBox(_T("IK�ļ���Ϊ��"));
		return; 
	}

	m_aryTxtFiles.Add(m_strSrcTXT);
	m_aryIKFiles.Add(m_strSrcIK);

	UpdateListCtrl();
	
}	// end of void CIKSplitDlg::OnBnClickedIkSpltAddBtn()

void CIKSplitDlg::OnBnClickedIkSpltDelBtn()
{
	int iSltInx = GetListCtrlSelectItem();
	if (iSltInx==-1) return;

	m_aryTxtFiles.RemoveAt(iSltInx);
	m_aryIKFiles.RemoveAt(iSltInx);

	UpdateListCtrl();

	int iIkSZ = m_aryIKFiles.GetSize();
	m_lstIkIFiles.SetFocus();
	if (iIkSZ>iSltInx)
		m_lstIkIFiles.SetItemState(iSltInx, LVIS_SELECTED|LVIS_FOCUSED,  LVIS_SELECTED|LVIS_FOCUSED);
	else
		m_lstIkIFiles.SetItemState(iSltInx-1>0 ?iSltInx-1:0, LVIS_SELECTED|LVIS_FOCUSED,  LVIS_SELECTED|LVIS_FOCUSED);
	
}	// end of void CIKSplitDlg::OnBnClickedIkSpltDelBtn()

void CIKSplitDlg::OnCbnSelchangeIkSporderCmb()
{
	UpdateCtrl();

}	// end of void CIKSplitDlg::OnCbnSelchangeIkSporderCmb()
