//---------------------------------------------------------------------------
#ifndef unicode_utilsH
#define unicode_utilsH
//---------------------------------------------------------------------------
#include <iostream>
#include <vector>
#include <filesystem>
#include <boost/locale/encoding_utf.hpp>
#include <utility>
//#include <boost/locale/utf8_codecvt.hpp>

//#include <cuchar>
#if defined(__BORLANDC__) && !defined(MINI_NO_AUTOLINK)
#pragma comment(lib, "mINI")
#endif

namespace unicode_utils
{
	namespace fs = std::filesystem;

	enum encoding {bom_none=0, bom_utf_1, bom_utf_7, bom_utf_8, bom_utf_16_be, bom_utf_16_le, bom_utf_32_be, bom_utf_32_le, bom_utf_ebcdiic, bom_scsu, bom_bocu_1, bom_gb_18030, bom_count};

	extern bool is_little_endian();
	inline bool is_big_endian() {return !is_little_endian();}

	extern unsigned get_char_size(encoding enc, bool& do_swap);

	extern void swap_bytes(char16_t& c);
	extern void swap_bytes(char32_t& c);

	template <typename CharT>
	inline void swap_bytes(CharT *ptr, std::size_t length)
	{
		for(std::size_t i=0; i<length; i++)
			swap_bytes(ptr[i]);
	}

	class bom
	{
	public:
		static const std::vector<int>& signature(encoding enc);
	private:
		std::istream& istrm;
		std::istream::pos_type	return_pos;
		bool							stream_state;
		std::size_t remaining_size;
		encoding file_bom;

		encoding check_bytes(encoding test_enc, std::vector<int>::const_iterator from, std::vector<int>::const_iterator to);
		encoding check_bom();

		bool has_return_pos() const {return return_pos>=0;}

		void save_state();
		void restore_state();

		bom(std::istream& is);
	public:
		static encoding check(std::istream& is, std::size_t& remaining_size);
		static bool write_signature(std::ostream& os, encoding enc);
	};

	struct file_props
	{
		encoding enc;
		bool use_cr;
		void init(encoding e = bom_none);
		file_props& assign(const file_props& other);
		void swap(file_props& other);

		file_props(encoding e = bom_none)	{init(e);}
		file_props(const file_props& other)	{assign(other);}
		file_props& operator =(const file_props& other) {return assign(other);}
	};

	class line_data : public std::vector<std::string>
	{
		file_props fprops;

		void add_line(const char *line_begin, const char *line_end)         {emplace_back(line_begin, line_end);}
		void add_line(const char16_t *line_begin, const char16_t *line_end)
        {
			std::string utf8_string = boost::locale::conv::utf_to_utf<char>(line_begin, line_end);
			emplace_back(std::move(utf8_string));
        }
		void add_line(const char32_t *line_begin, const char32_t *line_end)
		{
			std::string utf8_string = boost::locale::conv::utf_to_utf<char>(line_begin, line_end);
			emplace_back(std::move(utf8_string));
		}
//		void add_line(const char16_t *line_begin, const char16_t *line_end)	{emplace_back(std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t>().to_bytes(line_begin, line_end));}
//		void add_line(const char32_t *line_begin, const char32_t *line_end)	{emplace_back(std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>().to_bytes(line_begin, line_end));}

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
			if(line_begin < line_end) // The final line has no LF to consume.
				add_line(line_begin, line_end);
		}

		bool write_line(std::ostream& os, const std::string& line) const;

		template <typename CharT>
		bool write_line(std::ostream& os, const std::string& ln, bool do_swap) const
		{
			if(os) {
				std::string line(ln + (fprops.use_cr ? "\r\n" : "\n"));
				std::basic_string<CharT> wstr = boost::locale::conv::utf_to_utf<CharT>(line);
//				std::basic_string<CharT> wstr = std::wstring_convert<std::codecvt_utf8_utf16<CharT>, CharT>{}.from_bytes(line.data());
				for(auto ch : wstr) {
					if(do_swap)
						swap_bytes(ch);
					os.write(reinterpret_cast<const char *>(&ch), sizeof(CharT));
				}
			}
			return os.good();
		}

	public:
		line_data() {}
		line_data(const std::vector<std::string>& v) : std::vector<std::string>(v)			{}
		line_data(const line_data& ld) : std::vector<std::string>(ld), fprops(ld.fprops)	{}
		line_data(const fs::path& filename)																{read_file(filename);}
		line_data(line_data&& other) noexcept : std::vector<std::string>(std::move(other)), fprops(other.fprops) {}
		virtual ~line_data() {}

		const file_props& get_props() const 	{return fprops;}
		void set_props(const file_props& p)		{fprops = p;}

		std::size_t char_count() const;
		bool read_file(const fs::path& filename);
		bool read_file(std::istream& is);
		bool write_file(const fs::path& filename) const;
		bool write_file(std::ostream& os) const;
	};
} // namespace unicode_utils
#endif // unicode_utilsH

