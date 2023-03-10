#include "StdAfx.h"
#include "XSysBaseProxy.h"
#include "HYGsub.h"

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
	::ZeroMemory(&m_tagOpeionPrm,sizeof(HY_OPTION_PRM));

	// 当前目录
	int Path =  260;
	char pPath[MAX_PATH] = {0};
	GetCurrentDirectory(MAX_PATH,pPath);
	m_strAppDir.Format(_T("%s\\"),pPath);

	m_strLogFile = string(m_strAppDir)+"log.txt";

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
	m_strLogFile			= _T("");
	m_pMainFrame			= NULL;	
	ZeroMemory(&m_tagDrawParam, sizeof(DRAWFONTPARAM));
	m_tagDrawParam.iLine	= 1;
	m_tagDrawParam.iPoint	= 1;
	m_fBaseLine				= 0.0f;

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

}	// end of LRESULT	CXSysBaseProxy::UpdateAllViews()

void CXSysBaseProxy::LoadLocalProfile()
{	
	CString strProfileName = m_strAppDir + _T("data\\SmartSharperSetUp.ini");
	TCHAR	szInfo[MAX_PATH] = {0};

	/////////////////////////////////////////////////////////////////////////////////////////////
	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("SHOW PARAM", "Show Point", _T("0"), szInfo, MAX_PATH, strProfileName);	
	m_tagDrawParam.iPoint = atoi(szInfo);
	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("SHOW PARAM", "Show Point Number", _T("0"), szInfo, MAX_PATH, strProfileName);	
	m_tagDrawParam.iPointNo = atoi(szInfo);
	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("SHOW PARAM", "Show Line", _T("1"), szInfo, MAX_PATH, strProfileName);	
	m_tagDrawParam.iLine = 1;
	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("SHOW PARAM", "Show ReferLine", _T("1"), szInfo, MAX_PATH, strProfileName);	
	m_tagDrawParam.iReferLine= atoi(szInfo);
	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("SHOW PARAM", "Fill Contours", _T("0"), szInfo, MAX_PATH, strProfileName);	
	m_tagDrawParam.iFill= atoi(szInfo);
	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("SHOW PARAM", "View Type", _T("0"), szInfo, MAX_PATH, strProfileName);	
	m_tagDrawParam.iViewType= atoi(szInfo);

	// OPtion
	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("Option", "VERT", _T("1"), szInfo, MAX_PATH, strProfileName);
	m_tagOpeionPrm.bCmplVert = atoi(szInfo)==0 ? FALSE : TRUE;
	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("Option", "PSNAME", _T("0"), szInfo, MAX_PATH, strProfileName);
	m_tagOpeionPrm.bRePsName = atoi(szInfo) == 0 ? FALSE : TRUE;
	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("Option", "SORTUNI", _T("0"), szInfo, MAX_PATH, strProfileName);
	m_tagOpeionPrm.bReSortUni = atoi(szInfo) == 0 ? FALSE: TRUE;
	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("Option", "YITIZI", _T("0"), szInfo, MAX_PATH, strProfileName);
	m_tagOpeionPrm.bYitizi = atoi(szInfo) == 0 ? FALSE: TRUE;
	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("Option", "OLDSTANDARD", _T("1"), szInfo, MAX_PATH, strProfileName);
	m_tagOpeionPrm.bOldStandard = atoi(szInfo) == 0 ? FALSE : TRUE;

	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("Option", "KANGXI", _T("0"), szInfo, MAX_PATH, strProfileName);
	m_tagOpeionPrm.bKangXi = atoi(szInfo) == 0 ? FALSE : TRUE;
	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("Option", "CORRECT", _T("0"), szInfo, MAX_PATH, strProfileName);
	m_tagOpeionPrm.bCnturCorrect = atoi(szInfo) == 0 ? FALSE : TRUE;
	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("Option", "RENAME", _T("0"), szInfo, MAX_PATH, strProfileName);
	m_tagOpeionPrm.bFontname = atoi(szInfo) == 0 ? FALSE : TRUE;
	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("Option", "CMAP", _T("1"), szInfo, MAX_PATH, strProfileName);
	m_tagOpeionPrm.bCmplCMAP = atoi(szInfo) == 0 ? FALSE : TRUE;
	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("Option", "SETADW", _T("0"), szInfo, MAX_PATH, strProfileName);
	m_tagOpeionPrm.bsetADW = atoi(szInfo) == 0 ? FALSE : TRUE;
	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("Option", "SETADH", _T("0"), szInfo, MAX_PATH, strProfileName);
	m_tagOpeionPrm.bsetADH = atoi(szInfo) == 0 ? FALSE : TRUE;
	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("Option", "ADH", _T("0"), szInfo, MAX_PATH, strProfileName);
	m_tagOpeionPrm.usSetADH = atoi(szInfo);
	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("Option", "LAYOUT", _T("0"), szInfo, MAX_PATH, strProfileName);
	m_tagOpeionPrm.bCmplLayout = atoi(szInfo) == 0 ? FALSE : TRUE;
	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("Option", "HANYI", _T("0"), szInfo, MAX_PATH, strProfileName);
	m_tagOpeionPrm.bHanyi = atoi(szInfo) == 0 ? FALSE : TRUE;

	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("HEAD", _T("PEREM"), _T("1000"), szInfo, MAX_PATH, strProfileName);
	m_tagOpeionPrm.usUnitsPerEM = atoi(szInfo);
	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("Typo_Ascender", _T("Ascender"), _T("850"), szInfo, MAX_PATH, strProfileName);
	m_tagOpeionPrm.sTypo_Ascender = atoi(szInfo);
	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("Typo_Descender", _T("Descender"), _T("-150"), szInfo, MAX_PATH, strProfileName);	
	m_tagOpeionPrm.sTypo_Descender = atoi(szInfo);
	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("OS", "Weight", _T("400"), szInfo, MAX_PATH, strProfileName);
	m_tagOpeionPrm.usWeight = strtoul(szInfo, NULL, 0);
	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("OS", "Width", _T("5"), szInfo, MAX_PATH, strProfileName);
	m_tagOpeionPrm.usWidth = strtoul(szInfo, NULL, 0);
	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("OS", "CodePageRang1", _T("0x0004009f"), szInfo, MAX_PATH, strProfileName);
	m_tagOpeionPrm.ulCodePageRang1 = strtoul(szInfo, NULL, 16);
	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("OS", "CodePageRang2", _T("0x00000000"), szInfo, MAX_PATH, strProfileName);
	m_tagOpeionPrm.ulCodePageRang2 = strtoul(szInfo, NULL, 16);
	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("CFF", "CID", _T("1"), szInfo, MAX_PATH, strProfileName);
	m_tagOpeionPrm.bCIDFnt = atoi(szInfo) == 0 ? FALSE : TRUE;
	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("CFF", "ROS_Registroy", _T("Adobe"), szInfo, MAX_PATH, strProfileName);
	CopyMemory(m_tagOpeionPrm.Registry, szInfo,MAX_PATH);
	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("CFF", "ROS_Ordering", _T("Identity"), szInfo, MAX_PATH, strProfileName);
	CopyMemory(m_tagOpeionPrm.Ordering, szInfo, MAX_PATH);
	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("CFF", "ROS_Supplement", _T("0"), szInfo, MAX_PATH, strProfileName);
	CopyMemory(m_tagOpeionPrm.Supplement, szInfo, MAX_PATH);

	GetPrivateProfileString("FONTName", "CHSFaimly", _T("汉仪测试"), szInfo, MAX_PATH, strProfileName);
	CopyMemory(m_tagOpeionPrm.CHSFaimlyName, szInfo, MAX_PATH);
	GetPrivateProfileString("FONTName", "CHSSub", _T("Regular"), szInfo, MAX_PATH, strProfileName);
	CopyMemory(m_tagOpeionPrm.CHSSubName, szInfo, MAX_PATH);	
	GetPrivateProfileString("FONTName", "ENGFaimly", _T("HYTest"), szInfo, MAX_PATH, strProfileName);
	CopyMemory(m_tagOpeionPrm.ENGFaimlyName, szInfo, MAX_PATH);
	GetPrivateProfileString("FONTName", "ENGSub", _T("Regular"), szInfo, MAX_PATH, strProfileName);
	CopyMemory(m_tagOpeionPrm.ENGSubName, szInfo, MAX_PATH);
	GetPrivateProfileString("FONTName", "Version", _T("Version 9.00.00"), szInfo, MAX_PATH, strProfileName);
	CopyMemory(m_tagOpeionPrm.Version, szInfo, MAX_PATH);	

}	// end of void CXSysBaseProxy::LoadLocalProfile()

void CXSysBaseProxy::StoreLocalProfile()
{
	CString strProfileName = m_strAppDir + _T("data\\SmartSharperSetUp.ini");
	CString strTmp = _T("");
	
	/////////////////////////////////////////////////////////////////////////////////////////////	
	strTmp.Format(_T("%d"), m_tagDrawParam.iPoint);
	WritePrivateProfileString("SHOW PARAM", "Show Point", strTmp, strProfileName);
	strTmp.Format(_T("%d"), m_tagDrawParam.iPointNo);
	WritePrivateProfileString("SHOW PARAM", "Show Point Number", strTmp, strProfileName);	
	strTmp.Format(_T("%d"), m_tagDrawParam.iLine);
	WritePrivateProfileString("SHOW PARAM", "Show Line", strTmp, strProfileName);
	strTmp.Format(_T("%d"), m_tagDrawParam.iReferLine);
	WritePrivateProfileString("SHOW PARAM", "Show ReferLine", strTmp, strProfileName);
	strTmp.Format(_T("%d"), m_tagDrawParam.iFill);
	WritePrivateProfileString("SHOW PARAM", "Fill Contours", strTmp, strProfileName);	
	strTmp.Format(_T("%d"), m_tagDrawParam.iViewType);
	WritePrivateProfileString("SHOW PARAM", "View Type", strTmp, strProfileName);	

	// OPtion
	strTmp.Format(_T("%d"), m_tagOpeionPrm.bCmplVert);
	WritePrivateProfileString("Option", "VERT", strTmp, strProfileName);
	strTmp.Format(_T("%d"), m_tagOpeionPrm.bRePsName);
	WritePrivateProfileString("Option", "PSNAME", strTmp, strProfileName);
	strTmp.Format(_T("%d"), m_tagOpeionPrm.bReSortUni);
	WritePrivateProfileString("Option", "SORTUNI", strTmp, strProfileName);
	strTmp.Format(_T("%d"), m_tagOpeionPrm.bYitizi);
	WritePrivateProfileString("Option", "YITIZI", strTmp, strProfileName);
	strTmp.Format(_T("%d"), m_tagOpeionPrm.bOldStandard);
	WritePrivateProfileString("Option", "OLDSTANDARD", strTmp, strProfileName);
	strTmp.Format(_T("%d"), m_tagOpeionPrm.bKangXi);
	WritePrivateProfileString("Option", "KANGXI", strTmp, strProfileName);
	strTmp.Format(_T("%d"), m_tagOpeionPrm.bCnturCorrect);
	WritePrivateProfileString("Option", "CORRECT", strTmp, strProfileName);	
	strTmp.Format(_T("%d"), m_tagOpeionPrm.bFontname);
	WritePrivateProfileString("Option", "RENAME", strTmp, strProfileName);
	strTmp.Format(_T("%d"), m_tagOpeionPrm.bCmplCMAP);
	WritePrivateProfileString("Option", "CMAP", strTmp, strProfileName);
	strTmp.Format(_T("%d"), m_tagOpeionPrm.bsetADW);
	WritePrivateProfileString("Option", "SETADW", strTmp, strProfileName);	
	strTmp.Format(_T("%d"), m_tagOpeionPrm.bsetADH);
	WritePrivateProfileString("Option", "SETADH", strTmp, strProfileName);
	strTmp.Format(_T("%d"), m_tagOpeionPrm.usSetADH);
	WritePrivateProfileString("Option", "ADH", strTmp, strProfileName);		
	strTmp.Format(_T("%d"), m_tagOpeionPrm.bCmplLayout);
	WritePrivateProfileString("Option", "LAYOUT", strTmp, strProfileName);
	strTmp.Format(_T("%d"), m_tagOpeionPrm.bHanyi);
	WritePrivateProfileString("Option", "HANYI", strTmp, strProfileName);

	strTmp.Format(_T("%d"), m_tagOpeionPrm.usUnitsPerEM);
	WritePrivateProfileString("HEAD", _T("PEREM"), strTmp, strProfileName);
	strTmp.Format(_T("%d"), m_tagOpeionPrm.sTypo_Ascender);
	WritePrivateProfileString("Typo_Ascender", _T("Ascender"), strTmp, strProfileName);
	strTmp.Format(_T("%d"), m_tagOpeionPrm.sTypo_Descender);
	WritePrivateProfileString("Typo_Descender", _T("Descender"), strTmp, strProfileName);

	strTmp.Format(_T("%d"), m_tagOpeionPrm.usWeight);
	WritePrivateProfileString("OS", _T("Weight"), strTmp, strProfileName);
	strTmp.Format(_T("%d"), m_tagOpeionPrm.usWidth);
	WritePrivateProfileString("OS", _T("Width"), strTmp, strProfileName);	
	strTmp.Format(_T("%x"), m_tagOpeionPrm.ulCodePageRang1);
	WritePrivateProfileString("OS", _T("CodePageRang1"), strTmp, strProfileName);	
	strTmp.Format(_T("%x"), m_tagOpeionPrm.ulCodePageRang2);
	WritePrivateProfileString("OS", _T("CodePageRang2"), strTmp, strProfileName);

	strTmp.Format(_T("%d"), m_tagOpeionPrm.bCIDFnt);
	WritePrivateProfileString("CFF", "CID", strTmp, strProfileName);
	strTmp.Format(_T("%s"), m_tagOpeionPrm.Registry);
	WritePrivateProfileString("CFF", _T("ROS_Registroy"), strTmp, strProfileName);
	strTmp.Format(_T("%s"), m_tagOpeionPrm.Ordering);
	WritePrivateProfileString("CFF", _T("ROS_Ordering"), strTmp, strProfileName);
	strTmp.Format(_T("%s"), m_tagOpeionPrm.Supplement);
	WritePrivateProfileString("CFF", _T("ROS_Supplement"), strTmp, strProfileName);

	strTmp.Format(_T("%s"), m_tagOpeionPrm.CHSFaimlyName);
	WritePrivateProfileString("FONTName", _T("CHSFaimly"), strTmp, strProfileName);	
	strTmp.Format(_T("%s"), m_tagOpeionPrm.CHSSubName);
	WritePrivateProfileString("FONTName", _T("CHSSub"), strTmp, strProfileName);	
	strTmp.Format(_T("%s"), m_tagOpeionPrm.ENGFaimlyName);
	WritePrivateProfileString("FONTName", _T("ENGFaimly"), strTmp, strProfileName);
	strTmp.Format(_T("%s"), m_tagOpeionPrm.ENGSubName);
	WritePrivateProfileString("FONTName", _T("ENGSub"), strTmp, strProfileName);
	strTmp.Format(_T("%s"), m_tagOpeionPrm.Version);
	WritePrivateProfileString("FONTName", _T("Version"), strTmp, strProfileName);	
	strTmp.Format(_T("%s"), m_tagOpeionPrm.CHSCpyright);

}	// end of void CXSysBaseProxy::StoreLocalProfile()

void CXSysBaseProxy::LoadAdvancedTypographicTables( CHYFontCodec* pFntCdc)
{
	CString strProfileName = m_strAppDir + _T("data\\GSUB.xml");

	HYFONTCODEC::CHYFontCodec* pHYFontCode = pFntCdc;

	std::string		strNum = "", strSutItem = "";
	char			szTmp[4];
	int				iNum = 0;
	CMarkup		markUp;
	if (markUp.Load(strProfileName))
	{
		markUp.ResetMainPos();
		if (markUp.FindElem("FontProfile"))
		{
			markUp.IntoElem();

			if (markUp.FindElem("GSUB"))
			{
				CHYGsub& GSUB = pHYFontCode->m_HYGsub;
				GSUB.Setdefault();

				// load Scriptlist
				if (markUp.FindChildElem("scriptlist"))
				{
					markUp.IntoElem();
					while (markUp.FindChildElem("scripttable"))
					{
						CHYScriptRecord	ScriptRecord;
						markUp.IntoElem();
						strNum = markUp.GetAttrib("tag");
						memset(szTmp, 0, 4);
						memcpy(szTmp, strNum.data(), 4);
						memcpy(ScriptRecord.Tag, szTmp, 4);

						if (markUp.FindChildElem("DefaultLangSys"))
						{
							markUp.IntoElem();
							strNum = markUp.GetAttrib("ReqFeatureIndex");
							ScriptRecord.Script.DefaultLangSys.ReqFeatureIndex = strtol(strNum.data(), NULL, 10);

							while (markUp.FindChildElem("FeatureIndex"))
							{
								markUp.IntoElem();
								strNum = markUp.GetData();
								long FIndex = strtol(strNum.data(), NULL, 10);
								ScriptRecord.Script.DefaultLangSys.vtFeatureIndex.push_back(static_cast<unsigned int>(FIndex));
								markUp.OutOfElem();
							}
							markUp.OutOfElem();
						}

						while (markUp.FindChildElem("LangSysTable"))
						{
							markUp.IntoElem();

							CHYLangSysRecord	langSysRecord;
							strNum = markUp.GetAttrib("tag");
							memset(szTmp, 0, 4);
							memcpy(szTmp, strNum.data(), 4);
							memcpy(langSysRecord.SysTag, szTmp, 4);

							strNum = markUp.GetAttrib("ReqFeatureIndex");
							langSysRecord.LangSys.ReqFeatureIndex = strtol(strNum.data(), NULL, 10);

							while (markUp.FindChildElem("FeatureIndex"))
							{
								markUp.IntoElem();
								strNum = markUp.GetData();
								long FIndex = strtol(strNum.data(), NULL, 10);
								langSysRecord.LangSys.vtFeatureIndex.push_back(static_cast<unsigned int>(FIndex));
								markUp.OutOfElem();
							}
							markUp.OutOfElem();
							ScriptRecord.Script.vtLangSysRecord.push_back(langSysRecord);
						}

						markUp.OutOfElem();
						GSUB.vtScriptsList.push_back(ScriptRecord);
					}
					markUp.OutOfElem();
				}

				// load Fearturelist;			
				if (markUp.FindChildElem("featurelist"))
				{
					markUp.IntoElem();
					while (markUp.FindChildElem("featuretable"))
					{
						CHYFeatureRecord	FeatureRecord;
						markUp.IntoElem();

						strNum = markUp.GetAttrib("tag");
						memset(szTmp, 0, 4);
						memcpy(szTmp, strNum.data(), 4);
						memcpy(FeatureRecord.Tag, szTmp, 4);

						while (markUp.FindChildElem("LookupIndex"))
						{
							markUp.IntoElem();
							strNum = markUp.GetData();
							long lLkIndex = strtol(strNum.data(), NULL, 10);
							FeatureRecord.FeatureTable.vtLookupListIndex.push_back(static_cast<unsigned short>(lLkIndex));
							markUp.OutOfElem();
						}

						markUp.OutOfElem();
						GSUB.vtFeaturesList.push_back(FeatureRecord);
					}
					markUp.OutOfElem();
				}

				// load Lookuplist Info
				if (markUp.FindChildElem("lookuplist"))
				{
					markUp.IntoElem();
					while (markUp.FindChildElem("lookup"))
					{
						CLookUp	LookUpTable;

						markUp.IntoElem();
						strNum = markUp.GetAttrib("type");
						long Tmp = strtol(strNum.data(), NULL, 10);
						LookUpTable.LookUpType = static_cast<unsigned  short>(Tmp);
						strNum = markUp.GetAttrib("flag");
						Tmp = strtol(strNum.data(), NULL, 10);
						LookUpTable.LookUpFlag = static_cast<unsigned  short>(Tmp);
						
						if (LookUpTable.LookUpType == 1)
						{
							while (markUp.FindChildElem("SubTable"))
							{
								CLookUpType1	LookUpType1;
								markUp.IntoElem();
								strNum = markUp.GetAttrib("SubstFormat");
								LookUpType1.SubFormat = (unsigned short)strtol(strNum.data(), NULL, 10);

								if (LookUpType1.SubFormat == 2)
								{
									CLookUpSingleSubstitution2& SSubstitution2 = LookUpType1.Substitution2;
									CCoverage& Coverage = SSubstitution2.Coverage;
									// Ŀǰ��֧��1
									Coverage.CoverageFormat = 1;
									while (markUp.FindChildElem("charnum"))
									{
										markUp.IntoElem();
										strSutItem = markUp.GetData();

										std::string strTmp;
										std::string::size_type iTmp = strSutItem.find(",");
										if (iTmp > 0)
										{
											std::string strConverageID = strSutItem.substr(0, iTmp);
											std::string strTmp = strConverageID.substr(0, 1);
											if (!strTmp.empty())
											{
												transform(strTmp.begin(), strTmp.end(), strTmp.begin(), ::toupper);
												if (strTmp == "G")
												{
													strTmp = strConverageID.substr(1);
													iNum = strtol(strTmp.data(), NULL, 10);
													Coverage.vtGlyphID.push_back(static_cast<unsigned short>(iNum));
												}
												if (strTmp == "U")
												{
													strTmp = strConverageID.substr(1);
													DWORD dwUni = strtol(strTmp.data(), NULL, 16);
													iNum = pHYFontCode->FindGryphIndexByUnciodeEx(dwUni);
													//if (iNum!=-1)
													{
														Coverage.vtGlyphID.push_back(static_cast<unsigned short>(iNum));
													}
												}

												std::string strSubstituteID = strSutItem.substr(iTmp + 1);
												if (!strSubstituteID.empty())
												{
													strTmp = strSubstituteID.substr(0, 1);
													if (!strTmp.empty())
													{
														transform(strTmp.begin(), strTmp.end(), strTmp.begin(), ::toupper);
														if (strTmp == "G")
														{
															strTmp = strSubstituteID.substr(1);
															iNum = strtol(strTmp.data(), NULL, 10);
															SSubstitution2.vtGlyphID.push_back(static_cast<unsigned short>(iNum));
														}
														if (strTmp == "U")
														{
															strTmp = strSubstituteID.substr(1);
															DWORD dwUni = strtol(strTmp.data(), NULL, 16);
															iNum = pHYFontCode->FindGryphIndexByUnciodeEx(dwUni);
															//if (iNum!=-1)
															{
																SSubstitution2.vtGlyphID.push_back(static_cast<unsigned short>(iNum));
															}
														}
													}
												}
											}
										}
										markUp.OutOfElem();
									}
									Coverage.GlyphCount = (unsigned short)Coverage.vtGlyphID.size();
								}
								markUp.OutOfElem();
								LookUpType1.Substitution2.GlyphCount = (unsigned short)LookUpType1.Substitution2.vtGlyphID.size();
								LookUpTable.vtLookupType1.push_back(LookUpType1);
							}
						}
						LookUpTable.SubTableCount++;
						markUp.OutOfElem();
						GSUB.vtLookupList.push_back(LookUpTable);
					}
					markUp.OutOfElem();
				}
			}
			markUp.OutOfElem();
		}
	}

}	// end of void CXSysBaseProxy::LoadAdvancedTypographicTables()

BOOL CXSysBaseProxy::MulToUniHL(char* pDest, int& iDestlen, char* pSrc, int iSrclen)
{
	if ( pDest == NULL ) return FALSE;
	if ( pSrc == NULL )	return FALSE;

	WCHAR	wpString[MAX_PATH]	= {0};
	int		iWStringLen			= 0;	

	iWStringLen = MultiByteToWideChar(CP_ACP,0, pSrc, -1, NULL, 0 );
	if (iWStringLen == 0 )					return FALSE;		
	if (iWStringLen >= MAX_PATH / 2 )		return FALSE;	

	MultiByteToWideChar(CP_ACP, 0, pSrc, strlen(pSrc), wpString, iWStringLen);

	iWStringLen = wcslen(wpString);

	if (2*iWStringLen>iDestlen) return FALSE;
	iDestlen = 2*iWStringLen;

	// 大端模式
	short sTmp = 0;
	for ( int x = 0; x < iWStringLen ; x++ ){			
		sTmp = htons(wpString[x]);
		CopyMemory(pDest + x * 2, &sTmp,2);
	}

	return TRUE;

}	// end of void MToWCharBigStart()

int CXSysBaseProxy::FindString(std::vector<CString>& vt, CString str)
{
	for (size_t i = 0; i<vt.size(); i++)
	{
		if (str == vt[i])
			return i;
	}

	return -1;

}	// end of int CXSysProxy::FindString()
