#ifndef MYPARSER_PASS_REPR_HPP
#define MYPARSER_PASS_REPR_HPP

#include "myparser_pass.hpp"

namespace myparser {

template <>
class Pass<PASS_REPR>: public PassProto<PASS_REPR> {
protected:
    std::ostream &out;
    const bool allowEmpty;
    size_t indent;

    virtual void putName(const std::string name) = 0;
    virtual void putIndex(const size_t index) = 0;
    virtual void putText(const std::string text) = 0;
    virtual void putError(const std::string error) = 0;
    virtual void putBegin() = 0;
    virtual void putEnd() = 0;
    virtual void putLnBegin() = 0;
    virtual void putLnEnd() = 0;
    virtual void putLn() = 0;

public:
    inline Pass(std::ostream &target, const bool verbose):
        PassProto<PASS_REPR>(), out(target), allowEmpty(verbose), indent(0) {}

    // virtual ~Pass() {}

    void run(const NodeList<> *node) {
        putName(node->getRuleName());
        putIndex(node->getIndex());

        std::vector<const Node *> children1;

        if (!allowEmpty) {
            for (const Node *child: node->getChildren()) {
                if (!child->empty()) {
                    children1.push_back(child);
                }
            }
        }

        const std::vector<const Node *> &children =
            allowEmpty ? node->getChildren() : children1;

        if (children.size() == 1) {
            putBegin();
            children[0]->runPass(this);
            putEnd();
        } else {
            putLnBegin();
            for (const Node *child: children) {
                ++indent;
                putLn();
                child->runPass(this);
                --indent;
            }
            putLnEnd();
        }
    }

    void run(const NodeText<> *node) {
        putName(node->getRuleName());

        putBegin();
        putText(node->getText());
        putEnd();
    }

    template <class E>
    void run(const NodeErrorNative<E> *node) {
        putName(node->getRuleName());

        putBegin();
        putError(E::getStr());
        putEnd();
    }

    template <class E>
    void run(const NodeErrorWrap<E> *node) {
        putName(node->getRuleName());

        putBegin();
        putError(E::getStr());
        putEnd();

        ++indent;
        putLn();
        node->getChild()->runPass(this);
        --indent;
    }
};

template <class TX = void> // actually not a template
class PassReprText: public Pass<PASS_REPR> {
protected:
    virtual void putName(const std::string name) {
        (void) name;
    }

    virtual void putIndex(const size_t index) {
        (void) index;
    }

    virtual void putText(const std::string text) {
        out << text;
    }

    virtual void putError(const std::string error) {
        (void) error;
    }

    virtual void putBegin() {}

    virtual void putEnd() {}

    virtual void putLnBegin() {}

    virtual void putLnEnd() {}

    virtual void putLn() {}

    virtual void putError() {}

public:
    inline PassReprText(std::ostream &target):
        Pass<PASS_REPR>(target, true) {}

    // virtual ~PassReprText() {}
};

template <class TX = void> // actually not a template
class PassReprSimple: public Pass<PASS_REPR> {
protected:
    virtual void putName(const std::string name) {
        (void) name;
    }

    virtual void putIndex(const size_t index) {
        (void) index;
    }

    virtual void putText(const std::string text) {
        out << style_keyword << text << style_normal;
    }

    virtual void putError(const std::string error) {
        out << style_error << "ERROR: " << style_normal << error;
    }

    virtual void putBegin() {}

    virtual void putEnd() {}

    virtual void putLnBegin() {
        out << "=>";
    }

    virtual void putLnEnd() {}

    virtual void putLn() {
        out << '\n';
        for (size_t i = 0; i < indent; ++i) {
            out << "    ";
        }
    }

public:
    inline PassReprSimple(std::ostream &target, const bool verbose = false):
        Pass<PASS_REPR>(target, verbose) {}

    // virtual ~PassReprSimple() {}
};

template <class TX = void> // actually not a template
class PassReprFull: public Pass<PASS_REPR> {
protected:
    virtual void putName(const std::string name) {
        out << name;
    }

    virtual void putText(const std::string text) {
        out << style_keyword << text << style_normal;
    }

    virtual void putError(const std::string error) {
        out << style_error << "ERROR: " << style_normal << error;
    }

    virtual void putIndex(const size_t index) {
        out << '[';
        out << style_index << index << style_normal;
        out << ']';
    }

    virtual void putBegin() {
        out << style_faint << " - " << style_normal;
    }

    virtual void putEnd() {}

    virtual void putLnBegin() {}

    virtual void putLnEnd() {}

    virtual void putLn() {
        out << '\n';
        for (size_t i = 0; i < indent; ++i) {
            out << "    ";
        }
    }

public:
    inline PassReprFull(std::ostream &target, const bool verbose = false):
        Pass<PASS_REPR>(target, verbose) {}

    // virtual ~PassReprFull() {}
};

}

#endif
