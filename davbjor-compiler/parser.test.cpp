//#include "ast.cpp"
#include "parser.cpp"
//#include "lex.cpp"
#include <iostream>

#include <vector>
#include <string>

int main() {
    std::string code = "1 + 5 * 3 - 2 / 4 - 1";
    std::cout << code << "\n";

    auto tokens = lex(code);
    
    Parser parser = Parser(&tokens, 0);

    Node* expr = parse_e(&parser);

    std::cout << "eval:  " << (*expr).eval() << "\n";
    std::cout << "print: " << (*expr).print() << "\n";

    /*
    std::string code2 = "4 * 3 + 1";
    std::cout << "\n\n" << code2 << "\n";

    auto tokens2 = lex(code2);
    
    Parser parser2 = Parser(&tokens2, 0);

    Node* expr2 = parse_e(&parser2);

    std::cout << "eval:  " << (*expr2).eval() << "\n";
    std::cout << "print: " <<(*expr2).print() << "\n";
    */

    return 0;
}