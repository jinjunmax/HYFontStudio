#include "StdAfx.h"
#include "HYImageToFont.h"
#include <math.h>

#define		N           1000
#define		cosA        -0.7   //标记角度位
#define		bcdis       2       //bezier拟合时判断是否加点（数值越大加点的机会越小）
namespace HYIMGTOFONT
{
	CHYImageToFont::CHYImageToFont(void)
	{
	}

	CHYImageToFont::~CHYImageToFont(void)
	{
	}

	void CHYImageToFont::findfile(char * lpPath, std::vector<string>& vtFileNames)
	{  
		char szFind[MAX_PATH]; 
		WIN32_FIND_DATA FindFileData; 
		strcpy(szFind,lpPath); 
		strcat(szFind,"\\*.*"); 
		HANDLE hFind=::FindFirstFile(szFind,&FindFileData); 
		if(INVALID_HANDLE_VALUE == hFind)  return; 
		while(true) 
		{ 
			if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
			{ 
				if(FindFileData.cFileName[0]!='.') 
				{ 
					/*
					char szFile[MAX_PATH]; 
					strcpy(szFile,lpPath); 
					strcat(szFile,"\\"); 
					strcat(szFile,(char* )(FindFileData.cFileName)); 
					find(szFile,fileList); 
					*/
					continue;
				} 
			} 
			else 
			{ 
				char szFile[MAX_PATH]; 
				strcpy(szFile,lpPath); 
				strcat(szFile,"\\"); 
				strcat(szFile,(char* )(FindFileData.cFileName)); 
				vtFileNames.push_back(szFile); 
			} 
			if(!FindNextFile(hFind,&FindFileData))  break; 
		} 
		FindClose(hFind); 

	}	// end of void findfile()

	BOOL CHYImageToFont::ImageToGlyph(string strImage, HYFONTCODEC::CHYFontCodec& FntEncode)
	{		
		IplImage* clrImage = cvLoadImage(strImage.c_str(), CV_LOAD_IMAGE_GRAYSCALE);
		if (NULL == clrImage)	FALSE;		
		if (clrImage)
		{
			CHYGlyph		glyph;
			int iImgWD = clrImage->widthStep;
			int iImgHT = clrImage->height;

			char* pMtrxData = clrImage->imageData;
			int iMonoImgDataLen = iImgWD*iImgHT;
			char* pMonoImgData = new char[iMonoImgDataLen];
			ZeroMemory(pMonoImgData,iMonoImgDataLen);

			MonoImgData(pMtrxData,pMonoImgData,160,iImgWD,iImgHT);

			int		BaziderN = 2;
			int		ErrorItem = 1;
			//int		cutflag = 0;
			double	avrdis = 3;
			double	DisWidth = 8;

			DotMatrixToHanYi(pMonoImgData,iImgWD,iImgHT,glyph,BaziderN,ErrorItem,avrdis);		
			FntEncode.m_vtHYGlyphs.push_back(glyph);

			if (pMonoImgData) delete[] pMonoImgData;
			pMonoImgData  = NULL;

			if (pMtrxData) delete[] pMtrxData;
			pMtrxData = NULL;
		}		

		return TRUE;

	}	// end of BOOL	CHYImageToFont::ImageToGlyph()

	// 在最边缘的一个像素，全部设置成白色，防止扫描越界
	void CHYImageToFont::MonoImgData(char* GrayData,char* MonoImgData, int iGrayThres ,int iWD, int iHT)
	{
		for(int y=0; y<iHT; y++)
		{
			for(int x=0; x<iWD; x++)
			{
				int iOffset = y*iWD+x;
				if(x == 0 || y == 0 || y == (iHT - 1) || x == (iWD - 1))
				{
					MonoImgData[iOffset] = (char)255;				
				}
				else
				{	
					if((BYTE)GrayData[iOffset] < iGrayThres)
					{
						MonoImgData[iOffset] = (char)0;
					}
					else
					{															
						MonoImgData[iOffset] = (char)255;						
					}
				}
			}
		}

	}	// end of void CHYImageToFont::MonoImgData()

	void CHYImageToFont::DotMatrixToHanYi(char*	pMtrxData, int iWidth, int iHeight,HYFONTCODEC::CHYGlyph& HyGrph,int iBezierN,double dbErrorItem,double avrdis)
	{
		if (pMtrxData == NULL)  return;		

		double dbDelta = 0.5;
		if (iWidth > 1000) dbDelta = 1.0;		
		if (iWidth>2000) dbDelta = 1.5;
		if (iWidth>2500) dbDelta = 2;
		if (iWidth>3000) dbDelta = 2.5;
		if (iWidth>4000) dbDelta = 3;
		if (iWidth>5000) dbDelta = 3.5;
		if (iWidth>6000) dbDelta = 4;

		double alphad2	= 2*dbDelta;
		double alphad3	= 3*dbDelta;
		bool Leftcut	= false;
		bool Rightcut	= false;
		bool Upcut		= false;
		bool Belowcut	= false;
		bool repeat		= false;

		std::vector<CHYContour>&	vtContour = HyGrph.vtContour;
		std::vector<CHYContour>		ContourItemLst;
		std::vector<CHYContour>		ImageCutContourItemLst;
		std::vector<CHYContour>		p_ContourItemLst;
		std::vector<CHYContour>		CleanContourItemLst;
		std::vector<CHYContour>		pHy;

		double m_value = dbErrorItem;

		GetContour(pMtrxData,iWidth,iHeight,ContourItemLst,ImageCutContourItemLst,iBezierN);		
		//cutflag = ImageCut(ContourItemLst,ImageCutContourItemLst,iWidth,iHeight,dbDelta,Leftcut,Rightcut,Upcut,Belowcut,avrdis);
		CHYContour				ContourItem;
		std::vector<CHYPoint>	VectorPointList;
		CHYContour				AContourItem;
		std::vector<CHYPoint>   ClearPointList;

		int iCntrItemLstNum = ContourItemLst.size();		
		for (int i = 0; i<iCntrItemLstNum; i++)
		{			
			CHYContour& temp_ContourItem = ContourItemLst[i];
			std::vector<CHYPoint>& temp_PointList = temp_ContourItem.vtHYPoints;

			VectorPointList.clear();
			Vectorize(temp_PointList, VectorPointList,iWidth,dbDelta);
  			if(VectorPointList.size() <= 2)
			{
				continue;
			}

			if(AVRDisofPoint(VectorPointList) < avrdis )//&& OrientationOfPolygon(&VectorPointList)
			{
				continue;
			}
			repeat = false;
			int ip_CnturItemLst = (int)p_ContourItemLst.size();
			for(int j = 0;j<ip_CnturItemLst;j++)
			{
				CHYContour& temp_ContourItemA = p_ContourItemLst[j];
				std::vector<CHYPoint>& temp_PointListA = temp_ContourItemA.vtHYPoints;

				if(ContourOnContour(VectorPointList,temp_PointListA))
				{
					repeat = true;
				}
			}
			if(repeat)
			{
				continue;
			}
			ClearPointList.clear();
			Clearnoise(VectorPointList,ClearPointList,alphad2,alphad3);
			if(ClearPointList.size() <= 2)
			{
				continue;
			}

			AContourItem.vtHYPoints = ClearPointList;
			ClearPointList.clear();

			p_ContourItemLst.push_back(AContourItem);	
		}

		int toleft = 10000;
		int tobelow = 10000;
		int toright = -10000;
		int toabove = -10000;
		int iContour[4] = {0};

		int p_ContourItemNum = (int)p_ContourItemLst.size();
		for(int i=0; i<p_ContourItemNum; i++)
		{
			const CHYContour& temp_ContourItem = p_ContourItemLst[i];
			const std::vector<CHYPoint>& temp_PointList = temp_ContourItem.vtHYPoints;

			int temp_PointNum = (int)temp_PointList.size();
			for(int j=0; j<temp_PointNum; j++)
			{
				const CHYPoint& temp_HYPoint1 = temp_PointList[j];				
				if(temp_HYPoint1.x < toleft)
				{
					toleft = temp_HYPoint1.x;
					iContour[0] = i;
				}
				if(temp_HYPoint1.x > toright)
				{
					toright = temp_HYPoint1.x;
					iContour[1] = i;
				}
				if(temp_HYPoint1.y < tobelow)
				{
					tobelow = temp_HYPoint1.y;
					iContour[2] = i;
				}
				if(temp_HYPoint1.y > toabove)
				{
					toabove = temp_HYPoint1.y;
					iContour[3] = i;
				}
			}
		}
	
		for(int i=0; i<p_ContourItemNum; i++)
		{
			const CHYContour& temp_ContourItem = p_ContourItemLst[i];
			const std::vector<CHYPoint>& temp_PointList = temp_ContourItem.vtHYPoints;

			if((Leftcut && i == iContour[0]) || (Rightcut && i == iContour[1]) || 
				(Belowcut && i == iContour[2]) || (Upcut && i == iContour[3]))
			{
				int point[4] = {10000,-10000,10000,-10000};

				int itemp_PointNum = temp_PointList.size();
				for(int j=0; j<itemp_PointNum; j++)
				{
					const CHYPoint& temp_HYPoint1 = temp_PointList[j];					
					if(temp_HYPoint1.x < point[0])
					{
						point[0] = temp_HYPoint1.x;
					}
					if(temp_HYPoint1.x > point[1])
					{
						point[1] = temp_HYPoint1.x;
					}
					if(temp_HYPoint1.y < point[2])
					{
						point[2] = temp_HYPoint1.y;
					}
					if(temp_HYPoint1.y > point[3])
					{
						point[3] = temp_HYPoint1.y;
					}
				}
				if(Leftcut || Rightcut)
				{
					if(abs(point[1] - point[0]) <=10 || (abs(point[1] - point[0]) >= (iWidth - 3) && abs(point[3] - point [2]) <= (iWidth/25)) 
						||(abs(point[3]-point[2]) >= (iWidth - 3) && abs(point[1] - point[0]) <= (iWidth/25)))
					{
						continue;
					}
				}
				if(Belowcut || Upcut)
				{
					if(abs(point[3] - point[2]) <=10 || (abs(point[3] - point[2]) >= (iWidth - 3) && abs(point[1] - point[0]) <= (iWidth/25)) 
						||(abs(point[1]-point[0]) >= (iWidth - 3) && abs(point[3] - point[2]) <= (iWidth/25)))
					{
						continue;
					}
				}
				AContourItem.vtHYPoints = temp_PointList;
				CleanContourItemLst.push_back(AContourItem);
				continue;
			}
			AContourItem.vtHYPoints = temp_PointList;
			CleanContourItemLst.push_back(AContourItem);	
		}

		FilterContourList(CleanContourItemLst);
		BezierContour(CleanContourItemLst,pHy,m_value,iBezierN,dbDelta);
		toleft = 1000;
		tobelow = 1000;
		toright = -1000;
		toabove = -1000;

		int iPHYNUm = (int)pHy.size();
		for(int i = 0 ; i < iPHYNUm; i++)
		{
			CHYContour temp_ContourItem = pHy[i];
			std::vector<CHYPoint>& temp_PointList = temp_ContourItem.vtHYPoints;

			int itemp_PointListNum = (int)temp_PointList.size();
			for(int j=0; j<itemp_PointListNum; j++)
			{
				CHYPoint& temp_HYPoint1 = temp_PointList[j];				
				if(temp_HYPoint1.x < toleft)
				{
					toleft = temp_HYPoint1.x;
				}
				if(temp_HYPoint1.x > toright)
				{
					toright = temp_HYPoint1.x;
				}
				if(temp_HYPoint1.y < tobelow)
				{
					tobelow = temp_HYPoint1.y;
				}
				if(temp_HYPoint1.y > toabove)
				{
					toabove = temp_HYPoint1.y;
				}
			}
		}

		CHYContour				my_pHy_ContourItem;		
		std::vector<CHYPoint>	my_pHy_PointList;
		int x = 0,y = 0;
		int pNum = 0;

		for(int i=0 ; i<iPHYNUm; i++)
		{
			const CHYContour& temp_ContourItem = pHy[i];
			const std::vector<CHYPoint>& temp_PointList = temp_ContourItem.vtHYPoints;

			int itemp_PointListNum = (int)temp_PointList.size();
			pNum += itemp_PointListNum-1;
			for(int j=0; j<itemp_PointListNum-1; j++)
			{
				const CHYPoint& temp_HYPoint1 = temp_PointList[j];				
				x += ::HY_RealRount(temp_HYPoint1.x);
				y += ::HY_RealRount(temp_HYPoint1.y);
			}
		}
		if(pNum != 0)
		{x = x/pNum;
		y = y/pNum;}
		x = (abs(toright + toleft)/2 * 3 + x * 2)/5;
		y = (abs(toabove + tobelow)/2 * 3 + y * 2)/5;

		CHYPoint   HYPt;
		for(int i = 0; i<iPHYNUm; i++)
		{
			const CHYContour&				temp_ContourItem = pHy[i];
			const std::vector<CHYPoint>&	temp_PointList = temp_ContourItem.vtHYPoints;

			int itemp_PointListNum = (int)temp_PointList.size();
			if(itemp_PointListNum<= 2)
			{
				continue;
			}

			for(int j = 0; j <itemp_PointListNum; j++)
			{
				const CHYPoint& temp_HYPoint1 = temp_PointList[j];				
				HYPt.x = ::HY_RealRount(temp_HYPoint1.x);
				HYPt.y = ::HY_RealRount(temp_HYPoint1.y);
				HYPt.flag = temp_HYPoint1.flag;				
				my_pHy_PointList.push_back(HYPt);
			}

			my_pHy_ContourItem.vtHYPoints = my_pHy_PointList;
			my_pHy_PointList.clear();
			vtContour.push_back(my_pHy_ContourItem);
		}

	}	// end of int CHYImageToFont::DotMatrixToHanYi()

	void CHYImageToFont::FilterContourList(std::vector<CHYContour>& vtCntuLst)
	{
		for (size_t i = 0; i < vtCntuLst.size(); )
		{
			CHYContour& Cntu1 = vtCntuLst[i];
			FilterPointList(Cntu1.vtHYPoints);

			if (Cntu1.vtHYPoints.size() == 0)
				vtCntuLst.erase(vtCntuLst.begin() + i);
			else
				i++;
			
			
		}

	}	// end of void CHYImageToFont::FilterContourList()

	void CHYImageToFont::FilterPointList(std::vector<CHYPoint>& vtPntLst)	
	{
		for (size_t i = 1; i < vtPntLst.size(); i++)
		{
			CHYPoint& Pt1 = vtPntLst[i];
			for (size_t j = i+1; j < vtPntLst.size();)
			{
				CHYPoint& Pt2 = vtPntLst[j];
				if (Pt2.x == Pt1.x && Pt2.y == Pt1.y&& Pt2.flag == Pt1.flag)
				{

					vtPntLst.erase(vtPntLst.begin() + j);
				}
				else
					j++;
			}
		}

		if (vtPntLst.size() < 3) vtPntLst.clear();

	}	// end of void CHYImageToFont::FilterPointList()

	void CHYImageToFont::BezierContour(std::vector<CHYContour>& N_ContourItemLst,std::vector<CHYContour>& pHy,double dbvalue,int iBezierN,double dbDelta)
	{
		double					kk[1000]={0};
		//double					k[1000];
		int						fdflag;
		double					k0=0;
		double					k1=0;
		int						fsize=0;
		CHYContour				pHy_ContourItem;
		CHYPoint				pHy_HYPoint;
		std::vector<CHYPoint>	pHy_PointList;		
		std::vector<CHYPoint>	bezier_PointList;
		CHYPoint				bezier_HYPoint;
		CHYPoint				toPoint;

		std::vector<CHYPoint>	bPointList;

		size_t stN_ContourItemNum = N_ContourItemLst.size();
		for (size_t i=0; i<stN_ContourItemNum; i++)
		{
			CHYContour& temp_ContourItem = N_ContourItemLst[i];
			std::vector<CHYPoint>& temp_PointList = temp_ContourItem.vtHYPoints;

			size_t sttemp_PointLstNum = temp_PointList.size();
			if(sttemp_PointLstNum <= 2)
			{
				continue;
			}

			CHYPoint& temp_HYPoint1 = temp_PointList[0];			
			temp_HYPoint1.flag = POINT_FLG_ANCHOR;

			pHy_HYPoint = temp_HYPoint1;
			pHy_PointList.push_back(pHy_HYPoint);

			bezier_PointList.clear();
			bezier_HYPoint = temp_HYPoint1;
			bezier_PointList.push_back(bezier_HYPoint);
			fdflag=0;
			size_t j=1;
			while(j<(temp_PointList.size()-1))
			{
				CHYPoint& temp_HYPoint1 = temp_PointList[j-1];
				CHYPoint& temp_HYPoint2 = temp_PointList[j];
				CHYPoint& temp_HYPoint3 = temp_PointList[j+1];

				double a=sqrt((double)((temp_HYPoint1.x-temp_HYPoint2.x)*(temp_HYPoint1.x-temp_HYPoint2.x)+(temp_HYPoint1.y-temp_HYPoint2.y)*(temp_HYPoint1.y-temp_HYPoint2.y)));
				double b=sqrt((double)((temp_HYPoint2.x-temp_HYPoint3.x)*(temp_HYPoint2.x-temp_HYPoint3.x)+(temp_HYPoint2.y-temp_HYPoint3.y)*(temp_HYPoint2.y-temp_HYPoint3.y)));
				double c=sqrt((double)((temp_HYPoint1.x-temp_HYPoint3.x)*(temp_HYPoint1.x-temp_HYPoint3.x)+(temp_HYPoint1.y-temp_HYPoint3.y)*(temp_HYPoint1.y-temp_HYPoint3.y)));

				if (a == 0) break;
				if (b == 0) break;
				if (c == 0) break;

				double cosc=(a*a+b*b-c*c)/(2*a*b);
				if((temp_HYPoint2.x-temp_HYPoint1.x)!=0)
				{
					k0=(double)(temp_HYPoint2.y-temp_HYPoint1.y)/(temp_HYPoint2.x-temp_HYPoint1.x);
				}
				else
				{
					k0=N+1;
				}
				if((temp_HYPoint3.x-temp_HYPoint2.x)!=0)
				{
					k1=(double)(temp_HYPoint3.y-temp_HYPoint2.y)/(temp_HYPoint3.x-temp_HYPoint2.x);
				}
				else
				{
					k1=N+1;
				}
				if((cosc>=cosA)|| (abs(k0)>=1 && abs(k1)<1)  || (abs(k0)<1 && abs(k1)>=1) || temp_HYPoint3.flag == POINT_FLG_CONTROL)
				{						
					size_t b_size=bezier_PointList.size();
					if(b_size>1)
					{
						bezier_HYPoint = temp_HYPoint2;
						bezier_PointList.push_back(bezier_HYPoint);

						std::vector<CHYPoint>	toPointlist;
						std::vector<CHYPoint>	toPointlistOut;
						CHYPoint				toHYPoint;
						CHYPoint				b_HYPoint;
						bPointList.clear();
						if(iBezierN == 2)
						{
							bezierS2_2(bezier_PointList, toPointlist,dbvalue,iBezierN,dbDelta);
						}
						else if(iBezierN == 3)
						{
							bezierS(bezier_PointList, toPointlist,dbvalue,iBezierN);
						}

						int itoPointlistNum = (int)toPointlist.size();
						for(int l=0; l<itoPointlistNum; l++)				
						{
							toHYPoint=toPointlist[l];							
							b_HYPoint = toHYPoint;
							pHy_PointList.push_back(b_HYPoint);
						}

						temp_HYPoint2.flag = POINT_FLG_ANCHOR;
						pHy_HYPoint = temp_HYPoint2;
						pHy_PointList.push_back(pHy_HYPoint);

						bezier_PointList.clear();
						bPointList.clear();
						bezier_HYPoint = temp_HYPoint2;
						bezier_PointList.push_back(bezier_HYPoint);
						j=j+1;

						int itemp_PointListNum = (int)temp_PointList.size();
						if(j==itemp_PointListNum-1)
						{	
							temp_HYPoint3.flag = POINT_FLG_ANCHOR;
							pHy_HYPoint = temp_HYPoint3;
							pHy_PointList.push_back(pHy_HYPoint);
						}				
					}
					else
					{	
						temp_HYPoint2.flag = POINT_FLG_ANCHOR;						
						pHy_HYPoint = temp_HYPoint2;
						pHy_PointList.push_back(pHy_HYPoint);

						bezier_PointList.clear();
						bPointList.clear();

						bezier_HYPoint = temp_HYPoint2;
						bezier_PointList.push_back(bezier_HYPoint);
						j=j+1;

						int temp_PointListNum = (int)temp_PointList.size();
						if(j==temp_PointListNum-1)
						{	
							temp_HYPoint3.flag = POINT_FLG_ANCHOR;
							pHy_HYPoint = temp_HYPoint3;
							pHy_PointList.push_back(pHy_HYPoint);
						}					
					}
				}
				else
				{
					bezier_HYPoint = temp_HYPoint2;
					bezier_PointList.push_back(bezier_HYPoint);
					j=j+1;
					if(j==temp_PointList.size()-1)
					{
						bezier_HYPoint = temp_HYPoint3;
						bezier_PointList.push_back(bezier_HYPoint);
						int b_size;
						b_size=bezier_PointList.size();

						std::vector<CHYPoint>	toPointlist;
						std::vector<CHYPoint>	toPointlistOut;
						CHYPoint				toHYPoint;						
						CHYPoint				b_HYPoint;

						bPointList.clear();
						if(iBezierN == 2)
						{
							bezierS2_2(bezier_PointList, toPointlist,dbvalue,iBezierN,dbDelta);
						}
						else if(iBezierN == 3)
						{
							bezierS(bezier_PointList, toPointlist,dbvalue,iBezierN);
						}

						int itoPointlistNum = (int)toPointlist.size();
						for(int l=0; l<itoPointlistNum; l++)				
						{
							toHYPoint=toPointlist[l];							
							b_HYPoint = toHYPoint;
							pHy_PointList.push_back(b_HYPoint);
						}

						temp_HYPoint3.flag = POINT_FLG_ANCHOR;
						pHy_HYPoint = temp_HYPoint3;
						pHy_PointList.push_back(pHy_HYPoint);
					}
				}
			}		
			pHy_ContourItem.vtHYPoints = pHy_PointList;			
			pHy.push_back(pHy_ContourItem);
			pHy_PointList.clear();
		}
	}    // end of double  CHYImageToFont::BezierContour()

	void CHYImageToFont::bezierS(std::vector<CHYPoint>& temp_PointList,std::vector<CHYPoint>&	toPointlist,double dbvalue,int iBezierN)
	{
		int i,j;
		double DPhi;
		double dk[1000]={0};
		double maxdk=0;
		int    maxdki=0, maxdkii=0;
		int    NN = (int)temp_PointList.size();

		CHYPoint toPoint1,toPoint2,toPoint3,toPoint4,toPoint21,toPoint22;
		std::vector<CHYPoint> 	temp_PointList1;
		std::vector<CHYPoint>	temp_PointList2;
		std::vector<CHYPoint>   bPointList;
		CHYPoint m_HYPoint1,m_HYPoint2,m_HYPoint3,m_HYPoint4;
		CHYPoint  temp_HYPoint,temp_point;
		int      bflag=0;
		int      returnflag = 0;
		bflag=beziern(temp_PointList,toPointlist,iBezierN,dbvalue);
		if(bflag!=-1)
		{	
			return;	
		}
		else
		{
			bflag=0;
			toPointlist.clear();
			bPointList.clear();
			for(j=1;j<NN-1;j++)
			{
				m_HYPoint1=temp_PointList[0];
				m_HYPoint2=temp_PointList[NN-1];				
				temp_HYPoint=temp_PointList[j];	

				dk[j]=dispointtoline(m_HYPoint1,m_HYPoint2,temp_HYPoint);
				if(dk[j]>maxdk)
				{
					maxdk=dk[j];
					maxdkii=j;
					returnflag = 1;
				}
			}
			if(returnflag == 0) return;				
			returnflag = 0;

			if(NN >= 4)
			{
				//找拐点
				for(j=1;j<NN-2;j++)
				{
					m_HYPoint1=temp_PointList[j-1];
					m_HYPoint2=temp_PointList[j];
					m_HYPoint3=temp_PointList[j+1];
					m_HYPoint4=temp_PointList[j+2];

					DPhi = ((m_HYPoint2.x - m_HYPoint1.x)*(m_HYPoint3.y - m_HYPoint2.y) - (m_HYPoint3.x - m_HYPoint2.x)*(m_HYPoint2.y - m_HYPoint1.y))*((m_HYPoint3.x - m_HYPoint2.x)*(m_HYPoint4.y - m_HYPoint3.y) - (m_HYPoint4.x - m_HYPoint3.x)*(m_HYPoint3.y - m_HYPoint2.y));
					if(DPhi < 0)
					{
						maxdki = j;
						returnflag = 1;
					}
				}
				if(returnflag == 0)
					maxdki = maxdkii;
			}
			else
			{
				maxdki = maxdkii;
			}

			for(j=0;j<=maxdki;j++)
			{
				m_HYPoint1=temp_PointList[j];				
				temp_HYPoint = m_HYPoint1;
				temp_PointList1.push_back(temp_HYPoint);
			}

			if(temp_PointList1.size()<=2)
			{
				temp_PointList2.clear();
			}
			else if(temp_PointList1.size()<=4)
			{
				beziern(temp_PointList1,temp_PointList2,iBezierN,dbvalue);
			}
			else
			{
				bezierS(temp_PointList1,temp_PointList2,dbvalue,iBezierN);
			}

			int temp_PointList2Num = (int)temp_PointList2.size();
			for(i=0;i<temp_PointList2Num;i++)
			{
				m_HYPoint1=temp_PointList2[i];				
				temp_HYPoint = m_HYPoint1;
				toPointlist.push_back(temp_HYPoint);
			}
			m_HYPoint1=temp_PointList[maxdki];			
			temp_HYPoint = m_HYPoint1;
			toPointlist.push_back(temp_HYPoint);

			temp_PointList1.clear();
			temp_PointList2.clear();
			for(j=maxdki;j<NN;j++)
			{
				m_HYPoint1=temp_PointList[j];				
				temp_HYPoint = m_HYPoint1;
				temp_PointList1.push_back(temp_HYPoint);	
			}
			if(temp_PointList1.size()<=2)
			{
				temp_PointList2.clear();
			}
			else if(temp_PointList1.size()<=4)
			{
				beziern(temp_PointList1,temp_PointList2,iBezierN,dbvalue);
			}
			else
			{
				bezierS(temp_PointList1,temp_PointList2,dbvalue,iBezierN);
			}		

			int itemp_PointListNum = (int)temp_PointList2.size();
			for(i=0;i<itemp_PointListNum;i++)
			{
				m_HYPoint1=temp_PointList2[i];				
				temp_HYPoint = m_HYPoint1;
				toPointlist.push_back(temp_HYPoint);
			}
		}

	}  // end of double  CImgFontCodec::bezierS()


	void CHYImageToFont::bezierS2_2(std::vector<CHYPoint>&	temp_PointList,std::vector<CHYPoint>& toPointlist,double dbvalue,int dbBezierN,double dbDelta)
	{
		int			i,j;
		int			NN = (int)temp_PointList.size();
		int			ANN = (int)temp_PointList.size()+2;
		double		d[N]={0};               //累加弦长
		double		t[N]={0};              //对应轮廓段上离散点的估计的Bezier曲线上的点
		int			bflag = 0;
		double		dk[N]={0};
		double		maxdk = 0;
		int			maxdki = 0;
		int			dmmi=0;
		double		dmm=0;
		double		dmi=100000;

		CHYPoint	HYPoint1,HYPoint2,temp_HYPoint;
		CHYPoint	pt;

		std::vector<CHYPoint>	add_PointList;
		std::vector<CHYPoint>	PointList;
		std::vector<CHYPoint>	new_PointList;
		std::vector<CHYPoint>	add_PointList1;		

		bflag=beziern(temp_PointList,toPointlist,dbBezierN,dbvalue);
		if(bflag!=-1)
		{	
			return;	
		}

		for(i=0 ; i<NN;i++)
		{
			HYPoint2=temp_PointList[i];			
			temp_HYPoint = HYPoint2;
			add_PointList.push_back(temp_HYPoint);
		}

		HYPoint1=add_PointList[0];		
		temp_HYPoint = HYPoint1;
		add_PointList1.push_back(temp_HYPoint);

		int iAdd_PointListNum = (int)add_PointList.size();
		for(j=0 ; j < iAdd_PointListNum-1;j++)
		{
			if(j == 0)
			{
				HYPoint1=add_PointList[j];
				HYPoint2=add_PointList[j+1];

				if(dis(HYPoint1,HYPoint2) < 100)
				{
					temp_HYPoint.x=(HYPoint1.x * 2 + HYPoint2.x * 2)/4;
					temp_HYPoint.y=(HYPoint1.y * 2 + HYPoint2.y * 2)/4;					
					add_PointList1.push_back(temp_HYPoint);
				}
				temp_HYPoint = HYPoint2;
				add_PointList1.push_back(temp_HYPoint);

			}
			else if(j == iAdd_PointListNum-2)
			{
				HYPoint1=add_PointList[j];
				HYPoint2=add_PointList[j+1];

				if(dis(HYPoint1,HYPoint2) < 100)
				{
					temp_HYPoint.x=(HYPoint1.x * 2 + HYPoint2.x * 2)/4;
					temp_HYPoint.y=(HYPoint1.y * 2 + HYPoint2.y * 2)/4;					
					add_PointList1.push_back(temp_HYPoint);
				}
				temp_HYPoint = HYPoint2;
				add_PointList1.push_back(temp_HYPoint);
			}
			else
			{
				HYPoint2=add_PointList[j+1];				
				temp_HYPoint= HYPoint2;
				add_PointList1.push_back(temp_HYPoint);
			}			
		}
		if(isSCurve(temp_PointList) == 1)
		{
			int itemp_PointLstNum = temp_PointList.size();
			for(j=1;j<itemp_PointLstNum-1;j++)
			{
				HYPoint1=temp_PointList[0];
				HYPoint2=temp_PointList[itemp_PointLstNum-1];

				temp_HYPoint=temp_PointList[j];				
				dk[j]=dispointtoline(HYPoint1,HYPoint2,temp_HYPoint);
				if(dk[j]>maxdk)
				{
					maxdk=dk[j];
					maxdki=j;
				}
			}
			if(maxdk < 5)
			{
				MeanContour(add_PointList1,new_PointList);
			}
			else
			{
				int add_PointList1Num = add_PointList1.size();
				for(i=0 ; i<add_PointList1Num;i++)
				{
					HYPoint2=add_PointList1[i];					
					temp_HYPoint = HYPoint2;
					new_PointList.push_back(temp_HYPoint);
				}
			}
		}
		else
		{
			int add_PointList1Num = add_PointList1.size();
			for(i=0 ; i<add_PointList1Num;i++)
			{
				HYPoint2=add_PointList1[i];				
				temp_HYPoint = HYPoint2;
				new_PointList.push_back(temp_HYPoint);
			}
		}

		add_PointList.clear();
		toPointlist.clear();
		PointList.clear();

		//增加端点数据
		HYPoint1=new_PointList[0];		
		HYPoint2=new_PointList[1];		
		temp_HYPoint.x = 2*HYPoint1.x - HYPoint2.x;

		HYPoint1=new_PointList[new_PointList.size()-1];		
		temp_HYPoint.y = HYPoint1.y;

		add_PointList.push_back(temp_HYPoint);
		int inew_PointListNum = (int)new_PointList.size();
		for(i=0;i<inew_PointListNum;i++)
		{
			HYPoint2=new_PointList[i];			
			temp_HYPoint = HYPoint2;
			add_PointList.push_back(temp_HYPoint);
		}

		HYPoint1= new_PointList[inew_PointListNum-2];		
		HYPoint2=new_PointList[inew_PointListNum-1];		
		temp_HYPoint.x = 2*HYPoint2.x - HYPoint1.x;

		HYPoint1=new_PointList[0];		
		temp_HYPoint.y = HYPoint1.y;		
		add_PointList.push_back(temp_HYPoint);

		HYPoint1=add_PointList[0];		
		PointList.push_back(HYPoint1);

		int add_PointListNum = (int)add_PointList.size();
		for(i=1;i<add_PointListNum;i++)
		{
			HYPoint2=add_PointList[i];			
			if(HYPoint1.x !=HYPoint2.x || HYPoint1.y !=HYPoint2.y)
			{
				temp_HYPoint = HYPoint2;
				PointList.push_back(temp_HYPoint);
				HYPoint1.x = HYPoint2.x;
				HYPoint1.y = HYPoint2.y;
			}
		}

		ANN = (int)PointList.size();
		double		*A			= new double[ANN*ANN];
		double		*A_			= new double[ANN*ANN];
		double		*B			= new double[ANN*2*ANN];
		CHYPoint	*toPoint0	=new CHYPoint[ANN+2];
		CHYPoint	*toPoint1	=new CHYPoint[ANN+2];
		for(i=1; i<ANN;i++)
		{
			HYPoint1=PointList[i-1];
			HYPoint2=PointList[i];
			d[i]=sqrt((double)((HYPoint1.x-HYPoint2.x)*(HYPoint1.x-HYPoint2.x)+(HYPoint1.y-HYPoint2.y)*(HYPoint1.y-HYPoint2.y)));
		}
		for(i=1; i<ANN-1;i++)
		{
			HYPoint1=PointList[i-1];
			HYPoint2=PointList[i];			
			temp_HYPoint=PointList[i+1];		
			t[i]=d[i]/(d[i]+d[i+1]);
		}
		for(i=0;i<ANN;i++)
		{
			for(j=0; j<ANN; j++)
			{
				A[i*ANN+j]=0;
			}
		}
		for(i=1;i<ANN-1;i++)
		{
			A[i*ANN+i-1]=(t[i]*t[i]-2*t[i]+1)/2;
			A[i*ANN+i]=(-2*t[i]*t[i]+2*t[i]+1)/2;
			A[i*ANN+i+1]=(t[i]*t[i])/2;
		}
		/*****************************************************************/
		A[0]=(-2*t[ANN - 2]*t[ANN - 2]+2*t[ANN - 2]+1)/2;
		A[1]=(t[ANN - 2]*t[ANN - 2])/2;
		A[ANN-3]=(t[ANN - 2]*t[ANN - 2]-2*t[ANN - 2]+1)/2;

		A[(ANN-1)*ANN+2]=(t[1]*t[1])/2;
		A[(ANN-1)*ANN+ANN-2]=(t[1]*t[1]-2*t[1]+1)/2;
		A[(ANN-1)*ANN+ANN-1]=(-2*t[1]*t[1]+2*t[1]+1)/2;

		for(i = 0;i<ANN;i++)
		{
			temp_HYPoint=PointList[i];		
			toPoint1[i].x = temp_HYPoint.x;
			toPoint1[i].y = temp_HYPoint.y;
			toPoint1[i].flag = POINT_FLG_CONTROL;
		}

		double error = 10000;
		while(abs(error) > dbvalue)
		{
			error = 0;
			new_PointList.clear();
			for(i=0;i<ANN;i++)
			{
				temp_HYPoint=PointList[i];				
				toPoint0[i].x = toPoint1[i].x;
				toPoint0[i].y = toPoint1[i].y;
				toPoint0[i].flag = POINT_FLG_CONTROL;
			}

			temp_HYPoint=PointList[0];			
			HYPoint1.x = ::HY_RealRount(A[0] * toPoint0[0].x + A[1] * toPoint0[1].x + A[ANN-3] * toPoint0[ANN-3].x);
			HYPoint1.y = ::HY_RealRount(A[0] * toPoint0[0].y + A[1] * toPoint0[1].y + A[ANN-3] * toPoint0[ANN-3].y);
			error += dis(temp_HYPoint,HYPoint1);

			toPoint1[0].x = toPoint0[0].x + temp_HYPoint.x - HYPoint1.x;
			toPoint1[0].y = toPoint0[0].y + temp_HYPoint.y - HYPoint1.y;

			for(int i = 1;i < ANN - 1;i++)
			{
				temp_HYPoint=PointList[i];				
				HYPoint1.x = ::HY_RealRount(A[i*ANN+i-1] * toPoint0[i - 1].x + A[i*ANN+i] * toPoint0[i].x + A[i*ANN+i+1] * toPoint0[i+1].x);
				HYPoint1.y = ::HY_RealRount(A[i*ANN+i-1] * toPoint0[i - 1].y + A[i*ANN+i] * toPoint0[i].y + A[i*ANN+i+1] * toPoint0[i+1].y);
				error += dis(temp_HYPoint,HYPoint1);

				toPoint1[i].x = toPoint0[i].x + temp_HYPoint.x - HYPoint1.x;
				toPoint1[i].y = toPoint0[i].y + temp_HYPoint.y - HYPoint1.y;
			}

			temp_HYPoint=PointList[PointList.size()-1];

			double x = A[(ANN-1)*ANN+2] * toPoint0[2].x + A[(ANN-1)*ANN+ANN-2] * toPoint0[ANN-2].x + A[(ANN-1)*ANN+ANN-1] * toPoint0[ANN-1].x;
			HYPoint1.x = HY_RealRount(x);
			double y = A[(ANN-1)*ANN+2] * toPoint0[2].y + A[(ANN-1)*ANN+ANN-2] * toPoint0[ANN-2].y + A[(ANN-1)*ANN+ANN-1] * toPoint0[ANN-1].y;
			HYPoint1.y = HY_RealRount(y);
			error += dis(temp_HYPoint,HYPoint1);

			toPoint1[ANN-1].x = toPoint0[ANN-1].x + temp_HYPoint.x - HYPoint1.x;
			toPoint1[ANN-1].y = toPoint0[ANN-1].y + temp_HYPoint.y - HYPoint1.y;	
		}
		for(i=1;i<ANN-1;i++)
		{
			toPoint0[i].flag=POINT_FLG_CONTROL;
		}

		add_PointList.clear();
		PointList.clear();
		for(i=2;i<ANN-2;i++)
		{
			temp_HYPoint = toPoint0[i];
			PointList.push_back(temp_HYPoint);
		}

		HYPoint1=PointList[0];		
		toPointlist.push_back(HYPoint1);

		int PointListNum = (int)PointList.size();
		for(int j=1; j<PointListNum-1;j++)
		{
			HYPoint2=PointList[j+1];
			temp_HYPoint=PointList[j];

			if(temp_HYPoint.x == HYPoint1.x && temp_HYPoint.y == HYPoint1.y)continue;
			if(abs(dispointtoline(HYPoint1,HYPoint2,temp_HYPoint))<0.5*dbDelta)continue;

			HYPoint1=PointList[j];
			toPointlist.push_back(temp_HYPoint);
		}

		int iPointSize = PointList.size();
		if (iPointSize>0)
		{ 
			HYPoint1=PointList[PointList.size()-1];
			toPointlist.push_back(HYPoint1);
		}

		add_PointList.clear();
		PointList.clear();
		new_PointList.clear();
		add_PointList1.clear();

		if (A)			delete[] A;
		A = NULL;
		if (A_)			delete[] A_;
		A_ = NULL;
		if (B)			delete[] B;
		B = NULL;
		if (toPoint0)	delete[] toPoint0;
		toPoint0 = NULL;
		if (toPoint1)	delete[] toPoint1;
		toPoint1 = NULL;

	}// end of double  CImgFontCodec::bezierS2_2()

	//两点间距离函数
	float CHYImageToFont::dis(const CHYPoint& p1,const CHYPoint& p2) 
	{ 
		return(sqrt((float)((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y)))); 

	}// end of double  CHYImageToFont::dis()

	int  CHYImageToFont::beziern(std::vector<CHYPoint>&	temp_PointList,std::vector<CHYPoint>& toPointlist,int BN,double dbvalue)
	{
		int i,j,l;
		int NN = (int)temp_PointList.size();
		int bflag=0, Guassflag=0;
		double segma=0, tempx=0, tempy=0;
		double dis[N]={0};
		int maxdi=0, size=0;
		double sum=0;                 //累加弦长
		double t[N]={0};              //对应轮廓段上离散点的估计的Bezier曲线上的点
		double dt=0;                  //单位增加量
		double dk[1000]={0};
		double maxdk=0;
		int    maxdki=0;
		double dmm=0,dmi=100000;
		int		dmmi=0;
		double d[N-1]={0};              //弦长

		double *S=new double[(BN-1)*(BN+1)];
		double *T=new double[(BN-1)*2];
		double *A=new double[(BN-1)*(BN-1)];
		double *X=new double[BN-1];
		double *B=new double[BN-1];

		CHYPoint HYPoint1;
		CHYPoint HYPoint2;
		CHYPoint temp_HYPoint;
		CHYPoint temp_HYPoint0;		

		CHYPoint  *toPoint=new CHYPoint[BN-1];

		std::vector<CHYPoint>	add_PointList;
		std::vector<CHYPoint>	add_PointList1;
		std::vector<CHYPoint>	new_PointList;

		for(i=0;i<(BN-1)*(BN+1);i++)
		{S[i]=0;}
		for(i=0;i<(BN-1)*2;i++)
		{T[i]=0;}

		for(i=1; i<NN;i++)
		{
			HYPoint1=temp_PointList[i-1];
			HYPoint2=temp_PointList[i];		

			d[i-1]=sqrt((double)((HYPoint1.x-HYPoint2.x)*(HYPoint1.x-HYPoint2.x)+(HYPoint1.y-HYPoint2.y)*(HYPoint1.y-HYPoint2.y)));
			if(d[i-1]>dmm)
			{dmmi=i-1;dmm=d[i-1];}
			if(d[i-1]<dmi)
			{dmi=d[i-1];}
		}
		if((dmm/dmi)>=bcdis)
		{
			for(j=0 ; j<=dmmi;j++)
			{
				HYPoint1=temp_PointList[j];				
				temp_HYPoint = HYPoint1;
				add_PointList.push_back(temp_HYPoint);
			}
			for(j=dmmi ; j<(dmmi+1);j++)
			{
				HYPoint1=temp_PointList[j];
				HYPoint2=temp_PointList[j+1];

				temp_HYPoint.x=(HYPoint1.x * 3 + HYPoint2.x)/4;
				temp_HYPoint.y=(HYPoint1.y * 3 + HYPoint2.y)/4;				
				add_PointList.push_back(temp_HYPoint);

				temp_HYPoint.x=(HYPoint1.x * 2 + HYPoint2.x * 2)/4;
				temp_HYPoint.y=(HYPoint1.y * 2 + HYPoint2.y * 2)/4;				
				add_PointList.push_back(temp_HYPoint);

				temp_HYPoint.x=(HYPoint1.x + HYPoint2.x * 3)/4;
				temp_HYPoint.y=(HYPoint1.y + HYPoint2.y * 3)/4;				
				add_PointList.push_back(temp_HYPoint);
			}
			for(j=(dmmi+1) ; j<NN;j++)
			{
				HYPoint1= temp_PointList[j];				
				temp_HYPoint = HYPoint1;
				add_PointList.push_back(temp_HYPoint);
			}
		}
		else
		{
			for(i=0 ; i<NN;i++)
			{
				HYPoint2=temp_PointList[i];				
				temp_HYPoint=HYPoint2;
				add_PointList.push_back(temp_HYPoint);
			}
		}

		HYPoint1=add_PointList[0];		
		temp_HYPoint = HYPoint1;
		add_PointList1.push_back(temp_HYPoint);

		int stadd_PointLstNum = (int)add_PointList.size();
		for(j=0; j<stadd_PointLstNum-1;j++)
		{
			HYPoint1=add_PointList[j];
			HYPoint2=add_PointList[j+1];			
			temp_HYPoint.x=(HYPoint1.x+HYPoint2.x)/2;
			temp_HYPoint.y=(HYPoint1.y+HYPoint2.y)/2;			

			add_PointList1.push_back(temp_HYPoint);
			temp_HYPoint = HYPoint2;

			add_PointList1.push_back(temp_HYPoint);
		}
		if(isSCurve(temp_PointList) == 1)
		{
			for(j=1;j<NN-1;j++)
			{
				HYPoint1=temp_PointList[0];
				HYPoint2=temp_PointList[NN-1];				
				temp_HYPoint=temp_PointList[j];				
				dk[j]=dispointtoline(HYPoint1,HYPoint2,temp_HYPoint);
				if(dk[j]>maxdk)
				{
					maxdk=dk[j];
					maxdki=j;
				}
			}
			if(maxdk < 5)
			{
				MeanContour(add_PointList1,new_PointList);
			}
			else
			{
				int iadd_PointList1Num = add_PointList1.size();
				for(i=0;i<iadd_PointList1Num;i++)
				{
					HYPoint2=add_PointList1[i];					
					temp_HYPoint = HYPoint2;
					new_PointList.push_back(temp_HYPoint);
				}
			}
		}
		else
		{
			int iadd_PointList1Num = add_PointList1.size();
			for(i=0;i<iadd_PointList1Num;i++)
			{
				HYPoint2=add_PointList1[i];				
				temp_HYPoint = HYPoint2;
				new_PointList.push_back(temp_HYPoint);
			}
		}

		int addNN = (int)new_PointList.size();
		double *Phi=new double[(BN+1)*addNN];           //Bezier曲线的基函数
		for(i=1 ; i<addNN;i++)
		{
			HYPoint1=new_PointList[i-1];
			HYPoint2=new_PointList[i];			

			d[i-1]=sqrt((double)((HYPoint1.x-HYPoint2.x)*(HYPoint1.x-HYPoint2.x)+(HYPoint1.y-HYPoint2.y)*(HYPoint1.y-HYPoint2.y)));
		}

		for(i=0; i<addNN-1;i++)
		{
			sum+=d[i];
		}

		dt=1/sum;
		for(i=0; i<addNN; i++)
		{
			double sum0=0;
			for(j=0; j<i;j++)
			{
				sum0+=d[j];

			}
			t[i]=sum0*dt;
		}


		for(j=0;j<(BN+1);j++)
		{
			for(i=0; i<addNN; i++)
			{	
				Phi[j*addNN+i]=(float)(Factorial(BN)/(Factorial(j)*Factorial(BN-j)))*pow(1-t[i],BN-j)*pow(t[i],j);
			}
		}	


		for(i=0;i<BN-1;i++)
		{
			for(j=0;j<BN+1;j++)
			{
				for(l=0;l<addNN;l++)
				{
					S[i*(BN+1)+j]=S[i*(BN+1)+j]+Phi[(i+1)*addNN+l]*Phi[j*addNN+l];
				}
			}
		}
		for(i=0;i<BN-1;i++)
		{
			for(l=0;l<addNN;l++)
			{
				HYPoint1=new_PointList[l];				
				T[2*i]=T[2*i]+Phi[(i+1)*addNN+l]*HYPoint1.x;
				T[2*i+1]=T[2*i+1]+Phi[(i+1)*addNN+l]*HYPoint1.y;
			}
		}
		HYPoint1=new_PointList[0];
		HYPoint2=new_PointList[addNN-1];

		for(i=0;i<BN-1;i++)
		{
			for(j=0;j<BN-1;j++)
			{
				A[i*(BN-1)+j]=S[i*(BN+1)+j+1];
			}
		}
		for(i=0;i<BN-1;i++)
		{
			B[i]=T[2*i]-S[i*(BN+1)+0]*HYPoint1.x-S[i*(BN+1)+BN]*HYPoint2.x;
		}

		Guassflag=Guass(A,X,B,BN-1,Guassflag);
		for(i=0;i<BN-1;i++)
		{
			toPoint[i].x=::HY_RealRount(X[i]);
		}
		for(i=0;i<BN-1;i++)
		{
			B[i]=T[2*i+1]-S[i*(BN+1)+0]*HYPoint1.y-S[i*(BN+1)+BN]*HYPoint2.y;
		}
		Guassflag=Guass(A,X,B,BN-1,Guassflag);
		for(i=0;i<BN-1;i++)
		{
			toPoint[i].y=::HY_RealRount(X[i]);
		}

		for(i=0;i<BN-1;i++)
		{	
			toPoint[i].flag=POINT_FLG_CONTROL;
		}

		for(i=0;i<BN-1;i++)
		{
			temp_HYPoint=toPoint[i];
			toPointlist.push_back(temp_HYPoint);
		}

		bflag=bezierntest(new_PointList,toPointlist,BN,bflag,dbvalue);

		if (Phi) delete[] Phi;
		Phi = NULL;

		if (toPoint) delete[] toPoint;
		toPoint = NULL;

		if (S) delete[] S;
		S = NULL;
		if (T) delete[] T;
		T = NULL;
		if (A) delete[] A;
		A = NULL;
		if (X) delete[] X;
		X = NULL;
		if (B) delete[] B;
		B = NULL;

		return bflag;

	}  // end of double  CHYImageToFont::beziern()

	//bezier检验函数，如果点在bezier曲线上返回true否则返回false
	int  CHYImageToFont::bezierntest(std::vector<CHYPoint>&	temp_PointList01,std::vector<CHYPoint>&	toPointlist,int BN,int bflag,double dbvalue)//bezier检验函数，如果点在bezier曲线上返回true否则返回false
	{
		int			NN = (int)temp_PointList01.size();
		double		ts=0,te=1,d=0,ds=0,de=0,t=0,x=0,y=0;
		int			size=0;
		CHYPoint	HYPoint1;
		CHYPoint	HYPoint2;
		CHYPoint	temp_HYPoint;		

		CHYPoint*	toPoint=new CHYPoint[BN-1];
		HYPoint1=temp_PointList01[0];
		HYPoint2=temp_PointList01[NN-1];

		for(int i=1;i<NN-1;i++)
		{
			int countj=1;
			ts=0;
			te=1;
			temp_HYPoint=temp_PointList01[i];			
			t=(ts+te)/2;
			x=pow(1-t,BN)*HYPoint1.x+pow(t,BN)*HYPoint2.x;
			y=pow(1-t,BN)*HYPoint1.y+pow(t,BN)*HYPoint2.y;
			for(int j=1;j<BN;j++)
			{
				temp_HYPoint=toPointlist[j-1];
				toPoint[j-1]=temp_HYPoint;
				x=x+(Factorial(BN)/(Factorial(j)*Factorial(BN-j)))*pow(1-t,BN-j)*pow(t,j)*toPoint[j-1].x;
				y=y+(Factorial(BN)/(Factorial(j)*Factorial(BN-j)))*pow(1-t,BN-j)*pow(t,j)*toPoint[j-1].y;
			}
			d=sqrt((x-temp_HYPoint.x)*(x-temp_HYPoint.x)+(y-temp_HYPoint.y)*(y-temp_HYPoint.y));
			ds=sqrt((float)((HYPoint1.x-temp_HYPoint.x)*(HYPoint1.x-temp_HYPoint.x)+(HYPoint1.y-temp_HYPoint.y)*(HYPoint1.y-temp_HYPoint.y)));
			de=sqrt((float)((HYPoint2.x-temp_HYPoint.x)*(HYPoint2.x-temp_HYPoint.x)+(HYPoint2.y-temp_HYPoint.y)*(HYPoint2.y-temp_HYPoint.y)));
			while(countj<20)
			{
				if(ds<=de)
				{
					te=t;
					de=d;
				}
				else
				{
					ts=t;
					ds=d;
				}
				t=(ts+te)/2;
				x=pow(1-t,BN)*HYPoint1.x+pow(t,BN)*HYPoint2.x;
				y=pow(1-t,BN)*HYPoint1.y+pow(t,BN)*HYPoint2.y;

				for(int j=1;j<BN;j++)
				{
					temp_HYPoint=toPointlist[j-1];
					toPoint[j-1]=temp_HYPoint;
					x=x+(float)(Factorial(BN)/(Factorial(j)*Factorial(BN-j)))*pow(1-t,BN-j)*pow(t,j)*toPoint[j-1].x;
					y=y+(float)(Factorial(BN)/(Factorial(j)*Factorial(BN-j)))*pow(1-t,BN-j)*pow(t,j)*toPoint[j-1].y;
				}
				d=sqrt((x-temp_HYPoint.x)*(x-temp_HYPoint.x)+(y-temp_HYPoint.y)*(y-temp_HYPoint.y));
				countj=countj+1;

			}
			if(d>dbvalue)
			{
				bflag=-1;
				if (toPoint) delete[] toPoint;
				toPoint = NULL;

				return bflag;
			}
		}

		if (toPoint) delete[] toPoint;
		toPoint = NULL;

		return bflag;

	}// end of double  CImgFontCodec::bezierntest()

	int CHYImageToFont::Factorial(int n)
	{
		int i;
		int result=1;
		if(n==0)
		{result=1;}
		else
		{
			for(i=1;i<=n;i++)
			{
				result=result*i;
			}
		}
		return result;

	}  // end of double  CHYImageToFont::Factorial()

	int CHYImageToFont::Guass(double *A,double *X,double *B,int XN,int Guassflag)
	{
		int i,j,k,maxk=0;
		double maxA=0;
		double aa=0;
		double *tempA=new double[(XN+1)];
		double *AB=new double[(XN+1)*XN];
		double tempX=0;
		double absA=1;
		for(i=0;i<XN;i++)
		{
			for(j=0;j<XN;j++)
			{
				AB[i*(XN+1)+j]=A[i*XN+j];
			}
			AB[i*(XN+1)+XN]=B[i];
		}
		for(i=0;i<XN-1;i++)
		{
			maxA=abs(AB[i*(XN+1)+i]);
			maxk = i;
			for(j=i;j<XN;j++)
			{
				if(abs(AB[j*(XN+1)+i])>maxA)
				{maxA=AB[j*(XN+1)+i];maxk=j;}
			}
			for(j=0;j<(XN+1);j++)//两行互换
			{
				tempA[j]=AB[i*(XN+1)+j];
				AB[i*(XN+1)+j]=AB[maxk*(XN+1)+j];
				AB[maxk*(XN+1)+j]=tempA[j];
			}
			for(j=i+1;j<XN;j++)
			{
				if(AB[j*(XN+1)+i] == 0)continue;
				aa=-AB[i*(XN+1)+i]/AB[j*(XN+1)+i];
				for(k=0;k<(XN+1);k++)
				{
					AB[j*(XN+1)+k]=AB[j*(XN+1)+k]*aa+AB[i*(XN+1)+k];
				}
			}
		}
		for(i=0;i<XN;i++)
		{
			aa=AB[i*(XN+1)+i];
			for(j=0;j<(XN+1);j++)
			{
				AB[i*(XN+1)+j]=AB[i*(XN+1)+j]/aa;
			}
		}
		for(i=0;i<XN;i++)
		{
			absA=absA*AB[i*(XN+1)+i];
		}
		if(absA==0)
		{
			Guassflag=-1;

			if (AB) delete[] AB;
			AB =NULL;
			if (tempA) delete[] tempA;
			tempA =NULL;

			return Guassflag;
		}
		else
		{
			X[XN-1]=AB[(XN-1)*(XN+1)+XN];
			for(i=XN-2;i>=0;i--)
			{
				tempX=0;
				for(j=i+1;j<XN;j++)
				{
					tempX+=AB[i*(XN+1)+j]*X[j];
				}
				X[i]=AB[i*(XN+1)+XN]-tempX;
			}
			Guassflag=0;

			if (AB) delete[] AB;
			AB =NULL;
			if (tempA) delete[] tempA;
			tempA =NULL;

			return Guassflag;
		}

		if (AB) delete[] AB;
		AB =NULL;
		if (tempA) delete[] tempA;
		tempA =NULL;

		return Guassflag;

	}  // end of double  CHYImageToFont::Guass()

	void CHYImageToFont::MeanContour(std::vector<CHYPoint>& ContourItemLst,std::vector<CHYPoint>& pHy)
	{
		int				j;
		CHYPoint		HYPt;
		CHYPoint& temp_HYPoint1 = ContourItemLst[0];

		HYPt = temp_HYPoint1;
		pHy.push_back(HYPt);

		int iCntrItmLstNum = (int)ContourItemLst.size();
		for(j=1; j < iCntrItmLstNum-1; j++)
		{
			const CHYPoint& temp_HYPoint1 = ContourItemLst[j-1];
			const CHYPoint& temp_HYPoint2 = ContourItemLst[j];
			const CHYPoint& temp_HYPoint3 = ContourItemLst[j+1];

			HYPt.x = (temp_HYPoint1.x + temp_HYPoint2.x + temp_HYPoint3.x)/3;
			HYPt.y = (temp_HYPoint1.y + temp_HYPoint2.y + temp_HYPoint3.y)/3;			
			pHy.push_back(HYPt);
		}

		temp_HYPoint1 = ContourItemLst[iCntrItmLstNum-1];		
		HYPt = temp_HYPoint1;
		pHy.push_back(HYPt);

	}// end of double  CHYImageToFont::MeanContour()

	int  CHYImageToFont::isSCurve(std::vector<CHYPoint>& temp_PointList)
	{
		CHYPoint	HYPoint1;
		CHYPoint	HYPoint2;
		CHYPoint	temp_HYPoint;
		CHYPoint	pt1,pt2;

		double		k1,b1;
		int			temp_PointLstNum = (int)temp_PointList.size();

		HYPoint1=temp_PointList[0];
		HYPoint2=temp_PointList[temp_PointLstNum-1];

		if((HYPoint2.x - HYPoint1.x) != 0)
		{
			k1 = (HYPoint2.y - HYPoint1.y)/(HYPoint2.x - HYPoint1.x);
			b1 = HYPoint1.y - k1 * HYPoint1.x;
		}
		else
		{
			k1 = 100; b1 = 100;
		}
		temp_HYPoint=temp_PointList[1];
		pt1 = temp_HYPoint;

		if(temp_PointLstNum<4)
		{
			return 0;
		}
		for(int i=2; i<temp_PointLstNum-1;i++)
		{
			temp_HYPoint=temp_PointList[i];
			pt2 = temp_HYPoint;

			if(FindJLinetoLine(k1,b1,HYPoint1,pt1,pt2) == 1)
			{
				return 1;
			}
		}
		return 0;

	}// end of double  CHYImageToFont::isSCurve()

	//直线与直线相交的交点
	int CHYImageToFont::FindJLinetoLine(double k1,double b1,CHYPoint& Point11,CHYPoint& Point21,CHYPoint& Point22)
	{	
		CHYPoint		jpoint;
		PointF			Pt;
		double          xu2,xd2,yu2,yd2;
		double          k2,b2,y0,y1;		

		if(Point21.x > Point22.x)
		{
			xu2 = Point21.x;
			xd2 = Point22.x;
		}
		else
		{
			xu2 = Point22.x;
			xd2 = Point21.x;
		}

		if(Point21.y > Point22.y)
		{
			yu2 = Point21.y;
			yd2 = Point22.y;
		}
		else
		{
			yu2 = Point22.y;
			yd2 = Point21.y;
		}

		if(Point22.x - Point21.x != 0)
		{
			k2 = (double)(Point22.y - Point21.y)/(Point22.x - Point21.x);
			b2 = (double)Point22.y - k2 * Point22.x; 
		}
		else
		{
			k2 =100; b2 =100;
		}
		if(k1 != k2)
		{
			if(k1 == 100 || abs(k1) > 50)
			{
				Pt.X = (float)Point11.x;
				Pt.Y = (float)(k2*Pt.X+b2);
			}
			else if(k2 == 100 || abs(k2) > 50)
			{
				Pt.X = (float)Point21.x;
				Pt.Y = (float)(k1*Pt.X+b1);
			}
			else if(abs(k1) < 0.1 && k2 != 0)
			{
				Pt.Y = (float)Point11.y;
				Pt.X = (float)((Pt.Y-b2)/k2);
			}
			else if(abs(k2) < 0.1 && k1 != 0)
			{
				Pt.Y = (float)Point21.y;
				Pt.X = (float)((Pt.Y-b1)/k1);
			}
			else
			{	
				Pt.X = (float)((b2-b1)/(k1-k2));
				y0 = (k1*Pt.X+b1);
				y1 = (k2*Pt.X+b2);
				if(abs(k1-1) <= abs(k2-1))
				{
					Pt.Y = (float)y0;
				}
				else
				{
					Pt.Y = (float)y1;
				}			
			}

			jpoint.x = ::HY_RealRount(Pt.X);
			jpoint.y = ::HY_RealRount(Pt.Y);
			jpoint.flag = POINT_FLG_ANCHOR;
			if((jpoint.y>=yd2 && jpoint.y<=yu2 && jpoint.x>=xd2 && jpoint.x<=xu2)) 
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			return 0;
		}

	}// end of double  CHYImageToFont::FindJLinetoLine()


	//去噪
	void CHYImageToFont::Clearnoise(std::vector<CHYPoint>& VectorPointList,std::vector<CHYPoint>& ClearPointList,double alphad2,double alphad3)
	{
		int   stVectorPointList = (int)VectorPointList.size();
		if(stVectorPointList<4)
		{
			for(int i=0; i<stVectorPointList; i++)
			{
				const  CHYPoint& temp_HYPoint = VectorPointList[i];
				ClearPointList.push_back(temp_HYPoint);
			}
		}
		else
		{
			CHYPoint						irde_HYPoint;
			std::vector<CHYPoint>			irde_PointList;
			double							maxddi=0;		
			CHYPoint& temp_HYPoint1 = VectorPointList[0];
			irde_HYPoint = temp_HYPoint1;

			irde_PointList.push_back(irde_HYPoint);
			int iVectorPointListNum =(int)VectorPointList.size();  
			for(int j=1; j<iVectorPointListNum-1; j++)
			{
				const CHYPoint& temp_HYPoint1 = VectorPointList[j-1];
				const CHYPoint& temp_HYPoint2 = VectorPointList[j];
				const CHYPoint& temp_HYPoint3 = VectorPointList[j+1];

				double d1 = sqrt((double)((temp_HYPoint3.x-temp_HYPoint1.x)*(temp_HYPoint3.x-temp_HYPoint1.x)+(temp_HYPoint3.y-temp_HYPoint1.y)*(temp_HYPoint3.y-temp_HYPoint1.y)));
				maxddi = d1;
				double angle = Calc_ANGLEThreePoint(temp_HYPoint1,temp_HYPoint2,temp_HYPoint3);
				PointF pt1;
				PointF pt2;

				int VectorPointNum = (int)VectorPointList.size();
				if(j == 1)
				{					
					pt1.X = (float)(temp_HYPoint1.x - VectorPointList[VectorPointNum-2].x);
					pt1.Y = (float)(temp_HYPoint1.y - VectorPointList[VectorPointNum-2].y);

					pt2.X = (float)(VectorPointList[j+2].x - temp_HYPoint3.x);
					pt2.Y = (float)(VectorPointList[j+2].y - temp_HYPoint3.y);
				}
				else if(j == VectorPointNum-2)
				{
					pt1.X = (float)(temp_HYPoint1.x - VectorPointList[j-2].x);
					pt1.Y = (float)(temp_HYPoint1.y - VectorPointList[j-2].y);
					pt2.X = (float)(VectorPointList[1].x - temp_HYPoint3.x);
					pt2.Y = (float)(VectorPointList[1].y - temp_HYPoint3.y);
				}
				else
				{
					pt1.X = (float)(temp_HYPoint1.x - VectorPointList[j-2].x);
					pt1.Y = (float)(temp_HYPoint1.y - VectorPointList[j-2].y);
					pt2.X = (float)(VectorPointList[j+2].x - temp_HYPoint3.x);
					pt2.Y = (float)(VectorPointList[j+2].y - temp_HYPoint3.y);
				}
				double cosc = (pt1.X * pt2.X + pt1.Y * pt2.Y) /
					(sqrt((double)pt1.X * pt1.X + pt1.Y * pt1.Y) * sqrt((double)pt2.X * pt2.X + pt2.Y * pt2.Y));

				if(cosc > 0 && angle < 30 && maxddi<=alphad3)// && angle < 30  maxddi<=alphad3
				{
					continue;
				}
				else
				{
					irde_HYPoint = temp_HYPoint2;
					irde_PointList.push_back(irde_HYPoint);
				}
			}

			CHYPoint& temp_HYPoint3 = VectorPointList[VectorPointList.size()-1];			
			irde_HYPoint = temp_HYPoint3;
			irde_PointList.push_back(irde_HYPoint);

			int iIrde_PointNum = irde_PointList.size();
			if(iIrde_PointNum<4)
			{
				for(int i=0; i<iIrde_PointNum; i++)
				{
					CHYPoint& temp_HYPoint = irde_PointList[i];
					ClearPointList.push_back(temp_HYPoint);
				}
			}
			else
			{
				CHYContour				d_ContourItem;
				CHYPoint				d_HYPoint;
				std::vector<CHYPoint>	d_PointList;

				double fk=0;
				double bk=0;
				double dis2p=0;
				int atep=0;		
				temp_HYPoint1 = irde_PointList[0];
				d_HYPoint=temp_HYPoint1;				
				ClearPointList.push_back(d_HYPoint);


				temp_HYPoint1 = irde_PointList[iIrde_PointNum-2];
				CHYPoint& temp_HYPoint2 = irde_PointList[0];
				temp_HYPoint3 = irde_PointList[1];
				CHYPoint& temp_HYPoint4 = irde_PointList[2];				

				fk=(temp_HYPoint2.x-temp_HYPoint1.x)*(temp_HYPoint4.x-temp_HYPoint3.x);
				bk=(temp_HYPoint2.y-temp_HYPoint1.y)*(temp_HYPoint4.y-temp_HYPoint3.y);
				dis2p=sqrt((double)((temp_HYPoint2.x-temp_HYPoint3.x)*(temp_HYPoint2.x-temp_HYPoint3.x)+(temp_HYPoint2.y-temp_HYPoint3.y)*(temp_HYPoint2.y-temp_HYPoint3.y)));

				if(dis2p<=alphad2)//(dis2p<=alphad2 && fk>=0) || (dis2p<=alphad2 && bk>=0)
				{
					atep=2;
				}
				else
				{atep=1;}

				for(int j=atep ; j<iIrde_PointNum-2 ; j++)
				{
					temp_HYPoint1 = irde_PointList[j-1];
					temp_HYPoint2 = irde_PointList[j];
					temp_HYPoint3 = irde_PointList[j+1];
					temp_HYPoint4 = irde_PointList[j+2];

					fk=(temp_HYPoint2.x-temp_HYPoint1.x)*(temp_HYPoint4.x-temp_HYPoint3.x);
					bk=(temp_HYPoint2.y-temp_HYPoint1.y)*(temp_HYPoint4.y-temp_HYPoint3.y);
					dis2p=sqrt((double)((temp_HYPoint2.x-temp_HYPoint3.x)*(temp_HYPoint2.x-temp_HYPoint3.x)+(temp_HYPoint2.y-temp_HYPoint3.y)*(temp_HYPoint2.y-temp_HYPoint3.y)));
					if((dis2p<=alphad2 && fk>0) || (dis2p<=alphad2 && bk>0))
					{
						continue;
					}
					else
					{
						d_HYPoint = temp_HYPoint2;
						ClearPointList.push_back(d_HYPoint);
					}
				}
				temp_HYPoint3 = irde_PointList[iIrde_PointNum-2];				
				d_HYPoint = temp_HYPoint3;
				ClearPointList.push_back(d_HYPoint);
				temp_HYPoint4 = irde_PointList[iIrde_PointNum-1];				
				d_HYPoint = temp_HYPoint4;
				ClearPointList.push_back(d_HYPoint);
			}
		}

	}// end of double  CHYImageToFont::Clearnoise()

	double CHYImageToFont::Calc_ANGLEThreePoint(const CHYPoint& StartPt,const CHYPoint& MidPt,const CHYPoint& EndPt)
	{
		double angle = 0;
		double pi = 3.1415926;
		PointF pt1;
		PointF pt2;

		pt1.X = (float)(StartPt.x-MidPt.x);
		pt1.Y = (float)(StartPt.y-MidPt.y);
		pt2.X = (float)(EndPt.x-MidPt.x);
		pt2.Y = (float)(EndPt.y-MidPt.y);
		if ((pt1.X==0 && pt1.Y==0) || (pt2.X==0 && pt2.Y==0))
		{
			angle = 1;
		}
		else
		{
			double cosc = (pt1.X * pt2.X + pt1.Y * pt2.Y) /
				(sqrt((double)pt1.X * pt1.X + pt1.Y * pt1.Y) * sqrt((double)pt2.X * pt2.X + pt2.Y * pt2.Y));

			if (cosc <= -1) cosc = -1;
			else if (cosc >= 1) cosc = 1;
			angle = acos(cosc)*180/pi;
		}

		return angle;

	}	// end of double CHYImageToFont::Calc_ANGLEThreePoint()

	bool CHYImageToFont::ContourOnContour(std::vector<CHYPoint>& contourA,std::vector<CHYPoint>& contourB)
	{
		float count = 0;
		int icontourANum = (int)contourA.size();
		for(int i=0; i<icontourANum;i++)
		{
			const CHYPoint& temp_HYPointA = contourA[i];			

			int icontourBNum = (int)contourB.size();
			for(int j=0;j<icontourBNum;j++)
			{
				const CHYPoint& temp_HYPointB = contourB[j];				
				if(temp_HYPointA.x==temp_HYPointB.x && temp_HYPointA.y==temp_HYPointB.y)
				{
					count++;
					break;
				}
			}
		}

		if(count > (int)(contourB.size()/2))
		{
			return true;
		}

		return false;

	}	// end of bool CHYImageToFont::ContourOnContour()

	double CHYImageToFont::AVRDisofPoint(std::vector<CHYPoint>& contour)
	{
		double dis = 0;
		double avr = 0;
		PointF ptf;
		ptf.X = 0;
		ptf.Y = 0;

		int iCnturNum = (int)contour.size();
		for (int i=0; i<iCnturNum; i++)
		{
			CHYPoint& temp_HYPoint1 = contour[i];			
			ptf.X += temp_HYPoint1.x;
			ptf.Y += temp_HYPoint1.y;
		}
		ptf.X = ptf.X / iCnturNum;
		ptf.Y = ptf.Y / iCnturNum;

		int iContourNum = (int)contour.size();
		for(int i=0;i<iContourNum;i++)
		{
			const CHYPoint& temp_HYPoint1 = contour[i];			
			dis += sqrt((double)(temp_HYPoint1.x - ptf.X) * (temp_HYPoint1.x - ptf.X) +(temp_HYPoint1.y - ptf.Y) * (temp_HYPoint1.y - ptf.Y));
		}
		dis = (int)(dis/iCnturNum + 0.5f);

		return dis;

	}	// end of double CHYImageToFont::AVRDisofPoint()

	int CHYImageToFont::ImageCut(std::vector<CHYContour>& ContourItemLst,std::vector<CHYContour>& ImageCutContourItemLst,int iWidth, int iHeight,double dbDelta,bool& Leftcut,bool& Rightcut,bool& Upcut,bool& Belowcut,double avrdis)
	{
		double alphad2 = 2*dbDelta;
		double alphad3 = 3*dbDelta;
		int cutflag = 0;
		std::vector<CHYPoint>  VectorPointList;
		std::vector<CHYPoint>  ClearPointList;

		int iContourNum = (int)ContourItemLst.size();
		for(int i=0;i<iContourNum; i++)
		{
			CHYContour& temp_ContourItem = ContourItemLst[i];
			std::vector<CHYPoint>& temp_PointList = temp_ContourItem.vtHYPoints;

			VectorPointList.clear();
			ClearPointList.clear();

			Vectorize(temp_PointList,VectorPointList,iWidth,dbDelta);
			if(VectorPointList.size()<= 2)
			{
				continue;
			}
			if(AVRDisofPoint(VectorPointList) < avrdis && OrientationOfPolygon(VectorPointList))
			{
				continue;
			}
			ClearPointList.clear();
			Clearnoise(VectorPointList,ClearPointList,alphad2,alphad3);
			if(ClearPointList.size() <= 4)//LineLenth(&ClearPointList) < 100 ||
			{continue;}

			CHYContour& ImageContourItem = ImageCutContourItemLst[i];
			std::vector<CHYPoint>& ImagePointList = ImageContourItem.vtHYPoints;

			size_t stImagePointNum = ImagePointList.size();
			for(size_t j = 0;j<stImagePointNum ;j++)
			{
				const CHYPoint& temp_HYPoint = ImagePointList[j];				
				if(temp_HYPoint.x <= 1 || temp_HYPoint.x >= (iWidth - 2) ||
					temp_HYPoint.y <= 1 || temp_HYPoint.y >= (iHeight - 2))
				{
					if(temp_HYPoint.x <= 1)
						Leftcut = true;					
					if(temp_HYPoint.x >= (iWidth - 2))					
						Rightcut = true;					
					if(temp_HYPoint.y <= 1)					
						Belowcut = true;					
					if(temp_HYPoint.y >= (iHeight - 2))					
						Upcut = true;

					cutflag = 1;					
				}
			}
		}
		return cutflag;

	}	// end of int CHYImageToFont::ImageCut()

	//如果是逆时针则返回TRUE，如果是顺时针则返回false
	bool CHYImageToFont::OrientationOfPolygon(std::vector<CHYPoint>& contour)
	{
		CHYPoint Pti,Ptj,Ptk,Ptl;
		CHYPoint HYpt;

		int iPoint[4] = {0};
		int sortiPoint[4] = {0};
		int isort = 0;
		float toleft = 10000;
		float toright = -10000;
		float totop = -10000;
		float tobelow = 10000;

		int stPointNum = (int)contour.size();
		for (int j=0; j<stPointNum; j++)
		{
			const CHYPoint& temp_HYPoint1 = contour[j];			
			if (temp_HYPoint1.flag != POINT_FLG_CONTROL)
			{
				if (temp_HYPoint1.x<toleft)
				{
					iPoint[0] = j;
					toleft = (float)temp_HYPoint1.x;
				}
				if (temp_HYPoint1.x>toright)
				{
					iPoint[1] = j;
					toright = (float)temp_HYPoint1.x;
				}
				if (temp_HYPoint1.y<tobelow)
				{
					iPoint[2] = j;
					tobelow = (float)temp_HYPoint1.y;
				}
				if (temp_HYPoint1.y>totop)
				{
					iPoint[3] = j;
					totop = (float)temp_HYPoint1.y;
				}
			}
		}
		//对四个点的下标排序
		BubbleSort(iPoint, 4);
		sortiPoint[isort] = iPoint[0];
		isort++;
		for (int i = 1; i < 4; i++)
		{
			if (iPoint[i] != iPoint[i - 1])
			{
				sortiPoint[isort] = iPoint[i];
				isort++;
			}
		}
		if (isort == 2)
		{
			CHYPoint pt1, pt2;
			size_t cntPointNum = contour.size();

			if (sortiPoint[0] == 0)
			{
				const CHYPoint& temp_HYPoint1 = contour[0];				
				const CHYPoint& temp_HYPoint2 = contour[cntPointNum-1];

				if (temp_HYPoint1.x == temp_HYPoint2.x && temp_HYPoint1.y == temp_HYPoint2.y)
				{					
					HYpt = contour[cntPointNum-2];
					pt1 = HYpt;
					if (pt1.flag == POINT_FLG_CONTROL)
					{
						HYpt = contour[cntPointNum-3];
						pt1 = HYpt;
					}
				}
				else
				{
					HYpt = contour[cntPointNum-1];
					pt1 = HYpt;
					if (pt1.flag == POINT_FLG_CONTROL)
					{
						HYpt = contour[cntPointNum-2];
						pt1 = HYpt;
					}
				}
			}
			else
			{
				HYpt = contour[sortiPoint[0]-1];
				pt1 = HYpt;
				if (pt1.flag == POINT_FLG_CONTROL)
				{
					HYpt = contour[sortiPoint[0]-2];
					pt1 = HYpt;
				}
			}

			HYpt = contour[sortiPoint[0]];
			Pti = HYpt;

			if (sortiPoint[0] == cntPointNum-1)
			{
				const CHYPoint& temp_HYPoint1 = contour[0];				
				const CHYPoint& temp_HYPoint2 = contour[cntPointNum-1];

				if (temp_HYPoint1.x == temp_HYPoint2.x && temp_HYPoint1.y == temp_HYPoint2.y)
				{
					HYpt = contour[1];
					pt2 = HYpt;
					if (pt2.flag == POINT_FLG_CONTROL)
					{
						HYpt = contour[2];
						pt2 = HYpt;
					}
				}
				else
				{
					HYpt = contour[0];
					pt2 = HYpt;
					if (pt2.flag == POINT_FLG_CONTROL)
					{
						HYpt = contour[1];
						pt2 = HYpt;
					}
				}
			}
			else
			{
				HYpt = contour[sortiPoint[0]+1];
				pt2 = HYpt;
				if (pt2.flag == POINT_FLG_CONTROL)
				{
					HYpt = contour[sortiPoint[0]+2];
					pt2 = HYpt;
				}
			}

			PointF apt;
			PointF bpt;
			apt.X = (float)(Pti.x - pt1.x);
			apt.Y = (float)(Pti.y - pt1.y);
			bpt.X = (float)(pt2.x - Pti.x);
			bpt.Y = (float)(pt2.y - Pti.y);
			if ((apt.X * bpt.Y - bpt.X * apt.Y) > 0)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			HYpt = contour[sortiPoint[0]];
			Ptj = HYpt;
			HYpt = contour[sortiPoint[1]];
			Ptk = HYpt;
			HYpt = contour[sortiPoint[2]];
			Ptl = HYpt;

			PointF apt;
			PointF bpt;
			apt.X = (float)(Ptk.x - Ptj.x);
			apt.Y = (float)(Ptk.y - Ptj.y);
			bpt.X = (float)(Ptl.x - Ptk.x);
			bpt.Y = (float)(Ptl.y - Ptk.y);
			if ((apt.X * bpt.Y - bpt.X * apt.Y) > 0)
			{
				return true;
			}
			else
			{
				return false;
			}
		}

	}	// end of bool CHYImageToFont::OrientationOfPolygon()

	void CHYImageToFont::BubbleSort(int *R, int n)
	{
		//R（l..n)是待排序的文件，采用自下向上扫描，对R做冒泡排序
		int i, j;
		int R0;
		for (i = 0; i < n; i++)
		{
			//最多做n-1趟排序
			for (j = n - 1; j > i; j--) //对当前无序区R[i..n]自下向上扫描
				if (R[j] < R[j - 1])
				{
					//交换记录
					R0 = R[j]; //R[0]不是哨兵，仅做暂存单元
					R[j] = R[j - 1];
					R[j - 1] = R0;
				}
		} //endfor(外循环)

	}	// end of void CImgFontCodec::BubbleSort()


	//点列矢量化
	void CHYImageToFont::Vectorize(std::vector<CHYPoint>& APointList,std::vector<CHYPoint>& VectorPointList,int iWidth,double delta)
	{
		CHYPoint        p_HYPoint;		
		double          maxd = 0.0;
		double          dis = 0;
		int             e;

		//FilterPointList(APointList);

		int listsize=(int)APointList.size()-1;
		CHYPoint temp_HYPoint1 = APointList[0];
		CHYPoint temp_HYPoint2 = APointList[listsize];
		CHYPoint temp_HYPoint3 = APointList[listsize];

		p_HYPoint.x = temp_HYPoint1.x;
		p_HYPoint.y = iWidth - temp_HYPoint1.y;
		p_HYPoint.flag = temp_HYPoint1.flag;
		VectorPointList.push_back(p_HYPoint);

		int j=0;
		e=listsize;
		
		temp_HYPoint1.flag = 3;
		while(temp_HYPoint1.x != temp_HYPoint2.x || temp_HYPoint1.y != temp_HYPoint2.y || temp_HYPoint1.flag != temp_HYPoint2.flag)
		{
			maxd=0;	
			for(int k=j+1;k<e;k++)
			{
				CHYPoint& temp_HYPoint_m=APointList[k];
				//HY_POINT* temp_Point_m=temp_HYPoint_m->GetHY_POINT();			
				dis=dispointtoline(temp_HYPoint1,temp_HYPoint3,temp_HYPoint_m);
				if(dis>maxd)
				{
					maxd = dis;
				}
				if(dis>delta)
				{
					e=e-1;
					temp_HYPoint3 = APointList[e];					
					break;
				}
			}
			if(maxd <= delta)
			{
				p_HYPoint.x = temp_HYPoint3.x;
				p_HYPoint.y = iWidth - temp_HYPoint3.y;
				p_HYPoint.flag = temp_HYPoint3.flag;
				VectorPointList.push_back(p_HYPoint);

				temp_HYPoint1 = temp_HYPoint3;				
				temp_HYPoint3 = APointList[listsize];

				j=e;
				e=listsize;
			}
		}

	}// end of double  CHYImageToFont::Vectorize()

	//计算点到直线的距离
	double  CHYImageToFont::dispointtoline(CHYPoint& startpoint,CHYPoint& endpoint,CHYPoint& tagpoint)
	{
		double dis;
		double line;
		int A;
		int B;
		double temp;
		if(endpoint.x==startpoint.x && endpoint.y==startpoint.y)
		{
			dis=sqrt((double)((startpoint.x-tagpoint.x)*(startpoint.x-tagpoint.x)+(startpoint.y-tagpoint.y)*(startpoint.y-tagpoint.y)));
			return dis;
		}

		A=endpoint.x-startpoint.x;
		B=endpoint.y-startpoint.y;

		line=abs((double)(B*tagpoint.x-A*tagpoint.y+A*startpoint.y-B*startpoint.x));
		temp=(double)(A*A+B*B);
		dis=line/(sqrt(temp));

		return dis;

	}	// end of double  CImgFontCodec::dispointtoline()


	void CHYImageToFont::GetContour(char*	pMtrxData, int iWidth, int iHeight, std::vector<CHYContour>& ContourItemLst, std::vector<CHYContour>& ImageCutContourItemLst,int m_BezierN)
	{
		DWORD dwlen = iWidth*iHeight;
		BYTE *ImgData = new BYTE[dwlen];
		ZeroMemory(ImgData,dwlen);

		CHYContour	ContourItem;
		CHYContour	ImageCutContourItem;

		int i = 0, j = 0;
		int StartPoint[2]={1,0};

		bool findallcontour;
		findallcontour=false;
		while(!findallcontour)
		{			
			CHYPoint	HYPoint;

			ContourItem.SetDefault();
			std::vector<CHYPoint>&	vtHYPoints = ContourItem.vtHYPoints;

			CHYPoint				ImageCutPoint[4];
			CHYPoint				ImageCutHYPoint;

			std::vector<CHYPoint>	ImageCutlstPoint;

			ImageCutPoint[0].x = 10000;
			ImageCutPoint[1].x = -10000;
			ImageCutPoint[2].y = 10000;
			ImageCutPoint[3].y = -10000;

			BYTE col =0;
			BYTE colleft = 0;
			bool bFindStartPoint;
			bool bFindPoint;
			int	 CurrentPoint[2]={0,0};   
			int  Direction[8][2] = {{-1,1},{0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1},{-1,0}};
			int	 BeginDirect;

			int tempflag=-1;     //如果找到的起始点未被标记则tempflag=-1
			size_t	contournum	= 0;
			int		lstpointnum = 0;

			// 每一行从第二列开始遍历
			bFindStartPoint = false;
			for( i = StartPoint[1]; i<(iHeight - 1) && (!bFindStartPoint); i++)
			{
				for(j=1; j <(iWidth-1) && (!bFindStartPoint); j++)
				{
					col = pMtrxData[i*iWidth+j];
					colleft = pMtrxData[i*iWidth+j-1];

					if( col == 0 && colleft!=0 )
					{
						//判断此点是否被标记
						contournum=ContourItemLst.size();
						tempflag=-1;

						if(contournum>=1)
						{
							if(ImgData[i*iWidth+j] == 10)
							{ tempflag=1; }
							if(tempflag==-1)
							{
								bFindStartPoint = true;
								StartPoint[0] =  j;
								StartPoint[1] = i;
								HYPoint.x=j;
								HYPoint.y=i;								
								HYPoint.flag = POINT_FLG_ANCHOR;
								vtHYPoints.push_back(HYPoint);

								ImgData[i*iWidth+j] = 10;

								if(HYPoint.x<ImageCutPoint[0].x)
								{
									ImageCutPoint[0].x = HYPoint.x;
									ImageCutPoint[0].y = HYPoint.y;
								}
								if(HYPoint.x>ImageCutPoint[1].x)
								{
									ImageCutPoint[1].x = HYPoint.x;
									ImageCutPoint[1].y = HYPoint.y;
								}
								if(HYPoint.y < ImageCutPoint[2].y)
								{
									ImageCutPoint[2].x = HYPoint.x;
									ImageCutPoint[2].y = HYPoint.y;
								}
								if(HYPoint.y > ImageCutPoint[3].y)
								{
									ImageCutPoint[3].x = HYPoint.x;
									ImageCutPoint[3].y = HYPoint.y;
								}
							}
						}
						if(ContourItemLst.size()==0)
						{
							bFindStartPoint = true;
							StartPoint[0] =  j;
							StartPoint[1] = i;
							HYPoint.x=j;
							HYPoint.y=i;
							HYPoint.flag = POINT_FLG_ANCHOR;
							vtHYPoints.push_back(HYPoint);
							ImgData[i * iWidth + j] = 10;

							if(HYPoint.x < ImageCutPoint[0].x)
							{
								ImageCutPoint[0].x = HYPoint.x;
								ImageCutPoint[0].y = HYPoint.y;
							}
							if(HYPoint.x > ImageCutPoint[1].x)
							{
								ImageCutPoint[1].x = HYPoint.x;
								ImageCutPoint[1].y = HYPoint.y;
							}
							if(HYPoint.y < ImageCutPoint[2].y)
							{
								ImageCutPoint[2].x = HYPoint.x;
								ImageCutPoint[2].y = HYPoint.y;
							}
							if(HYPoint.y > ImageCutPoint[3].y)
							{
								ImageCutPoint[3].x = HYPoint.x;
								ImageCutPoint[3].y = HYPoint.y;
							}
						}
					}
				}
			}
			if(i==iHeight - 1 && j==iWidth - 1)
			{
				findallcontour=true;
				break;
			}
			int k=1;
			BeginDirect = 1;
			bFindStartPoint = false;
			CurrentPoint[0] = StartPoint[0];
			CurrentPoint[1] = StartPoint[1];
			BYTE col1 = 0;
			int  iIndex = 0;

			while(!bFindStartPoint)
			{
				bFindPoint = false;
				int num=0;

				while(!bFindPoint)
				{
					if(num>7)
					{
						bFindPoint = true;
						bFindStartPoint = true;
					}
					int x =	CurrentPoint[0]+Direction[BeginDirect][0];
					int y = CurrentPoint[1]+Direction[BeginDirect][1];

					if (y >= iHeight - 1 || x >= iWidth - 1)
					{	
						bFindStartPoint=true;
						break;
					}

					col1 = pMtrxData[ x + y *iWidth];
					//if(ImgData[ x + y *iWidth] != 10)
					if( col1== 0)//&& ImgData[ x + y *iWidth] != 10
					{
						bFindPoint = true;
						ImgData[ x + y *iWidth] = 10;
						CurrentPoint[0] = CurrentPoint[0] + Direction[BeginDirect][0];
						CurrentPoint[1] = CurrentPoint[1] + Direction[BeginDirect][1];
						if((CurrentPoint[0]==StartPoint[0]) && (CurrentPoint[1] == StartPoint[1]))
						{
							bFindStartPoint = true;
						}

						HYPoint.x=CurrentPoint[0];
						HYPoint.y=CurrentPoint[1];
						HYPoint.flag = POINT_FLG_ANCHOR;

						if(bFindStartPoint == true)
						{	
							HYPoint.flag = POINT_FLG_ANCHOR;
						}						
						vtHYPoints.push_back(HYPoint);

						BeginDirect--;
						if(BeginDirect == -1)
						{
							BeginDirect =7;
						}
						BeginDirect--;
						if(BeginDirect == -1)
						{
							BeginDirect = 7;
						}

						if(HYPoint.x<ImageCutPoint[0].x)
						{
							ImageCutPoint[0].x = HYPoint.x;
							ImageCutPoint[0].y = HYPoint.y;
						}
						if(HYPoint.x>ImageCutPoint[1].x)
						{
							ImageCutPoint[1].x = HYPoint.x;
							ImageCutPoint[1].y = HYPoint.y;
						}
						if(HYPoint.y<ImageCutPoint[2].y)
						{
							ImageCutPoint[2].x = HYPoint.x;
							ImageCutPoint[2].y = HYPoint.y;
						}
						if(HYPoint.y>ImageCutPoint[3].y)
						{
							ImageCutPoint[3].x = HYPoint.x;
							ImageCutPoint[3].y = HYPoint.y;
						}
					}
					else
					{
						BeginDirect++;
						num++;
						if(BeginDirect == 8)
						{
							BeginDirect = 0;
						}
					}
				}
			}

			ContourItemLst.push_back(ContourItem);
			for(int iImagecut=0;iImagecut<4;iImagecut++)
			{	
				ImageCutlstPoint.push_back(ImageCutHYPoint);
			}

			ImageCutContourItem.vtHYPoints = ImageCutlstPoint;
			ImageCutlstPoint.clear();

			ImageCutContourItemLst.push_back(ImageCutContourItem);
		}

		if (ImgData) delete[] ImgData;
		ImgData = NULL;

	}	// end of void CHYImageToFont::GetContour()	

	void CHYImageToFont::FindImgFile(char* strFoldername,char* extName,std::vector<CString>& vtFiles)
	{  
		CString m_cstrFileList="";   
		CFileFind tempFind;   
		BOOL bFound;		
		//判断是否成功找到文件  

		CString strFindName;
		strFindName.Format("%s\\%s",strFoldername,extName);
		bFound=tempFind.FindFile(strFindName);		//修改" "内内容给限定查找文件类型  

		CString strTmp;												//如果找到的是文件夹 存放文件夹路径  
		while(bFound)      //遍历所有文件  
		{   
			bFound=tempFind.FindNextFile(); //第一次执行FindNextFile是选择到第一个文件，以后执行为选择到下一个文件  
			if(!tempFind.IsDots())   
				continue; //如果找到的是返回上层的目录 则结束本次查找  
			if(tempFind.IsDirectory())   //找到的是文件夹，则遍历该文件夹下的文件  
			{   
				continue;
			}   
			else   
			{   
				strTmp=tempFind.GetFileName(); //保存文件名，包括后缀名
				vtFiles.push_back(strTmp);
			}   
		}   
		tempFind.Close();

		return;

	}  // end of void CHYImageToFont::FindBmpFile()

	/*
	char* CHYImageToFont::GetImageMtrxData(CxImage* pImg)
	{
		if (pImg==NULL) return NULL;

		int iWidth = pImg->GetWidth();
		int iHeight = pImg->GetHeight();
		int iMtrxEffWidth= pImg->GetEffWidth();

		int iDataSize = iHeight*iWidth;
		char* pMtrxData = new char[iDataSize];
		if (iWidth == iMtrxEffWidth)
		{
			CopyMemory(pMtrxData,(char*)pImg->GetBits(),iDataSize);		
		}
		else 
		{
			int iOffset1 = 0;
			int iOffset2 = 0;
			for (int i =0; i<iHeight; i++) 
			{
				iOffset1 = i*iWidth;
				iOffset2 = i* iMtrxEffWidth;
				CopyMemory(pMtrxData+iOffset1,(char*)pImg->GetBits()+iOffset2,iWidth);		
			}
		}

		return pMtrxData;

	}	// end of void	CHYImageToFont::GetImageMtrxData()
	*/
	/*
	CxImage*	CHYImageToFont::SquareImage(const char* ImgFile)
	{		
		CxImage Img;
		if (!Img.Load(ImgFile)) 
			return NULL;	

		int iWidth = Img.GetWidth();
		int iHeight = Img.GetHeight();
		int iMtrxEffWidth= Img.GetEffWidth();

		CxImage* Square = new CxImage();
		RGBQUAD rgb;
		rgb.rgbBlue = 255;
		rgb.rgbGreen = 255;
		rgb.rgbRed = 255;
		rgb.rgbReserved = 0;		
		if (iWidth>iHeight)
		{
			Square->Create(iWidth,iWidth,32);			
			Square->SetRectColor(0, 0, iWidth, iWidth, rgb);

			int iOffSetY = ::HY_RealRount((iWidth-iHeight)/2);
			Square->Mix(Img,CxImage::OpAnd,0,-iOffSetY);

		}
		else 
		{
			Square->Create(iHeight,iHeight,8);			
			Square->SetRectColor(0, 0, iHeight, iHeight, rgb);

			int iOffSetX = ::HY_RealRount((iHeight-iWidth)/2);
			Square->Mix(Img,CxImage::OpAnd,iOffSetX,0);
		}

		//DeleteFile("d:\\Test.jpg");
		//Square->Save("d:\\Test.jpg",CXIMAGE_SUPPORT_JPG);		

		return Square;

	}	// end of CxImage*	CHYImageToFont::SquareImage()
	*/

	void CHYImageToFont::AdjustMetrics(int BsLine, float fEMScale, HYFONTCODEC::CHYGlyph & Glyph)
	{
		size_t stCuntNum = Glyph.vtContour.size();
		for (size_t j = 0; j != stCuntNum; j++)
		{
			CHYContour& cntur = Glyph.vtContour[j];

			size_t stPoinNum = cntur.vtHYPoints.size();
			for (size_t x = 0; x != stPoinNum; x++)
			{
				CHYPoint& pt = cntur.vtHYPoints[x];
				pt.x *= fEMScale;
				pt.y *= fEMScale;
				pt.y -= BsLine;
			}
		}
	
	}	// end of void CHYImageToFont::AdjustMetrics()

	void CHYImageToFont::AdjustMetrics(int BsLine,float fEMScale ,HYFONTCODEC::CHYFontCodec& FntEncode)
	{
		size_t stGlyphNum = FntEncode.m_vtHYGlyphs.size();
		for (size_t i = 0; i != stGlyphNum; i++)
		{
			CHYGlyph& Glyph = FntEncode.m_vtHYGlyphs[i];

			size_t stCuntNum = Glyph.vtContour.size();
			for (size_t j=0; j != stCuntNum; j++)
			{
				CHYContour& cntur = Glyph.vtContour[j];

				size_t stPoinNum = cntur.vtHYPoints.size();
				for (size_t x = 0; x != stPoinNum; x++)
				{
					CHYPoint& pt = cntur.vtHYPoints[x];
					pt.x *= fEMScale;
					pt.y *= fEMScale;
					pt.y -= BsLine;
				}
			}
		}

	}	// end of void CHYImageToFont::AlignBaseLine()

}
