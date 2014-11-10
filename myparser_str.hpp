#ifndef MP_UTIL_H
#define MP_UTIL_H

#include <string>

#define MP_STR_0(str) /* none */
#define MP_STR_1(str) str[0]
#include "myparser_str_chain.inc"
#define MP_STR(str, len) StaticStr<MP_STR_##len(str)>

namespace myparser {

template <char... C>
class StaticStr {
public:
    static inline const std::string &getStr() {
        static const std::string stdstr = {C...};

        return stdstr;
    }
};

}

#endif
