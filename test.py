import myparser

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

*id*:

    [a-z_][a-z0-9_]*

**root**:

    <id><id>
    <id> <id>

'''.splitlines())

print(test1.dump())
c1 = test1.compile()
print(c1('aaa   bbb', 0).get_full())
