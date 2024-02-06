
// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // �� Windows ͷ���ų�����ʹ�õ�����
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

// �ر� MFC ��ĳЩ�����������ɷ��ĺ��Եľ�����Ϣ������
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxext.h>         // MFC ��չ
#include <afxdisp.h>        // MFC �Զ�����

#include <afxole.h>
#include <afxoutlookbartabctrl.h>
#include <afxcontrolbars.h>     // �������Ϳؼ����� MFC ֧��


#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC �� Internet Explorer 4 �����ؼ���֧��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC �� Windows �����ؼ���֧��
#endif // _AFX_NO_AFXCMN_SUPPORT



#pragma warning(disable:4309)
#pragma warning(disable:4018)
#pragma warning(disable:4244)

#include <vector>
using namespace std;
#include <GdiPlus.h>
using namespace Gdiplus;
#include "HYFontCodecHead.h"
using namespace HYFONTCODEC;

#define		VW_PARAM_HEAD		0
#define		VW_PARAM_HHEA		1
#define		VW_PARAM_OS_TYPO	2
#define		VW_PARAM_OS_WIN		3

typedef struct	_DRAWFONTPARAM
{
	int		iPoint;
	int		iPointNo;
	int		iLine;
	int		iReferLine;
	int		iFill;
	int		iViewType;

} DRAWFONTPARAM;

#define		VM_UPDATEALLVIEWS			(WM_USER + 0x2000)			// �����Ӵ���
#define		MSG_POPVIEW_WND				(VM_UPDATEALLVIEWS+1000)	// ��Ԥ������
#define		MSG_PREVIEW_WND				(VM_UPDATEALLVIEWS+1001)	// ��Ԥ������

#define		MSG_SET_VARATION_WND		(VM_UPDATEALLVIEWS+1002)	// �����������ر��Ͳ���
#define		MSG_FONT_LOAD_FINISH		(VM_UPDATEALLVIEWS+1003)	// �����������ر��Ͳ���


#define		WNDSZ_THUM_EM				54
#define		CNVT_PREPOINT_NUM			4096

#ifdef		DEBUG
#define		MYTRACE						0001
#endif

#define	    ERROR_MAX					-99999.0f
#define		ERROR_MIN					99999.0f

#define		MAKE_J						1
#define		MAKE_F						2

#define		MYDEBUG						1
//#define		BIGFILE						1

#include <afxwin.h>
#include <afxdlgs.h>
#include <math.h>
#include <complex>
#include <sstream>
#include <fstream>
#include <errno.h>
#include <list>
#include <algorithm>
#include "resource.h"

#include "HYErrorDef.h"
#include "Markup.h"
#include "HYBaseFunction.h"
#include "HYFontCodecHead.h"
#include "FontShowWnd.h"
#include "CompareFontWnd.h"
#include "ThumFontWnd.h"
#include "FontZoomWnd.h"
#include "FontCmprZoomWnd.h"
#include "HYIkCodec.h"
#include "IKName.h"
#include "HY_Global.h"
#include "DefIK.h"
#include "HYFontEngine.h"
#include "TTFFilter.h"
#include "OTFCore.h"
#include "HYFontApi.h"
#include "FontExtract.h"

#include "VrAxisRecords.h"
#include "CVrMasters.h"

#include "XSysBaseProxy.h"
#include "XSysProxy.h"
#include <afxdlgs.h>

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


