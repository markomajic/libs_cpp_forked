/*
 * The MIT License (MIT)
 * Copyright (c) 2018 Danijel Durakovic
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

///////////////////////////////////////////////////////////////////////////////
//
//  /mINI/ v0.9.7
//  An INI file reader and writer for the modern age.
//
///////////////////////////////////////////////////////////////////////////////
//
//  A tiny utility library for manipulating INI files with a straightforward
//  API and a minimal footprint. It conforms to the (somewhat) standard INI
//  format - sections and keys are case insensitive and all leading and
//  trailing whitespace is ignored. Comments are lines that begin with a
//  semicolon. Trailing comments are allowed on section lines.
//
//  Files are read on demand, upon which data is kept in memory and the file
//  is closed. This utility supports lazy writing, which only writes changes
//  and updates to a file and preserves custom formatting and comments. A lazy
//  write invoked by a write() call will read the output file, find what
//  changes have been made and update the file accordingly. If you only need to
//  generate files, use generate() instead. Section and key order is preserved
//  on read, write and insert.
//
///////////////////////////////////////////////////////////////////////////////
//
//  /* BASIC USAGE EXAMPLE: */
//
//  /* read from file */
//  mINI::INIFile file("myfile.ini");
//  mINI::INIStructure ini;
//  file.read(ini);
//
//  /* read value; gets a reference to actual value in the structure.
//     if key or section don't exist, a new empty value will be created */
//  std::string& value = ini["section"]["key"];
//
//  /* read value safely; gets a copy of value in the structure.
//     does not alter the structure */
//  std::string value = ini.get("section").get("key");
//
//  /* set or update values */
//  ini["section"]["key"] = "value";
//
//  /* set multiple values */
//  ini["section2"].set({
//      {"key1", "value1"},
//      {"key2", "value2"}
//  });
//
//  /* write updates back to file, preserving comments and formatting */
//  file.write(ini);
//
//  /* or generate a file (overwrites the original) */
//  file.generate(ini);
//
///////////////////////////////////////////////////////////////////////////////
//
//  Long live the INI file!!!
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MINI_INI_H_
#define MINI_INI_H_

#include <string>
#include <sstream>
#include <algorithm>
#include <utility>
#include <unordered_map>
#include <vector>
#include <memory>
#include <fstream>
#include <filesystem>

#include <mini/ini_common.h>
#include <mini/ini_map.h>
#include <mini/ini_parser.h>
#include <mini/ini_reader.h>
#include <mini/ini_generator.h>
#include <mini/ini_writer.h>

namespace mINI
{

template <class T>
class INIFile
{
private:
	fs::path filename;
	mutable file_props fprops;
public:
	using INIStructure = T;

	INIFile(const fs::path& fn, file_props fp = file_props()) : filename(fn), fprops(fp) {}
	~INIFile() {}

	const file_props& get_props() const 	{return fprops;}
	void set_props(const file_props& p)		{fprops = p;}

	bool exists() const
	{
		if(!filename.empty()) {
			std::error_code ec;
			return fs::exists(filename, ec) && !ec;
		}
		return false;
	}

	bool read(INIStructure& data) const
	{
		if(exists()) {
			data.clear();
			INIReader<INIStructure> reader(filename);
			if(reader.read(data)) {
				fprops = reader.get_props();
				return true;
			}
		}
		return false;
	}

	bool generate(INIStructure const& data, bool pretty = false) const
	{
		if(!filename.empty()) {
			INIGenerator<INIStructure> generator(filename, fprops, pretty);
			return generator << data;
		}
		return false;
	}

	bool write(INIStructure& data, bool pretty = false) const
	{
		if(!filename.empty()) {
			if(exists()) {
				INIWriter<INIStructure> writer(filename, pretty);
				return writer << data;
			}
			else
				return generate(data, pretty);
		}
		return false;
	}
};

}

#endif // MINI_INI_H_
