#include "parser.h"

#include <vector>
#include <string>
#include <iostream>

// TODO:
// IMPLEMENT MOD, MOD_EQ

// FIGURE OUT WHEN TO EVAL!!!
// NEED TO EVAL EVERY STATEMENT, BUT CANT DO IT INSIDE NOT ACCESSIBLE IF STATEMENTS!!!

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
        first_p = parse_c(parser, table);

        //std::cout << "TRY CLOSE PAREN\n";
        print_tokens(std::vector<Token>{parser->get_token()});
        // When the parenthesis is closing - look for if parenthesis can be used as term or factor
        if (parser->get_token().kind != R_PAREN && 
            parser->get_token().kind != END_OF_FILE && 
            parser->get_token().kind != L_CURLY && 
            parser->get_token().kind != SEMICOLON ){
            //print_tokens(std::vector<Token>{parser->get_token()});
            //std::cout << "dont close p -> "  << " " << first_p->print() << " = " << first_p->eval() << "\n";
            //print_tokens(std::vector<Token>{parser->get_token()});

            if (parser->get_token().kind == STAR || parser->get_token().kind == DIV)
                first_p = parse_f(parser, table, first_p);

            else if (parser->get_token().kind == PLUS || parser->get_token().kind == MINUS)
                first_p = parse_t(parser, table, first_p);

            else parser->bug();
        }
        if (parser->get_token().kind == R_PAREN){
            // Exit the parenthesis
            if(!parser->next_token())return first_p;
        }
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
        //std::cout << "CREATED == \n";
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
Node* parse_s(Parser* parser, std::map<std::string, int,std::less<>>* table, bool should_eval){
    Node* first_s;

    // ASSIGNING VARIABLES
    if (parser->get_token().kind == IDENTIFIER && (
        parser->state_machine(1).kind == ASSIGN ||
        parser->state_machine(1).kind == PLUS_EQ ||
        parser->state_machine(1).kind == MINUS_EQ ||
        parser->state_machine(1).kind == STAR_EQ ||
        parser->state_machine(1).kind == DIV_EQ
    )){        
        //std::cout << "ASSIGNING: \n";
        // STORE VALUES
        std::string name = parser->get_token().value;
        int value = table->at(name);

        if(!parser->next_token())parser->bug();
        
        TokenKind kind = parser->get_token().kind;
        if(!parser->next_token())parser->bug();

        // Assume assignment
        Node* next_v = parse_c(parser, table);

        if (kind == PLUS_EQ)
            next_v = new Add(new Number(value), next_v);
        else if (kind == MINUS_EQ)
            next_v = new Minus(new Number(value), next_v);
        else if (kind == STAR_EQ)
            next_v = new Mult(new Number(value), next_v);
        else if (kind == DIV_EQ)
            next_v = new Div(new Number(value), next_v);

        // ADD TO TABLE
        //table->at(name) = next_v->eval();
    
        Node* id = new Id(name, table);
        return new Assign(id, next_v);
    }

    // IF STATEMENT
    if (parser->get_token().kind == IF){        
        if(!parser->next_token())parser->bug();


        Node* c;
        Node* then;

        // Expect Parenthesis
        if (parser->get_token().kind == L_PAREN) {
            if(!parser->next_token())return first_s;

            c = parse_c(parser, table);

            if(parser->get_token().kind == R_PAREN){
                if(!parser->next_token())return first_s;
            }
            else {
                //std::cout << "Expected R_PAREN\n";
                print_tokens(std::vector<Token>{parser->get_token()});

            }
            //std::cout << "parsed condition: " << c->print() << " = " << c->eval() << "\n";
        }
        // Otherwise bug
        else parser->bug();

        //std::cout << "AFTER L_PAREN \n";
        
        //print_tokens(std::vector<Token>{parser->get_token()});

        // Expect Curly
        //if (parser->get_token().kind == L_CURLY) {
        //    if(!parser->next_token())parser->bug();


        //std::cout << "INSIDE L_CURLY \n";

        then = parse_b(parser, table);

        // Expect Close Curly
        /*if (parser->get_token().kind != R_CURLY) {
            std::cout << "NOT CURLY!\n";
        }else std::cout << "CURLY\n";*/

        if(!parser->next_token())parser->bug();

        Node* next_s = new If(c, then);
        if (should_eval)
            next_s->eval();

        return next_s;
    }

    first_s = parse_c(parser, table);
    /*if (should_eval)
        first_s->eval();*/

    return first_s;
}


// parse block
Node* parse_b(Parser* parser, std::map<std::string, int,std::less<>>* table, bool should_eval){
    //std::cout << "start block at : \n";
    //print_tokens(std::vector<Token> {parser->get_token()});
    bool empty = true;
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
        
        Node* next_b;
        // ALLOW FOR ASSIGNMENT (DEFAULT 0)
        if(parser->get_token().kind == ASSIGN){
            if(!parser->next_token())parser->bug();
            next_b = parse_c(parser, table);
            value = next_b->eval();
        }

        // ADD TO TABLE
        table->insert({name, value});
        return new Assign(new Id(name, table), next_b);
    }

    // PRINT EVALS
    if (parser->get_token().kind == PRINT){        
        if(!parser->next_token())parser->bug();
        /*
        // Expect Parenthesis
        if (parser->get_token().kind == L_PAREN) {
            
            //int value = parse_paren(parser, table)->eval();
            
            Node* next_b = parse_paren(parser, table);
            
            //std::cout << "RETURN PRINT " << "\n";
            first_b = new Print(next_b);

            if (should_eval)
                first_b->eval();
            empty = false;
        }
        */

        // Expect Parenthesis
        if (parser->get_token().kind == L_PAREN) {
            if(!parser->next_token())return first_b;

            Node* next_b = parse_paren(parser, table);

            if(parser->get_token().kind == R_PAREN){
                if(!parser->next_token())return first_b;
            }
            else {
                //std::cout << "Expected R_PAREN\n";
                print_tokens(std::vector<Token>{parser->get_token()});
                if(!parser->next_token())return first_b;

            }

            first_b = new Print(next_b);

            /*if (should_eval)
                first_b->eval();*/
            empty = false;
            //std::cout << "parsed condition: " << c->print() << " = " << c->eval() << "\n";
        }
        // Read the next token
        else parser->bug();
    }

    // READ ANOTHER BLOCK ON SEMICOLON
    while (parser->get_token().kind == SEMICOLON){        
        if(!parser->next_token())parser->bug();
        //std::cout << "READ ANOTHER BLOCK \n";
        Node* next_b = parse_b(parser, table, should_eval);
        /* ALMOST WORKS WHEN EVAL IS HERE!!!   */
        /*if(should_eval)
            next_b->eval();*/

        if (empty)
            first_b = next_b;
        else
            first_b = new Node(first_b, next_b);

        empty = false;

        //std::cout << "BLOCK AFTER SEMIC \n";

        //return first_b;
    }

    // END READ
    if (parser->get_token().kind == R_CURLY || parser->get_token().kind == END_OF_FILE){ 
        parser->next_token();


        //std::cout << "READ END \n";
        return first_b;
    }

    if (empty)
        return parse_s(parser, table, should_eval);

    /*if (should_eval)
        first_b->eval();*/

    return first_b;
}

// parse program
Node* parse_p(Parser* parser, std::map<std::string, int,std::less<>>* table){
    Node* first_p = parse_b(parser, table, true);

    while(parser->get_token().kind != END_OF_FILE && parser->get_token().kind != NONE){
        //first_p->eval();
        Node* next_p = parse_b(parser, table, true);
        //next_p->eval();
        Node* first_p = new Node(first_p, next_p);
    }

    //first_p->eval();
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
   PRINT (ID | S) 
   S {; B}
S: 
   ID {=, +=, -=, /=, *=} C
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