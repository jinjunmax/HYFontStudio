
namespace HYFONTCODEC
{
	class HY_DLL_CLASS CMetaDateMap
	{
		public:
			CMetaDateMap();
			~CMetaDateMap();
		public:
			char tag[4];
			unsigned int dataOffset;
			unsigned int dataLength;
	};

	class HY_DLL_CLASS CHYmeta
	{
		public:
			CHYmeta(void);
			~CHYmeta(void);

		public:
			void Setdefault();

		public:
			std::vector<CMetaDateMap> vtDatamap;

	};

}