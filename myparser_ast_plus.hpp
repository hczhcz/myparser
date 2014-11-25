#ifndef MYPARSER_AST_PLUS_HPP
#define MYPARSER_AST_PLUS_HPP

#include "myparser_ast.hpp"

namespace myparser {

template <size_t I>
class NodeSpace: public NodeListIndexed<I> {
public:
    inline NodeSpace(const Input &input): NodeListIndexed(input) {}

    // virtual ~NodeSpace() {}

    virtual bool empty() const {
        return true;
    }
};

template <class T, class E>
class NodeData: public NodeTextOrError<E> {
private:
    const T *data;

public:
    inline NodeData(
        const Input &input, std::string &&value
    ): NodeTextOrError(input, value) {
        std::istringstream conv(value);
        data = new T;
        if (conv >> *data && conv.eof()) {
            // success
        } else {
            delete data;
            data = nullptr;
        }
    }

    virtual ~NodeData() {
        if (data) {
            delete data;
        }
    }

    virtual bool accepted() const {
        return data;
    }

    virtual bool empty() const {
        return !data || text.size() == 0;
    }

    inline const T &getData() {
        return *data;
    }
};

template <size_t I>
class NodeTyped<BuiltinSpace, NodeListIndexed<I>>:
public NodeTypedProto<BuiltinSpace, NodeSpace<I>> {
public:
    using NodeTypedProto<BuiltinSpace, NodeSpace<I>>::NodeTypedProto;
};

}

#endif
