#ifndef MYPARSER_RULE_HPP
#define MYPARSER_RULE_HPP

#include "myparser_common.hpp"
#include "myparser_str.hpp"
#include "myparser_ast.hpp"

namespace myparser {

using BuiltinRoot = MP_STR("root", 4);
using BuiltinSpace = MP_STR("space", 5);
using BuiltinKeyword = MP_STR("keyword", 7);

using ErrorList = MP_STR("Nothing matched", 15);
using ErrorRegex = MP_STR("Regex not matched", 17);
using ErrorKeyword = MP_STR("Bad keyword", 11);
using ErrorLine = MP_STR("Longest bad match", 17);

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
protected:
    inline RuleNamed(): Rule() {}

    // virtual ~RuleNamed() {}

public:
    virtual const std::string &getName() const {
        static const std::string name = N::getStr();

        return name;
    }
};

//////// Named ////////

template <class N, class... RL>
class RuleList: public RuleNamed<N> {
protected:
    inline RuleList(): RuleNamed<N>() {}

    // virtual ~RuleList() {}

public:
    using SelfType = RuleList<N, RL...>;

    static inline const SelfType *getInstance() {
        static const SelfType instance;

        return &instance;
    }

private:
    template <class R, class... Rx>
    static inline const Node *runRule(
        Input &input, const Input &end
    ) {
        Input input_new = input;

        using Member =
            typename R
            ::template Helper<SelfType>;

        const Node *current = Member::parse(input_new, end);

        if (current) {
            input = input_new;

            return current;
        } else {
            const Node *later = runRule<Rx...>(input, end);

            // select the best one
            if (later || later->getPos() > current->getPos()) {
                delete current;

                return later;
            } else {
                delete later;

                return current;
            }
        }
    }

    template <std::nullptr_t P = nullptr> // iteration finished
    static inline const Node *runRule(
        Input &input, const Input &end
    ) {
        return new NodeErrorNativeTyped<SelfType, ErrorList>(input);
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
protected:
    inline RuleRegex(): RuleNamed<N>() {}

    // virtual ~RuleRegex() {}

public:
    using SelfType = RuleRegex<N, RX>;

    static inline const SelfType *getInstance() {
        static const SelfType instance;

        return &instance;
    }

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
            return new NodeErrorNativeTyped<SelfType, ErrorRegex>(input);
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

        if (result && result->getFullText() == keyword) {
            return result;
        } else {
            return new NodeErrorWrap<ErrorKeyword>(input, result);
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
                const Node *current = R::parse(input, end);

                result->putChild(current);
                if (!current) {
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
                return new NodeErrorWrapTyped<LST, ErrorLine>(input, result);
            }
        }
    };
};

}

#endif
