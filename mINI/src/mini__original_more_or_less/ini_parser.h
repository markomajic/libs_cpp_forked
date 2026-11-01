#ifndef mini_ini_parser_h
#define mini_ini_parser_h

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

		inline PDataType parseLine(std::string line, T_ParseValues& parseData)
		{
			parseData.first.clear();
			parseData.second.clear();
			INIStringUtil::trim(line);
			if (line.empty())
			{
				return PDataType::PDATA_NONE;
			}
			char firstCharacter = line[0];
			if (firstCharacter == ';')
			{
				return PDataType::PDATA_COMMENT;
			}
			if (firstCharacter == '[')
			{
				auto commentAt = line.find_first_of(';');
				if (commentAt != std::string::npos)
				{
					line = line.substr(0, commentAt);
				}
				auto closingBracketAt = line.find_last_of(']');
				if (closingBracketAt != std::string::npos)
				{
					auto section = line.substr(1, closingBracketAt - 1);
					INIStringUtil::trim(section);
					parseData.first = section;
					return PDataType::PDATA_SECTION;
				}
			}
			auto lineNorm = line;
			INIStringUtil::replace(lineNorm, "\\=", "  ");
			auto equalsAt = lineNorm.find_first_of('=');
			if (equalsAt != std::string::npos)
			{
				auto key = line.substr(0, equalsAt);
				INIStringUtil::trim(key);
				INIStringUtil::replace(key, "\\=", "=");
				auto value = line.substr(equalsAt + 1);
				INIStringUtil::trim(value);
				parseData.first = key;
				parseData.second = value;
				return PDataType::PDATA_KEYVALUE;
			}
			return PDataType::PDATA_UNKNOWN;
		}
	};
}

#endif // mini_ini_parser_h
