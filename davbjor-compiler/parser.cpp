#include "parser.h"

#include <string>
#include <vector>
#include <map>

Node* parse_i(Parser* parser){
    Node* self = new Node(NUMBER, 0);
    Token token = parser->get_token();
    
    if (token.kind == L_PAREN){
        if (!parser->next_token())return self;  
    
        Node* next = parse_t(parser);

        parser->next_token();
        return next;
    }

    if (token.kind == NUMBER){
        self = new Node(NUMBER, stoi(token.value));

        if (!parser->next_token())return self;
        token = parser->get_token();
    }

    if (token.kind == IDENTIFIER){
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
    Token next_token = parser->state_machine(1);

    if (token.kind == IDENTIFIER && next_token.kind == ASSIGN){
        self->kind = ASSIGN;
        self->left = new Node(token.kind, token.value);

        if (!parser->next_token())parser->bug();
        if (!parser->next_token())parser->bug();

        self->right = parse_c(parser);
        return self;
    }


    if (token.kind == IDENTIFIER && (
        next_token.kind == PLUS_EQ
        || next_token.kind == MINUS_EQ
        || next_token.kind == STAR_EQ
        || next_token.kind == DIV_EQ
    )){
        TokenKind op;
        if (next_token.kind == PLUS_EQ)op = PLUS;
        if (next_token.kind == MINUS_EQ)op = MINUS;
        if (next_token.kind == STAR_EQ)op = STAR;
        if (next_token.kind == DIV_EQ)op = DIV;

        self->kind = ASSIGN;
        self->left = new Node(token.kind, token.value);

        if (!parser->next_token())parser->bug();
        if (!parser->next_token())parser->bug();

        self->right = new Node(
            op,
            new Node(IDENTIFIER, token.value),
            parse_c(parser)
        );

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

        //self->next = parse_b(parser);
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
