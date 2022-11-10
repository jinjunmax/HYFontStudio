#ifndef HYZIP
#define	HYZIP

#ifdef HYZIPCODEC_EXPORTS
#define HY_ZIP_DLL __declspec(dllexport)
#else
#define HY_ZIP_DLL __declspec(dllimport)
#endif

#define WRITEBUFFERSIZE (16384)
#define MAXFILENAME (256)

int HY_ZIP_DLL HY_ZipFiles(std::string& zipFileName, std::vector<std::string> files, bool bAppendFile = false, int nCompressLevel = 0);



#endif // !HYZIP


