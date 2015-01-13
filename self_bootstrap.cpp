#if 0
python self_syntax.py &&
clang++ -std=c++11 -g -ferror-limit=1 self_bootstrap.cpp -o self_bootstrap.out &&
./self_bootstrap.out

exit
#endif

#include <fstream>
#include <streambuf>
#include "self_syntax.hpp"

int main() {
    using namespace myparser;

    std::ifstream fs("README.md");
    std::string input(
        (std::istreambuf_iterator<char>(fs)),
        std::istreambuf_iterator<char>()
    );

    PassReprFull<> repr(std::cout);
    PassHighlight<> highlight(std::cout);

    auto parsed = Parser<>::parse(input, false);

    parsed->runPass(&repr);
    std::cout << std::endl;
    parsed->runPass(&highlight);

    return 0;
}
