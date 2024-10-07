#include "lex.cpp"

#include <iostream>
#include <vector>

int main() {
    std::string code = "let a = 13 + 4;\nif(a == 17){\n   let b = 21 - 7;\n   a = b * 2 - 1;\n}";


    std::vector<Token> tokens = lex(code);

    std::cout << code << "\n";
    print_tokens(tokens);

    return 0;
}