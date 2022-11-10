#pragma once
class CVrMasterinf
{
public:
	CVrMasterinf();
	~CVrMasterinf();

public:
	bool			m_bMaster;	// true Master, False Instance
	int				m_iNameId;
	CString			m_strName;	

};

