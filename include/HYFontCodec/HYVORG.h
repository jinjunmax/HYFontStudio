#pragma once

namespace HYFONTCODEC
{

	class HY_DLL_CLASS CHYVertOriginYMetrics 
	{	
		public:
			CHYVertOriginYMetrics();
			~CHYVertOriginYMetrics();
		public:
			unsigned short glyphIndex;
			short	vertOriginY;
	};

	class HY_DLL_CLASS CHYVORG
	{
		public:
			CHYVORG(void);
			~CHYVORG(void);
		public:
			unsigned short						majorVersion;
			unsigned short						minorVersion;
			short								defaultVertOriginY;
			unsigned short						numVertOriginYMetrics; 
			std::vector<CHYVertOriginYMetrics>	vtVertOriginYMetrics;
		public:
			void	SetDefault();

	};
}
