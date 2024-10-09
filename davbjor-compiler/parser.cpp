#include "parser.h"

#include <vector>
#include <string>
#include <iostream>

// CONVERTING LEX TOKENS TO AST USING EBNF
/*
E: T
T: T {+- F} | F
F: F {/* U } | U
U: U(I) | I
I: <identifier>
*/

// parse integer
Node* parse_i(Parser* parser){
    // Parenthesis has priority
    if (parser->get_token().kind == L_PAREN) {
        return parse_p(parser);
    }

    if (parser->get_token().kind == NUMBER) {
        // READ NUMBER
        std::string s = parser->get_token().value;
        parser->next_token();

        // CONVERT NUMBER TO INT
        int val = std::stoi(s);
        Node* num = new Number(val);
        
        return num;
    }

    // DEFAULT NUMBER
    return new Number(0);
}


// parse unary
Node* parse_u(Parser* parser){
    // Found Negation
    if (parser->get_token().kind == MINUS) {
        if(!parser->next_token())return 0;

        Node* first_u;

        // Parenthesis has priority
        if (parser->get_token().kind == L_PAREN) {
            first_u = new Neg(parse_p(parser));
        }
        // Read the next token
        else first_u = new Neg(parse_u(parser));

        return first_u;
    }

    // Read integer
    return parse_i(parser);;
}

// parse factor
Node* parse_f(Parser* parser, Node* first_f = nullptr){
    if (first_f == nullptr)
        first_f = parse_u(parser);

    // Keep reading recursive factors
    while(parser->get_token().kind == STAR || parser->get_token().kind == DIV){
        // Found multiplier or division
        TokenKind kind = parser->get_token().kind;
        Node* next_f;

        // Go to next token
        if(!parser->next_token())return first_f;

        // Parenthesis has priority
        if (parser->get_token().kind == L_PAREN) {
            next_f = parse_p(parser);
        }
        // Read the next token
        else next_f = parse_u(parser);

        // Update the factor to be the mult of the previous factor
        if (kind == STAR)   first_f = new Mult(first_f, next_f);
        else                first_f = new Div(first_f, next_f);
    }

    return first_f;
}

// parse term
Node* parse_t(Parser* parser, Node* first_t = nullptr){
    if (first_t == nullptr)
        first_t = parse_f(parser);

    // Keep reading recursive terms
    while(parser->get_token().kind == PLUS || parser->get_token().kind == MINUS){
        TokenKind kind = parser->get_token().kind;
        // Go to next token
        if(!parser->next_token())return first_t;
        
        Node* next_t;

        // Parenthesis has priority
        if (parser->get_token().kind == L_PAREN) {
            next_t = parse_p(parser);
        }
        // Read the next token
        else next_t = parse_f(parser);
        
        // Update the term to be the addition or subtraction of the previous terms
        if (kind == PLUS)   first_t = new Add(first_t, next_t);
        else                first_t = new Minus(first_t, next_t);
    }

    return first_t;
}


Node* parse_p(Parser* parser){
    Node* first_p = new Number(0);

    // Parenthesis has priority
    if (parser->get_token().kind == L_PAREN) {
        if(!parser->next_token())return first_p;
        // Read inside the parenthesis
        first_p = parse_p(parser);

        // When the parenthesis is closing - look for if parenthesis can be used as term or factor
        if (parser->get_token().kind != R_PAREN){
            if (parser->get_token().kind == STAR || parser->get_token().kind == DIV)
                first_p = parse_f(parser, first_p);

            else if (parser->get_token().kind == PLUS || parser->get_token().kind == MINUS)
                first_p = parse_t(parser, first_p);

            else parser->bug();
        }

        // Exit the parenthesis
        if(!parser->next_token())return first_p;
    }
    // Read the next token if no parenthesis
    else first_p = parse_t(parser);

    return first_p;
}

Node* parse_e(Parser* parser){
    return parse_p(parser);
}
