#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

template<class Compressor, class Decompressor>
void check(const char* name, const std::string& input)
{
    std::ostringstream compressed;
    {
        boost::iostreams::filtering_ostream out;
        out.push(Compressor());
        out.push(compressed);
        out.write(input.data(), static_cast<std::streamsize>(input.size()));
    }
    std::istringstream source(compressed.str());
    boost::iostreams::filtering_istream in;
    in.push(Decompressor());
    in.push(source);
    std::ostringstream restored;
    boost::iostreams::copy(in, restored);
    if(restored.str() != input)
        throw std::runtime_error(std::string(name) + " round trip failed");
    std::cout << name << ": " << input.size() << " bytes -> "
              << compressed.str().size() << " bytes -> exact original\n";
}

int main()
{
    try {
        std::string input;
        for(unsigned i = 0; i < 65536; ++i)
            input += static_cast<char>((i * 17 + i / 251) & 255);
        check<boost::iostreams::bzip2_compressor, boost::iostreams::bzip2_decompressor>("bzip2", input);
        check<boost::iostreams::zlib_compressor, boost::iostreams::zlib_decompressor>("zlib", input);
        check<boost::iostreams::gzip_compressor, boost::iostreams::gzip_decompressor>("gzip", input);
        return 0;
    }
    catch(const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
