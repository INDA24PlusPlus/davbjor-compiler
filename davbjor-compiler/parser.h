#include "ast.h"
#include "lex.cpp"

#include <vector>
#include <string>
#include <iostream>
#include <map>

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
            if (pos >= (*tokens).size())
                return Token(NONE, "", pos);
            
            if (skip)next_token();
            return tokens->at(pos);
        }
        bool next_token(){
            if (++pos >= (*tokens).size())return false;
            //if((*tokens)[pos].kind == END_OF_FILE)return false;
            return true;
        }
        void bug(){
            throw std::invalid_argument( "unexpected token at: "+pos );
        }
        Token state_machine(int steps){
            if (pos + steps >= tokens->size())  
                return Token(NONE, "", pos + steps);
            
            return tokens->at(pos + steps);
        }
};

Node* parse_i(Parser* parser, std::map<std::string, int,std::less<>>* table);

Node* parse_u(Parser* parser, std::map<std::string, int,std::less<>>* table);

Node* parse_f(Parser* parser, std::map<std::string, int,std::less<>>* table, Node* first_f);

Node* parse_t(Parser* parser, std::map<std::string, int,std::less<>>* table, Node* first_t);

Node* parse_paren(Parser* parser, std::map<std::string, int,std::less<>>* table);

Node* parse_e(Parser* parser, std::map<std::string, int,std::less<>>* table);

Node* parse_c(Parser* parser, std::map<std::string, int,std::less<>>* table);

Node* parse_s(Parser* parser, std::map<std::string, int,std::less<>>* table, bool should_eval = false);

Node* parse_b(Parser* parser, std::map<std::string, int,std::less<>>* table, bool should_eval = false);

Node* parse_p(Parser* parser, std::map<std::string, int,std::less<>>* table);

Node* parse(std::vector<Token> *tokens);
