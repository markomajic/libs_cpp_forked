//---------------------------------------------------------------------------
#pragma hdrstop

#include "ini_common.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

namespace mINI
{
namespace INIStringUtil
{

const std::string& whitespaceDelimiters()
{
	static std::string ws_delim(" \t\n\r\f\v");
	return ws_delim;
}

void trim(std::string& str)
{
	str.erase(str.find_last_not_of(whitespaceDelimiters()) + 1);
	str.erase(0, str.find_first_not_of(whitespaceDelimiters()));
}

void replace(std::string& str, std::string const& a, std::string const& b)
{
	if (!a.empty()) {
		std::size_t pos = 0;
		while ((pos = str.find(a, pos)) != std::string::npos)
		{
			str.replace(pos, a.size(), b);
			pos += b.size();
		}
	}
}

} // namespace INIStringUtil
} // namespace mINI
