#ifndef mini_ini_common_h
#define mini_ini_common_h
#include <filesystem>
#include <mini/unicode_utils.hpp>

namespace fs = std::filesystem;

namespace mINI
{
	namespace INIStringUtil
	{
		const std::string whitespaceDelimiters = " \t\n\r\f\v";
		inline void trim(std::string& str)
		{
			str.erase(str.find_last_not_of(whitespaceDelimiters) + 1);
			str.erase(0, str.find_first_not_of(whitespaceDelimiters));
		}
//#ifndef MINI_CASE_SENSITIVE
//		inline void toLower(std::string& str)
//		{
//			std::transform(str.begin(), str.end(), str.begin(), ::tolower);
//		}
//#endif
		inline void replace(std::string& str, std::string const& a, std::string const& b)
		{
			if (!a.empty())
			{
				std::size_t pos = 0;
				while ((pos = str.find(a, pos)) != std::string::npos)
				{
					str.replace(pos, a.size(), b);
					pos += b.size();
				}
			}
		}
//#ifdef _WIN32
//		const std::string endl = "\r\n";
//#else
//		const std::string endl = "\n";
//#endif
	} // namespace INIStringUtil

//	using T_LineData = std::vector<std::string>;
	using T_LineData = unicode_utils::line_data;
	using encoding = unicode_utils::encoding;
	using file_props = unicode_utils::file_props;
//	using T_LineDataPtr = std::shared_ptr<T_LineData>;
} // namespace mINI

#endif // mini_ini_common_h
