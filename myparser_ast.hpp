#ifndef MYPARSER_AST_HPP
#define MYPARSER_AST_HPP

#include "myparser_pass.hpp"

namespace myparser {

using Input = std::string::const_iterator;

class Node {
private:
    const Input pos;

    inline Node() = delete;

protected:
    inline Node(const Input &input): pos(input) {}

public:
    virtual ~Node() {} // destructable (public)

    virtual bool empty() const = 0;

    virtual void runPass(PassBase *pass) const = 0;

    virtual const std::string &getRuleName() const = 0;

    virtual size_t getLen() const = 0;

    virtual void getFullText(std::ostream &out) const = 0;

    inline const std::string getFullText() const {
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

    inline Node *challengeLonger(Node *target) {
        if (!target) {
            return this;
        }

        if (getTail() > target->getTail()) {
            delete target;
            return this;
        } else {
            delete this;
            return target;
        }
    }
};

template <class TX = void> // actually not a template
class NodeList: public Node {
private:
    std::vector<const Node *> children;

protected:
    inline NodeList(const Input &input):
        Node(input), children() {}

public:
    virtual ~NodeList() {
        for (const Node *child: children) {
            delete child;
        }
    }

    virtual bool empty() const {
        for (const Node *child: children) {
            if (!child->empty()) {
                return false;
            }
        }

        return true;
    }

    inline void putChild(const Node *value) {
        children.push_back(value);
    }

    virtual size_t getLen() const {
        size_t result = 0;

        for (const Node *child: children) {
            result += child->getLen();
        }

        return result;
    }

    virtual void getFullText(std::ostream &out) const {
        for (const Node *child: children) {
            child->getFullText(out);
        }
    }

    virtual size_t getIndex() const = 0;

    inline const std::vector<const Node *> &getChildren() const {
        return children;
    }
};

template <size_t I>
class NodeListIndexed: public NodeList<> {
public:
    inline NodeListIndexed(const Input &input):
        NodeList(input) {}

    // virtual ~NodeListIndexed() {}

    virtual size_t getIndex() const {
        return I;
    }
};

template <class TX = void> // actually not a template
class NodeText: public Node {
private:
    const std::string text;

public:
    inline NodeText(
        const Input &input, std::string &&value
    ): Node(input), text(std::move(value)) {}

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

template <class E>
class NodeTextOrError: public NodeText<> {
public:
    inline NodeTextOrError(
        const Input &input, std::string &&value
    ): NodeText(input, std::move(value)) {}

};

template <class E>
class NodeError: public Node {
public:
    inline NodeError(const Input &input):
        Node(input) {}

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

template <class N, class T, size_t I = 0>
class NodeTypedProto: public T {
public:
    using T::T;

    virtual void runPass(PassBase *pass) const {
        Pass<I>::call(pass, pass->getId(), this);
    }

    virtual const std::string &getRuleName() const {
        return N::getStr();
    }
};

// could specialization
template <class N, class T>
class NodeTyped: public NodeTypedProto<N, T> {
public:
    using NodeTypedProto<N, T>::NodeTypedProto;
};

template <class NT, size_t I>
using NodeListTyped = NodeTyped<NT, NodeListIndexed<I>>;

template <class NT>
using NodeTextTyped = NodeTyped<NT, NodeText<>>;

template <class NT, class E>
using NodeErrorTyped = NodeTyped<NT, NodeError<E>>;

}

#endif
