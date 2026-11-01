#ifndef unicode_utils_hpp
#define unicode_utils_hpp
#include <windows.h>
#include <iostream>
#include <vector>
#include <filesystem>
#include <cuchar>

namespace unicode_utils
{
	namespace fs = std::filesystem;

	enum encoding {bom_none=0, bom_utf_1, bom_utf_7, bom_utf_8, bom_utf_16_be, bom_utf_16_le, bom_utf_32_be, bom_utf_32_le, bom_utf_ebcdiic, bom_scsu, bom_bocu_1, bom_gb_18030, bom_count};

	inline bool is_little_endian()
	{
		static const int test = 0xff;
		static const bool is_le = *reinterpret_cast<const char *>(&test);
		return is_le;
	}
	inline bool is_big_endian() {return !is_little_endian();}

	inline unsigned get_char_size(encoding enc, bool& do_swap)
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

	inline void swap_bytes(char16_t& c)
	{
		char *p = reinterpret_cast<char *>(&c);
		std::swap(p[0], p[1]);
	}
	inline void swap_bytes(char32_t& c)
	{
		char *p = reinterpret_cast<char *>(&c);
		std::swap(p[0], p[3]);
		std::swap(p[1], p[2]);
	}

	template <typename CharT>
	inline void swap_bytes(CharT *ptr, std::size_t length)
	{
		for(std::size_t i=0; i<length; i++)
			swap_bytes(ptr[i]);
	}

	class bom
	{
	public:
		static const std::vector<int>& signature(encoding enc)
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
	private:
		std::istream& istrm;
		std::istream::pos_type	return_pos;
		bool							stream_state;
		std::size_t remaining_size;
		encoding file_bom;

		encoding check_bytes(encoding test_enc, std::vector<int>::const_iterator from, std::vector<int>::const_iterator to)
		{
			for(std::vector<int>::const_iterator it = from; it != to; it++) {
				if(istrm.get() != *it)
					return bom_none;
			}
			save_state(); // save position and state
			return test_enc;
		}

		encoding check_bom()
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

		bool has_return_pos() const {return return_pos>=0;}

		void save_state()
		{
			stream_state = istrm;
			if(stream_state)
				return_pos = istrm.tellg();
		}

		void restore_state()
		{
			if(istrm && has_return_pos()) {
				istrm.seekg(0, std::ios::end);
				remaining_size = istrm.tellg() - return_pos;
			}
			else
				remaining_size = 0;
			if(stream_state) {
				istrm.clear();
				if(has_return_pos())
					istrm.seekg(return_pos, std::ios::beg);
			}
		}

		bom(std::istream& is) : istrm(is), stream_state(false), remaining_size{0}, return_pos{-1}, file_bom{bom_none}
		{
			if(istrm) {
				save_state();
				file_bom = check_bom();
				restore_state();

			}
		}
	public:
		static encoding check(std::istream& is, std::size_t& remaining_size)
		{
			bom __bom(is);
			remaining_size = __bom.remaining_size;
			return __bom.file_bom;
		}

		static bool write_signature(std::ostream& os, encoding enc)
		{
			if(os) {
				const std::vector<int>& sig(signature(enc));
				for(auto ch : sig)
					os.put(ch);
			}
			return os;
		}
	};

	struct file_props
	{
		encoding enc;
		bool use_cr;
		void init(encoding e = bom_none)
		{
			enc		= e;
#ifdef _WIN32
			use_cr	= true;
#else
			use_cr	= false;
#endif
		}
		file_props& assign(const file_props& other)
		{
			enc		= other.enc;
			use_cr	= other.use_cr;
			return *this;
		}
		void swap(file_props& other)
		{
			std::swap(enc, other.enc);
			std::swap(use_cr, other.use_cr);
		}

		file_props(encoding e = bom_none)	{init(e);}
		file_props(const file_props& other)	{assign(other);}
		file_props& operator =(const file_props& other) {return assign(other);}
	};

	class line_data : public std::vector<std::string>
	{
		file_props fprops;

		void add_line(const char *line_begin, const char *line_end)				{emplace_back(line_begin, line_end);}
		void add_line(const char16_t *line_begin, const char16_t *line_end)	{emplace_back(std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t>().to_bytes(line_begin, line_end));}
		void add_line(const char32_t *line_begin, const char32_t *line_end)	{emplace_back(std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>().to_bytes(line_begin, line_end));}

		template <typename CharT>
		const CharT *add_line(const CharT *line_begin, const CharT *line_end, CharT cr_char)
		{
			const CharT *new_line_begin = line_end+1;
			if(line_end>line_begin && *(line_end-1) == cr_char) {
				fprops.use_cr = true;
				--line_end;
			}
			add_line(line_begin, line_end);
			return new_line_begin;
		}

		template <typename CharT>
		void read_lines(const CharT *data_begin, std::size_t data_length, CharT lf_char, CharT cr_char)
		{
			const CharT *data_end = data_begin + data_length;
			const CharT *line_begin = data_begin;
			const CharT *line_end = line_begin;
			while(line_end < data_end) {
				if (*line_end == lf_char)
					line_end = line_begin = add_line(line_begin, line_end, cr_char);
				else
					++line_end;
			}
			if(line_begin < line_end) // add trailing bytes only if the last character in the sequence was NOT '\n' (if it was then there are no trailing blanks)
				add_line(line_begin, line_end, cr_char);
		}

		bool write_line(std::ostream& os, const std::string& line) const
		{
			if(os) {
				os << line;
				if(fprops.use_cr)
					os << '\r';
				os << '\n';
			}
			return os;
		}

		template <typename CharT>
		bool write_line(std::ostream& os, const std::string& ln, bool do_swap) const
		{
			if(os) {
				std::string line(ln + (fprops.use_cr ? "\r\n" : "\n"));
				std::basic_string<CharT> wstr = std::wstring_convert<std::codecvt_utf8_utf16<CharT>, CharT>{}.from_bytes(line.data());
				for(auto ch : wstr) {
					if(do_swap)
						swap_bytes(ch);
					os.write(reinterpret_cast<const char *>(&ch), sizeof(CharT));
				}
			}
			return os;
		}

	public:
		line_data() {}
		line_data(const std::vector<std::string>& v) : std::vector<std::string>(v)			{}
		line_data(const line_data& ld) : std::vector<std::string>(ld), fprops(ld.fprops)	{}
		line_data(const fs::path& filename)																{read_file(filename);}
		line_data(line_data&& other) noexcept : std::vector<std::string>(other)				{fprops.swap(other.fprops);}
		virtual ~line_data() {}

		const file_props& get_props() const 	{return fprops;}
		void set_props(const file_props& p)		{fprops = p;}

		std::size_t char_count() const
		{
			std::size_t ch_count = 0;
			for(const auto& line : *this)
				ch_count += line.size();
			return ch_count;
		}

		bool read_file(const fs::path& filename)
		{
			OutputDebugStringW(L"line_data::read_file(path) #1");
			OutputDebugStringW(filename.c_str());
			std::ifstream file_strm;
			OutputDebugStringW(L"line_data::read_file(path) #2");
			file_strm.open(filename, std::ios::in | std::ios::binary);
			OutputDebugStringW(L"line_data::read_file(path) #3");
			return file_strm ? read_file(file_strm) : false;
		}

		bool read_file(std::istream& is)
		{
			OutputDebugStringW(L"line_data::read_file() #1");
			clear();
			std::size_t data_size;
			OutputDebugStringW(L"line_data::read_file() #2");
			fprops.enc = bom::check(is, data_size);
			OutputDebugStringW(L"line_data::read_file() #3");
			bool do_swap;
			unsigned char_size = get_char_size(fprops.enc, do_swap);
			OutputDebugStringW(L"line_data::read_file() #4");
			if(data_size%char_size)
				return false; // remaining size should always be a multiple of char_size - if it isn't, something is wrong with the file...
			data_size /= char_size;
			OutputDebugStringW(L"line_data::read_file() #5");

			if(is) {
				if(data_size>0) {
					try {
						OutputDebugStringW(L"line_data::read_file() #6");
						std::unique_ptr<char[]> file_contents(new char[data_size*char_size]);
						is.read(file_contents.get(), data_size*char_size);
						OutputDebugStringW(L"line_data::read_file() #7");
						switch(char_size)
						{
							case 1:
								read_lines(file_contents.get(), data_size, '\n', '\r');
								break;
							case 2:
							{
								char16_t *data_ptr = reinterpret_cast<char16_t *>(file_contents.get());
								if(do_swap)
									swap_bytes(data_ptr, data_size);
								read_lines(data_ptr, data_size, u'\n', u'\r');
								break;
							}
							case 4:
							{
								char32_t *data_ptr = reinterpret_cast<char32_t *>(file_contents.get());
								if(do_swap)
									swap_bytes(data_ptr, data_size);
								read_lines(data_ptr, data_size, U'\n', U'\r');
								break;
							}
						}
						OutputDebugStringW(L"line_data::read_file() #8");
					} catch (...) {return false;}
				}
				OutputDebugStringW(L"line_data::read_file() return true");
				return true;
			}
			OutputDebugStringW(L"line_data::read_file() return false");
			return false;
		}

		bool write_file(const fs::path& filename) const
		{
			std::ofstream file_strm;
			file_strm.open(filename, std::ios::out|std::ios::trunc|std::ios::binary);
			return file_strm ? write_file(file_strm) : false;
		}

		bool write_file(std::ostream& os) const
		{
			bool do_swap;
			unsigned char_size = get_char_size(fprops.enc, do_swap);
			if(char_size>1)
				std::setlocale(LC_ALL, "en_US.utf8");
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
			return os;
		}
	};
} // namespace unicode_utils
#endif // unicode_utils_hpp
