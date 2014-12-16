#ifndef MYPARSER_PASS_HPP
#define MYPARSER_PASS_HPP

#include "myparser_common.hpp"

namespace myparser {

template <class TX = void> // actually not a template
class PassBase {
private:
    size_t id;

protected:
    inline PassBase(const size_t pass): id(pass) {}

    // virtual ~PassBase() {}

public:
    inline size_t getId() const {
        return id;
    }
};

template <size_t I>
class PassProto: public PassBase<> {
public:
    inline PassProto(): PassBase<>(I) {}

    // virtual ~PassProto() {}

    template <class T>
    static MYPARSER_INLINE void call(
        PassBase<> *pass, const size_t target, const T *node
    ) {
        if (target == I) {
            ((Pass<I> *) pass)->run(node);
        } else {
            Pass<I + 1>::call(pass, target, node);
        }
    }
};

template <size_t I = 0>
class Pass: public Pass<I + 1> {};

template <>
class Pass<PASS_FIN> {
public:
    template <class T>
    static MYPARSER_INLINE void call(
        PassBase<> *pass, const size_t target, const T *node
    ) const {
        // never reach
        (void) pass;
        (void) target;
        (void) node;
    }
};

}

#endif
