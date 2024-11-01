# davbjor-compiler
This program defines a brand new programming language: <strong> DBLang </strong>.

This program is able to <strong> interpret </strong> DBLang code, and also <strong> transpile to C++ </strong>. 

Under the hood it first does a tokenization using a lexer, and then parses the tokens into an AST-tree. Then a semantic analysys checks for variable handling. The AST is then optimized with methods including <strong> constant folding </strong> and <strong> dead code removal </strong>. Finally the AST is either transpiled to C++, or real-time interpreted.


## BNF definition av DBLang
```xml
<program> ::= <block> | <block> <nl> <program>
<block> ::= <ws> <block> 
    | <while> 
    | <if> 
    | <declaration> 
    | <assignment> 
    | <output>

<while> ::= "while" <ws> "(" <ws> <condition> <ws> ")" <ws> "{" <ws> <nl> <program> <nl> <ws> "}" <ws> ";"
<if> ::= "if" <ws> "(" <ws> <condition> <ws> ")" <ws> "{" <nl> <program> <nl> <ws> "}" <ws> ";"

<declaration> ::= "let " <ws> <identifier> <ws> "=" <ws> <statement> <ws> ";"
<assignment> ::= <identifier> <ws> <assignor> <ws> <statement> <ws> ";"

<output> ::= "print(" <ws> <statement> <ws> ")" <ws> ";" 

<statement> ::= <condition> | <expression>
<condition> ::= <ws> <expression> <ws> <comparator> <ws> <expression>
<expression> ::= <value> 
    | <expression> <ws> <operator> <ws> <expression> 
    | "(" <ws> <expression> <ws> ")"

<value> ::= <number> | <identifier>

<identifier> ::= <letter> | <identifier> <letter>
<number> ::= <digit> | <digit> <number> | <unary> <digit> | <unary> <digit> <number>
<ws> ::= " " <ws> | ""
<nl> ::= "\n" <nl> | ""
<comparator> ::= "<" | ">" | "!=" | "==" | "<=" | ">="
<assignor> ::= "=" | "+=" | "-=" | "*=" | "/="
<operator> ::= "+" | "-" | "*" | "/"
<unary> ::= "-"
<digit> ::= "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9"
<letter> ::= "A" | "B" | "C" | "D" | "E" | "F" | "G" | "H" | "I" | "J" | "K" | "L" | "M" | "N" | "O" | "P" | "Q" | "R" | "S" | "T" | "U" | "V" | "W" | "X" | "Y" | "Z" | "a" | "b" | "c" | "d" | "e" | "f" | "g" | "h" | "i" | "j" | "k" | "l" | "m" | "n" | "o" | "p" | "q" | "r" | "s" | "t" | "u" | "v" | "w" | "x" | "y" | "z" 
```


[https://bnfplayground.pauliankline.com...](https://bnfplayground.pauliankline.com/?bnf=%3Cprogram%3E%20%3A%3A%3D%20%3Cblock%3E%20%7C%20%3Cblock%3E%20%3Cnl%3E%20%3Cprogram%3E%0A%3Cblock%3E%20%3A%3A%3D%20%3Cws%3E%20%3Cblock%3E%20%0A%20%20%20%20%7C%20%3Cwhile%3E%20%0A%20%20%20%20%7C%20%3Cif%3E%20%0A%20%20%20%20%7C%20%3Cdeclaration%3E%20%0A%20%20%20%20%7C%20%3Cassignment%3E%20%0A%20%20%20%20%7C%20%3Coutput%3E%0A%0A%3Cwhile%3E%20%3A%3A%3D%20%22while%22%20%3Cws%3E%20%22(%22%20%3Cws%3E%20%3Ccondition%3E%20%3Cws%3E%20%22)%22%20%3Cws%3E%20%22%7B%22%20%3Cnl%3E%20%3Cprogram%3E%20%3Cnl%3E%20%3Cws%3E%20%22%7D%22%20%3Cws%3E%20%22%3B%22%0A%3Cif%3E%20%3A%3A%3D%20%22if%22%20%3Cws%3E%20%22(%22%20%3Cws%3E%20%3Ccondition%3E%20%3Cws%3E%20%22)%22%20%3Cws%3E%20%22%7B%22%20%3Cnl%3E%20%3Cprogram%3E%20%3Cnl%3E%20%3Cws%3E%20%22%7D%22%20%3Cws%3E%20%22%3B%22%0A%0A%3Cdeclaration%3E%20%3A%3A%3D%20%22let%20%22%20%3Cws%3E%20%3Cidentifier%3E%20%3Cws%3E%20%22%3D%22%20%3Cws%3E%20%3Cstatement%3E%20%3Cws%3E%20%22%3B%22%0A%3Cassignment%3E%20%3A%3A%3D%20%3Cidentifier%3E%20%3Cws%3E%20%3Cassignor%3E%20%3Cws%3E%20%3Cstatement%3E%20%3Cws%3E%20%22%3B%22%0A%0A%3Coutput%3E%20%3A%3A%3D%20%22print(%22%20%3Cws%3E%20%3Cstatement%3E%20%3Cws%3E%20%22)%22%20%3Cws%3E%20%22%3B%22%20%0A%0A%3Cstatement%3E%20%3A%3A%3D%20%3Ccondition%3E%20%7C%20%3Cexpression%3E%0A%3Ccondition%3E%20%3A%3A%3D%20%3Cws%3E%20%3Cexpression%3E%20%3Cws%3E%20%3Ccomparator%3E%20%3Cws%3E%20%3Cexpression%3E%0A%3Cexpression%3E%20%3A%3A%3D%20%3Cvalue%3E%20%0A%20%20%20%20%7C%20%3Cexpression%3E%20%3Cws%3E%20%3Coperator%3E%20%3Cws%3E%20%3Cexpression%3E%20%0A%20%20%20%20%7C%20%22(%22%20%3Cws%3E%20%3Cexpression%3E%20%3Cws%3E%20%22)%22%0A%0A%3Cvalue%3E%20%3A%3A%3D%20%3Cnumber%3E%20%7C%20%3Cidentifier%3E%0A%0A%3Cidentifier%3E%20%3A%3A%3D%20%3Cletter%3E%20%7C%20%3Cidentifier%3E%20%3Cletter%3E%0A%3Cnumber%3E%20%3A%3A%3D%20%3Cdigit%3E%20%7C%20%3Cdigit%3E%20%3Cnumber%3E%20%7C%20%3Cunary%3E%20%3Cdigit%3E%20%7C%20%3Cunary%3E%20%3Cdigit%3E%20%3Cnumber%3E%0A%3Cws%3E%20%3A%3A%3D%20%22%20%22%20%3Cws%3E%20%7C%20%22%20%22%0A%3Cnl%3E%20%3A%3A%3D%20%22%5Cn%22%20%3Cnl%3E%20%7C%20%22%5Cn%22%0A%3Ccomparator%3E%20%3A%3A%3D%20%22%3C%22%20%7C%20%22%3E%22%20%7C%20%22!%3D%22%20%7C%20%22%3D%3D%22%20%7C%20%22%3C%3D%22%20%7C%20%22%3E%3D%22%0A%3Cassignor%3E%20%3A%3A%3D%20%22%3D%22%20%7C%20%22%2B%3D%22%20%7C%20%22-%3D%22%20%7C%20%22*%3D%22%20%7C%20%22%2F%3D%22%0A%3Coperator%3E%20%3A%3A%3D%20%22%2B%22%20%7C%20%22-%22%20%7C%20%22*%22%20%7C%20%22%2F%22%0A%3Cunary%3E%20%3A%3A%3D%20%22-%22%0A%3Cdigit%3E%20%3A%3A%3D%20%220%22%20%7C%20%221%22%20%7C%20%222%22%20%7C%20%223%22%20%7C%20%224%22%20%7C%20%225%22%20%7C%20%226%22%20%7C%20%227%22%20%7C%20%228%22%20%7C%20%229%22%0A%3Cletter%3E%20%3A%3A%3D%20%22A%22%20%7C%20%22B%22%20%7C%20%22C%22%20%7C%20%22D%22%20%7C%20%22E%22%20%7C%20%22F%22%20%7C%20%22G%22%20%7C%20%22H%22%20%7C%20%22I%22%20%7C%20%22J%22%20%7C%20%22K%22%20%7C%20%22L%22%20%7C%20%22M%22%20%7C%20%22N%22%20%7C%20%22O%22%20%7C%20%22P%22%20%7C%20%22Q%22%20%7C%20%22R%22%20%7C%20%22S%22%20%7C%20%22T%22%20%7C%20%22U%22%20%7C%20%22V%22%20%7C%20%22W%22%20%7C%20%22X%22%20%7C%20%22Y%22%20%7C%20%22Z%22%20%7C%20%22a%22%20%7C%20%22b%22%20%7C%20%22c%22%20%7C%20%22d%22%20%7C%20%22e%22%20%7C%20%22f%22%20%7C%20%22g%22%20%7C%20%22h%22%20%7C%20%22i%22%20%7C%20%22j%22%20%7C%20%22k%22%20%7C%20%22l%22%20%7C%20%22m%22%20%7C%20%22n%22%20%7C%20%22o%22%20%7C%20%22p%22%20%7C%20%22q%22%20%7C%20%22r%22%20%7C%20%22s%22%20%7C%20%22t%22%20%7C%20%22u%22%20%7C%20%22v%22%20%7C%20%22w%22%20%7C%20%22x%22%20%7C%20%22y%22%20%7C%20%22z%22%20%0A&name=dblang)



## Examples

### DBLang Code To Print The First 28 Fibbonacci Numbers
This messy code gets optimized into clean C++.
Let's see the optimizations:
```js
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
```

### Optimized And Transpiled To C++ Code
```cpp
#include <iostream>

int main(){
    int a = 0;
    int b = 1;
    int i = 0;
    while(i < 28){
        std::cout << (a) << "\n";
        b = a + b;
        a = b - a;
        i = i + 1;
    }
}
```
