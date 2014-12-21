#ifndef MYPARSER_AST_HPP
#define MYPARSER_AST_HPP

#include "myparser_str.hpp"
#include "myparser_pass.hpp"

namespace myparser {

using Input = std::string::const_iterator;

using BuiltinRoot = MP_STR("root", 4);
using BuiltinSpace = MP_STR("space", 5);
using BuiltinKeyword = MP_STR("keyword", 7);
using BuiltinError = MP_STR("error", 5);

template <class TX = void> // actually not a template
class Node {
private:
    inline Node() = delete;

    const Input pos;
    Input tail;

protected:
    inline Node(
        const Input &input
    ): pos(input), tail(input) {}

    inline void seek(const Input &end) {
        tail = end;
    }

public:
    virtual ~Node() {} // destructable (public)

    virtual void free() {
        delete this;
    }

    virtual bool empty() const = 0;

    virtual void runPass(PassBase<> *pass) const = 0;

    virtual const std::string &getRuleName() const = 0;

    inline const std::string getFullText() const {
        return std::string(pos, tail);
    }

    inline const Input &getPos() const {
        return pos;
    }

    inline const Input &getTail() const {
        return tail;
    }

    inline Node<> *challengeLonger(Node<> *target) {
        if (!target) {
            return this;
        }

        if (getTail() > target->getTail()) {
            target->free();
            return this;
        } else {
            this->free();
            return target;
        }
    }
};

template <class TX = void> // actually not a template
class NodeList: public Node<> {
private:
    std::vector<Node<> *> children;

    size_t basepos;
    NodeList<> *brother = nullptr;

protected:
    inline NodeList(
        const Input &input
    ): Node<>(input), children() {}

public:
    virtual ~NodeList() {
        for (size_t i = basepos; i < children.size(); ++i) {
            children[i]->free();
        }
    }

    virtual void free() {
        if (brother) {
            brother->basepos = 0;
            brother->brother = nullptr;
        }
        delete this;
    }

    virtual bool empty() const {
        for (Node<> *child: children) {
            if (!child->empty()) {
                return false;
            }
        }

        return true;
    }

    inline void bind(NodeList<> *target, const size_t pos) {
        brother = target;
        basepos = pos;
    }

    inline void putChild(Node<> *value) {
        children.push_back(value);
        seek(value->getTail());
    }

    inline const std::vector<Node<> *> &getChildren() const {
        return children;
    }
};

template <size_t I>
class NodeListIndexed: public NodeList<> {
public:
    inline NodeListIndexed(
        const Input &input
    ): NodeList<>(input) {}

    // virtual ~NodeListIndexed() {}

    inline size_t getIndex() const {
        return I;
    }
};

// TODO: remove .text and use getFullText() with cache
template <class TX = void> // actually not a template
class NodeText: public Node<> {
private:
    const std::string text;

protected:
    inline NodeText(
        const Input &input, std::string &&value
    ): Node<>(input), text(std::move(value)) {
        seek(getTail() + text.size());
    }

    inline NodeText(
        const Input &input, const std::string &value
    ): Node<>(input), text(value) {
        seek(getTail() + text.size());
    }

public:
    // virtual ~NodeText() {}

    virtual bool accepted() const {
        return true;
    }

    virtual bool empty() const {
        return accepted() && text.size() == 0;
    }

    inline const std::string &getText() const {
        return text;
    }
};

template <class TX = void> // actually not a template
class NodeTextPure: public NodeText<> {
public:
    inline NodeTextPure(
        const Input &input, std::string &&value
    ): NodeText<>(input, std::move(value)) {}

    // virtual ~NodeTextPure() {}
};

template <class E>
class NodeTextOrError: public NodeText<> {
public:
    inline NodeTextOrError(
        const Input &input, std::string &&value
    ): NodeText<>(input, std::move(value)) {}

    // virtual ~NodeTextOrError() {}
};

template <class E>
class NodeError: public Node<> {
public:
    inline NodeError(
        const Input &input
    ): Node<>(input) {}

    // virtual ~NodeError() {}

    virtual bool empty() const {
        return false;
    }
};

// could specialize
template <class N>
class NodeBaseList {
public:
    template <size_t I>
    using Type = NodeListIndexed<I>;
};

// could specialize
template <class N>
class NodeBaseText {
public:
    template <class TX = void> // actually not a template
    using Type = NodeTextPure<>;
};

// could specialize
template <class N>
class NodeBaseError {
public:
    template <class E>
    using Type = NodeError<E>;
};

template <class N, class T, size_t I = 0 /* bind later */>
class NodeTyped: public T {
public:
    using T::T;

    virtual void runPass(PassBase<> *pass) const {
        Pass<I>::call(pass, pass->getId(), this);
    }

    virtual const std::string &getRuleName() const {
        return N::getStr();
    }
};

template <class N, size_t I>
using NodeTypedList = NodeTyped<N, typename NodeBaseList<N>::template Type<I>>;

template <class N>
using NodeTypedText = NodeTyped<N, typename NodeBaseText<N>::template Type<>>;

template <class N, class E>
using NodeTypedError = NodeTyped<N, typename NodeBaseError<N>::template Type<E>>;

}

#endif
