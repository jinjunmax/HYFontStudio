#ifndef HY_BASEFUNCTION_H
#define HY_BASEFUNCTION_H

#include <string>
#include <vector>

#ifdef HYBASEFUNCTION_EXPORTS
#define HY_DLL_API __declspec(dllexport)
#else
#define HY_DLL_API __declspec(dllimport)
#endif

# define hy_cdr_int64_to(from)               \
	( (((from) & (0xffull << 56)) >> 56)	 \
	| (((from) & (0xffull << 48)) >> 40)	 \
	| (((from) & (0xffull << 40)) >> 24)	 \
	| (((from) & (0xffull << 32)) >> 8)		 \
	| (((from) & (0xffull << 24)) << 8)      \
	| (((from) & (0xffull << 16)) << 24)     \
	| (((from) & (0xffull << 8)) << 40)      \
	| (((from) & 0xffull) << 56) )

# define hy_cdr_int32_to(from)               \
	( (((from) & 0xff000000) >> 24)			 \
	| (((from) & 0x00ff0000) >> 8)           \
	| (((from) & 0x0000ff00) << 8)           \
	| (((from) & 0x000000ff) << 24) )

# define hy_cdr_int16_to(from)               \
	( (((from) & (0xffu << 8)) >> 8)         \
	| (((from) & 0xffu) << 8) )

#ifndef HY_FIXED_STRUCT
#define HY_FIXED_STRUCT
typedef struct HY_FIXED 
{
	short			value;
	unsigned short	fract;	
} HYFIXED;

#endif
unsigned int		HY_DLL_API  HY_NumerAlign(unsigned int n, unsigned int align);
int					HY_DLL_API  HY_RealRount(double db);
long				HY_DLL_API  HY_FileExist(const char* pFile);
long				HY_DLL_API	HY_FileLength(const char* pFile);

unsigned __int64	HY_DLL_API  HY_CVS_int64_to(unsigned __int64 from);
unsigned long		HY_DLL_API  HY_CVS_int32_to(unsigned long from);
unsigned short		HY_DLL_API  HY_CVS_int16_to(unsigned short from);

float 				HY_DLL_API  HY_fixed_to_float(long fx);
long				HY_DLL_API	HY_float_to_fixed(float f);

float 				HY_DLL_API  HY_HYFIXED_to_float(HYFIXED fx);
HYFIXED				HY_DLL_API	HY_float_to_HYFIXED(float f);

float 				HY_DLL_API	HY_F2DOT14_to_float(short F2DOT14);
short				HY_DLL_API	HY_float_to_F2DOT14(float f);

void				HY_DLL_API	HY_GetFilesFromDir(std::string strPath, std::vector<std::string>& files);
std::string			HY_DLL_API	HY_GetDirFromPath(std::string strPath);
std::string			HY_DLL_API	HY_GetFileNameFromPath(std::string strPath);
std::string			HY_DLL_API	HY_GetExtNameFromPath(std::string strPath);
std::string			HY_DLL_API	HY_GetFullNameFromPath(std::string strPath);
int					HY_DLL_API	HY_SpliteString(const char* pSrcString, char* pSpliteFlag, std::vector<std::string>& vtSplitItem);

std::string			HY_DLL_API	HY_StringToLowercase (std::string str);
std::string			HY_DLL_API	HY_StringToUppercase (std::string str);
std::string			HY_DLL_API	HY_String_to_hex(const std::string& str);
std::string			HY_DLL_API  HY_Hex_to_string(const std::string& str);
std::string			HY_DLL_API	HY_StringFormat(const char *fmt, ...);
long				HY_DLL_API	HY_HexStringToInt(std::string hex);

unsigned long		HY_DLL_API	HY_CalcFontTableChecksum(unsigned long *Table, unsigned long Length);

int					HY_DLL_API  CheckUniRepeat(unsigned long ulUnicod, std::vector<unsigned long>& vtUnicode);
void				HY_DLL_API  RemoveUniRepeat(std::vector<unsigned long>& vtInUni);


// 读取字表文件
int					HY_DLL_API  HY_LoadCharTableFile(char* strTxtFile, std::vector<unsigned long>& vtUnicode);
// 读取字表文件
int					HY_DLL_API	HY_TXTFileToUniArray(char* pTxt, std::vector<unsigned long>& vtUni);
// 保存unicode到字表文件
int					HY_DLL_API	HY_UniArrayToTXTFile(std::vector<unsigned long>& vtUni,char* pTxt);
// 读取PostScrpitName文件
int					HY_DLL_API	HY_LoadPsNameFile(char* strNameFile, std::vector<std::string>& vtName);
// 读取码表文件
void				HY_DLL_API	HY_LoadCodeFile(char* strcodeFile, std::vector<unsigned long>& vtUni);

bool				HY_DLL_API  HY_MultiToWChar(WCHAR* pDest, int& iDestlen, char* pSrc);
bool				HY_DLL_API	MulToWCharBL(char* pDest, int& iDestlen, const char* pSrc, int iSrclen);
std::string			HY_DLL_API	HY_Unicode_to_AnsiString(unsigned short Unicode);
unsigned long		HY_DLL_API	HY_AnsiStringToUnicode(char* pAnsiString);
bool				HY_DLL_API  HY_WCharTOMultiChar(char* pDest, int& iDestlen, unsigned short Unicode);
bool				HY_DLL_API	HY_WCharTOMultiCharEx(std::string  strOut, char* pSrc, int iSrclen);

std::string			HY_DLL_API	HY_trim(std::string const& str);
int					HY_DLL_API  HY_ReadTextLine(char* filename, std::vector<std::string>& szlineText);
void				HY_DLL_API  HY_WriteLog(std::string SavePath, std::string& Text, bool bNew=false);
void				HY_DLL_API	HY_4ByteAlign(FILE* pF, unsigned long length);
void				HY_DLL_API  HY_BrodMessage(UINT Msg, WPARAM wParam, LPARAM lParam);




#endif 