#include "StdAfx.h"
#include "XSysBaseProxy.h"
#include "HYGsub.h"
//#include "boost/algorithm/string.hpp"
//using namespace  boost::algorithm;

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

	//m_strAppDir = ::HY_GetDirFromPath(std::string(strHelpFile)).data();
	m_strAppDir.Format(_T("%s\\"),pPath);

	TCHAR   szBuffer[MAX_PATH]; 
	ZeroMemory(szBuffer, MAX_PATH);
	GetSystemDirectory(szBuffer, MAX_PATH); 
	m_strSysDir.Format(_T("%s\\"), szBuffer);

	LoadLocalProfile();

	CString strFeatureFile;
	strFeatureFile.Format(_T("%sdata\\Profile.xml"), m_strAppDir);
	LoadFontFeature(strFeatureFile, m_hyFontCodec);

	m_strBlackBkName.Format(_T("%sdata\\BlackBK.png "),m_strAppDir);
	m_strWhiteBkName.Format(_T("%sdata\\WhiteBK.png "), m_strAppDir);

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
	ZeroMemory(&m_tagDrawParam, sizeof(DRAWFONTPARAM));
	ZeroMemory(&m_tagScanPrm,sizeof(HY_SCAN_PRM));

	m_tagDrawParam.iLine	= 1;
	m_tagDrawParam.iPoint	= 1;
	m_fBaseLine				= 0.0f;
	m_rctTraceRect.SetRectEmpty();

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
	GetPrivateProfileString("Option", "VERT", _T("0"), szInfo, MAX_PATH, strProfileName);
	m_tagOpeionPrm.bCmplVert = atoi(szInfo)==0 ? FALSE : TRUE;
	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("Option", "PSNAME", _T("0"), szInfo, MAX_PATH, strProfileName);
	m_tagOpeionPrm.bRePsName = atoi(szInfo) == 0 ? FALSE : TRUE;
	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("Option", "SORTUNI", _T("0"), szInfo, MAX_PATH, strProfileName);
	m_tagOpeionPrm.bReSortUni = atoi(szInfo) == 0 ? FALSE: TRUE;
	GetPrivateProfileString("Option", "YITIZI", _T("1"), szInfo, MAX_PATH, strProfileName);
	m_tagOpeionPrm.bYitizi = atoi(szInfo) == 0 ? FALSE: TRUE;

	//HandWrite
	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("HandWrite", "BaseLine", _T("0.15"), szInfo, MAX_PATH, strProfileName);
	m_fBaseLine = atof(szInfo);

	// Image To Font
	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("ImageToFont", "TopEdge", _T("10"), szInfo, MAX_PATH, strProfileName);
	m_tagScanPrm.iTopEdge = atoi(szInfo);

	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("ImageToFont", "LeftEdge", _T("10"), szInfo, MAX_PATH, strProfileName);
	m_tagScanPrm.iLeftEdge = atoi(szInfo);

	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("ImageToFont", "BottomEdge", _T("10"), szInfo, MAX_PATH, strProfileName);
	m_tagScanPrm.iBottomEdge = atoi(szInfo);

	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("ImageToFont", "RightEdge", _T("10"), szInfo, MAX_PATH, strProfileName);
	m_tagScanPrm.iRightEdge = atoi(szInfo);

	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("ImageToFont", "GrayThre", _T("200"), szInfo, MAX_PATH, strProfileName);
	m_tagScanPrm.iGrayThre = atoi(szInfo);

	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("ImageToFont", "ScanDirection", _T("0"), szInfo, MAX_PATH, strProfileName);
	m_tagScanPrm.iScanDirection = atoi(szInfo);

	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("ImageToFont", "Rows", _T("5"), szInfo, MAX_PATH, strProfileName);
	m_tagScanPrm.iRows = atoi(szInfo);

	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("ImageToFont", "Colmns", _T("4"), szInfo, MAX_PATH, strProfileName);
	m_tagScanPrm.iColmns = atoi(szInfo);

	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("ImageToFont", "Avrdis", _T("3"), szInfo, MAX_PATH, strProfileName);
	m_tagScanPrm.iAvrdis= atoi(szInfo);

	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("ImageToFont", "DisWidth", _T("8"), szInfo, MAX_PATH, strProfileName);
	m_tagScanPrm.iDisWidth = atoi(szInfo);

	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("ImageToFont", "SingleBorder", _T("5"), szInfo, MAX_PATH, strProfileName);
	m_tagScanPrm.iSingleBorder = atoi(szInfo);

	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("ImageToFont", "GridThres", _T("1"), szInfo, MAX_PATH, strProfileName);
	m_tagScanPrm.iGridThres = atoi(szInfo);	

	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("ImageToFont", "GridThresLen", _T("1"), szInfo, MAX_PATH, strProfileName);
	m_tagScanPrm.iGridLenThres = atoi(szInfo);

	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("ImageToFont", "Smooth", _T("9"), szInfo, MAX_PATH, strProfileName);
	m_tagScanPrm.iSmoothParam = atoi(szInfo);

	// Split Image
	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("SplitImage", "ImgCode", _T("0x20000"), szInfo, MAX_PATH, strProfileName);
	m_tagScanPrm.iGridImgNumber = strtol(szInfo,NULL,16);

	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("SplitImage", "SaveDir", _T("d:\\"), szInfo, MAX_PATH, strProfileName);
	strcpy_s(m_tagScanPrm.szGridImgDir,MAX_PATH, szInfo);

	// image optimize
	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("ImageOptimze", "Contrast", _T("0"), szInfo, MAX_PATH, strProfileName);
	m_tagImgOptim.Contrast = atoi(szInfo);

	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("ImageOptimze", "Light", _T("0"), szInfo, MAX_PATH, strProfileName);
	m_tagImgOptim.Light = atoi(szInfo);

	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("ImageOptimze", "Invert", _T("1"), szInfo, MAX_PATH, strProfileName);
	m_tagImgOptim.InVert = atoi(szInfo);

	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("ImageOptimze", "IsMono", _T("1"), szInfo, MAX_PATH, strProfileName);
	m_tagImgOptim.bMono = atoi(szInfo);

	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("ImageOptimze", "MonoThres", _T("125"), szInfo, MAX_PATH, strProfileName);
	m_tagImgOptim.MonoThres = atoi(szInfo);

	ZeroMemory(szInfo, MAX_PATH);
	GetPrivateProfileString("ImageOptimze", "Background", _T("0"), szInfo, MAX_PATH, strProfileName);
	m_tagImgOptim.BKType = atoi(szInfo);
	
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

	//HandWrite	
	strTmp.Format(_T("%02f"),m_fBaseLine);
	WritePrivateProfileString("HandWrite", "BaseLine", strTmp, strProfileName);

	// Image To Font
	strTmp.Format(_T("%d"), m_tagScanPrm.iTopEdge);
	WritePrivateProfileString("ImageToFont", "TopEdge", strTmp, strProfileName);	
	strTmp.Format(_T("%d"), m_tagScanPrm.iLeftEdge);
	WritePrivateProfileString("ImageToFont", "LeftEdge", strTmp, strProfileName);
	strTmp.Format(_T("%d"), m_tagScanPrm.iBottomEdge);
	WritePrivateProfileString("ImageToFont", "BottomEdge", strTmp, strProfileName);		
	strTmp.Format(_T("%d"), m_tagScanPrm.iRightEdge);
	WritePrivateProfileString("ImageToFont", "RightEdge", strTmp, strProfileName);
	strTmp.Format(_T("%d"), m_tagScanPrm.iGrayThre);
	WritePrivateProfileString("ImageToFont", "GrayThre", strTmp, strProfileName);
	strTmp.Format(_T("%d"), m_tagScanPrm.iScanDirection);
	WritePrivateProfileString("ImageToFont", "ScanDirection", strTmp, strProfileName);
	strTmp.Format(_T("%d"), m_tagScanPrm.iRows);
	WritePrivateProfileString("ImageToFont", "Rows", strTmp, strProfileName);
	strTmp.Format(_T("%d"), m_tagScanPrm.iColmns);
	WritePrivateProfileString("ImageToFont", "Colmns", strTmp, strProfileName);
	strTmp.Format(_T("%d"), m_tagScanPrm.iAvrdis);
	WritePrivateProfileString("ImageToFont", "Avrdis", strTmp, strProfileName);	
	strTmp.Format(_T("%d"), m_tagScanPrm.iDisWidth);
	WritePrivateProfileString("ImageToFont", "DisWidth", strTmp, strProfileName);
	strTmp.Format(_T("%d"), m_tagScanPrm.iSingleBorder);
	WritePrivateProfileString("ImageToFont", "SingleBorder", strTmp, strProfileName);
	strTmp.Format(_T("%d"), m_tagScanPrm.iGridThres);
	WritePrivateProfileString("ImageToFont", "GridThres", strTmp, strProfileName);
	strTmp.Format(_T("%d"), m_tagScanPrm.iGridLenThres);
	WritePrivateProfileString("ImageToFont", "GridThresLen", strTmp, strProfileName);
	strTmp.Format(_T("%d"), m_tagScanPrm.iSmoothParam);
	WritePrivateProfileString("ImageToFont", "Smooth", strTmp, strProfileName);
	// Split Image
	strTmp.Format(_T("0x%X"), m_tagScanPrm.iGridImgNumber);
	WritePrivateProfileString("SplitImage", "ImgCode", strTmp, strProfileName);
	strTmp.Format(_T("%s"), m_tagScanPrm.szGridImgDir);
	WritePrivateProfileString("SplitImage", "SaveDir", strTmp, strProfileName);

	// Image optim
	strTmp.Format(_T("%d"), m_tagImgOptim.Contrast);
	WritePrivateProfileString("ImageOptimze", "Contrast", strTmp, strProfileName);
	strTmp.Format(_T("%d"), m_tagImgOptim.Light);
	WritePrivateProfileString("ImageOptimze", "Light", strTmp, strProfileName);
	strTmp.Format(_T("%d"), (int)m_tagImgOptim.InVert);
	WritePrivateProfileString("ImageOptimze", "Invert", strTmp, strProfileName);
	strTmp.Format(_T("%d"), (int)m_tagImgOptim.bMono);
	WritePrivateProfileString("ImageOptimze", "IsMono", strTmp, strProfileName);
	strTmp.Format(_T("%d"), m_tagImgOptim.MonoThres);
	WritePrivateProfileString("ImageOptimze", "MonoThres", strTmp, strProfileName);
	strTmp.Format(_T("%d"), m_tagImgOptim.BKType);
	WritePrivateProfileString("ImageOptimze", "Background", strTmp, strProfileName);

}	// end of void CXSysBaseProxy::StoreLocalProfile()

void	CXSysBaseProxy::LoadFontFeature(CString strProfileName, HYFONTCODEC::CHYFontCodec& hyFontCodec)
{
	//::LoadFontFeature((LPTSTR)(LPCTSTR)strProfileName,hyFontCodec);
	
}	// end of void	CXSysBaseProxy::LoadFontFeature()

void	CXSysBaseProxy::StoreFontFeature(CString strProfileName, HYFONTCODEC::CHYFontCodec& hyFontCodec)
{	
	DeleteFile(strProfileName);
	
	TCHAR	szInfo[MAX_PATH] = {0};
	CHYName& name = hyFontCodec.m_HYName;
	CString strTmp;

	CString strSubKey, strKey;
	// name segment
	strTmp.Format(_T("%d"), name.vtNameRecord.size());
	WritePrivateProfileString("NAME", "Segment", strTmp, strProfileName);

	// name Record
	for (size_t i=0; i<name.vtNameRecord.size(); i++)
	{
		strSubKey.Format("NAME_RECORD_%d", i);
		NAMERECORD&	nameRecord = name.vtNameRecord[i];
		strTmp.Format(_T("%d,%d,%d,%d,%s"),
			nameRecord.platformID,
			nameRecord.encodingID,
			nameRecord.languageID,
			nameRecord.nameID,
			nameRecord.strContent.c_str()
			);	
		
		WritePrivateProfileString("NAME", strSubKey, strTmp, strProfileName);
	}

	//POST TABLE 
	CHYPost& post = hyFontCodec.m_HYPost;
	strKey = _T("POST_TABLE");
	strTmp.Format(_T("%d.%d"), post.italicAngle.value, post.italicAngle.fract);
	WritePrivateProfileString(strKey, _T("ItalicAngle"), strTmp, strProfileName);
	strTmp.Format(_T("%d"), post.underlinePosition);
	WritePrivateProfileString(strKey, _T("UnderlinePosition"), strTmp, strProfileName);
	strTmp.Format(_T("%d"), post.underlineThickness);
	WritePrivateProfileString(strKey, _T("UnderlineThickness"), strTmp, strProfileName);
	strTmp.Format(_T("%d"), post.isFixedPitch);
	WritePrivateProfileString(strKey, _T("IsFixedPitch"), strTmp, strProfileName);

	//HHEA TABLE 
	CHYHhea& hhea = hyFontCodec.m_HYHhea;
	strKey = _T("HHEA_TABLE");
	strTmp.Format(_T("%d"), hhea.Ascender);
	WritePrivateProfileString(strKey, _T("Ascender"), strTmp, strProfileName);	
	strTmp.Format(_T("%d"), hhea.Descender);
	WritePrivateProfileString(strKey, _T("Descender"), strTmp, strProfileName);
	strTmp.Format(_T("%d"), hhea.LineGap);
	WritePrivateProfileString(strKey, _T("LineGap"), strTmp, strProfileName);	

	//HEAD TABLE 
	CHYHead& head = hyFontCodec.m_HYhead;
	strKey = _T("HEAD_TABLE");
	strTmp.Format(_T("%d.%03d"), head.fontRevision.value,head.fontRevision.fract/65536.0f+0.0005*1000);
	WritePrivateProfileString(strKey, _T("fontvision"),strTmp, strProfileName);
	strTmp.Format(_T("%d"), head.flags);
	WritePrivateProfileString(strKey, _T("Flag"), strTmp, strProfileName);
	strTmp.Format(_T("%d"), head.unitsPerEm);
	WritePrivateProfileString(strKey, _T("UnitsPerEM"), strTmp, strProfileName);
	strTmp.Format(_T("%d"), head.macStyle);
	WritePrivateProfileString(strKey, _T("Macstyle"), strTmp, strProfileName);
	strTmp.Format(_T("%d"), head.lowestRecPPEM);
	WritePrivateProfileString(strKey, _T("lowestRecPPEM"), strTmp, strProfileName);
	strTmp.Format(_T("%d"), head.fontDirectionHint);
	WritePrivateProfileString(strKey, _T("FontDirectionHint"), strTmp, strProfileName);

	//OS_2 TABLE
	CHYOS& os2 = hyFontCodec.m_HYOS2;
	strKey = _T("OS/2_TABLE");
	strTmp.Format(_T("%d"), os2.usWeightClass);
	WritePrivateProfileString(strKey, _T("WeightClass"), strTmp, strProfileName);	
	strTmp.Format(_T("%d"), os2.usWidthClass);
	WritePrivateProfileString(strKey, _T("WidthClass"), strTmp, strProfileName);	
	strTmp.Format(_T("%d"), os2.fsType);
	WritePrivateProfileString(strKey, _T("FsType"), strTmp, strProfileName);
	strTmp.Format(_T("%d"), os2.panose.FamilyType);
	WritePrivateProfileString(strKey, _T("Panose_FamilyType"), strTmp, strProfileName);	
	strTmp.Format(_T("%d"), os2.panose.SerifStyle);
	WritePrivateProfileString(strKey, _T("Panose_SerifStyle"), strTmp, strProfileName);
	strTmp.Format(_T("%d"), os2.panose.Weight);
	WritePrivateProfileString(strKey, _T("Panose_Weight"), strTmp, strProfileName);
	strTmp.Format(_T("%d"), os2.panose.Proportion);
	WritePrivateProfileString(strKey, _T("Panose_Proportion"), strTmp, strProfileName);
	strTmp.Format(_T("%d"), os2.panose.Contrast);
	WritePrivateProfileString(strKey, _T("Panose_Contrast"), strTmp, strProfileName);
	strTmp.Format(_T("%d"), os2.panose.StrokeVariation);
	WritePrivateProfileString(strKey, _T("Panose_StrokeVariation"), strTmp, strProfileName);
	strTmp.Format(_T("%d"), os2.panose.ArmStyle);
	WritePrivateProfileString(strKey, _T("Panose_ArmStyle"), strTmp, strProfileName);
	strTmp.Format(_T("%d"), os2.panose.Letterform);
	WritePrivateProfileString(strKey, _T("Panose_Letterform"), strTmp, strProfileName);
	strTmp.Format(_T("%d"), os2.panose.Midline);
	WritePrivateProfileString(strKey, _T("Panose_Midline"), strTmp, strProfileName);
	strTmp.Format(_T("%d"), os2.panose.XHeight);
	WritePrivateProfileString(strKey, _T("Panose_XHeight"), strTmp, strProfileName);
	strTmp.Format(_T("0x%x"), os2.ulUnicodeRange1);
	WritePrivateProfileString(strKey, _T("UnicodeRange1"), strTmp, strProfileName);
	strTmp.Format(_T("0x%x"), os2.ulUnicodeRange2);
	WritePrivateProfileString(strKey, _T("UnicodeRange2"), strTmp, strProfileName);
	strTmp.Format(_T("0x%x"), os2.ulUnicodeRange3);
	WritePrivateProfileString(strKey, _T("UnicodeRange3"), strTmp, strProfileName);
	strTmp.Format(_T("0x%x"), os2.ulUnicodeRange4);
	WritePrivateProfileString(strKey, _T("UnicodeRange4"), strTmp, strProfileName);
	ZeroMemory(szInfo, MAX_PATH);
	szInfo[0] = os2.vtachVendID[0];
	szInfo[1] = os2.vtachVendID[1];
	szInfo[2] = os2.vtachVendID[2];
	szInfo[3] = os2.vtachVendID[3];
	strTmp.Format(_T("%s"), szInfo);
	WritePrivateProfileString(strKey, _T("achVendID"), strTmp, strProfileName);
	strTmp.Format(_T("0x%x"), os2.fsSelection);
	WritePrivateProfileString(strKey, _T("fsSelection"), strTmp, strProfileName);
	strTmp.Format(_T("%d"), os2.sTypoAscender);
	WritePrivateProfileString(strKey, _T("TypoAscender"), strTmp, strProfileName);
	strTmp.Format(_T("%d"), os2.sTypoDescender);
	WritePrivateProfileString(strKey, _T("TypoDscender"), strTmp, strProfileName);
	strTmp.Format(_T("%d"), os2.sTypoLineGap);
	WritePrivateProfileString(strKey, _T("TypoLineGap"), strTmp, strProfileName);
	strTmp.Format(_T("%d"), os2.usWinAscent);
	WritePrivateProfileString(strKey, _T("WinAscent"), strTmp, strProfileName);	
	strTmp.Format(_T("%d"), os2.usWinDescent);
	WritePrivateProfileString(strKey, _T("WinDescent"), strTmp, strProfileName);	
	strTmp.Format(_T("0x%x"), os2.ulCodePageRange1);
	WritePrivateProfileString(strKey, _T("CodePageRange1"), strTmp, strProfileName);
	strTmp.Format(_T("0x%x"), os2.ulCodePageRange2);
	WritePrivateProfileString(strKey, _T("CodePageRange2"), strTmp, strProfileName);
	strTmp.Format(_T("%d"), os2.ySubscriptXSize);
	WritePrivateProfileString(strKey, _T("SubscriptXSize"), strTmp, strProfileName);	
	strTmp.Format(_T("%d"), os2.ySubscriptYSize);
	WritePrivateProfileString(strKey, _T("SubscriptYSize"), strTmp, strProfileName);	
	strTmp.Format(_T("%d"), os2.ySubscriptXOffset);
	WritePrivateProfileString(strKey, _T("SubscriptXOffset"), strTmp, strProfileName);	
	strTmp.Format(_T("%d"), os2.ySubscriptYOffset);
	WritePrivateProfileString(strKey, _T("SubscriptYOffset"), strTmp, strProfileName);
	strTmp.Format(_T("%d"), os2.ySuperscriptXSize);
	WritePrivateProfileString(strKey, _T("SuperscriptXSize"), strTmp, strProfileName);
	strTmp.Format(_T("%d"), os2.ySuperscriptYSize);
	WritePrivateProfileString(strKey, _T("SuperscriptYSize"), strTmp, strProfileName);
	strTmp.Format(_T("%d"), os2.ySuperscriptXOffset);
	WritePrivateProfileString(strKey, _T("SuperscriptXOffset"), strTmp, strProfileName);
	strTmp.Format(_T("%d"), os2.ySuperscriptYOffset);
	WritePrivateProfileString(strKey, _T("SuperscriptYOffset"), strTmp, strProfileName);
	strTmp.Format(_T("%d"), os2.yStrikeoutSize);
	WritePrivateProfileString(strKey, _T("StrikeoutSize"), strTmp, strProfileName);
	strTmp.Format(_T("%d"), os2.yStrikeoutPosition);
	WritePrivateProfileString(strKey, _T("StrikeoutPosition"), strTmp, strProfileName);
	strTmp.Format(_T("%d"), os2.sxHeight);
	WritePrivateProfileString(strKey, _T("xHeight"), strTmp, strProfileName);
	strTmp.Format(_T("%d"), os2.sCapHeight);
	WritePrivateProfileString(strKey, _T("CapHeight"), strTmp, strProfileName);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// BALSELine
	strTmp.Format(_T("%f"), m_fBaseLine);
	WritePrivateProfileString(_T("BASELINE"), _T("baseline"), strTmp, strProfileName);

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	// 读取CFF配置	
	CHYCFFInfo& cffInfo = hyFontCodec.m_HYCFFInfo;
	strKey = _T("CFF");
	strTmp.Format(_T("%d"), cffInfo.TopDICT.IsCIDFont);
	WritePrivateProfileString(strKey, _T("CID_Font"),strTmp,strProfileName);
	strTmp.Format(_T("%s"), cffInfo.TopDICT.Ros.strRegistry.c_str());
	WritePrivateProfileString(strKey, _T("ROS_Registroy"),strTmp,strProfileName);
	strTmp.Format(_T("%s"), cffInfo.TopDICT.Ros.strOrdering.c_str());
	WritePrivateProfileString(strKey, _T("ROS_Ordering"),strTmp,strProfileName);
	strTmp.Format(_T("%d"), cffInfo.TopDICT.Ros.Supplement);
	WritePrivateProfileString(strKey, _T("ROS_Supplement"),strTmp,strProfileName);
	strTmp.Format(_T("%d"), cffInfo.TopDICT.CIDFontVersion);
	WritePrivateProfileString(strKey, _T("CID_FontVersion"),strTmp,strProfileName);
	
}	// end of void	CXSysBaseProxy::StroeTTFAttchInfo()

void CXSysBaseProxy::LoadAdvancedTypographicTables(HYFONTCODEC::CHYFontCodec& hyFontCodec)
{
	CString strATFileName;
	strATFileName.Format(_T("%s\\data\\%s"), m_strAppDir, _T("Profile.xml"));

	//::LoadAdvancedTypographicTables((LPTSTR)(LPCTSTR)strATFileName,(void*)&hyFontCodec);	

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
	for ( int x = 0; x < iWStringLen ; x++ )
	{			
		sTmp = htons(wpString[x]);
		CopyMemory(pDest + x * 2, &sTmp,2);
	}

	return TRUE;

}	// end of void MToWCharBigStart()

// 波峰
void CXSysBaseProxy::FindPeak(std::vector<int>& vtSum, std::vector<int>& vtPeak)
{
	vtPeak.clear();

	size_t stSum = vtSum.size();
	vector<int> vtdiff(stSum - 1, 0);
	for (size_t i = 0; i < stSum - 1; i++)
	{
		if ((vtSum[i + 1] - vtSum[i]) > 0) {
			vtdiff[i] = 1;
		}
		else if ((vtSum[i + 1] - vtSum[i]) < 0) {
			vtdiff[i] = -1;
		}
		else
			vtdiff[i] = 0;
	}

	size_t stdiff = vtdiff.size();
	for (int i = stdiff - 1; i >= 0; i--)
	{
		if (vtdiff[i] == 0 && i == stdiff - 1)
		{
			vtdiff[i] = 1;
		}
		else if (vtdiff[i] == 0)
		{
			if (vtdiff[i + 1] >= 0)
				vtdiff[i] = 1;
			else
				vtdiff[i] = -1;
		}
	}

	// 波峰
	for (vector<int>::size_type i = 0; i != vtdiff.size() - 1; i++)
	{
		// 波峰
		if (vtdiff[i + 1] - vtdiff[i] == -2)
			vtPeak.push_back(i + 1);		
	}

}	// end of void CXSysBaseProxy::FindPeak()

void CXSysBaseProxy::FindTrough(std::vector<int>& vtSum, std::vector<int>& vtTrough)
{
	vtTrough.clear();

	size_t stSum = vtSum.size();
	vector<int> vtdiff(stSum - 1, 0);
	for (size_t i = 0; i < stSum - 1; i++)
	{
		if ((vtSum[i + 1] - vtSum[i]) > 0) {
			vtdiff[i] = 1;
		}
		else if ((vtSum[i + 1] - vtSum[i]) < 0) {
			vtdiff[i] = -1;
		}
		else
			vtdiff[i] = 0;
	}

	size_t stdiff = vtdiff.size();
	for (int i = stdiff - 1; i >= 0; i--)
	{
		if (vtdiff[i] == 0 && i == stdiff - 1)
		{
			vtdiff[i] = 1;
		}
		else if (vtdiff[i] == 0)
		{
			if (vtdiff[i + 1] >= 0)
				vtdiff[i] = 1;
			else
				vtdiff[i] = -1;
		}
	}

	// 波谷
	for (vector<int>::size_type i = 0; i != vtdiff.size() - 1; i++)
	{	
		// 波谷
		if (vtdiff[i + 1] - vtdiff[i] == 2)
			vtTrough.push_back(i + 1);
	}

}	// end of void CXSysBaseProxy::FindTrough()

void CXSysBaseProxy::FindPeakAndTrough(std::vector<int>& vtSum, std::vector<int>& vtPeak, std::vector<int>& vtTrough)
{	
	vtPeak.clear();
	vtTrough.clear();

	size_t stSum = vtSum.size();
	vector<int> vtdiff(stSum - 1, 0);
	for (size_t i = 0; i < stSum - 1; i++)
	{
		if ((vtSum[i + 1] - vtSum[i]) > 0) {
			vtdiff[i] = 1;
		}
		else if ((vtSum[i + 1] - vtSum[i]) < 0) {
			vtdiff[i] = -1;
		}
		else
			vtdiff[i] = 0;
	}

	size_t stdiff = vtdiff.size();
	for (int i = stdiff - 1; i >= 0; i--)
	{
		if (vtdiff[i] == 0 && i == stdiff - 1)
		{
			vtdiff[i] = 1;
		}
		else if (vtdiff[i] == 0)
		{
			if (vtdiff[i + 1] >= 0)
				vtdiff[i] = 1;
			else
				vtdiff[i] = -1;
		}
	}

	// 波峰,波谷
	for (vector<int>::size_type i = 0; i != vtdiff.size() - 1; i++)
	{
		// 波峰
		if (vtdiff[i + 1] - vtdiff[i] == -2)		
			vtPeak.push_back(i + 1);
		// 波谷
		if (vtdiff[i + 1] - vtdiff[i] == 2)
			vtTrough.push_back(i+1);
	}

}	// end of void CXSysBaseProxy::FindPeakAndTrough()

void CXSysBaseProxy::LinearSmooth5(std::vector<int>& in, std::vector<int>& out, int N)
{
	int i;
	if (N < 5){

		for (i = 0; i <= N - 1; i++){
			out[i] = in[i];
		}
	}
	else{
		out[0] = (int)((3.0 * in[0] + 2.0 * in[1] + in[2] - in[4])/5.0 + 0.5);
		out[0] = out[0] < 0 ? 0 : out[0];
		out[1] = (int)((4.0 * in[0] + 3.0 * in[1] + 2 * in[2] + in[3])/10.0+0.5);

		for (i = 2; i <= N - 3; i++)
		{
			out[i] = (int)((in[i - 2] + in[i - 1] + in[i] + in[i + 1] + in[i + 2])/5.0+0.5);		
		}
		out[N - 2] = (int)((4.0 * in[N - 1] + 3.0 * in[N - 2] + 2 * in[N - 3] + in[N - 4])/10.0+0.5);
		out[N - 1] = (int)((3.0 * in[N - 1] + 2.0 * in[N - 2] + in[N - 3] - in[N - 5])/5.0+0.5);
		out[N - 1] = out[N - 1] < 0 ? 0 : out[N - 1];
	}

}	// end of void CXSysBaseProxy::linearSmooth5()

void CXSysBaseProxy::LinearSmooth7(std::vector<int>& in, std::vector<int>& out, int N)
{
	int i;
	if (N < 7){
		for (i = 0; i <= N - 1; i++){
			out[i] = in[i];
		}
	}
	else{

		out[0] = (int)((13.0*in[0]+10.0*in[1]+7.0*in[2]+4.0*in[3]+in[4]-2.0*in[5]-5.0*in[6])/28.0+0.5);
		out[0] = out[0] < 0 ? 0 : out[0];
		out[1] = (int)((5.0*in[0]+4.0*in[1]+3*in[2]+2*in[3]+in[4]-in[5])/14.0+0.5);
		out[2] = (int)((7.0*in[0]+6.0*in[1]+5.0*in[2]+4.0*in[3]+3.0*in[4]+2.0*in[5]+in[6])/28.0+0.5);

		for (i=3;i<=N-4;i++){
			out[i] = (int)((in[i-3]+in[i-2]+in[i-1]+in[i]+in[i+1]+in[i+2]+in[i+3])/7.0+0.5);
		}

		out[N-3] = (int)((7.0*in[N-1]+6.0*in[N-2]+5.0*in[N-3]+4.0*in[N-4]+3.0*in[N-5]+2.0*in[N-6]+in[N-7])/28.0+0.5);
		out[N-2] = (int)((5.0*in[N-1]+4.0*in[N-2]+3.0*in[N-3]+2.0*in[N-4]+in[N-5]-in[N-6])/14.0+0.5);
		out[N-2] = out[N-2]<0?0:out[N-2];
		out[N-1] = (int)((13.0*in[N-1]+10.0*in[N-2]+7.0*in[N-3]+4*in[N-4]+in[N-5]-2 *in[N-6]-5*in[N-7])/28.0+0.5);
		out[N-1] = out[N-1]<0?0:out[N-1];
		
	}
}	// end of void CXSysBaseProxy::LinearSmooth7()

void CXSysBaseProxy::QuadraticSmooth5(std::vector<int>& in, std::vector<int>& out, int N)
{
	int i;
	if (N < 5){
		for (i = 0; i <= N - 1; i++){
			out[i] = in[i];
		}
	}
	else
	{
		out[0] = (int)((31.0 * in[0] + 9.0 * in[1] - 3.0 * in[2] - 5.0 * in[3] + 3.0 * in[4])/35.0+0.5);
		out[1] = (int)((9.0 * in[0] + 13.0 * in[1] + 12 * in[2] + 6.0 * in[3] - 5.0 *in[4])/35.0+0.5);

		for (i=2; i<=N-3; i++){
			out[i] = (int)((-3.0*(in[i - 2] + in[i + 2]) + 12.0*(in[i - 1] + in[i + 1]) + 17 * in[i])/35.0+0.5);
		}
		out[N-2] = (int)((9.0*in[N-1]+13.0*in[N-2]+12.0*in[N-3]+6.0*in[N-4]-5.0*in[N-5])/35.0+0.5);
		out[N-1] = (int)((31.0*in[N-1]+9.0*in[N-2]-3.0*in[N-3]-5.0*in[N-4]+3.0*in[N-5])/35.0+0.5);
	}

}	// end of void CXSysBaseProxy::QuadraticSmooth5()


bool SortClipSumDsc(const Gridclip& vt1, Gridclip& vt2)
{
	return (vt1.iSum > vt2.iSum);

}	// end of bool SortClipSum()

bool SortAsc(const Gridclip1& i1, Gridclip1& i2)
{
	return (i1.iValue < i2.iValue);

}	// end of bool SortAsc()

bool SortDsc(const Gridclip1& i1, Gridclip1& i2)
{
	return (i1.iValue > i2.iValue);

}	// end of bool SortAsc()

//
void CXSysBaseProxy::FindMargin1(std::vector<int>& vtSum, std::vector<int>& Margin, int TopNum)
{
	Margin.clear();
	int pos = 0;
	size_t stSum = vtSum.size();
	bool b = false;	
	std::vector<Gridclip1> vt;
	Gridclip1 grdClip;	
	// 找起点
	for (size_t i = 0; i < stSum - 1; i++)
	{
		if ((vtSum[i] != vtSum[i + 1]))
		{
			grdClip.iPos = i;
			grdClip.iValue = vtSum[i];
			vt.push_back(grdClip);
			break;
		}

	}	// 

	// 滤波 
	size_t i = 0, j = 0, ZF = 30;
	while (i < stSum && j < stSum)
	{	
		if (abs(vtSum[i] - vtSum[j]) > ZF)
		{
			grdClip.iPos = j;
			grdClip.iValue = vtSum[j];
			vt.push_back(grdClip);
			i=j;
		}
		else
			j++;		
	}

	// 找波峰
	std::vector<Gridclip1> vtCrest;	
	size_t stVt = vt.size();
	for (size_t i = 0; i < stVt; i++){		
		if (vt[i].iValue < vt[i + 1].iValue){
			b = true;
			continue;
		}
		else if (vt[i].iValue> vt[i+1].iValue){		
			if (b)
			{
				grdClip.iPos= vt[i].iPos;
				grdClip.iValue= vt[i].iValue;
				vtCrest.push_back(grdClip);
			}
			b = false;
		}
	}

	// 找波谷
	std::vector<Gridclip1> vtTrough;
	for (size_t i = 0; i < stVt; i++){
		if (vt[i].iValue < vt[i + 1].iValue) {
			if (b){
				grdClip.iPos = vt[i].iPos;
				grdClip.iValue = vt[i].iValue;
				vtTrough.push_back(grdClip);
			}
		}
		else if (vt[i].iValue > vt[i+1].iValue) {
				b = true;
				continue;
		}
	}

	// 波峰排序
	std::sort(vtCrest.begin(), vtCrest.end(), SortDsc);
	// 波谷排序
	std::sort(vtTrough.begin(), vtTrough.end(), SortAsc);

	/*
	int iVTNum = (int)vtClip.size() > TopNum ? TopNum : (int)vtClip.size();

	for (int i = 0; i < iVTNum; i++)
	{
		Gridclip tmpGridClip = vtClip[i];
		for (int j = i + 1; j < iVTNum; j++)
		{
			if (tmpGridClip.iPos1 > vtClip[j].iPos1)
			{
				vtClip[i].iPos1 = vtClip[j].iPos1;
				vtClip[i].iPos2 = vtClip[j].iPos2;
				vtClip[j].iPos1 = tmpGridClip.iPos1;
				vtClip[j].iPos2 = tmpGridClip.iPos2;

				tmpGridClip.iPos1 = vtClip[i].iPos1;
				tmpGridClip.iPos2 = vtClip[i].iPos2;
			}
		}
	}

	for (int i = iVTNum; i < (int)vtClip.size(); i++)
	{
		Gridclip tmpClip = vtClip[i];
		for (int j = 0; j < iVTNum; j++)
		{
			if (tmpClip.iPos1 > vtClip[j].iPos2 && j == iVTNum - 1)
			{
				vtClip[j].iPos2 = tmpClip.iPos2;
			}
			else if (tmpClip.iPos2 < vtClip[j].iPos1 && j == 0)
			{
				vtClip[j].iPos1 = tmpClip.iPos1;
			}
			else if (tmpClip.iPos1 > vtClip[j].iPos2&&tmpClip.iPos2 < vtClip[j + 1].iPos1)
			{
				if (abs(tmpClip.iPos1 - vtClip[j].iPos2) > abs(tmpClip.iPos2 - vtClip[j + 1].iPos1))
				{
					vtClip[j + 1].iPos1 = tmpClip.iPos1;
				}
				else
				{
					vtClip[j].iPos2 = tmpClip.iPos2;
				}
			}
		}
	}

	for (int i = 0; i < iVTNum; i++)
	{
		Margin.push_back(vtClip[i].iPos1);
		Margin.push_back(vtClip[i].iPos2);
	}
	*/

}	// end of void CXSysBaseProxy::FindMargin1()

void CXSysBaseProxy::FindMargin(std::vector<int>& vtSum, std::vector<int>& Margin, int TopNum)
{
	Margin.clear();
	int pos = 0;
	size_t stSum = vtSum.size();
	bool b = false;	
	
	std::vector<Gridclip> vtClip;
	Gridclip grdClip;	
	for (size_t i = 0; i < stSum; i++)
	{	
		if (!b) {
			if (vtSum[i] > 0)
			{	
				b = true;
				grdClip.iPos1 = i;
				grdClip.iSum += vtSum[i];				
			}
		}
		else
		{
			if (vtSum[i] == 0){	
				grdClip.iPos2 = i;
				grdClip.iSum += vtSum[i];

				bool b1 = true;
				for (int j = i; j < stSum; j++){
					if (vtSum[j] > 0){
						if (j - i < ::XSysproxy().m_tagScanPrm.iGridLenThres) {
							b1 = false;
							break;
						}
						else
							break;
					}
				}				

				if (b1)	{
					vtClip.push_back(grdClip);
					b = false;
				}

			}
			else { 
				grdClip.iSum += vtSum[i];
			}
		}
	}

	std::sort(vtClip.begin(), vtClip.end(), SortClipSumDsc);

	int iVTNum = (int)vtClip.size() > TopNum ? TopNum : (int)vtClip.size();
	
	for (int i = 0; i < iVTNum; i++)
	{
		Gridclip tmpGridClip = vtClip[i];
		for (int j = i + 1; j < iVTNum; j++)
		{			
			if (tmpGridClip.iPos1 > vtClip[j].iPos1)
			{
				vtClip[i].iPos1 = vtClip[j].iPos1;
				vtClip[i].iPos2 = vtClip[j].iPos2;
				vtClip[j].iPos1 = tmpGridClip.iPos1;
				vtClip[j].iPos2 = tmpGridClip.iPos2;

				tmpGridClip.iPos1 = vtClip[i].iPos1;
				tmpGridClip.iPos2 = vtClip[i].iPos2;				 
			}
		}		
	}	
	
	for (int i = iVTNum; i < (int)vtClip.size(); i++)
	{
		Gridclip tmpClip = vtClip[i];
		for (int j = 0; j < iVTNum; j++)
		{
			if (tmpClip.iPos1 > vtClip[j].iPos2 && j == iVTNum-1)
			{
				vtClip[j].iPos2 = tmpClip.iPos2;
			}
			else if (tmpClip.iPos2 < vtClip[j].iPos1 && j == 0)
			{
				vtClip[j].iPos1 = tmpClip.iPos1;
			}
			else if (tmpClip.iPos1 > vtClip[j].iPos2&&tmpClip.iPos2 < vtClip[j + 1].iPos1)
			{
				if (abs(tmpClip.iPos1 - vtClip[j].iPos2) > abs(tmpClip.iPos2 - vtClip[j + 1].iPos1))
				{
					vtClip[j + 1].iPos1 = tmpClip.iPos1;
				}
				else
				{
					vtClip[j].iPos2 = tmpClip.iPos2;
				}
			}
		}
	}

	for (int i = 0; i < iVTNum; i++)
	{
		Margin.push_back(vtClip[i].iPos1);
		Margin.push_back(vtClip[i].iPos2);
	}

}	// end of void CXSysBaseProxy::FindMargin()

//////////////////////////////////////////////////////////////////
//函数功能：用向量来做COSα=两向量之积/两向量模的乘积求两条线段夹角
//输入：   线段3个点坐标pt1,pt2,pt0,最后一个参数为公共点
//输出：   线段夹角，单位为角度
//////////////////////////////////////////////////////////////////
double CXSysBaseProxy::Angle(CPoint* pt1, CPoint* pt2, CPoint* ptCen)
{
	double dx1 = pt1->x - ptCen->x;
	double dy1 = pt1->y - ptCen->y;
	double dx2 = pt2->x - ptCen->x;
	double dy2 = pt2->y - ptCen->y;
	double angle_line = (dx1*dx2 + dy1 * dy2) / sqrt((dx1*dx1 + dy1 * dy1)*(dx2*dx2 + dy2 * dy2) + 1e-10);//余弦值
	return acos(angle_line) * 180 / 3.141592653;

}	// end of double CHYImageFontBuilderDoc::Angle()

int CXSysBaseProxy::FindMaxValuePos(std::vector<int>& vt, int Pos1, int Pos2)
{
	size_t stNumber = vt.size();
	if (Pos2 >= stNumber) return -1;

	int iMax = vt[Pos1];
	int iMaxPos = Pos1;
	for (size_t i = Pos1+1; i <= Pos2; i++)
	{
		int iNum = vt[i];
		if (iNum > iMax)
		{
			iMax = vt[i];
			iMaxPos = i;
		}
	}

	return iMaxPos;

}	// end of double CXSysBaseProxy::FindMaxValue()
