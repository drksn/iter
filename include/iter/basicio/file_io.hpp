#ifndef ITER_FILE_IO_HPP
#define ITER_FILE_IO_HPP

#include <iter/log.hpp>
#include <functional>
#include <fstream>
#include <string>

namespace iter {

class FileReader : public std::binary_function <const std::string&, std::string*, bool> {
public:
    bool operator () (first_argument_type filename, second_argument_type content);
};

class FileWriter : public std::binary_function <const std::string&, const std::string&, bool> {
public:
    bool operator () (first_argument_type content, second_argument_type filename);
};

inline bool FileReader::operator () (first_argument_type filename, second_argument_type content) {
    std::ifstream in(filename.c_str(), std::ios::in | std::ios::binary);
    if (in) {
        try {
            in.seekg(0, std::ios::end);
            auto file_size = in.tellg();
            content->resize(file_size);
            if (file_size > 0) {
                in.seekg(0, std::ios::beg);
                in.read(&content->at(0), file_size);
            }
            in.close();
            if (in) return true;
        }
        catch (std::ios_base::failure &ex) {
            ITER_WARN_KV(MSG("Load failed."),
                KV("exception", ex.what()), KV(filename));
        }
    }
    ITER_WARN_KV(MSG("Load failed."), KV(filename));
    return false;
}

inline bool FileWriter::operator() (first_argument_type content, second_argument_type filename) {
   std::ofstream out(filename.c_str(), std::ios::out | std::ios::binary);
   if (out) {
        try {
            out.write(&content[0], content.size());
            out.close();
            if (out) return true;
        }
        catch (std::ios_base::failure &ex) {
            ITER_WARN_KV(MSG("Write failed."),
                KV("exception", ex.what()), KV(filename));
        }
    }
    ITER_WARN_KV(MSG("Write failed."), KV(filename));
    return false;
}

} // namespace iter

#endif // ITER_FILE_IO_HPP
