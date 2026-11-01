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
bool writeTestFile(const T_INIFileData& testData)
{
	path filename = testData.first;
	T_LineData lines = testData.second;
	std::ofstream fileWriteStream(filename);
	if (fileWriteStream.is_open())
	{
		if (lines.size())
		{
			auto it = lines.begin();
			for (;;)
			{
				fileWriteStream << *it;
				if (++it == lines.end())
				{
					break;
				}
				fileWriteStream << std::endl;
			}
		}
		return true;
	}
	return false;
}

void outputData(const INIStructure& ini)
{
	for (auto const& it : ini)
	{
		auto const& section = it.first;
		auto const& collection = it.second;
		std::cout << "[" << section << "]" << std::endl;
		for (auto const& it2 : collection)
		{
			auto const& key = it2.first;
			auto const& value = it2.second;
			std::cout << key << "=" << value << std::endl;
		}
		std::cout << std::endl;
	}
}

//
// test data
//
const T_INIFileData testDataBasic = {
	// filename
	"data01.ini",
	// test data
	{
		"[fruit]",
		"bananas=1",
		"apples=2",
		"grapes=3",
		"[veggies]",
		"lettuce=scarce",
		"onions=sufficient",
		"potatoes=plentiful"
	}
};

const T_INIFileData testDataWellFormed = {
	// filename
	"data02.ini",
	// test data
	{
		"; this is a comment",
		"[first section]",
		"someKey = 1",
		"anotherKey = 2",
		"",
		"; this is another comment",
		"[second section]",
		"humans = 16",
		"orcs = 8",
		"barbarians = 20",
		"",
		"; this is yet another comment",
		"[third section]",
		"spiders=25",
		"bugs=0",
		"ants=100",
		"flies=5"
	}
};

const T_INIFileData testDataNotWellFormed = {
	// filename
	"data03.ini",
	// test data
	{
		"GARBAGE",
		"; this is a comment",
		"    ; abcd   ",
		"[first section]   ;test comment",
		"someKey= 1",
		"GARBAGE",
		"anotherKey =2",
		"",
		"; this is another comment",
		"GARBAGE",
		"GARBAGE",
		"                [second section]",
		"GARBAGE",
		"hUmAnS=16",
		"GARBAGE",
		" orcs                   =   8    ",
		"   barbarians    =     20      ",
		"",
		"          GARBAGE",
		"; this is yet another comment",
		"[      third section   ]      ;;; test comment[]]] ",
		"spiders    = 25",
		"bugs       =0    ",
		"ants=       100",
		"GARBAGE   ",
		"GARBAGE  ",
		"FLIES =    5",
		"GARBAGE   "
	}
};

const T_INIFileData testDataEmpty = {
	// filename
	"data04.ini",
	// test data
	{}
};

const T_INIFileData testDataEdgeCase1 = {
	// filename
	"data05.ini",
	// test data
	{
		"ignored1=value1",
		"ignored2=value2"
	}
};

const T_INIFileData testDataEdgeCase2 = {
	// filename
	"data06.ini",
	// test data
	{
		"ignored1=value1",
		"ignored2=value2",
		"[data]",
		"proper1=a",
		"proper2=b"
	}
};

const T_INIFileData testDataEdgeCase3 = {
	// filename
	"data07.ini",
	// test data
	{
		"[empty]"
	}
};

const T_INIFileData testDataEdgeCase4 = {
	// filename
	"data08.ini",
	// test data
	{
		"[empty1]",
		"[empty2]",
		"[empty3]",
		"[empty4]",
		"[empty5]"
	}
};

const T_INIFileData testDataEdgeCase5 = {
	// filename
	"data09.ini",
	// test data
	{
		"[data]",
		"valueA=1",  // expected: ignored
		"valueA=2"   // expected: not ignored
	}
};

const T_INIFileData testDataEdgeCase6 = {
	// filename
	"data10.ini",
	// test data
	{
		"[data]",    // expected: ignored
		"valueA=10", // expected: ignored
		"valueB=20", // expected: not ignored
		"[data]",
		"valueA=30"  // expected: not ignored
	}
};

const T_INIFileData testDataEdgeCase7 = {
	// filename
	"data11.ini",
	// test data
	{
		"[]",
		"key1=value1",
		"[a]",
		"key2=value2"
	}
};

const T_INIFileData testDataEdgeCase8 = {
	// filename
	"data12.ini",
	// test data
	{
		"[a]",
		"=1"
	}
};

const T_INIFileData testDataMalformed1 = {
	// filename
	"data13.ini",
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
	"data14.ini",
	// test data
	{
		"[name]",
		"key\\==value",
		"\\===", // expected key: "="   expected value: "="
		"a\\= \\===b",  //  expected key: "a= ="  expected value: "=b"
		"   c\\= \\=  =       =d "  //  expected key: "c= ="  expected value: "=d"
	}
};

//
// test cases
//
const lest::test mINI_tests[] = {
	CASE("Test: Basic read")
	{
		// read a basic INI file and check if values are read correctly
		path filename = testDataBasic.first;
		INIFile file(filename);
		INIStructure ini;
		EXPECT(file.read(ini) == true);
		std::cout << filename << std::endl;
		outputData(ini);
		EXPECT(ini["fruit"]["bananas"] == "1");
		EXPECT(ini["fruit"]["apples"] == "2");
		EXPECT(ini["fruit"]["grapes"] == "3");
		EXPECT(ini["veggies"]["lettuce"] == "scarce");
		EXPECT(ini["veggies"]["onions"] == "sufficient");
		EXPECT(ini["veggies"]["potatoes"] == "plentiful");
	},
	CASE("Test: Read and compare")
	{
		// read two INI files with differing form and check if all read values match
		path filename1 = testDataWellFormed.first;
		path filename2 = testDataNotWellFormed.first;
		INIFile file1(filename1);
		INIFile file2(filename2);
		INIStructure ini1, ini2;
		EXPECT(file1.read(ini1) == true);
		EXPECT(file2.read(ini2) == true);
		std::cout << filename1 << std::endl;
		outputData(ini1);
		std::cout << filename2 << std::endl;
		outputData(ini2);
		// compare sizes
		EXPECT(ini1.size() == ini2.size());
		EXPECT(ini1.get("first section").size() == ini2.get("first section").size());
		EXPECT(ini1.get("second section").size() == ini2.get("second section").size());
		EXPECT(ini1.get("third section").size() == ini2.get("third section").size());
		// compare data
		EXPECT(ini1.get("first section").get("somekey") == ini2.get("first section").get("somekey"));
		EXPECT(ini1["first section"]["anotherkey"] == ini2["first section"]["anotherkey"]);
		EXPECT(ini1["second section"]["humans"] == ini2["second section"]["humans"]);
		EXPECT(ini1["second section"]["orcs"] == ini2["second section"]["orcs"]);
		EXPECT(ini1["second section"]["barbarians"] == ini2["second section"]["barbarians"]);
		EXPECT(ini1["third section"]["spiders"] == ini2["third section"]["spiders"]);
		EXPECT(ini1["third section"]["bugs"] == ini2["third section"]["bugs"]);
		EXPECT(ini1["third section"]["ants"] == ini2["third section"]["ants"]);
		EXPECT(ini1["third section"]["flies"] == ini2["third section"]["flies"]);
	},
	CASE("Test: Read empty")
	{
		path filename = testDataEmpty.first;
		INIFile file(filename);
		INIStructure ini;
		EXPECT(file.read(ini) == true);
		EXPECT(ini.size() == 0u);
	},
	CASE("Test: Edge case 1")
	{
		path filename = testDataEdgeCase1.first;
		INIFile file(filename);
		INIStructure ini;
		EXPECT(file.read(ini) == true);
		EXPECT(ini.size() == 0u);
	},
	CASE("Test: Edge case 2")
	{
		path filename = testDataEdgeCase2.first;
		INIFile file(filename);
		INIStructure ini;
		EXPECT(file.read(ini) == true);
		EXPECT(ini.size() == 1u);
		EXPECT(ini.get("data").size() == 2u);
		EXPECT(ini["data"]["proper1"] == "a");
		EXPECT(ini["data"]["proper2"] == "b");
	},
	CASE("Test: Edge case 3")
	{
		path filename = testDataEdgeCase3.first;
		INIFile file(filename);
		INIStructure ini;
		EXPECT(file.read(ini) == true);
		EXPECT(ini.size() == 1u);
		EXPECT(ini.get("empty").size() == 0u);
	},
	CASE("Test: Edge case 4")
	{
		path filename = testDataEdgeCase4.first;
		INIFile file(filename);
		INIStructure ini;
		EXPECT(file.read(ini) == true);
		EXPECT(ini.size() == 5u);
		EXPECT(ini.get("empty1").size() == 0u);
		EXPECT(ini.get("empty2").size() == 0u);
		EXPECT(ini.get("empty3").size() == 0u);
		EXPECT(ini.get("empty4").size() == 0u);
		EXPECT(ini.get("empty5").size() == 0u);
	},
	CASE("Test: Edge case 5")
	{
		path filename = testDataEdgeCase5.first;
		INIFile file(filename);
		INIStructure ini;
		EXPECT(file.read(ini) == true);
		EXPECT(ini.size() == 1u);
		EXPECT(ini.get("data").size() == 1u);
		EXPECT(ini["data"]["valueA"] == "2");
	},
	CASE("Test: Edge case 6")
	{
		path filename = testDataEdgeCase6.first;
		INIFile file(filename);
		INIStructure ini;
		EXPECT(file.read(ini) == true);
		EXPECT(ini.size() == 1u);
		EXPECT(ini.get("data").size() == 2u);
		EXPECT(ini["data"]["valueA"] == "30");
		EXPECT(ini["data"]["valueB"] == "20");
	},
	CASE("Test: Edge case 7")
	{
		path filename = testDataEdgeCase7.first;
		INIFile file(filename);
		INIStructure ini;
		EXPECT(file.read(ini) == true);
		EXPECT(ini.size() == 2u);
		EXPECT(ini.get("").size() == 1u);
		EXPECT(ini.get("").get("key1") == "value1");
		EXPECT(ini.get("a").size() == 1u);
		EXPECT(ini["a"]["key2"] == "value2");
	},
	CASE("Test: Edge case 8")
	{
		path filename = testDataEdgeCase8.first;
		INIFile file(filename);
		INIStructure ini;
		EXPECT(file.read(ini) == true);
		EXPECT(ini.size() == 1u);
		EXPECT(ini.get("a").size() == 1u);
		EXPECT(ini["a"][""] == "1");
	},
	CASE("Test: Malformed case 1")
	{
		// read INI with malformed section names
		path filename = testDataMalformed1.first;
		INIFile file(filename);
		INIStructure ini;
		EXPECT(file.read(ini) == true);
		EXPECT(ini.size() == 3u);
		EXPECT(ini.get("[name1").size() == 1u);
		EXPECT(ini.get("name2]").size() == 1u);
		EXPECT(ini.get("[name3]").size() == 1u);
	},
	CASE("Test: Malformed case 2")
	{
		// read INI with malformed key names
		path filename = testDataMalformed2.first;
		INIFile file(filename);
		INIStructure ini;
		EXPECT(file.read(ini) == true);
		EXPECT(ini.get("name").get("key=") == "value");
		EXPECT(ini.get("name").get("=") == "=");
		EXPECT(ini.get("name").get("a= =") == "=b");
		EXPECT(ini.get("name").get("c= =") == "=d");
	}
};

int main(int argc, char** argv)
{
	// write test files
	writeTestFile(testDataBasic);
	writeTestFile(testDataWellFormed);
	writeTestFile(testDataNotWellFormed);
	writeTestFile(testDataEmpty);
	writeTestFile(testDataEdgeCase1);
	writeTestFile(testDataEdgeCase2);
	writeTestFile(testDataEdgeCase3);
	writeTestFile(testDataEdgeCase4);
	writeTestFile(testDataEdgeCase5);
	writeTestFile(testDataEdgeCase6);
	writeTestFile(testDataEdgeCase7);
	writeTestFile(testDataEdgeCase8);
	writeTestFile(testDataMalformed1);
	writeTestFile(testDataMalformed2);

	// run tests
	if (int failures = lest::run(mINI_tests, argc, argv))
	{
		return failures;
	}
	return std::cout << std::endl << "All tests passed!" << std::endl, EXIT_SUCCESS;
}
