#pragma once

namespace HYFONTCODEC
{

	class HY_DLL_CLASS	CHYFvarAxisRecord
	{
		public:
		CHYFvarAxisRecord();
		~CHYFvarAxisRecord();

		unsigned long		Tag;
		HYFIXED				minValue;
		HYFIXED				defaultValue;
		HYFIXED				maxValue;
		unsigned short		flags;
		unsigned short		axisNameID;
	};

	class HY_DLL_CLASS CHYFvarInstncRecord
	{
		public:
			CHYFvarInstncRecord();
			~CHYFvarInstncRecord();

			unsigned short			subfamilyNameID;
			unsigned short			flags;
			std::vector<HYFIXED>	coordinates;			
			unsigned short			postScriptNameID;
	};

	class HY_DLL_CLASS	CHYfvar
	{
	public:
		CHYfvar(void);
		~CHYfvar(void);

		void									setDefault();
		CHYFvarAxisRecord*						GetAxisRecord(unsigned long		Tag);

	public:
		unsigned short							majorVersion;
		unsigned short							minorVersion;
		unsigned short							offsetToAxesArray;
		unsigned short							reserved;
		unsigned short							axisCount;
		unsigned short							axisSize;
		unsigned short							instanceCount;
		unsigned short							instanceSize;
		std::vector<CHYFvarAxisRecord>			vtaxes;
		std::vector<CHYFvarInstncRecord>		vtinstances;

	public:
		// 通过NameID获取	指定AxisRecord
		BOOL			FindAxisRecord(int iNameID, CHYFvarAxisRecord& AxisRcrd);

		// 通过NameID获取	指定InstanceRecord
		BOOL			FindInstanceRecord(int iNameID, CHYFvarInstncRecord& InStanceRcrd);

		
	};
};

