#ifndef ITER_FILE_IO_IMPL_HPP
#define ITER_FILE_IO_IMPL_HPP

#include <iter/log.hpp>
#include <fstream>

namespace iter {

bool FileReader::operator () (
        const FirstArg& filename, SecondArg& content) {
    std::ifstream in(filename.c_str(), std::ios::in | std::ios::binary);
    if (in) {
        try {
            in.seekg(0, std::ios::end);
            content.resize(in.tellg());
            in.seekg(0, std::ios::beg);
            in.read(&content[0], content.size());
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

bool FileWriter::operator() (
        const FirstArg& content, const SecondArg& filename) {
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

#endif // ITER_FILE_IO_IMPL_HPP
