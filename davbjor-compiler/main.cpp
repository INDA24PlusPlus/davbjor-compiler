/*
#include "parser.cpp"
*/

#include <string>
#include <vector>
#include <map>

#include "lex.cpp"
#include "main.h"

std::map<std::string, int> table;


Node* parse_i(Parser* parser){
    Node* self = new Node(NUMBER, 0);
    Token token = parser->get_token();
    
    std::cout << "i - ";
    print_tokens(std::vector<Token>{token});

    if (token.kind == L_PAREN){
        if (!parser->next_token())return self;  
        std::cout << "LEFT PAREN\n";

        Node* next = parse_t(parser);

        std::cout << "RIGHT PAREN\n";
        parser->next_token();
        return next;
    }

    if (token.kind == NUMBER){
        std::cout << "READ NUM (" << token.value << ")\n";
        self = new Node(NUMBER, stoi(token.value));

        if (!parser->next_token())return self;
        token = parser->get_token();
    }

    if (token.kind == IDENTIFIER){
        std::cout << "READ ID\n";
        self = new Node(IDENTIFIER, token.value);

        if (!parser->next_token())return self;
        token = parser->get_token();
    }

    return self;
}

Node* parse_u(Parser* parser){
    Node* self = new Node(NUMBER, 0);
    Token token = parser->get_token();

    if (token.kind == MINUS){
        if (!parser->next_token())return self;
        std::cout << "READ NEG\n";

        self->kind = MINUS;
        self->left = new Node(NUMBER, 0);
        self->right = parse_i(parser);
        return self;
    }

    return parse_i(parser);
}

Node* parse_f(Parser* parser){
    Node* self = parse_u(parser);

    Token token = parser->get_token();
    
    if (token.kind == STAR || token.kind == DIV){
        if (!parser->next_token())return self;

        Node* next = new Node(token.kind);
        next->left = self;
        next->right = parse_f(parser);
        
        self = next;

        token = parser->get_token();
    }

    return self;
}

Node* parse_t(Parser* parser){
    Node* self = parse_f(parser);

    Token token = parser->get_token();
    
    while (token.kind == PLUS || token.kind == MINUS){
        if (!parser->next_token())return self;

        Node* next = new Node(token.kind);
        next->left = self;
        next->right = parse_f(parser);

        self = next;

        token = parser->get_token();
    }


    return self;
}

Node* parse_c(Parser* parser){
    Node* self = parse_t(parser);

    Token token = parser->get_token();

    if (token.kind == EQ ||
        token.kind == NOT_EQ ||
        token.kind == LESS ||
        token.kind == LESS_EQ ||
        token.kind == GREATER_EQ ||
        token.kind == GREATER
    ){
        if (!parser->next_token())return self;

        Node* next = new Node(token.kind);
        next->left = self;
        next->right = parse_t(parser);
        return next;
    }
    
    return self;
}

Node* parse_s(Parser* parser){
    Node* self = new Node(NONE);
    Token token = parser->get_token();

    if (token.kind == IDENTIFIER && parser->state_machine(1).kind == ASSIGN){
        self->kind = ASSIGN;
        self->left = new Node(token.kind, token.value);

        if (!parser->next_token())parser->bug();
        if (!parser->next_token())parser->bug();

        self->right = parse_c(parser);
        return self;
    }

    if (token.kind == IF || token.kind == WHILE) {
        if (!parser->next_token())parser->bug();

        self->kind = token.kind;
        if (parser->get_token(true).kind != L_PAREN)parser->bug();
        self->left = parse_c(parser);
        if (parser->get_token(true).kind != R_PAREN)parser->bug();

        if (parser->get_token(true).kind != L_CURLY)parser->bug();
        self->right = parse_b(parser);
        if (parser->get_token(true).kind != R_CURLY)parser->bug();

        return self;
    }

    self = parse_c(parser);

    return self;
}

Node* parse_b(Parser* parser){
    Node* self = parse_s(parser);

    Token token = parser->get_token();


    if (token.kind == PRINT) {
        if (!parser->next_token())parser->bug();

        self->kind = token.kind;
        if (parser->get_token(true).kind != L_PAREN)parser->bug();
        self->left = parse_c(parser);
        if (parser->get_token(true).kind != R_PAREN)parser->bug();

    }
    token = parser->get_token();

    if (token.kind == SEMICOLON){
        if (!parser->next_token())return self;

        if (   parser->get_token().kind == END_OF_FILE
            || parser->get_token().kind == NONE 
            || parser->get_token().kind == R_CURLY
        )return self;

        Node* next = new Node(SEMICOLON);
        next->left = self;
        next->right = parse_b(parser);
        return next;
    }
    

    return self;
}

Node* parse_p(Parser* parser){
    return parse_b(parser);
}

Node* parse(std::vector<Token> *tokens){
    Parser* parser = new Parser(tokens, 0);

    return parse_p(parser);
}

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

int main() {
    /*
    std::string code = R"""(
    print (22);
    let a = 13 + 4;
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



    /*
    std::string code = R"""(
        a = (3 + 2) * (4 + 1);
        b = 3 * (3 + 1) / 4;
        print(a);
        print(b);
        if (3 + -6 - 1 == 2 * 4) {
            a = a + 4 * 2 + 3;
            print(a);
        };
        while (i < 10) {
            b = b - 3 + 8;
            print(b);
            i = i + 1;
        };
    )""";
    
    std::string code = "3 - 2 + 1";
    */
    std::string code = R"""(
        a = 0;
        b = 1;
        i = 0;
        while(i < 16){
            print(a);
            
            c = a + b;
            a = b;
            b = c;


            i = i + 1;
        }
    )""";

    std::cout << code << "\n";

    auto tokens = lex(code);
    print_tokens(tokens);

    Node* expr = parse(&tokens);

    std::cout << expr->print() << "\n";

    std::cout << "-----------------\n";

    interpret(expr);

    return 0;
}