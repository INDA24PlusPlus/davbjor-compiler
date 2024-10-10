#include "parser.h"

#include <vector>
#include <string>
#include <iostream>

// CONVERTING LEX TOKENS TO AST USING EBNF
// TAIL RECURSIVE PARSER

/*
P: B { B}
B: 
   LET ID = E ;
   PRINT (ID / S) 
   S {; B}
S: 
   ID = E
   "if" "(" C ")" "{" B "}" {"else" B}
   "while" "(" C ")" "{" B "}"
   C

C: E (comparison) E | E
E: T
Pa: "(" T ")" | T
T: T {+- F} | F
F: F {/* U } | U
U: U(I) | I
I: <identifier>


From wikipedia:
https://en.wikipedia.org/wiki/Recursive_descent_parser

 program = block "." .
 
 block =
     ["const" ident "=" number {"," ident "=" number} ";"]
     ["var" ident {"," ident} ";"]
     {"procedure" ident ";" block ";"} statement .
 
 statement =
     ident ":=" expression
     | "call" ident
     | "begin" statement {";" statement } "end"
     | "if" condition "then" statement
     | "while" condition "do" statement .
 
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
Node* parse_i(Parser* parser, std::map<std::string, int,std::less<>>* table){
    // Parenthesis has priority
    if (parser->get_token().kind == L_PAREN) {
        return parse_paren(parser, table);
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

    if (parser->get_token().kind == IDENTIFIER) {
        // READ IDENTIFIER
        std::string s = parser->get_token().value;
        parser->next_token();

        // MAKE SURE IDENTIFIER EXISTS
        if (table->find(s) == table->end())parser->bug();

        // CONVERT IDENTIFIER TO NUMBER
        int val = table->at(s);
        Node* num = new Number(val);
        
        return num;
    }

    // DEFAULT NUMBER
    return new Number(0);
}


// parse unary
Node* parse_u(Parser* parser, std::map<std::string, int,std::less<>>* table){
    // Found Negation
    if (parser->get_token().kind == MINUS) {
        if(!parser->next_token())return 0;

        Node* first_u;

        // Parenthesis has priority
        if (parser->get_token().kind == L_PAREN) {
            first_u = new Neg(parse_paren(parser, table));
        }
        // Read the next token
        else first_u = new Neg(parse_u(parser, table));

        return first_u;
    }

    // Read integer
    return parse_i(parser, table);;
}

// parse factor
Node* parse_f(Parser* parser, std::map<std::string, int,std::less<>>* table, Node* first_f = nullptr){
    if (first_f == nullptr)
        first_f = parse_u(parser, table);

    // Keep reading recursive factors
    while(parser->get_token().kind == STAR || parser->get_token().kind == DIV){
        // Found multiplier or division
        TokenKind kind = parser->get_token().kind;
        Node* next_f;

        // Go to next token
        if(!parser->next_token())return first_f;

        // Parenthesis has priority
        if (parser->get_token().kind == L_PAREN) {
            next_f = parse_paren(parser, table);
        }
        // Read the next token
        else next_f = parse_u(parser, table);

        // Update the factor to be the mult of the previous factor
        if (kind == STAR)   first_f = new Mult(first_f, next_f);
        else                first_f = new Div(first_f, next_f);
    }

    return first_f;
}

// parse term
Node* parse_t(Parser* parser, std::map<std::string, int,std::less<>>* table, Node* first_t = nullptr){
    if (first_t == nullptr)
        first_t = parse_f(parser, table);

    // Keep reading recursive terms
    while(parser->get_token().kind == PLUS || parser->get_token().kind == MINUS){
        TokenKind kind = parser->get_token().kind;
        // Go to next token
        if(!parser->next_token())return first_t;
        
        Node* next_t;

        // Parenthesis has priority
        if (parser->get_token().kind == L_PAREN) {
            next_t = parse_paren(parser, table);
        }
        // Read the next token
        else next_t = parse_f(parser, table);
        
        // Update the term to be the addition or subtraction of the previous terms
        if (kind == PLUS)   first_t = new Add(first_t, next_t);
        else                first_t = new Minus(first_t, next_t);
    }

    //std::cout << "t -> " << first_t->print() << " = " << first_t->eval() << "\n";
    return first_t;
}

// parse parenthesis
Node* parse_paren(Parser* parser, std::map<std::string, int,std::less<>>* table){
    Node* first_p = new Number(0);


    // Parenthesis has priority
    if (parser->get_token().kind == L_PAREN) {
        if(!parser->next_token())return first_p;
        // Read inside the parenthesis
        first_p = parse_paren(parser, table);

        // When the parenthesis is closing - look for if parenthesis can be used as term or factor
        if (parser->get_token().kind != R_PAREN && parser->get_token().kind != END_OF_FILE){
            //print_tokens(std::vector<Token>{parser->get_token()});
            //std::cout << "dont close p -> " << first_p->print() << " = " << first_p->eval() << "\n";

            if (parser->get_token().kind == STAR || parser->get_token().kind == DIV)
                first_p = parse_f(parser, table, first_p);

            else if (parser->get_token().kind == PLUS || parser->get_token().kind == MINUS)
                first_p = parse_t(parser, table, first_p);

            else parser->bug();
        }

        // Exit the parenthesis
        if(!parser->next_token())return first_p;
    }
    // Read the next token if no parenthesis
    else first_p = parse_t(parser, table);

    return first_p;
}

// parse expression
Node* parse_e(Parser* parser, std::map<std::string, int,std::less<>>* table){
    Node* first_e = parse_paren(parser, table);

    if (parser->get_token().kind == NONE)return first_e;

    if (parser->get_token().kind == STAR || parser->get_token().kind == DIV)
        first_e = parse_f(parser, table, first_e);

    else if (parser->get_token().kind == PLUS || parser->get_token().kind == MINUS)
        first_e = parse_t(parser, table, first_e);

    return first_e;
}

// parse conditional
Node* parse_c(Parser* parser, std::map<std::string, int,std::less<>>* table){
    Node* first_c = parse_e(parser, table);

    if (parser->get_token().kind == END_OF_FILE)return first_c;

    if (parser->get_token().kind == EQ){        
        if(!parser->next_token())parser->bug();
        first_c = new Eq(first_c, parse_e(parser, table));
    }
    
    if (parser->get_token().kind == NOT_EQ){
        if(!parser->next_token())parser->bug();
        first_c = new NotEq(first_c, parse_e(parser, table));
    }
    
    if (parser->get_token().kind == LESS){        
        if(!parser->next_token())parser->bug();
        first_c = new Less(first_c, parse_e(parser, table));
    }
    
    if (parser->get_token().kind == LESS_EQ){        
        if(!parser->next_token())parser->bug();
        first_c = new LessEq(first_c, parse_e(parser, table));
    }
    
    if (parser->get_token().kind == GREATER){        
        if(!parser->next_token())parser->bug();
        first_c = new Greater(first_c, parse_e(parser, table));
    }
    
    if (parser->get_token().kind == GREATER_EQ){        
        if(!parser->next_token())parser->bug();
        first_c = new GreaterEq(first_c, parse_e(parser, table));
    }


    return first_c;
}

// parse block
Node* parse_s(Parser* parser, std::map<std::string, int,std::less<>>* table){
    Node* first_s;

    // ASSIGNING VARIABLES
    if (parser->get_token().kind == IDENTIFIER && parser->state_machine(1).kind == ASSIGN){        
        // STORE VALUES
        std::string name = parser->get_token().value;
        int value = table->at(name);

        if(!parser->next_token())parser->bug();
        // SKIP "="
        if(!parser->next_token())parser->bug();

        value = parse_c(parser, table)->eval();

        // ADD TO TABLE
        table->at(name) = value;
    
        Node* id = new Id(name, table);
        return new Assign(id, parse_c(parser, table));
    }


    first_s = parse_c(parser, table);
    return first_s;
}


// parse block
Node* parse_b(Parser* parser, std::map<std::string, int,std::less<>>* table){
    Node* first_b;

    // DECLARATION OF VARIABLES
    if (parser->get_token().kind == LET){        
        if(!parser->next_token())parser->bug();

        // EXPECT IDENTIFIER
        if(!parser->get_token().kind == IDENTIFIER)parser->bug();

        // STORE VALUES
        std::string name = parser->get_token().value;
        int value = 0;

        if(!parser->next_token())parser->bug();
        
        // ALLOW FOR ASSIGNMENT (DEFAULT 0)
        if(parser->get_token().kind == ASSIGN){
            if(!parser->next_token())parser->bug();

            value = parse_c(parser, table)->eval();
        }

        // ADD TO TABLE
        table->insert({name, value});
    }

    // PRINT EVALS
    if (parser->get_token().kind == PRINT){        
        if(!parser->next_token())parser->bug();

        // Expect Parenthesis
        if (parser->get_token().kind == L_PAREN) {
            int value = parse_paren(parser, table)->eval();

            std::cout << value << "\n";
        }
        // Read the next token
        else parser->bug();
    }

    // READ ANOTHER BLOCK ON SEMICOLON
    if (parser->get_token().kind == SEMICOLON){        
        if(!parser->next_token())parser->bug();

        Node* next_b = parse_b(parser, table);
        Node* first_b = new Node(first_b, next_b);
    }

    first_b = parse_s(parser, table);

    return first_b;
}

// parse program
Node* parse_p(Parser* parser, std::map<std::string, int,std::less<>>* table){
    Node* first_p = parse_b(parser, table);

    while(parser->get_token().kind != END_OF_FILE && parser->get_token().kind != NONE){
        Node* next_p = parse_b(parser, table);
        Node* first_p = new Node(first_p, next_p);
    }

    return first_p;
}

// parse tokens
Node* parse(std::vector<Token>* tokens){
    Parser* parser = new Parser(tokens, 0);
    std::map<std::string, int,std::less<>>* table = new std::map<std::string, int,std::less<>> ();

    return parse_p(parser, table);
}

/*
P: B { B}
B: 
   LET ID = C;
   PRINT (ID / S) 
   S {; B}
S: 
   ID = C
   "if" "(" C ")" "{" B "}" {"else" B}
   "while" "(" C ")" "{" B "}"
   C

C: E (comparison) E | E
E: T
Pa: "(" T ")" | T
T: T {+- F} | F
F: F {/* U } | U
U: U(I) | I
I: <identifier>
*/