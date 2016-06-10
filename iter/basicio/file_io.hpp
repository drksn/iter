#ifndef ITER_FILE_IO_HPP
#define ITER_FILE_IO_HPP

#include <string>

namespace iter {

class FileReader {
public:
    typedef std::string FirstArg;
    typedef std::string SecondArg;
    bool operator () (const FirstArg& filename, SecondArg& content);
};

class FileWriter {
public:
    typedef std::string FirstArg;
    typedef std::string SecondArg;
    bool operator () (const FirstArg& content, const SecondArg& filename);
};

} // namespace iter

#include <iter/basicio/detail/file_io_impl.hpp>

#endif // ITER_FILE_IO_HPP
