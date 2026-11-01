//---------------------------------------------------------------------------
#pragma hdrstop
#include "unicode_utils.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#include <fstream>
#include <limits>
//#include <cuchar>

namespace unicode_utils
{
bool is_little_endian()
{
	static const int test = 0xff;
	static const bool is_le = *reinterpret_cast<const char *>(&test);
	return is_le;
}

unsigned get_char_size(encoding enc, bool& do_swap)
{
	switch(enc)
	{
		case bom_utf_16_be:	do_swap = is_little_endian(); return 2;
		case bom_utf_16_le:  do_swap = is_big_endian(); return 2;
		case bom_utf_32_be:	do_swap = is_little_endian(); return 4;
		case bom_utf_32_le:  do_swap = is_big_endian(); return 4;
		default:					do_swap = false; return 1;
	}
}

void swap_bytes(char16_t& c)
{
	char *p = reinterpret_cast<char *>(&c);
	std::swap(p[0], p[1]);
}

void swap_bytes(char32_t& c)
{
	char *p = reinterpret_cast<char *>(&c);
	std::swap(p[0], p[3]);
	std::swap(p[1], p[2]);
}


//---------------------------------------------------------------------------
// class bom
//---------------------------------------------------------------------------
const std::vector<int>& bom::signature(encoding enc)
{
	static const std::vector<int> sig[bom_count] = {
		{},
		{0xf7, 0x64, 0x4c},
		{0x2b, 0x2f, 0x76},
		{0xef, 0xbb, 0xbf},
		{0xfe, 0xff},
		{0xff, 0xfe},
		{0x00, 0x00, 0xfe, 0xff},
		{0xff, 0xfe, 0x00, 0x00},
		{0xdd, 0x73, 0x66, 0x73},
		{0x0e, 0xfe, 0xff},
		{0xfb, 0xee, 0x28},
		{0x84, 0x31, 0x95, 0x33}
	};
	return sig[enc];
}

encoding bom::check_bytes(encoding test_enc, std::vector<int>::const_iterator from, std::vector<int>::const_iterator to)
{
	for(std::vector<int>::const_iterator it = from; it != to; it++) {
		if(istrm.get() != *it)
			return bom_none;
	}
	save_state(); // save position and state
	return test_enc;
}

encoding bom::check_bom()
{
	int first_byte = istrm.get();
	if(first_byte>=0) {
		for(unsigned enc_type = bom_none+1; enc_type < bom_count; enc_type++) {
			const std::vector<int>& sig(signature(encoding(enc_type)));
			if(first_byte==sig[0]) {
				encoding __enc = check_bytes(encoding(enc_type), sig.begin()+1, sig.end());
				// now we should simply return the result of check_bytes() call - the only exception to this
				// is if the bom that was found was UTF16LE because then we still don't know if we're, in fact,
				// dealing with UTF16LE or UTF32LE (UTF16LE will always be found first because it appears first in
				// the enum/sig array). So, in this case, we also need to check the next 2 bytes to determine if the
				// bom is UTF16LE or UTF32LE...
				if(__enc == bom_utf_16_le) {
					const std::vector<int>& sig_32le(signature(bom_utf_32_le));
					if(check_bytes(bom_utf_32_le, sig_32le.begin()+2, sig_32le.end())==bom_utf_32_le)
						return bom_utf_32_le;
				}
				return __enc;
			}
		}
	}
	return bom_none;
}

void bom::save_state()
{
	stream_state = istrm.good();
	if(stream_state)
		return_pos = istrm.tellg();
}

void bom::restore_state()
{
	remaining_size = 0;
	// BOM probing may have reached EOF on a short or empty stream.
	if(stream_state && has_return_pos()) {
		istrm.clear();
		istrm.seekg(0, std::ios::end);
		auto end_pos = istrm.tellg();
		if(!istrm || end_pos < return_pos) {
			istrm.setstate(std::ios::failbit);
			return;
		}
		remaining_size = static_cast<std::size_t>(end_pos - return_pos);
		istrm.seekg(return_pos);
	}
}

bom::bom(std::istream& is) : istrm(is), stream_state(false), remaining_size{0}, return_pos{-1}, file_bom{bom_none}
{
	if(istrm) {
		save_state();
		file_bom = check_bom();
		restore_state();

	}
}

encoding bom::check(std::istream& is, std::size_t& remaining_size)
{
	bom __bom(is);
	remaining_size = __bom.remaining_size;
	return __bom.file_bom;
}

bool bom::write_signature(std::ostream& os, encoding enc)
{
	if(os) {
		const std::vector<int>& sig(signature(enc));
		for(auto ch : sig)
			os.put(ch);
	}
	return os.good();
}

//---------------------------------------------------------------------------
// struct file_props
//---------------------------------------------------------------------------
void file_props::init(encoding e)
{
	enc		= e;
#ifdef _WIN32
	use_cr	= true;
#else
	use_cr	= false;
#endif
}

file_props& file_props::assign(const file_props& other)
{
	enc		= other.enc;
	use_cr	= other.use_cr;
	return *this;
}

void file_props::swap(file_props& other)
{
	std::swap(enc, other.enc);
	std::swap(use_cr, other.use_cr);
}


//---------------------------------------------------------------------------
// class line_data
//---------------------------------------------------------------------------
bool line_data::write_line(std::ostream& os, const std::string& line) const
{
	if(os) {
		os << line;
		if(fprops.use_cr)
			os << '\r';
		os << '\n';
	}
	return os.good();
}

std::size_t line_data::char_count() const
{
	std::size_t ch_count = 0;
	for(const auto& line : *this)
		ch_count += line.size();
	return ch_count;
}

bool line_data::read_file(const fs::path& filename)
{
	std::ifstream file_strm;
	file_strm.open(filename, std::ios::in | std::ios::binary);
	return file_strm ? read_file(file_strm) : false;
}

bool line_data::read_file(std::istream& is)
{
	clear();
	fprops.init();
	fprops.use_cr = false;
	std::size_t data_size;
	fprops.enc = bom::check(is, data_size);
	bool do_swap;
	unsigned char_size = get_char_size(fprops.enc, do_swap);
	if(data_size%char_size)
		return false; // remaining size should always be a multiple of char_size - if it isn't, something is wrong with the file...
	if(data_size > static_cast<std::size_t>((std::numeric_limits<std::streamsize>::max)()))
		return false;
	const auto byte_size = static_cast<std::streamsize>(data_size);
	data_size /= char_size;

	if(is) {
		if(data_size>0) {
			try {
				switch(char_size)
				{
					case 1:
					{
						std::vector<char> contents(data_size);
						if(!is.read(contents.data(), byte_size))
							return false;
						read_lines(contents.data(), data_size, '\n', '\r');
						break;
					}
					case 2:
					{
						std::vector<char16_t> contents(data_size);
						if(!is.read(reinterpret_cast<char*>(contents.data()), byte_size))
							return false;
						auto data_ptr = contents.data();
						if(do_swap)
							swap_bytes(data_ptr, data_size);
						read_lines(data_ptr, data_size, u'\n', u'\r');
						break;
					}
					case 4:
					{
						std::vector<char32_t> contents(data_size);
						if(!is.read(reinterpret_cast<char*>(contents.data()), byte_size))
							return false;
						auto data_ptr = contents.data();
						if(do_swap)
							swap_bytes(data_ptr, data_size);
						read_lines(data_ptr, data_size, U'\n', U'\r');
						break;
					}
				}
			} catch (...) {return false;}
		}
		return true;
	}
	return false;
}

bool line_data::write_file(const fs::path& filename) const
{
	std::ofstream file_strm;
	file_strm.open(filename, std::ios::out|std::ios::trunc|std::ios::binary);
	if(!file_strm || !write_file(file_strm))
		return false;
	file_strm.close();
	return !file_strm.fail();
}

bool line_data::write_file(std::ostream& os) const
{
	bool do_swap;
	unsigned char_size = get_char_size(fprops.enc, do_swap);
	if(bom::write_signature(os, fprops.enc)) {
		for(const auto& line : *this) {
			bool success = false;
			switch(char_size)
			{
				case 1:	success = write_line(os, line); break;
				case 2:	success = write_line<char16_t>(os, line, do_swap); break;
				case 4:	success = write_line<char32_t>(os, line, do_swap); break;
			}
			if(!success)
				return false;
		}
	}
	return os.good();
}
} // namespace unicode_utils

