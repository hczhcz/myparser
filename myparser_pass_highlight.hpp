#ifndef MYPARSER_PASS_HIGHLIGHT_HPP
#define MYPARSER_PASS_HIGHLIGHT_HPP

#include "myparser_pass.hpp"

namespace myparser {

/*
template <>
class Pass<PASS_HIGHLIGHT>: public PassProto<PASS_HIGHLIGHT> {
public:
    inline Pass(std::ostream &target):
        PassProto<PASS_HIGHLIGHT>(), out(target) {}

    // virtual ~Pass() {}

    void run(const NodeList<> *node) {
        putMainBegin();

        putName(node->getRuleName());
        putIndex(node->getIndex());

        std::vector<const Node *> children1;

        if (!optionV) {
            for (const Node *child: node->getChildren()) {
                if (!child->empty()) {
                    children1.push_back(child);
                }
            }
        }

        const std::vector<const Node *> &children =
            optionV ? node->getChildren() : children1;

        if (optionC && children.size() == 1) {
            putBegin();
            children[0]->runPass(this);
            putEnd();
        } else {
            putPlaceHolder();

            putLnBegin();
            ++indent;

            bool first = true;
            for (const Node *child: children) {
                putLn(first);
                first = false;
                child->runPass(this);
            }

            --indent;
            putLnEnd();
        }

        putMainEnd();
    }

    void run(const NodeText<> *node) {
        putMainBegin();

        putName(node->getRuleName());

        putBegin();
        putText(node->getText());
        putEnd();

        putMainEnd();
    }

    template <class E>
    void run(const NodeTextOrError<E> *node) {
        putMainBegin();

        putName(node->getRuleName());

        putBegin();
        if (node->accepted()) {
            putText(node->getText());
        } else {
            putError(E::getStr());
        }
        putEnd();

        putMainEnd();
    }

    template <class E>
    void run(const NodeError<E> *node) {}
};*/

}

#endif
