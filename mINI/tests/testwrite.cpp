#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <fstream>
#include "lest/lest.hpp"
#include "mINI/ini.h"

using T_LineData = mINI::T_LineData;
using T_INIFileData = std::tuple<std::string, std::vector<std::string>, std::vector<std::string>>;
using INIStructure = mINI::INIStructureCI;
using INIFile = mINI::INIFile<INIStructure>;
using path = std::filesystem::path;

//
// helper functions
//
//bool writeTestFile(const T_INIFileData& testData)
//{
//	path filename = std::get<0>(testData);
//	T_LineData lines = std::get<1>(testData);
//	std::ofstream fileWriteStream(filename);
//	if (fileWriteStream.is_open())
//	{
//		if (lines.size())
//		{
//			auto it = lines.begin();
//			for (;;)
//			{
//				fileWriteStream << *it;
//				if (++it == lines.end())
//				{
//					break;
//				}
//				fileWriteStream << std::endl;
//			}
//		}
//		return true;
//	}
//	return false;
//}

bool writeTestFile(const T_INIFileData& testData)
{
	path filename = std::get<0>(testData);
	T_LineData lines = std::get<1>(testData);
	return lines.write_file(filename);
}

bool verifyData(const T_INIFileData& testData)
{
	// compares file contents to expected data
	path filename = std::get<0>(testData);
	T_LineData linesExpected = std::get<2>(testData);
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

//void outputData(const INIStructure& ini)
//{
//	for (auto const& it : ini)
//	{
//		auto const& section = it.first;
//		auto const& collection = it.second;
//		std::cout << "[" << section << "]" << std::endl;
//		for (auto const& it2 : collection)
//		{
//			auto const& key = it2.first;
//			auto const& value = it2.second;
//			std::cout << key << "=" << value << std::endl;
//		}
//		std::cout << std::endl;
//	}
//}

//
// test data
//
const T_INIFileData testDataBasic {
	// filename
	"data01.ini",
	// original data
	{
		";some comment",
		"[some section]",
		"some key=1",
		"other key=2"
	},
	// expected result
	{
		";some comment",
		"[some section]",
		"some key=2",
		"other key=2",
		"yet another key=3"
	}
};

const T_INIFileData testDataWithGarbage {
	// filename
	"data02.ini",
	// original data
	{
		"",
		"  GARBAGE       ; ALSO GARBAGE ",
		";;;;;;;;;;;;;;;comment comment",
		";;;;",
		";;;;       ",
		"          ;",
		"         ;;      ;;xxxx       ",
		"ignored key = ignored value",
		"ignored=ignored",
		"GARBAGE",
		"",
		"ignored key2=ignored key2",
		"GARBAGE             ;;;;;;;;;;;;;;;;;;;;;",
		"[section] ; trailing comment",
		"",
		"GARBAGE",
		";;",
		"a=1",
		"b = 2",
		"c =3",
		"d= 4",
		"e =  5",
		"f   =6",
		"",
		"@#%$@(*(!@*@GARBAGE#!@GARBAGE%$@#GARBAGE%@&*%@$",
		"GARBAGE",
		"[other section] ;also a comment",
		"GARBAGE",
		"dinosaurs= 16",
		"GARBAGE",
		"birds= 123456",
		"giraffes= 22",
		"GARBAGE",
		"[extra section];also a comment",
		"         aaa = 1",
		"         bbb=2",
		"         ccc  =  3",
		"GARBAGE",
		"",
		""
	},
	// expected result
	{
		"",
		";;;;;;;;;;;;;;;comment comment",
		";;;;",
		";;;;       ",
		"          ;",
		"         ;;      ;;xxxx       ",
		"",
		"[section] ; trailing comment",
		"",
		";;",
		"a=2",
		"b = 3",
		"c =4",
		"d= 5",
		"e =  6",
		"f   =7",
		"g=8",
		"",
		"[other section] ;also a comment",
		"birds= 123456",
		"giraffes= 22",
		"[extra section];also a comment",
		"         aaa = 2",
		"         bbb=3",
		"         ccc  =  4",
		"ddd=5",
		"",
		"",
		"[new section]",
		"test=something"
	}
};

const T_INIFileData testDataRemEntry {
	// filename
	"data04.ini",
	// original data
	{
		"[section]",
		"data1=A",
		"data2=B"
	},
	// expected result
	{
		"[section]",
		"data2=B"
	}
};

const T_INIFileData testDataRemSection {
	// filename
	"data05.ini",
	// original data
	{
		"[section]",
		"data1=A",
		"data2=B"
	},
	// expected result
	{
	}
};

const T_INIFileData testDataDuplicateKeys {
	// filename
	"data06.ini",
	// original data
	{
		"[section]",
		"data=A",
		"data=B",
		"[section]",
		"data=C"
	},
	// expected result
	{
		"[section]",
		"data=D",
		"data=D",
		"[section]",
		"data=D"
	}
};

const T_INIFileData testDataDuplicateSections {
	// filename
	"data07.ini",
	// original data
	{
		"[section]",
		"[section]",
		"[section]"
	},
	// expected result
	{
		"[section]",
		"data=A",
		"[section]",
		"data=A",
		"[section]",
		"data=A"
	}
};

const T_INIFileData testDataPrettyPrint {
	// filename
	"data08.ini",
	// oiriginal data
	{
		"[section1]",
		"value1=A",
		"value2=B",
		"[section2]",
		"value1=A"
	},
	// expected result
	{
		"[section1]",
		"value1=A",
		"value2=B",
		"value3 = C",
		"[section2]",
		"value1=A",
		"value2 = B",
		"",
		"[section3]",
		"value1 = A",
		"value2 = B"
	}
};

const T_INIFileData testDataEmptyFile {
	// filename
	"data09.ini",
	// original data
	{},
	// expected results
	{
		"[section]",
		"key=value"
	}
};

const T_INIFileData testDataEmptySection {
	// filename
	"data10.ini",
	// original data
	{
		"[section]"
	},
	// expected result
	{
		"[section]",
		"key=value"
	}
};

const T_INIFileData testDataManyEmptySections {
	// filename
	"data11.ini",
	// original data
	{
		"[section1]",
		"[section2]",
		"[section3]",
		"[section4]",
		"[section5]"
	},
	// expected result
	{
		"[section1]",
		"[section2]",
		"[section3]",
		"key=value",
		"[section4]",
		"[section5]"
	}
};

const T_INIFileData testDataEmptyNames {
	// filename
	"data12.ini",
	// original data
	{
		"[]",
		"key=value",
		"key2=value2",
		"[section]",
		"=value"
	},
	// expected result
	{
		"[]",
		"key=",
		"=value3",
		"[section]",
		"=value2",
		"[section2]",
		"="
	}
};

const T_INIFileData testDataMalformed1 {
	// filename
	"data13.ini",
	// original data
	{
		"[[name1]",
		"key=value",
		"[name2]]",
		"key=value",
		"[[name3]]",
		"key=value"
	},
	// expected result
	{
		"[[name1]",
		"key=value1",
		"[name2]]",
		"key=value2",
		"[[name3]]",
		"key=value3"
	}
};

const T_INIFileData testDataMalformed2 {
	// filename
	"data14.ini",
	// original data
	{
		"[name]",
		"\\===",         //  key: "="     value: "="
		"a\\= \\===b",   //  key: "a= ="  value: "=b"
		"c\\= \\===d"    //  key: "c= ="  value: "=d"
	},
	// expected result
	{
		"[name]",
		"\\====",         //  key: "="     value: "=="
		"a\\= \\===bb",   //  key: "a= ="  value: "=bb"
		"e\\===f=",         //  key: "e="  value: "=f=",
		"[other]",
		"\\==="
	}
};

const T_INIFileData testDataConsecutiveWrites {
	// filename
	"data15.ini",
	// original data
	{
		"[ Food ]",
		"Cheese = 32",
		"Ice Cream = 64",
		"Bananas = 128",
		"",
		"[  Things  ]",
		"Scissors   = AAA",
		"Wooden Box = BBB",
		"Speakers   = CCC"
	},
	// expected result
	{
		"[ Food ]",
		"Cheese = AAA",
		"Ice Cream = BBB",
		"Bananas = CCC",
		"soup=DDD",
		"",
		"[  Things  ]",
		"Scissors   = 32",
		"Wooden Box = 64",
		"Speakers   = 128",
		"book=256"
	}
};

const T_INIFileData testDataEmptyValues {
	// filename
	"data16.ini",
	// original data
	{
		"[section]",
		"key=value"
	},
	// expected result
	{
		"[section]",
		"key="
	}
};

//
// test cases
//
const lest::test mINI_tests[] = {
	CASE("TEST: Basic write")
	{
		// do some basic modifications to an INI file
		// then compare resulting file to expected data
		path filename = std::get<0>(testDataBasic);
		INIFile file(filename);
		INIStructure ini;
		EXPECT(file.read(ini) == true);
		ini["some section"]["some key"] = "2";
		ini["some section"]["yet another key"] = "3";
		EXPECT(file.write(ini) == true);
		EXPECT(verifyData(testDataBasic));
	},
	CASE("TEST: Garbage data")
	{
		path filename = std::get<0>(testDataWithGarbage);
		// read from file
		INIFile file(filename);
		INIStructure ini;
		EXPECT(file.read(ini) == true);
		// update data
		ini["section"].set({
			{"a", "2"},
			{"b", "3"},
			{"c", "4"},
			{"d", "5"},
			{"e", "6"},
			{"f", "7"},
			{"g", "8"}
		});
		ini["other section"].remove("dinosaurs"); // sorry, dinosaurs
		ini["extra section"].set({
			{"aaa", "2"},
			{"bbb", "3"},
			{"ccc", "4"},
			{"ddd", "5"}
		});
		ini["new section"]["test"] = "something";
		// write to file
		EXPECT(file.write(ini) == true);
		// verify data
		EXPECT(verifyData(testDataWithGarbage));
	},
	CASE("Test: Remove entry")
	{
		path filename = std::get<0>(testDataRemEntry);
		// read from file
		INIFile file(filename);
		INIStructure ini;
		EXPECT(file.read(ini) == true);
		// update data
		ini["section"].remove("data1");
		// write to file
		EXPECT(file.write(ini) == true);
		// verify data
		EXPECT(verifyData(testDataRemEntry));
	},
	CASE("Test: Remove section")
	{
		path filename = std::get<0>(testDataRemSection);
		// read from file
		INIFile file(filename);
		INIStructure ini;
		EXPECT(file.read(ini) == true);
		// update data
		ini.remove("section");
		// write to file
		EXPECT(file.write(ini) == true);
		// verify data
		EXPECT(verifyData(testDataRemSection));
	},
	CASE("Test: Duplicate keys")
	{
		path filename = std::get<0>(testDataDuplicateKeys);
		// read from file
		INIFile file(filename);
		INIStructure ini;
		EXPECT(file.read(ini) == true);
		// update data
		ini["section"]["data"] = "D";
		// write to file
		EXPECT(file.write(ini) == true);
		// verify data
		EXPECT(verifyData(testDataDuplicateKeys));
	},
	CASE("Test: Duplicate sections")
	{
		path filename = std::get<0>(testDataDuplicateSections);
		// read from file
		INIFile file(filename);
		INIStructure ini;
		EXPECT(file.read(ini) == true);
		// update data
		ini["section"]["data"] = "A";
		// write to file
		EXPECT(file.write(ini) == true);
		// verify data
		EXPECT(verifyData(testDataDuplicateSections));
	},
	CASE("Test: Pretty print")
	{
		path filename = std::get<0>(testDataPrettyPrint);
		// read from file
		INIFile file(filename);
		INIStructure ini;
		EXPECT(file.read(ini) == true);
		// update data
		ini["section1"]["value3"] = "C";
		ini["section2"]["value2"] = "B";
		ini["section3"].set({
			{"value1", "A"},
			{"value2", "B"}
		});
		// write to file
		EXPECT(file.write(ini, true) == true);
		// verify data
		EXPECT(verifyData(testDataPrettyPrint));
	},
	CASE("Test: Write to empty file")
	{
		path filename = std::get<0>(testDataEmptyFile);
		// read from file
		INIFile file(filename);
		INIStructure ini;
		EXPECT(file.read(ini) == true);
		// update data
		ini["section"]["key"] = "value";
		// write to file
		EXPECT(file.write(ini) == true);
		// verify data
		EXPECT(verifyData(testDataEmptyFile));
	},
	CASE("Test: Write to empty section")
	{
		path filename = std::get<0>(testDataEmptySection);
		// read from file
		INIFile file(filename);
		INIStructure ini;
		EXPECT(file.read(ini) == true);
		// update data
		ini["section"]["key"] = "value";
		// write to file
		EXPECT(file.write(ini) == true);
		// verify data
		EXPECT(verifyData(testDataEmptySection));
	},
	CASE("Test: Write to a single empty section among many")
	{
		path filename = std::get<0>(testDataManyEmptySections);
		// read from file
		INIFile file(filename);
		INIStructure ini;
		EXPECT(file.read(ini) == true);
		// update data
		ini["section3"]["key"] = "value";
		// write to file
		EXPECT(file.write(ini) == true);
		// verify data
		EXPECT(verifyData(testDataManyEmptySections));
	},
	CASE("Test: Write with empty section and key names")
	{
		path filename = std::get<0>(testDataEmptyNames);
		// read from file
		INIFile file(filename);
		INIStructure ini;
		EXPECT(file.read(ini) == true);
		// update data
		ini[""]["key"] = "";
		EXPECT(ini[""].remove("key2") == true);
		ini[""][""] = "value3";
		ini["section"][""] = "value2";
		ini["section2"][""] = "";
		// write to file
		EXPECT(file.write(ini) == true);
		// verify data
		EXPECT(verifyData(testDataEmptyNames));
	},
	CASE("Test: Write malformed section names")
	{
		path filename = std::get<0>(testDataMalformed1);
		// read from file
		INIFile file(filename);
		INIStructure ini;
		EXPECT(file.read(ini) == true);
		// update data
		ini["[name1"]["key"] = "value1";
		ini["name2]"]["key"] = "value2";
		ini["[name3]"]["key"] = "value3";
		// write to file
		EXPECT(file.write(ini) == true);
		// verify data
		EXPECT(verifyData(testDataMalformed1));
	},
	CASE("Test: Write malformed key names")
	{
		path filename = std::get<0>(testDataMalformed2);
		// read from file
		INIFile file(filename);
		INIStructure ini;
		EXPECT(file.read(ini) == true);
		// update data
		ini["name"].set({
			{"=", "  ==  "},
			{"a= =", "=bb"},
			{"e=", "  =f=  "}
		});
		ini["name"].remove("c= =");
		ini["other"]["="] = "=";
		// write to file
		EXPECT(file.write(ini) == true);
		// verify data
		EXPECT(verifyData(testDataMalformed2));
	},
	CASE("Test: Consecutive writes")
	{
		path filename = std::get<0>(testDataConsecutiveWrites);
		// read from file
		INIFile file(filename);
		INIStructure ini;
		EXPECT(file.read(ini) == true);
		// update data
		ini["food"].set({
			{"cheese", "AAA"},
			{"ice cream", "BBB"},
			{"bananas", "CCC"},
			{"soup", "DDD"}
		});
		ini["things"].set({
			{"scissors", "32"},
			{"wooden box", "64"},
			{"   speakers  ", "  128   "},
			{"  book  ", "  256  "}
		});
		// write to file multiple times
		for (unsigned int i = 0; i < 10; ++i)
		{
			EXPECT(file.write(ini) == true);
		}
		// verify data
		EXPECT(verifyData(testDataConsecutiveWrites));
	},
	CASE("Test: Empty values")
	{
		path filename = std::get<0>(testDataEmptyValues);
		// read from file
		INIFile file(filename);
		INIStructure ini;
		EXPECT(file.read(ini) == true);
		// update data
		ini["section"]["key"].clear();
		// write to file
		EXPECT(file.write(ini) == true);
		// verify data
		EXPECT(verifyData(testDataEmptyValues));
	}
};

int main(int argc, char** argv)
{
	// write test files
	writeTestFile(testDataBasic);
	writeTestFile(testDataWithGarbage);
	writeTestFile(testDataRemEntry);
	writeTestFile(testDataRemSection);
	writeTestFile(testDataDuplicateKeys);
	writeTestFile(testDataDuplicateSections);
	writeTestFile(testDataPrettyPrint);
	writeTestFile(testDataEmptyFile);
	writeTestFile(testDataEmptySection);
	writeTestFile(testDataManyEmptySections);
	writeTestFile(testDataEmptyNames);
	writeTestFile(testDataMalformed1);
	writeTestFile(testDataMalformed2);
	writeTestFile(testDataConsecutiveWrites);
	writeTestFile(testDataEmptyValues);
	
	// run tests
	if (int failures = lest::run(mINI_tests, argc, argv))
	{
		return failures;
	}
	return std::cout << std::endl << "All tests passed!" << std::endl, EXIT_SUCCESS;
}
