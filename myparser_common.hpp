#ifndef MYPARSER_COMMON_HPP
#define MYPARSER_COMMON_HPP

// option list

#ifndef MYPARSER_CUSTOMIZED
    #define MYPARSER_DEBUG
    #define MYPARSER_AST_CCC

    // use boost instead of libstdc++
    #ifdef __GLIBCXX__
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

// forward declaration

template <class TX = void>
class NodeList;

template <class TX = void>
class NodeText;

template <class TX = void>
class NodeError;

template <class E>
class NodeErrorNative;

template <class E>
class NodeErrorWrap;

class Rule;

}

#endif
