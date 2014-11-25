#ifndef MYPARSER_AST_PLUS_HPP
#define MYPARSER_AST_PLUS_HPP

#include "myparser_ast.hpp"

namespace myparser {

template <size_t I>
class NodeSpace: public NodeListIndexed<I> {
public:
    using NodeListIndexed<I>::NodeListIndexed;

    virtual bool empty() const {
        return true;
    }
};

template <size_t I>
class NodeTyped<BuiltinSpace, NodeListIndexed<I>>:
public NodeTypedProto<BuiltinSpace, NodeSpace<I>> {
public:
    using NodeTypedProto<BuiltinSpace, NodeSpace<I>>::NodeTypedProto;
};

}

#endif
