#include "StdAfx.h"
#include "XSysBaseProxy.h"

CXSysBaseProxy::CXSysBaseProxy(void)
{
	InitParam();

}	// end of CXSysBaseProxy::CXSysBaseProxy()

CXSysBaseProxy::~CXSysBaseProxy(void)
{
	UninitParm();

}	// end of CXSysBaseProxy::~CXSysBaseProxy()

BOOL CXSysBaseProxy::Init(CString strHelpFile)
{
	// µ±Ç°Ä¿Â¼
	int Path =  260;
	char pPath[MAX_PATH] = {0};
	GetCurrentDirectory(MAX_PATH,pPath);
	m_strAppDir.Format(_T("%s\\"),pPath);

	TCHAR   szBuffer[MAX_PATH]; 
	ZeroMemory(szBuffer, MAX_PATH);
	GetSystemDirectory(szBuffer, MAX_PATH); 
	m_strSysDir.Format(_T("%s\\"), szBuffer);

	LoadLocalProfile();

	return TRUE;

}	// end of BOOL CXSysBaseProxy::Init()

void CXSysBaseProxy::Uninit()
{
	StoreLocalProfile();

}	// end of void CXSysBaseProxy::Uninit()

void CXSysBaseProxy::InitParam()
{
	m_strAppDir				= _T("");
	m_strSysDir				= _T("");
	m_pMainFrame			= NULL;	

}	// end of void CXSysBaseProxy::InitParam()

void CXSysBaseProxy::UninitParm()
{

}	// end of void CXSysBaseProxy::UninitParm()

BOOL CXSysBaseProxy::IsWndValid(CWnd* pWnd)
{
	if ( pWnd == NULL )						return FALSE;
	if ( pWnd->GetSafeHwnd() == NULL )		return FALSE;

	return TRUE;

}	// end of BOOL CXSysBaseProxy::IsWndValid()

LRESULT	CXSysBaseProxy::UpdateAllViews(UINT nMsg, LPARAM lParam)
{
	if ( !IsWndValid(m_pMainFrame) )	return 0L;

	return m_pMainFrame->SendMessage(VM_UPDATEALLVIEWS, (WPARAM)nMsg, lParam);
	//return m_pMainFrame->PostMessageA(VM_UPDATEALLVIEWS, (WPARAM)nMsg, lParam);

}	// end of LRESULT	CXSysBaseProxy::UpdateAllViews()

void CXSysBaseProxy::LoadLocalProfile()
{		

}	// end of void CXSysBaseProxy::LoadLocalProfile()

void CXSysBaseProxy::StoreLocalProfile()
{
	
}	// end of void CXSysBaseProxy::StoreLocalProfile()
