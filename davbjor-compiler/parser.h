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
            //if((*tokens)[pos].kind == END_OF_FILE)return false;
            return true;
        }
        void bug(){
            throw std::invalid_argument( "unexpected token at: "+pos );
        }
};

Node* parse_i(Parser* parser);

Node* parse_u(Parser* parser);

Node* parse_f(Parser* parser, Node* first_f);

Node* parse_t(Parser* parser, Node* first_t);

Node* parse_p(Parser* parser);

Node* parse_e(Parser* parser);

Node* parse_c(Parser* parser);

Node* parse(std::vector<Token> *tokens);
