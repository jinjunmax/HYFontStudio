// HYIkCodec.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "HYIkCodec.h"

namespace HYFONTCODEC
{
	CHYIkCodec::CHYIkCodec()
	{
		m_pIKData			= NULL;
		m_lIkDataLen		= 0;
		m_ulOffset			= 0;
		m_lRealHeaderLen	= 0;
	
	}	// end of CHYIkCodec::CHYIkCodec()

	CHYIkCodec::~CHYIkCodec()
	{
		if (m_pIKData)
		{
			delete[] m_pIKData;
		}
		m_lIkDataLen	= 0;

	}	// end of CHYIkCodec::~CHYIkCodec()

	void CHYIkCodec::SetDefault()
	{	
		if (m_pIKData)
		{
			delete[] m_pIKData;
		}

		m_pIKData			= NULL;
		m_lIkDataLen		= 0;
		m_ulOffset			= 0;
		m_lRealHeaderLen	= 0;

		m_IKFont.SetDefault();
	
	}	// end of void	CHYIkCodec::SetDefault()

	int CHYIkCodec::Decode(char* pIKFile)
	{
		if(pIKFile==NULL) return HY_ERR_POINT_NULL;

		SetDefault();

		struct _stat info;  
		_stat(pIKFile, &info);
		m_lIkDataLen = info.st_size;

		FILE* pIKF = fopen(pIKFile,"rb");
		if (pIKF==NULL)  return HY_ERR_FILE_READ;

		m_pIKData = new char[m_lIkDataLen];
		fread(m_pIKData,m_lIkDataLen,1,pIKF);
		fclose(pIKF);

		CheckDataFormat();

		DecodeHeader();
		DecodeCharacter();

		return HY_NOERROR;

	}	// end of int CHYIkCodec::Decode()

	int	 CHYIkCodec::Encode(char* pIKFile)
	{
		FILE* pDstFile = NULL;
		pDstFile = fopen(pIKFile,"wb");
		if (pIKFile == 0)
		{			
			return HY_ERR_FILE_WRITE;
		}
		
		size_t szCharact = m_IKFont.vtCharacter.size();
		if(szCharact>HY_MAX_NUMBER) return IK_ERR_CHAR_OVERLOW;
		for (size_t i=0; i<szCharact; i++)
		{
			CIKCharacter& Character = m_IKFont.vtCharacter[i];
			CountCharacterLength(Character);
		}
		CountIKHeader(m_IKFont);		

		EncodeHeader(pDstFile,m_IKFont);
		EncodeCharacter(pDstFile,m_IKFont);

		fflush(pDstFile);
		fclose(pDstFile);	

		return HY_NOERROR;

	}	// end of int	 CHYIkCodec::Encode()

	int	 CHYIkCodec::Encode(char* pIKFile, CIKFont& IkFont)
	{
		size_t szCharact = IkFont.vtCharacter.size();
		if(szCharact>HY_MAX_NUMBER) return IK_ERR_CHAR_OVERLOW;		

		for (size_t i=0; i<szCharact; i++)
		{
			CIKCharacter& Character = IkFont.vtCharacter[i];
			CountCharacterLength(Character);
		}

		FILE* pDstFile = NULL;
		pDstFile = fopen(pIKFile,"wb");
		if (pIKFile == 0)
		{			
			return HY_ERR_FILE_WRITE;
		}

		strcpy_s(IkFont.Header.Name.FileName,IK_FILENAME_LENG,HY_GetFullNameFromPath(pIKFile).data());
		CountIKHeader(IkFont);		

		EncodeHeader(pDstFile,IkFont);
		EncodeCharacter(pDstFile,IkFont);

		fflush(pDstFile);
		fclose(pDstFile);	

		return HY_NOERROR;

	}	// end of int  CHYIkCodec::Encode()

	int	CHYIkCodec::DecodeHeader()
	{
		CIKHeader& Header = m_IKFont.Header;

		// length of header
		memcpy(&Header.usLength, m_pIKData+m_ulOffset, HY_SHORT_SZ);
		m_ulOffset += HY_SHORT_SZ;

		// name section
		// lenght of name seciton
		memcpy(&Header.Name.sNameLength, m_pIKData+m_ulOffset, HY_SHORT_SZ);
		m_ulOffset += HY_SHORT_SZ;
		// nubmer on URW list
		memcpy(&Header.Name.usURWLstNumber, m_pIKData+m_ulOffset, HY_SHORT_SZ);
		m_ulOffset += HY_SHORT_SZ;
		// File name
		memcpy(&Header.Name.FileName, m_pIKData+m_ulOffset, IK_FILENAME_LENG);
		m_ulOffset += IK_FILENAME_LENG;
		// Font name
		memcpy(&Header.Name.FontName, m_pIKData+m_ulOffset, IK_FONTNAME_LENG);
		m_ulOffset += IK_FONTNAME_LENG;
		// Data format
		memcpy(&Header.Name.sDataFormat, m_pIKData+m_ulOffset, HY_SHORT_SZ);
		m_ulOffset += HY_SHORT_SZ;
		// Production date
		memcpy(&Header.Name.sProductDate, m_pIKData+m_ulOffset, HY_SHORT_SZ*3);
		m_ulOffset += HY_SHORT_SZ*3;
		// date of change 
		memcpy(&Header.Name.sChangeDate, m_pIKData+m_ulOffset, HY_SHORT_SZ*3);
		m_ulOffset += HY_SHORT_SZ*3;

		// font information
		// length of font information
		memcpy(&Header.FontInf.sLength, m_pIKData+m_ulOffset, HY_SHORT_SZ);
		m_ulOffset += HY_SHORT_SZ;
		// indicator for font
		memcpy(&Header.FontInf.sIndicator, m_pIKData+m_ulOffset, HY_SHORT_SZ);
		m_ulOffset += HY_SHORT_SZ;
		// number of characters
		memcpy(&Header.FontInf.sCharNums, m_pIKData+m_ulOffset, HY_SHORT_SZ);
		m_ulOffset += HY_SHORT_SZ;
		// cap height
		memcpy(&Header.FontInf.sCapHeight, m_pIKData+m_ulOffset, HY_SHORT_SZ);
		m_ulOffset += HY_SHORT_SZ;
		// Body size
		memcpy(&Header.FontInf.sBodySize, m_pIKData+m_ulOffset, HY_SHORT_SZ);
		m_ulOffset += HY_SHORT_SZ;
		// x-height
		memcpy(&Header.FontInf.sX_Height, m_pIKData+m_ulOffset, HY_SHORT_SZ);
		m_ulOffset += HY_SHORT_SZ;
		// Distance base line
		memcpy(&Header.FontInf.sBaseLine, m_pIKData+m_ulOffset, HY_SHORT_SZ);
		m_ulOffset += HY_SHORT_SZ;
		// Text line distance for setting
		memcpy(&Header.FontInf.sTextLine, m_pIKData+m_ulOffset, HY_SHORT_SZ);
		m_ulOffset += HY_SHORT_SZ;
		// stem thickness
		memcpy(&Header.FontInf.sStemThickness, m_pIKData+m_ulOffset, HY_SHORT_SZ);
		m_ulOffset += HY_SHORT_SZ;
		// angle of italisation
		memcpy(&Header.FontInf.sAngle, m_pIKData+m_ulOffset, HY_SHORT_SZ);
		m_ulOffset += HY_SHORT_SZ;
		// optimum point size for setting
		memcpy(&Header.FontInf.sOptimum, m_pIKData+m_ulOffset, HY_SHORT_SZ);
		m_ulOffset += HY_SHORT_SZ;
		// average character width
		memcpy(&Header.FontInf.sAverageWidth, m_pIKData+m_ulOffset, HY_SHORT_SZ);
		m_ulOffset += HY_SHORT_SZ;
		// hierarchy section	this IK format  == 1 
		memcpy(&Header.sHierarchy, m_pIKData+m_ulOffset, HY_SHORT_SZ);
		m_ulOffset += HY_SHORT_SZ;

		// character index
		// leghth of character index in words
		memcpy(&Header.CharIndexInf.usCharIndexLength, m_pIKData+m_ulOffset, HY_SHORT_SZ);
		m_ulOffset += HY_SHORT_SZ;
		// Point to last written record(= length of font in records)
		memcpy(&Header.CharIndexInf.usLastReocrd, m_pIKData+m_ulOffset, HY_SHORT_SZ);
		m_ulOffset += HY_SHORT_SZ;
		// Point to last written word of last record 
		memcpy(&Header.CharIndexInf.usLastWord, m_pIKData+m_ulOffset, HY_SHORT_SZ);
		m_ulOffset += HY_SHORT_SZ;		
		for (int i = 0; i < Header.FontInf.sCharNums; i++)
		{
			CIKCharIndex	CharIndex;
			memcpy(&CharIndex.sCharNumber, m_pIKData+m_ulOffset, HY_SHORT_SZ);
			m_ulOffset += HY_SHORT_SZ;
			memcpy(&CharIndex.sStartRecord, m_pIKData+m_ulOffset, HY_SHORT_SZ);
			m_ulOffset += HY_SHORT_SZ;
			memcpy(&CharIndex.sWordPoint, m_pIKData+m_ulOffset, HY_SHORT_SZ);
			m_ulOffset += HY_SHORT_SZ;
			Header.CharIndexInf.vtCharIndex.push_back(CharIndex);
		}

		return HY_NOERROR;

	}	// end of int	CHYIkCodec::DecodeHeader()

	int	CHYIkCodec::DecodeCharacter()
	{
		const CIKHeader& Header = m_IKFont.Header;
		for (int i=0; i<Header.FontInf.sCharNums; i++)
		{
			const CIKCharIndex&	CharIndex = Header.CharIndexInf.vtCharIndex[i];

			// 因为IK中索引以1开头，所以需要-1才能变成以0开头
			m_ulOffset = ((CharIndex.sStartRecord-1)*HY_IK_RECORD+CharIndex.sWordPoint-1)<<1;

			CIKCharacter	Character;
			memcpy(&Character.sRecords, m_pIKData+m_ulOffset, HY_SHORT_SZ);
			m_ulOffset += HY_SHORT_SZ;
			memcpy(&Character.sWords, m_pIKData+m_ulOffset, HY_SHORT_SZ);
			m_ulOffset += HY_SHORT_SZ;

			// name seciton
			memcpy(&Character.CharNameInf.sNameLength, m_pIKData+m_ulOffset, HY_SHORT_SZ);
			m_ulOffset += HY_SHORT_SZ;			
			memcpy(&Character.CharNameInf.sCharNumber, m_pIKData+m_ulOffset, HY_SHORT_SZ);
			m_ulOffset += HY_SHORT_SZ;
			memcpy(&Character.CharNameInf.Reserved, m_pIKData+m_ulOffset, HY_SHORT_SZ);
			m_ulOffset += HY_SHORT_SZ;
			// setting information
			memcpy(&Character.CharSet.sSetLength, m_pIKData+m_ulOffset, HY_SHORT_SZ);
			m_ulOffset += HY_SHORT_SZ;
			memcpy(&Character.CharSet.sCharacterType, m_pIKData+m_ulOffset, HY_SHORT_SZ);
			m_ulOffset += HY_SHORT_SZ;
			memcpy(&Character.CharSet.sDigitiNumber, m_pIKData+m_ulOffset, HY_SHORT_SZ);
			m_ulOffset += HY_SHORT_SZ;
			memcpy(&Character.CharSet.sTotalWidth, m_pIKData+m_ulOffset, HY_SHORT_SZ);
			m_ulOffset += HY_SHORT_SZ;
			memcpy(&Character.CharSet.sLeftBearing, m_pIKData+m_ulOffset, HY_SHORT_SZ);
			m_ulOffset += HY_SHORT_SZ;
			memcpy(&Character.CharSet.sWidth, m_pIKData+m_ulOffset, HY_SHORT_SZ);
			m_ulOffset += HY_SHORT_SZ;
			memcpy(&Character.CharSet.sRightBearing, m_pIKData+m_ulOffset, HY_SHORT_SZ);
			m_ulOffset += HY_SHORT_SZ;
			memcpy(&Character.CharSet.sXMin, m_pIKData+m_ulOffset, HY_SHORT_SZ);
			m_ulOffset += HY_SHORT_SZ;
			memcpy(&Character.CharSet.sXMax, m_pIKData+m_ulOffset, HY_SHORT_SZ);
			m_ulOffset += HY_SHORT_SZ;
			memcpy(&Character.CharSet.sYMin, m_pIKData+m_ulOffset, HY_SHORT_SZ);
			m_ulOffset += HY_SHORT_SZ;
			memcpy(&Character.CharSet.sYMax, m_pIKData+m_ulOffset, HY_SHORT_SZ);
			m_ulOffset += HY_SHORT_SZ;
			memcpy(&Character.CharSet.sUnit, m_pIKData+m_ulOffset, HY_SHORT_SZ);
			m_ulOffset += HY_SHORT_SZ;

			// contour index
			unsigned long ulTmp = m_ulOffset;
			memcpy(&Character.ContourInfo.sRecords, m_pIKData+ulTmp, HY_SHORT_SZ);
			ulTmp += HY_SHORT_SZ;
			memcpy(&Character.ContourInfo.sWords, m_pIKData+ulTmp, HY_SHORT_SZ);
			ulTmp += HY_SHORT_SZ;

			unsigned long ulCnturIndxEnd = (Character.ContourInfo.sRecords*HY_IK_RECORD+Character.ContourInfo.sWords)*2 + m_ulOffset;
			m_ulOffset = ulTmp;

			while(m_ulOffset<ulCnturIndxEnd)
			{
				CIKContourIndex  ContourIndex;
				memcpy(&ContourIndex.sRecordPoint, m_pIKData+m_ulOffset, HY_SHORT_SZ);
				m_ulOffset += HY_SHORT_SZ;
				memcpy(&ContourIndex.sWordPoint, m_pIKData+m_ulOffset, HY_SHORT_SZ);
				m_ulOffset += HY_SHORT_SZ;
				memcpy(&ContourIndex.sRotation, m_pIKData+m_ulOffset, HY_SHORT_SZ);
				m_ulOffset += HY_SHORT_SZ;
				memcpy(&ContourIndex.sHierarchy, m_pIKData+m_ulOffset, HY_SHORT_SZ);
				m_ulOffset += HY_SHORT_SZ;
				memcpy(&ContourIndex.sColor, m_pIKData+m_ulOffset, HY_SHORT_SZ);
				m_ulOffset += HY_SHORT_SZ;
				memcpy(&ContourIndex.sNumber, m_pIKData+m_ulOffset, HY_SHORT_SZ);
				m_ulOffset += HY_SHORT_SZ;
				Character.ContourInfo.vtContourIndex.push_back(ContourIndex);
			}

			unsigned long ulImageStart = m_ulOffset;

			//image length 
			short sImageRecord = 0;
			short sImageWord = 0;
			memcpy(&sImageRecord, m_pIKData+m_ulOffset, HY_SHORT_SZ);
			m_ulOffset += HY_SHORT_SZ;
			memcpy(&sImageWord, m_pIKData+m_ulOffset, HY_SHORT_SZ);
			m_ulOffset += HY_SHORT_SZ;
			
			size_t cnturSZ = Character.ContourInfo.vtContourIndex.size();
			if (cnturSZ>0)
			{
				for (size_t j=0; j<cnturSZ-1; j++)
				{
					CIKContourIndex&  ContourIndex = Character.ContourInfo.vtContourIndex[j];
					m_ulOffset = (ContourIndex.sRecordPoint*HY_IK_RECORD+ContourIndex.sWordPoint-1)*2+ulImageStart;				

					CIKContourIndex&  Next = Character.ContourInfo.vtContourIndex[j+1];
					unsigned long ulNextCnur = (Next.sRecordPoint*HY_IK_RECORD+Next.sWordPoint-1)*2+ulImageStart;

					short s1=0, s2=0;
					while (m_ulOffset<ulNextCnur)
					{
						s1=s2=0;
						memcpy(&s1, m_pIKData+m_ulOffset, HY_SHORT_SZ);
						m_ulOffset += HY_SHORT_SZ;
						memcpy(&s2, m_pIKData+m_ulOffset, HY_SHORT_SZ);
						m_ulOffset += HY_SHORT_SZ;

						CIKPoint pt;
						DataToPoint(s1,s2,pt,Character.CharSet);
						ContourIndex.vtIKPoint.push_back(pt);				
					}
				}

				unsigned long ulImageEnd = (sImageRecord*HY_IK_RECORD+sImageWord)*2+ulImageStart;
				CIKContourIndex&  ContourIndex = Character.ContourInfo.vtContourIndex[Character.ContourInfo.vtContourIndex.size()-1];
				m_ulOffset = (ContourIndex.sRecordPoint*HY_IK_RECORD+ContourIndex.sWordPoint-1)*2+ulImageStart;

				short s1=0, s2=0;
				while (m_ulOffset<ulImageEnd)
				{
					s1=s2=0;
					memcpy(&s1, m_pIKData+m_ulOffset, HY_SHORT_SZ);
					m_ulOffset += HY_SHORT_SZ;
					memcpy(&s2, m_pIKData+m_ulOffset, HY_SHORT_SZ);
					m_ulOffset += HY_SHORT_SZ;

					CIKPoint pt;
					DataToPoint(s1,s2,pt,Character.CharSet);
					ContourIndex.vtIKPoint.push_back(pt);				
				}
			}			
			m_IKFont.vtCharacter.push_back(Character);
		}

		return HY_NOERROR;

	}	// end of int CHYIkCodec::DecodeCharacter()

	int	CHYIkCodec::PointToData(const CIKPoint& pt, short& s1, short& s2, const CIKCharSet& charSet)
	{
		// start
		if (pt.t == IK_PT_FLG_START)
		{			
			s1 = -(pt.x+1);
			s2 = pt.y-charSet.sYMin+1;
		}

		// CORNER
		if (pt.t == IK_PT_FLG_CORNER)
		{		
			s1 = -(pt.x+1);			
			s2 = -(pt.y-charSet.sYMin+1);
		}

		// CURVE
		if (pt.t == IK_PT_FLG_CURVE)
		{			
			s1 = pt.x+1;
			s2 = pt.y-charSet.sYMin+1;
		}

		// TANGENT
		if (pt.t  == IK_PT_FLG_TANGENT)
		{			
			s1 = pt.x+1;
			s2 = -(pt.y-charSet.sYMin+1);
		}

		return HY_NOERROR;

	}	// end of int CHYIkCodec::PointToData()

	int	CHYIkCodec::DataToPoint(short s1, short s2, CIKPoint& pt, const CIKCharSet& charSet)
	{
		// k == 12  start
		if (s1<0 && s2>0)
		{
			pt.t = IK_PT_FLG_START;
			pt.x = -1-s1;
			pt.y = s2+charSet.sYMin-1;
		}

		// k == 13 CORNER
		if (s1<0 && s2< 0)
		{
			pt.t = IK_PT_FLG_CORNER;
			pt.x = -1-s1;		
			pt.y = -s2+charSet.sYMin-1;
		}

		// k == 14  CURVE
		if (s1>0 && s2>0)
		{
			pt.t = IK_PT_FLG_CURVE;
			pt.x = s1-1;
			pt.y = s2+charSet.sYMin-1;
		}

		// k == 15 TANGENT
		if (s1>0 && s2<0)
		{
			pt.t  = IK_PT_FLG_TANGENT;
			pt.x = s1-1;
			pt.y = -s2+charSet.sYMin-1;
		}

		return HY_NOERROR;

	}	// end of int CHYIkCodec::DataToPoint()	

	int	CHYIkCodec::CountCharacterLength(CIKCharacter& Char)
	{
		unsigned short& 		ImgLen	= Char.ImgLen;
		ImgLen	= 0;

		std::vector<long>	vtCuntrImgOffset;
		size_t				szCnutr = Char.ContourInfo.vtContourIndex.size();
		ImgLen+=2;		
		vtCuntrImgOffset.push_back(2);
		for (size_t i=0; i<szCnutr; i++)
		{			
			CIKContourIndex& cntur = Char.ContourInfo.vtContourIndex[i];
			ImgLen+=cntur.vtIKPoint.size()*2;
			vtCuntrImgOffset.push_back(ImgLen);
		}		

		long	CnutrIndexLen = 2;
		for (size_t i=0; i<szCnutr; i++)
		{
			CIKContourIndex& cnturIndx = Char.ContourInfo.vtContourIndex[i];			
			long ImgOffst = vtCuntrImgOffset[i];
			cnturIndx.sRecordPoint = (short)(ImgOffst/HY_IK_RECORD);
			cnturIndx.sWordPoint = (short)(ImgOffst%HY_IK_RECORD)+1;
			CnutrIndexLen+=6;
		}

		Char.ContourInfo.sRecords = (short)(CnutrIndexLen/HY_IK_RECORD);
		Char.ContourInfo.sWords = (short)(CnutrIndexLen%HY_IK_RECORD);

		// char name section == 3 short 
		short sNameLen = 3;	
		// char set  information == 12 short
		short sSetLen = 12;
		
		long lCharLen = 2+sNameLen+sSetLen+CnutrIndexLen+ImgLen;	//2+N1+N2+N3+N4
		Char.sRecords = (short)(lCharLen/HY_IK_RECORD);
		Char.sWords = (short)(lCharLen%HY_IK_RECORD);

		return HY_NOERROR;

	}	// end of int int CHYIkCodec::CountCharacterLength()	

	int CHYIkCodec::CountIKHeader(CIKFont& ikFont)
	{		
		unsigned short 	sCharsIndexLen		= 3 + (unsigned short)ikFont.vtCharacter.size()*3;		
		ikFont.Header.Name.sNameLength		= 55;
		ikFont.Header.FontInf.sLength		= 12;
		ikFont.Header.sHierarchy			= 1;
		ikFont.Header.usLength			=	1+ikFont.Header.Name.sNameLength
											+ikFont.Header.FontInf.sLength
											+ikFont.Header.sHierarchy+sCharsIndexLen;		// 1+I+J+L+M

		
		ikFont.Header.CharIndexInf.usCharIndexLength = sCharsIndexLen;

		//long	lCharOffset = ikFont.Header.usLength;
		m_lRealHeaderLen = ikFont.Header.usLength+(2048-ikFont.Header.usLength%2048);
		long	lCharOffset = m_lRealHeaderLen;
		size_t szChar = ikFont.vtCharacter.size();		
		for (size_t i=0; i<szChar; i++)
		{
			CIKCharIndex& CharIndx = ikFont.Header.CharIndexInf.vtCharIndex[i];
			CIKCharacter& Char = ikFont.vtCharacter[i];

			CharIndx.sStartRecord = (short)(lCharOffset/HY_IK_RECORD+1);
			CharIndx.sWordPoint = (short)(lCharOffset%HY_IK_RECORD+1);

			lCharOffset+= Char.sRecords*HY_IK_RECORD;
			lCharOffset+= Char.sWords;
		}

		ikFont.Header.CharIndexInf.usLastReocrd = (short)(lCharOffset/HY_IK_RECORD+1);
		ikFont.Header.CharIndexInf.usLastWord = (short)(lCharOffset%HY_IK_RECORD);
		ikFont.Header.FontInf.sCharNums = (short)ikFont.vtCharacter.size();

		return HY_NOERROR;

	}	// end of int CHYIkCodec::CountIKHeader()

	int CHYIkCodec::EncodeHeader(FILE* pDst,CIKFont& ikFont)
	{		

		//length of font header in words
		fseek(pDst,0,SEEK_SET);
		fwrite(&ikFont.Header.usLength,2,1,pDst);

		// name secton
		// length of name section in words
		fwrite(&ikFont.Header.Name.sNameLength,2,1,pDst);
		// number on URW list 
		fwrite(&ikFont.Header.Name.usURWLstNumber,2,1,pDst);
		// file name
		fwrite(&ikFont.Header.Name.FileName,IK_FILENAME_LENG,1,pDst);
		// font name
		fwrite(&ikFont.Header.Name.FontName,IK_FONTNAME_LENG,1,pDst);
		// format ident of data
		fwrite(&ikFont.Header.Name.sDataFormat,2,1,pDst);
		// production date
		fwrite(&ikFont.Header.Name.sProductDate[0],2,1,pDst);
		fwrite(&ikFont.Header.Name.sProductDate[1],2,1,pDst);
		fwrite(&ikFont.Header.Name.sProductDate[2],2,1,pDst);
		// Data of last change
		fwrite(&ikFont.Header.Name.sChangeDate[0],2,1,pDst);
		fwrite(&ikFont.Header.Name.sChangeDate[1],2,1,pDst);
		fwrite(&ikFont.Header.Name.sChangeDate[2],2,1,pDst);

		// font  information
		// length of font data in word
		fwrite(&ikFont.Header.FontInf.sLength,2,1,pDst);
		// ident of font
		fwrite(&ikFont.Header.FontInf.sIndicator,2,1,pDst);
		//number of characters
		fwrite(&ikFont.Header.FontInf.sCharNums,2,1,pDst);
		//cap height 
		fwrite(&ikFont.Header.FontInf.sCapHeight,2,1,pDst);
		//body size
		fwrite(&ikFont.Header.FontInf.sBodySize,2,1,pDst);
		//x-height
		fwrite(&ikFont.Header.FontInf.sX_Height,2,1,pDst);
		//Distance of base line from
		fwrite(&ikFont.Header.FontInf.sBaseLine,2,1,pDst);
		//line distance for compositon
		fwrite(&ikFont.Header.FontInf.sTextLine,2,1,pDst);
		//stroke thickness
		fwrite(&ikFont.Header.FontInf.sStemThickness,2,1,pDst);
		//italic angle
		fwrite(&ikFont.Header.FontInf.sAngle,2,1,pDst);
		//Optimum size for compositon in didot points
		fwrite(&ikFont.Header.FontInf.sOptimum,2,1,pDst);
		//average character width
		fwrite(&ikFont.Header.FontInf.sAverageWidth,2,1,pDst);

		// hierarchy section
		fwrite(&ikFont.Header.sHierarchy,2,1,pDst);

		//character index
		//length of character index
		fwrite(&ikFont.Header.CharIndexInf.usCharIndexLength,2,1,pDst);
		//pointer to last written record
		fwrite(&ikFont.Header.CharIndexInf.usLastReocrd,2,1,pDst);
		//potiner to last written word of last record
		fwrite(&ikFont.Header.CharIndexInf.usLastWord,2,1,pDst);

		size_t szChars = ikFont.Header.CharIndexInf.vtCharIndex.size();
		for (size_t i=0; i<szChars; i++)
		{
			CIKCharIndex& ikcharIndx = ikFont.Header.CharIndexInf.vtCharIndex[i];

			//character number
			fwrite(&ikcharIndx.sCharNumber,2,1,pDst);
			//pointer to last written record
			fwrite(&ikcharIndx.sStartRecord,2,1,pDst);
			//pointer to last written word
			fwrite(&ikcharIndx.sWordPoint,2,1,pDst);
		}

		long lReservedSize =  (2048-ikFont.Header.usLength%2048)*2;
		if (lReservedSize>0)
		{
			char* pReserved = new char[lReservedSize];
			memset(pReserved,0,lReservedSize);
			fwrite(pReserved,lReservedSize,1,pDst);
			delete[] pReserved; 
		}	
		

		return HY_NOERROR;

	}	// end of int CHYIkCodec::EncodeHeader()

	int	CHYIkCodec::EncodeCharacter(FILE* pDst,CIKFont& ikFont)
	{
		size_t szChars = ikFont.vtCharacter.size();
		for (size_t i=0; i<szChars; i++)
		{
			CIKCharacter& ikChar = ikFont.vtCharacter[i];

			// length of first character			
			fwrite(&ikChar.sRecords,2,1,pDst);
			fwrite(&ikChar.sWords,2,1,pDst);

			// name section
			// leogth of name section in words
			fwrite(&ikChar.CharNameInf.sNameLength,2,1,pDst);
			// Character number
			fwrite(&ikChar.CharNameInf.sCharNumber,2,1,pDst);
			// reserved
			fwrite(&ikChar.CharNameInf.Reserved,2,1,pDst);

			// settting information
			// length of settin information
			fwrite(&ikChar.CharSet.sSetLength,2,1,pDst);
			// Character Type			
			fwrite(&ikChar.CharSet.sCharacterType,2,1,pDst);
			// Number of digitizations
			fwrite(&ikChar.CharSet.sDigitiNumber,2,1,pDst);
			// total width
			fwrite(&ikChar.CharSet.sTotalWidth,2,1,pDst);
			// left side bearing
			fwrite(&ikChar.CharSet.sLeftBearing,2,1,pDst);
			// width
			fwrite(&ikChar.CharSet.sWidth,2,1,pDst);
			// Right side bearing
			fwrite(&ikChar.CharSet.sRightBearing,2,1,pDst);
			// X minimum
			fwrite(&ikChar.CharSet.sXMin,2,1,pDst);
			// x maximum
			fwrite(&ikChar.CharSet.sXMax,2,1,pDst);
			// y minimum
			fwrite(&ikChar.CharSet.sYMin,2,1,pDst);
			// y maximum
			fwrite(&ikChar.CharSet.sYMax,2,1,pDst);
			// unit size
			fwrite(&ikChar.CharSet.sUnit,2,1,pDst);

			// Contour index
			// length of contour index
			fwrite(&ikChar.ContourInfo.sRecords,2,1,pDst);
			fwrite(&ikChar.ContourInfo.sWords,2,1,pDst);

			size_t szCntr = ikChar.ContourInfo.vtContourIndex.size();
			for(size_t j=0; j<szCntr; j++)
			{
				CIKContourIndex& cntrIndx =  ikChar.ContourInfo.vtContourIndex[j];

				// record pointer
				fwrite(&cntrIndx.sRecordPoint,2,1,pDst);
				// word pointer
				fwrite(&cntrIndx.sWordPoint,2,1,pDst);
				// sense of rotation of first contour
				fwrite(&cntrIndx.sRotation,2,1,pDst);
				// identifier for nesting hierarchy
				fwrite(&cntrIndx.sHierarchy,2,1,pDst);
				//color inside contour
				fwrite(&cntrIndx.sColor,2,1,pDst);
				//number of digitizatioin of the contour
				fwrite(&cntrIndx.sNumber,2,1,pDst);
			}

			//image information
			short s1=0, s2=0;
			if (ikChar.ImgLen>=HY_IK_RECORD)
			{
				s1=ikChar.ImgLen/HY_IK_RECORD;
			}

			s2 = ikChar.ImgLen%HY_IK_RECORD;

			fwrite(&s1,2,1,pDst);
			fwrite(&s2,2,1,pDst);

			for(size_t j=0; j<szCntr; j++)
			{
				CIKContourIndex& cntrIndx =  ikChar.ContourInfo.vtContourIndex[j];

				size_t szPnt = cntrIndx.vtIKPoint.size();
				for (size_t x=0; x<szPnt; x++)
				{
					s1=0;
					s2=0;

					PointToData(cntrIndx.vtIKPoint[x],s1,s2,ikChar.CharSet);
					//s1
					fwrite(&s1,2,1,pDst);
					//s2
					fwrite(&s2,2,1,pDst);
				}
			}
		}

		return HY_NOERROR;

	}	// end of int	CHYIkCodec::EncodeCharacter()

	int	CHYIkCodec::CheckDataFormat()
	{
		long	lOffset = 2;
		short	sFlag = 0;
		memcpy(&sFlag,m_pIKData+lOffset,HY_SHORT_SZ);
		if (sFlag==0x3700)
		{
			lOffset = 0;
			short sTmp = 0, sTmp2 =0;
			while(lOffset<m_lIkDataLen)
			{				
				memcpy(&sTmp, m_pIKData+lOffset,HY_SHORT_SZ);
				sTmp2 = hy_cdr_int16_to(sTmp); 
				memcpy(m_pIKData+lOffset, &sTmp2,HY_SHORT_SZ);
				lOffset+= HY_SHORT_SZ;
			}		
		}		
	
		return HY_NOERROR;

	}	// end of int CHYIkCodec::AnalyseDataFormat()	
}

