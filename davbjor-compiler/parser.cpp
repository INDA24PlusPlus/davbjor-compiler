#include "parser.h"

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

int parse_i(Parser* parser){
    //std::cout << "READING INT : \n";
    if (parser->get_token().kind == NUMBER) {
        std::string s = parser->get_token().value;
        parser->next_token();

        //std::cout << "->  READ :!" << s << "! \n";

        int val = std::stoi(s);
        //std::cout << "stoi -> " << val << "\n";
        return val;
    }
    //std::cout << "  NAN \n";
    return 0;
}



Node* parse_f(Parser* parser){
    // Read integer
    int first_n = parse_i(parser);
    //std::cout << "f found : " << first_n << "\n";
    Node* first_f = new Number(first_n);
    //std::cout << "f saved : " << first_f->eval() << "\n";

    //if(!parser->next_token())return first_f;

    // Found multiplier
    while(parser->get_token().kind == STAR || parser->get_token().kind == DIV){
        TokenKind kind = parser->get_token().kind;
        // Go to next token
        if(!parser->next_token())return first_f;

        // Read the next token
        int next_n = parse_i(parser);
        Node* next_f = new Number(next_n);

        // Update the factor to be the mult of the previous factor
        if (kind == STAR)   first_f = new Mult(first_f, next_f);
        else                first_f = new Div(first_f, next_f);
        //std::cout << "at  f- " << parser->pos << " print =>" << first_f->print() << "\n";
    }

    return first_f;
}


Node* parse_t(Parser* parser){
    Node* first_t = parse_f(parser);
    //if(!parser->next_token())return first_t;
    //std::cout << "t saved : " << first_t->eval() << "\n";
    //std::cout << "t ind: " << parser->pos << "\n";

    // Found plus
    while(parser->get_token().kind == PLUS || parser->get_token().kind == MINUS){
        TokenKind kind = parser->get_token().kind;
        // Go to next token
        if(!parser->next_token())return first_t;

        // Read the next token
        Node* next_t = parse_f(parser);
        

        // Update the term to be the addition of the previous terms
        
        if (kind == PLUS)   first_t = new Add(first_t, next_t);
        else                first_t = new Minus(first_t, next_t);
        //std::cout << "at  t- " << parser->pos << " print =>" << first_t->print() << "\n";
    }

    return first_t;
}


Node* parse_e(Parser* parser){
    return parse_t(parser);
}
