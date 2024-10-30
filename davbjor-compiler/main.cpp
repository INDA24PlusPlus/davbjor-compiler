/*
#include "parser.cpp"
*/

#include <string>
#include <vector>
#include <map>

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
        while (interpret(left)){
            interpret(right);
        }
        return 0;
    }
    if(kind == ASSIGN){
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

Node* constfactor(Node* node, int i = 0){
    if (node->kind == NUMBER || node->kind == PRINT)return node;
    
    std::cout << std::string(i, ' ') << std::string(i, ' ');
    print_tokens(std::vector<Token>{Token(node->kind, "", 0)});

    TokenKind kind = node->kind;

    if (node->left != nullptr) node->left = constfactor(node->left, i+1);
    if (node->right != nullptr)node->right = constfactor(node->right, i+1);

    
    if( kind == PLUS ||
        kind == MINUS ||
        kind == STAR ||
        kind == DIV ||
        kind == EQ ||
        kind == NOT_EQ ||
        kind == LESS ||
        kind == LESS_EQ ||
        kind == GREATER ||
        kind == GREATER_EQ){
        if ((node->left->kind == NUMBER)
        && (node->right->kind == NUMBER)){
            return new Node(NUMBER, interpret(node));
        }
    }

    return node;
}

void debug_node(Node* node, int i = 0){
    TokenKind kind = node->kind;
    Node* left = node->left;
    Node* right = node->right;

    if (kind == NONE)return;
    
    if (kind == NUMBER){
        std::cout << std::string(i, ' ') << std::string(i, ' ') << "NUMBER: " << node->value << "\n"; 
        return;
    }
    if (kind == IDENTIFIER){
        std::cout << std::string(i, ' ') << std::string(i, ' ') << "IDENTIFIER: " << table[node->name] << "\n"; 
        return;
    }
    if (kind == SEMICOLON){
        std::cout << std::string(i, ' ') << std::string(i, ' ') << "SEMICOLON" << "\n";
        debug_node(left, i+1);
        debug_node(right, i+1);
        return;
    }
    if (kind == IF){
        std::cout << std::string(i, ' ') << std::string(i, ' ') << "IF" << "\n";
        debug_node(left, i+1);
        std::cout << std::string(i, ' ') << std::string(i, ' ') << "THEN" << "\n";
        debug_node(right, i+1);

        return;
    }
    if(kind == WHILE){
        std::cout << std::string(i, ' ') << std::string(i, ' ') << "WHILE" << "\n";
        debug_node(left, i+1);
        std::cout << std::string(i, ' ') << std::string(i, ' ') << "DO" << "\n";
        debug_node(right, i+1);

        return;
    }
    if(kind == ASSIGN){
        std::cout << std::string(i, ' ') << std::string(i, ' ') << "ASSIGN" << "\n";
        debug_node(left, i+1);
        std::cout << std::string(i, ' ') << std::string(i, ' ') << "TO" << "\n";
        debug_node(right, i+1);
        
        return;
    }
    if(kind == PRINT){
        std::cout << std::string(i, ' ') << std::string(i, ' ') << "PRINT" << "\n";
        debug_node(left, i+1);

        return;
    }
    /*
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
    */
    std::cout << std::string(i, ' ') << std::string(i, ' ');
    print_tokens(std::vector<Token>{Token(node->kind, "", 0)});
    debug_node(left, i+1);
    debug_node(right, i+1);

    return;
}

bool is_arithmetic(TokenKind kind){
    if (kind == PLUS)            { return true; }
    else if (kind == MINUS)      { return true; }
    else if (kind == STAR)       { return true; }
    else if (kind == DIV)        { return true; }
    else if (kind == EQ)         { return true; }
    else if (kind == NOT_EQ)     { return true; }
    else if (kind == LESS)       { return true; }
    else if (kind == LESS_EQ)    { return true; }
    else if (kind == GREATER)    { return true; }
    else if (kind == GREATER_EQ) { return true; }

    return false;

}

Node* fix_code(Node* node, int i = 0){
    TokenKind kind = node->kind;
    Node* left = node->left;
    Node* right = node->right;

    if (kind == NONE)return node;
    
    if (kind == NUMBER){
        std::cout << std::string(i, ' ') << std::string(i, ' ') << "NUMBER: " << node->value << "\n"; 
        return node;
    }
    if (kind == IDENTIFIER){
        std::cout << std::string(i, ' ') << std::string(i, ' ') << "IDENTIFIER: " << node->name << "\n"; 
        return node;
    }
    if (kind == SEMICOLON){
        std::cout << std::string(i, ' ') << std::string(i, ' ') << "SEMICOLON" << "\n";
        node->left = fix_code(left);
        node->right = fix_code(right);
        return node;
    }
    if (kind == IF){
        std::cout << std::string(i, ' ') << std::string(i, ' ') << "IF" << "\n";
        node->left = fix_code(left, i+1);
        if (node->left->kind == NUMBER && node->left->value == 0){
            std::cout << std::string(i, ' ') << std::string(i, ' ') << "NEVER!\n";
            return new Node();
        }
        std::cout << std::string(i, ' ') << std::string(i, ' ') << "THEN" << "\n";
        node->right = fix_code(right, i+1);

        return node;
    }
    if(kind == WHILE){
        std::cout << std::string(i, ' ') << std::string(i, ' ') << "WHILE" << "\n";
        node->left = fix_code(left, i+1);
        std::cout << std::string(i, ' ') << std::string(i, ' ') << "DO" << "\n";
        right = fix_code(right, i+1);

        return node;
    }
    if(kind == ASSIGN){
        std::cout << std::string(i, ' ') << std::string(i, ' ') << "ASSIGN" << "\n";
        //left = fix_code(left, i+1);
        std::cout << std::string(i+1, ' ') << std::string(i+1, ' ') << left->name << "\n";
        std::cout << std::string(i, ' ') << std::string(i, ' ') << "TO" << "\n";
        node->right = fix_code(right, i+1);
        
        return node;
    }
    if(kind == PRINT){
        std::cout << std::string(i, ' ') << std::string(i, ' ') << "PRINT" << "\n";
        node->left = fix_code(left, i+1);

        return node;
    }

    std::cout << std::string(i, ' ') << std::string(i, ' ');
    print_tokens(std::vector<Token>{Token(node->kind, "", 0)});
    
    if (is_arithmetic(kind)) { 
        std::cout << std::string(i, ' ') << std::string(i, ' ') << "IS ARTITM\n";
        node->left = fix_code(left, i+1); 
        node->right = fix_code(right, i+1); 
    }
    else return node;

    if (node->left->kind == NUMBER && node->right->kind == NUMBER){
        std::cout << std::string(i, ' ') << std::string(i, ' ') << "INTERPRET -> " << interpret(node) << "\n";
        return new Node(NUMBER, interpret(node));
    }
    
    std::cout << std::string(i, ' ') << std::string(i, ' ') << "NOT INTERPRETED\n";
    /*left = fix_code(left, i+1);
    right = fix_code(right, i+1);*/
    /*
        std::cout << std::string(i, ' ') << std::string(i, ' ') << "CHANGE TO : ";
        print_tokens(std::vector<Token>{Token(node->kind, "", 0)});

        if (kind == PLUS)            return new Node(NUMBER, left->value + right->value);
        else if (kind == MINUS)      return new Node(NUMBER, left->value - right->value);
        else if (kind == STAR)       return new Node(NUMBER, left->value * right->value);
        else if (kind == DIV)        return new Node(NUMBER, left->value / right->value);
        else if (kind == EQ)         return new Node(NUMBER, left->value == right->value);
        else if (kind == NOT_EQ)     return new Node(NUMBER, left->value != right->value);
        else if (kind == LESS)       return new Node(NUMBER, left->value < right->value);
        else if (kind == LESS_EQ)    return new Node(NUMBER, left->value <= right->value);
        else if (kind == GREATER)    return new Node(NUMBER, left->value > right->value);
        else if (kind == GREATER_EQ) return new Node(NUMBER, left->value >= right->value);
        else return node;

        std::cout << std::string(i, ' ') << std::string(i, ' ') << "CREATED NUMBER " << val << "\n";
        return new Node(NUMBER, val);
    }*/
    
    return node;


    /*std::cout << std::string(i, ' ') << std::string(i, ' ');
    print_tokens(std::vector<Token>{Token(node->kind, "", 0)});*/
    /*debug_node(left, i+1);
    debug_node(right, i+1);*/

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
    */

    //std::string code = "a = (3 + 2) / 4; b = (3 + 2) * (4 + 1) + a; ";
    //; a = 3 * (3 + 2) / 4"


    /*
    std::string code = R"""(
        a = 3 + 1;
        b = 6 * a;
    )""";
    */
    std::string code = R"""(
        a = (3 + 2) * (4 + 1);
        b = 3 * (3 + 1) / 4;
        print(a);
        print(b);
        if (3 + -6 - 1 == 2 * 4 + b) {
            a = a + 4 * 2 + 3;
            print(a);
        };
        while (i < 10) {
            b = b - 3 + 8;
            print(b);
            i = i + 1;
        };
        // FIBONACCI
        a = 0;
        b = 1;
        i = 0;
        while(i < 25){
            print(a);

            b = a + b;
            a = b - a;


            i += 1;
        }
    )""";
    /*
    )-""";
    std::string code = R"""(*/
    

    std::cout << code << "\n";

    auto tokens = lex(code);
    print_tokens(tokens);

    Node* expr = parse(&tokens);

    interpret(expr);
    std::cout << expr->print() << "\n";

    std::cout << "*****************\nSTART DEBUG\n";

    debug_node(expr);
    
    std::cout << "END DEBUG\n-----------------\nFIX CODE\n---------------\n";


    Node* fixed = fix_code(expr);

    std::cout << "END FIX\n******************\nSTART DEBUG\n---------------\n";

    debug_node(fixed);

    std::cout << fixed->print() << "\n";

    interpret(fixed);

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