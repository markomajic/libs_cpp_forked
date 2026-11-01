#ifndef mini_ini_writer_h
#define mini_ini_writer_h

namespace mINI
{

template <class T>
class INIWriter
{
private:
   template <typename> friend class INIFile;
	using INIStructure = T;
	fs::path filename;
	bool prettyPrint;

	void getLazyOutput(T_LineData& output, const T_LineData& line_data, INIStructure& data, INIStructure& original) const;
//public:
	INIWriter(const fs::path& fn, bool pp = false) : filename(fn), prettyPrint(pp) {}
	~INIWriter() {}

	bool operator<<(INIStructure& data) const
	{
		std::error_code ec;
		bool file_exists = fs::exists(filename, ec) && !ec;
		if(file_exists) {
			INIStructure original_data;
			INIReader<INIStructure> reader(filename);
			if(reader.read(original_data)) {
				T_LineData output;
				getLazyOutput(output, reader, data, original_data);
				return output.write_file(filename);
			}
		}
		return false;
	}
};

template <class T> inline
void INIWriter<T>::getLazyOutput(T_LineData& output, const T_LineData& line_data, INIStructure& data, INIStructure& original) const
{
	output.clear();
	output.set_props(line_data.get_props());
	INIParser::T_ParseValues parseData;
	std::string sectionCurrent;
	bool parsingSection = false;
	bool continueToNextSection = false;
	bool discardNextEmpty = false;
	bool writeNewKeys = false;
	std::size_t lastKeyLine = 0;
	for(auto line = line_data.begin(); line != line_data.end(); ++line) {
		if (!writeNewKeys)
		{
			auto parseResult = INIParser::parseLine(*line, parseData);
			if (parseResult == INIParser::PDataType::PDATA_SECTION)
			{
				if (parsingSection)
				{
					writeNewKeys = true;
					parsingSection = false;
					--line;
					continue;
				}
				sectionCurrent = parseData.first;
				if (data.has(sectionCurrent))
				{
					parsingSection = true;
					continueToNextSection = false;
					discardNextEmpty = false;
					output.emplace_back(*line);
					lastKeyLine = output.size();
				}
				else
				{
					continueToNextSection = true;
					discardNextEmpty = true;
					continue;
				}
			}
			else if (parseResult == INIParser::PDataType::PDATA_KEYVALUE)
			{
				if (continueToNextSection)
				{
					continue;
				}
				if (data.has(sectionCurrent))
				{
					auto& collection = data[sectionCurrent];
					auto const& key = parseData.first;
					auto const& value = parseData.second;
					if (collection.has(key))
					{
						auto outputValue = collection[key];
						if (value == outputValue)
						{
							output.emplace_back(*line);
						}
						else
						{
							INIStringUtil::trim(outputValue);
							auto lineNorm = *line;
							INIStringUtil::replace(lineNorm, "\\=", "  ");
							auto equalsAt = lineNorm.find_first_of('=');
							auto valueAt = lineNorm.find_first_not_of(INIStringUtil::whitespaceDelimiters, equalsAt + 1);
							std::string outputLine = line->substr(0, valueAt);
							if (prettyPrint && equalsAt + 1 == valueAt)
							{
								outputLine += " ";
							}
							outputLine += outputValue;
							output.emplace_back(outputLine);
						}
						lastKeyLine = output.size();
					}
				}
			}
			else
			{
				if (discardNextEmpty && line->empty())
				{
					discardNextEmpty = false;
				}
				else if (parseResult != INIParser::PDataType::PDATA_UNKNOWN)
				{
					output.emplace_back(*line);
				}
			}
		}
		if (writeNewKeys || std::next(line) == line_data.end())
		{
			T_LineData linesToAdd;
			if (data.has(sectionCurrent) && original.has(sectionCurrent))
			{
				auto const& collection = data[sectionCurrent];
				auto const& collectionOriginal = original[sectionCurrent];
				for (auto const& it : collection)
				{
					auto key = it.first;
					if (collectionOriginal.has(key))
					{
						continue;
					}
					auto value = it.second;
					INIStringUtil::replace(key, "=", "\\=");
					INIStringUtil::trim(value);
					linesToAdd.emplace_back(key + ((prettyPrint) ? " = " : "=") + value);
				}
			}
			if (!linesToAdd.empty())
			{
				output.insert(
					output.begin() + lastKeyLine,
					linesToAdd.begin(),
					linesToAdd.end()
				);
			}
			if (writeNewKeys)
			{
				writeNewKeys = false;
				--line;
			}
		}
	}
	for (auto const& it : data)
	{
		auto const& section = it.first;
		if (original.has(section))
		{
			continue;
		}
		if (prettyPrint && output.size() > 0 && !output.back().empty())
		{
			output.emplace_back();
		}
		output.emplace_back("[" + section + "]");
		auto const& collection = it.second;
		for (auto const& it2 : collection)
		{
			auto key = it2.first;
			auto value = it2.second;
			INIStringUtil::replace(key, "=", "\\=");
			INIStringUtil::trim(value);
			output.emplace_back(
				key + ((prettyPrint) ? " = " : "=") + value
			);
		}
	}
}

}

#endif // mini_ini_writer_h
