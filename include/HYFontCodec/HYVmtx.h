#pragma once

#include "HYFontCodecDef.h"
#include <vector>

namespace HYFONTCODEC
{
	class HY_DLL_CLASS VMTX_LONGVRTMETRIC
	{
		public:
			VMTX_LONGVRTMETRIC();
			~VMTX_LONGVRTMETRIC();

			void SetDefault();
		public:
			unsigned short 	advanceHeight;
			short			tsb;
	};

	class HY_DLL_CLASS CHYVmtx
	{
		public:
			CHYVmtx(void);
			~CHYVmtx(void);

			void	SetDefault();
			int		GetAvgHeight();
			int		GetZeroHeight();
		public:
			std::vector<VMTX_LONGVRTMETRIC> vtLongVrtmetric;
			std::vector<short>				vtTopsidebearing;

		public:
			void	GetVrtMetric(int gid, VMTX_LONGVRTMETRIC& tagVerMetrics);


	};
};

