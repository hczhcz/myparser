#ifndef MYPARSER_COMMON_HPP
#define MYPARSER_COMMON_HPP

// option list

#ifndef MYPARSER_CUSTOMIZED
    #define MYPARSER_DEBUG

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

#if defined(MYPARSER_DEBUG)
    template <class T>
    inline void mpDebug(T value) {
        std::cerr << value << std::endl;
    }
#else
    template <class T>
    inline void mpDebug(T value) {}
#endif

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

#if defined(MYPARSER_CCC)
    #include "lib/ccc.hpp"
#endif

namespace myparser {

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
