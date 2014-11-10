import os

from myparser_tool import char_maybe, char_any0, char_any1


def str_gen(value):
    result = repr(value)[1:-1]

    return 'MP_STR("' + result + '", ' + str(len(result)) + ')'


indent0 = os.linesep
indent1 = os.linesep + '    '
indent1c = ',' + indent1
indent2 = os.linesep + '        '
indent2c = ',' + indent2

cplusplus_dump = {
    'space': lambda: 'RuleItemSpace<>',
    'keyword': lambda x: 'RuleItemKeyword<' + str_gen(x) + '>',
    'ref': lambda x: 'RuleItemRef<' + str_gen(x) + '>',
    'ref' + char_maybe: lambda x: 'RuleItemRef<' + str_gen(x) + ', TagMaybe>',
    'ref' + char_any0: lambda x: 'RuleItemRef<' + str_gen(x) + ', TagAny0>',
    'ref' + char_any1: lambda x: 'RuleItemRef<' + str_gen(x) + ', TagAny1>',
    'error': lambda x: 'RuleItemError<' + str_gen(x) + '>',
    'line': lambda l: (
        'RuleLine<' + indent2 + indent2c.join(l) + indent1 + '>'
    ),

    'list': lambda n, l: (
        'template<>' + os.linesep
        + 'class RuleDef<' + str_gen(n) + '>:' + os.linesep
        + 'public RuleList<' + str_gen(n)
        + indent1c + indent1c.join(l) + indent0
        + '> {};' + os.linesep
    ),
    'regex': lambda n, x: (
        'template<>' + os.linesep
        + 'class RuleDef<' + str_gen(n) + '>:' + os.linesep
        + 'public RuleRegex<' + str_gen(n)
        + indent1c + str_gen(x) + indent0
        + '> {};' + os.linesep
    )
}
