#include "stdafx.h"
#include "XSysProxy.h"

static CXSysProxy	g_XSysproxy;
CXSysProxy&		XSysproxy() { return g_XSysproxy; }
CXSysProxy*		PXSysproxy() { return &g_XSysproxy; }

FntCheckResult::FntCheckResult()
{
	iGlyphIndex = -1;
	iFlag = -1;	

}	// end of FntCheckResult::FntCheckResult()

FntCheckResult::~FntCheckResult()
{


}	// end of FntCheckResult::~FntCheckResult()

CXSysProxy::CXSysProxy(void)
{

}	// end of CXSysProxy::CXSysProxy(void)

CXSysProxy::~CXSysProxy(void)
{

}	// end of CXSysProxy::~CXSysProxy(void)

void CXSysProxy::MakeFontName(CHYFontCodec& FnCdc)
{
	FnCdc.m_HYName.SetDefault();

	std::string strFamilyEN = m_tagOpeionPrm.ENGFaimlyName;
	std::string strSubFamileEN = m_tagOpeionPrm.ENGSubName;
	std::string strFamilyCN = m_tagOpeionPrm.CHSFaimlyName;
	std::string strSubFamileCN = m_tagOpeionPrm.CHSSubName;
	std::string strVersion = m_tagOpeionPrm.Version;

	NAMERECORD nmRecord;
	CString  IDString;
	// 1.0.0 
	nmRecord.platformID = 1;
	nmRecord.encodingID = 0;
	nmRecord.languageID = 0;
	nmRecord.nameID = 0;
	IDString.LoadString(IDS_NAME_ID_0_EN);
	nmRecord.strContent = std::string(IDString);
	FnCdc.m_HYName.vtNameRecord.push_back(nmRecord);

	nmRecord.nameID = 1;
	nmRecord.strContent = strFamilyEN;
	FnCdc.m_HYName.vtNameRecord.push_back(nmRecord);

	nmRecord.nameID = 2;
	nmRecord.strContent = strSubFamileEN;
	FnCdc.m_HYName.vtNameRecord.push_back(nmRecord);

	nmRecord.nameID = 3;
	nmRecord.strContent = strFamilyEN + "-" + strSubFamileEN + " v9.00.00";
	FnCdc.m_HYName.vtNameRecord.push_back(nmRecord);

	nmRecord.nameID = 4;
	if (strSubFamileEN == _T("Regular")) {
		nmRecord.strContent = strFamilyEN;
	}
	else {
		nmRecord.strContent = strFamilyEN + " " + strSubFamileEN;
	}
	FnCdc.m_HYName.vtNameRecord.push_back(nmRecord);

	nmRecord.nameID = 5;
	nmRecord.strContent = strVersion;
	FnCdc.m_HYName.vtNameRecord.push_back(nmRecord);

	nmRecord.nameID = 6;
	nmRecord.strContent = strFamilyEN + "-" + strSubFamileEN;
	FnCdc.m_HYName.vtNameRecord.push_back(nmRecord);

	nmRecord.nameID = 7;
	IDString.LoadString(IDS_NAME_ID_7_EN);
	nmRecord.strContent = strFamilyEN + " " + std::string(IDString);
	FnCdc.m_HYName.vtNameRecord.push_back(nmRecord);

	nmRecord.nameID = 8;
	IDString.LoadString(IDS_NAME_ID_8_EN);
	nmRecord.strContent = std::string(IDString);
	FnCdc.m_HYName.vtNameRecord.push_back(nmRecord);

	nmRecord.nameID = 11;
	IDString.LoadString(IDS_NAME_ID_11);
	nmRecord.strContent = std::string(IDString);
	FnCdc.m_HYName.vtNameRecord.push_back(nmRecord);

	nmRecord.nameID = 13;
	IDString.LoadString(IDS_NAME_ID_13_EN);
	nmRecord.strContent = std::string(IDString);
	FnCdc.m_HYName.vtNameRecord.push_back(nmRecord);

	nmRecord.nameID = 14;
	IDString.LoadString(IDS_NAME_ID_14);
	nmRecord.strContent = std::string(IDString);
	FnCdc.m_HYName.vtNameRecord.push_back(nmRecord);

	//1.25.33
	nmRecord.platformID = 1;
	nmRecord.encodingID = 25;
	nmRecord.languageID = 33;
	nmRecord.nameID = 0;
	IDString.LoadString(IDS_NAME_ID_0_CHS);
	nmRecord.strContent = std::string(IDString);
	FnCdc.m_HYName.vtNameRecord.push_back(nmRecord);

	nmRecord.nameID = 1;
	nmRecord.strContent = strFamilyCN;
	FnCdc.m_HYName.vtNameRecord.push_back(nmRecord);

	nmRecord.nameID = 2;
	nmRecord.strContent = strSubFamileCN;
	FnCdc.m_HYName.vtNameRecord.push_back(nmRecord);

	nmRecord.nameID = 4;
	if (strSubFamileCN == _T("Regular")) {
		nmRecord.strContent = strFamilyCN;
	}
	else {
		nmRecord.strContent = strFamilyCN + " " + strSubFamileCN;
	}
	FnCdc.m_HYName.vtNameRecord.push_back(nmRecord);

	nmRecord.nameID = 7;
	IDString.LoadString(IDS_NAME_ID_7_CHS);
	nmRecord.strContent = strFamilyEN + std::string(IDString);
	FnCdc.m_HYName.vtNameRecord.push_back(nmRecord);
	nmRecord.nameID = 8;
	IDString.LoadString(IDS_NAME_ID_8_CHS);
	nmRecord.strContent = std::string(IDString);
	FnCdc.m_HYName.vtNameRecord.push_back(nmRecord);
	nmRecord.nameID = 11;
	IDString.LoadString(IDS_NAME_ID_11);
	nmRecord.strContent = std::string(IDString);
	FnCdc.m_HYName.vtNameRecord.push_back(nmRecord);
	nmRecord.nameID = 13;
	IDString.LoadString(IDS_NAME_ID_13_CHS);
	nmRecord.strContent = std::string(IDString);
	FnCdc.m_HYName.vtNameRecord.push_back(nmRecord);
	nmRecord.nameID = 14;
	IDString.LoadString(IDS_NAME_ID_14);
	nmRecord.strContent = std::string(IDString);
	FnCdc.m_HYName.vtNameRecord.push_back(nmRecord);

	//3.1.1033
	nmRecord.platformID = 3;
	nmRecord.encodingID = 1;
	nmRecord.languageID = 1033;
	nmRecord.nameID = 0;
	IDString.LoadString(IDS_NAME_ID_0_EN);
	nmRecord.strContent = std::string(IDString);
	FnCdc.m_HYName.vtNameRecord.push_back(nmRecord);

	if (strSubFamileEN == _T("Regular")
		|| strSubFamileEN == _T("Bold")
		|| strSubFamileEN == _T("Italic")
		|| strSubFamileEN == _T("Bold Italic")) {
		nmRecord.nameID = 1;
		nmRecord.strContent = strFamilyEN;
		FnCdc.m_HYName.vtNameRecord.push_back(nmRecord);
		nmRecord.nameID = 2;
		nmRecord.strContent = strSubFamileEN;
		FnCdc.m_HYName.vtNameRecord.push_back(nmRecord);
	}
	else {
		nmRecord.nameID = 1;
		nmRecord.strContent = strFamilyEN + " " + strSubFamileEN;
		FnCdc.m_HYName.vtNameRecord.push_back(nmRecord);
		nmRecord.nameID = 2;
		nmRecord.strContent = _T("Regular");
		FnCdc.m_HYName.vtNameRecord.push_back(nmRecord);
	}

	nmRecord.nameID = 3;
	nmRecord.strContent = strFamilyEN + "-" + strSubFamileEN + " v9.00.00";
	FnCdc.m_HYName.vtNameRecord.push_back(nmRecord);
	nmRecord.nameID = 4;
	if (strSubFamileEN == _T("Regular")) {
		nmRecord.strContent = strFamilyEN;
	}
	else {
		nmRecord.strContent = strFamilyEN + " " + strSubFamileEN;
	}
	FnCdc.m_HYName.vtNameRecord.push_back(nmRecord);

	nmRecord.nameID = 5;
	nmRecord.strContent = strVersion;
	FnCdc.m_HYName.vtNameRecord.push_back(nmRecord);
	nmRecord.nameID = 6;
	nmRecord.strContent = strFamilyEN + "-" + strSubFamileEN;
	FnCdc.m_HYName.vtNameRecord.push_back(nmRecord);
	nmRecord.nameID = 7;
	IDString.LoadString(IDS_NAME_ID_7_EN);
	nmRecord.strContent = strFamilyEN + " " + std::string(IDString);
	FnCdc.m_HYName.vtNameRecord.push_back(nmRecord);
	nmRecord.nameID = 8;
	IDString.LoadString(IDS_NAME_ID_8_EN);
	nmRecord.strContent = std::string(IDString);
	FnCdc.m_HYName.vtNameRecord.push_back(nmRecord);
	nmRecord.nameID = 11;
	IDString.LoadString(IDS_NAME_ID_11);
	nmRecord.strContent = std::string(IDString);
	FnCdc.m_HYName.vtNameRecord.push_back(nmRecord);
	nmRecord.nameID = 13;
	IDString.LoadString(IDS_NAME_ID_13_EN);
	nmRecord.strContent = std::string(IDString);
	FnCdc.m_HYName.vtNameRecord.push_back(nmRecord);
	nmRecord.nameID = 14;
	IDString.LoadString(IDS_NAME_ID_14);
	nmRecord.strContent = std::string(IDString);
	FnCdc.m_HYName.vtNameRecord.push_back(nmRecord);
	if (strSubFamileEN != _T("Regular")
		&& strSubFamileEN != _T("Bold")
		&& strSubFamileEN != _T("Italic")
		&& strSubFamileEN == _T("Bold Italic")) {

		nmRecord.nameID = 16;
		nmRecord.strContent = strFamilyEN;
		FnCdc.m_HYName.vtNameRecord.push_back(nmRecord);

		nmRecord.nameID = 17;
		nmRecord.strContent = strSubFamileEN;
		FnCdc.m_HYName.vtNameRecord.push_back(nmRecord);
	}

	//3.1.2052
	nmRecord.platformID = 3;
	nmRecord.encodingID = 1;
	nmRecord.languageID = 2052;
	nmRecord.nameID = 0;
	IDString.LoadString(IDS_NAME_ID_0_CHS);
	nmRecord.strContent = std::string(IDString);
	FnCdc.m_HYName.vtNameRecord.push_back(nmRecord);

	if (strSubFamileCN == _T("Regular")
		|| strSubFamileCN == _T("Bold")
		|| strSubFamileCN == _T("Italic")
		|| strSubFamileCN == _T("Bold Italic")) {
		nmRecord.nameID = 1;
		nmRecord.strContent = strFamilyCN;
		FnCdc.m_HYName.vtNameRecord.push_back(nmRecord);
		nmRecord.nameID = 2;
		nmRecord.strContent = strSubFamileCN;
		FnCdc.m_HYName.vtNameRecord.push_back(nmRecord);
	}
	else {
		nmRecord.nameID = 1;
		nmRecord.strContent = strFamilyEN + " " + strSubFamileEN;
		FnCdc.m_HYName.vtNameRecord.push_back(nmRecord);
		nmRecord.nameID = 2;
		nmRecord.strContent = _T("Regular");
		FnCdc.m_HYName.vtNameRecord.push_back(nmRecord);
	}
	nmRecord.nameID = 4;
	if (strSubFamileCN == _T("Regular")) {
		nmRecord.strContent = strFamilyCN;
	}
	else {
		nmRecord.strContent = strFamilyCN + " " + strSubFamileCN;
	}
	FnCdc.m_HYName.vtNameRecord.push_back(nmRecord);

	nmRecord.nameID = 7;
	IDString.LoadString(IDS_NAME_ID_7_CHS);
	nmRecord.strContent = strFamilyEN + std::string(IDString);
	FnCdc.m_HYName.vtNameRecord.push_back(nmRecord);
	nmRecord.nameID = 8;
	IDString.LoadString(IDS_NAME_ID_8_CHS);
	nmRecord.strContent = std::string(IDString);
	FnCdc.m_HYName.vtNameRecord.push_back(nmRecord);
	nmRecord.nameID = 11;
	IDString.LoadString(IDS_NAME_ID_11);
	nmRecord.strContent = std::string(IDString);
	FnCdc.m_HYName.vtNameRecord.push_back(nmRecord);
	nmRecord.nameID = 13;
	IDString.LoadString(IDS_NAME_ID_13_CHS);
	nmRecord.strContent = std::string(IDString);
	FnCdc.m_HYName.vtNameRecord.push_back(nmRecord);
	nmRecord.nameID = 14;
	IDString.LoadString(IDS_NAME_ID_14);
	nmRecord.strContent = std::string(IDString);
	FnCdc.m_HYName.vtNameRecord.push_back(nmRecord);
	if (strSubFamileCN != _T("Regular")
		&& strSubFamileCN != _T("Bold")
		&& strSubFamileCN != _T("Italic")
		&& strSubFamileCN == _T("Bold Italic")) {

		nmRecord.nameID = 16;
		nmRecord.strContent = strFamilyCN;
		FnCdc.m_HYName.vtNameRecord.push_back(nmRecord);
		nmRecord.nameID = 17;
		nmRecord.strContent = strSubFamileCN;
		FnCdc.m_HYName.vtNameRecord.push_back(nmRecord);
	}

}	// end of bool CFontSetupOptionPage::MakeName(CHYFontCodec&	inDecode, CHYFontCodec&	Encode)

BOOL CXSysProxy::MToWChar(WCHAR* pDest, int& iDestlen, char* pSrc )
{
	if ( pDest == NULL )			return FALSE;
	if ( pSrc == NULL )				return FALSE;

	WCHAR*	wpString			= NULL;
	int		iWStringLen			= 0;

	iWStringLen = MultiByteToWideChar(CP_ACP,0, pSrc, -1, NULL, 0 );
	if ( iWStringLen == 0 )			return FALSE;
	if ( iWStringLen > iDestlen )	return FALSE;

	wpString = new WCHAR[iWStringLen];
	ZeroMemory(wpString, sizeof(WCHAR) * iWStringLen);

	MultiByteToWideChar(CP_ACP, 0, pSrc, strlen(pSrc), wpString, iWStringLen);

	iDestlen = iWStringLen;
	CopyMemory(pDest,wpString, sizeof(WCHAR) * iDestlen );

	if ( wpString ) delete[] wpString;
	return TRUE;

}	// end of BOOL	CXSysProxy::MToWChar()

void CXSysProxy::TrimTTFContour(const CHYContour& inCntur, CHYContour& OutCntur)
{
	const std::vector<CHYPoint>& vtPoints = inCntur.vtHYPoints;
	size_t vtSize		= inCntur.vtHYPoints.size();
	size_t vtTmpIndex	= -1;

	if (vtPoints[0].flag == POINT_FLG_CONTROL)
	{
		for (size_t i=0; i<vtSize; i++)
		{
			if (vtPoints[i].flag == POINT_FLG_ANCHOR)
			{
				vtTmpIndex = i;
				break;
			}
		}

		if (vtTmpIndex!=-1)
		{			
			for (size_t i=vtTmpIndex; i<vtSize; i++)
			{
				const CHYPoint& tmpPt = vtPoints[i];
				OutCntur.vtHYPoints.push_back(tmpPt);				
			}

			for (size_t i=0; i<vtTmpIndex; i++)
			{
				const CHYPoint& tmpPt = vtPoints[i];
				OutCntur.vtHYPoints.push_back(tmpPt);				
			}
		}
		else // �����������û��ONLine�㣬�������ΪOnline��
		{
			const CHYPoint&		Pt1 = vtPoints[0];
			const CHYPoint&		Pt2 = vtPoints[vtSize-1];		
			CHYPoint		PtStart;

			PtStart.x			= Pt1.x+((Pt2.x-Pt1.x)/2);
			PtStart.y			= Pt1.y+((Pt2.y-Pt1.y)/2);
			PtStart.flag		= POINT_FLG_ANCHOR;
			OutCntur.vtHYPoints.push_back(PtStart);			

			for (size_t i=0; i<vtSize; i++)
			{			
				const CHYPoint& tmpPt = vtPoints[i];
				OutCntur.vtHYPoints.push_back(tmpPt);				
			}		
		}	
	}
	else 
	{
		for (size_t i=0; i<vtSize; i++)
		{			
			const CHYPoint& tmpPt = vtPoints[i];
			OutCntur.vtHYPoints.push_back(tmpPt);				
		}
	}

	vtSize = OutCntur.vtHYPoints.size();
	if (vtSize>0)
	{
		OutCntur.vtHYPoints[0].flag = HY_CRV_FLG_ANCHOR_START;
	}

	for (size_t i=1; i<vtSize; i++)
	{
		
		CHYPoint& tmpPT = OutCntur.vtHYPoints[i];

		if (tmpPT.flag == POINT_FLG_ANCHOR) tmpPT.flag = HY_CRV_FLG_ANCHOR;
		else if (tmpPT.flag == POINT_FLG_CONTROL) tmpPT.flag = HY_CRV_FLG_CURVE;
	}

	if (OutCntur.vtHYPoints[0].x == OutCntur.vtHYPoints[vtSize-1].x && OutCntur.vtHYPoints[0].y == OutCntur.vtHYPoints[vtSize-1].y)
	{
		OutCntur.vtHYPoints[vtSize-1].flag = HY_CRV_FLG_ANCHOR_END;
	}
	else 
	{
		CHYPoint tmpPT = OutCntur.vtHYPoints[0];
		tmpPT.flag = HY_CRV_FLG_ANCHOR_END;
		OutCntur.vtHYPoints.push_back(tmpPT);
	}
	
}	// end of void CXSysProxy::TrimContour()

void CXSysProxy::CheckFont(HYFONTCODEC::CHYFontCodec& baseFntCodec, HYFONTCODEC::CHYFontCodec& inFntCodec, std::vector<FntCheckResult>& vtResult)
{
	if (baseFntCodec.m_HYMaxp.numGlyphs != inFntCodec.m_HYMaxp.numGlyphs){
		FntCheckResult chckResult;
		chckResult.iGlyphIndex = -1;
		vtResult.push_back(chckResult);
		return;
	}		

	for (int i = 0; i < baseFntCodec.m_HYMaxp.numGlyphs; i++){
		FntCheckResult chckResult;

		CHYGlyph& baseGlyph = baseFntCodec.m_vtHYGlyphs[i];
		CHYGlyph& inGlyph = inFntCodec.m_vtHYGlyphs[i];

		int iBaseTalPoints =  baseGlyph.GetTotalPointsNumber();
		int iInTalPoints = inGlyph.GetTotalPointsNumber();

		if (iBaseTalPoints != iInTalPoints){
			chckResult.iGlyphIndex = i;
			chckResult.iFlag =0;

			vtResult.push_back(chckResult);
			continue;
		}

		int iBaseContour = (int)baseGlyph.vtContour.size();
		int iInContour = (int)inGlyph.vtContour.size();
		if (iBaseContour != iInContour) {

			chckResult.iGlyphIndex = i;
			chckResult.iFlag = 1;

			vtResult.push_back(chckResult);
			continue;
		}

		bool bBreak = false;
		int  PointIndex = 0;
		for (int j = 0; j < baseGlyph.vtContour.size(); j++){
			CHYContour& cnturBase = baseGlyph.vtContour[j];
			CHYContour& cnturIn = inGlyph.vtContour[j];

			int iBaseCntPoints = (int)cnturBase.vtHYPoints.size();
			int iInCntPoins = (int)cnturIn.vtHYPoints.size();

			for (int x = 0; x != iBaseCntPoints; x++){
				CHYPoint& ptBase =  cnturBase.vtHYPoints[x];
				CHYPoint& ptIn = cnturIn.vtHYPoints[x];

				if (ptBase.flag != ptIn.flag){
					
					chckResult.iGlyphIndex = i;
					chckResult.iFlag = 2;
					chckResult.vtPointIndex.push_back(PointIndex);

					bBreak = true;
					continue;
				}
				PointIndex++;
			}
		}

		if (bBreak) {
			vtResult.push_back(chckResult);
			break;
		}
	}

}	// end of void CXSysProxy::CheckFont()

void CXSysProxy::SumVrGlyphs(std::vector<CVrGlyphs>& invtVrGlyphs, CVrGlyphs& outVrGlyph)
{
	int iInVrGlyphsSize = invtVrGlyphs.size();
	int i = 0;

	while (i < iInVrGlyphsSize){		
		CVrGlyphs& vrglyphs = invtVrGlyphs[i];		
		size_t stSize = vrglyphs.vtVrGlyphs.size();
		if (i == 0){
			outVrGlyph.vtVrGlyphs.resize(stSize);
		}

		for (size_t j = 0; j < stSize; j++) {
			CVrGlyph& vrglyph = vrglyphs.vtVrGlyphs[j];
			CVrGlyph& Outglyph = outVrGlyph.vtVrGlyphs[j];

			size_t stPtSize = vrglyph.vtX.size();
			if (i == 0){
				Outglyph.vtX.resize(stPtSize, 0);
				Outglyph.vtY.resize(stPtSize, 0);
			}

			for (size_t x = 0; x < stPtSize; x++){
				if (i == 0){
					Outglyph.vtX[x] = vrglyph.vtX[x];
					Outglyph.vtY[x] = vrglyph.vtY[x];
				}
				else{
					Outglyph.vtX[x] += vrglyph.vtX[x];
					Outglyph.vtY[x] += vrglyph.vtY[x];
				}
			}
		}
		i++;
	}

}	// end of void CXSysProxy::SumVrGlyphs()

void CXSysProxy::SubVrGlyphs(CVrGlyphs& inVrGlyphs1, CVrGlyphs& inVrGlyphs2, CVrGlyphs& outVrGlyphs)
{
	outVrGlyphs.vtVrGlyphs.clear();
	if (inVrGlyphs1.vtVrGlyphs.size() != inVrGlyphs2.vtVrGlyphs.size())
		return;

	size_t stSize = inVrGlyphs1.vtVrGlyphs.size();
	for (size_t i = 0; i < stSize; i++){
		CVrGlyph oudtGlyph;
		CVrGlyph& inGlyph1 = inVrGlyphs1.vtVrGlyphs[i];
		CVrGlyph& inGlyph2 = inVrGlyphs2.vtVrGlyphs[i];
		
		if (inGlyph1.vtX.size() != inGlyph2.vtX.size()) return;
		if (inGlyph1.vtY.size() != inGlyph2.vtY.size()) return;

		size_t stPt = inGlyph1.vtX.size();
		for (size_t j = 0; j < stPt; j++){
			short x = inGlyph2.vtX[j] - inGlyph1.vtX[j];
			short y = inGlyph2.vtY[j] - inGlyph1.vtY[j];

			oudtGlyph.vtX.push_back(x);
			oudtGlyph.vtY.push_back(y);
		}
		outVrGlyphs.vtVrGlyphs.push_back(oudtGlyph);
	}

}	// end of void CXSysProxy::SubVrGlyphs()

bool GlyphSortPredicate(CHYGlyph& d1, CHYGlyph& d2)
{
	if (d1.vtUnicode.size() == 0) return true;
	if (d2.vtUnicode.size() == 0) return true;

	unsigned long uni1 = d1.vtUnicode[0];
	unsigned long uni2 = d2.vtUnicode[0];

	if (uni1 == 0xffff) return true;
	if (uni2 == 0xffff) return false;

	return (uni1 < uni2);

}	// end of BOOL GlyphSortPredicate()

void CXSysProxy::InitEncodeOption(CHYFontCodec& Encode)
{
	Encode.CountFontBound();

	if (::XSysproxy().m_tagOpeionPrm.bCmplCMAP) {
		//Encode.AddControlChar();
		Encode.MakeCmap();
	}
	if (::XSysproxy().m_tagOpeionPrm.bRePsName) {
		// ֻ��������������
		Encode.RestPsName();
	}
	if (::XSysproxy().m_tagOpeionPrm.bReSortUni) {
		std::sort(Encode.m_vtHYGlyphs.begin(), Encode.m_vtHYGlyphs.end(), GlyphSortPredicate);
	}
	//Maxp
	Encode.m_HYMaxp.numGlyphs = Encode.m_vtHYGlyphs.size();
	if (Encode.m_iFontType == FONTTYPE_OTF) {
		Encode.m_HYMaxp.version.value = 0;
		Encode.m_HYMaxp.version.fract = 0x5000;
	}
	if (Encode.m_iFontType == FONTTYPE_TTF) {
		Encode.m_HYMaxp.version.value = 1;
		Encode.m_HYMaxp.version.fract = 0;
	}
	for (unsigned short i = 0; i < Encode.m_HYMaxp.numGlyphs; i++) {
		size_t szTmpPoint = 0;
		CHYGlyph& glyph = Encode.m_vtHYGlyphs[i];
		size_t szCntursNum = glyph.vtContour.size();
		if (szCntursNum == 0 && glyph.vtComponents.size() == 0) continue;
		if (szCntursNum > 0) {
			if (Encode.m_HYMaxp.maxContours < szCntursNum)
				Encode.m_HYMaxp.maxContours = szCntursNum;

			for (unsigned short j = 0; j < szCntursNum; j++) {
				szTmpPoint += glyph.vtContour[j].vtHYPoints.size();
			}
			if (Encode.m_HYMaxp.maxPoints < szTmpPoint)
				Encode.m_HYMaxp.maxPoints = szTmpPoint;
		}
		else {
			size_t  maxCmpnntElmTmp = glyph.vtComponents.size();
			size_t  maxCompositePointTmp = 0, maxCompositeContourTmp = 0;

			for (unsigned int j = 0; j < maxCmpnntElmTmp; j++) {
				CHYCmpst& cmpst = glyph.vtComponents[j];
				CHYGlyph& cmpGlyph = Encode.m_vtHYGlyphs[cmpst.glyphIndex];
				maxCompositePointTmp += cmpGlyph.GetTotalPointsNumber();
				maxCompositeContourTmp += cmpGlyph.vtContour.size();
			}
			if (maxCmpnntElmTmp > Encode.m_HYMaxp.maxComponentElements)
				Encode.m_HYMaxp.maxComponentElements = maxCmpnntElmTmp;
			if (maxCompositePointTmp > Encode.m_HYMaxp.maxCompositePoints)
				Encode.m_HYMaxp.maxCompositePoints = maxCompositePointTmp;
			if (maxCompositeContourTmp > Encode.m_HYMaxp.maxCompositeContours)
				Encode.m_HYMaxp.maxCompositeContours = maxCompositeContourTmp;
		}
		Encode.m_HYMaxp.maxZones = 2;
		Encode.m_HYMaxp.maxTwilightPoints = 4;
		Encode.m_HYMaxp.maxStorage = 32;
		Encode.m_HYMaxp.maxFunctionDefs = 96;
		Encode.m_HYMaxp.maxInstructionDefs = 96;
		Encode.m_HYMaxp.maxStackElements = 256;
		Encode.m_HYMaxp.maxSizeOfInstructions = 8;
		Encode.m_HYMaxp.maxComponentDepth = 1;
	}
	// Head
	Encode.m_HYhead.version.value = 1;
	Encode.m_HYhead.version.fract = 0;
	Encode.m_HYhead.fontRevision.value = 1;
	Encode.m_HYhead.fontRevision.fract = 0;
	Encode.m_HYhead.magicNumber = 0x5F0F3CF5;
	Encode.m_HYhead.unitsPerEm = ::XSysproxy().m_tagOpeionPrm.usUnitsPerEM;
	// Bit 0: Baseline for font at y=0. 
	//Bit 1: Left sidebearing point at x=0 (relevant only for TrueType rasterizers) �� see the note below regarding variable fonts.
	Encode.m_HYhead.flags = 3;
	__time64_t	 tspace = 2082844799;
	CTime tCurTime = CTime::GetCurrentTime();
	__time64_t  t64CurTime = tCurTime.GetTime();
	t64CurTime += tspace;
	char p[8] = { 0 };
	CopyMemory((PBYTE)p, &t64CurTime, sizeof(__time64_t));
	for (int i = 0; i < 8; i++) {
		Encode.m_HYhead.created[i] = p[7 - i];
		Encode.m_HYhead.modified[i] = p[7 - i];
	}
	// Hhea
	Encode.m_HYHhea.version.value = 1;
	Encode.m_HYHhea.version.fract = 0;
	Encode.m_HYHhea.Ascender = Encode.m_HYhead.xMax;
	Encode.m_HYHhea.Descender = Encode.m_HYhead.yMin;
	Encode.m_HYHhea.LineGap = 0;
	Encode.m_HYHhea.advanceWidthMax = Encode.GetAdvancMaxWidth();
	Encode.m_HYHhea.minLeftSideBearing = Encode.m_HYhead.xMin;
	Encode.m_HYHhea.minRightSideBearing = Encode.FindMinRightSideBearing();
	Encode.m_HYHhea.xMaxExtent = Encode.m_HYHhea.minLeftSideBearing + (Encode.m_HYhead.xMax - Encode.m_HYhead.xMin);
	int szGlyphNum = Encode.m_vtHYGlyphs.size();
	Encode.m_HYHhea.numberOfHMetrics = 0;
	if (szGlyphNum > 0) {
		int iBaseadvanceWidth = Encode.m_vtHYGlyphs[--szGlyphNum].advanceWidth;
		while (--szGlyphNum >= 0) {
			if (Encode.m_vtHYGlyphs[szGlyphNum].advanceWidth == iBaseadvanceWidth)
				Encode.m_HYHhea.numberOfHMetrics++;
			else
				break;
		}
		Encode.m_HYHhea.numberOfHMetrics = (unsigned short)Encode.m_vtHYGlyphs.size() - Encode.m_HYHhea.numberOfHMetrics;
	}
	//OS2
	Encode.m_HYOS2.usWeightClass = ::XSysproxy().m_tagOpeionPrm.usWeight;
	Encode.m_HYOS2.usWidthClass = ::XSysproxy().m_tagOpeionPrm.usWidth;
	Encode.m_HYOS2.sTypoAscender = ::XSysproxy().m_tagOpeionPrm.sTypo_Ascender;
	Encode.m_HYOS2.sTypoDescender = ::XSysproxy().m_tagOpeionPrm.sTypo_Descender;
	Encode.m_HYOS2.usWinAscent = Encode.m_HYhead.yMax;
	Encode.m_HYOS2.usWinDescent = Encode.m_HYhead.yMin;
	Encode.m_HYOS2.ulCodePageRange1 = ::XSysproxy().m_tagOpeionPrm.ulCodePageRang1;
	Encode.m_HYOS2.ulCodePageRange2 = ::XSysproxy().m_tagOpeionPrm.ulCodePageRang2;
	Encode.m_HYOS2.vtachVendID.clear();
	Encode.m_HYOS2.vtachVendID.push_back('H');
	Encode.m_HYOS2.vtachVendID.push_back('N');
	Encode.m_HYOS2.vtachVendID.push_back('Y');
	Encode.m_HYOS2.vtachVendID.push_back('I');
	int iAvg = 0, iCount = 0;
	for (size_t i = 0; i < Encode.m_vtHYGlyphs.size(); i++) {
		if (Encode.m_vtHYGlyphs[i].advanceWidth != 0) {
			iAvg += Encode.m_vtHYGlyphs[i].advanceWidth;
			iCount++;
		}
	}
	if (iCount == 0) Encode.m_HYOS2.xAvgCharWidth = 0;
	else Encode.m_HYOS2.xAvgCharWidth = (unsigned short)(iAvg / iCount + 0.5f);

	Encode.m_HYOS2.fsType = 0x0008;
	Encode.m_HYOS2.ySubscriptXSize = Encode.m_HYhead.unitsPerEm / 10;
	Encode.m_HYOS2.ySubscriptYSize = Encode.m_HYhead.unitsPerEm / 10;
	Encode.m_HYOS2.ySuperscriptXSize = Encode.m_HYhead.unitsPerEm / 10;
	Encode.m_HYOS2.ySuperscriptYSize = Encode.m_HYhead.unitsPerEm / 10;
	Encode.m_HYOS2.ySuperscriptYOffset = Encode.m_HYhead.unitsPerEm / 10;
	int iGID = Encode.FindGlyph(0x005F, Encode.m_vtHYGlyphs);
	if (iGID != -1) {
		Encode.m_HYOS2.yStrikeoutSize = Encode.m_vtHYGlyphs[iGID].maxY - Encode.m_vtHYGlyphs[iGID].minY;
	}
	Encode.m_HYOS2.yStrikeoutPosition = Encode.m_HYhead.yMax - (Encode.m_HYhead.yMax - Encode.m_HYhead.yMin) / 2;
	Encode.m_HYOS2.sFamilyClass = 0;
	Encode.m_HYOS2.panose.FamilyType = 2;
	Encode.m_HYOS2.panose.SerifStyle = 0;
	if (Encode.m_HYOS2.usWeightClass <= 100)
		Encode.m_HYOS2.panose.Weight = 2;
	if (Encode.m_HYOS2.usWeightClass <= 200)
		Encode.m_HYOS2.panose.Weight = 3;
	if (Encode.m_HYOS2.usWeightClass <= 300)
		Encode.m_HYOS2.panose.Weight = 4;
	if (Encode.m_HYOS2.usWeightClass <= 400)
		Encode.m_HYOS2.panose.Weight = 5;
	if (Encode.m_HYOS2.usWeightClass <= 500)
		Encode.m_HYOS2.panose.Weight = 6;
	if (Encode.m_HYOS2.usWeightClass <= 600)
		Encode.m_HYOS2.panose.Weight = 7;
	if (Encode.m_HYOS2.usWeightClass <= 700)
		Encode.m_HYOS2.panose.Weight = 8;
	if (Encode.m_HYOS2.usWeightClass <= 800)
		Encode.m_HYOS2.panose.Weight = 9;
	if (Encode.m_HYOS2.usWeightClass <= 900)
		Encode.m_HYOS2.panose.Weight = 10;
	if (Encode.m_HYOS2.usWeightClass <= 1000)
		Encode.m_HYOS2.panose.Weight = 11;
	Encode.m_HYOS2.panose.Proportion = 0;
	Encode.m_HYOS2.panose.Contrast = 0;
	Encode.m_HYOS2.panose.StrokeVariation = 0;
	Encode.m_HYOS2.panose.ArmStyle = 0;
	Encode.m_HYOS2.panose.Letterform = 0;
	Encode.m_HYOS2.panose.Midline = 0;
	Encode.m_HYOS2.panose.XHeight = 0;
	Encode.CountUnicodeRange(Encode.m_HYOS2.ulUnicodeRange1, Encode.m_HYOS2.ulUnicodeRange2, Encode.m_HYOS2.ulUnicodeRange3, Encode.m_HYOS2.ulUnicodeRange4);
	Encode.m_HYOS2.usFirstCharIndex = 0x0020;
	Encode.m_HYOS2.usLastCharIndex = Encode.FindLastCharIndex(Encode.m_vtHYGlyphs);
	iGID = Encode.FindGlyph(0x0078, Encode.m_vtHYGlyphs);
	if (iGID != -1) {
		Encode.m_HYOS2.sxHeight = Encode.m_vtHYGlyphs[iGID].maxY;
	}
	iGID = Encode.FindGlyph(0x0048, Encode.m_vtHYGlyphs);
	if (iGID != -1) {
		Encode.m_HYOS2.sCapHeight = Encode.m_vtHYGlyphs[iGID].maxY;
	}
	Encode.m_HYOS2.usDefaultChar = 0;
	Encode.m_HYOS2.usBreakChar = 0x0020;
	Encode.m_HYOS2.usMaxContext = 3;
	//POST
	Encode.m_HYPost.Format.value = 2;
	Encode.m_HYPost.Format.fract = 0;
	Encode.m_HYPost.italicAngle.value = 0;
	Encode.m_HYPost.italicAngle.fract = 0;
	Encode.m_HYPost.underlineThickness = Encode.m_HYOS2.yStrikeoutSize;
	Encode.m_HYPost.underlinePosition = Encode.m_HYOS2.yStrikeoutPosition;
	Encode.m_HYPost.isFixedPitch = 0;

	Encode.m_HYCFFInfo.TopDICT.IsCIDFont = ::XSysproxy().m_tagOpeionPrm.bCIDFnt;
	Encode.m_HYCFFInfo.TopDICT.Ros.strRegistry = ::XSysproxy().m_tagOpeionPrm.Registry;
	Encode.m_HYCFFInfo.TopDICT.Ros.strOrdering = ::XSysproxy().m_tagOpeionPrm.Ordering;
	Encode.m_HYCFFInfo.TopDICT.Ros.Supplement = atol(::XSysproxy().m_tagOpeionPrm.Supplement);

	if (!Encode.m_HYCFFInfo.TopDICT.IsCIDFont) {
		for (size_t i = 0; i < Encode.m_HYMaxp.numGlyphs; i++) {
			const CHYGlyph& glyph = Encode.m_vtHYGlyphs[i];
			Encode.m_HYCFFInfo.stnStrings.InsertString(glyph.strPostName);
		}
	}
	if (::XSysproxy().m_tagOpeionPrm.bFontname) {
		MakeFontName(Encode);
	}

}	// end of void CXSysProxy::InitEncodeOption()

void CXSysProxy::SetEncodeOption(CHYFontCodec& Encode, CHYFontCodec& Original)
{
	// Head
	Encode.m_HYhead.unitsPerEm = Original.m_HYhead.unitsPerEm;
	memcpy(Encode.m_HYhead.created, Original.m_HYhead.created, 8);

	// Hhea
	if (!m_tagOpeionPrm.bHanyi) {
		Encode.m_HYHhea.Ascender = Original.m_HYHhea.Ascender;
		Encode.m_HYHhea.Descender = Original.m_HYHhea.Descender;
		Encode.m_HYHhea.LineGap = Original.m_HYHhea.LineGap;
	}

	//OS2
	Encode.m_HYOS2.usWeightClass = Original.m_HYOS2.usWeightClass;
	Encode.m_HYOS2.usWidthClass = Original.m_HYOS2.usWidthClass;
	if (!m_tagOpeionPrm.bHanyi) {
		Encode.m_HYOS2.sTypoAscender = Original.m_HYOS2.sTypoAscender;
		Encode.m_HYOS2.sTypoDescender = Original.m_HYOS2.sTypoDescender;
		Encode.m_HYOS2.sTypoLineGap = Original.m_HYOS2.sTypoLineGap;
		Encode.m_HYOS2.usWinAscent = Original.m_HYOS2.usWinAscent;
		Encode.m_HYOS2.usWinDescent = Original.m_HYOS2.usWinDescent;

		Encode.m_HYOS2.vtachVendID.clear();
		for each (char var in Original.m_HYOS2.vtachVendID) {
			Encode.m_HYOS2.vtachVendID.push_back(var);
		}

		Encode.m_HYOS2.ulCodePageRange1 = Original.m_HYOS2.ulCodePageRange1;
		Encode.m_HYOS2.ulCodePageRange2 = Original.m_HYOS2.ulCodePageRange2;
	}

	Encode.m_HYOS2.ySubscriptXSize = Original.m_HYOS2.ySubscriptXSize;
	Encode.m_HYOS2.ySubscriptYSize = Original.m_HYOS2.ySubscriptYSize;
	Encode.m_HYOS2.ySuperscriptXSize = Original.m_HYOS2.ySuperscriptXSize;
	Encode.m_HYOS2.ySuperscriptYSize = Original.m_HYOS2.ySuperscriptYSize;
	Encode.m_HYOS2.ySuperscriptYOffset = Original.m_HYOS2.ySuperscriptYOffset;
	Encode.m_HYOS2.yStrikeoutSize = Original.m_HYOS2.yStrikeoutSize;
	Encode.m_HYOS2.yStrikeoutPosition = Original.m_HYOS2.yStrikeoutPosition;
	Encode.m_HYOS2.sFamilyClass = Original.m_HYOS2.sFamilyClass;
	Encode.m_HYOS2.panose.FamilyType = Original.m_HYOS2.panose.FamilyType;
	Encode.m_HYOS2.panose.SerifStyle = Original.m_HYOS2.panose.SerifStyle;
	Encode.m_HYOS2.panose.Weight = Original.m_HYOS2.panose.Weight;
	Encode.m_HYOS2.panose.Proportion = Original.m_HYOS2.panose.Proportion;
	Encode.m_HYOS2.panose.Contrast = Original.m_HYOS2.panose.Contrast;
	Encode.m_HYOS2.panose.StrokeVariation = Original.m_HYOS2.panose.StrokeVariation;
	Encode.m_HYOS2.panose.ArmStyle = Original.m_HYOS2.panose.StrokeVariation;
	Encode.m_HYOS2.panose.Letterform = Original.m_HYOS2.panose.Letterform;
	Encode.m_HYOS2.panose.Midline = Original.m_HYOS2.panose.Midline;
	Encode.m_HYOS2.panose.XHeight = Original.m_HYOS2.panose.XHeight;
	Encode.m_HYOS2.usDefaultChar = Original.m_HYOS2.usDefaultChar;
	Encode.m_HYOS2.usBreakChar = Original.m_HYOS2.usBreakChar;
	Encode.m_HYOS2.usMaxContext = Original.m_HYOS2.usMaxContext;

	Encode.m_HYPost.italicAngle.value = Original.m_HYPost.italicAngle.value;
	Encode.m_HYPost.italicAngle.fract = Original.m_HYPost.italicAngle.fract;
	Encode.m_HYPost.underlineThickness = Original.m_HYPost.underlineThickness;
	Encode.m_HYPost.underlinePosition = Original.m_HYPost.underlinePosition;
	Encode.m_HYPost.isFixedPitch = Original.m_HYPost.isFixedPitch;

	if (!::XSysproxy().m_tagOpeionPrm.bCmplCMAP) {
		Encode.m_HYCmap = Original.m_HYCmap;
		Encode.SetCmapEntry(Original.m_HYCmap.vtCamp_tb_entry);
	}

	if (!::XSysproxy().m_tagOpeionPrm.bFontname) {
		Encode.m_HYName = Original.m_HYName;
	}

}	// end of void CXSysProxy::SetEncodeOption()
