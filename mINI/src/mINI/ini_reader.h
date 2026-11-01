#ifndef mini_ini_reader_h
#define mini_ini_reader_h

namespace mINI
{
	template <class T>
	class INIReader : public T_LineData
	{
//	public:
		using INIStructure = T;
	private:
		template <typename> friend class INIFile;
		template <typename> friend class INIWriter;

		fs::path filename;
//	public:
		INIReader(const fs::path& p) : filename(p) {}
		~INIReader() {}

		bool read(INIStructure& data)
		{
			T_LineData file_lines;
			if(file_lines.read_file(filename)) {
				set_props(file_lines.get_props());

				std::string section;
				bool inSection = false;
				INIParser::T_ParseValues parseData;
				for (auto const& line : file_lines)
				{
					auto parseResult = INIParser::parseLine(line, parseData);
					if (parseResult == INIParser::PDataType::PDATA_SECTION)
					{
						inSection = true;
						data[section = parseData.first];
					}
					else if (inSection && parseResult == INIParser::PDataType::PDATA_KEYVALUE)
					{
						auto const& key = parseData.first;
						auto const& value = parseData.second;
						data[section][key] = value;
					}
					if(parseResult != INIParser::PDataType::PDATA_UNKNOWN && !(parseResult == INIParser::PDataType::PDATA_KEYVALUE && !inSection))
						emplace_back(line);
				}
				return true;
			}
			return false;
		}

		bool operator>>(INIStructure& data) {return read(data);}
	};
}

#endif // mini_ini_reader_h
