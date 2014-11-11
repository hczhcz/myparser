#ifndef MYPARSER_RULE_HPP
#define MYPARSER_RULE_HPP

#include <limits>
#include <string>
#include <regex>

#include "myparser_str.hpp"
#include "myparser_ast.hpp"

namespace myparser {

using BuiltinRoot = MP_STR("root", 4);
using BuiltinSpace = MP_STR("space", 5);
using BuiltinKeyword = MP_STR("keyword", 7);

template <size_t L, size_t M>
class Tag {
public:
    static const size_t least = L;
    static const size_t most = M;
};
using TagNormal = Tag<1, 1>;
using TagMaybe = Tag<0, 1>;
using TagAny0 = Tag<0, std::numeric_limits<size_t>::max()>;
using TagAny1 = Tag<1, std::numeric_limits<size_t>::max()>;

class Rule {
protected:
    inline Rule() {} // force singleton

    // virtual ~Rule() {}

public:
    virtual const std::string &getName() const = 0;
};

template <class N>
class RuleNamed: public Rule {
public:
    virtual const std::string &getName() const {
        static const std::string name = N::getStr();

        return name;
    }
};

//////// Named ////////

template <class N, class... RL>
class RuleList: public RuleNamed<N> {
public:
    using SelfType = RuleList<N, RL...>;

    static const SelfType instance;

private:
    template <class R, class... Rx>
    static inline const Node *runRule(
        Input &input, const Input &end
    ) {
        Input input_new = input;

        using Member =
            typename R
            ::template Helper<SelfType>;

        Node *current = Member::parse(input_new, end);

        if (current) {
            input = input_new;

            return current;
        } else {
            return runRule<Rx...>(input, end);
        }
    }

    template <std::nullptr_t P = nullptr> // iteration finished
    static inline const Node *runRule(
        Input &input, const Input &end
    ) {
        return new NodeErrorNativeTyped<SelfType, MP_STR("Nothing matched", 15)>();
    }

public:
    static const Node *parse(Input &input, const Input &end) {
        return runRule<RL...>(input, end);
    }
};

template <class N, class... RL>
using RuleBuiltin = RuleList<N, RL...>;

template <class N, class RX>
class RuleRegex: public RuleNamed<N> {
public:
    using SelfType = RuleRegex<N, RX>;

    static const SelfType instance;

private:
    static inline const Node *runRegex(
        Input &input, const Input &end
    ) {
        static const std::regex regex(
            RX::getStr(),
            std::regex_constants::extended
        );

        std::match_results<Input> mdata;

        if (
            regex_search(
                input, end, mdata, regex,
                std::regex_constants::match_continuous
            )
        ) {
            auto str = mdata.str();
            input += str.size();

            return new NodeTextTyped<SelfType>(input, str);
        } else {
            return new NodeErrorNativeTyped<SelfType, MP_STR("Regex not matched", 17)>();
        }
    }

public:
    static const Node *parse(Input &input, const Input &end) {
        return runRegex(input, end);
    }
};

//////// Cell ////////

template <template <class N> class RD, class TAG = TagNormal>
class RuleItemSpace: public TAG {
public:
    static const Node *parse(Input &input, const Input &end) {
        return RD<BuiltinSpace>::parse(input, end);
    }
};

template <template <class N> class RD, class KW, class TAG = TagNormal>
class RuleItemKeyword: public TAG {
public:
    static const Node *parse(Input &input, const Input &end) {
        static const std::string keyword = KW::getStr();

        const Node *result = RD<BuiltinKeyword>::parse(input, end);

        if (result->getFullText() == keyword) {
            return result;
        } else {
            return new NodeErrorWrap<>(input, result);
        }
    }
};

template <template <class N> class RD, class N, class TAG = TagNormal>
class RuleItemRef: public TAG {
public:
    static const Node *parse(Input &input, const Input &end) {
        return RD<N>::parse(input, end);
    }
};

template <class E, class TAG = TagNormal>
class RuleItemError: public TAG {
public:
    static const Node *parse(Input &input, const Input &end) {
        static const std::string error = E::getStr();

        return new NodeErrorNative<E>(input);
    }
};

template <class... RL>
class RuleLine {
public:
    template <class LST>
    class Helper {
    private:
        template <class R, class... Rx>
        static inline bool runRule(
            NodeListTyped<LST> *&result, Input &input, const Input &end
        ) {
            for (size_t i = 0; i < R::most; ++i) {
                Node *current = R::parse(input, end);

                if (current) {
                    result->putChild(current);
                } else {
                    if (i < R::least) {
                        return false;
                    } else {
                        break;
                    }
                }
            }

            return runRule<Rx...>(result, input, end);
        }

        template <std::nullptr_t P = nullptr> // iteration finished
        static inline bool runRule(
            NodeListTyped<LST> *&result, Input &input, const Input &end
        ) {
            return true;
        }

    public:
        static const Node *parse(Input &input, const Input &end) {
            NodeListTyped<LST> *result = new NodeListTyped<LST>(input);

            if (runRule<RL...>(result, input, end)) {
                return result;
            } else {
                return new NodeErrorWrapTyped<LST>(result);
            }
        }
    };
};

}

#endif
