// HYBaseFunction.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "HYBaseFunction.h"
#include <ios>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <algorithm> 
#include <string>
#include <locale>
#include <functional>
#include <cctype>
#include <math.h>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <iostream>
#include <fstream>

using namespace std;

unsigned int HY_NumerAlign(unsigned int n, unsigned int align)
{
	return ((n + align - 1) & (~(align - 1)));

}	// end of int  HY_FourAlign()

int HY_RealRount(double db)
{
	int iRtn = 0;
	db>0? iRtn=(int)(db+0.5f):iRtn=(int)(db-0.5f);

	return iRtn;

}	// end of BOOL int HYDoubleRount()

long  HY_FileExist(const char* pFile)
{
	struct _stat info;   
	memset(&info, 0, sizeof(info));
	int result = _stat(pFile, &info);   
	long size = 0;

	// Check if staHY_ERR_FILE_ZEROtistics are valid: 
	if( result != 0 )
	{		
		switch (errno)
		{
		case ENOENT:
			size =  -1;
			break;
		case EINVAL:
			size = -2;
			break;
		default:
			size = -3;
			break;			
		}
	}
	else 
	{
		size = info.st_size;
	}

	_set_errno(0);

	return size;

}	// end of int  HY_IsFileExist()

long HY_FileLength(const char* pFile)
{
	struct _stat info;   
	memset(&info, 0, sizeof(info));
	int result = _stat(pFile, &info);   
	long size = 0;

	// Check if statistics are valid: 
	if( result != 0 )
	{		
		switch (errno)
		{
		case ENOENT:			
			break;
		case EINVAL:			
			break;
		default:			
			break;			
		}
	}
	else 
	{
		size = info.st_size;
	}

	_set_errno(0);

	return size;

}	// end of long HY_FileLength()

unsigned __int64 HY_CVS_int64_to(unsigned __int64 from)
{
	unsigned __int64 out;

	out = (((from)&(0xffull<<56))>>56);
	out |= (((from)&(0xffull<<48))>>40);
	out |= (((from)&(0xffull<<40))>>24);
	out |= (((from)&(0xffull<<32))>>8);
	out |= (((from)&(0xffull<<24))<<8);
	out |= (((from)&(0xffull<<16))<<24);
	out |= (((from)&(0xffull<<8))<<40);
	out |= (((from) & 0xffull) << 56);	

	return out;

}	// end of unsigned __int64 HY_Cdr_int64_to()

unsigned long HY_CVS_int32_to(unsigned long from)
{
	unsigned long out = 0;

	out = (((from) & (0xfful << 24)) >> 24);
	out |=((from) & (0xfful << 16)) >> 8;
	out |=((from) & (0xfful << 8)) << 8;
	out |= ((from) & 0xfful) << 24;

	return out;
	
}	// end of unsigned long HY_Cdr_int32_to()

unsigned short HY_CVS_int16_to(unsigned short from)
{
	unsigned short out = 0;
	out = (((from) & (0xffu<<8)) >> 8);
	out |= (((from)&0xffu)<< 8);

	return out;

}	// end of unsigned short HY_Cdr_int64_to()

float HY_fixed_to_float(long fx)
{
	return (fx>>16)+(fx&0xffff)/65536.0f;

}	// end of float HY_fixed_to_float()

long HY_float_to_fixed(float f)
{
	//long n=(long)(f*65536);
	HYFIXED	 fixed;
	fixed.value = (short)f;
	fixed.fract = abs((f-fixed.value))*65536;

	long n =  fixed.value<<16;
	n = n|fixed.fract;

	return n;

}	// end of long HY_float_to_fixed()

float HY_HYFIXED_to_float(HYFIXED fx)
{
	return fx.value+(fx.fract&0xffff)/65536.0f;

}	// end of float HY_HYFIXED_to_float()

HYFIXED	 HY_float_to_HYFIXED(float f)
{
	HYFIXED	 fixed;
	fixed.value = (short)f;
	fixed.fract = (f-fixed.value)*65536;

	return fixed;

}	// end of HYFIXED	 HY_float_to_HYFIXED()

float HY_F2DOT14_to_float(short F2DOT14)
{	
	short  sValue;
	if (0x8000&F2DOT14)
		sValue = -((~(F2DOT14>>14))+1);					
	else 					
		sValue = F2DOT14>>14;

	float fFraction = (F2DOT14&0x3FFF)/16384.0f;
	float fValue = sValue+fFraction;

	return fValue;

}	// end of float HY_F2DOT14_to_float()

short HY_float_to_F2DOT14(float f)
{
	if (f == 0) return (short)0;

	if (f>1.999939 || f<-2.0) return 0;

	short sValue = (short)f;

	//记录扩大10的n次方倍
	int a =0;
	float ftmp = f;
	while(((ftmp-(int)ftmp)<0.00001) && ((ftmp-(int)ftmp)>-0.00001)) //用标准的float型判断为零的方式更好：((x-(int)x)<0.00001)&&((x-(int)x)>-0.00001)
	{
		ftmp= ftmp*10;
		a++;	
	}

	unsigned short fraction = abs(f-sValue)* pow(10.0,a);

	return (sValue<<14)|fraction;

}	// end of short HY_float_to_F2DOT14()

void HY_GetFilesFromDir(std::string strPath, std::vector<std::string>& files)
{
	//文件句柄  
	long   hFile = 0;
	//文件信息，声明一个存储文件信息的结构体  
	struct _finddata_t fileinfo;
	std::string p;//字符串，存放路径
	if ((hFile = _findfirst(p.assign(strPath).append("\\*").c_str(), &fileinfo)) != -1)//若查找成功，则进入
	{
		do
		{
			//如果是目录,迭代之（即文件夹内还有文件夹）  
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				//文件名不等于"."&&文件名不等于".."
				//.表示当前目录
				//..表示当前目录的父目录
				//判断时，两者都要忽略，不然就无限递归跳不出去了！
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					continue;
					//getFiles(p.assign(strPath).append("\\").append(fileinfo.name), files);
			}
			//如果不是,加入列表
			else
			{
				files.push_back(p.assign(strPath).append("\\").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		//_findclose函数结束查找
		_findclose(hFile);
	}

}	// end of void HY_GetFilesFromDir()

std::string HY_GetDirFromPath(std::string strPath)
{
	char szPath[260], szDrive[20], szDir[256], szFName[256], szExt[256];
	std::string strout;

	lstrcpy(szPath, strPath.data());	
	_splitpath_s(szPath, szDrive, szDir, szFName, szExt);
	strout = szDrive;
	strout += szDir;

	return strout;

}	//end of HY_GetDirFromPath()

std::string HY_GetFileNameFromPath(std::string strPath)
{
	char szPath[_MAX_PATH], szDrive[20], szDir[256], szFName[256], szExt[256];
	std::string strout;

	lstrcpy(szPath, strPath.data());	
	_splitpath_s(szPath, szDrive, szDir, szFName, szExt);

	strout = szFName;
	return strout;

}	//end of HY_GetFileNameFromPath()

std::string HY_GetExtNameFromPath(std::string strPath)
{
	char szPath[_MAX_PATH], szDrive[20], szDir[256], szFName[256], szExt[256];
	std::string strout;

	lstrcpy(szPath, strPath.data());	
	_splitpath_s(szPath, szDrive, szDir, szFName, szExt);

	strout = szExt;

	return strout;

}	//end of HY_GetExtNameFromPath()

std::string HY_GetFullNameFromPath(std::string strPath)
{
	char szPath[_MAX_PATH], szDrive[20], szDir[256], szFName[256], szExt[256];
	std::string strout;

	lstrcpy(szPath, strPath.data());	
	_splitpath_s(szPath, szDrive, szDir, szFName, szExt);
	
	strout = szFName;
	strout += szExt;

	return strout;

}	//end of HY_GetFullNameFromPath()

int	HY_SpliteString(const char* pSrcString, char* pSpliteFlag, std::vector<std::string>& vtSplitItem)
{	
	//char*	temp = (char*)malloc(sizeof(char)*(strlen(pSrcString)+1));
	int ilen = strlen(pSrcString)+1;
	char*	temp = new char[ilen];
	memset(temp,0, ilen);
	strcpy(temp, pSrcString);

	char*	token = strtok(temp, pSpliteFlag);
	int		iCount = 0;
	while (NULL != token)
	{
		std::string strToken;
		strToken = token;
		vtSplitItem.push_back(strToken);

		token = strtok(NULL, pSpliteFlag);
		iCount++;
	}
	delete[] temp;
	//free(temp);

	return iCount;

}	// end of int HY_SpliteString()

std::string	HY_StringToLowercase (std::string str)
{	
	transform(str.begin(), str.end(), str.begin(), ::tolower);
	return str;

}	// end of std::string	HY_StringToLowercase ()

std::string	HY_StringToUppercase (std::string str)
{
	transform(str.begin(), str.end(), str.begin(), ::toupper);
	return str;

}	// end of std::string	HY_StringToUppercase ()

std::string HY_String_to_hex(const std::string& str)
{
	static const char* const lut = "0123456789ABCDEF";
	size_t len = str.length();

	std::string output;
	output.reserve(2 * len);
	for (size_t i = 0; i < len; ++i)
	{
		const unsigned char c = str[i];
		output.push_back(lut[c >> 4]);
		output.push_back(lut[c & 15]);
	}
	return output;

}	// end of std::string string_to_hex()

std::string HY_Hex_to_string(const std::string& Hex)
{
	static const char* const lut = "0123456789ABCDEF";
	size_t len = Hex.length();
	if (len & 1) return "";

	std::string output;
	output.reserve(len/2);
	for (size_t i = 0; i < len; i += 2)
	{
		char a = Hex[i];
		const char* p = std::lower_bound(lut, lut + 16, a);
		if (*p != a) return "";

		char b = Hex[i + 1];
		const char* q = std::lower_bound(lut, lut + 16, b);
		if (*q != b)  return "";

		output.push_back(((p - lut) << 4) | (q - lut));
	}
	return output;

}	// end of std::string hex_to_string()

std::string HY_Int_to_Hexstring(long l)
{	
	std::stringstream sstream;
	sstream << std::hex << l;

	return HY_StringToUppercase(sstream.str());

}	// end of std::string HY_Int_to_Hexstring()

std::string HY_StringFormat(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);

	const size_t SIZE = 512;
	char buffer[SIZE] = { 0 };
	vsnprintf(buffer, SIZE, fmt, ap);

	va_end(ap);

	return std::string(buffer);

}	// end of std::string HY_StringFormat()

long HY_HexStringToInt(std::string hex)
{
	return strtol(hex.c_str(), NULL, 16);

}	// end of long HY_HexStringToInt()

unsigned long HY_CalcFontTableChecksum(unsigned long *Table, unsigned long Length)
{
	unsigned long	Sum = 0L;	
	unsigned long*	Endptr = Table+((Length+3) & ~3) / sizeof(unsigned long);

	while (Table < Endptr)
	{
		Sum += HY_CVS_int32_to(*Table);
		Table++;		
	}

	return Sum;

}	// end of unsigned long HY_CalcFontTableChecksum()

int	 CheckUniRepeat(unsigned long	ulUnicod, std::vector<unsigned long>& vtUnicode)
{
	size_t stlp = vtUnicode.size();
	for (size_t i = 0; i < stlp; i++){
		if (vtUnicode[i] == ulUnicod) return -1;
	}

	return 0;

}	// end of int CheckUniRepeat()

void RemoveUniRepeat(std::vector<unsigned long>& vtInUni)
{
	if (vtInUni.size() == 0) return;

	std::vector<unsigned long> vtOutUni = vtInUni;
	vtInUni.clear();

	size_t  stlp = vtOutUni.size();
	vtInUni.push_back(vtOutUni[0]);
	for (size_t i = 1; i < stlp; i++) {			
		if (CheckUniRepeat(vtOutUni[i], vtInUni) == 0){
			vtInUni.push_back(vtOutUni[i]);
		}	
	}

}	// end of int RemoveUniRepeat()

int	HY_LoadCharTableFile(char* strTxtFile, std::vector<unsigned long>& vtUnicode)
{	
	return HY_TXTFileToUniArray(strTxtFile, vtUnicode);

}	// end of int HY_LoadCharTableFile

int HY_TXTFileToUniArray(char* pTxt, std::vector<unsigned long>& vtUni)
{
	vtUni.clear();

	unsigned long dwOffset = 0;
	unsigned long dwFileLenth = HY_FileLength(pTxt);
	if (dwFileLenth==0) return HY_ERR_FILE_NOEXIST;

	FILE* pTXT  = fopen(pTxt,"rb");
	char* pData = new char[dwFileLenth];
	memset(pData,0,dwFileLenth);
	fread(pData,dwFileLenth,1,pTXT);	
	fclose(pTXT);	
	
	int WORDSZ = sizeof(unsigned short);
	//验证文件头
	unsigned short	wordTmp1 = 0, wordTmp2 = 0;
	unsigned long	dwTmp1 = 0;
	memcpy(&wordTmp1, pData+dwOffset, WORDSZ);

	// 转换为大端格式
	if (wordTmp1!=0xFEFF){	
		while(dwOffset<dwFileLenth){				
			memcpy(&wordTmp1, pData+dwOffset,WORDSZ);
			wordTmp2 = hy_cdr_int16_to(wordTmp1); 
			memcpy(pData+dwOffset, &wordTmp2,WORDSZ);
			dwOffset+= WORDSZ;
		}
	}

	dwOffset=2;
	vtUni.clear();

	while(dwOffset<dwFileLenth){
		memcpy(&wordTmp1, pData+dwOffset, WORDSZ);
		dwOffset+=2;

		// 说明是4字节的unicode
		if(wordTmp1>=0xD800 && wordTmp1<=0xDBFF){
			dwTmp1 = 0x00000000;
			dwTmp1 = wordTmp1 & 0x3FF;
			dwTmp1 = dwTmp1<<10;

			memcpy(&wordTmp2, pData+dwOffset, WORDSZ);
			dwOffset += 2;
			wordTmp2 &= 0x3FF;
			
			dwTmp1 = (dwTmp1|wordTmp2)+0x10000;
			vtUni.push_back(dwTmp1);
		}
		else {
			// 0x0A 换行, 0x0D 回车
			if (wordTmp1==0x000D||wordTmp1==0x000A) continue;
			vtUni.push_back(wordTmp1);			
		}		
	}

	if (pData) delete[] pData;
	pData = NULL;

	RemoveUniRepeat(vtUni);

	return HY_NOERROR;

}	// end of int HY_TXTToUniArray()

int	 HY_UniArrayToTXTFile(std::vector<unsigned long>& vtUni,char* pTxt)
{
	int err = 0;
	FILE* pCreatFile = fopen(pTxt,"wb");
	if (pCreatFile == 0){
		_get_errno(&err);
		_set_errno(0);
		return err;	
	}

	unsigned short usUni=0xFEFF;
	fwrite(&usUni,2,1,pCreatFile);

	size_t sz = vtUni.size();
	for (size_t i=0; i<sz; i++){
		unsigned long ulUni = vtUni[i];
		if(ulUni <=0x0000FFFF)	{
			usUni = (unsigned short)ulUni;
			fwrite(&usUni,2,1,pCreatFile);
		}
		else {
			ulUni -= 0x00010000;

			usUni = (unsigned short)((ulUni>>10)+0xD800);
			fwrite(&usUni,2,1,pCreatFile);
			usUni = (unsigned short)((ulUni&0x000003FF)+0xDC00);
			fwrite(&usUni,2,1,pCreatFile);
		}
	}
	
	fflush(pCreatFile);
	fclose(pCreatFile);

	return HY_NOERROR;

}	// end of int HY_UniArrayToTXTFile()

int  HY_LoadPsNameFile(char* strNameFile, std::vector<std::string>& vtName)
{
	unsigned long			dwOffset = 0;
	unsigned long size = ::HY_FileExist(strNameFile);

	if (size == 0)								return HY_ERR_FILE_NOEXIST;
	vtName.clear();

	ifstream in(strNameFile);
	string line;
	if (in) {
		while (getline(in, line)) { // line中不包括每行的换行符
			vtName.push_back(line);
		}
	}
	else {
		return HY_ERR_FILE_OPEN;
	}

	return HY_NOERROR;

}	// end of int  HY_LoadPsNameFile()

void HY_LoadCodeFile(char* strcodeFile, std::vector<unsigned long>& vtUni)
{
	unsigned long			dwOffset = 0;
	unsigned long size = ::HY_FileExist(strcodeFile);
	if (size == 0)			return;

	vtUni.clear();

	ifstream in(strcodeFile);
	string line;
	if (in) {
		while (getline(in, line)) { // line中不包括每行的换行符

			if (line.length() == 0) continue;
			int uni = strtol(line.data(), NULL, 16);
			vtUni.push_back(uni);
		}
	}
	RemoveUniRepeat(vtUni);	

	return;
	

}	// end of int HY_LoadCodeFile()

bool  HY_MultiToWChar(WCHAR* pDest, int& iDestlen, char* pSrc )
{
	if ( pDest == NULL )			return FALSE;
	if ( pSrc == NULL )				return FALSE;

	WCHAR*	wpString			= NULL;
	int		iWStringLen			= 0;

	iWStringLen = MultiByteToWideChar(CP_ACP,0, pSrc, -1, NULL, 0 );
	if ( iWStringLen == 0 )			return FALSE;
	if ( iWStringLen > iDestlen )	return FALSE;

	wpString = new WCHAR[iWStringLen];
	ZeroMemory(wpString, sizeof(WCHAR) * iWStringLen);

	MultiByteToWideChar(CP_ACP, 0, pSrc, strlen(pSrc), wpString, iWStringLen);

	iDestlen = iWStringLen;
	CopyMemory(pDest,wpString, sizeof(WCHAR) * iDestlen );

	if ( wpString ) delete[] wpString;
	return TRUE;

}	// end of bool HY_MultiToWChar()

bool MulToWCharBL(char* pDest, int& iDestlen, const char* pSrc, int iSrclen)
{
	if (pDest == NULL) return false;
	if (pSrc == NULL)	return false;

	WCHAR	wpString[260] = { 0 };
	int		iWStringLen = 0;

	iWStringLen = MultiByteToWideChar(CP_ACP, 0, pSrc, -1, NULL, 0);
	if (iWStringLen == 0)					return false;
	if (iWStringLen >= MAX_PATH / 2)		return false;

	MultiByteToWideChar(CP_ACP, 0, pSrc, strlen(pSrc), wpString, iWStringLen);

	iWStringLen = wcslen(wpString);

	iDestlen = 2 * iWStringLen;

	// 大端模式
	short sTmp = 0;
	for (int x = 0; x < iWStringLen; x++)
	{
		sTmp = hy_cdr_int16_to(wpString[x]);
		memcpy(pDest + x * 2, &sTmp, 2);
	}
	return true;

}	// end of void MulToWCharBL()

unsigned long HY_AnsiStringToUnicode(char* pAnsiString)
{
	WCHAR	wChar[4];
	int		wCharlen = 4;

	HY_MultiToWChar(wChar, wCharlen, pAnsiString);

	return wChar[0];

}	// end of unsigned long HY_AnsiStringToUnicode()

std::string	HY_DLL_API	HY_Unicode_to_AnsiString(unsigned short Unicode)
{
	char	Dest[4] = {0};
	int		iDestlen = 4;

	HY_WCharTOMultiChar(Dest, iDestlen, Unicode);

	std::string str(Dest);
	return  str;

}	// end of std::string	HY_DLL_API	HY_Unicode_to_AnsiString()

bool HY_WCharTOMultiChar(char* pDest, int& iDestlen, unsigned short Unicode)
{
	WCHAR  wString[MAX_PATH] = {0};	
	CopyMemory((PBYTE)wString, &Unicode, 2);	

	//另外的方式转换
	iDestlen = WideCharToMultiByte(CP_ACP, NULL, wString, -1, NULL,0, NULL, 0);
	WideCharToMultiByte(CP_ACP, NULL, wString, -1, pDest, iDestlen, NULL, 0);

	return TRUE;

}	// end of bool HY_WCharTOMultiChar()

bool HY_WCharTOMultiCharEx(std::string  strOut, char* pSrc, int iSrclen)
{
	WCHAR  wString[MAX_PATH] = { 0 };
	CopyMemory((PBYTE)wString, pSrc, iSrclen);

	char  cString[MAX_PATH] = {0};
	strcpy_s(cString, MAX_PATH, strOut.c_str());

	//另外的方式转换
	int iDestlen = WideCharToMultiByte(CP_ACP, NULL, wString, -1, NULL, 0, NULL, 0);
	WideCharToMultiByte(CP_ACP, NULL, wString, -1, cString, iDestlen, NULL, 0);

	return TRUE;

}	// end of bool HY_WCharTOMultiChar()

std::string HY_trim(std::string const& str)   
{  
	if(str.empty())		return str;

	std::size_t firstScan = str.find_first_not_of(' ');
	std::size_t first     = firstScan == std::string::npos ? str.length() : firstScan;
	std::size_t last      = str.find_last_not_of(' ');
	return str.substr(first, last-first+1);

}  // end of std::string& HY_DLL_API trim(std::string &s)

int  HY_ReadTextLine(char* filename, std::vector<std::string>& szlineText)
{	
	std::locale::global(std::locale(""));

	std::ifstream file;
	file.open(filename,std::ios::in);
	if(!file.is_open())  
	{		 
		std::ios_base::iostate tt = file.rdstate();
		return HY_ERR_FILE_OPEN;
	}

	std::string strLine;  
	while(getline(file,strLine))
	{  
		if(strLine.empty())			continue;	
		szlineText.push_back(strLine);
	}

	std::locale::global(std::locale("C"));//还原全局区域设定

	return HY_NOERROR;

}	// end of bool HY_DLL_API  HY_ReadLineText()

void  HY_WriteLog(std::string SavePath,std::string& Text,bool bNew)
{
	int err = 0;
	FILE* pCreatFile = 0;	
	if (bNew) {
		pCreatFile = fopen(SavePath.c_str(), "wt");
	}
	else {
		pCreatFile = fopen(SavePath.c_str(), "at");
	}

	if (pCreatFile == 0) {
		_get_errno(&err);
		_set_errno(0);
		return;
	}
	fwrite(Text.c_str(),1, Text.length()+1, pCreatFile);
	fflush(pCreatFile);
	fclose(pCreatFile);

}	// end of void HY_WriteLog()

void HY_4ByteAlign(FILE* pF, unsigned long length)
{
	int iTail = 4 - length % 4;
	if (length % 4 > 0) {
		char c = 0;		
		fwrite(&c, iTail, 1, pF);
	}

}	// end of void HY_4ByteAlign()

void  HY_BrodMessage(UINT Msg, WPARAM wParam,LPARAM lParam)
{
	PostMessage(HWND_BROADCAST, Msg, wParam, lParam);

}	// end of void  HY_BrodMessage()