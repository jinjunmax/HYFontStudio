#pragma once


class CRegistryAxis
{
	public:
		CRegistryAxis();
		~CRegistryAxis();
	public:
		CString m_strName;
		CString m_strTag;
};

class CRegistryAxises
{
public:
	CRegistryAxises();
	~CRegistryAxises();
public:
	int	 FindAxis(CString strName);
	int	 FindAxisEx(CString strTag);

public:
	std::vector<CRegistryAxis> vtRegistryAxise;
};

class CVrAxisValue
{
public:
	CVrAxisValue();
	~CVrAxisValue();
public:
	bool operator==(const CVrAxisValue& other);
	bool operator!=(const CVrAxisValue& other);
public:
	unsigned short	Format;
	float			Value;
	float			nominalValue;
	float			rangeMinValue;
	float			rangeMaxValue;
	float			linkedValue;
	int				axisIndex;

	unsigned short 	valueNameID;
	CString			valueName;
};

class CVrAxisRecord
{
public:
	CVrAxisRecord();
	~CVrAxisRecord();
public:
	bool operator==(const CVrAxisRecord& other);
	bool operator!=(const CVrAxisRecord& other);

public:	
	int				m_iNameID;
	CString			m_strName;
	CString			m_strTag;
	BOOL			m_bHidden;
	CVrAxisValue	m_AxisValue;
};

class CVrAxisRecords
{
public:
	CVrAxisRecords();
	~CVrAxisRecords();
public:
	bool operator==(const CVrAxisRecords& other);
	bool operator!=(const CVrAxisRecords& other);

	void Setdefault();
	int	 FindAxisIndex(CString strName);
	int	 FindAxisIndexEx(CString strTag);
	float FindAxisMax(CString strTag);
	float FindAxisMin(CString strTag);

public:
	std::vector<CVrAxisRecord>		vtVrAxisInfs;
};


