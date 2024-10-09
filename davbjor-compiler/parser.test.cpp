#include "parser.cpp"

#include <iostream>
#include <vector>
#include <string>

int main() {
    std::string code = "((-(1) + ((5 * (6 - 2)) / 4)) - -(1)) == (16 / 4) + 3 / (1 + 2)";
    std::cout << code << "\n";

    auto tokens = lex(code);
    
    Node* expr = parse(&tokens);

    std::cout << "eval:  " << (*expr).eval() << "\n";
    std::cout << "print: " << (*expr).print() << "\n";

    

    std::string code1 = "((-(1) + ((5 * (6 - 2)) / 4)) - -(1)) > (16 / 8) + 3 / (1 + 2)";
    std::cout << "\n\n" << code1 << "\n";

    auto tokens1 = lex(code1);
    
    Node* expr1 = parse(&tokens1);

    std::cout << "eval:  " << (*expr1).eval() << "\n";
    std::cout << "print: " <<(*expr1).print() << "\n";
    /*
    std::string code1 = "((-(1) + ((5 * (6 - 2)) / 4)) - -(1))";
    std::cout << "\n\n" << code1 << "\n";

    auto tokens1 = lex(code1);
    
    Parser parser1 = Parser(&tokens1, 0);

    Node* expr1 = parse_e(&parser1);

    std::cout << "eval:  " << (*expr1).eval() << "\n";
    std::cout << "print: " <<(*expr1).print() << "\n";



    std::string code2 = "(16 / 4) + 3 / (1 + 2)";
    std::cout << "\n\n" << code2 << "\n";

    auto tokens2 = lex(code2);
    
    Parser parser2 = Parser(&tokens2, 0);

    Node* expr2 = parse_e(&parser2);

    std::cout << "eval:  " << (*expr2).eval() << "\n";
    std::cout << "print: " <<(*expr2).print() << "\n";*/
    

    return 0;
}