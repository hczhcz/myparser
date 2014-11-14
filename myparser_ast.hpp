#ifndef MYPARSER_AST_HPP
#define MYPARSER_AST_HPP

#include <vector>
#include <string>

namespace myparser {

// forward declaration

class Rule;

template <class TX = void>
class NodeList;

template <class TX = void>
class NodeText;

template <class TX = void>
class NodeError;

template <class E>
class NodeErrorNative;

template <class E>
class NodeErrorWrap;

// forward declaration finished

using RulePtr = const Rule *;
using Input = std::string::iterator;

class Node {
private:
    const Input pos;

    inline Node() = delete;

public:
    inline Node(const Input &input): pos(input) {}

    virtual ~Node() {}

    virtual operator bool() const = 0;

    virtual RulePtr getRule() const = 0;

    virtual const std::string getFullText() const = 0;

    virtual const std::string getTree(size_t indent = 0) const = 0;

    inline const Input &getPos() const {
        return pos;
    }

    inline const NodeList<> *castList() const;

    inline const NodeText<> *castText() const;

    inline const NodeError<> *castError() const;
};

template <class TX> // actually not a template
class NodeList: public Node {
private:
    std::vector<const Node *> children;

public:
    inline NodeList(const Input &input):
        Node(input), children() {}

    virtual ~NodeList() {
        for (const Node *child: children) {
            delete child;
        }
    }

    virtual operator bool() const {
        return true;
    }

    inline void putChild(const Node *value) {
        children.push_back(value);
    }

    virtual const std::string getFullText() const {
        std::string result = "";

        for (const Node *child: children) {
            result += child->getFullText();
        }

        return result;
    }

    virtual const std::string getTree(size_t indent = 0) const {
        std::string result = getRule()->getName();

        for (const Node *child: children) {
            result += '\n';
            for (size_t i = 0; i < indent + 1; ++i) {
                result += "    ";
            }
            result += child->getTree(indent + 1);
        }

        return result;
    }

    inline const std::vector<const Node *> &getChildren() const {
        return children;
    }
};

template <class TX> // actually not a template
class NodeText: public Node {
private:
    const std::string text;

public:
    inline NodeText(const Input &input, const std::string &value):
        Node(input), text(value) {}

    virtual ~NodeText() {}

    virtual operator bool() const {
        return true;
    }

    virtual const std::string getFullText() const {
        return text;
    }

    virtual const std::string getTree(size_t indent = 0) const {
        return getRule()->getName() + " - " + text;
    }

    inline const std::string &getText() const {
        return text;
    }

    template <class T>
    inline const T &getValue() const = delete;
};

template <class TX> // actually not a template
class NodeError: public Node {
public:
    inline NodeError(const Input &input):
        Node(input) {}

    virtual ~NodeError() {}

    virtual operator bool() const {
        return true;
    }

    virtual RulePtr getRule() const {
        return nullptr; // TODO: really?
    }

    // TODO: virtual function: getErrorMessage()
};

template <class E>
class NodeErrorNative: public NodeError<> {
public:
    using ErrorType = E;

    inline NodeErrorNative(const Input &input):
        NodeError(input) {}

    virtual ~NodeErrorNative() {}

    virtual const std::string getFullText() const {
        return "";
    }

    virtual const std::string getTree(size_t indent = 0) const {
        static const std::string error = E::getStr();

        return getRule()->getName() + " - ERROR: " + error;
    }
};

template <class E>
class NodeErrorWrap: public NodeError<> {
private:
    const Node *child;

public:
    using ErrorType = E;

    inline NodeErrorWrap(const Input &input, const Node *value):
        NodeError<>(input), child(value) {}

    virtual ~NodeErrorWrap() {
        delete child;
    }

    virtual const std::string getFullText() const {
        return child->getFullText();
    }

    virtual const std::string getTree(size_t indent = 0) const {
        static const std::string error = E::getStr();

        std::string result = getRule()->getName() + " - ERROR: " + error;

        result += '\n';
        for (size_t i = 0; i < indent + 1; ++i) {
            result += "    ";
        }
        result += child->getTree(indent + 1);

        return result;
    }

    inline const Node *getChild() const {
        return child;
    }
};

inline const NodeList<> *Node::castList() const {
    return dynamic_cast<const NodeList<> *>(this);
}

inline const NodeText<> *Node::castText() const {
    return dynamic_cast<const NodeText<> *>(this);
}

inline const NodeError<> *Node::castError() const {
    return dynamic_cast<const NodeError<> *>(this);
}

template <class NT, class T>
class NodeTyped: public T {
public:
    using T::T;

    using RuleType = NT;

    virtual RulePtr getRule() const {
        return NT::getInstance();
    }
};

template <class NT>
using NodeListTyped = NodeTyped<NT, NodeList<>>;

template <class NT>
using NodeTextTyped = NodeTyped<NT, NodeText<>>;

template <class NT, class E>
using NodeErrorNativeTyped = NodeTyped<NT, NodeErrorNative<E>>;

template <class NT, class E>
using NodeErrorWrapTyped = NodeTyped<NT, NodeErrorWrap<E>>;

}

#endif
