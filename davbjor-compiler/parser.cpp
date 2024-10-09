#include "parser.h"

#include <vector>
#include <string>
#include <iostream>

// CONVERTING LEX TOKENS TO AST USING EBNF
// TAIL RECURSIVE PARSER

/*
C: E (comparison) E | E
E: T
T: T {+- F} | F
F: F {/* U } | U
U: U(I) | I
I: <identifier>


 condition =
     "odd" expression
     | expression ("="|"#"|"<"|"<="|">"|">=") expression .
 
 expression = ["+"|"-"] term {("+"|"-") term} .
 
 term = factor {("*"|"/") factor} .
 
 factor =
     ident
     | number
     | "(" expression ")" .
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

    //std::cout << "t -> " << first_t->print() << " = " << first_t->eval() << "\n";
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
        if (parser->get_token().kind != R_PAREN && parser->get_token().kind != END_OF_FILE){
            //print_tokens(std::vector<Token>{parser->get_token()});
            //std::cout << "dont close p -> " << first_p->print() << " = " << first_p->eval() << "\n";

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
    Node* first_e = parse_p(parser);

    if (parser->get_token().kind == NONE)return first_e;

    if (parser->get_token().kind == STAR || parser->get_token().kind == DIV)
        first_e = parse_f(parser, first_e);

    else if (parser->get_token().kind == PLUS || parser->get_token().kind == MINUS)
        first_e = parse_t(parser, first_e);

    return first_e;
}

Node* parse_c(Parser* parser){
    Node* first_c = parse_e(parser);

    if (parser->get_token().kind == END_OF_FILE)return first_c;

    if (parser->get_token().kind == EQ){        
        if(!parser->next_token())parser->bug();
        first_c = new Eq(first_c, parse_e(parser));
    }
    
    if (parser->get_token().kind == NOT_EQ){
        if(!parser->next_token())parser->bug();
        first_c = new NotEq(first_c, parse_e(parser));
    }
    
    if (parser->get_token().kind == LESS){        
        if(!parser->next_token())parser->bug();
        first_c = new Less(first_c, parse_e(parser));
    }
    
    if (parser->get_token().kind == LESS_EQ){        
        if(!parser->next_token())parser->bug();
        first_c = new LessEq(first_c, parse_e(parser));
    }
    
    if (parser->get_token().kind == GREATER){        
        if(!parser->next_token())parser->bug();
        first_c = new Greater(first_c, parse_e(parser));
    }
    
    if (parser->get_token().kind == GREATER_EQ){        
        if(!parser->next_token())parser->bug();
        first_c = new GreaterEq(first_c, parse_e(parser));
    }


    return first_c;
}

Node* parse(std::vector<Token>* tokens){
    Parser* parser = new Parser(tokens, 0);

    return parse_c(parser);
}