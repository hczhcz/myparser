import myparser
from myparser_cpp import cplusplus_dump, cplusplus_gen

test = myparser.MyParser()
test.add_rules(open('0.txt', 'r').read().splitlines())
# print(test.dump())
# c = test.compile()
# c(open('test.ml', 'r').read())


test1 = myparser.MyParser()
test1.add_rules(r'''
**space**:

    <ignored>

**keyword**:

    <id>

*ignored*:

    ([ \t\n]|\/\/.*$)*

id:

    <id1>
    <id2>

*bang*:

    !

*num*:

    [0-9]+

*id1*:

    [a-z_][a-z0-9_]*

*id2*:

    @@@@

**root**:

    <id> <root>
    <id>
    <bang><+bang> <root>
    <num> <!error>

'''.splitlines())

print(test1.dump())
c1 = test1.compile()
print(test1.match('!!aaa   bbb@@@@').get_tree())
# print(test1.match('1 xxx').get_tree())

'''
print('========')
print(cplusplus_gen(
    'mylang',
    test1.xdump(cplusplus_dump),
    'parser/',
    'MYLANG_SYNTAX_HPP'
))
print('========')
'''
