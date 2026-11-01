#ifndef mini_ini_generator_h
#define mini_ini_generator_h

namespace mINI
{
	template <class T>
	class INIGenerator
	{
	private:
		template <typename> friend class INIFile;
		fs::path filename;
		file_props fprops;
		bool prettyPrint;
//	public:
		using INIStructure = T;

		INIGenerator(const fs::path p, const file_props& fp, bool pp = false)
		 : filename(p), fprops(fp), prettyPrint(pp)
		{
			std::error_code ec;
			fs::remove(filename, ec);
		}

		bool operator<<(const INIStructure& data) const
		{
			T_LineData lines;
			std::string eq_op = (prettyPrint) ? " = " : "=";
			bool is_first_section = true;
			for(const auto& sect_pair : data) {
				const auto& section = sect_pair.first;
				const auto& collection = sect_pair.second;
				if(prettyPrint && !is_first_section)
					lines.emplace_back(std::string());
				lines.emplace_back("[" + section + "]");
				for(const auto& key_pair : collection) {
					auto key = key_pair.first;
					INIStringUtil::replace(key, "=", "\\=");
					auto value = key_pair.second;
					INIStringUtil::trim(value);
					lines.emplace_back(key + eq_op + value);
				}
				is_first_section = false;
			}
			return lines.empty() ? true : lines.write_file(filename);
		}
	};
}

#endif // mini_ini_generator_h
