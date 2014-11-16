#ifndef MYPARSER_AST_HPP
#define MYPARSER_AST_HPP

#include "myparser_common.hpp"

namespace myparser {

using Input = std::string::const_iterator;

class Node {
private:
    const Input pos;

    inline Node() = delete;

public:
    inline Node(const Input &input): pos(input) {}

    virtual ~Node() {}

    virtual bool accepted() const = 0;

    virtual bool empty() const = 0;

    virtual const Rule *getRule() const = 0;

    virtual size_t getLen() const = 0;

    virtual void getFullText(std::ostream &out) const = 0;

    inline const std::string getFullText() const {
        std::ostringstream result;

        getFullText(result);

        return result.str();
    }

    virtual void getTree(
        std::ostream &out, size_t indent = 0, size_t simplify = 1
    ) const = 0;

    inline const std::string getTree(size_t indent = 0, size_t simplify = 1) const {
        std::ostringstream result;

        getTree(result, indent, simplify);

        return result.str();
    }

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

    virtual bool accepted() const {
        #if defined(MYPARSER_ERROR_LINE)
            return true;
        #else
            for (const Node *child: children) {
                if (!child->accepted()) {
                    return false;
                }
            }

            return true;
        #endif
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
            out << child->getFullText();
        }
    }

    virtual size_t getIndex() const = 0;

    virtual void getTree(
        std::ostream &out, size_t indent = 0, size_t simplify = 1
    ) const {
        std::vector<const Node *> children1;

        if (simplify < 1) {
            // nothing
        } else {
            for (const Node *child: children) {
                if (!child->empty()) {
                    children1.push_back(child);
                }
            }
        }

        const std::vector<const Node *> children2 =
            (simplify < 1) ? children : children1;

        if (simplify < 2) {
            out << getRule()->getName() << '[';
            out << style_index << getIndex() << style_normal;
            out << ']';
        } else {
            out << '*';
        }

        if (children2.size() == 1) {
            out << style_faint << " - " << style_normal;
            children2[0]->getTree(out, indent, simplify);
        } else {
            for (const Node *child: children2) {
                out << '\n';

                for (size_t i = 0; i < indent + 1; ++i) {
                    out << "    ";
                }
                child->getTree(out, indent + 1, simplify);
            }
        }
    }

    inline const std::vector<const Node *> &getChildren() const {
        return children;
    }
};

template <size_t I>
class NodeListIndexed: public NodeList<> {
public:
    using NodeList<>::NodeList;

    virtual size_t getIndex() const {
        return I;
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

    virtual bool accepted() const {
        return true;
    }

    virtual bool empty() const {
        return text.size() == 0;
    }

    virtual size_t getLen() const {
        return text.size();
    }

    virtual void getFullText(std::ostream &out) const {
        out << text;
    }

    virtual void getTree(
        std::ostream &out, size_t indent = 0, size_t simplify = 1
    ) const {
        (void) indent;

        if (simplify < 2) {
            out << getRule()->getName();
        } else {
            out << '*';
        }

        out << style_faint << " - " << style_normal;
        out << style_keyword << text << style_normal;
    }

    inline const std::string &getText() const {
        return text;
    }

    template <class T>
    inline const T &getValue() const = delete; // reserved
};

template <class TX> // actually not a template
class NodeError: public Node {
public:
    inline NodeError(const Input &input):
        Node(input) {}

    virtual ~NodeError() {}

    virtual bool accepted() const {
        return false;
    }

    virtual bool empty() const {
        return false;
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

    virtual size_t getLen() const {
        return 0;
    }

    virtual void getFullText(std::ostream &out) const {
        // nothing
        (void) out;
    }

    virtual void getTree(
        std::ostream &out, size_t indent = 0, size_t simplify = 1
    ) const {
        (void) indent;

        static const std::string error = E::getStr();

        if (simplify < 2) {
            out << getRule()->getName();
        } else {
            out << '*';
        }

        out << style_faint << " - " << style_normal;
        out << style_error << "ERROR: " << style_normal;
        out << error;
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

    virtual size_t getLen() const {
        return child->getLen();
    }

    virtual void getFullText(std::ostream &out) const {
        out << child->getFullText();
    }

    virtual void getTree(
        std::ostream &out, size_t indent = 0, size_t simplify = 1
    ) const {
        static const std::string error = E::getStr();

        if (simplify < 2) {
            out << getRule()->getName();
        } else {
            out << '*';
        }

        out << style_faint << " - " << style_normal;
        out << style_error << "ERROR: " << style_normal;
        out << error << '\n';

        for (size_t i = 0; i < indent + 1; ++i) {
            out << "    ";
        }
        child->getTree(out, indent + 1, simplify);
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

    virtual const Rule *getRule() const {
        return NT::getInstance();
    }
};

template <class NT, size_t I>
using NodeListTyped = NodeTyped<NT, NodeListIndexed<I>>;

template <class NT>
using NodeTextTyped = NodeTyped<NT, NodeText<>>;

template <class NT, class E>
using NodeErrorNativeTyped = NodeTyped<NT, NodeErrorNative<E>>;

template <class NT, class E>
using NodeErrorWrapTyped = NodeTyped<NT, NodeErrorWrap<E>>;

}

#endif
