#include "compiler.cpp"


/*
-I /davbjor-compiler/
INCLUDE PATH IN COMPILER WITH FLAG

EX:
g++ ./tests/functional_test1/fib-messy.cpp -o ./target/fib-messy.exe -I ./davbjor-compiler/
./target/fib-messy.exe
g++ ./tests/functional_test1/fib-messy-out.cpp -o ./target/fib-messy-out.exe
./target/fib-messy-out.exe

*/

int main(){
    std::string input = read_file("./tests/functional_test1/fib-messy.txt");
    std::string output = compile(input);

    std::cout << "INPUT CODE -> \n\n" << input << "\n";
    std::cout << "COMPILED INTO -> \n\n" << output << "\n";

    write_file("./tests/functional_test1/fib-messy-out.cpp", output);

    std::cout << "INTERPRET CODE -> \n\n";
    interpret(input);

    return 0;
}