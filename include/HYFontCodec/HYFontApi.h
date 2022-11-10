#ifndef	HY_FONT_API
#define HY_FONT_API

// dwFlag 
/******************
#define TAG_OS2 	0x00000001
#define TAG_CMAP	0x00000002
#define TAG_GLYF	0x00000004
#define TAG_HEAD	0x00000008
#define TAG_HHEA	0x00000010
#define TAG_HMTX	0x00000020
#define TAG_LOCA	0x00000040
#define TAG_MAXP	0x00000080
#define TAG_NAME	0x00000100
#define TAG_POST	0x00000200
#define TAG_LTSH	0x00000400
#define TAG_VDMX	0x00000800
#define TAG_CVT		0x00001000
#define TAG_FPGM	0x00002000
#define TAG_GASP	0x00004000
#define TAG_HDMX	0x00008000
#define TAG_PREP	0x00010000
#define TAG_DSIG	0x00020000
#define TAG_VHEA	0x00040000
#define TAG_VMTX	0x00080000
#define TAG_GSUB	0x00100000
#define TAG_CFF		0x00200000
#define TAG_BASE	0x00400000
#define TAG_VORG	0x00800000
#define TAG_HANY	0x80000000
*******************/

/************************************************************************/
//EmojiToImage
//char*				pFontName			Emoji字库名称
//char*				pDirectory			保存图像的目录
/************************************************************************/
extern "C" __declspec(dllexport) int	__cdecl	EmojiToImage(char* pFontName, char* pDirectory);
extern "C" __declspec(dllexport) int	__cdecl	WoffToFONT(char* pWoffFile, char* pFontName);
extern "C" __declspec(dllexport) int	__cdecl	TTF2WebFont(char* pFontName, char* pzipFile);
extern "C" __declspec(dllexport) int	__cdecl	TTF2WebFont1(char* pFontName, char* pzipFile, char* strTextFile);

/************************************************************************/
// ExtractFONT                                                          
// char*			pFontName			原始字库名称
// char*			pNewFontName		新字库名称
// unsigned long*	pUni				待抽取字符unicode数组
// unsigned long	ulUniSize			unicode数组大小
// int				CMAP				CMAP处理方式 0保持原有CMAP表 1新建CMAP表
/************************************************************************/
extern "C" __declspec(dllexport) int	__cdecl	ExtractFONT(char* pFontName, char* pNewFontName, unsigned long* pUni, unsigned long ulUniSize,int CMAP);

/************************************************************************/
//ExtractFONT1
//char*				pFontName			原始字库名称
//char*				pNewFontName		新字库名称
//char*				pTextFile			待抽取字符文本
//int				CMAP				CMAP处理方式 0保持原有CMAP表 1新建CMAP表
/************************************************************************/
extern "C" __declspec(dllexport) int	__cdecl	ExtractFONT1(char* pFontName, char* pNewFontName, char* pTextFile,int CMAP);

/************************************************************************/
//从将truetype字库转换到SVG
//char*				pFontName				truetype字库
//char*				pSVG					SVG字库
/************************************************************************/
extern "C" __declspec(dllexport) int	__cdecl	FontToSVG(char* pFontName, char* pSVG);

/************************************************************************/
//从将truetype字库转换到EOT
//char*				pFontName				truetype字库
//char*				pEotFile				EOT字库
/************************************************************************/
extern "C" __declspec(dllexport) int	__cdecl	FontToEOT(const char* pFontName, const char* pEotFile);

/************************************************************************/
//更新字库通过指定文件
//char*				sourcefile				源字库
//char*				targetfile				目标字库
//int				flag					文件标识 0 cha 1 txt 2 fea 	
//int				Thres					阈值
/************************************************************************/
extern "C" __declspec(dllexport) int __cdecl UpdateFont(char* sourcefile, char* targetfile, int flag, char* profile);

/************************************************************************/
//通过指定文件更改VMTX表
//char*				pFont				源字库
//char*				nwFnt				目标字库
//char*				pVmtx				待替换的VMTX文件
/************************************************************************/
extern "C" __declspec(dllexport) int	__cdecl ModifyVMTX1(char* pFont, char* nwFnt, char* pVmtx);

/************************************************************************/
//改变VMTX表中tsb为固定模式ADH为变高模式
//char*				pFont				源字库
//char*				nwFnt				目标字库
//int				space					固定上下边缘值
/************************************************************************/
extern "C" __declspec(dllexport) int	__cdecl ModifyVMTX2(char* pFont, char* nwFnt, int space);


/************************************************************************/
//根据PSName更新VMTX表中tsb为固定模式ADH为变高模式
//char*				pFont				源字库
//char*				nwFnt				目标字库
//int				space					固定上下边缘值
/************************************************************************/
extern "C" __declspec(dllexport) int	__cdecl ModifyVMTX3(char* pFont, char* pPSNameFile, char* nwFnt, int space);

/************************************************************************/
//ChaFileToHanyiFile	cha格式转换到汉仪格式的文本
//char*				sourcefile			cha文件
//char*				targetfile			汉仪文件
//int				iflag				0 码表 1 字表
/************************************************************************/
extern "C" __declspec(dllexport) int	__cdecl ChaFileToHanyiFile(char* sourcefile, char* targetfile, int iflag);



extern "C" __declspec(dllexport) int	__cdecl ReplaceCAMP(char* pSource, char* pTatget, char* ReplaceXml);

#endif