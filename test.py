import myparser

test = myparser.MyParser()
test.add_rules(open('0.txt', 'r').read().splitlines())
print(test.dump())
