// HYFontConsoleTool.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "HYFontConsoleTool.h"
#include "HYFontApi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#if defined(UNICODE)
#define _tcout std::wcout
#else
#define _tcout std::cout
#endif

// The one and only application object

CWinApp theApp;

using namespace std;
using namespace HYFONTCODEC;

#include "HYFontCodecHead.h"

#define  SF_TAG 1
#define  ST_TAG 2
#define  WF_TAG 3
#define  WM_TAG 4
#define  SWP_TAG 5

void FindTableFlag(std::vector<unsigned long>& vtTblFlg, CString strTableName)
{
	char splitFlg[]	= _T(",");
	std::vector<std::string> vtStrArry;

	int iFlgNumber = ::HY_SpliteString((LPTSTR)(LPCTSTR)strTableName,splitFlg,vtStrArry);
	for (int j = 0; j < iFlgNumber; j++ )
	{
		std::transform(vtStrArry[j].begin(),vtStrArry[j].end(),vtStrArry[j].begin(),std::toupper);

		if (vtStrArry[j]=="CMAP"){
			vtTblFlg.push_back(CMAP_TAG);
		}
		if (vtStrArry[j]=="HEAD"){
			vtTblFlg.push_back(HEAD_TAG);
		}
		if (vtStrArry[j]=="HHEA"){
			vtTblFlg.push_back(HHEA_TAG);
		}
		if (vtStrArry[j]=="HMTX"){
			vtTblFlg.push_back(HMTX_TAG);
		}				
		if (vtStrArry[j]=="MAXP"){
			vtTblFlg.push_back(MAXP_TAG);
		}
		if (vtStrArry[j]=="NAME"){
			vtTblFlg.push_back(NAME_TAG);
		}
		if (vtStrArry[j]=="OS/2"){
			vtTblFlg.push_back(OS2_TAG);
		}
		if (vtStrArry[j]=="POST"){
			vtTblFlg.push_back(POST_TAG);
		}
		if (vtStrArry[j]=="CVT " || vtStrArry[j]=="CVT")
		{
			vtTblFlg.push_back(CVT_TAG);
		}
		if (vtStrArry[j]=="FPGM"){
			vtTblFlg.push_back(FPGM_TAG);
		}
		if (vtStrArry[j]=="GLYF"){
			vtTblFlg.push_back(GLYF_TAG);
		}
		if (vtStrArry[j]=="LOCA"){
			vtTblFlg.push_back(LOCA_TAG);
		}
		if (vtStrArry[j]=="PREP"){
			vtTblFlg.push_back(PREP_TAG);
		}
		if (vtStrArry[j]=="GASP"){
			vtTblFlg.push_back(GASP_TAG);
		}
		if (vtStrArry[j]=="CFF " || vtStrArry[j]== "CFF"){
			vtTblFlg.push_back(CFF_TAG);
		}
		if (vtStrArry[j]=="VORG"){
			vtTblFlg.push_back(VORG_TAG);
		}
		if (vtStrArry[j]=="SVG" || vtStrArry[j]=="SVG "){
			vtTblFlg.push_back(SVG_TAG);
		}
		if (vtStrArry[j]=="EBDT"){
			vtTblFlg.push_back(EBDT_TAG);
		}
		if (vtStrArry[j]=="EBLC"){
			vtTblFlg.push_back(EBLC_TAG);
		}
		if (vtStrArry[j]=="EBSC"){
			vtTblFlg.push_back(EBSC_TAG);
		}
		if (vtStrArry[j]=="CBDT"){
			vtTblFlg.push_back(CBDT_TAG);
		}
		if (vtStrArry[j]=="CBLC"){
			vtTblFlg.push_back(CBLC_TAG);
		}				
		if (vtStrArry[j]=="BASE"){
			vtTblFlg.push_back(BASE_TAG);
		}
		if (vtStrArry[j]=="GDEF"){
			vtTblFlg.push_back(GDEF_TAG);
		}
		if (vtStrArry[j]=="GPOS"){
			vtTblFlg.push_back(GPOS_TAG);
		}
		if (vtStrArry[j]=="GSUB"){
			vtTblFlg.push_back(GSUB_TAG);
		}	
		if (vtStrArry[j]=="JSTF"){
			vtTblFlg.push_back(JSTF_TAG);
		}	
		if (vtStrArry[j]=="MATH"){
			vtTblFlg.push_back(MATH_TAG);
		}	
		if (vtStrArry[j]=="DSIG"){
			vtTblFlg.push_back(DSIG_TAG);
		}	
		if (vtStrArry[j]=="HDMX"){
			vtTblFlg.push_back(HDMX_TAG);
		}	
		if (vtStrArry[j]=="KERN"){
			vtTblFlg.push_back(KERN_TAG);
		}	
		if (vtStrArry[j]=="LTSH"){
			vtTblFlg.push_back(LTSH_TAG);
		}	
		if (vtStrArry[j]=="PCLT"){
			vtTblFlg.push_back(PCLT_TAG);
		}
		if (vtStrArry[j]=="VDMX"){
			vtTblFlg.push_back(VDMX_TAG);
		}
		if (vtStrArry[j]=="VHEA"){
			vtTblFlg.push_back(VHEA_TAG);
		}
		if (vtStrArry[j]=="VMTX"){
			vtTblFlg.push_back(VMTX_TAG);
		}
		if (vtStrArry[j]=="COLR"){
			vtTblFlg.push_back(COLR_TAG);
		}
		if (vtStrArry[j]=="CPAL"){
			vtTblFlg.push_back(CPAL_TAG);
		}				
		if (vtStrArry[j]=="FEAT"){
			vtTblFlg.push_back(FEAT_TAG);
		}				
		if (vtStrArry[j]=="MORX"){
			vtTblFlg.push_back(MORX_TAG);
		}
	}

}	// end of void FindTableFlag()

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{	

	int				nRetCode = 0;
	CFontExtract	FontExtract;
	CString			strArg = _T("");
	CString			strFile = _T("");
	CString			strPrint;
	unsigned long	ulFlag = 0;  

	std::vector<CString> vtFileName;
	std::vector<int>	vtTag;	

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
		nRetCode = 1;
	}
	else
	{
		strArg.Format(_T("%s"), argv[1]);
		strArg.MakeLower();

		if (strArg == _T("-h") )
		{
			//提取一个子字库 0 保留码位 1 不保留码位
			_tcout << _T("1: 提取一个子字库 0 txt为字表文本，保留码位 1 txt为字表文件不保留码位 2 txt为name文本，不保留码位") << std::endl;	
			_tcout << _T("sample: HYFontConsoleTool -exf d:\\sample.ttf d:\\sample.txt 0") << std::endl;
			// ttf 转换到woff 
			_tcout << _T("\n2: ttf转换到webfont") << std::endl;			
			_tcout << _T("sample: HYFontConsoleTool -t2w d:\\sample.ttf d:\\sample.zip") << std::endl;

			_tcout << _T("\n3: 通过字表将ttf转换到webfont") << std::endl;			
			_tcout << _T("字表文件格式要求: UTF-16,LITTLE ENDIAN") << std::endl;
			_tcout << _T("sample: HYFontConsoleTool -t2w1 d:\\sample.ttf d:\\sample.zip d:\\sample.txt") << std::endl;

			// woff转换到ttf
			_tcout << _T("\n4: woff转换到ttf") << std::endl;
			_tcout << _T("sample: HYFontExtract -wf2tf d:\\sample.woff d:\\sample.ttf") << std::endl;
			
			return nRetCode;
		}		

		CString strTemp;
		//提取一个子字库 0 保留码位 1 不保留码位
		if (strArg==_T("-exf")){	
			strTemp.Format(_T("%s"), argv[3]);
			strTemp.Trim();
			strTemp.MakeUpper();

			//解析TXT文档
			CString strNewTTFFile = _T("");
			strNewTTFFile = strTemp.Left(strTemp.ReverseFind(_T('.'))+1);
			strNewTTFFile += _T("ttf");	

			int iError = FontExtract.ExtractFont((LPTSTR)(LPCTSTR)argv[2],(LPTSTR)(LPCTSTR)strNewTTFFile,(LPTSTR)(LPCTSTR)argv[3],atoi(argv[4]));
			if ( iError == 0) {
				_tcout << _T("Font file extract success") << std::endl;
			}
			else {	
				_tcout << _T("Font file extract fail") << std::endl;
			} 
		}
		// ttf转换到webfont
		else if (strArg == _T("-t2w")) {
			if (::TTF2WebFont((LPTSTR)(LPCTSTR)argv[2], (LPTSTR)(LPCTSTR)argv[3]) == 0)
				_tcout << _T("Font convert to woff success") << std::endl;
			else
				_tcout << _T("Font convert to woff fail") << std::endl;

		}	
		//通过字表将ttf转换到webfont	
		else if (strArg == _T("-t2w1")){
			if ( ::TTF2WebFont1((LPTSTR)(LPCTSTR)argv[2],(LPTSTR)(LPCTSTR)argv[3],(LPTSTR)(LPCTSTR)argv[4]) == 0)
					_tcout << _T("Font convert to woff success") << std::endl;				
				else 
					_tcout << _T("Font convert to woff fail") << std::endl;
		} 
		 //woff转换到ttf
		else if (strArg==_T("-wtf")){
			HYFONTCODEC::WoffCodec		wffc;
			if (wffc.Woff2Sfnt((LPTSTR)(LPCTSTR)argv[2],(LPTSTR)(LPCTSTR)argv[3]) == HY_NOERROR)
				_tcout << _T("woff convert to font success") << std::endl;
			else 
				_tcout << _T("woff convert to font fail") << std::endl;
		}		
		else{
			_tcout << _T("Processing error") << std::endl;
		}
	}	
	return nRetCode;

}	// end of int _tmain()
