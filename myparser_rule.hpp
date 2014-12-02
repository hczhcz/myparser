#ifndef MYPARSER_RULE_HPP
#define MYPARSER_RULE_HPP

#include "myparser_str.hpp"
#include "myparser_ast_plus.hpp"

namespace myparser {

using BuiltinRoot = MP_STR("root", 4);
using BuiltinSpace = MP_STR("space", 5);
using BuiltinKeyword = MP_STR("keyword", 7);
using BuiltinError = MP_STR("error", 5);

using ErrorList = MP_STR("Nothing matched", 15);
using ErrorRegex = MP_STR("Regex not matched", 17);
using ErrorChecking = MP_STR("Match not accepted", 18);
using ErrorKeyword = MP_STR("Bad keyword", 11);

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
};

template <class N>
class RuleNamed: public Rule {
protected:
    inline RuleNamed(): Rule() {}

    // virtual ~RuleNamed() {}
};

// need specialization
template <class N>
class RuleDef: public RuleNamed<N> {
public:
    static std::pair<Node *, Node *> parse(
        Input &input, const Input &end
    );
};

template <>
class RuleDef<BuiltinError>: public RuleNamed<BuiltinError> {};

//////// Named ////////

template <class N, class... RL>
class RuleList: public RuleNamed<N> {
private:
    template <size_t I, class R, class... Rx>
    static MYPARSER_INLINE std::pair<Node *, Node *> runRule(
        Input &input, const Input &end
    ) {
        using Member =
            typename R
            ::template Helper<N, I>;

        Input input_rev = input;

        auto current = Member::parse(input, end);

        if (current.first) {
            return current;
        } else {
            input = input_rev;

            auto next = runRule<I + 1, Rx...>(input, end);

            if (!next.second) {
                return {next.first, current.second};
            } else {
                return {
                    next.first,
                    next.second->challengeLonger(current.second)
                };
            }
        }
    }

    template <size_t I> // iteration finished
    static MYPARSER_INLINE std::pair<Node *, Node *> runRule(
        Input &input, const Input &end
    ) {
        (void) end;

        return {
            nullptr,
            new NodeErrorTyped<N, ErrorList>(input)
        };
    }

protected:
    MYPARSER_INLINE RuleList(): RuleNamed<N>() {}

    // virtual ~RuleList() {}

public:
    static std::pair<Node *, Node *> parse(
        Input &input, const Input &end
    ) {
        return runRule<0, RL...>(input, end);
    }
};

template <class N, class RX>
class RuleRegex: public RuleNamed<N> {
public:
    using ResultType = NodeTextTyped<N>;

private:
    static MYPARSER_INLINE std::pair<Node *, Node *> runRegex(
        Input &input, const Input &end
    ) {
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
            std::string str = mdata.str();
            input += str.size();

            ResultType *result =
                new ResultType(input, std::move(str));

            if (result->accepted()) {
                return {result, nullptr};
            } else {
                delete result;

                return {
                    nullptr,
                    new NodeErrorTyped<N, ErrorChecking>(input)
                };
            }
        } else {
            return {
                nullptr,
                new NodeErrorTyped<N, ErrorRegex>(input)
            };
        }
    }

protected:
    MYPARSER_INLINE RuleRegex(): RuleNamed<N>() {}

    // virtual ~RuleRegex() {}

public:
    static std::pair<Node *, Node *> parse(
        Input &input, const Input &end
    ) {
        return runRegex(input, end);
    }
};

//////// Cell ////////

template <class N = BuiltinSpace, class TAG = TagNormal>
class RuleItemSpace: public TAG {
public:
    static MYPARSER_INLINE std::pair<Node *, Node *> parse(
        Input &input, const Input &end
    ) {
        return RuleDef<N>::parse(input, end);
    }
};

template <class KW, class N = BuiltinKeyword, class TAG = TagNormal>
class RuleItemKeyword: public TAG {
public:
    static MYPARSER_INLINE std::pair<Node *, Node *> parse(
        Input &input, const Input &end
    ) {
        auto current = RuleDef<N>::parse(input, end);

        if (
            current.first
            &&
            current.first->getFullText() == KW::getStr()
        ) {
            return current;
        } else {
            if (current.first) {
                current.first->free();
            }

            return {
                nullptr,
                (new NodeErrorTyped<BuiltinError, ErrorKeyword>(input))
                    ->challengeLonger(current.second)
            };
        }
    }
};

template <class N, class TAG = TagNormal>
class RuleItemRef: public TAG {
public:
    static MYPARSER_INLINE std::pair<Node *, Node *> parse(
        Input &input, const Input &end
    ) {
        return RuleDef<N>::parse(input, end);
    }
};

template <class E, class TAG = TagNormal>
class RuleItemError: public TAG {
public:
    static MYPARSER_INLINE std::pair<Node *, Node *> parse(
        Input &input, const Input &end
    ) {
        (void) end;

        return {
            nullptr,
            new NodeErrorTyped<BuiltinError, E>(input)
        };
    }
};

//////// Misc ////////

template <class... RL>
class RuleLine {
public:
    using ResultType = NodeListTyped<N, I>;

    template <class N, size_t I>
    class Helper {
    private:
        template <class R, class... Rx>
        static MYPARSER_INLINE bool runRule(
            ResultType *&result, Node *&err, size_t &errpos,
            Input &input, const Input &end
        ) {
            for (size_t i = 0; i < R::most; ++i) {
                auto current = R::parse(input, end);

                if (current.second) {
                    err = current.second->challengeLonger(err);

                    // if err updated
                    if (err == current.second) {
                        errpos = result->getChildren().size();
                    }
                }

                if (!current.first) {
                    if (i < R::least) {
                        return false;
                    } else {
                        break;
                    }
                } else {
                    result->putChild(current.first);
                }
            }

            return runRule<Rx...>(result, err, errpos, input, end);
        }

        template <std::nullptr_t P = nullptr> // iteration finished
        static MYPARSER_INLINE bool runRule(
            ResultType *&result, Node *&err, size_t &errpos,
            Input &input, const Input &end
        ) {
            (void) result;
            (void) err;
            (void) errpos;
            (void) input;
            (void) end;

            return true;
        }

    public:
        static MYPARSER_INLINE std::pair<Node *, Node *> parse(
            Input &input, const Input &end
        ) {
            ResultType *result = new ResultType(input);
            ResultType *result_err = new ResultType(input);

            Node *err = nullptr;
            size_t errpos = 0;

            bool succeed = runRule<RL...>(result, err, errpos, input, end);

            result->bind(result_err, errpos);
            result_err->bind(result, errpos);

            if (err) {
                for (size_t i = 0; i < errpos; ++i) {
                    result_err->putChild(result->getChildren()[i]);
                }
                result_err->putChild(err);
            } else {
                result_err->free();
                result_err = nullptr;
            }

            if (!succeed) {
                result->free();
                result = nullptr;
            }

            return {result, result_err};
        }
    };
};

template <class N = BuiltinRoot>
class Parser {
public:
    static inline Node *parse(Input &input, const Input &end) {
        auto current = RuleDef<N>::parse(input, end);

        if (current.first) {
            if (current.second) {
                current.second->free();
            }

            return current.first;
        } else {
            return current.second;
        }
    }

    static inline Node *parse(const std::string input) {
        Input iter = input.cbegin();

        return parse(iter, input.cend());
    }

    static inline Node *parse(std::string &&input) {
        Input iter = input.cbegin();

        return parse(iter, input.cend());
    }
};

}

#endif
