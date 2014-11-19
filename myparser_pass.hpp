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

template <size_t I>
class Pass: public PassProto {
public:
    MYPARSER_INLINE Pass(): PassProto(I) {}

    template <class T>
    MYPARSER_INLINE void run(const T *node) {
        // T::need_specialization();
        // never reach
    }
};

template <size_t I = 0>
class PassMgr {
public:
    template <class T>
    static MYPARSER_INLINE void run(
        PassProto *pass, const size_t offset, const T *node
    ) {
        if (offset == 0) {
            ((Pass<I> *) pass)->run(node);
        } else {
            PassMgr<I + 1>::run(pass, offset - 1, node);
        }
    }

    template <class T>
    static MYPARSER_INLINE void run(PassProto *pass, const T *node) {
        run(pass, pass->getId(), node);
    }
};

template <>
class PassMgr<max_pass> {
public:
    template <class T>
    static MYPARSER_INLINE void run(
        PassProto *pass, const size_t offset, const T *node
    ) {
        // never reach
    }
};

}

#endif
