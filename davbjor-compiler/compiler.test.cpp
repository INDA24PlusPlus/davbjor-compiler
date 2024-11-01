#include "compiler.cpp"

#include <iostream>
#include <vector>
#include <string>

int main() {
    std::string code = "let a = 1; let b = 1; a * ((-(1) + ((5 * (6 - 2)) / 4)) - -(1)) == b * (16 / 4) + 3 / (1 + 2)";
    std::cout << "in\n" << code << "\n";

    auto tokens = lex(code);
    Node* expr = parse(&tokens);
    expr = constant_folding(expr);
    expr = dead_code(expr);

    std::cout << "\nout: \n" << (*expr).print() << "\n";

    
    //std::string code1 = "((-(1) + ((5 * (6 - 2)) / 4)) - -(1))";
    std::string code1 = "let c = 1; let d = 1; c * ((-(1) + ((5 * (6 - 2)) / 4)) - -(1)) > d + (16 / 8) + 3 / (1 + 2)";
    std::cout << "\n\nin: \n" << code1 << "\n";

    auto tokens1 = lex(code1);
    
    Node* expr1 = parse(&tokens1);
    expr1 = constant_folding(expr1);
    expr1 = dead_code(expr1);

    std::cout << "\nout: " <<(*expr1).print() << "\n";
    


    //std::string code2 = "(16 / 4) + 3 / (1 + 2)";
    std::string code2 = "let a = 4 + 3; print(a + 1); a /= 7; print(a + 1)";
    std::cout << "\n\nin \n" << code2 << "\n";

    auto tokens2 = lex(code2);

    Node* expr2 = parse(&tokens2);
    expr2 = constant_folding(expr2);
    expr2 = dead_code(expr2);

    std::cout << "\nout: \n" <<(*expr2).print() << "\n";
    

    return 0;
}