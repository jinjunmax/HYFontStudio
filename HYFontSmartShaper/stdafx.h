
// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展
#include <afxdisp.h>        // MFC 自动化类

#include <afxole.h>
#include <afxoutlookbartabctrl.h>
#include <afxcontrolbars.h>     // 功能区和控件条的 MFC 支持


#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC 对 Windows 公共控件的支持
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

#define		VM_UPDATEALLVIEWS			(WM_USER + 0x2000)			// 更新视窗口
#define		MSG_POPVIEW_WND				(VM_UPDATEALLVIEWS+1000)	// 打开预览窗口
#define		MSG_PREVIEW_WND				(VM_UPDATEALLVIEWS+1001)	// 打开预览窗口

#define		MSG_SET_VARATION_WND		(VM_UPDATEALLVIEWS+1002)	// 可以字体字重变型参数
#define		MSG_FONT_LOAD_FINISH		(VM_UPDATEALLVIEWS+1003)	// 可以字体字重变型参数


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


