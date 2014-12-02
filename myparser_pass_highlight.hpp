#ifndef MYPARSER_PASS_HIGHLIGHT_HPP
#define MYPARSER_PASS_HIGHLIGHT_HPP

#include "myparser_ast_plus.hpp"

namespace myparser {

template <>
class Pass<PASS_HIGHLIGHT>: public PassProto<PASS_HIGHLIGHT> {
protected:
    std::ostream &out;

    virtual void putLnEnd(const std::string &text) {}

    virtual void putText(const std::string &text) {}

public:
    inline Pass(std::ostream &target):
        PassProto<PASS_HIGHLIGHT>(), out(target) {}

    // virtual ~Pass() {}

    // TODO ...

    void run(const NodeList<> *node) {
        for (const Node *child: node->getChildren()) {
            child->runPass(this);
        }
    }

    void run(const NodeText<> *node) {
        putText(node->getText());
    }

    template <class E>
    void run(const NodeTextOrError<E> *node) {
        if (node->accepted()) {
            putText(node->getText());
        }
    }

    template <class E>
    void run(const NodeError<E> *node) {}
};

}

#endif
