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
//char*				pFontName			Emoji�ֿ�����
//char*				pDirectory			����ͼ���Ŀ¼
/************************************************************************/
extern "C" __declspec(dllexport) int	__cdecl	EmojiToImage(char* pFontName, char* pDirectory);
extern "C" __declspec(dllexport) int	__cdecl	WoffToFONT(char* pWoffFile, char* pFontName);
extern "C" __declspec(dllexport) int	__cdecl	TTF2WebFont(char* pFontName, char* pzipFile);
extern "C" __declspec(dllexport) int	__cdecl	TTF2WebFont1(char* pFontName, char* pzipFile, char* strTextFile);

/************************************************************************/
// ExtractFONT                                                          
// char*			pFontName			ԭʼ�ֿ�����
// char*			pNewFontName		���ֿ�����
// unsigned long*	pUni				����ȡ�ַ�unicode����
// unsigned long	ulUniSize			unicode�����С
// int				CMAP				CMAP����ʽ 0����ԭ��CMAP�� 1�½�CMAP��
/************************************************************************/
extern "C" __declspec(dllexport) int	__cdecl	ExtractFONT(char* pFontName, char* pNewFontName, unsigned long* pUni, unsigned long ulUniSize,int CMAP);

/************************************************************************/
//ExtractFONT1
//char*				pFontName			ԭʼ�ֿ�����
//char*				pNewFontName		���ֿ�����
//char*				pTextFile			����ȡ�ַ��ı�
//int				CMAP				CMAP����ʽ 0����ԭ��CMAP�� 1�½�CMAP��
/************************************************************************/
extern "C" __declspec(dllexport) int	__cdecl	ExtractFONT1(char* pFontName, char* pNewFontName, char* pTextFile,int CMAP);

/************************************************************************/
//�ӽ�truetype�ֿ�ת����SVG
//char*				pFontName				truetype�ֿ�
//char*				pSVG					SVG�ֿ�
/************************************************************************/
extern "C" __declspec(dllexport) int	__cdecl	FontToSVG(char* pFontName, char* pSVG);

/************************************************************************/
//�ӽ�truetype�ֿ�ת����EOT
//char*				pFontName				truetype�ֿ�
//char*				pEotFile				EOT�ֿ�
/************************************************************************/
extern "C" __declspec(dllexport) int	__cdecl	FontToEOT(const char* pFontName, const char* pEotFile);

/************************************************************************/
//�����ֿ�ͨ��ָ���ļ�
//char*				sourcefile				Դ�ֿ�
//char*				targetfile				Ŀ���ֿ�
//int				flag					�ļ���ʶ 0 cha 1 txt 2 fea 	
//int				Thres					��ֵ
/************************************************************************/
extern "C" __declspec(dllexport) int __cdecl UpdateFont(char* sourcefile, char* targetfile, int flag, char* profile);

/************************************************************************/
//ͨ��ָ���ļ�����VMTX��
//char*				pFont				Դ�ֿ�
//char*				nwFnt				Ŀ���ֿ�
//char*				pVmtx				���滻��VMTX�ļ�
/************************************************************************/
extern "C" __declspec(dllexport) int	__cdecl ModifyVMTX1(char* pFont, char* nwFnt, char* pVmtx);

/************************************************************************/
//�ı�VMTX����tsbΪ�̶�ģʽADHΪ���ģʽ
//char*				pFont				Դ�ֿ�
//char*				nwFnt				Ŀ���ֿ�
//int				space					�̶����±�Եֵ
/************************************************************************/
extern "C" __declspec(dllexport) int	__cdecl ModifyVMTX2(char* pFont, char* nwFnt, int space);


/************************************************************************/
//����PSName����VMTX����tsbΪ�̶�ģʽADHΪ���ģʽ
//char*				pFont				Դ�ֿ�
//char*				nwFnt				Ŀ���ֿ�
//int				space					�̶����±�Եֵ
/************************************************************************/
extern "C" __declspec(dllexport) int	__cdecl ModifyVMTX3(char* pFont, char* pPSNameFile, char* nwFnt, int space);

/************************************************************************/
//ChaFileToHanyiFile	cha��ʽת�������Ǹ�ʽ���ı�
//char*				sourcefile			cha�ļ�
//char*				targetfile			�����ļ�
//int				iflag				0 ��� 1 �ֱ�
/************************************************************************/
extern "C" __declspec(dllexport) int	__cdecl ChaFileToHanyiFile(char* sourcefile, char* targetfile, int iflag);



extern "C" __declspec(dllexport) int	__cdecl ReplaceCAMP(char* pSource, char* pTatget, char* ReplaceXml);

#endif