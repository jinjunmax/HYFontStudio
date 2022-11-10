// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

// Windows Header Files:
#include <windows.h>

#pragma warning(disable:4275)
#pragma warning(disable:4251)
#pragma warning(disable:4996)
#pragma warning(disable:4244)
#pragma warning(disable:4309)
#pragma warning(disable:4018)
#pragma warning(disable:4267)

#include <process.h> 
#include <vector>
#include <map>
#include <fstream>
#include <iostream>
#include <sstream>
#include <list>
#include <sys/types.h>
#include <sys/stat.h>
#include <direct.h>
#include <algorithm>

#include "Markup.h"
#include "HYErrorDef.h"
#include "HYFontCodecDef.h"
#include "HYBaseFunction.h"
#include "HYFontCodecPrivteDef.h"
#include "HYCharSetRang.h"

// TODO: reference additional headers your program requires here
