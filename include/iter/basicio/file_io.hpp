#ifndef ITER_FILE_IO_HPP
#define ITER_FILE_IO_HPP

#include <string>

namespace iter {

class FileReader {
public:
    typedef bool result_type;
    typedef const std::string& first_argument_type;
    typedef std::string& second_argument_type;
    bool operator () (first_argument_type filename, second_argument_type content);
};

class FileWriter {
public:
    typedef bool result_type;
    typedef const std::string& first_argument_type;
    typedef const std::string& second_argument_type;
    bool operator () (first_argument_type content, second_argument_type filename);
};

} // namespace iter

#include <iter/basicio/detail/file_io_impl.hpp>

#endif // ITER_FILE_IO_HPP
