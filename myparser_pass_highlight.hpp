#ifndef MYPARSER_PASS_HIGHLIGHT_HPP
#define MYPARSER_PASS_HIGHLIGHT_HPP

#include "myparser_ast_plus.hpp"

namespace myparser {

template <>
class Pass<PASS_HIGHLIGHT>: public PassProto<PASS_HIGHLIGHT> {
protected:
    std::ostream &out;

    virtual void putText(const std::string &text) {
        out << text;
    }

    virtual void putKeyword(const std::string &text) {
        out << text << "test"; // TODO
    }

public:
    inline Pass(std::ostream &target):
        PassProto<PASS_HIGHLIGHT>(), out(target) {}

    // virtual ~Pass() {}

    // specialized nodes

    // TODO: NodeData<>, if string ... / if number ...

    template <class T>
    void run(const NodeTyped<BuiltinKeyword, T> *node) {
        putKeyword(node->Node::getFullText());
    }

    // common nodes

    template <size_t I>
    void run(const NodeListIndexed<I> *node) {
        for (const Node *child: node->getChildren()) {
            child->runPass(this);
        }
    }

    template <class TX = void> // actually not a template
    void run(const NodeTextPure<> *node) {
        putText(node->getText());
    }

    template <class E>
    void run(const NodeTextOrError<E> *node) {
        if (node->accepted()) {
            putText(node->getText());
        }
    }

    template <class E>
    void run(const NodeError<E> *node) {
        (void) node;
    }
};

template <class TX = void> // actually not a template
class PassHighlight: public Pass<PASS_HIGHLIGHT> { // console ver
protected:
    // virtual void putText(const std::string &text) {
    //     out << style_word << text << style_normal;
    // }

public:
    inline PassHighlight(std::ostream &target):
        Pass<PASS_HIGHLIGHT>(target) {}

    // virtual ~PassHighlight() {}
};

}

#endif
