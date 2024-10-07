#include "lex.cpp"

#include <iostream>
#include <vector>
#include <fstream>
#include <streambuf>

int main() {
    std::string code = R"""(
    let a = 13 + 4;
    if(a == 17){
        let b = 21 - 7;
        a = b * 2 - 1;
    }
    )""";

    std::vector<Token> tokens = lex(code);

    std::cout << code << "\n";
    print_tokens(tokens);

    return 0;
}