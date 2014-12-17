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
    const Input pos;

    inline Node() = delete;

protected:
    inline Node(
        const Input &input
    ): pos(input) {}

public:
    virtual ~Node() {} // destructable (public)

    virtual void free() {
        delete this;
    }

    virtual bool empty() const = 0;

    virtual void runPass(PassBase<> *pass) const = 0;

    virtual const std::string &getRuleName() const = 0;

    virtual size_t getLen() const = 0;

    virtual void getFullText(std::ostream &out) const = 0;

    inline const std::string asFullText() const {
        std::ostringstream result;

        getFullText(result);

        return result.str();
    }

    inline const Input &getPos() const {
        return pos;
    }

    inline const Input getTail() const {
        return pos + getLen();
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
    }

    virtual size_t getLen() const {
        size_t result = 0;

        for (Node<> *child: children) {
            result += child->getLen();
        }

        return result;
    }

    virtual void getFullText(std::ostream &out) const {
        for (Node<> *child: children) {
            child->getFullText(out);
        }
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

template <class TX = void> // actually not a template
class NodeText: public Node<> {
private:
    const std::string text;

protected:
    inline NodeText(
        const Input &input, std::string &&value
    ): Node<>(input), text(std::move(value)) {}

    inline NodeText(
        const Input &input, const std::string &value
    ): Node<>(input), text(value) {}

public:
    // virtual ~NodeText() {}

    virtual bool accepted() const {
        return true;
    }

    virtual bool empty() const {
        return accepted() && text.size() == 0;
    }

    virtual size_t getLen() const {
        return text.size();
    }

    virtual void getFullText(std::ostream &out) const {
        out << text;
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

    virtual size_t getLen() const {
        return 0;
    }

    virtual void getFullText(std::ostream &out) const {
        // nothing
        (void) out;
    }
};

// could specialize
template <class N, size_t I>
class NodeBaseList {
public:
    using Type = NodeListIndexed<I>;
};

// could specialize
template <class N>
class NodeBaseText {
public:
    using Type = NodeTextPure<>;
};

// could specialize
template <class N, class E>
class NodeBaseError {
public:
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
using NodeTypedList = NodeTyped<N, typename NodeBaseList<N, I>::Type>;

template <class N>
using NodeTypedText = NodeTyped<N, typename NodeBaseText<N>::Type>;

template <class N, class E>
using NodeTypedError = NodeTyped<N, typename NodeBaseError<N, E>::Type>;

}

#endif
