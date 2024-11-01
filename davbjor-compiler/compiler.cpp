#include "compiler.h"


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

int interpret(std::string code){
    auto tokens = lex(code);
    //print_tokens(tokens);

    Node* expr = parse(&tokens);

    if (!lexical(expr))return 0;    
    expr = constant_folding(expr);
    expr = dead_code(expr);

    return interpret(expr);
}

void debug_node(Node* node, int i){
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

// PERFORMS A RECURSIVE CHECK FOR UNDEFINED VARIABLES and REPEATED DEFINITIONS
bool lexical(Node* node, std::set<std::string>* variables){
    TokenKind kind = node->kind;

    if (kind == NONE || kind == NUMBER)
        return true;

    if (kind == IDENTIFIER){
        // REQURIE VARIABLE TO BE DEFINED
        if (variables->find(node->name) == variables->end()){
            std::cout << "VARIABLE <" << node->name << "> USED BUT NEVER DEFINED\n";
            return false;
        }
        return true;
    }

    if(kind == LET){
        // ONLY ALLOW ONE DEFINITION OF A VARIABLE
        if (variables->find(node->left->name) != variables->end()){
            std::cout << "VARIABLE <" << node->left->name << "> ALREADY DEFINED ONCE\n";
            return false;
        }

        // MAKE SURE VALUE TO BE ASSIGNED IS OK
        if (!lexical(node->right, variables))return false;

        variables->insert(node->left->name);
        return true;
    }

    if(kind == SEMICOLON){
        // PRINT ENTIRE LINE OF ERROR
        bool l = lexical(node->left, variables);

        if (node->left->kind != SEMICOLON && !l){
            std::cout << "ERROR ON LINE:\n" << "\n";
            std::cout << node->left->print() << "\n";
        }

        bool r = lexical(node->right, variables);

        if (node->right->kind != SEMICOLON && !r){
            std::cout << "ERROR ON LINE:\n" << "\n";
            std::cout << node->right->print() << "\n";
        }

        return l && r;
    }

    return lexical(node->left, variables) && lexical(node->right, variables);
}


// EVALUATE ALL CONSTANT EXPRESSIONS DIRECTLY
Node* constant_folding(Node* node){
    TokenKind kind = node->kind;

    if (kind == NONE || kind == NUMBER || kind == IDENTIFIER)
        return node;
    
    node->left = constant_folding(node->left); 
    node->right = constant_folding(node->right); 

    if (is_arithmatic_kind(kind))
        if(node->left->kind  == NUMBER && node->right->kind == NUMBER)
            return new Node(NUMBER, interpret(node));

    // 0 + X = X
    if(kind == PLUS)
        if(node->left->kind == NUMBER && node->left->value == 0)
            return node->right;

    //X - 0 = X, X + 0 = X
    if (kind == PLUS || kind == MINUS)
        if(node->right->kind == NUMBER && node->right->value == 0)
            return node->left;
    

    // 1 * X = X
    if(kind == STAR)
        if(node->left->kind == NUMBER && node->left->value == 1)
            return node->right;

    // X * 1 = X, X / 1 = X
    if (kind == STAR || kind == DIV)
        if(node->right->kind == NUMBER && node->right->value == 1)
            return node->left;

    // 0 * X = 0, X * 0 = 0
    if (kind == STAR)
        if(node->left->kind  == NUMBER && node->left->value == 0
        || node->right->kind == NUMBER && node->right->value == 0)
            return new Node(NUMBER, 0);
    

    return node;
}

// REMOVES CODE THAT CONSTANTLY NEVER WILL GET EXECUTED
Node* dead_code(Node* node){
    TokenKind kind = node->kind;

    if (kind == NONE || kind == NUMBER || kind == IDENTIFIER)
        return node;

    // REMOVE IF AND WHILE STATEMENTS THAT CONSTANTLY NEVER EXECUTES
    if (kind == IF || kind == WHILE){
        if (node->left->kind == NUMBER && node->left->value == 0)
            return new Node(NUMBER, 0);
    }

    node->left = dead_code(node->left);
    node->right = dead_code(node->right);

    // REMOVE BLOCKS IN SEMICOLON THAT ARE CONSTANT 0 - ex. dead IFs and WHILEs
    if (kind == SEMICOLON){
        if (node->left->kind == NUMBER && node->left->value == 0)
            return node->right;
        if (node->right->kind == NUMBER && node->right->value == 0)
            return node->left;
    }

    // REMOVE DEAD ASSIGN TO OWN IDENTIFIER 
    if (kind == ASSIGN){
        if (node->left->kind == IDENTIFIER && node->right->kind == IDENTIFIER)
            if (node->left->name == node->right->name)
                return new Node(NUMBER, 0);
    }

    // REMOVE IFs with NO BODY
    if (kind == IF){
        if (node->right->kind == NUMBER && node->right->value == 0)
            return new Node(NUMBER, 0);
    }

    return node;
}

std::string compile(std::string code){
    auto tokens = lex(code);
    //print_tokens(tokens);

    Node* expr = parse(&tokens);

    if (!lexical(expr))return "";    
    expr = constant_folding(expr);
    expr = dead_code(expr);
    //debug_node(expr);
    

    return expr->print(1);
}

std::string read_file(std::string path){
    // Attempt to open file
    std::ifstream file(path);

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << path << "\n";
        return "";
    }

    // Read the whole file into a string
    std::string input((std::istreambuf_iterator<char>(file)),std::istreambuf_iterator<char>());

    // Close the file
    file.close();

    return input;
}


bool write_file(std::string path, std::string data){
    // Attempt to open file
    std::ofstream file(path);

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << path << "\n";
        return 0;
    }

    file << "#include <iostream>\n\n" << "//AUTOCOMPILED CODE\n\n" << "int main(){\n\n";
    file << data << "\n";
    file << "}\n";

    // Close the file
    file.close();

    return 1;
    
}

