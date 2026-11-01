#include "lest/lest.hpp"
#include <mINI/ini.h>
#include <clocale>
#include <iterator>
#include <sstream>
#include <streambuf>

using INIStructure = mINI::INIStructureCI;
using INIFile = mINI::INIFile<INIStructure>;
using namespace unicode_utils;

std::string bytes(const fs::path& path)
{
    std::ifstream input(path, std::ios::binary);
    return {std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>()};
}

// Seeking reports the full length, but reading supplies fewer bytes.
class short_read_buffer : public std::stringbuf
{
public:
    short_read_buffer() : std::stringbuf("[section]\nkey=value\n") {}
protected:
    std::streamsize xsgetn(char* data, std::streamsize size) override
    {
        return std::stringbuf::xsgetn(data, size > 0 ? size - 1 : 0);
    }
};

const lest::test tests[] = {
    CASE("Short streams and BOM-only files retain their contents and properties")
    {
        for (const auto& text : {std::string(), std::string("x"), std::string("[]"),
                                std::string("\xef"), std::string("\xef\xbb")}) {
            std::istringstream input(text);
            line_data lines;
            EXPECT(lines.read_file(input));
            EXPECT(lines.size() == (text.empty() ? 0u : 1u));
            if (!text.empty())
                EXPECT(lines.front() == text);
        }
        for (auto enc : {bom_utf_8, bom_utf_16_le, bom_utf_16_be, bom_utf_32_le, bom_utf_32_be}) {
            std::ostringstream output;
            EXPECT(bom::write_signature(output, enc));
            std::istringstream input(output.str());
            line_data lines;
            EXPECT(lines.read_file(input));
            EXPECT(lines.empty());
            EXPECT(lines.get_props().enc == enc);
        }
        // UTF-16LE detection probes two extra bytes to rule out UTF-32LE.
        std::istringstream input(std::string("\xff\xfe", 2) + std::string("x\0", 2));
        line_data lines;
        EXPECT(lines.read_file(input));
        EXPECT(lines.size() == 1u);
        EXPECT(lines.front() == "x");
    },
    CASE("Each read detects newlines independently and preserves a final CR")
    {
        line_data lines;
        std::istringstream crlf("a\r\nb\r\n");
        EXPECT(lines.read_file(crlf));
        EXPECT(lines.get_props().use_cr);
        std::istringstream lf("a\nb\n");
        EXPECT(lines.read_file(lf));
        EXPECT(!lines.get_props().use_cr);
        std::ostringstream output;
        EXPECT(lines.write_file(output));
        EXPECT(output.str() == "a\nb\n");
        std::istringstream final_cr("a\r");
        EXPECT(lines.read_file(final_cr));
        EXPECT(lines.front() == "a\r");
        EXPECT(!lines.get_props().use_cr);
    },
    CASE("Truncated encoded units and short reads fail")
    {
        for (auto text : {std::string("\xff\xfe", 2) + "x",
                          std::string("\0\0\xfe\xff", 4) + "xy"}) {
            std::istringstream input(text);
            line_data lines;
            EXPECT(!lines.read_file(input));
        }
        short_read_buffer buffer;
        std::istream input(&buffer);
        line_data lines;
        EXPECT(!lines.read_file(input));
        EXPECT(lines.empty());
    },
    CASE("Generate and lazy write preserve requested Unicode encoding and newlines")
    {
        const std::string value = "Euro \xe2\x82\xac, CJK \xe4\xbd\xa0\xe5\xa5\xbd, Gothic \xf0\x90\x8d\x88";
        for (auto enc : {bom_none, bom_utf_8, bom_utf_16_le, bom_utf_16_be, bom_utf_32_le, bom_utf_32_be}) {
            for (bool cr : {false, true}) {
                file_props props(enc);
                props.use_cr = cr;
                INIFile file("encoding.ini", props);
                INIStructure data;
                data["Section"]["Key"] = value;
                EXPECT(file.generate(data));
                auto raw = bytes("encoding.ini");
                auto& signature = bom::signature(enc);
                for (std::size_t i = 0; i < signature.size(); ++i)
                    EXPECT(static_cast<unsigned char>(raw[i]) == signature[i]);
                INIStructure actual;
                EXPECT(file.read(actual));
                EXPECT(actual.get("section").get("key") == value);
                EXPECT(file.get_props().enc == enc);
                EXPECT(file.get_props().use_cr == cr);
                EXPECT(file.write(actual));
                EXPECT(bytes("encoding.ini") == raw);
                actual["section"]["key"] += " changed";
                actual["section"]["new"] = value;
                EXPECT(file.write(actual));
                EXPECT(file.read(actual));
                EXPECT(actual.get("section").get("key") == value + " changed");
                EXPECT(actual.get("section").get("new") == value);
                EXPECT(file.get_props().enc == enc);
                EXPECT(file.get_props().use_cr == cr);
            }
        }
    },
    CASE("Wide Unicode I/O matches independently specified bytes")
    {
        const std::vector<std::pair<encoding, std::vector<unsigned char>>> fixtures = {
            {bom_utf_16_le, {0xff, 0xfe, 0xac, 0x20, 0x00, 0xd8, 0x48, 0xdf, 0x0a, 0x00}},
            {bom_utf_16_be, {0xfe, 0xff, 0x20, 0xac, 0xd8, 0x00, 0xdf, 0x48, 0x00, 0x0a}},
            {bom_utf_32_le, {0xff, 0xfe, 0x00, 0x00, 0xac, 0x20, 0x00, 0x00,
                            0x48, 0x03, 0x01, 0x00, 0x0a, 0x00, 0x00, 0x00}},
            {bom_utf_32_be, {0x00, 0x00, 0xfe, 0xff, 0x00, 0x00, 0x20, 0xac,
                            0x00, 0x01, 0x03, 0x48, 0x00, 0x00, 0x00, 0x0a}}
        };
        const std::string text = "\xe2\x82\xac\xf0\x90\x8d\x88";
        for (const auto& fixture : fixtures) {
            const std::string expected(fixture.second.begin(), fixture.second.end());
            std::istringstream input(expected);
            line_data decoded;
            EXPECT(decoded.read_file(input));
            EXPECT(decoded.size() == 1u);
            EXPECT(decoded.front() == text);
            line_data encoded(std::vector<std::string>{text});
            file_props props(fixture.first);
            props.use_cr = false;
            encoded.set_props(props);
            std::ostringstream output;
            EXPECT(encoded.write_file(output));
            EXPECT(output.str() == expected);
        }
    },
    CASE("Empty generation creates or truncates a real file and reports invalid paths")
    {
        INIStructure empty;
        INIFile file("empty.ini");
        EXPECT(file.generate(empty));
        EXPECT(file.exists());
        EXPECT(fs::file_size("empty.ini") == 0u);
        INIStructure data;
        data["section"]["key"] = "value";
        EXPECT(file.generate(data));
        EXPECT(file.generate(empty));
        EXPECT(fs::file_size("empty.ini") == 0u);
        INIFile missing("missing-parent/empty.ini");
        EXPECT(!missing.generate(empty));
        fs::create_directory("directory.ini");
        INIFile directory("directory.ini");
        EXPECT(!directory.generate(empty));
        EXPECT(fs::is_directory("directory.ini"));
    },
    CASE("Unicode writing leaves the process locale unchanged")
    {
        const std::string previous = std::setlocale(LC_ALL, nullptr);
        std::setlocale(LC_ALL, "C");
        line_data lines(std::vector<std::string>{"\xe2\x82\xac"});
        lines.set_props(file_props(bom_utf_16_le));
        std::ostringstream output;
        const bool written = lines.write_file(output);
        const std::string after = std::setlocale(LC_ALL, nullptr);
        std::setlocale(LC_ALL, previous.c_str());
        EXPECT(written);
        EXPECT(after == "C");
    },
    CASE("Case insensitive lookup handles UTF-8 and embedded NUL bytes consistently")
    {
        const std::string a("A\0x", 3), b("a\0y", 3), c("a\0X", 3);
        EXPECT(!mINI::ci::comp{}(a, b));
        EXPECT(mINI::ci::comp{}(a, c));
        EXPECT(mINI::ci::hash{}(a) == mINI::ci::hash{}(c));
        INIStructure data;
        data[a]["\xe4\xbd\xa0Key"] = "first";
        data[b]["\xe4\xbd\xa0Key"] = "second";
        EXPECT(data.size() == 2u);
        EXPECT(data.get(c).get("\xe4\xbd\xa0KEY") == "first");
        EXPECT(data.get(b).get("\xe4\xbd\xa0key") == "second");
    }
};

int main(int argc, char** argv)
{
    return lest::run(tests, argc, argv);
}
