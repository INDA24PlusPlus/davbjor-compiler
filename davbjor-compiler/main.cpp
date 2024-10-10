#include "parser.cpp"

#include <string>


int main() {
    
    std::string code = R"""(
    let a = 13 + 4;
    print(a);

    print(1);
    if(a == 17){
        print(a);
        a += 1;
        print(a)
    };
    print(2);

    if(a == 18){
        print(a);
        a += 1;
        print(a)
    };

    let b = 21 - 7;
    print(b);

    a = b * 2 - 1;
    print(a)
    )""";

    auto tokens = lex(code);
    
    Node* expr = parse(&tokens);


    return 0;
}