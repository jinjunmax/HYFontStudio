#ifndef HY_IK_CODEC_DEF
#define HY_IK_CODEC_DEF

#ifdef HYIKCODEC_EXPORTS
#define HYIKCODEC_API __declspec(dllexport)
#else
#define HYIKCODEC_API __declspec(dllimport)
#endif

#include <vector>

#define	HY_SHORT_SZ			sizeof(short)
#define	HY_IK_RECORD		2048
#define HY_MAX_NUMBER		21800

#define IK_PT_FLG_START		12
#define IK_PT_FLG_CORNER	13
#define IK_PT_FLG_CURVE		14
#define IK_PT_FLG_TANGENT	15


#endif 