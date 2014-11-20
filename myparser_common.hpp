#ifndef MYPARSER_COMMON_HPP
#define MYPARSER_COMMON_HPP

// option list

#if !defined(MYPARSER_CUSTOMIZED)
    // global

    #define MYPARSER_DEBUG

    // parser

    #define MYPARSER_AST_CCC
    // #define MYPARSER_ERROR_LINE
    #define MYPARSER_FORCE_INLINE

    // use boost instead of libstdc++
    #if defined(__GLIBCXX__)
        #define MYPARSER_BOOST_XPRESSIVE
    #else
        #define MYPARSER_STD_REGEX
    #endif
#endif

// library

#include <limits>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

#if defined(MYPARSER_BOOST_REGEX)
    #include <boost/regex.hpp>
    namespace regex_lib = boost;
#elif defined(MYPARSER_BOOST_XPRESSIVE)
    #include <boost/xpressive/xpressive_dynamic.hpp>
    namespace regex_lib = boost::xpressive;
#elif defined(MYPARSER_STD_REGEX)
    #include <regex>
    namespace regex_lib = std;
#else
    // Error!
#endif

#if defined(MYPARSER_AST_CCC)
    #include "lib/ccc.hpp"
#endif

#if defined(MYPARSER_FORCE_INLINE)
    #define MYPARSER_INLINE __attribute__((always_inline)) inline
#else
    #define MYPARSER_INLINE inline
#endif

namespace myparser {

#if defined(MYPARSER_DEBUG)
    template <class T>
    inline void mpDebug(T value) {
        std::cerr << value << std::endl;
    }
#else
    template <class T>
    inline void mpDebug(T value) {}
#endif

#if defined(MYPARSER_AST_CCC)
    const auto style_index = ccc::cf_magenta + ccc::s_bold;
    const auto style_keyword = ccc::cf_yellow + ccc::s_bold + ccc::s_underline_single;
    const auto style_error = ccc::cf_red + ccc::s_bold;
    // const auto style_faint = ccc::s_faint;
    const auto style_faint = "";
    const auto style_normal = ccc::d_all;
#else
    const auto style_index = "";
    const auto style_keyword = "";
    const auto style_error = "";
    const auto style_faint = "";
    const auto style_normal = "";
#endif

enum {
    PASS_REPR,
    PASS_MESSAGE, // input [(list, message), ...] output [lineno:pos message]

    PASS_MAX = 64
};

// forward declaration

template <size_t I>
class Pass;

}

#endif
