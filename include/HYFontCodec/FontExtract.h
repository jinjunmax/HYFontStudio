#pragma once

#include <vector>
#include <map>
#include "HYFontCodec.h"

namespace HYFONTCODEC
{
	class HY_DLL_CLASS chaRecordItem
	{
		public:
			chaRecordItem()
			{
				iGID = -1;
				URWNum = -1;
				UNINum = -1;
				AdvanceWith = 0;
				PSName = "";
			}
			~chaRecordItem()
			{
				iGID = -1;
				URWNum = -1;
				UNINum = -1;
				AdvanceWith = 0;
				PSName = "";
			}
		public:
			int		iGID;
			int		URWNum;
			int		UNINum;
			int		AdvanceWith;
			string	PSName;
	};

	class HY_DLL_CLASS chaRecord
	{
	public:
		chaRecord()	{
			vesion = 0;
		}
		~chaRecord(){}

		float				vesion;
		std::vector<chaRecordItem>			m_vtchaList;
	};

	class HY_DLL_CLASS GlyfData
	{
	public:
		GlyfData()	{
			pDataLen = 0;
			usAdvWidth = 0;
			sLsb = 0;
			usAdvHeight = 0;
			sTsb = 0;
			xMin = 0;
			yMin = 0;
			xMax = 0;
			yMax = 0;
			CnturNum = 0;			
		}
		~GlyfData()	{}

		std::vector<char>				vtData;
		int								pDataLen;
		std::vector<unsigned long>		vtUnicode;
		unsigned short					usAdvWidth;
		short							sLsb;
		unsigned short					usAdvHeight;
		short							sTsb;
		std::string						strName;
		short 							xMin;
		short 							yMin;
		short 							xMax;
		short 							yMax;	
		short							CnturNum;
	};

	class HY_DLL_CLASS CFontExtract
	{	
		struct GroupRecord	{
			unsigned long		startCode;
			unsigned long		endCode;
			int 				startGID;
			bool				contiguous;
		};		

		std::map<int,int>					m_mpModifiedVMTX;
		chaRecord							m_chaRecord;

		class CodeMapItem
		{
		public:
			int GID;
			std::vector<unsigned long>  vtUnicode;
		};	

	public:
		CFontExtract(void);
		~CFontExtract(void);
		
		int				CheckNumberRepeat(unsigned long ulUnicod, std::vector<unsigned long>& vtUnicode);
		int				PrepareExtractFont(const char* strTTFFile, HYFONTCODEC::CHYFontCodec& FontDecode);

		int				GetSubset(const char* strTTFFile, const char* strNewTTFFile, const char* strTxtFile, bool bRovlp=false);
		char*			FilterGlyphs(CHYFontCodec& Decode, std::vector<unsigned long>& vtInUni, std::vector<CHYCodeMapItem>& vtCodeMapItem, unsigned long& ulGlyphsLen);
		int				IsCompositeGlyph(FILE* f,std::vector<int>& vtGID);
		int				FilterSVG(CHYFontCodec& Decode, std::vector<CHYCodeMapItem>& vtCodeMapItem);
		int				FilterColors(CHYFontCodec& Decode, std::vector<CHYCodeMapItem>& vtCodeMapItem);
		int				FilterEmbeddedBitmap(CHYFontCodec& Decode, std::vector<CHYCodeMapItem>& vtCodeMapItem);
		void			RemoveEmbeddedBitmap(CHYCBLC& BLC, int GID);

		/************************************************************************/
		//ExtractFont
		//char*				pFontName			ԭʼ�ֿ�����
		//char*				pNewFontName		���ֿ�����
		//char*				pTextFile			����ȡ�ַ��ı�
		//int				flag				����ʽ 0 �ı�Ϊ�ֱ�����ԭ��CMAP�� 1 �ı�Ϊ�ֱ��½�CMAP�� 2 �ı�Ϊpsname���½�cmap��
		/************************************************************************/
		int				ExtractFont(const char* strTTFFile, const char* strNewTTFFile, const char* strTxtFile,int flag);

		/************************************************************************/
		//ExtractFont
		//char*				pFontName			ԭʼ�ֿ�����
		//char*				pNewFontName		���ֿ�����		
		//int				CMAP				CMAP����ʽ 0����ԭ��CMAP�� 1�½�CMAP��
		/************************************************************************/
		int				ExtractFont(const char* strTTFFile, const char* strNewTTFFile,int CMAP);

		/************************************************************************/
		//ChaFileToHanyiFile	cha��ʽת�������Ǹ�ʽ���ı�(�ֱ����)
		//char*				sourcefile			cha�ļ�
		//char*				targetfile			�����ļ�
		//int				iflag				0 ��� 1 �ֱ�
		/************************************************************************/
		int				ChaFileToHanyiFile(char* sourcefile,char* targetfile, int iflag);
		int				ReadChaFile(char* filename,chaRecord& Records);
		int				WriteChaFile(char* filename,chaRecord& Records);
		int				ChaFileToHanyiCodeTxt(char* sourcefile,char* targetfile);
		int				ChaFileToHanyiCharTxt(char* sourcefile,char* targetfile);

		/************************************************************************/
		//�����ڲ���λӳ�����
		//CHYFontCodec		FontEncode				�ֿ������
		//chaRecord			chaRecorder				cha�ļ�����
		/************************************************************************/
		int				MakeHYCodeMap(CHYFontCodec& FontEncode,chaRecord& chaRecorder);
		/************************************************************************/
		//�����ڲ���λӳ�����
		//CHYFontCodec		FontEncode				�ֿ������
		//chaRecord			chaRecorder				unicode����
		/************************************************************************/
		int				MakeHYCodeMap(CHYFontCodec& FontEncode, std::vector<unsigned long>& vtUnicode);		

		/************************************************************************/
		//��ȡCha�ļ���chaList
		//char*				chafile					cha�ļ�
		//chaRecord&								cha
		/************************************************************************/
		int				readChaFile(char* chafile, chaRecord&	cha);

		char* 			Extrace(HYFONTCODEC::CHYFontCodec&  HYFontCode, std::vector<unsigned long>& vtUnicode, int& GlyphDataLen);
		char*			Extrace1(HYFONTCODEC::CHYFontCodec&  HYFontCode, std::vector<unsigned long>& vtUnicode, int& GlyphDataLen,CHYCodeMap& HYCodeMap);
		void			ReCountHVMTX(HYFONTCODEC::CHYFontCodec&  HYFontCode, std::vector<HMTX_LONGHORMERTRIC>& vtExtraceHMTX, std::vector<VMTX_LONGVRTMETRIC>&	vtExtraceVMTX);
		void			ReCountCOLOR(HYFONTCODEC::CHYFontCodec&  HYFontCode, std::vector<unsigned long>& vtExtraceGid, std::vector<unsigned long>& vtNewGID, std::vector<CLayerRecord>&  vtnewLayerRecord);
		char*			Extrace2(HYFONTCODEC::CHYFontCodec&  HYFontCode, std::vector<string>& vtPSName, int& GlyphDataLen, CHYCodeMap& HYCodeMap);

		int				ExtractCAMP(char* pFontName, char* sourcechar, char* targetXml);
		void			CMapTOXml(std::vector<CodeMapItem>& CMAPCodeMap, char* targetXml);
		int				ReplaceCAMP(char* pSource, char* pTatget, char* ReplaceXml);
		int				UpdateCMAP(std::vector<CodeMapItem>& CMAPCodeMap, char* ReplaceXml);		
	private:
		//	getFlag		0 unicode 1 glyph name
		int				PrepareMergeData(char* fontfile,HYFONTCODEC::CHYFontCodec& FntDecode,std::vector<GlyfData>& vtMrg1GlyfData);
		void			Merge(HYFONTCODEC::CHYFontCodec& Fnt1Decode,std::vector<GlyfData>& vtMrg1GlyfData, HYFONTCODEC::CHYFontCodec& Fnt2Decode,std::vector<GlyfData>& vtMrg2GlyfData,char* GlyfTableData,int& iGlyfTableDataLen,int getFlag);
		int				FindMergeGlyhDataIndex(std::vector<GlyfData>& vtMrgGlyfData, unsigned long Unicode);
		int				FindMergeGlyhDataIndex(std::vector<GlyfData>& vtMrgGlyfData, std::string& glyphName);

	public:
		// ��ȡ��Ҫ����ADHֵ�ģ�ӳ���ļ�
		int				LoadModifyVMTXFile(char* pFile);
		// ���ݵ����VMTXӳ���ļ��� �޸��ֿ��е�VMTX����
		int				ModifyVMTX1(char* pFont, char* nwFnt, char* pVmtx);
		//�ı�VMTX����tsbΪ�̶�ģʽADHΪ���ģʽ
		int				ModifyVMTX2(char* pFont, char* nwFnt, int space);
		//����PSName�ı�VMTX����tsbΪ�̶�ģʽADHΪ���ģʽ
		int				ModifyVMTX3(char* pFont, char* pPsNameFile,char* nwFnt, int space);
		// ��ȡPsNameFile
		int				ReadPsNameFile(char* pPsName, std::vector<std::string>& vtPsName);		
		
	public:
		std::vector<unsigned long> m_vtUnicode;
		std::vector<std::string> m_vtPsName;
	private:		
		BOOL	CheckGid(std::vector<int>& vtgid, int iGid);
		BOOL	CheckGid(std::vector<CHYCodeMapItem>& vtMpItem, int iGid);
		BOOL	CheckUnicode(std::vector<unsigned long>& m_vtunicode, unsigned long unicode);
		void	WriteTableEntry(FILE* pFile, CHYTableDirectory&	HYTbDirectory);	
		BOOL	FindItem(unsigned long ulItem, std::vector<unsigned long> vtUL);

		int		NextGroup(int ix, int eov, GroupRecord& group,CHYCodeMap& CodeMap);
		int		EncodeCmapFmt4(CMAP_TABLE_ENTRY& entry,CHYCodeMap& CodeMap);
		int		EncodeCmapFmt12(CMAP_TABLE_ENTRY& entry,CHYCodeMap& CodeMap);
		int		EncodeCmapFmt14(CMAP_TABLE_ENTRY& entry,CHYCodeMap& CodeMap);

		BOOL	HitTable(std::vector<ULONG>& vtTableFlag, ULONG ulTableFlag);

		void	CountVertTable(HYFONTCODEC::CHYFontCodec& FontCodec, std::vector<GlyfData>& Mrg1GlyfData);

		//�����������壬ֻ�������б䶯�ı���б���
		int		BulidFont(HYFONTCODEC::CHYFontCodec& FontCodec,char* pGlpyData,int GlyphDataLen, const char* strNewTTFFile, CHYCodeMap& HYCodeMap);
		int		BulidFont(HYFONTCODEC::CHYFontCodec& FontCodec,char* pGlpyData,int GlyphDataLen,const char* strNewTTFFile, std::vector<ULONG>& vtTableFlag,CHYCodeMap& HYCodeMap);
		void	BulidLOCAL(HYFONTCODEC::CHYFontCodec& FontCodec,CHYTableEntry& HYEntry,FILE* pCreatFile);
		void	BulidGlyph(HYFONTCODEC::CHYFontCodec& FontCodec,CHYTableEntry& HYEntry, FILE* pCreatFile,char* pGlpyData,int GlyphDataLen);
		void	BulidHead(HYFONTCODEC::CHYFontCodec& FontCodec,CHYTableEntry& HYEntry, FILE* pCreatFile);
		void	BulidMAXP(HYFONTCODEC::CHYFontCodec& FontCodec,CHYTableEntry& HYEntry,FILE* pCreatFile);

		void	MakeCMAP(HYFONTCODEC::CHYFontCodec& FontCodec, CHYCodeMap& HYCodeMap);
		void	BulidCMAP(HYFONTCODEC::CHYFontCodec& FontCodec,CHYTableEntry& HYEntry,FILE* pCreatFile, CHYCodeMap& HYCodeMap);
		void	BulidHHEA(HYFONTCODEC::CHYFontCodec& FontCodec,CHYTableEntry& HYEntry,FILE* pCreatFile);
		void	BulidHMTX(HYFONTCODEC::CHYFontCodec& FontCodec,CHYTableEntry& HYEntry,FILE* pCreatFile);
		void	BulidVHEA(HYFONTCODEC::CHYFontCodec& FontCodec, CHYTableEntry& HYEntry, FILE* pCreatFile);
		void	BulidVMTX(HYFONTCODEC::CHYFontCodec& FontCodec, CHYTableEntry& HYEntry,FILE* pCreatFile);
		// ֻ��Ҫ֧��V3.0
		void	BulidPOST(HYFONTCODEC::CHYFontCodec& FontCodec, CHYTableEntry& HYEntry,FILE* pCreatFile);
		void	BulidCOLR(HYFONTCODEC::CHYFontCodec& FontCodec, CHYTableEntry& HYEntry,FILE* pCreatFile);
		void	BulidSVG(HYFONTCODEC::CHYFontCodec& FontCodec, CHYTableEntry& HYEntry, FILE* pCreatFile);

		int		FindVertMinTop(HYFONTCODEC::CHYFontCodec& FontCodec);
		int		FindVertMinBottom(HYFONTCODEC::CHYFontCodec& FontCodec);
		unsigned short	GetAdvancMaxHeight(HYFONTCODEC::CHYFontCodec& FontCodec);

	};
};
