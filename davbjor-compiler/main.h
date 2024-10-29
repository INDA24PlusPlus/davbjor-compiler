#include "lex.h"

struct Parser {
    public:
        std::vector<Token> *tokens;
        int pos;
        Parser(std::vector<Token> *t, int p) : tokens(t), pos(p) {};
        Token get_token(bool skip = false){
            if (pos >= (*tokens).size())
                return Token(NONE, "", pos);
            
            Token t = tokens->at(pos);
            if (skip)next_token();
            return t;
        }
        void print_token(){
            if (pos >= (*tokens).size())
                return;
            
            print_tokens(std::vector<Token>{tokens->at(pos)});
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

struct Node {
    TokenKind kind;
    int value;
    std::string name;

    Node* left;
    Node* right;

    Node(TokenKind k) : kind(k) {}
    Node(TokenKind k, int v) : kind(k), value(v) {}
    Node(TokenKind k, std::string n) : kind(k), name(n) {}

    std::string print(){
        if (kind == NONE)return "";
        //print_tokens(std::vector<Token>{Token {kind,0,0}});
        if (kind == NUMBER)return std::to_string(value); 
        if (kind == IDENTIFIER)return name; 
        if (kind == SEMICOLON){
            if (left == nullptr)return "";
            if (right == nullptr)return left->print();
            return left->print() + ";\n" + right->print();
        }
        if (kind == IF){
            return "IF ( " + left->print() + " ) {\n " + right->print() + "\n}"; 
        }
        if(kind == WHILE){
            return "WHILE ( " + left->print() + " ) {\n " + right->print() + "\n}"; 
        }
        if(kind == ASSIGN){
            return left->print() + " = " + right->print(); 
        }
        if(kind == PRINT){
            return "PRINT ( " + left->print() + " )"; 
        }

        std::string c = "~";
        if (kind == PLUS)c = "+";
        if (kind == MINUS)c = "-";
        if (kind == STAR)c = "*";
        if (kind == DIV)c = "/";
        if (kind == EQ)c = "==";
        if (kind == NOT_EQ)c = "!=";
        if (kind == LESS)c = "<";
        if (kind == LESS_EQ)c = "<=";
        if (kind == GREATER)c = ">";
        if (kind == GREATER_EQ)c = ">=";

        if (c == "~")return "";
        
        return "( " + left->print() + " " + c + " " + right->print() + " )";
    }
};

Node* parse_i(Parser* parser);

Node* parse_u(Parser* parser);

Node* parse_f(Parser* parser);

Node* parse_t(Parser* parser);

Node* parse_c(Parser* parser);

Node* parse_s(Parser* parser);

Node* parse_b(Parser* parser);

Node* parse_p(Parser* parser);

Node* parse(std::vector<Token> *tokens);

int main();