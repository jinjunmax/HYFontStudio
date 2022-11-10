#include "StdAfx.h"
#include "HYVmtx.h"

namespace HYFONTCODEC
{
	VMTX_LONGVRTMETRIC::VMTX_LONGVRTMETRIC()
	{
		advanceHeight	= 0;
		tsb				= 0;

	}	// end of VMTX_LONGVRTMETRIC::VMTX_LONGVRTMETRIC()

	VMTX_LONGVRTMETRIC::~VMTX_LONGVRTMETRIC()
	{

	}	// end of VMTX_LONGVRTMETRIC::~VMTX_LONGVRTMETRIC()

	void VMTX_LONGVRTMETRIC::SetDefault()
	{
		advanceHeight	= 0;
		tsb				= 0;

	}	// end of void VMTX_LONGVRTMETRIC::SetDefault()

	CHYVmtx::CHYVmtx(void)
	{

	}	// end of CHYVmtx::CHYVmtx(void)

	CHYVmtx::~CHYVmtx(void)
	{
		vtLongVrtmetric.clear();
		vtTopsidebearing.clear();

	}	// end of CHYVmtx::~CHYVmtx(void)

	void CHYVmtx::SetDefault()
	{
		vtLongVrtmetric.clear();
		vtTopsidebearing.clear();

	}	// end of void CHYVmtx::SetDefault()

	int CHYVmtx::GetAvgHeight()
	{
		int iLongVrtmetricSize = vtLongVrtmetric.size();
		if (iLongVrtmetricSize == 0) return -1;

		int iAvgHeight = 0;
		for (int i = 0; i < iLongVrtmetricSize; i++)
		{
			iAvgHeight += vtLongVrtmetric[i].advanceHeight;
		}

		int iTopsidebearing = vtTopsidebearing.size();
		for (int i = 0; i < iTopsidebearing; i++)
		{
			iAvgHeight += vtLongVrtmetric[iLongVrtmetricSize - 1].advanceHeight;
		}

		if (iAvgHeight == 0) return 0;
		return iAvgHeight / (iLongVrtmetricSize + iTopsidebearing) + 0.5f;

	}	// end of void CHYVmtx::GetAvgHeight()

	int	CHYVmtx::GetZeroHeight()
	{
		int iLongVrtmetricSize = vtLongVrtmetric.size();
		for (int i = 0; i < iLongVrtmetricSize; i++)
		{
			if( vtLongVrtmetric[i].advanceHeight == 0) return i;
		}

		return -1;

	}	// end of int CHYVmtx::GetZeroHeight()

	void CHYVmtx::GetVrtMetric(int gid, VMTX_LONGVRTMETRIC& tagVerMetrics)
	{
		tagVerMetrics.tsb = 0;
		tagVerMetrics.advanceHeight = 0;

		size_t  stVtmetricLng = vtLongVrtmetric.size();
		if (stVtmetricLng==0) return;

		if (gid < stVtmetricLng)
		{
			tagVerMetrics.tsb = vtLongVrtmetric[gid].tsb;
			tagVerMetrics.advanceHeight = vtLongVrtmetric[gid].advanceHeight;
		}
		else 
		{
			tagVerMetrics.tsb = vtTopsidebearing[gid-stVtmetricLng];
			tagVerMetrics.advanceHeight = vtLongVrtmetric[stVtmetricLng-1].advanceHeight;
		}	

	}	// end of VMTX_LONGVRTMETRIC&	GetVrtMetric()

}


