//---------------------------------------------------------------------------
#ifndef ini_commonH
#define ini_commonH
//---------------------------------------------------------------------------
#include <string>
#include <filesystem>
#include <mini/unicode_utils.h>

namespace fs = std::filesystem;

namespace mINI
{
namespace INIStringUtil
{
extern const std::string& whitespaceDelimiters();
extern void trim(std::string& str);
extern void replace(std::string& str, std::string const& a, std::string const& b);
} // namespace INIStringUtil

using T_LineData = unicode_utils::line_data;
using encoding = unicode_utils::encoding;
using file_props = unicode_utils::file_props;
} // namespace mINI

#endif
