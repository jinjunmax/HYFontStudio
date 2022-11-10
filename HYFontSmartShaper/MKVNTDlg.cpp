// MKVNTDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "HYFontSmartShaper.h"
#include "MKVNTDlg.h"
#include "afxdialogex.h"


// CMKVNTDlg 对话框

IMPLEMENT_DYNAMIC(CMKVNTDlg, CDialog)

CMKVNTDlg::CMKVNTDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_FNT_MKVNT_DLG, pParent)
{

}	// end of CMKVNTDlg::CMKVNTDlg()

CMKVNTDlg::~CMKVNTDlg()
{

}	// end of CMKVNTDlg::~CMKVNTDlg()

void CMKVNTDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

}	// end of void CMKVNTDlg::DoDataExchange()

void CMKVNTDlg::InitParam()
{
	m_sheet.Create(this, WS_CHILD | WS_VISIBLE, 0);
	//使Tab键消息有效，可以通过Tab键转移输入焦点
	m_sheet.ModifyStyleEx(0, WS_EX_CONTROLPARENT| WS_EX_CLIENTEDGE);
	m_sheet.ModifyStyle(0, WS_TABSTOP);
	CRect rcSheet;
	this->GetClientRect(rcSheet);
	m_sheet.SetWindowPos(NULL, rcSheet.left, rcSheet.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);

	m_MKVFntCodec.SetDefault();

}	// end of void	CMKVNTDlg::InitParam()

void	CMKVNTDlg::UninitParam()
{
	
}	// end of void	CMKVNTDlg::UninitParam()

BEGIN_MESSAGE_MAP(CMKVNTDlg, CDialog)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_FNT_MKVNT_MK_BTN, &CMKVNTDlg::OnBnClickedFntMkvntMkBtn)
END_MESSAGE_MAP()

// CMKVNTDlg 消息处理程序
BOOL CMKVNTDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	InitParam();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE

}	// end of BOOL CMKVNTDlg::OnInitDialog()

void CMKVNTDlg::OnDestroy()
{
	CDialog::OnDestroy();

	::XSysproxy().Instances.m_vtinstances.clear();
	::XSysproxy().Masters.m_vtMaster.clear();
	::XSysproxy().AxisRecords.Setdefault();

}	// end of void CMKVNTDlg::OnDestroy()

void CMKVNTDlg::OnBnClickedFntMkvntMkBtn()
{
	CWaitCursor wc;	

	DecodeMastersFont();
	if (!CheckMaster())
	{
		AfxMessageBox("字体验证失败！");
		return;
	}

	makeFontName();
	makefvar(m_MKVFntCodec.m_HYfvar);
	makegvar(m_MKVFntCodec.m_HYgvar);
	makeSTAT(m_MKVFntCodec.m_HYStat);

	CHAR szFilters[] = _T("true type 文件(*.ttf)|*.ttf||");
	CFileDialog  saveFileDlg(FALSE, _T(""), _T(""), OFN_LONGNAMES, szFilters);
	if (saveFileDlg.DoModal() != IDOK)	return;
	CString strDstFileName = saveFileDlg.GetPathName();

	if (::HY_FileExist((LPTSTR)(LPCTSTR)strDstFileName)>0){

		int rtn = AfxMessageBox("存在同名文件，请问是否删除？", MB_YESNO);
		if (rtn == IDNO)
			return;
		
		if (::DeleteFile(strDstFileName)==0){
			AfxMessageBox("无法正确删除文件！");
			return;
		}
	}

	std::vector<unsigned long> ulTableFlag;
	ulTableFlag.push_back(CMAP_TAG);
	ulTableFlag.push_back(DSIG_TAG);
	ulTableFlag.push_back(HEAD_TAG);
	ulTableFlag.push_back(HHEA_TAG);
	ulTableFlag.push_back(HMTX_TAG);
	ulTableFlag.push_back(MAXP_TAG);
	ulTableFlag.push_back(NAME_TAG);
	ulTableFlag.push_back(OS2_TAG);
	ulTableFlag.push_back(POST_TAG);
	ulTableFlag.push_back(VHEA_TAG);
	ulTableFlag.push_back(VMTX_TAG);
	ulTableFlag.push_back(GASP_TAG);
	ulTableFlag.push_back(GLYF_TAG);
	ulTableFlag.push_back(LOCA_TAG);
	ulTableFlag.push_back(PREP_TAG);
	ulTableFlag.push_back(STAT_TAG);
	ulTableFlag.push_back(FVAR_TAG);
	ulTableFlag.push_back(GVAR_TAG);

	CVrMaster* pMaster = ::XSysproxy().Masters.GetDefaultMater();
	if (pMaster){
		size_t stGNum = pMaster->m_FontCodec.m_vtHYGlyphs.size();
		for (int i = 0; i < stGNum; i++){
			CHYGlyph&  defGlyph = pMaster->m_FontCodec.m_vtHYGlyphs[i];
			m_MKVFntCodec.m_vtHYGlyphs.push_back(defGlyph);
			m_MKVFntCodec.m_HYPost.InsertName(defGlyph.strPostName);
		}

		if (m_MKVFntCodec.Encode((LPTSTR)(LPCTSTR)strDstFileName, ulTableFlag, ::XSysproxy().m_tagOpeionPrm) == HY_NOERROR)
			AfxMessageBox(_T("字库生成完成"));
		else
			AfxMessageBox(_T("字库生成失败"));
	}

}	// end of void CMKVNTDlg::OnBnClickedFntMkvntMkBtn()

void CMKVNTDlg::makefvar(CHYfvar& fvr)
{
	unsigned short  usAxisCount = (unsigned short)::XSysproxy().AxisRecords.vtVrAxisInfs.size();
	unsigned short  usInstanceCount = (unsigned short)::XSysproxy().Instances.m_vtinstances.size();
	unsigned short  usInstanceRecordSize = 2 + 2 + 4 * usAxisCount + 2;

	fvr.majorVersion = 1;
	fvr.minorVersion = 0;

	fvr.axisCount = usAxisCount;
	fvr.axisSize = usAxisCount * 20;
	fvr.instanceCount = usInstanceCount;
	fvr.instanceSize = usInstanceCount * usInstanceRecordSize;

	CVrAxisRecords&	vrAxisRecords = ::XSysproxy().AxisRecords;
	for (unsigned short i = 0; i < usAxisCount; i++){
		CHYFvarAxisRecord	AxsRecord;		
		if (vrAxisRecords.vtVrAxisInfs[i].m_strTag == AXES_WEIGHT) {
			AxsRecord.Tag = AXIS_TAG_WGHT;			
		}
		if (vrAxisRecords.vtVrAxisInfs[i].m_strTag == AXES_WIDTH) {
			AxsRecord.Tag = AXIS_TAG_WDTH;			
		}
		CountAxesValue(vrAxisRecords.vtVrAxisInfs[i].m_strTag, AxsRecord.minValue, AxsRecord.defaultValue, AxsRecord.maxValue);
		AxsRecord.axisNameID = vrAxisRecords.vtVrAxisInfs[i].m_iNameID;
		AxsRecord.flags = vrAxisRecords.vtVrAxisInfs[i].m_bHidden;
		
		fvr.vtaxes.push_back(AxsRecord);
	}

	for (int i = 0; i < usInstanceCount; i++) {
		CVrInstance& vrInstance = ::XSysproxy().Instances.m_vtinstances[i];
		CHYFvarInstncRecord  FvarInstnc;

		FvarInstnc.subfamilyNameID = vrInstance.m_iNameID;
		for (int j = 0; j < usAxisCount; j++) {
			CVrAxisValue& axsValue = vrInstance.m_AxisRecords.vtVrAxisInfs[j].m_AxisValue;
			HYFIXED fxd = ::HY_float_to_HYFIXED(axsValue.Value);
			FvarInstnc.coordinates.push_back(fxd);
		}

		fvr.vtinstances.push_back(FvarInstnc);
	}

} // end of void CMKVNTDlg::makefvar()

void CMKVNTDlg::CountAxesValue(CString strAxisTag, HYFIXED& outminValue, HYFIXED& outdefaultValue, HYFIXED& outmaxValue)
{
	unsigned short  usMasterCount = (unsigned short)::XSysproxy().Masters.m_vtMaster.size();

	double dbminValue = 99999999.0, dbdefaultValue = 0.0, dbmaxValue = 0.0;

	for (unsigned short i = 0; i < usMasterCount; i++) {
		CVrMaster& vrMasterRecord = ::XSysproxy().Masters.m_vtMaster[i];
		
		unsigned short  usAxisInfsSize = (unsigned short)vrMasterRecord.m_AxisRecords.vtVrAxisInfs.size();
		for (unsigned short j = 0; j < usAxisInfsSize; j++) {
			CVrAxisRecord& vrAxisRecord =  vrMasterRecord.m_AxisRecords.vtVrAxisInfs[j];
			if (vrAxisRecord.m_strTag == AXES_WEIGHT) {
				if (vrMasterRecord.m_bDefault){
					dbdefaultValue = vrAxisRecord.m_AxisValue.Value;
				}
				else
				{
					if (vrAxisRecord.m_AxisValue.Value < dbminValue){
						dbminValue = vrAxisRecord.m_AxisValue.Value;
					}

					if (vrAxisRecord.m_AxisValue.Value > dbmaxValue) {
						dbmaxValue = vrAxisRecord.m_AxisValue.Value;
					}
				}
			}

			if (vrAxisRecord.m_strTag == AXES_WIDTH) {
				if (vrMasterRecord.m_bDefault) {
					dbdefaultValue = vrAxisRecord.m_AxisValue.Value;
				}
				else
				{
					if (vrAxisRecord.m_AxisValue.Value < dbminValue) {
						dbminValue = vrAxisRecord.m_AxisValue.Value;
					}

					if (vrAxisRecord.m_AxisValue.Value > dbmaxValue) {
						dbmaxValue = vrAxisRecord.m_AxisValue.Value;
					}
				}
			}
		}
	}

	outminValue = HY_float_to_HYFIXED((float)dbminValue);
	outdefaultValue = HY_float_to_HYFIXED((float)dbdefaultValue);
	outmaxValue = HY_float_to_HYFIXED((float)dbmaxValue);

}	// end of void CMKVNTDlg::CountAxesValue()

void CMKVNTDlg::makeSTAT(CHYSTAT& Stat)
{
	Stat.majorVersion = 1;
	Stat.minorVersion = 1;
	Stat.designAxisSize = 8; // axisTag + axisNameID + axisOrdering
	Stat.designAxisCount = m_MKVFntCodec.m_HYfvar.axisCount;
	Stat.designAxesOffset = 20;
	Stat.axisValueCount = 0;
	Stat.offsetToAxisValueOffsets = 0;
	Stat.elidedFallbackNameID = 2;

	for (int i = 0; i < Stat.designAxisCount; i++){
		CHYAxisRecord	AxsRcrd;
		AxsRcrd.axisNameID = m_MKVFntCodec.m_HYfvar.vtaxes[i].axisNameID;
		AxsRcrd.axisTag = m_MKVFntCodec.m_HYfvar.vtaxes[i].Tag;
		AxsRcrd.axisOrdering = i;

		Stat.vtdesignAxes.push_back(AxsRcrd);
	}	
	
	for (int i = 0; i < Stat.designAxisCount; i++) {
		int iaxisOrder = i;
		for (int j = 0; j < m_MKVFntCodec.m_HYfvar.instanceCount; j++){
			CHYAxisvalueFormat	AxsVlue;
			::XSysproxy().Instances.m_vtinstances[j].m_AxisRecords;

			CHYFvarInstncRecord& InstncRecrd = m_MKVFntCodec.m_HYfvar.vtinstances[j];

			bool b = true;
			for (int x = 0; x < InstncRecrd.coordinates.size(); x++){
				if (x != i)
				{
					if (InstncRecrd.coordinates[x].value != 0 && InstncRecrd.coordinates[x].fract != 0)
						b = false;
				}
			}

			if (b){
				CHYAxisvalueFormat   AxVluFormat;
				AxVluFormat.format = 1;
				AxVluFormat.axisIndex = i;
				AxVluFormat.flags = 0;
				AxVluFormat.valueNameID = InstncRecrd.subfamilyNameID;
				AxVluFormat.fxValue.value = InstncRecrd.coordinates[i].value;
				AxVluFormat.fxValue.fract = InstncRecrd.coordinates[i].fract;
				Stat.vtAxisvalueFormat.push_back(AxVluFormat);
			}
		}
	}
	Stat.axisValueCount = (unsigned short)Stat.vtAxisvalueFormat.size();

}	// end of void CMKVNTDlg::makeSTAT()

void CMKVNTDlg::makegvar(CHYgvar& gvar)
{	
	size_t stMasterSize = ::XSysproxy().Masters.m_vtMaster.size();
	if (stMasterSize == 0) return;

	Normalizedcoordinate(::XSysproxy().Masters);
	MakeMasterDeltas(::XSysproxy().Masters);

	gvar.majorVersion = 1;
	gvar.minorVersion = 0;
	gvar.axisCount = (short)::XSysproxy().AxisRecords.vtVrAxisInfs.size();
	MakeSharedTuple(gvar.sharedTuples, ::XSysproxy().Masters);
	gvar.sharedTupleCount = (unsigned short)gvar.sharedTuples.size();
	gvar.offsetToSharedTuples = 0;
	gvar.glyphCount = (unsigned short)::XSysproxy().Masters.m_vtMaster[0].m_FontCodec.m_HYMaxp.numGlyphs;
	gvar.flags = 1;//bit 0 is set, the offsets are uint32.
	gvar.offsetToGlyphVariationData = 0;
	gvar.vtOffsets.resize(gvar.glyphCount+1,0);

	for (int i = 0; i < gvar.glyphCount; i++)
	{
		glyphVariationData gvartnData;

		// 即使没有共享点，也用0占位。	
		gvartnData.tupleVariationCount = (0x8000|gvar.sharedTupleCount);
		MakeGlyphVariationData(gvartnData, ::XSysproxy().Masters, i);
		gvar.vtGlyphVariationData.push_back(gvartnData);
	}

}	// end of void CMKVNTDlg::makegvar()

void CMKVNTDlg::MakeGlyphVariationData(glyphVariationData& gvartnData, CVrMasters& Masters, int iGlyphID)
{	
	// head
	gvartnData.SharepointNumers.usPointNumer = 0;
	gvartnData.SharepointNumers.vtPointIndex.clear();
	
	gvartnData.offsetToData = 0;
	for (int i = 0; i < (gvartnData.tupleVariationCount&COUNT_MASK); i++) {
		CVrMaster& Master = ::XSysproxy().Masters.m_vtMaster[i];
		MakeTupleVariationHeader(gvartnData, Master, i, iGlyphID);
	}
	MakeSerializeddata(gvartnData, Masters, iGlyphID);	

}	//end of bool CMKVNTDlg::MakeGlyphVariationData()

void CMKVNTDlg::MakeTupleVariationHeader(glyphVariationData& gvartnData, CVrMaster& Master, short sSharTuple, int iGlyphID)
{
	TupleVariationHeader	tupleHeader;
	//tupleHeader.tupleIndex = (0x2000 | sSharTuple);
	tupleHeader.tupleIndex = sSharTuple;
	tupleHeader.variationDataSize = 0;
	tupleHeader.iSharedTupleInx = sSharTuple;
	gvartnData.vttupleVariationHeaders.push_back(tupleHeader);

}	// end of bool CMKVNTDlg::MakeTupleVariationHeader()

void CMKVNTDlg::MakeSerializeddata(glyphVariationData& gvartnData, CVrMasters& Masters, int iGlyphID)
{
	size_t stVrtnCunt = gvartnData.tupleVariationCount & 0x0FFF;
	size_t MasterIndex = 0;	
	int iTupleIndex = 0;
	while (MasterIndex <Masters.m_vtMaster.size())
	{
		if (Masters.m_vtMaster[MasterIndex].m_bDefault)
		{
			MasterIndex++;
			continue;
		}

		TupleVariationHeader& tplVrHead = gvartnData.vttupleVariationHeaders[iTupleIndex];
		//private point Number
		tplVrHead.prvtpointNumbers.usPointNumer = 0;
		tplVrHead.prvtpointNumbers.vtPointIndex.clear();

		CVrMaster& Mstr = Masters.m_vtMaster[MasterIndex];
		//Packed Deltas
		size_t stSize = Mstr.m_vrGlyphs.vtVrGlyphs[iGlyphID].vtX.size();
		for (size_t j = 0; j < stSize; j++) {
			tplVrHead.vtVariationPoint_X.push_back(Mstr.m_vrGlyphs.vtVrGlyphs[iGlyphID].vtX[j]);
		}

		stSize = Mstr.m_vrGlyphs.vtVrGlyphs[iGlyphID].vtY.size();
		for (size_t j = 0; j < stSize; j++) {
			tplVrHead.vtVariationPoint_Y.push_back(Mstr.m_vrGlyphs.vtVrGlyphs[iGlyphID].vtY[j]);
		}

		iTupleIndex++;
		MasterIndex++;
	}

	/*
	for (size_t i = 0; i < stVrtnCunt; i++) {		
		TupleVariationHeader& tplVrHead = gvartnData.vttupleVariationHeaders[i];

		//private point Number
		tplVrHead.prvtpointNumbers.usPointNumer = 0;
		tplVrHead.prvtpointNumbers.vtPointIndex.clear();

		//Packed Deltas
		size_t stSize = Masters.m_vtMaster[i].m_vrGlyphs.vtVrGlyphs[iGlyphID].vtX.size();
		for (size_t j = 0; j < stSize; j++){
			tplVrHead.vtVariationPoint_X.push_back(Masters.m_vtMaster[i].m_vrGlyphs.vtVrGlyphs[iGlyphID].vtX[j]);
		}

		stSize = Masters.m_vtMaster[i].m_vrGlyphs.vtVrGlyphs[iGlyphID].vtY.size();
		for (size_t j = 0; j < stSize; j++) {
			tplVrHead.vtVariationPoint_Y.push_back(Masters.m_vtMaster[i].m_vrGlyphs.vtVrGlyphs[iGlyphID].vtY[j]);
		}
	}
	*/

}	// end of bool CMKVNTDlg::MakeSerializeddata()

void CMKVNTDlg::PackedDeltas(std::vector<unsigned char>& outData, std::vector<short>& invtOffset)
{
	// 0 初始状态 1 单字节 2 双字节 3 Deltas 值为0
	char ctrlType = 0;
	size_t i = 0;
	size_t stSize = invtOffset.size();
	short s = 0;
	char segment = 0;
	int iCtrlPos = 0;
	char tmp = 0;
	while(i<stSize){
		s = invtOffset[i];
		i++;
		if (ctrlType == 0){
			if (s > 127 || s < -127){
				ctrlType = 2;
				// control byte 双字节
				outData.push_back(0x40);
				iCtrlPos = outData.size() - 1;

				// delta
				tmp = (char)(s>>8);
				outData.push_back(tmp);
				tmp = (char)(s&0x00ff);
				outData.push_back(tmp);				
			}else if (s == 0){
				ctrlType = 3;

				// control byte
				outData.push_back(0x00);
				iCtrlPos = outData.size() - 1;
			}
			else{
				ctrlType = 1;
				// control byte 单字节
				outData.push_back(0x00);
				iCtrlPos = outData.size() - 1;

				// delta
				tmp = (char)s;
				outData.push_back(tmp);
			}
			segment++;
		}

		if (ctrlType == 1){
			if (s > 127 || s < -127) {
				tmp = outData[iCtrlPos];
				tmp |= (segment-1);
				outData[iCtrlPos] = tmp;
				segment = 0;

				ctrlType = 2;
				// control byte 双字节
				outData.push_back(0x40);
				iCtrlPos = outData.size() - 1;

				// delta
				tmp = (char)(s >> 8);
				outData.push_back(tmp);
				tmp = (char)(s & 0x00ff);
				outData.push_back(tmp);
			}
			else if (s == 0) {
				tmp = outData[iCtrlPos];
				tmp |= (segment - 1);
				outData[iCtrlPos] = tmp;
				segment = 0;

				ctrlType = 3;
				// control byte
				outData.push_back(0x00);
				iCtrlPos = outData.size() - 1;
			}
			else {

				if (segment == 64){					
					tmp = (segment - 1);
					outData[iCtrlPos] = tmp;
					segment = 0;
				}

				// control byte 单字节
				ctrlType = 1;
				outData.push_back(0x00);
				iCtrlPos = outData.size() - 1;

				// delta
				tmp = (char)s;
				outData.push_back(tmp);
			}
			segment++;
		}

		if (ctrlType == 2) {
			if (s > 127 || s < -127) {
				if (segment == 64) {
					tmp = outData[iCtrlPos];
					tmp |= (segment - 1);
					outData[iCtrlPos] = tmp;
					segment = 0;
				}

				ctrlType = 2;
				// control byte 双字节
				outData.push_back(0x40);
				iCtrlPos = outData.size() - 1;

				// delta
				tmp = (char)(s >> 8);
				outData.push_back(tmp);
				tmp = (char)(s & 0x00ff);
				outData.push_back(tmp);
			}
			else if (s == 0) {
				tmp = outData[iCtrlPos];
				tmp |= (segment - 1);
				outData[iCtrlPos] = tmp;
				segment = 0;

				ctrlType = 3;
				// control byte
				outData.push_back(0x00);
				iCtrlPos = outData.size() - 1;
			}
			else {				
				tmp = outData[iCtrlPos];
				tmp |= (segment - 1);
				outData[iCtrlPos] = tmp;
				segment = 0;

				ctrlType = 1;
				// control byte 单字节
				outData.push_back(0x00);
				iCtrlPos = outData.size() - 1;

				// delta
				tmp = (char)s;
				outData.push_back(tmp);
			}
			segment++;
		}

		if (ctrlType == 3) {
			if (s > 127 || s < -127) {
				tmp = outData[iCtrlPos];
				tmp |= (segment - 1);
				outData[iCtrlPos] = tmp;
				segment = 0;

				ctrlType = 2;
				// control byte 双字节
				outData.push_back(0x40);
				iCtrlPos = outData.size() - 1;

				// delta
				tmp = (char)(s >> 8);
				outData.push_back(tmp);
				tmp = (char)(s & 0x00ff);
				outData.push_back(tmp);
			}
			else if (s == 0) {
				if (segment == 64) {
					tmp = outData[iCtrlPos];
					tmp |= (segment - 1);
					outData[iCtrlPos] = tmp;
					segment = 0;
				}

				ctrlType = 3;
				// control byte
				outData.push_back(0x00);
				iCtrlPos = outData.size() - 1;
			}
			else {
				tmp = outData[iCtrlPos];
				tmp |= (segment - 1);
				outData[iCtrlPos] = tmp;
				segment = 0;

				ctrlType = 1;
				// control byte 单字节
				outData.push_back(0x00);
				iCtrlPos = outData.size() - 1;

				// delta
				tmp = (char)s;
				outData.push_back(tmp);
			}
			segment++;
		}
	}

}	// end of void CMKVNTDlg::PackedDeltas()

bool CMKVNTDlg::MakeMasterDeltas(CVrMasters& Masters)
{
	CVrMaster*  defMaster = Masters.GetDefaultMater();
	if (defMaster == NULL) return false;

	for (int j = 0; j < ::XSysproxy().Masters.m_vtMaster.size(); j++)
	{
		CVrMaster& Mstr = ::XSysproxy().Masters.m_vtMaster[j];
		if (Mstr.m_bDefault) continue;

		for (int x = 0; x < Mstr.m_AxisRecords.vtVrAxisInfs.size(); x++)
		{
			CString strTag = Mstr.m_AxisRecords.vtVrAxisInfs[x].m_strTag;
			float fvalue = Mstr.m_AxisRecords.vtVrAxisInfs[x].m_AxisValue.Value;
			float fdefvalue = defMaster->m_AxisRecords.vtVrAxisInfs[x].m_AxisValue.Value;

			CVrMaster* pMaster = NULL;
			if (fvalue > fdefvalue)
			{ 
				pMaster = ::XSysproxy().Masters.GetRangMaxMaster(strTag, fvalue);
				if (pMaster != NULL){
					MakeGlphysDelta(*defMaster, *pMaster);
					MakeFinalGlpyhDelta(*defMaster, *pMaster);
				}
			}
			else
			{
				pMaster = ::XSysproxy().Masters.GetRangMinMaster(strTag, fvalue);
				if (pMaster != NULL) {
					MakeGlphysDelta(*defMaster, *pMaster);
					MakeFinalGlpyhDelta(*defMaster, *pMaster);
				}
			}
		}
	}

	return true;

}	// end of void CMKVNTDlg::MakeMasterDeltas()

void CMKVNTDlg::MakeFinalGlpyhDelta(CVrMaster& defaultMaster, CVrMaster& inMaster)
{
	CHYFontCodec&  defultFntCodec = defaultMaster.m_FontCodec;
	CHYFontCodec&  inFntCodec = inMaster.m_FontCodec;

	std::vector<CHYGlyph>& vtdefultGlyphs = defultFntCodec.m_vtHYGlyphs;
	std::vector<CHYGlyph>& vtinGlyphs = inFntCodec.m_vtHYGlyphs;

	// 得出与默认母版直接的差值
	CVrGlyphs  vrGlyphs;
	size_t stGlyphs = vtdefultGlyphs.size();
	for (size_t i = 0; i < stGlyphs; i++) {
		CHYGlyph&  defultGlyph = vtdefultGlyphs[i];
		CHYGlyph&  inGlyph = vtinGlyphs[i];
		CVrGlyph   outVrGlyph;
		MakeGlphyDelta(defultGlyph, inGlyph, outVrGlyph);
		vrGlyphs.vtVrGlyphs.push_back(outVrGlyph);
	}

	CVrGlyphs  vrFinalGlyph;	
	if (inMaster.IsSingleAxis()){
		::XSysproxy().SumVrGlyphs(inMaster.m_vtoffsetGlyphs, inMaster.m_vrGlyphs);
	}
	else{

		CVrGlyphs sumGlyphs;
		::XSysproxy().SumVrGlyphs(inMaster.m_vtoffsetGlyphs, sumGlyphs);
		::XSysproxy().SubVrGlyphs(sumGlyphs, vrGlyphs, inMaster.m_vrGlyphs);
	}


}	// end of void CMKVNTDlg::MakeFinalGlpyhDelta()

void CMKVNTDlg::MakeGlphysDelta(CVrMaster& defaultMaster, CVrMaster& inMaster)
{
	CHYFontCodec&  defultFntCodec = defaultMaster.m_FontCodec;
	CHYFontCodec&  inFntCodec = inMaster.m_FontCodec;

	std::vector<CHYGlyph>& vtdefultGlyphs = defultFntCodec.m_vtHYGlyphs;
	std::vector<CHYGlyph>& vtinGlyphs = inFntCodec.m_vtHYGlyphs;

	CVrGlyphs  vrGlyphs;
	size_t stGlyphs = vtdefultGlyphs.size();
	for (size_t i = 0; i < stGlyphs; i++){
		CHYGlyph&  defultGlyph = vtdefultGlyphs[i];
		CHYGlyph&  inGlyph = vtinGlyphs[i];
		CVrGlyph	outVrGlyph;
		MakeGlphyDelta(defultGlyph, inGlyph, outVrGlyph);
		vrGlyphs.vtVrGlyphs.push_back(outVrGlyph);
	}

	inMaster.m_vtoffsetGlyphs.push_back(vrGlyphs);

}	// end of void CMKVNTDlg::MakeGlphysDelta()

void CMKVNTDlg::MakeGlphyDelta(CHYGlyph& defultGlyph, CHYGlyph &inGlyph, CVrGlyph& outVrGlyph)
{	
	size_t stContour = defultGlyph.vtContour.size();

	for (size_t i = 0; i < stContour; i++){
		CHYContour& defultCntour = defultGlyph.vtContour[i];
		CHYContour& inCntour = inGlyph.vtContour[i];

		size_t stPnts = defultCntour.vtHYPoints.size();
		for (size_t j = 0; j < stPnts; j++){
			CHYPoint& defultPnt = defultCntour.vtHYPoints[j];
			CHYPoint& inPnt = inCntour.vtHYPoints[j];

			short dltx = inPnt.x - defultPnt.x;
			short dlty = inPnt.y - defultPnt.y;

			outVrGlyph.vtX.push_back(dltx);
			outVrGlyph.vtY.push_back(dlty);
		}
	}	

	AddPhantomPoint(defultGlyph, inGlyph, outVrGlyph);	

}	// end of void CMKVNTDlg::MakeGlphyDelta()

void CMKVNTDlg::AddPhantomPoint(CHYGlyph& defultGlyph, CHYGlyph& inGlyph, CVrGlyph& outVrGlyph)
{
	short x = 0, y = 0;	

	// lsb 
	outVrGlyph.vtX.push_back(0);
	outVrGlyph.vtY.push_back(0);

	// rsb 
	x = inGlyph.advanceWidth - defultGlyph.advanceWidth;
	outVrGlyph.vtX.push_back(x);
	outVrGlyph.vtY.push_back(0);

	// tsb
	outVrGlyph.vtX.push_back(0);
	outVrGlyph.vtY.push_back(0);

	// bsb
	y = inGlyph.advanceHeight - defultGlyph.advanceHeight;
	outVrGlyph.vtX.push_back(0);
	outVrGlyph.vtY.push_back(y);

}	// end of void CMKVNTDlg::AddPhantomPoint()

void CMKVNTDlg::MakeSharedTuple(std::vector<CHYTuple>& sharedTuples, CVrMasters& Masters)
{
	for (size_t i = 0; i < Masters.m_vtMaster.size(); i++) {
		CVrMaster& Master = Masters.m_vtMaster[i];
		if (Master.m_bDefault) continue;

		CHYTuple Tuple;
		for (size_t j=0; j<Master.m_tupNormCoordinates.size(); j++){
			short f2t14 = HY_float_to_F2DOT14(Master.m_tupNormCoordinates[j]);
			Tuple.coordinates.push_back(f2t14);
		}
		sharedTuples.push_back(Tuple);
	}

}	// end of void CMKVNTDlg::MakeSharedTuple()

void CMKVNTDlg::Normalizedcoordinate(CVrMasters& Masters)
{
	CVrAxisRecords& Axsrcrd = ::XSysproxy().AxisRecords;
	
	for (size_t i = 0; i<Axsrcrd.vtVrAxisInfs.size(); i++){
		CString strTag = Axsrcrd.vtVrAxisInfs[i].m_strTag;		

		float fDefault = Masters.GetAxisDefault(strTag);
		float fAxisMax = Masters.GetAxisMax(strTag);
		float fAxisMin = Masters.GetAxisMin(strTag);

		for (size_t j = 0; j < Masters.m_vtMaster.size(); j++){
			CVrMaster& Master =  Masters.m_vtMaster[j];
			int iIndex = Master.m_AxisRecords.FindAxisIndexEx(strTag);
			float fMasterValue = Master.m_AxisRecords.vtVrAxisInfs[iIndex].m_AxisValue.Value;
			float fNrmlz = Normalize(fDefault, fAxisMax, fAxisMin, fMasterValue);
			Master.m_tupNormCoordinates.push_back(fNrmlz);
		}
	}

}	// end of void CMKVNTDlg::Normalizedcoordinate()

float  CMKVNTDlg::Normalize(float fDefault, float fMax,float fMin,float fInValue)
{
	float  normalizedValue = 0.0f;

	if (fInValue < fMin)
		fInValue = fMin;
	if (fInValue > fMax)
		fInValue = fMax;

	if (fInValue < fDefault){
		normalizedValue = -(fDefault - fInValue) / (fDefault - fMin);
	}
	else if (fInValue > fDefault){
		normalizedValue = (fInValue - fDefault) / (fMax - fDefault);
	}
	
	return	normalizedValue;

}	// end of float  CMKVNTDlg::Normalize()

void CMKVNTDlg::DecodeMastersFont()
{
	CVrMasters& Masters = ::XSysproxy().Masters;
	size_t stMaster = ::XSysproxy().Masters.m_vtMaster.size();
	for (size_t i = 0; i < stMaster; i++)
	{
		CVrMaster& Master = Masters.m_vtMaster[i];
		Master.m_FontCodec.Decode((LPTSTR)(LPCTSTR)Master.m_strFileName);
	}

}	// end of void CMKVNTDlg::DecodeMastersFont()

bool CMKVNTDlg::CheckMaster()
{
	bool brtn = true;
	CVrMasters& Masters = ::XSysproxy().Masters;
	CVrMaster* defMaster = ::XSysproxy().Masters.GetDefaultMater();
	if (defMaster != NULL)
	{
		for (size_t i = 0; i < ::XSysproxy().Masters.m_vtMaster.size(); i++)
		{
			CVrMaster& Master = Masters.m_vtMaster[i];
			if (Master.m_bDefault) continue;
			std::vector<FntCheckResult> vtResult;
			::XSysproxy().CheckFont(defMaster->m_FontCodec, Master.m_FontCodec, vtResult);

			if (vtResult.size() > 0){
				TRACE("\nFont Name = %s\n", Master.m_strFontName);
				for (size_t t = 0; t < vtResult.size(); t++){
					if (vtResult[t].iFlag == 0) {
						TRACE("GID = %d, 字形点数量与默认字形不等\n", vtResult[t].iGlyphIndex);
					}
					if (vtResult[t].iFlag == 1) {
						TRACE("GID = %d, 字形轮廓数量与默认字形不等\n", vtResult[t].iGlyphIndex);
					}
					if (vtResult[t].iFlag == 2){
						TRACE("GID = %d, 点属性与默认字形不符\n", vtResult[t].iGlyphIndex);
						for (size_t e = 0; e<vtResult[t].vtPointIndex.size(); e++){
							TRACE("Point ID = %d\n", vtResult[t].vtPointIndex[e]);						
						}
					}
				}
				brtn = false;
			}
		}
	}	
	else 
		return false;

	return brtn;

}	// end of void CMKVNTDlg::CheckMaster()

void CMKVNTDlg::makeFontName()
{
	CHYName& name = m_MKVFntCodec.m_HYName;
	name.SetDefault();

	makeEnglishName(1, 0, 0, name.vtNameRecord);
	makeVrName(1, 0, 0, name.vtNameRecord);
	makeChineseName(1, 25, 33, name.vtNameRecord);
	makeVrName(1, 25, 33, name.vtNameRecord);
	makeEnglishName(3, 1, 1033, name.vtNameRecord);
	makeVrName(3, 1, 1033, name.vtNameRecord);
	makeChineseName(3, 1, 2052, name.vtNameRecord);
	makeVrName(3, 1, 2052, name.vtNameRecord);

}	// end of void CMKVNTDlg::makeFontName()

void CMKVNTDlg::makeEnglishName(unsigned short	platformID, unsigned short	encodingID, unsigned short	languageID, std::vector<NAMERECORD>	& vtNameRecord)
{	
	for (int i = 0; i < 11; i++) {
		NAMERECORD	nameRecord;
		nameRecord.platformID = platformID;
		nameRecord.encodingID = encodingID;
		nameRecord.languageID = languageID;
		nameRecord.nameID = i;

		CString strUniqeidf, strTrademark;

		switch (i) {
		case 0:
			nameRecord.strContent = ::XSysproxy().VrFntName.m_strENGCopyright;
			vtNameRecord.push_back(nameRecord);
			break;
		case 1:
			nameRecord.strContent = ::XSysproxy().VrFntName.m_strENGFamilyName;
			vtNameRecord.push_back(nameRecord);
			break;
		case 2:
			nameRecord.strContent = "Regular";
			vtNameRecord.push_back(nameRecord);
			break;
		case 3:
			strUniqeidf.Format("Hanyi %s %s", ::XSysproxy().VrFntName.m_strENGFamilyName, ::XSysproxy().VrFntName.m_strVersion);
			nameRecord.strContent = strUniqeidf;
			vtNameRecord.push_back(nameRecord);
			break;
		case 4:
			nameRecord.strContent = ::XSysproxy().VrFntName.m_strENGFamilyName;
			vtNameRecord.push_back(nameRecord);
			break;
		case 5:
			nameRecord.strContent = ::XSysproxy().VrFntName.m_strVersion;
			vtNameRecord.push_back(nameRecord);
			break;
		case 6:
			nameRecord.strContent = ::XSysproxy().VrFntName.m_strPostScriptName;
			vtNameRecord.push_back(nameRecord);
			break;
		case 7:
			strTrademark.Format("%s is a trademark of Beijing Hanyi Innovation Technology Co.,Ltd.",::XSysproxy().VrFntName.m_strENGFamilyName);
			nameRecord.strContent = strTrademark;
			vtNameRecord.push_back(nameRecord);
			break;
		case 8:
			nameRecord.strContent = "Hanyi Fonts";
			vtNameRecord.push_back(nameRecord);
			break;
		case 9:
			nameRecord.nameID = 11;
			nameRecord.strContent = "http://www.hanyi.com.cn/";
			vtNameRecord.push_back(nameRecord);
			break;
		case 10:
			nameRecord.nameID = 13;
			nameRecord.strContent = "Please visit http://hanyi.com.cn/law.php for license information.";
			vtNameRecord.push_back(nameRecord);
			break;
		case 11:
			nameRecord.nameID = 14;
			nameRecord.strContent = "http://hanyi.com.cn/law.php";
			vtNameRecord.push_back(nameRecord);
			break;
		default:
			break;
		}
	}

}	// end of void CMKVNTDlg::makeEnglishName()

void CMKVNTDlg::makeChineseName(unsigned short	platformID, unsigned short	encodingID, unsigned short	languageID, std::vector<NAMERECORD>	& vtNameRecord)
{
	for (int i = 0; i < 7; i++) {
		NAMERECORD	nameRecord;
		nameRecord.platformID = platformID;
		nameRecord.encodingID = encodingID;
		nameRecord.languageID = languageID;		

		CString strTrademark;
		switch (i) {
			case 0:
				nameRecord.nameID = 0;
				nameRecord.strContent = ::XSysproxy().VrFntName.m_strCHSCopyright;
				vtNameRecord.push_back(nameRecord);
				break;
			case 1:
				nameRecord.nameID = 1;
				nameRecord.strContent = ::XSysproxy().VrFntName.m_strCHSFamilyName;
				vtNameRecord.push_back(nameRecord);
				break;
			case 2:
				nameRecord.nameID = 2;
				nameRecord.strContent = "Regular";
				vtNameRecord.push_back(nameRecord);
				break;
			case 3:
				nameRecord.nameID = 4;			
				nameRecord.strContent = ::XSysproxy().VrFntName.m_strCHSFamilyName;
				vtNameRecord.push_back(nameRecord);
				break;
			case 4:
				nameRecord.nameID = 7;
				strTrademark.Format("%s是北京汉仪科印信息技术有限公司的商标。",::XSysproxy().VrFntName.m_strCHSFamilyName);
				nameRecord.strContent = strTrademark;
				vtNameRecord.push_back(nameRecord);
				break;
			case 5:
				nameRecord.nameID = 8;
				nameRecord.strContent ="汉仪字库";
				vtNameRecord.push_back(nameRecord);
				break;
			case 6:
				nameRecord.nameID = 13;
				nameRecord.strContent = "请访问 http://hanyi.com.cn/law.php 了解授权协议。";
				vtNameRecord.push_back(nameRecord);
				break;
			default:
				break;
		}
	}

}	// end of void CMKVNTDlg::makeChineseName()

void CMKVNTDlg::makeVrName(unsigned short	platformID, unsigned short	encodingID, unsigned short	languageID, std::vector<NAMERECORD>	& vtNameRecord)
{
	NAMERECORD	nameRecord;
	nameRecord.platformID = platformID;
	nameRecord.encodingID = encodingID;
	nameRecord.languageID = languageID;

	// axis name
	unsigned short unNameID = 256;
	nameRecord.nameID = unNameID++;
	nameRecord.strContent = "Regular";
	vtNameRecord.push_back(nameRecord);

	CVrAxisRecords& AxisRecords = ::XSysproxy().AxisRecords;
	for (size_t i = 0; i < AxisRecords.vtVrAxisInfs.size(); i++)
	{	
		if (AxisRecords.vtVrAxisInfs[i].m_strTag == AXES_WEIGHT) {
			nameRecord.nameID = unNameID++;
			nameRecord.strContent = "Weight";
			vtNameRecord.push_back(nameRecord);
			AxisRecords.vtVrAxisInfs[i].m_iNameID = nameRecord.nameID;
		}

		if (AxisRecords.vtVrAxisInfs[i].m_strTag == AXES_WIDTH) {
			nameRecord.nameID = unNameID++;
			nameRecord.strContent = "Width";
			vtNameRecord.push_back(nameRecord);
			AxisRecords.vtVrAxisInfs[i].m_iNameID = nameRecord.nameID;
		}
	}

	// instance name
	CVrInstances& Instances = ::XSysproxy().Instances;
	for (size_t i = 0; i < Instances.m_vtinstances.size(); i++)
	{
		nameRecord.nameID = unNameID++;
		nameRecord.strContent = Instances.m_vtinstances[i].m_strStyleName;
		vtNameRecord.push_back(nameRecord);
		Instances.m_vtinstances[i].m_iNameID = nameRecord.nameID;
	}

}	// end of void CMKVNTDlg::makeVrName()
