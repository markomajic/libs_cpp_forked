//---------------------------------------------------------------------------
#ifndef ini_parserH
#define ini_parserH
//---------------------------------------------------------------------------
#include <string>
#include <utility>
#if defined(__BORLANDC__) && !defined(MINI_NO_AUTOLINK)
#pragma comment(lib, "mINI")
#endif

namespace mINI
{
namespace INIParser
{
		using T_ParseValues = std::pair<std::string, std::string>;

		enum class PDataType : char
		{
			PDATA_NONE,
			PDATA_COMMENT,
			PDATA_SECTION,
			PDATA_KEYVALUE,
			PDATA_UNKNOWN
		};

		extern PDataType parseLine(std::string line, T_ParseValues& parseData);
} // namespace INIParser
} // namespace mINI
#endif
