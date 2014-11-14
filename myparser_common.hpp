#ifndef MYPARSER_COMMON_HPP
#define MYPARSER_COMMON_HPP

// library

#include <limits>
#include <string>
#include <vector>
#include <regex>

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
