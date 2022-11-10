namespace HYFONTCODEC
{
	class HY_DLL_CLASS CHYAxisValueMap
	{
		public:
			CHYAxisValueMap();
			~CHYAxisValueMap();
		public:
			F2Dot14	fromCoordinate;
			F2Dot14	toCoordinate;
	};

	class HY_DLL_CLASS CHYSegmentMaps
	{
		public:
			CHYSegmentMaps();
			~CHYSegmentMaps();
		public:
			unsigned short					positionMapCount;
			std::vector<CHYAxisValueMap>	vtAxisValueMaps;
	};	

	class HY_DLL_CLASS CHYavar
	{
		public:
			CHYavar();
			~CHYavar();
		public:
			unsigned short					majorVersion;
			unsigned short					minorVersion;
			unsigned short					reserved;
			unsigned short					axisCount;
			std::vector<CHYSegmentMaps>		vtSegmenMaps;
	public:
			void	SetDefault();
	};
}