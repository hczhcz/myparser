#ifndef MYPARSER_AST_PLUS_HPP
#define MYPARSER_AST_PLUS_HPP

#include "myparser_ast.hpp"

namespace myparser {

template <size_t I>
class NodeSpace: public NodeListIndexed<I> {
public:
    inline NodeSpace(const Input &input):
        NodeListIndexed<I>(input) {}

    // virtual ~NodeSpace() {}

    virtual bool empty() const {
        return true;
    }
};

template <class T, class E>
class NodeData: public NodeTextOrError<E> {
private:
    const T data;
    const bool succeed;

public:
    inline NodeData(
        const Input &input, std::string &&value
    ): NodeTextOrError<E>(input, std::move(value)) {
        std::istringstream conv(value);
        succeed = conv >> data && conv.eof();
    }

    virtual ~NodeData() {}

    virtual bool accepted() const {
        return succeed;
    }

    inline const T &getData() {
        return data;
    }
};

template <class T, class E>
class NodeDataPtr: public NodeTextOrError<E> {
private:
    const T *data;

public:
    inline NodeDataPtr(
        const Input &input, std::string &&value
    ): NodeTextOrError<E>(input, std::move(value)) {
        std::istringstream conv(value);
        data = new T;
        if (conv >> *data && conv.eof()) {
            // success
        } else {
            delete data;
            data = nullptr;
        }
    }

    virtual ~NodeDataPtr() {
        if (data) {
            delete data;
        }
    }

    virtual bool accepted() const {
        return data;
    }

    inline const T &getData() {
        return *data;
    }
};

}

#endif
