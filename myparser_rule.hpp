#ifndef MYPARSER_RULE_HPP
#define MYPARSER_RULE_HPP

#include "myparser_common.hpp"
#include "myparser_str.hpp"
#include "myparser_ast.hpp"

namespace myparser {

using BuiltinRoot = MP_STR("root", 4);
using BuiltinSpace = MP_STR("space", 5);
using BuiltinKeyword = MP_STR("keyword", 7);
using BuiltinError = MP_STR("error", 5);

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
    using SelfType = RuleNamed<N>;

    static inline const SelfType *getInstance() {
        static const SelfType instance;

        return &instance;
    }

    virtual const std::string &getName() const {
        static const std::string name = N::getStr();

        return name;
    }
};

//////// Named ////////

template <template <class N> class RD, class N, class... RL>
class RuleList: public RuleNamed<N> {
private:
    template <size_t I, class R, class... Rx>
    static inline const Node *runRule(
        Input &input, const Input &end
    ) {
        using Member =
            typename R
            ::template Helper<RD<N>, I>;

        Input input_new = input;

        const Node *current = Member::parse(input_new, end);

        if (current->accepted()) {
            input = input_new;

            return current;
        } else {
            const Node *later = runRule<I + 1, Rx...>(input, end);

            // select the best one
            if (later->accepted() || later->getPos() > current->getPos()) {
                delete current;

                return later;
            } else {
                delete later;

                return current;
            }
        }
    }

    template <size_t I, std::nullptr_t P = nullptr> // iteration finished
    static inline const Node *runRule(
        Input &input, const Input &end
    ) {
        (void) end;

        return new NodeErrorNativeTyped<RD<N>, ErrorList>(input);
    }

protected:
    inline RuleList(): RuleNamed<N>() {}

    // virtual ~RuleList() {}

public:
    using SelfType = RuleList<RD, N, RL...>;

    static inline const SelfType *getInstance() {
        static const SelfType instance;

        return &instance;
    }

    static const Node *parse(Input &input, const Input &end) {
        // mpDebug(N::getStr());
        // mpDebug(*input);

        return runRule<0, RL...>(input, end);
    }
};

template <template <class N> class RD, class N, class... RL>
using RuleBuiltin = RuleList<RD, N, RL...>;

template <template <class N> class RD, class N, class RX>
class RuleRegex: public RuleNamed<N> {
private:
    static inline const Node *runRegex(
        Input &input, const Input &end
    ) {
        // mpDebug(N::getStr());
        // mpDebug(*input);

        #if defined(MYPARSER_BOOST_XPRESSIVE)
            static const regex_lib::basic_regex<Input> re =
                regex_lib::basic_regex<Input>::compile<Input>(
                    RX::getStr().cbegin(),
                    RX::getStr().cend()
                );
        #else
            static const regex_lib::regex re(
                RX::getStr()
            );
        #endif

        regex_lib::match_results<Input> mdata;

        if (
            regex_lib::regex_search(
                input, end, mdata, re,
                regex_lib::regex_constants::match_continuous
            )
        ) {
            auto str = mdata.str();
            input += str.size();

            return new NodeTextTyped<RD<N>>(input, str);
        } else {
            return new NodeErrorNativeTyped<RD<N>, ErrorRegex>(input);
        }
    }

protected:
    inline RuleRegex(): RuleNamed<N>() {}

    // virtual ~RuleRegex() {}

public:
    using SelfType = RuleRegex<RD, N, RX>;

    static inline const SelfType *getInstance() {
        static const SelfType instance;

        return &instance;
    }

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

        if (result->accepted() && result->getFullText() == keyword) {
            return result;
        } else {
            return new NodeErrorWrapTyped<RuleNamed<BuiltinError>, ErrorKeyword>(input, result);
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

template <template <class N> class RD, class E, class TAG = TagNormal>
class RuleItemError: public TAG {
public:
    static const Node *parse(Input &input, const Input &end) {
        (void) end;

        static const std::string error = E::getStr();

        return new NodeErrorNativeTyped<RuleNamed<BuiltinError>, E>(input);
    }
};

template <class... RL>
class RuleLine {
public:
    template <class LST, size_t I>
    class Helper {
    private:
        template <class R, class... Rx>
        static inline bool runRule(
            NodeListTyped<LST, I> *&result, Input &input, const Input &end
        ) {
            for (size_t i = 0; i < R::most; ++i) {
                const Node *current = R::parse(input, end);

                result->putChild(current);
                if (!current->accepted()) {
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
            NodeListTyped<LST, I> *&result, Input &input, const Input &end
        ) {
            (void) result;
            (void) input;
            (void) end;

            return true;
        }

    public:
        static const Node *parse(Input &input, const Input &end) {
            NodeListTyped<LST, I> *result = new NodeListTyped<LST, I>(input);

            if (runRule<RL...>(result, input, end)) {
                return result;
            } else {
                #if defined(MYPARSER_ERROR_LINE)
                    return new NodeErrorWrapTyped<LST, ErrorLine>(input, result);
                #else
                    return result;
                #endif
            }
        }
    };
};

}

#endif
