#include "ast.cpp"
#include <iostream>

int main() {
    std::cout << "Type four numbers\n";
    std::cout << "a, b, c, d\n";
    std::cout << "Calculate a + (b = d) - c \n";
    int a,b,c,d;
    std::cin >> a >> b >> c >> d;

    Node *minus = new Minus(
        new Add(
            new Number(&a), 
            new Assign(
                new Number(&b), 
                new Number(&d)
            )
        ), 
        new Number(&c)
    );

    std::cout << (*minus).eval() << "\n";
    std::cout << (*minus).print() << "\n";


    
    std::cout << "\n\nType four numbers\n";
    std::cout << "a, b, c, d\n";
    std::cout << "Calculate if (a == b) { c } else { d } \n";
    int x = 2;
    int y = 1;
    int z = 4;
    int k = 3;
    Node *statement = new If(
        new Eq(
            new Number(&x), 
            new Number(&y)
        ),
        new Number(&z), 
        new Number(&k)
    );
    
    std::cout << (*statement).eval() << "\n";
    std::cout << (*statement).print() << "\n";

    return 0;
}