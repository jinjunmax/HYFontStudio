#include "IKName.h"

namespace HYFONTCODEC
{
	class HYIKCODEC_API CHYIkCodec 
	{
	public:
		CHYIkCodec(void);
		~CHYIkCodec(void);

		void	SetDefault();
		int		Decode(char* pIKFile);
		int		Encode(char* pIKFile, CIKFont& IkFont);
		int		Encode(char* pIKFile);	

		int		DecodeHeader();
		int		DecodeCharacter();

		int		PointToData(const CIKPoint& pt, short& s1, short& s2, const CIKCharSet& charSet);
		int		DataToPoint(short s1, short s2, CIKPoint& pt, const CIKCharSet& charSet);
		
		int		CountCharacterLength(CIKCharacter& Char);
		int		CountIKHeader(CIKFont& ikFont);
		int		EncodeHeader(FILE* pDst,CIKFont& ikFont);
		int		EncodeCharacter(FILE* pDst,CIKFont& ikFont);

	private:
		int		CheckDataFormat();

		long				m_lIkDataLen;
		long				m_lRealHeaderLen;
		char*				m_pIKData;
		unsigned long		m_ulOffset;		
	public:
		CIKFont				m_IKFont;

	public:
		CIKCharacter&		GetCharByMapCode(unsigned long ulMapCode);

	};
};

