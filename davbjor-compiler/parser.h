#include "ast.h"
#include "lex.cpp"

#include <vector>
#include <string>
#include <iostream>

// CONVERTING LEX TOKENS TO AST USING EBNF
/*
E: T
T: T {+- F} | F
F: F {/* I } | I
I: <identifier>
*/

struct Parser {
    public:
        std::vector<Token> *tokens;
        int pos;
        Parser(std::vector<Token> *t, int p) : tokens(t), pos(p) {};
        Token get_token(bool skip = false){
            if (pos >= (*tokens).size()){
                return Token(NONE, "", pos);
            }
            if (skip)next_token();
            return (*tokens)[pos];
        }
        bool next_token(){
            if (++pos >= (*tokens).size())return false;
            return true;
        }
};

int parse_i(Parser* parser);

Node* parse_f(Parser* parser);


Node* parse_t(Parser* parser);

Node* parse_e(Parser* parser);
