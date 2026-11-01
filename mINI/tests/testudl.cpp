#pragma hdrstop
#pragma argsused

#ifdef _WIN32
#include <tchar.h>
#else
  typedef char _TCHAR;
  #define _tmain main
#endif

#include "mini/ini.h"
#include "mini/unicode_utils.hpp"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

using INIStructure = mINI::INIStructureCI;
using INIFile = mINI::INIFile<INIStructure>;

int test_udl(const fs::path& p)
{
	fs::path filename(p);
	filename.replace_filename(p.stem().string() + "_regen" + p.extension().string());
	fs::copy_file(p, filename, fs::copy_options::overwrite_existing);

	INIFile udlfile(filename);
	INIStructure udl;
	if(!udlfile.read(udl)) {
		std::cout << "UDL read failed!" << std::endl;
		return 1;
	}
	std::cout << "UDL files read without errors!" << std::endl << std::endl;

	std::cout << "UDL content:" << std::endl;
	for(auto& sections : udl) {
		std::cout << "[" << sections.first << "]" << std::endl;
		for(auto& keys : udl[sections.first])
			std::cout << keys.first << " = " << keys.second << std::endl;
	}
	std::cout << std::endl << std::endl;

	udl["oledb"]["new_key"] = "new value";
	udl["new_section"]["new_key"] = "new value";
	udlfile.write(udl);

	return 0;
}

int test_ini(const fs::path& p)
{
	std::cout << "This system is " << (unicode_utils::is_little_endian() ? "little" : "big") << " endian architecture!" << std::endl << std::endl;
	std::string new_fn = p.stem().string() + "_regen" + p.extension().string();
	fs::path filename(p);
	filename.replace_filename(new_fn);
	fs::copy_file(p, filename, fs::copy_options::overwrite_existing);

	INIFile file(filename);
	INIStructure ini;
	if(!file.read(ini)) {
		std::cout << "INI read failed!" << std::endl;
		return 1;
	}
	std::cout << "No errors!" << std::endl << std::endl;

	ini.remove("printdrivers");
	ini["main"].remove("edi_import_path");
	ini["main"]["chq_import_path"] = "update existing value";
	ini["main"]["new__key"] = "new value";

	std::cout << "INI content:" << std::endl;
	for(auto& sections : ini) {
		std::cout << "[" << sections.first << "]" << std::endl;
		for(auto& keys : ini[sections.first])
			std::cout << keys.first << " = " << keys.second << std::endl;
	}
	std::cout << std::endl << std::endl;

	std::cout << "Main/CHQ_IMPORT_PATH = \"" << ini["Main"]["CHQ_IMPORT_PATH"] << "\""<< std::endl;
	std::cout << "main/chq_import_path = \"" << ini["main"]["chq_import_path"] << "\""<< std::endl;

	file.write(ini, true);
	return 0;
}

void check_encoding(const fs::path& p)
{
	std::ifstream istrm(p, std::ios::binary|std::ios::in);
	std::size_t remaining_size;
	unicode_utils::encoding file_enc = unicode_utils::bom::check(istrm, remaining_size);
	std::cout << "File '" << p << "' (bom) encoding is " << file_enc << ", remaining size (in bytes) is " << remaining_size << std::endl;
}

int _tmain(int argc, _TCHAR* argv[])
{

	fs::path cp_ini("X:/CPClient4/DirectEnergy/cpoint.ini");
	fs::path cpdata_udl("X:/CPClient4/DirectEnergy/data_links/cpdata.udl");

	check_encoding(cpdata_udl);

	return test_udl(cpdata_udl);

//	return test_ini(cp_ini);
}
