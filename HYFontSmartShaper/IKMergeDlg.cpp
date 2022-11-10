// IKMergeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "HYFontSmartShaper.h"
#include "IKMergeDlg.h"


// CIKMergeDlg dialog

IMPLEMENT_DYNAMIC(CIKMergeDlg, CDialog)

CIKMergeDlg::CIKMergeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CIKMergeDlg::IDD, pParent)	
{
	InitParam();

}	// end of CIKMergeDlg::CIKMergeDlg()

CIKMergeDlg::~CIKMergeDlg()
{
	UninitParam();

}	// end of CIKMergeDlg::~CIKMergeDlg()

void CIKMergeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDD_IK_MRG_LST, m_lstIkIFiles);
	DDX_Control(pDX, IDD_IK_MRG_INF_LST, m_lstbMergerInfo);
	
}	// end of void CIKMergeDlg::DoDataExchange()

BEGIN_MESSAGE_MAP(CIKMergeDlg, CDialog)	
	ON_NOTIFY(LVN_ITEMCHANGED, IDD_IK_MRG_LST, &CIKMergeDlg::OnLvnItemchangedIkMrgLst)	
	ON_BN_CLICKED(IDD_IK_MRG_DEL_BTN, &CIKMergeDlg::OnBnClickedIkMrgDelBtn)
	ON_BN_CLICKED(IDD_IK_MRG_BTN, &CIKMergeDlg::OnBnClickedIkMrgBtn)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDD_IK_MRG_UP_BTN, &CIKMergeDlg::OnBnClickedIkMrgUpBtn)
	ON_BN_CLICKED(IDD_IK_MRG_DOWN_BTN, &CIKMergeDlg::OnBnClickedIkMrgDownBtn)
	ON_BN_CLICKED(IDD_IK_MRG_IKF_BTN, &CIKMergeDlg::OnBnClickedIkMrgIkfBtn)
	ON_BN_CLICKED(IDD_IK_MRG_CLEAR_BTN, &CIKMergeDlg::OnBnClickedIkMrgClearBtn)
END_MESSAGE_MAP()

void CIKMergeDlg::SetDefault()
{
	m_aryIKFiles.RemoveAll();	
	m_lstbMergerInfo.ResetContent();
	m_MergeIKHd.SetDefault();

}	// end of void	CIKMergeDlg::SetDefault()

void CIKMergeDlg::InitParam()
{
	m_aryIKFiles.RemoveAll();
	
}	// end of void	CIKMergeDlg::InitParam()

void CIKMergeDlg::UninitParam()
{

}	// end of void	CIKMergeDlg::UninitParam()

void CIKMergeDlg::IniCtrl()
{
	LVCOLUMN		tagColumn;
	ZeroMemory(&tagColumn, sizeof(LVCOLUMN));

	tagColumn.mask = LVCF_WIDTH|LVCF_TEXT;

	tagColumn.cx = 250;
	tagColumn.pszText = _T("IK文件");	
	m_lstIkIFiles.InsertColumn(0, &tagColumn);
	m_lstIkIFiles.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);	

}	// end of void	CIKMergeDlg::IniCtrl()

void CIKMergeDlg::UninitCtrl()
{

}	// end of void	CIKMergeDlg::UninitCtrl()

void CIKMergeDlg::UpdateListCtrl()
{
	m_lstIkIFiles.DeleteAllItems();
	for (INT_PTR i=0; i<m_aryIKFiles.GetSize(); i++)
	{
		LVITEM	tagLvItem;
		ZeroMemory(&tagLvItem, sizeof(LVITEM));

		tagLvItem.mask			= LVIF_TEXT|LVIF_STATE;
		tagLvItem.iItem			= i;
		tagLvItem.iSubItem		= 0;		
		tagLvItem.pszText		= new char[MAX_PATH];
		tagLvItem.cchTextMax	= MAX_PATH;
		tagLvItem.stateMask		= -1;
		tagLvItem.state			= 0;


		ZeroMemory(tagLvItem.pszText, MAX_PATH);
		strcpy_s(tagLvItem.pszText, MAX_PATH, ::HY_GetFullNameFromPath(std::string(m_aryIKFiles[i])).data());
		m_lstIkIFiles.InsertItem(&tagLvItem);

		delete[] tagLvItem.pszText;		
	}
	m_lstIkIFiles.SetFocus();

}	// end of void	CIKMergeDlg::UpdateListCtrl()

int	CIKMergeDlg::GetListCtrlSelectItem()
{
	int nItem = -1;
	POSITION pos = m_lstIkIFiles.GetFirstSelectedItemPosition();	
	if (pos)
	{
		nItem = m_lstIkIFiles.GetNextSelectedItem(pos);
	}
	return nItem;

}	// end of int CIKMergeDlg::GetListCtrlSelectItem()

void CIKMergeDlg::PrintMergeInfo(CString strInfo)
{
	m_lstbMergerInfo.AddString(strInfo);
	m_lstbMergerInfo.UpdateWindow();	

}	// end of void	CIKMergeDlg::PrintMergeInfo()

void CIKMergeDlg::ResetPrintMergeInfo()
{
	m_lstbMergerInfo.ResetContent();

}	// end of void	CIKMergeDlg::ResetPrintMergeInfo()

int	CIKMergeDlg::IkMerge(CString strDstIK)
{	
	CString strInfo;

	m_IKMergeFont.SetDefault();
	m_MergeIKHd.SetDefault();	

	ResetPrintMergeInfo();
	
	INT_PTR iFileSz = m_aryIKFiles.GetSize();
	for (INT_PTR i=0; i<iFileSz; i++)
	{
		if (i==0)
		{			
			IkS2B(m_aryIKFiles[i],TRUE);
		}
		else 
		{
			IkS2B(m_aryIKFiles[i],FALSE);
		}		
	}

	m_IKMergeFont.SortCharASC();
	size_t sz = m_IKMergeFont.vtCharacter.size();
	for(size_t i=0; i<sz; i++)
	{
		CIKCharacter&		ikChar = m_IKMergeFont.vtCharacter[i];
		CIKCharIndexInf&	CharIndexInf = m_IKMergeFont.Header.CharIndexInf;
		CIKCharIndex		CharIndex;
		CharIndex.sCharNumber = ikChar.CharNameInf.sCharNumber;
		CharIndexInf.vtCharIndex.push_back(CharIndex);
	}

	CString strInf;
	HYFONTCODEC::CHYIkCodec IKCdec;
	if (IKCdec.Encode((LPTSTR)(LPCTSTR)strDstIK,m_IKMergeFont)!=HY_NOERROR)
	{
		strInf.Format(_T("%s文件生成失败。"), HY_GetFileNameFromPath(std::string(strDstIK)).data());
		PrintMergeInfo(strInf);
	}

	PrintMergeInfo(_T("IK文件生成成功"));
	
	if(m_lstbMergerInfo.GetCount()>0)
		m_lstbMergerInfo.SetCurSel(m_lstbMergerInfo.GetCount()-1);

	return HY_NOERROR;

}	// end of int 	CIKMergeDlg::IkMerge()

int	CIKMergeDlg::IkS2B(CString strSIKFile, BOOL bHead)
{	
	int iRT = HY_NOERROR;

	HYFONTCODEC::CHYIkCodec		ikCodec;
	if (HY_NOERROR!=ikCodec.Decode((LPTSTR)(LPCTSTR)strSIKFile))
	{
		return iRT;
	}

	if (bHead)
	{
		m_IKMergeFont.Header = ikCodec.m_IKFont.Header;
		m_IKMergeFont.Header.CharIndexInf.SetDefault();
	}

	size_t szCodecIK = ikCodec.m_IKFont.vtCharacter.size();
	CString strInf;
	for(size_t i=0; i<szCodecIK;i++)
	{		
		CIKCharacter& ikCdcChar = ikCodec.m_IKFont.vtCharacter[i];
		size_t stIKIndex = m_IKMergeFont.IkCharExistByIKNumber(ikCdcChar.CharNameInf.sCharNumber);
		if (stIKIndex != -1)
		{
			m_IKMergeFont.SetIkChar(stIKIndex,ikCdcChar);
			strInf.Format(_T("IkNumber %d重复,已覆盖"),ikCdcChar.CharNameInf.sCharNumber);
			PrintMergeInfo(strInf);
		}
		else
		{
			m_IKMergeFont.vtCharacter.push_back(ikCdcChar);
		}		
	}

	return HY_NOERROR;

}	// end of int CIKMergeDlg::IkS2B()

// CIKMergeDlg message handlers
void CIKMergeDlg::OnLvnItemchangedIkMrgLst(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

}	// end of void CIKMergeDlg::OnLvnItemchangedIkMrgLst()

void CIKMergeDlg::OnBnClickedIkMrgDelBtn()
{
	int iSltInx = GetListCtrlSelectItem();
	if (iSltInx==-1) return;
	m_aryIKFiles.RemoveAt(iSltInx);

	UpdateListCtrl();

	int iIkSZ = m_aryIKFiles.GetSize();	
	if (iIkSZ>iSltInx)
		m_lstIkIFiles.SetItemState(iSltInx, LVIS_SELECTED|LVIS_FOCUSED,  LVIS_SELECTED|LVIS_FOCUSED );
	else
		m_lstIkIFiles.SetItemState(iSltInx-1>0 ?iSltInx-1:0, LVIS_SELECTED|LVIS_FOCUSED,  LVIS_SELECTED|LVIS_FOCUSED );

}	// end of void CIKMergeDlg::OnBnClickedIkMrgDelBtn()

void CIKMergeDlg::OnBnClickedIkMrgBtn()
{
	TCHAR szFilters[]=_T("IK 文件(*.ik)|*.ik||");
	CFileDialog  saveFileDlg(FALSE, _T(""), _T(""), OFN_READONLY|OFN_LONGNAMES|OFN_FILEMUSTEXIST,szFilters);
	if (saveFileDlg.DoModal() != IDOK)	return;			

	CWaitCursor					wc;
	CString strFileName = saveFileDlg.GetPathName();

	std::string strFN = HY_GetFullNameFromPath(std::string(strFileName)).data();
	const char* pFullName = strFN.data();
	int iFNLen = strlen(pFullName);
	if (iFNLen+1 >=IK_FILENAME_LENG)
	{
		PrintMergeInfo("文件名过长！不能超过11个字符");
		return;
	}
	
	if (HY_FileExist((LPTSTR)(LPCTSTR)strFileName)>0)
	{
		if ( AfxMessageBox(_T("文件已存在，请问是否覆盖？"),MB_YESNO) == IDYES)
		{
			IkMerge(strFileName);
			return;
		}
	}

	IkMerge(strFileName);

}	// end of void CIKMergeDlg::OnBnClickedIkMrgBtn()

BOOL CIKMergeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	IniCtrl();

	return TRUE;
	
}	// end of BOOL CIKMergeDlg::OnInitDialog()

void CIKMergeDlg::OnDestroy()
{
	CDialog::OnDestroy();

	UninitCtrl();

}	// end of void CIKMergeDlg::OnDestroy()

void CIKMergeDlg::OnBnClickedIkMrgUpBtn()
{
	m_lstIkIFiles.SetFocus();
	int iIndex = GetListCtrlSelectItem();
	if (iIndex!=-1)
	{
		if (iIndex!=0)
		{
			CString strTmp = m_aryIKFiles[iIndex];
			m_aryIKFiles.RemoveAt(iIndex);
			m_aryIKFiles.InsertAt(iIndex-1,strTmp);			
			UpdateListCtrl();			
			m_lstIkIFiles.SetItemState(iIndex-1, LVIS_SELECTED|LVIS_FOCUSED, -1);
		}
	}

}	// end of void CIKMergeDlg::OnBnClickedIkMrgUpBtn()

void CIKMergeDlg::OnBnClickedIkMrgDownBtn()
{	
	m_lstIkIFiles.SetFocus();
	UINT iIndex = GetListCtrlSelectItem();
	if (iIndex!=-1)
	{
		if (iIndex<m_aryIKFiles.GetCount()-1)
		{
			CString strTmp = m_aryIKFiles[iIndex];
			m_aryIKFiles.RemoveAt(iIndex);
			m_aryIKFiles.InsertAt(iIndex+1,strTmp);
			UpdateListCtrl();
			m_lstIkIFiles.SetItemState(iIndex+1, LVIS_SELECTED|LVIS_FOCUSED, -1);
		}
	}

}	// end of void CIKMergeDlg::OnBnClickedIkMrgDownBtn()

void CIKMergeDlg::OnBnClickedIkMrgIkfBtn()
{
	TCHAR	szFilters[]=_T("IK 文件(*.ik)|*.ik||");
	DWORD	MAXFILE = 2048;
	TCHAR*	pc = new TCHAR[MAXFILE*(MAX_PATH+1)+1];
	CFileDialog  openFileDlg(TRUE, _T(""), _T(""), OFN_ALLOWMULTISELECT|OFN_READONLY|OFN_LONGNAMES|OFN_FILEMUSTEXIST,szFilters);
	if (openFileDlg.DoModal() != IDOK)	return;

	int nCount = 0;  
	POSITION pos = openFileDlg.GetStartPosition();  
	while (pos!=NULL)  
	{  
		CString szPathName = openFileDlg.GetNextPathName(pos);
		m_aryIKFiles.Add(szPathName);		
	}
	delete[] pc;

	UpdateListCtrl();

}	// end of void CIKMergeDlg::OnBnClickedIkMrgIkfBtn()

void CIKMergeDlg::OnBnClickedIkMrgClearBtn()
{
	m_aryIKFiles.RemoveAll();
	m_lstIkIFiles.DeleteAllItems();		
	ResetPrintMergeInfo();

}	// end of void CIKMergeDlg::OnBnClickedIkMrgClearBtn()
