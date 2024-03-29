
// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

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


#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxole.h>
#include <afxoutlookbartabctrl.h>
#include <afxcontrolbars.h>     // 功能区和控件条的 MFC 支持

#include <afxwin.h>
#include <math.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <GdiPlus.h>
#include<direct.h>
using namespace Gdiplus;

typedef struct	_DRAWFONTPARAM
{
	int		iPoint;
	int		iPointNo;
	int		iLine;
	int		iReferLine;
	int		iFill;
	int		iViewType;

} DRAWFONTPARAM;

#define		VW_PARAM_HEAD		0
#define		VW_PARAM_HHEA		1
#define		VW_PARAM_OS_TYPO	2
#define		VW_PARAM_OS_WIN		3

#define		VM_UPDATEALLVIEWS			(WM_USER + 0x2000)			// 更新视窗口
#define		MSG_PREVIEW_WND				(VM_UPDATEALLVIEWS+100)
#define		MSG_POPVIEW_WND				(VM_UPDATEALLVIEWS+101)
#define		MSG_TRACE_BUILD_ADD_INF		(VM_UPDATEALLVIEWS+1000)	// 添加跟踪信息
#define		MSG_TRACE_BUILD_CLEAR_INF	(VM_UPDATEALLVIEWS+1001)	// 清除跟踪信息
#define		MSG_UPDATETHUM				(VM_UPDATEALLVIEWS+1002)	// 更新字体预览窗口
#define		MSG_TRACERECT				(VM_UPDATEALLVIEWS+1003)	// 
#define		MSG_PRS_MAIN				(VM_UPDATEALLVIEWS+1004)	// 总处理进度
#define		MSG_PRS_SUB					(VM_UPDATEALLVIEWS+1005)	// 每页文件处理进度
#define		MSG_PRS_SHOW				(VM_UPDATEALLVIEWS+1006)	// 显示/隐藏进度窗口


#define		WNDSZ_THUM_EM				54
#define		CNVT_PREPOINT_NUM			4096

#define		PVEEM						700
#define		MYTEST						1

//#include "ximage.h"
#include "Markup.h"

#include "opencv2/core/core.hpp"  
#include "opencv2/core.hpp"  
#include "opencv2/highgui/highgui.hpp"  
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
using namespace cv;


#include "HYErrorDef.h"
#include "HYBaseFunction.h"
#include "HYFontCodecHead.h"
#include "HYImageToFont.h"
#include "FontShowWnd.h"
#include "HYFontApi.h"

#include "XSysBaseProxy.h"
#include "XSysProxy.h"



using namespace HYFONTCODEC;
using namespace HYIMGTOFONT;

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


