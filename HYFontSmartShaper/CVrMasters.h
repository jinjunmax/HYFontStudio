#pragma once
class CVrGlyph
{
public:
	CVrGlyph();
	~CVrGlyph();
public:
	vector<short> vtX;
	vector<short> vtY;
};
class CVrGlyphs
{
	public:
		CVrGlyphs();
		~CVrGlyphs();
	public:
		std::vector<CVrGlyph> vtVrGlyphs;
};


class CVrMaster
{
public:
	CVrMaster();
	~CVrMaster();
public:
	void SetDefault();
	bool IsSingleAxis();

public:
	CString						m_strMasterName;
	CString						m_strFontName;
	CString						m_strFileName;
	CVrAxisRecords				m_AxisRecords;
	std::vector<float>			m_tupNormCoordinates;
	int							m_iAcsender;
	int							m_iDescender;
	int							m_iTypoAscender;
	int							m_iTypoDescender;
	HYFONTCODEC::CHYFontCodec	m_FontCodec;
	bool						m_bDefault;
	std::vector<CVrGlyphs>		m_vtoffsetGlyphs;	// 数组的大小与轴对应
	CVrGlyphs					m_vrGlyphs;		//保存计算后的deltes	
};

class CVrMasters
{
public:
	CVrMasters();
	~CVrMasters();

public:
	std::vector<CVrMaster> m_vtMaster;

public:
	float GetAxisMax(CString strTag);
	float GetAxisMin(CString strTag);
	float GetAxisDefault(CString strTag);
	CVrMaster* GetDefaultMater();
	CVrMaster* GetMaster(std::vector<float>& tupNormCoordinates);
	CVrMaster* GetRangMaxMaster(CString strTag, float axsValue); // axsValue 是轴值，不是归一化数值
	CVrMaster* GetRangMinMaster(CString strTag, float axsValue); // axsvalue 是轴值，不是归一化数值

	
};

class CVrInstance
{
public:
	CVrInstance();
	~CVrInstance();

	void SetDefault();
	bool operator==(const CVrInstance& other);
	bool operator!=(const CVrInstance& other);

public:
	CString			m_strStyleName;
	int				m_iWeight;
	int				m_iWidth;
	CVrAxisRecords	m_AxisRecords;
	int				m_iNameID;
	
};	// end of class CVrInstance

class CVrInstances
{
public:
	CVrInstances();
	~CVrInstances();	

	int FindInstancesIdx(CString StyleName);
	int FindInstancesIdx(CVrAxisRecords& inAxisRecords);
	int IsExist(CVrInstance& inInstance);

	std::vector<CVrInstance> m_vtinstances;

};	// end of class CVrInstances

class CVrFontName
{
	public:
		CVrFontName();
		~CVrFontName();

	public:
		CString		m_strENGFamilyName;
		CString		m_strCHSFamilyName;
		CString		m_strPostScriptName;
		CString		m_strENGCopyright;
		CString		m_strCHSCopyright;
		CString     m_strVersion;
};

