// FontInfDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "HYFontSmartShaper.h"
#include "FontInfDlg.h"
#include "afxdialogex.h"

CUnicodeSection::CUnicodeSection()
{
	m_strEngName = "";
	m_strHanName = "";
	uniStart = 0;
	uniEnd = 0;
	HitCout = 0;

}	// end of CUnicodeSection::CUnicodeSection()

CUnicodeSection::~CUnicodeSection()
{
	m_strEngName = "";
	m_strHanName = "";
	uniStart = 0;
	uniEnd = 0;
	HitCout = 0;

}	// end of CUnicodeSection::~CUnicodeSection()

CUnicodeMapping::CUnicodeMapping()
{


}	// end of CUnicodeMapping::CUnicodeMapping()

CUnicodeMapping::~CUnicodeMapping()
{

}	// end of CUnicodeMapping::~CUnicodeMapping()

void CUnicodeMapping::Init()
{
	vtUniSecitons.clear();

	CUnicodeSection sctn;
	sctn.m_strEngName = "Basic Latin";
	sctn.m_strHanName = "基本拉丁文";
	sctn.uniStart = 0x0000;
	sctn.uniEnd = 0x007F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Latin-1 Supplement";
	sctn.m_strHanName = "拉丁文-1补充";
	sctn.uniStart = 0x0080;
	sctn.uniEnd = 0x00FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Latin Extended-A";
	sctn.m_strHanName = "拉丁文扩展-A";
	sctn.uniStart = 0x0100;
	sctn.uniEnd = 0x017F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Latin Extended-B";
	sctn.m_strHanName = "拉丁文扩展-B";
	sctn.uniStart = 0x0180;
	sctn.uniEnd = 0x024F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "IPA Extensions";
	sctn.m_strHanName = "国际音标扩展";
	sctn.uniStart = 0x0250;
	sctn.uniEnd = 0x02AF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Spacing Modifier Letters";
	sctn.m_strHanName = "占位修饰符号";
	sctn.uniStart = 0x02B0;
	sctn.uniEnd = 0x02FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Combining Diacritics Marks";
	sctn.m_strHanName = "结合附加符号";
	sctn.uniStart = 0x0300;
	sctn.uniEnd = 0x036F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Greek and Coptic";
	sctn.m_strHanName = "希腊字母及科普特字母";
	sctn.uniStart = 0x0370;
	sctn.uniEnd = 0x03FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Cyrillic";
	sctn.m_strHanName = "西里尔字母";
	sctn.uniStart = 0x0400;
	sctn.uniEnd = 0x04FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Cyrillic Supplement";
	sctn.m_strHanName = "西里尔字母补充";
	sctn.uniStart = 0x0500;
	sctn.uniEnd = 0x052F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Armenian";
	sctn.m_strHanName = "亚美尼亚字母";
	sctn.uniStart = 0x0530;
	sctn.uniEnd = 0x058F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Hebrew";
	sctn.m_strHanName = "希伯来文";
	sctn.uniStart = 0x0590;
	sctn.uniEnd = 0x05FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Arabic";
	sctn.m_strHanName = "阿拉伯文";
	sctn.uniStart = 0x0600;
	sctn.uniEnd = 0x06FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Syriac";
	sctn.m_strHanName = "叙利亚文";
	sctn.uniStart = 0x0700;
	sctn.uniEnd = 0x074F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Arabic Supplement";
	sctn.m_strHanName = "阿拉伯文补充";
	sctn.uniStart = 0x0750;
	sctn.uniEnd = 0x077F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Thaana";
	sctn.m_strHanName = "它拿字母";
	sctn.uniStart = 0x0780;
	sctn.uniEnd = 0x07BF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "N'Ko";
	sctn.m_strHanName = "西非书面文字";
	sctn.uniStart = 0x07C0;
	sctn.uniEnd = 0x07FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Samaritan";
	sctn.m_strHanName = "撒玛利亚字母";
	sctn.uniStart = 0x0800;
	sctn.uniEnd = 0x083F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Mandaic";
	sctn.m_strHanName = "曼达文字";
	sctn.uniStart = 0x0840;
	sctn.uniEnd = 0x085F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Syriac Supplement";
	sctn.m_strHanName = "叙利亚文补充";
	sctn.uniStart = 0x0860;
	sctn.uniEnd = 0x086F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Arabic Extended-B";
	sctn.m_strHanName = "阿拉伯文扩展-B";
	sctn.uniStart = 0x0870;
	sctn.uniEnd = 0x089F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Arabic Extended-A";
	sctn.m_strHanName = "阿拉伯文扩展-A";
	sctn.uniStart = 0x08A0;
	sctn.uniEnd = 0x08FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Devanagari";
	sctn.m_strHanName = "天城文";
	sctn.uniStart = 0x0900;
	sctn.uniEnd = 0x097F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Bengali";
	sctn.m_strHanName = "孟加拉文";
	sctn.uniStart = 0x0980;
	sctn.uniEnd = 0x09FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Gurmukhi";
	sctn.m_strHanName = "古木基文";
	sctn.uniStart = 0x0A00;
	sctn.uniEnd = 0x0A7F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Gujarati";
	sctn.m_strHanName = "古吉拉特文";
	sctn.uniStart = 0x0A80;
	sctn.uniEnd = 0x0AFF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Oriya";
	sctn.m_strHanName = "奥里亚文";
	sctn.uniStart = 0x0B00;
	sctn.uniEnd = 0x0B7F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Tamil";
	sctn.m_strHanName = "泰米尔文";
	sctn.uniStart = 0x0B80;
	sctn.uniEnd = 0x0BFF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Telugu";
	sctn.m_strHanName = "泰卢固文";
	sctn.uniStart = 0x0C00;
	sctn.uniEnd = 0x0C7F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Kannada";
	sctn.m_strHanName = "卡纳达文";
	sctn.uniStart = 0x0C80;
	sctn.uniEnd = 0x0CFF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Malayalam";
	sctn.m_strHanName = "马拉雅拉姆文";
	sctn.uniStart = 0x0D00;
	sctn.uniEnd = 0x0D7F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Sinhala";
	sctn.m_strHanName = "僧伽罗文";
	sctn.uniStart = 0x0D80;
	sctn.uniEnd = 0x0DFF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Thai";
	sctn.m_strHanName = "泰文";
	sctn.uniStart = 0x0E00;
	sctn.uniEnd = 0x0E7F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Lao";
	sctn.m_strHanName = "老挝文";
	sctn.uniStart = 0x0E80;
	sctn.uniEnd = 0x0EFF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Tibetan";
	sctn.m_strHanName = "藏文";
	sctn.uniStart = 0x0F00;
	sctn.uniEnd = 0x0FFF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Myanmar";
	sctn.m_strHanName = "缅甸文";
	sctn.uniStart = 0x1000;
	sctn.uniEnd = 0x109F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Georgian";
	sctn.m_strHanName = "格鲁吉亚字母";
	sctn.uniStart = 0x10A0;
	sctn.uniEnd = 0x10FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Hangul Jamo";
	sctn.m_strHanName = "谚文字母";
	sctn.uniStart = 0x1100;
	sctn.uniEnd = 0x11FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Ethiopic";
	sctn.m_strHanName = "吉兹字母";
	sctn.uniStart = 0x1200;
	sctn.uniEnd = 0x137F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Ethiopic Supplement";
	sctn.m_strHanName = "吉兹字母补充";
	sctn.uniStart = 0x1380;
	sctn.uniEnd = 0x139F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Cherokee";
	sctn.m_strHanName = "切罗基字母";
	sctn.uniStart = 0x13A0;
	sctn.uniEnd = 0x13FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Unified Canadian Aboriginal Syllabics";
	sctn.m_strHanName = "统一加拿大原住民音节文字";
	sctn.uniStart = 0x1400;
	sctn.uniEnd = 0x167F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Ogham";
	sctn.m_strHanName = "欧甘字母";
	sctn.uniStart = 0x1680;
	sctn.uniEnd = 0x169F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Runic";
	sctn.m_strHanName = "卢恩字母";
	sctn.uniStart = 0x16A0;
	sctn.uniEnd = 0x16FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Tagalog";
	sctn.m_strHanName = "他加禄字母";
	sctn.uniStart = 0x1700;
	sctn.uniEnd = 0x171F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Hanunóo";
	sctn.m_strHanName = "哈努诺文";
	sctn.uniStart = 0x1720;
	sctn.uniEnd = 0x173F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Buhid";
	sctn.m_strHanName = "布希德文";
	sctn.uniStart = 0x1740;
	sctn.uniEnd = 0x175F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Tagbanwa";
	sctn.m_strHanName = "塔格班瓦文";
	sctn.uniStart = 0x1760;
	sctn.uniEnd = 0x177F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Khmer";
	sctn.m_strHanName = "高棉文";
	sctn.uniStart = 0x1780;
	sctn.uniEnd = 0x17FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Mongolian";
	sctn.m_strHanName = "蒙古文";
	sctn.uniStart = 0x1800;
	sctn.uniEnd = 0x18AF ;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Unified Canadian Aboriginal Syllabics Extended";
	sctn.m_strHanName = "加拿大原住民音节文字扩展";
	sctn.uniStart = 0x18B0;
	sctn.uniEnd = 0x18FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Limbu";
	sctn.m_strHanName = "林布文";
	sctn.uniStart = 0x1900;
	sctn.uniEnd = 0x194F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Tai Le";
	sctn.m_strHanName = "德宏傣文";
	sctn.uniStart = 0x1950;
	sctn.uniEnd = 0x197F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "New Tai Lue";
	sctn.m_strHanName = "新傣仂文";
	sctn.uniStart = 0x1980;
	sctn.uniEnd = 0x19DF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Khmer Symbols";
	sctn.m_strHanName = "高棉文符号";
	sctn.uniStart = 0x19E0;
	sctn.uniEnd = 0x19FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Buginese";
	sctn.m_strHanName = "布吉文";
	sctn.uniStart = 0x1A00;
	sctn.uniEnd = 0x1A1F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Tai Tham";
	sctn.m_strHanName = "老傣文";
	sctn.uniStart = 0x1A20;
	sctn.uniEnd = 0x1AAF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Combining Diacritical Marks Extended";
	sctn.m_strHanName = "组合变音标记扩展";
	sctn.uniStart = 0x1AB0;
	sctn.uniEnd = 0x1AFF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Balinese";
	sctn.m_strHanName = "巴厘字母";
	sctn.uniStart = 0x1B00;
	sctn.uniEnd = 0x1B7F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Sundanese";
	sctn.m_strHanName = "巽他字母";
	sctn.uniStart = 0x1B80;
	sctn.uniEnd = 0x1BBF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Batak";
	sctn.m_strHanName = "巴塔克文";
	sctn.uniStart = 0x1BC0;
	sctn.uniEnd = 0x1BFF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Lepcha";
	sctn.m_strHanName = "雷布查字母";
	sctn.uniStart = 0x1C00;
	sctn.uniEnd = 0x1C4F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Ol Chiki";
	sctn.m_strHanName = "桑塔利文";
	sctn.uniStart = 0x1C50;
	sctn.uniEnd = 0x1C7F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Cyrillic Extended-C";
	sctn.m_strHanName = "西里尔字母扩充-C";
	sctn.uniStart = 0x1C80;
	sctn.uniEnd = 0x1C8F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Georgian Extended";
	sctn.m_strHanName = "格鲁吉亚字母扩展";
	sctn.uniStart = 0x1C90;
	sctn.uniEnd = 0x1CBF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Sudanese Supplement";
	sctn.m_strHanName = "巽他字母补充";
	sctn.uniStart = 0x1CC0;
	sctn.uniEnd = 0x1CCF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Vedic Extensions";
	sctn.m_strHanName = "梵文吠陀扩展";
	sctn.uniStart = 0x1CD0;
	sctn.uniEnd = 0x1CFF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Phonetic Extensions";
	sctn.m_strHanName = "音标扩展";
	sctn.uniStart = 0x1D00;
	sctn.uniEnd = 0x1D7F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Phonetic Extensions Supplement";
	sctn.m_strHanName = "音标扩展补充";
	sctn.uniStart = 0x1D80;
	sctn.uniEnd = 0x1DBF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Combining Diacritics Marks Supplement";
	sctn.m_strHanName = "结合附加符号补充";
	sctn.uniStart = 0x1DC0;
	sctn.uniEnd = 0x1DFF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Latin Extended Additional";
	sctn.m_strHanName = "拉丁文扩展附加";
	sctn.uniStart = 0x1E00;
	sctn.uniEnd = 0x1EFF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Greek Extended";
	sctn.m_strHanName = "希腊文扩展";
	sctn.uniStart = 0x1F00;
	sctn.uniEnd = 0x1FFF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "General Punctuation";
	sctn.m_strHanName = "常用标点";
	sctn.uniStart = 0x2000;
	sctn.uniEnd = 0x206F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Superscripts and Subscripts";
	sctn.m_strHanName = "上标及下标";
	sctn.uniStart = 0x2070;
	sctn.uniEnd = 0x209F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Currency Symbols";
	sctn.m_strHanName = "货币符号";
	sctn.uniStart = 0x20A0;
	sctn.uniEnd = 0x20CF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Combining Diacritics Marks for Symbols";
	sctn.m_strHanName = "组合用记号";
	sctn.uniStart = 0x20D0;
	sctn.uniEnd = 0x20FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Letterlike Symbols";
	sctn.m_strHanName = "字母式符号";
	sctn.uniStart = 0x2100;
	sctn.uniEnd = 0x214F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Number Forms";
	sctn.m_strHanName = "数字形式";
	sctn.uniStart = 0x2150;
	sctn.uniEnd = 0x218F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Arrows";
	sctn.m_strHanName = "箭头";
	sctn.uniStart = 0x2190;
	sctn.uniEnd = 0x21FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Mathematical Operators";
	sctn.m_strHanName = "数学运算符";
	sctn.uniStart = 0x2200;
	sctn.uniEnd = 0x22FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Miscellaneous Technical";
	sctn.m_strHanName = "杂项工业符号";
	sctn.uniStart = 0x2300;
	sctn.uniEnd = 0x23FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Control Pictures";
	sctn.m_strHanName = "控制图片";
	sctn.uniStart = 0x2400;
	sctn.uniEnd = 0x243F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Optical Character Recognition";
	sctn.m_strHanName = "光学识别符";
	sctn.uniStart = 0x2440;
	sctn.uniEnd = 0x245F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Enclosed Alphanumerics";
	sctn.m_strHanName = "带圈字母和数字";
	sctn.uniStart = 0x2460;
	sctn.uniEnd = 0x24FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Box Drawing";
	sctn.m_strHanName = "制表符";
	sctn.uniStart = 0x2500;
	sctn.uniEnd = 0x257F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Block Elements";
	sctn.m_strHanName = "方块元素";
	sctn.uniStart = 0x2580;
	sctn.uniEnd = 0x259F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Geometric Shapes";
	sctn.m_strHanName = "几何图形";
	sctn.uniStart = 0x25A0;
	sctn.uniEnd = 0x25FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Miscellaneous Symbols";
	sctn.m_strHanName = "杂项符号";
	sctn.uniStart = 0x2600;
	sctn.uniEnd = 0x26FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Dingbats";
	sctn.m_strHanName = "装饰符号";
	sctn.uniStart = 0x2700;
	sctn.uniEnd = 0x27BF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Miscellaneous Mathematical Symbols-A";
	sctn.m_strHanName = "杂项数学符号-A";
	sctn.uniStart = 0x27C0;
	sctn.uniEnd = 0x27EF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Supplemental Arrows-A";
	sctn.m_strHanName = "追加箭头-A";
	sctn.uniStart = 0x27F0;
	sctn.uniEnd = 0x27FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Braille Patterns";
	sctn.m_strHanName = "盲文点字模型";
	sctn.uniStart = 0x2800;
	sctn.uniEnd = 0x28FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Supplemental Arrows-B";
	sctn.m_strHanName = "追加箭头-B";
	sctn.uniStart = 0x2900;
	sctn.uniEnd = 0x297F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Miscellaneous Mathematical Symbols-B";
	sctn.m_strHanName = "杂项数学符号-B";
	sctn.uniStart = 0x2980;
	sctn.uniEnd = 0x29FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Supplemental Mathematical Operator";
	sctn.m_strHanName = "追加数学运算符";
	sctn.uniStart = 0x2A00;
	sctn.uniEnd = 0x2AFF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Miscellaneous Symbols and Arrows";
	sctn.m_strHanName = "杂项符号和箭头";
	sctn.uniStart = 0x2B00;
	sctn.uniEnd = 0x2BFF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Glagolitic";
	sctn.m_strHanName = "格拉哥里字母";
	sctn.uniStart = 0x2C00;
	sctn.uniEnd = 0x2C5F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Latin Extended-C";
	sctn.m_strHanName = "拉丁文扩展-C";
	sctn.uniStart = 0x2C60;
	sctn.uniEnd = 0x2C7F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Coptic";
	sctn.m_strHanName = "科普特字母";
	sctn.uniStart = 0x2C80;
	sctn.uniEnd = 0x2CFF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Georgian Supplement";
	sctn.m_strHanName = "格鲁吉亚字母补充";
	sctn.uniStart = 0x2D00;
	sctn.uniEnd = 0x2D2F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Tifinagh";
	sctn.m_strHanName = "提非纳文";
	sctn.uniStart = 0x2D30;
	sctn.uniEnd = 0x2D7F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Ethiopic Extended";
	sctn.m_strHanName = "吉兹字母扩展";
	sctn.uniStart = 0x2D80;
	sctn.uniEnd = 0x2DDF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Cyrillic Extended-A";
	sctn.m_strHanName = "西里尔字母扩展-A";
	sctn.uniStart = 0x2DE0;
	sctn.uniEnd = 0x2DFF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Supplemental Punctuation";
	sctn.m_strHanName = "追加标点";
	sctn.uniStart = 0x2E00;
	sctn.uniEnd = 0x2E7F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "CJK Radicals Supplement";
	sctn.m_strHanName = "中日韩汉字部首补充";
	sctn.uniStart = 0x2E80;
	sctn.uniEnd = 0x2EFF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Kangxi Radicals";
	sctn.m_strHanName = "康熙部首";
	sctn.uniStart = 0x2F00;
	sctn.uniEnd = 0x2FDF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Ideographic Description Characters";
	sctn.m_strHanName = "表意文字序列";
	sctn.uniStart = 0x2FF0;
	sctn.uniEnd = 0x2FFF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "CJK Symbols and Punctuation";
	sctn.m_strHanName = "中日韩符号和标点";
	sctn.uniStart = 0x3000;
	sctn.uniEnd = 0x303F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Hiragana";
	sctn.m_strHanName = "日文平假名";
	sctn.uniStart = 0x3040;
	sctn.uniEnd = 0x309F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Katakana";
	sctn.m_strHanName = "日文片假名";
	sctn.uniStart = 0x30A0;
	sctn.uniEnd = 0x30FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Bopomofo";
	sctn.m_strHanName = "注音符号";
	sctn.uniStart = 0x3100;
	sctn.uniEnd = 0x312F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Hangul Compatibility Jamo";
	sctn.m_strHanName = "谚文兼容字母";
	sctn.uniStart = 0x3130;
	sctn.uniEnd = 0x318F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Kanbun";
	sctn.m_strHanName = "汉文注释标志";
	sctn.uniStart = 0x3190;
	sctn.uniEnd = 0x319F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Bopomofo Extended";
	sctn.m_strHanName = "注音字母扩展";
	sctn.uniStart = 0x31A0;
	sctn.uniEnd = 0x31BF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "CJK Strokes";
	sctn.m_strHanName = "中日韩笔画";
	sctn.uniStart = 0x31C0;
	sctn.uniEnd = 0x31EF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Katakana Phonetic Extensions";
	sctn.m_strHanName = "日文片假名拼音扩展";
	sctn.uniStart = 0x31F0;
	sctn.uniEnd = 0x31FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Enclosed CJK Letters and Months";
	sctn.m_strHanName = "带圈的CJK字符及月份";
	sctn.uniStart = 0x3200;
	sctn.uniEnd = 0x32FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "CJK Compatibility";
	sctn.m_strHanName = "中日韩兼容字符";
	sctn.uniStart = 0x3300;
	sctn.uniEnd = 0x33FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "CJK Unified Ideographs Extension A";
	sctn.m_strHanName = "中日韩兼容字符";
	sctn.uniStart = 0x3400;
	sctn.uniEnd = 0x4DBF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Yijing Hexagrams Symbols";
	sctn.m_strHanName = "易经六十四卦符号";
	sctn.uniStart = 0x4DC0;
	sctn.uniEnd = 0x4DFF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "CJK Unified Ideographs";
	sctn.m_strHanName = "中日韩统一表意文字";
	sctn.uniStart = 0x4E00;
	sctn.uniEnd = 0x9FFF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Yi Syllables";
	sctn.m_strHanName = "彝文音节";
	sctn.uniStart = 0xA000;
	sctn.uniEnd = 0xA48F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Yi Radicals";
	sctn.m_strHanName = "彝文字根";
	sctn.uniStart = 0xA490;
	sctn.uniEnd = 0xA4CF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Lisu";
	sctn.m_strHanName = "老傈僳文";
	sctn.uniStart = 0xA4D0;
	sctn.uniEnd = 0xA4FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Vai";
	sctn.m_strHanName = "瓦伊语";
	sctn.uniStart = 0xA500;
	sctn.uniEnd = 0xA63F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Cyrillic Extended-B";
	sctn.m_strHanName = "西里尔字母扩展-B";
	sctn.uniStart = 0xA640;
	sctn.uniEnd = 0xA69F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Bamum";
	sctn.m_strHanName = "巴姆穆文字";
	sctn.uniStart = 0xA6A0;
	sctn.uniEnd = 0xA6FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Modifier Tone Letters";
	sctn.m_strHanName = "声调修饰符号";
	sctn.uniStart = 0xA700;
	sctn.uniEnd = 0xA71F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Latin Extended-D";
	sctn.m_strHanName = "拉丁文扩展-D";
	sctn.uniStart = 0xA720;
	sctn.uniEnd = 0xA7FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Syloti Nagri";
	sctn.m_strHanName = "锡尔赫特文";
	sctn.uniStart = 0xA800;
	sctn.uniEnd = 0xA82F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Common Indic Number Forms";
	sctn.m_strHanName = "通用印度数字格式";
	sctn.uniStart = 0xA830;
	sctn.uniEnd = 0xA83F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Phags-pa";
	sctn.m_strHanName = "八思巴字";
	sctn.uniStart = 0xA840;
	sctn.uniEnd = 0xA87F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Saurashtra";
	sctn.m_strHanName = "索拉什特拉文";
	sctn.uniStart = 0xA880;
	sctn.uniEnd = 0xA8DF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Devanagari Extended";
	sctn.m_strHanName = "天城文扩展";
	sctn.uniStart = 0xA8E0;
	sctn.uniEnd = 0xA8FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Kayah Li";
	sctn.m_strHanName = "克耶里字母";
	sctn.uniStart = 0xA900;
	sctn.uniEnd = 0xA92F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Rejang";
	sctn.m_strHanName = "勒姜字母";
	sctn.uniStart = 0xA930;
	sctn.uniEnd = 0xA95F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Hangul Jamo Extended-A";
	sctn.m_strHanName = "谚文扩展-A";
	sctn.uniStart = 0xA960;
	sctn.uniEnd = 0xA97F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Javanese";
	sctn.m_strHanName = "爪哇字母";
	sctn.uniStart = 0xA980;
	sctn.uniEnd = 0xA9DF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Myanmar Extended-B";
	sctn.m_strHanName = "缅甸文扩展-B";
	sctn.uniStart = 0xA9E0;
	sctn.uniEnd = 0xA9FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Cham";
	sctn.m_strHanName = "占语字母";
	sctn.uniStart = 0xAA00;
	sctn.uniEnd = 0xAA5F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Myanmar Extended-A";
	sctn.m_strHanName = "缅甸文扩展-A";
	sctn.uniStart = 0xAA60;
	sctn.uniEnd = 0xAA7F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Tai Viet";
	sctn.m_strHanName = "越南傣文";
	sctn.uniStart = 0xAA80;
	sctn.uniEnd = 0xAADF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Meetei Mayek Extensions";
	sctn.m_strHanName = "曼尼普尔文扩展";
	sctn.uniStart = 0xAAE0;
	sctn.uniEnd = 0xAAFF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Ethiopic Extended-A";
	sctn.m_strHanName = "吉兹字母扩展-A";
	sctn.uniStart = 0xAB00;
	sctn.uniEnd = 0xAB2F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Latin Extended-E";
	sctn.m_strHanName = "拉丁文扩展-E";
	sctn.uniStart = 0xAB30;
	sctn.uniEnd = 0xAB6F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Cherokee Supplement";
	sctn.m_strHanName = "切罗基语补充";
	sctn.uniStart = 0xAB70;
	sctn.uniEnd = 0xABBF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Meetei Mayek";
	sctn.m_strHanName = "曼尼普尔文";
	sctn.uniStart = 0xABC0;
	sctn.uniEnd = 0xABFF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Hangul Syllables";
	sctn.m_strHanName = "谚文音节";
	sctn.uniStart = 0xAC00;
	sctn.uniEnd = 0xD7AF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Hangul Jamo Extended-B";
	sctn.m_strHanName = "谚文字母扩展-B";
	sctn.uniStart = 0xD7B0;
	sctn.uniEnd = 0xD7FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "High-half zone of UTF-16";
	sctn.m_strHanName = "UTF-16的高半区";
	sctn.uniStart = 0xD800;
	sctn.uniEnd = 0xDBFF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Low-half zone of UTF-16";
	sctn.m_strHanName = "UTF-16的低半区";
	sctn.uniStart = 0xDC00;
	sctn.uniEnd = 0xDFFF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Private Use Area";
	sctn.m_strHanName = "私用区";
	sctn.uniStart = 0xE000;
	sctn.uniEnd = 0xF8FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "CJK Compatibility Ideographs";
	sctn.m_strHanName = "中日韩兼容表意文字";
	sctn.uniStart = 0xF900;
	sctn.uniEnd = 0xFAFF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Alphabetic Presentation Forms";
	sctn.m_strHanName = "字母表达形式";
	sctn.uniStart = 0xFB00;
	sctn.uniEnd = 0xFB4F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Arabic Presentation Forms A";
	sctn.m_strHanName = "阿拉伯字母表达形式-A";
	sctn.uniStart = 0xFB50;
	sctn.uniEnd = 0xFDFF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Variation Selector";
	sctn.m_strHanName = "异体字选择1器";
	sctn.uniStart = 0xFE00;
	sctn.uniEnd = 0xFE0F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Vertical Forms";
	sctn.m_strHanName = "竖排形式";
	sctn.uniStart = 0xFE10;
	sctn.uniEnd = 0xFE1F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Combining Half Marks";
	sctn.m_strHanName = "组合用半符号";
	sctn.uniStart = 0xFE20;
	sctn.uniEnd = 0xFE2F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "CJK Compatibility Forms";
	sctn.m_strHanName = "中日韩兼容形式";
	sctn.uniStart = 0xFE30;
	sctn.uniEnd = 0xFE4F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Small Form Variants";
	sctn.m_strHanName = "小写变体形式";
	sctn.uniStart = 0xFE50;
	sctn.uniEnd = 0xFE6F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Arabic Presentation Forms B";
	sctn.m_strHanName = "阿拉伯文表达形式-B";
	sctn.uniStart = 0xFE70;
	sctn.uniEnd = 0xFEFF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Halfwidth and Fullwidth Forms";
	sctn.m_strHanName = "半角及全角字符";
	sctn.uniStart = 0xFF00;
	sctn.uniEnd = 0xFFEF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Specials";
	sctn.m_strHanName = "特殊字符区";
	sctn.uniStart = 0xFFF0;
	sctn.uniEnd = 0xFFFF;
	vtUniSecitons.push_back(sctn);

	//plane 1
	sctn.m_strEngName = "Linear B Syllabary";
	sctn.m_strHanName = "线形文字B音节文字";
	sctn.uniStart = 0x00010000;
	sctn.uniEnd = 0x0001007F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Linear B Ideograms";
	sctn.m_strHanName = "线形文字B表意文字";
	sctn.uniStart = 0x00010080;
	sctn.uniEnd = 0x000100FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Aegean Numbers";
	sctn.m_strHanName = "爱琴海数字";
	sctn.uniStart = 0x00010100;
	sctn.uniEnd = 0x0001013F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Ancient Greek Numbers";
	sctn.m_strHanName = "古希腊数字";
	sctn.uniStart = 0x00010140;
	sctn.uniEnd = 0x0001018F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Ancient Symbols";
	sctn.m_strHanName = "古代记数系统";
	sctn.uniStart = 0x00010190;
	sctn.uniEnd = 0x000101CF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Phaistos Disc";
	sctn.m_strHanName = "费斯托斯圆盘";
	sctn.uniStart = 0x000101D0;
	sctn.uniEnd = 0x000101FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Lycian";
	sctn.m_strHanName = "吕基亚字母";
	sctn.uniStart = 0x00010280;
	sctn.uniEnd = 0x0001029F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Carian";
	sctn.m_strHanName = "卡利亚字母";
	sctn.uniStart = 0x000102A0;
	sctn.uniEnd = 0x000102DF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Coptic Epact Numbers";
	sctn.m_strHanName = "科普特闰余数字";
	sctn.uniStart = 0x000102E0;
	sctn.uniEnd = 0x000102FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Old Italic";
	sctn.m_strHanName = "古意大利字母";
	sctn.uniStart = 0x00010300;
	sctn.uniEnd = 0x0001032F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Gothic";
	sctn.m_strHanName = "哥特字母";
	sctn.uniStart = 0x00010330;
	sctn.uniEnd = 0x0001034F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Old Permic";
	sctn.m_strHanName = "古彼尔姆文";
	sctn.uniStart = 0x00010350;
	sctn.uniEnd = 0x0001037F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Ugaritic";
	sctn.m_strHanName = "乌加里特字母";
	sctn.uniStart = 0x00010380;
	sctn.uniEnd = 0x0001039F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Old Persian";
	sctn.m_strHanName = "古波斯楔形文字";
	sctn.uniStart = 0x000103A0;
	sctn.uniEnd = 0x000103DF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Deseret";
	sctn.m_strHanName = "德赛莱特字母";
	sctn.uniStart = 0x00010400;
	sctn.uniEnd = 0x0001044F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Shavian";
	sctn.m_strHanName = "萧伯纳字母";
	sctn.uniStart = 0x00010450;
	sctn.uniEnd = 0x0001047F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Osmanya";
	sctn.m_strHanName = "奥斯曼亚字母";
	sctn.uniStart = 0x00010480;
	sctn.uniEnd = 0x000104AF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Osage";
	sctn.m_strHanName = "欧塞奇字母";
	sctn.uniStart = 0x000104B0;
	sctn.uniEnd = 0x000104FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Elbasan";
	sctn.m_strHanName = "艾尔巴桑字母";
	sctn.uniStart = 0x00010500;
	sctn.uniEnd = 0x0001052F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Caucasian Albanian";
	sctn.m_strHanName = "高加索阿尔巴尼亚文";
	sctn.uniStart = 0x00010530;
	sctn.uniEnd = 0x0001056F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Vithkuqi";
	sctn.m_strHanName = "维斯库奇文";
	sctn.uniStart = 0x00010570;
	sctn.uniEnd = 0x000105BF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Linear A";
	sctn.m_strHanName = "线形文字A";
	sctn.uniStart = 0x00010600;
	sctn.uniEnd = 0x0001077F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Latin Extended-F";
	sctn.m_strHanName = "拉丁文扩展-F";
	sctn.uniStart = 0x00010780;
	sctn.uniEnd = 0x000107BF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Cypriot Syllabary";
	sctn.m_strHanName = "塞浦路斯音节文字";
	sctn.uniStart = 0x00010800;
	sctn.uniEnd = 0x0001083F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Imperial Aramaic";
	sctn.m_strHanName = "帝国亚兰文字";
	sctn.uniStart = 0x00010840;
	sctn.uniEnd = 0x0001085F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Palmyrene";
	sctn.m_strHanName = "帕尔迈拉字母";
	sctn.uniStart = 0x00010860;
	sctn.uniEnd = 0x0001087F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Nabataean";
	sctn.m_strHanName = "纳巴泰字母";
	sctn.uniStart = 0x00010880;
	sctn.uniEnd = 0x000108AF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Hatran";
	sctn.m_strHanName = "哈特兰文";
	sctn.uniStart = 0x000108E0;
	sctn.uniEnd = 0x000108FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Phoenician";
	sctn.m_strHanName = "腓尼基字母";
	sctn.uniStart = 0x00010900;
	sctn.uniEnd = 0x0001091F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Lydian";
	sctn.m_strHanName = "吕底亚字母";
	sctn.uniStart = 0x00010920;
	sctn.uniEnd = 0x0001093F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Meroitic Hieroglyphs";
	sctn.m_strHanName = "麦罗埃文圣书体";
	sctn.uniStart = 0x00010980;
	sctn.uniEnd = 0x0001099F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Meroitic Cursive";
	sctn.m_strHanName = "麦罗埃文草书体";
	sctn.uniStart = 0x000109A0;
	sctn.uniEnd = 0x000109FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Kharoshthi";
	sctn.m_strHanName = "佉卢文";
	sctn.uniStart = 0x00010A00;
	sctn.uniEnd = 0x00010A5F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Old South Arabian";
	sctn.m_strHanName = "古南阿拉伯字母";
	sctn.uniStart = 0x00010A60;
	sctn.uniEnd = 0x00010A7F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Old North Arabian";
	sctn.m_strHanName = "古北阿拉伯字母";
	sctn.uniStart = 0x00010A80;
	sctn.uniEnd = 0x00010A9F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Manichaean";
	sctn.m_strHanName = "摩尼字母";
	sctn.uniStart = 0x00010AC0;
	sctn.uniEnd = 0x00010AFF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Avestan";
	sctn.m_strHanName = "阿维斯陀字母";
	sctn.uniStart = 0x00010B00;
	sctn.uniEnd = 0x00010B3F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Inscriptional Parthian";
	sctn.m_strHanName = "碑刻帕提亚文";
	sctn.uniStart = 0x00010B40;
	sctn.uniEnd = 0x00010B5F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Inscriptional Pahlav";
	sctn.m_strHanName = "碑刻巴列维文";
	sctn.uniStart = 0x00010B60;
	sctn.uniEnd = 0x00010B7F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Psalter Pahlavi";
	sctn.m_strHanName = "诗篇巴列维文";
	sctn.uniStart = 0x00010B80;
	sctn.uniEnd = 0x00010BAF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Old Turkic";
	sctn.m_strHanName = "古突厥文";
	sctn.uniStart = 0x00010C00;
	sctn.uniEnd = 0x00010C4F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Hanifi Rohingya";
	sctn.m_strHanName = "哈乃斐罗兴亚文字";
	sctn.uniStart = 0x00010D00;
	sctn.uniEnd = 0x00010D3F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Old Hungarian";
	sctn.m_strHanName = "古匈牙利字母";
	sctn.uniStart = 0x00010C80;
	sctn.uniEnd = 0x00010CFF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Rumi Numeral Symbols";
	sctn.m_strHanName = "卢米文数字";
	sctn.uniStart = 0x00010E60;
	sctn.uniEnd = 0x00010E7F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Yezidi";
	sctn.m_strHanName = "雅兹迪文";
	sctn.uniStart = 0x00010E80;
	sctn.uniEnd = 0x00010EBF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Old Sogdian";
	sctn.m_strHanName = "古粟特字母";
	sctn.uniStart = 0x00010F00;
	sctn.uniEnd = 0x00010F2F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Sogdian";
	sctn.m_strHanName = "粟特字母";
	sctn.uniStart = 0x00010F30;
	sctn.uniEnd = 0x00010F6F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Old Uyghur";
	sctn.m_strHanName = "回鹘字母";
	sctn.uniStart = 0x00010F70;
	sctn.uniEnd = 0x00010FAF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Chorasmian";
	sctn.m_strHanName = "花剌子模字母";
	sctn.uniStart = 0x00010FB0;
	sctn.uniEnd = 0x00010FDF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Elymaic";
	sctn.m_strHanName = "埃利迈斯字母";
	sctn.uniStart = 0x00010FE0;
	sctn.uniEnd = 0x00010FFF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Brahmi";
	sctn.m_strHanName = "婆罗米文字";
	sctn.uniStart = 0x00011000;
	sctn.uniEnd = 0x0001107F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Kaithi";
	sctn.m_strHanName = "凯提文";
	sctn.uniStart = 0x00011080;
	sctn.uniEnd = 0x000110CF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Sora Sompeng";
	sctn.m_strHanName = "索拉僧平文字";
	sctn.uniStart = 0x000110D0;
	sctn.uniEnd = 0x000110FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Chakma";
	sctn.m_strHanName = "查克马文";
	sctn.uniStart = 0x00011100;
	sctn.uniEnd = 0x0001114F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Mahajani";
	sctn.m_strHanName = "马哈佳尼文";
	sctn.uniStart = 0x00011150;
	sctn.uniEnd = 0x0001117F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Sharada";
	sctn.m_strHanName = "夏拉达文";
	sctn.uniStart = 0x00011180;
	sctn.uniEnd = 0x000111DF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Sinhala Archaic Numbers";
	sctn.m_strHanName = "古僧伽罗文数字";
	sctn.uniStart = 0x000111E0;
	sctn.uniEnd = 0x000111FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Khojki";
	sctn.m_strHanName = "可吉文";
	sctn.uniStart = 0x00011200;
	sctn.uniEnd = 0x0001124F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Multani";
	sctn.m_strHanName = "穆尔塔尼文";
	sctn.uniStart = 0x00011280;
	sctn.uniEnd = 0x000112AF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Khudawadi";
	sctn.m_strHanName = "库达瓦迪文";
	sctn.uniStart = 0x000112B0;
	sctn.uniEnd = 0x000112FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Grantha";
	sctn.m_strHanName = "古兰塔文";
	sctn.uniStart = 0x00011300;
	sctn.uniEnd = 0x0001137F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Newa";
	sctn.m_strHanName = "尼泊尔纽瓦字母";
	sctn.uniStart = 0x00011400;
	sctn.uniEnd = 0x0001147F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Tirhuta";
	sctn.m_strHanName = "底罗仆多文";
	sctn.uniStart = 0x00011480;
	sctn.uniEnd = 0x000114DF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Siddham";
	sctn.m_strHanName = "悉昙文字";
	sctn.uniStart = 0x00011580;
	sctn.uniEnd = 0x000115FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Modi";
	sctn.m_strHanName = "莫迪文";
	sctn.uniStart = 0x00011600;
	sctn.uniEnd = 0x0001165F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Mongolian Supplement";
	sctn.m_strHanName = "蒙古文补充";
	sctn.uniStart = 0x00011660;
	sctn.uniEnd = 0x0001167F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Takri";
	sctn.m_strHanName = "塔克里字母";
	sctn.uniStart = 0x00011680;
	sctn.uniEnd = 0x000116CF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Ahom";
	sctn.m_strHanName = "阿洪姆文";
	sctn.uniStart = 0x00011700;
	sctn.uniEnd = 0x0001173F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Dogra";
	sctn.m_strHanName = "多格拉文";
	sctn.uniStart = 0x00011800;
	sctn.uniEnd = 0x0001184F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Warang Citi";
	sctn.m_strHanName = "瓦兰齐地文";
	sctn.uniStart = 0x000118A0;
	sctn.uniEnd = 0x000118FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Dhives Akuru";
	sctn.m_strHanName = "岛屿字母";
	sctn.uniStart = 0x00011900;
	sctn.uniEnd = 0x0001195F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Nandinagari";
	sctn.m_strHanName = "南迪那嘎黎文";
	sctn.uniStart = 0x000119A0;
	sctn.uniEnd = 0x000119FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Zanabazar Square";
	sctn.m_strHanName = "札那巴札尔方形字母";
	sctn.uniStart = 0x00011A00;
	sctn.uniEnd = 0x00011A4F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Soyombo";
	sctn.m_strHanName = "索永布文字";
	sctn.uniStart = 0x00011A50;
	sctn.uniEnd = 0x00011AAF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Unified Canadian Aboriginal Syllabics Extended-A";
	sctn.m_strHanName = "加拿大原住民音节文字扩展A";
	sctn.uniStart = 0x00011AB0;
	sctn.uniEnd = 0x00011ABF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Pau Cin Hau";
	sctn.m_strHanName = "包钦豪文";
	sctn.uniStart = 0x00011AC0;
	sctn.uniEnd = 0x00011AFF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Bhaiksuki";
	sctn.m_strHanName = "拜克舒基文";
	sctn.uniStart = 0x00011C00;
	sctn.uniEnd = 0x00011C6F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Marchen";
	sctn.m_strHanName = "玛钦文";
	sctn.uniStart = 0x00011C70;
	sctn.uniEnd = 0x00011CBF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Masaram Gondi";
	sctn.m_strHanName = "马萨拉姆共地文字";
	sctn.uniStart = 0x00011D00;
	sctn.uniEnd = 0x00011D5F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Gunjala Gondi";
	sctn.m_strHanName = "古吉拉共地文字";
	sctn.uniStart = 0x00011D60;
	sctn.uniEnd = 0x00011DAF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Makasar";
	sctn.m_strHanName = "玛卡莎文";
	sctn.uniStart = 0x00011EE0;
	sctn.uniEnd = 0x00011EFF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Lisu Supplement";
	sctn.m_strHanName = "老傈僳文";
	sctn.uniStart = 0x00011FB0;
	sctn.uniEnd = 0x00011FBF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Tamil Supplement";
	sctn.m_strHanName = "泰米尔文补充";
	sctn.uniStart = 0x00011FC0;
	sctn.uniEnd = 0x00011FFF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Cuneiform";
	sctn.m_strHanName = "楔形文字";
	sctn.uniStart = 0x00012000;
	sctn.uniEnd = 0x000123FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Cuneiform Numbers and Punctuation";
	sctn.m_strHanName = "楔形文字数字和标点符号";
	sctn.uniStart = 0x00012400;
	sctn.uniEnd = 0x0001247F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Early Dynastic Cuneiform";
	sctn.m_strHanName = "早期王朝楔形文字";
	sctn.uniStart = 0x00012480;
	sctn.uniEnd = 0x0001254F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Cypro-Minoan";
	sctn.m_strHanName = "塞浦路斯-米诺斯文字";
	sctn.uniStart = 0x00012F90;
	sctn.uniEnd = 0x00012FFF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Egyptian Hieroglyphs";
	sctn.m_strHanName = "埃及圣书体";
	sctn.uniStart = 0x00013000;
	sctn.uniEnd = 0x0001342F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Egyptian Hieroglyphs Format Controls";
	sctn.m_strHanName = "埃及圣书体格式控制";
	sctn.uniStart = 0x00013430;
	sctn.uniEnd = 0x0001343F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Anatolian Hieroglyphs";
	sctn.m_strHanName = "安纳托利亚象形文字";
	sctn.uniStart = 0x00014400;
	sctn.uniEnd = 0x0001467F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Bamum Supplement";
	sctn.m_strHanName = "巴姆穆文字补充";
	sctn.uniStart = 0x00016800;
	sctn.uniEnd = 0x00016A3F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Mro";
	sctn.m_strHanName = "默文";
	sctn.uniStart = 0x00016A40;
	sctn.uniEnd = 0x00016A6F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Tangsa";
	sctn.m_strHanName = "唐萨文";
	sctn.uniStart = 0x00016A70;
	sctn.uniEnd = 0x00016ACF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Bassa Vah";
	sctn.m_strHanName = "巴萨哇文字";
	sctn.uniStart = 0x00016AD0;
	sctn.uniEnd = 0x00016AFF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Pahawh Hmong";
	sctn.m_strHanName = "救世苗文";
	sctn.uniStart = 0x00016B00;
	sctn.uniEnd = 0x00016B8F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Medefaidrin";
	sctn.m_strHanName = "梅德法伊德林文";
	sctn.uniStart = 0x00016E40;
	sctn.uniEnd = 0x00016E9F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Miao";
	sctn.m_strHanName = "柏格理苗文";
	sctn.uniStart = 0x00016F00;
	sctn.uniEnd = 0x00016F9F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Ideographic Symbols and Punctuation";
	sctn.m_strHanName = "表意符号和标点符号";
	sctn.uniStart = 0x00016FE0;
	sctn.uniEnd = 0x00016FFF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Tangut";
	sctn.m_strHanName = "西夏文";
	sctn.uniStart = 0x00017000;
	sctn.uniEnd = 0x000187FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Tangut Components";
	sctn.m_strHanName = "西夏文部首";
	sctn.uniStart = 0x00018800;
	sctn.uniEnd = 0x00018AFF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Khitan Small Script";
	sctn.m_strHanName = "契丹小字";
	sctn.uniStart = 0x00018B00;
	sctn.uniEnd = 0x00018CFF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Tangut Supplement";
	sctn.m_strHanName = "西夏文补充";
	sctn.uniStart = 0x00018D00;
	sctn.uniEnd = 0x00018D8F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Kana Extended-B";
	sctn.m_strHanName = "日文假名扩展-B";
	sctn.uniStart = 0x0001AFF0;
	sctn.uniEnd = 0x0001AFFF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Kana Supplement";
	sctn.m_strHanName = "日文假名补充";
	sctn.uniStart = 0x0001B000;
	sctn.uniEnd = 0x0001B0FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Kana Extended-A";
	sctn.m_strHanName = "日文假名扩展-A";
	sctn.uniStart = 0x0001B100;
	sctn.uniEnd = 0x0001B12F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Small Kana Extension";
	sctn.m_strHanName = "小假名扩充";
	sctn.uniStart = 0x0001B130;
	sctn.uniEnd = 0x0001B16F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Nushu";
	sctn.m_strHanName = "女书";
	sctn.uniStart = 0x0001B170;
	sctn.uniEnd = 0x0001B2FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Duployan";
	sctn.m_strHanName = "杜普雷速记";
	sctn.uniStart = 0x0001BC00;
	sctn.uniEnd = 0x0001BC9F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Shorthand Format Controls";
	sctn.m_strHanName = "速记格式控制符";
	sctn.uniStart = 0x0001BCA0;
	sctn.uniEnd = 0x0001BCAF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Znamenny Musical Notation";
	sctn.m_strHanName = "赞玫尼圣歌音乐符号";
	sctn.uniStart = 0x0001CF00;
	sctn.uniEnd = 0x0001CFFF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Byzantine Musical Symbols";
	sctn.m_strHanName = "拜占庭音乐符号";
	sctn.uniStart = 0x0001D000;
	sctn.uniEnd = 0x0001D0FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Musical Symbols";
	sctn.m_strHanName = "音乐符号";
	sctn.uniStart = 0x0001D100;
	sctn.uniEnd = 0x0001D1FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Ancient Greek Musical Notation";
	sctn.m_strHanName = "古希腊音乐记号";
	sctn.uniStart = 0x0001D200;
	sctn.uniEnd = 0x0001D24F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Mayan Numerals";
	sctn.m_strHanName = "玛雅数字";
	sctn.uniStart = 0x0001D2E0;
	sctn.uniEnd = 0x0001D2FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Tai Xuan Jing Symbols";
	sctn.m_strHanName = "太玄经符号";
	sctn.uniStart = 0x0001D300;
	sctn.uniEnd = 0x0001D35F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Counting Rod Numerals";
	sctn.m_strHanName = "算筹";
	sctn.uniStart = 0x0001D360;
	sctn.uniEnd = 0x0001D37F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Mathematical Alphanumeric Symbols";
	sctn.m_strHanName = "数学字母数字符号";
	sctn.uniStart = 0x0001D400;
	sctn.uniEnd = 0x0001D7FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Sutton SignWriting";
	sctn.m_strHanName = "萨顿书写符号";
	sctn.uniStart = 0x0001D800;
	sctn.uniEnd = 0x0001DAAF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Latin Extended-G";
	sctn.m_strHanName = "拉丁文扩展-G";
	sctn.uniStart = 0x0001DF00;
	sctn.uniEnd = 0x0001DFFF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Glagolitic Supplement";
	sctn.m_strHanName = "格拉哥里字母补充";
	sctn.uniStart = 0x0001E000;
	sctn.uniEnd = 0x0001E02F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Nyiakeng Puachue Hmong";
	sctn.m_strHanName = "创世纪苗文";
	sctn.uniStart = 0x0001E100;
	sctn.uniEnd = 0x0001E14F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Toto";
	sctn.m_strHanName = "托托文";
	sctn.uniStart = 0x0001E290;
	sctn.uniEnd = 0x0001E2BF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Mende Kikakui";
	sctn.m_strHanName = "门地奇卡奎文";
	sctn.uniStart = 0x0001E800;
	sctn.uniEnd = 0x0001E8DF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Adlam";
	sctn.m_strHanName = "阿德拉姆字母";
	sctn.uniStart = 0x0001E900;
	sctn.uniEnd = 0x0001E95F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Ethiopic Extended-B";
	sctn.m_strHanName = "吉兹字母扩展-B";
	sctn.uniStart = 0x0001E7E0;
	sctn.uniEnd = 0x0001E7FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Indic Siyaq Numbers";
	sctn.m_strHanName = "印度西亚克数字";
	sctn.uniStart = 0x0001EC70;
	sctn.uniEnd = 0x0001ECBF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Ottoman Siyaq Numbers";
	sctn.m_strHanName = "奥斯曼西亚克数字";
	sctn.uniStart = 0x0001ED00;
	sctn.uniEnd = 0x0001ED4F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Arabic Mathematical Alphanumeric Symbols";
	sctn.m_strHanName = "阿拉伯字母数字符号";
	sctn.uniStart = 0x0001EE00;
	sctn.uniEnd = 0x0001EEFF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Mahjong Tiles";
	sctn.m_strHanName = "麻将牌";
	sctn.uniStart = 0x0001F000;
	sctn.uniEnd = 0x0001F02F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Domino Tiles";
	sctn.m_strHanName = "多米诺骨牌";
	sctn.uniStart = 0x0001F030;
	sctn.uniEnd = 0x0001F09F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Playing Cards";
	sctn.m_strHanName = "扑克牌";
	sctn.uniStart = 0x0001F0A0;
	sctn.uniEnd = 0x0001F0FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Enclosed Alphanumeric Supplement";
	sctn.m_strHanName = "带圈字母数字补充";
	sctn.uniStart = 0x0001F100;
	sctn.uniEnd = 0x0001F1FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Enclosed Ideographic Supplement";
	sctn.m_strHanName = "带圈表意文字补充";
	sctn.uniStart = 0x0001F200;
	sctn.uniEnd = 0x0001F2FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Miscellaneous Symbols And Pictographs";
	sctn.m_strHanName = "杂项符号和象形文字";
	sctn.uniStart = 0x0001F300;
	sctn.uniEnd = 0x0001F5FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Emotions";
	sctn.m_strHanName = "表情符号";
	sctn.uniStart = 0x0001F600;
	sctn.uniEnd = 0x0001F64F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Ornamental Dingbats";
	sctn.m_strHanName = "装饰符号";
	sctn.uniStart = 0x0001F650;
	sctn.uniEnd = 0x0001F67F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Transport And Map Symbols";
	sctn.m_strHanName = "交通和地图符号";
	sctn.uniStart = 0x0001F680;
	sctn.uniEnd = 0x0001F6FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Alchemical Symbols";
	sctn.m_strHanName = "炼金术符号";
	sctn.uniStart = 0x0001F700;
	sctn.uniEnd = 0x0001F77F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Geometric Shapes Extended";
	sctn.m_strHanName = "几何图形扩展";
	sctn.uniStart = 0x0001F780;
	sctn.uniEnd = 0x0001F7FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Supplemental Arrows-C";
	sctn.m_strHanName = "追加箭头-C";
	sctn.uniStart = 0x0001F800;
	sctn.uniEnd = 0x0001F8FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Supplemental Symbols and Pictographs";
	sctn.m_strHanName = "补充符号和象形文字";
	sctn.uniStart = 0x0001F900;
	sctn.uniEnd = 0x0001F9FF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Chess Symbols";
	sctn.m_strHanName = "国际象棋符号";
	sctn.uniStart = 0x0001FA00;
	sctn.uniEnd = 0x0001FA6F;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Symbols and Pictographs Extended-A";
	sctn.m_strHanName = "符号和象形文字扩展-A";
	sctn.uniStart = 0x0001FA70;
	sctn.uniEnd = 0x0001FAFF;
	vtUniSecitons.push_back(sctn);

	sctn.m_strEngName = "Symbols for Legacy Computing";
	sctn.m_strHanName = "保留计算符号";
	sctn.uniStart = 0x0001FB00;
	sctn.uniEnd = 0x0001FBFF;
	vtUniSecitons.push_back(sctn);

}	// end of CUnicodeMapping::Init()

void CUnicodeMapping::Hit(unsigned long uni)
{


}	// end of void CUnicodeMapping::Hit()

// CFontInfDlg 对话框
IMPLEMENT_DYNAMIC(CFontInfDlg, CDialogEx)

CFontInfDlg::CFontInfDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_FNT_INF_DLG, pParent)
{
	m_pFontDeCodec = NULL;

}	// end of CFontInfDlg::CFontInfDlg()

CFontInfDlg::CFontInfDlg(CHYFontCodec* p,CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_FNT_INF_DLG, pParent)
{
	m_pFontDeCodec = p;

}	// end of CFontInfDlg::CFontInfDlg()

CFontInfDlg::~CFontInfDlg()
{

}	// end of CFontInfDlg::~CFontInfDlg()

void CFontInfDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FNT_INF_LST, m_lstFontInf);

}	//end of void CFontInfDlg::DoDataExchange()

BEGIN_MESSAGE_MAP(CFontInfDlg, CDialogEx)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// CFontInfDlg 消息处理程序
void CFontInfDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
}	// end of void CFontInfDlg::OnDestroy()

BOOL CFontInfDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	FontMetricsInf();

	return TRUE;  // return TRUE unless you set the focus to a control				  

}	// end of BOOL CFontInfDlg::OnInitDialog()

void CFontInfDlg::FontMetricsInf()
{
	if (m_pFontDeCodec) {

		int iGidXmin = m_pFontDeCodec->FindHeadXminGID();
		int iGidXmax = m_pFontDeCodec->FindHeadXmaxGID();
		int iGidYmin = m_pFontDeCodec->FindHeadYminGID();
		int iGidYmax = m_pFontDeCodec->FindHeadYmaxGID();

		CString strInf;

		strInf = "Metrics";
		m_lstFontInf.AddString(strInf);
		strInf.Format("字库中X最小值的字形索引 = %d", iGidXmin);
		m_lstFontInf.AddString(strInf);
		strInf.Format("字库中X最大值的字形索引 = %d", iGidXmax);
		m_lstFontInf.AddString(strInf);
		strInf.Format("字库中Y最小值的字形索引 = %d", iGidYmin);
		m_lstFontInf.AddString(strInf);
		strInf.Format("字库中Y最大值的字形索引 = %d", iGidYmax);
		m_lstFontInf.AddString(strInf);	
		
		strInf.Format("Hhea Ascender = %d", m_pFontDeCodec->m_HYHhea.Ascender);
		m_lstFontInf.AddString(strInf);
		strInf.Format("Hhea descender = %d", m_pFontDeCodec->m_HYHhea.Descender);
		m_lstFontInf.AddString(strInf);
		strInf.Format("Hhea linegap = %d", m_pFontDeCodec->m_HYHhea.LineGap);
		m_lstFontInf.AddString(strInf);
		strInf.Format("OS2 TypoAscender = %d", m_pFontDeCodec->m_HYOS2.sTypoAscender);
		m_lstFontInf.AddString(strInf);
		strInf.Format("OS2 TypoDescender = %d", m_pFontDeCodec->m_HYOS2.sTypoDescender);
		m_lstFontInf.AddString(strInf);
		strInf.Format("OS2 LineGap = %d", m_pFontDeCodec->m_HYOS2.sTypoLineGap);
		m_lstFontInf.AddString(strInf);
		strInf.Format("OS2 WinAscender = %d", m_pFontDeCodec->m_HYOS2.usWinAscent);
		m_lstFontInf.AddString(strInf);
		strInf.Format("OS2 WinDescender = %d", m_pFontDeCodec->m_HYOS2.usWinDescent);
		m_lstFontInf.AddString(strInf);
		
	}

}	// end of void CFontInfDlg::FontMetrics()

void CFontInfDlg::FontUnicodeSection()
{		
	for (int i = 0; i < m_pFontDeCodec->m_HYMaxp.numGlyphs; i++){
		size_t sz = m_pFontDeCodec->m_vtHYGlyphs[i].vtUnicode.size();
		for (size_t j = 0; j < sz; j++)	{
			unsigned long uni = m_pFontDeCodec->m_vtHYGlyphs[i].vtUnicode[j];
		}
	}

}	// end of void CFontInfDlg::FontUnicodeSection()

