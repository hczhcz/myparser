#ifndef MYPARSER_PASS_FILTER_HPP
#define MYPARSER_PASS_FILTER_HPP

#include "myparser_ast_plus.hpp"

namespace myparser {

template <>
class Pass<PASS_FILTER>: public PassProto<PASS_FILTER> {
public:
    inline Pass() {}

    // virtual ~Pass() {}

    template <class N, class T>
    void run(const NodeTyped<N, T> *node) {
        // TODO
        (void) node;
    }
};

}

#endif
