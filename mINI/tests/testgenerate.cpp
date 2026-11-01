#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <fstream>
#include "lest/lest.hpp"
#include "mini/ini.h"

using T_LineData = mINI::T_LineData;
using T_INIFileData = std::pair<std::string, std::vector<std::string>>;
using INIStructure = mINI::INIStructureCI;
using INIFile = mINI::INIFile<INIStructure>;
using path = std::filesystem::path;

//
// helper functions
//
bool verifyData(const T_INIFileData& testData)
{
	// compares file contents to expected data
	path filename = testData.first;
	T_LineData linesExpected = testData.second;
	T_LineData linesActual(filename);

	std::size_t lines_to_test = std::min(linesExpected.size(), linesActual.size());
	for(std::size_t i=0; i<lines_to_test; i++)
		if (linesExpected[i] != linesActual[i]) {
			std::cout << "Line " << i << " does not match expected!" << std::endl;
			std::cout << "Expected: " << linesExpected[i] << std::endl;
			std::cout << "Is: " << linesActual[i] << std::endl;
			return false;
		}

	if(linesExpected.size()!=linesActual.size()) {
		std::cout << "Line count mismatch!  Expected " << linesExpected.size() << ", actual " << linesActual.size() << std::endl;
		return false;
	}

	return true;
}

//
// test data
//
const T_INIFileData testDataBasic = {
	// filename
	"data01.ini",
	// expected result
	{
		"[section]",
		"key1=value1",
		"key2=value2"
	}
};

const T_INIFileData testDataManySections = {
	// filename
	"data02.ini",
	// expected result
	{
		"[section1]",
		"key1=value1",
		"key2=value2",
		"key3=value3",
		"[section2]",
		"key1=value1",
		"key2=value2",
		"[section3]",
		"key1=value1"
	}
};

const T_INIFileData testDataEmptySection = {
	// filename
	"data03.ini",
	// expected result
	{
		"[empty]"
	}
};

const T_INIFileData testDataManyEmptySections = {
	// filename
	"data04.ini",
	// expected result
	{
		"[empty1]",
		"[empty2]",
		"[empty3]"
	}
};

const T_INIFileData testDataEmpty = {
	// filename
	"data05.ini",
	// expected result
	{}
};

const T_INIFileData testDataPrettyPrint = {
	// filename
	"data06.ini",
	// expected result
	{
		"[section1]",
		"key1 = value1",
		"key2 = value2",
		"",
		"[section2]",
		"key1 = value1"
	}
};

const T_INIFileData testDataEmptyNames = {
	// filename
	"data07.ini",
	// expected result
	{
		"[]",
		"key=value",
		"[section]",
		"=value",
		"[section2]",
		"="
	}
};

const T_INIFileData testDataMalformed1 = {
	// filename
	"data08.ini",
	// test data
	{
		"[[name1]",
		"key=value",
		"[name2]]",
		"key=value",
		"[[name3]]",
		"key=value"
	}
};

const T_INIFileData testDataMalformed2 = {
	// filename
	"data09.ini",
	// test data
	{
		"[name]",
		"\\===",         // key: "="   value: "="
		"a\\= \\===b",  //  key: "a= ="  value: "=b"
		"c\\= \\===d"  //  key: "c= ="  value: "=d"
	}
};

//
// test cases
//
const lest::test mINI_tests[] = {
	CASE("Test: Basic generate")
	{
		// create a very basic INI file and verify resulting file has correct data
		path filename = testDataBasic.first;
		INIFile file(filename);
		INIStructure ini;
		ini["section"]["key1"] = "value1";
		ini["section"]["key2"] = "value2";
		EXPECT(file.generate(ini) == true);
		EXPECT(verifyData(testDataBasic) == true);
	},
	CASE("Test: Generate many sections")
	{
		path filename = testDataManySections.first;
		INIFile file(filename);
		INIStructure ini;
		ini["section1"].set({
			{"key1", "value1"},
			{"key2", "value2"},
			{"key3", "value3"}
		});
		ini["section2"].set({
			{"key1", "value1"},
			{"key2", "value2"}
		});
		ini["section3"]["key1"] = "value1";
		EXPECT(file.generate(ini) == true);
		EXPECT(verifyData(testDataManySections));
	},
	CASE("Test: Generate empty section")
	{
		path filename = testDataEmptySection.first;
		INIFile file(filename);
		INIStructure ini;
		ini["empty"];
		EXPECT(file.generate(ini) == true);
		EXPECT(verifyData(testDataEmptySection));
	},
	CASE("Test: Generate many empty sections")
	{
		path filename = testDataManyEmptySections.first;
		INIFile file(filename);
		INIStructure ini;
		ini["empty1"];
		ini["empty2"];
		ini["empty3"];
		EXPECT(file.generate(ini) == true);
		EXPECT(verifyData(testDataManyEmptySections));
	},
	CASE("Test: Generate empty file")
	{
		path filename = testDataEmpty.first;
		INIFile file(filename);
		INIStructure ini;
		EXPECT(file.generate(ini) == true);
		EXPECT(verifyData(testDataEmpty));
	},
	CASE("Test: Generate with pretty-print")
	{
		path filename = testDataPrettyPrint.first;
		INIFile file(filename);
		INIStructure ini;
		ini["section1"].set({
			{"key1", "value1"},
			{"key2", "value2"},
		});
		ini["section2"]["key1"] = "value1";
		EXPECT(file.generate(ini, true) == true);
		EXPECT(verifyData(testDataPrettyPrint));
	},
	CASE("Test: Generate empty section and key names")
	{
		path filename = testDataEmptyNames.first;
		INIFile file(filename);
		INIStructure ini;
		ini[""]["key"] = "value";
		ini["section"][""] = "value";
		ini["section2"][""] = "";
		EXPECT(file.generate(ini) == true);
		EXPECT(verifyData(testDataEmptyNames));
	},
	CASE("Test: Generate malformed section names")
	{
		path filename = testDataMalformed1.first;
		INIFile file(filename);
		INIStructure ini;
		ini["[name1"]["key"] = "value";
		ini["name2]"]["key"] = "value";
		ini["[name3]"]["key"] = "value";
		EXPECT(file.generate(ini) == true);
		EXPECT(verifyData(testDataMalformed1));
	},
	CASE("Test: Generate malformed key names")
	{
		path filename = testDataMalformed2.first;
		INIFile file(filename);
		INIStructure ini;
		ini["name"].set({
			{"=", "="},
			{"a= =", "=b"},
			{"     c= =  ", "    =d  "}
		});
		EXPECT(file.generate(ini) == true);
		EXPECT(verifyData(testDataMalformed2));
	}
};

int main(int argc, char** argv)
{
	// run tests
	if (int failures = lest::run(mINI_tests, argc, argv))
	{
		return failures;
	}
	return std::cout << std::endl << "All tests passed!" << std::endl, EXIT_SUCCESS;
}

