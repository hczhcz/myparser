import myparser
import myparser_cpp

parser = myparser.MyParser()
parser.add_file('README.md')

with open('self_syntax.hpp', 'w') as syntax_hpp:
    syntax_hpp.write(
        myparser_cpp.cplusplus_gen(
            parser.xdump(myparser_cpp.cplusplus_dump),
            '',
            'SELF_SYNTAX_HPP'
        )
    )
