#ifndef ITER_SPLIT_HPP
#define ITER_SPLIT_HPP

#include <vector>
#include <string>

namespace iter {

inline std::vector <std::string> Split(
        const std::string& str, const std::string& sep) {
    std::vector <std::string> ret;
    // If no seperator, return the whole line.
    if (sep.size() == 0){
        ret.push_back(str);
        return ret;
    }
    std::string tmp;
    for (size_t i = 0; i < str.size(); i++){
        while (i + sep.size() <= str.size()){
            bool match = true;
            for (size_t j = 0; j < sep.size(); j++){
                if (str[i + j] != sep[j]){
                    match = false;
                    break;
                }
            }
            if (!match) break;
            ret.push_back(tmp);
            tmp = "";
            i += sep.size();
        }
        if (i < str.size()) tmp += str[i];
    }
    ret.push_back(tmp);
    return ret;
}

} // namespace iter

#endif // ITER_SPLIT_HPP
