#ifndef MYPARSER_PASS_HPP
#define MYPARSER_PASS_HPP

#include "myparser_common.hpp"

namespace myparser {

class PassBase {
private:
    size_t id;

public:
    inline PassBase(const size_t pass): id(pass) {}

    // virtual ~PassBase() {}

    inline size_t getId() {
        return id;
    }
};

template <size_t I>
class PassProto: public PassBase {
public:
    inline PassProto(): PassBase(I) {}

    // virtual ~PassProto() {}

    // TODO: add run() with template, hold bad calls

    template <class T>
    static MYPARSER_INLINE void call(
        PassBase *pass, const size_t offset, const T *node
    ) {
        if (offset == 0) {
            ((Pass<I> *) pass)->run(node);
        } else {
            Pass<I + 1>::call(pass, offset - 1, node);
        }
    }
};

template <size_t I = 0>
class Pass {
public:
    template <class T>
    static MYPARSER_INLINE void call(
        PassBase *pass, const size_t offset, const T *node
    ) {
        Pass<I + 1>::call(pass, offset - 1, node);
    }
};

template <>
class Pass<PASS_MAX> {
public:
    template <class T>
    static MYPARSER_INLINE void call(
        PassBase *pass, const size_t offset, const T *node
    ) {
        // never reach
        (void) pass;
        (void) offset;
        (void) node;
    }
};

}

#endif
