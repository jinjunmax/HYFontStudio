#pragma once
namespace HYFONTCODEC
{
	class HY_DLL_CLASS CMaskData
	{
		public:
			CMaskData();
			~CMaskData();

			void setdefault();
		public:
			unsigned int		maskLength;
			unsigned short		reserved;
			unsigned short		blendingMode;
			short				originOffsetX;
			short				originOffsetY;
			char				graphicType[4];
			std::vector<unsigned char>	vtData;
	};

	class HY_DLL_CLASS CStrikesMask
	{
		public:
			CStrikesMask();
			~CStrikesMask();

			void setdefault();

		public:
			unsigned int			maskOffset;
			char					graphicType[4];
			std::vector<CMaskData>	vtMaskData;
	};


	class HY_DLL_CLASS CStrikesDatarecord
	{
		public:
			CStrikesDatarecord(void);
			~CStrikesDatarecord(void);

			void setdefault();
		public:			
			unsigned short					originOffsetX;
			unsigned short					originOffsetY;
			char							graphicType[4];
			std::vector<unsigned char>		vtdata;
			std::vector<CStrikesMask>		vtMask;
			unsigned long					gid;
	};

	class HY_DLL_CLASS CSbixStrikes
	{
		public:
			CSbixStrikes(void);
			~CSbixStrikes(void);

			void setdefault();
		public:
			unsigned short						ppem;
			unsigned short						resolution;
			std::vector<unsigned long>			vtglyphDataOffset;
			std::vector<CStrikesDatarecord>		vtStrikesDatarecord;
	};

	class HY_DLL_CLASS CHYsbix
	{
		public:
			CHYsbix(void);
			~CHYsbix(void);
		public:
			void setdefault();
		public:
			unsigned short				version;
			unsigned short				flags;
			unsigned long				numStrikes;
			std::vector<unsigned long>  vtstrikeOffset;
			std::vector<CSbixStrikes>   vtSbixStrikes;

	};
}
