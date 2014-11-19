#ifndef MYPARSER_PASS_HPP
#define MYPARSER_PASS_HPP

#include "myparser_common.hpp"

namespace myparser {

class PassProto {
private:
    const size_t id;

public:
    inline PassProto(const size_t pass): id(pass) {}

    // virtual ~PassProto() {}

    inline size_t getId() {
        return id;
    }
};

template <class T, size_t I>
class Pass: PassProto {
public:
    MYPARSER_INLINE Pass(): PassProto(I) {}

    MYPARSER_INLINE void run(const T *node) {
        //T::need_specialization();
    }
};

template <class T, size_t I>
class PassChain {
public:
    static MYPARSER_INLINE void run(
        PassProto *pass, const size_t offset, const T *node
    ) {
        if (offset == 0) {
            ((Pass<T, I> *) pass)->run(node);
        } else {
            PassChain<T, I + 1>::run(pass, offset - 1, node);
        }
    }
};

template <class T>
class PassChain<T, max_pass> {
public:
    static MYPARSER_INLINE void run(
        PassProto *pass, const size_t offset, const T *node
    ) {
        // never reach
    }
};

template <class T>
class PassMgr {
public:
    static MYPARSER_INLINE void run(PassProto *pass, const T *node) {
        PassChain<T, 0>::run(pass, pass->getId(), node);
    }
};

}

#endif
