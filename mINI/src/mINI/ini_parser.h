//---------------------------------------------------------------------------
#ifndef ini_parserH
#define ini_parserH
//---------------------------------------------------------------------------
#include <string>
#pragma comment(lib, "mINI")

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
