#include "compiler.cpp"


int main() {
    std::string code = R"""(
    // FIBONACCI MESSY
    let a = (3 + 2) * (4 + 1) * (6 - 5 - 1);
    let b = (3 + 1) / 4;
    let i = 31 + 3 - 3 * 11 - 1;

    while(i < (13 + 4) + (4 * 4 - 5) * (2 - 1)){
        if(13 != 41){
            b += 1 - 1;
        };
        while(2 == 0){
            print(b);
        };
        print(1 - 1 + a);

        b = a + (((13 + 4) - (4 * 4 + 1)) + (1 - 1 + b) * (2 - 1));
        a = b * (3 * 4 - 11) - a + (6 * 5) * (1 - 1);

        i -= 300 - 30 * 10;
        i += 1;
    };
    
    )""";
    

    std::cout << "INPUT CODE -> \n\n" << code << "\n";
    std::cout << "COMPILED INTO -> \n\n" << compile(code) << "\n";

    std::cout << "INTERPRET CODE -> \n";
    interpret(code);

    return 0;
}