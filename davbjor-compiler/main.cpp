/*
#include "parser.cpp"
*/

#include <string>
#include <vector>
#include <map>
#include <set>

#include "lex.cpp"
#include "parser.cpp"

std::map<std::string, int> table;


int interpret(Node* node){
    TokenKind kind = node->kind;
    Node* left = node->left;
    Node* right = node->right;

    if (kind == NONE)return 0;
    
    if (kind == NUMBER)return node->value; 
    if (kind == IDENTIFIER)return table[node->name]; 
    if (kind == SEMICOLON){
        interpret(left);
        return interpret(right);
    }
    if (kind == IF){
        if (interpret(left)){
            interpret(right);
        }
        return 0;
    }
    if(kind == WHILE){
        int i = 0;
        while (interpret(left) && i < 100000){
            i++;
            interpret(right);
        }
        if (i == 100000)std::cout << "MAXIMUM DEPTH REACHED\n";
        return 0;
    }
    if(kind == ASSIGN || kind == LET){
        table[left->name] = interpret(right); 
        return 0;
    }
    if(kind == PRINT){
        std::cout << interpret(left) << "\n";
        return 0;
    }

    if (kind == PLUS)       return interpret(left) + interpret(right);
    if (kind == MINUS)      return interpret(left) - interpret(right);
    if (kind == STAR)       return interpret(left) * interpret(right);
    if (kind == DIV)        return interpret(left) / interpret(right);
    if (kind == EQ)         return interpret(left) == interpret(right);
    if (kind == NOT_EQ)     return interpret(left) != interpret(right);
    if (kind == LESS)       return interpret(left) < interpret(right);
    if (kind == LESS_EQ)    return interpret(left) <= interpret(right);
    if (kind == GREATER)    return interpret(left) > interpret(right);
    if (kind == GREATER_EQ) return interpret(left) >= interpret(right);

    return 0;
}

void debug_node(Node* node, int i = 0){
    TokenKind kind = node->kind;
    Node* left = node->left;
    Node* right = node->right;

    if (kind == NONE)return;
    
    if (kind == NUMBER){
        std::cout << std::string(i*2, ' ') << "NUMBER: " << node->value << "\n"; 
        return;
    }
    if (kind == IDENTIFIER){
        std::cout << std::string(i*2, ' ') << "IDENTIFIER: " << table[node->name] << "\n"; 
        return;
    }
    if (kind == SEMICOLON){
        std::cout << std::string(i*2, ' ') << "SEMICOLON" << "\n";
        debug_node(left, i+1);
        debug_node(right, i+1);
        return;
    }
    if (kind == IF){
        std::cout << std::string(i*2, ' ') << "IF" << "\n";
        debug_node(left, i+1);
        std::cout << std::string(i*2, ' ') << "THEN" << "\n";
        debug_node(right, i+1);

        return;
    }
    if(kind == WHILE){
        std::cout << std::string(i*2, ' ') << "WHILE" << "\n";
        debug_node(left, i+1);
        std::cout << std::string(i*2, ' ') << "DO" << "\n";
        debug_node(right, i+1);

        return;
    }
    if(kind == ASSIGN || kind == LET){
        std::cout << std::string(i*2, ' ') << "ASSIGN" << "\n";
        debug_node(left, i+1);
        std::cout << std::string(i*2, ' ') << "TO" << "\n";
        debug_node(right, i+1);
        
        return;
    }
    if(kind == PRINT){
        std::cout << std::string(i*2, ' ') << "PRINT" << "\n";
        debug_node(left, i+1);

        return;
    }

    std::cout << std::string(i*2, ' ');
    print_tokens(std::vector<Token>{Token(node->kind, "", 0)});
    debug_node(left, i+1);
    debug_node(right, i+1);

    return;
}

bool lexical(Node* node, std::set<std::string>* variables){
    TokenKind kind = node->kind;
    Node* left = node->left;
    Node* right = node->right;

    if (kind == NONE)return true;
    
    if (kind == NUMBER){
        return true;
    }
    if (kind == IDENTIFIER){
        // REQURIE VARIABLE TO BE DEFINED
        if (variables->find(node->name) == variables->end()){
            std::cout << "VARIABLE " << node->name << " USED BUT NEVER DEFINED\n";
            return false;
        }
        return true;
    }
    if(kind == LET){
        // ONLY ALLOW ONE DEFINITION OF A VARIABLE
        if (variables->find(node->left->name) != variables->end()){
            std::cout << "VARIABLE " << node->left->name << " ALREADY DEFINED ONCE\n";
            return false;
        }

        if (!lexical(node->right, variables))return false;

        variables->insert(node->left->name);
        return true;
    }

    if(kind == SEMICOLON){
        bool l = lexical(left, variables);

        if (node->left->kind != SEMICOLON && !l){
            std::cout << "ERROR ON LINE:\n" << "\n";
            std::cout << node->left->print() << "\n";
        }

        bool r = lexical(right, variables);

        if (node->right->kind != SEMICOLON && !l){
            std::cout << "ERROR ON LINE:\n" << "\n";
            std::cout << node->right->print() << "\n";
        }

        return l && r;
    }

    return lexical(left, variables) && lexical(right, variables);
}

bool lexical_analasys(Node* node){
    if (!lexical(node, new std::set<std::string>())){
        std::cout << "FAILED LEXICAL!\n";
        return 0;
    }
    return 1;
}


Node* constant_folding(Node* node, int i = 0, bool comment = false){
    TokenKind kind = node->kind;
    Node* left = node->left;
    Node* right = node->right;

    if (kind == NONE)return node;
    
    if (kind == NUMBER){
        if(comment)std::cout << std::string(i*2, ' ') << "NUMBER: " << node->value << "\n"; 
        return node;
    }
    if (kind == IDENTIFIER){
        if(comment)std::cout << std::string(i*2, ' ') << "IDENTIFIER: " << node->name << "\n"; 
        return node;
    }
    if (kind == SEMICOLON){
        if(comment)std::cout << std::string(i*2, ' ') << "SEMICOLON" << "\n";
        node->left = constant_folding(left);
        node->right = constant_folding(right);
        return node;
    }
    if (kind == IF){
        if(comment)std::cout << std::string(i*2, ' ') << "IF" << "\n";
        node->left = constant_folding(left, i+1);
        /*if (node->left->kind == NUMBER && node->left->value == 0){
            if(comment)std::cout << std::string(i*2, ' ') << "NEVER!\n";
            return new Node();
        }*/
        if(comment)std::cout << std::string(i*2, ' ') << "THEN" << "\n";
        node->right = constant_folding(right, i+1);

        return node;
    }
    if(kind == WHILE){
        if(comment)std::cout << std::string(i*2, ' ') << "WHILE" << "\n";
        node->left = constant_folding(left, i+1);
        if(comment)std::cout << std::string(i*2, ' ') << "DO" << "\n";
        right = constant_folding(right, i+1);

        return node;
    }
    if(kind == ASSIGN || kind == LET){
        if(comment)std::cout << std::string(i*2, ' ') << "ASSIGN" << "\n";

        if(comment)std::cout << std::string(i+1, ' ') << std::string(i+1, ' ') << left->name << "\n";
        if(comment)std::cout << std::string(i*2, ' ') << "TO" << "\n";
        node->right = constant_folding(right, i+1);
        
        return node;
    }
    if(kind == PRINT){
        if(comment)std::cout << std::string(i*2, ' ') << "PRINT" << "\n";
        node->left = constant_folding(left, i+1);

        return node;
    }

    if(comment)std::cout << std::string(i*2, ' ');
    if(comment)print_tokens(std::vector<Token>{Token(node->kind, "", 0)});
    
    if (is_arithmatic_kind(kind)) { 
        if(comment)std::cout << std::string(i*2, ' ') << "IS ARTITM\n";
        node->left = constant_folding(left, i+1); 
        node->right = constant_folding(right, i+1); 
    }
    else return node;

    if (node->left->kind == NUMBER && node->right->kind == NUMBER){
        if(comment)std::cout << std::string(i*2, ' ') << "INTERPRET -> " << interpret(node) << "\n";
        return new Node(NUMBER, interpret(node));
    }
    
    if(comment)std::cout << std::string(i*2, ' ') << "NOT INTERPRETED\n";

    return node;
}


Node* dead_code(Node* node){
    TokenKind kind = node->kind;

    if (kind == NONE)return node;
    
    if (kind == NUMBER)return node;
    
    if (kind == IDENTIFIER)return node;

    if (kind == IF || kind == WHILE){
        if (node->left->kind == NUMBER && node->left->value == 0)
            return new Node(NUMBER, 0);
    }


    node->left = dead_code(node->left);
    node->right = dead_code(node->right);
    if (kind == SEMICOLON){
        if (node->left->kind == NUMBER && node->left->value == 0){
            return node->right;
        }
        if (node->right->kind == NUMBER && node->right->value == 0){
            return node->left;
        }
    }
    return node;
}


int main() {
    /*
    std::string code = R"""(
    print(22);
    a = 13 + 4;
    print(a);   
    
    print(1);
    if(a == 17){
        print(a);
        a = a + 1;
        print(a);
    };
    print(2);

    if(a == 18){
        print(a);
        a += 1;
        print(a);
    };

    print(3);
    print(a)
    
    )""";
    let H = 1;let l = 2;let h = 3;let z = 4;let NH = 5;let a = 6;
    if (  H /  l > h  ) {
        print( z  ) ;
        while  (  21 ==  1    )    {
            H  /= (   (  NH  ) )   ;
                a = (  ( 5  )  )  ;
        } ;
    } ;
    
    a = (3 + 2) * (4 + 1);
    let b = 3 * (3 + 1) / 4;
    print(a);
    print(b);
    if (3 + -6 - 1 == 2 * 4 + b) {
        a = a + 4 * 2 + 3;
        print(a);
    };
    let i = 0;
    while (i < 10) {
        b = b - 3 + 8;
        print(b);
        i = i + 1;
    };
    */

    //std::string code = "let a = (3 + 2) / 4; let b = (3 + 2) * (4 + 1) + a; ";
    //; a = 3 * (3 + 2) / 4"

    
    /*
    */
    std::string code = R"""(
        // FIBONACCI
        let a = 0;
        let b = 1;
        let i = 0;
        while(i < 28){
            print(a);

            b = a + b;
            a = b - a;

            i += 1;
        };

    )""";

    /*
    std::string code = R"""(
    let rIC = (3 + 2 ) / 4;
    let j = (16 / 4) * (1 + 2);
    let WU = 32;
    let MiW = 7;
    if   (  ( 0    ) -  (  0   /  39 ) +   ( 93  /  02  ) !=   77 - j )  {
        print(  j ) ;
    }  ;
    if  (  3 !=  075 )  {
        while (  28 >= rIC  ) {
            WU  /= MiW +   1 ;
        } ;
    } ;
    )""";

    /*
    IF L_PAREN L_PAREN NUMBER(86) DIV IDENTIFIER(ca) IDENTIFIER(a) DIV NUMBER(1) DIV IDENTIFIER(Fc) IDENTIFIER(c) NUMBER(9) STAR IDENTIFIER(JI) IDENTIFIER(I) NUMBER(3) R_PAREN DIV IDENTIFIER(Y) NUMBER(4) IDENTIFIER(L) NUMBER(5) GREATER_EQ L_PAREN NUMBER(3) R_PAREN R_PAREN L_CURLY IF L_PAREN NUMBER(5) DIV NUMBER(21) DIV NUMBER(6115) GREATER_EQ L_PAREN NUMBER(8) R_PAREN DIV NUMBER(3) R_PAREN L_CURLY IDENTIFIER(R) NUMBER(6) ASSIGN IDENTIFIER(D) NUMBER(7) STAR NUMBER(138) SEMICOLON IF L_PAREN L_PAREN IDENTIFIER(x) PLUS NUMBER(6) STAR IDENTIFIER(Y) STAR NUMBER(59) STAR NUMBER(17) R_PAREN EQ L_PAREN NUMBER(3) R_PAREN R_PAREN L_CURLY IF L_PAREN L_PAREN NUMBER(50) R_PAREN LESS_EQ L_PAREN NUMBER(5) R_PAREN R_PAREN L_CURLY IDENTIFIER(J) ASSIGN IDENTIFIER(Y) SEMICOLON WHILE L_PAREN L_PAREN L_PAREN IDENTIFIER(n) R_PAREN R_PAREN GREATER L_PAREN IDENTIFIER(H) R_PAREN R_PAREN L_CURLY WHILE L_PAREN L_PAREN L_PAREN IDENTIFIER(z) NUMBER(6) R_PAREN DIV IDENTIFIER(e) NUMBER(1) R_PAREN GREATER_EQ NUMBER(3) R_PAREN L_CURLY IDENTIFIER(I) ASSIGN IDENTIFIER(Y) DIV IDENTIFIER(t) SEMICOLON IF L_PAREN IDENTIFIER(f) NUMBER(3) MINUS NUMBER(79) NOT_EQ NUMBER(9128) R_PAREN L_CURLY IDENTIFIER(w) NUMBER(87) IDENTIFIER(X) ASSIGN NUMBER(86) MINUS NUMBER(39) MINUS IDENTIFIER(UU) IDENTIFIER(U) SEMICOLON R_CURLY SEMICOLON R_CURLY SEMICOLON R_CURLY SEMICOLON R_CURLY SEMICOLON R_CURLY SEMICOLON R_CURLY SEMICOLON R_CURLY SEMICOLON IF L_PAREN L_PAREN NUMBER(2) R_PAREN GREATER_EQ NUMBER(8) DIV NUMBER(1) PLUS IDENTIFIER(Y) R_PAREN L_CURLY IDENTIFIER(N) NUMBER(0) ASSIGN NUMBER(19) SEMICOLON PRINT L_PAREN IDENTIFIER(vpq) IDENTIFIER(q) R_PAREN SEMICOLON IDENTIFIER(wE) IDENTIFIER(E) NUMBER(6) IDENTIFIER(Y) ASSIGN L_PAREN NUMBER(3) R_PAREN SEMICOLON R_CURLY SEMICOLON NONE END_OF_FILE
    */
    /*
    )-""";
    std::string code = R"""(*/
    

    /*



<program> ::= <block> | <block> "\n" <program>
<block> ::= <ws> <block> | <while> | <if> | <declaration> | <assignment> | <output>

<while> ::= "while" <ws> "(" <ws> <condition> <ws> ")" <ws> "{\n" <program> "\n" <ws> "}" <ws> ";"
<if> ::= "if" <ws> "(" <ws> <condition> <ws> ")" <ws> "{\n" <program> "\n" <ws> "}" <ws> ";"

<declaration> ::= "let " <ws> <identifier> <ws> "=" <ws> <statement> <ws> ";"
<assignment> ::= <identifier> <ws> <assignor> <ws> <statement> <ws> ";"

<output> ::= "print(" <ws> <statement> <ws> ")" <ws> ";" 

<statement> ::= <condition> | <expression>
<condition> ::= <ws> <expression> <ws> <comparator> <ws> <expression>
<expression> ::= <value> | <expression> <ws> <operator> <ws> <expression> | "(" <ws> <expression> <ws> ")"

<value> ::= <number> | <identifier>

<identifier> ::= <letter> | <identifier> <letter>
<number> ::= <digit> | <digit> <number>
<ws> ::= " " <ws> | " "
<comparator> ::= "<" | ">" | "!=" | "==" | "<=" | ">="
<assignor> ::= "=" | "+=" | "-=" | "*=" | "/="
<operator> ::= "+" | "-" | "*" | "/"
<digit> ::= "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9"
<letter> ::= "A" | "B" | "C" | "D" | "E" | "F" | "G" | "H" | "I" | "J" | "K" | "L" | "M" | "N" | "O" | "P" | "Q" | "R" | "S" | "T" | "U" | "V" | "W" | "X" | "Y" | "Z" | "a" | "b" | "c" | "d" | "e" | "f" | "g" | "h" | "i" | "j" | "k" | "l" | "m" | "n" | "o" | "p" | "q" | "r" | "s" | "t" | "u" | "v" | "w" | "x" | "y" | "z" 

https://bnfplayground.pauliankline.com/?bnf=%3Cprogram%3E%20%3A%3A%3D%20%3Csyntax%3E%20%7C%20%3Csyntax%3E%20%22%5Cn%22%20%3Cprogram%3E%0A%3Csyntax%3E%20%3A%3A%3D%20%3Cws%3E%20%3Csyntax%3E%20%7C%20%3Cwhile%3E%20%7C%20%3Cif%3E%20%7C%20%3Cdeclaration%3E%20%7C%20%3Cassignment%3E%20%7C%20%3Coutput%3E%0A%0A%3Cwhile%3E%20%3A%3A%3D%20%22while%22%20%3Cws%3E%20%22(%22%20%3Cws%3E%20%3Ccondition%3E%20%3Cws%3E%20%22)%22%20%3Cws%3E%20%22%7B%5Cn%22%20%3Cprogram%3E%20%22%5Cn%22%20%3Cws%3E%20%22%7D%22%20%3Cws%3E%20%22%3B%22%0A%3Cif%3E%20%3A%3A%3D%20%22if%22%20%3Cws%3E%20%22(%22%20%3Cws%3E%20%3Ccondition%3E%20%3Cws%3E%20%22)%22%20%3Cws%3E%20%22%7B%5Cn%22%20%3Cprogram%3E%20%22%5Cn%22%20%3Cws%3E%20%22%7D%22%20%3Cws%3E%20%22%3B%22%0A%3Cdeclaration%3E%20%3A%3A%3D%20%22let%20%22%20%3Cws%3E%20%3Cvariable%3E%20%3Cws%3E%20%22%3D%22%20%3Cws%3E%20%3Cexpression%3E%20%3Cws%3E%20%22%3B%22%0A%0A%3Cassignment%3E%20%3A%3A%3D%20%3Cvariable%3E%20%3Cws%3E%20%3Cassignor%3E%20%3Cws%3E%20%3Cexpression%3E%20%3Cws%3E%20%22%3B%22%0A%3Coutput%3E%20%3A%3A%3D%20%22print(%22%20%3Cws%3E%20%3Cvariable%3E%20%3Cws%3E%20%22)%22%20%3Cws%3E%20%22%3B%22%20%0A%0A%0A%3Cws%3E%20%3A%3A%3D%20%22%20%22%20%3Cws%3E%20%7C%20%22%20%22%0A%0A%3Ccondition%3E%20%3A%3A%3D%20%3Cws%3E%20%3Cexpression%3E%20%3Cws%3E%20%3Ccomparator%3E%20%3Cws%3E%20%3Cexpression%3E%0A%3Cexpression%3E%20%3A%3A%3D%20%3Cvalue%3E%20%7C%20%3Cexpression%3E%20%3Cws%3E%20%3Coperator%3E%20%3Cws%3E%20%3Cexpression%3E%20%7C%20%22(%22%20%3Cws%3E%20%3Cexpression%3E%20%3Cws%3E%20%22)%22%0A%0A%3Cvalue%3E%20%3A%3A%3D%20%3Cnumber%3E%20%7C%20%3Cvariable%3E%0A%0A%0A%3Cvariable%3E%20%3A%3A%3D%20%3Cletter%3E%20%7C%20%3Cvariable%3E%20%3Cletter%3E%20%7C%20%3Cvariable%3E%20%3Cdigit%3E%0A%3Cnumber%3E%20%3A%3A%3D%20%3Cdigit%3E%20%7C%20%3Cdigit%3E%20%3Cnumber%3E%0A%3Ccomparator%3E%20%3A%3A%3D%20%22%3C%22%20%7C%20%22%3E%22%20%7C%20%22!%3D%22%20%7C%20%22%3D%3D%22%20%7C%20%22%3C%3D%22%20%7C%20%22%3E%3D%22%0A%3Cassignor%3E%20%3A%3A%3D%20%22%3D%22%20%7C%20%22%2B%3D%22%20%7C%20%22-%3D%22%20%7C%20%22*%3D%22%20%7C%20%22%2F%3D%22%0A%3Coperator%3E%20%3A%3A%3D%20%22%2B%22%20%7C%20%22-%22%20%7C%20%22*%22%20%7C%20%22%2F%22%0A%3Cdigit%3E%20%3A%3A%3D%20%220%22%20%7C%20%221%22%20%7C%20%222%22%20%7C%20%223%22%20%7C%20%224%22%20%7C%20%225%22%20%7C%20%226%22%20%7C%20%227%22%20%7C%20%228%22%20%7C%20%229%22%0A%3Cletter%3E%20%3A%3A%3D%20%22A%22%20%7C%20%22B%22%20%7C%20%22C%22%20%7C%20%22D%22%20%7C%20%22E%22%20%7C%20%22F%22%20%7C%20%22G%22%20%7C%20%22H%22%20%7C%20%22I%22%20%7C%20%22J%22%20%7C%20%22K%22%20%7C%20%22L%22%20%7C%20%22M%22%20%7C%20%22N%22%20%7C%20%22O%22%20%7C%20%22P%22%20%7C%20%22Q%22%20%7C%20%22R%22%20%7C%20%22S%22%20%7C%20%22T%22%20%7C%20%22U%22%20%7C%20%22V%22%20%7C%20%22W%22%20%7C%20%22X%22%20%7C%20%22Y%22%20%7C%20%22Z%22%20%7C%20%22a%22%20%7C%20%22b%22%20%7C%20%22c%22%20%7C%20%22d%22%20%7C%20%22e%22%20%7C%20%22f%22%20%7C%20%22g%22%20%7C%20%22h%22%20%7C%20%22i%22%20%7C%20%22j%22%20%7C%20%22k%22%20%7C%20%22l%22%20%7C%20%22m%22%20%7C%20%22n%22%20%7C%20%22o%22%20%7C%20%22p%22%20%7C%20%22q%22%20%7C%20%22r%22%20%7C%20%22s%22%20%7C%20%22t%22%20%7C%20%22u%22%20%7C%20%22v%22%20%7C%20%22w%22%20%7C%20%22x%22%20%7C%20%22y%22%20%7C%20%22z%22%20%0A&name=    */

    std::cout << code << "\n";

    auto tokens = lex(code);
    //print_tokens(tokens);

    Node* expr = parse(&tokens);

    if (!lexical_analasys(expr))return 0;
    
    expr = constant_folding(expr);

    expr = dead_code(expr);

    std::cout << expr->print() << "\n";

    interpret(expr);
    /*std::cout << expr->print() << "\n";

    std::cout << "*****************\nSTART DEBUG\n";

    debug_node(expr);
    
    std::cout << "END DEBUG\n-----------------\nFIX CODE\n---------------\n";
    */

    
    /*
    std::cout << "END FIX\n******************\nSTART DEBUG\n---------------\n";

    debug_node(fixed);
    */

    /*
    Node* expr2 = constfactor(expr);

    std::cout << "*****************\nSTART DEBUG\n";

    debug_node(expr);
    
    std::cout << "END DEBUG\n-----------------\n";

    std::cout << expr2->print() << "\n";


    Node* expr3 = constfactor(expr2);
    */



    return 0;
}