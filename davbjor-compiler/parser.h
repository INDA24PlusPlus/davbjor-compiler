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
            std::cout << "BUG AT " << pos << " -> " << token_as_string((*tokens).at(pos).kind) << " -> " << ((*tokens).at(pos).value) << " => " << token_as_string((*tokens).at(pos+1).kind) <<  "\n";
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

    Node() : kind(NONE) {}
    Node(TokenKind k) : kind(k) {}
    Node(TokenKind k, int v) : kind(k), value(v) {}
    Node(TokenKind k, std::string n) : kind(k), name(n) {}
    Node(TokenKind k, Node* l, Node* r) : kind(k), left(l), right(r) {}

    std::string print(int indent = 0, bool parent_aritm = false){
        if (kind == NONE)return "";
        //print_tokens(std::vector<Token>{Token {kind,0,0}});
        if (kind == NUMBER)return std::to_string(value); 
        if (kind == IDENTIFIER)return name; 
        if (kind == SEMICOLON){
            std::string l = "";
            std::string r = "";
            if (left != nullptr)
                l = left->print(indent);
            if (right != nullptr)
                r = right->print(indent);
            //std::cout << "r => +" << r << "+\n";
            //if (l != "")l = std::string(indent*4, ' ') + l;
            if (l.at(l.size()-1) != '\n')l = std::string(indent*4, ' ') + l + ";\n";
            if (r.at(r.size()-1) != '\n')r = std::string(indent*4, ' ') + r + ";\n";
            return l + r;
        }
        if (kind == IF){
            return std::string(indent*4, ' ') +"if(" + left->print() + "){\n" + right->print(indent+1) + std::string(indent*4, ' ') + "}\n"; 
        }
        if(kind == WHILE){
            return std::string(indent*4, ' ') +"while(" + left->print() + "){\n" + right->print(indent+1) + std::string(indent*4, ' ') + "}\n"; 
        }
        if(kind == LET){
            return std::string(indent*4, ' ') + "int " + left->print() + " = " + right->print(indent+1) + ";\n"; 
        }
        if(kind == ASSIGN){
            return std::string(indent*4, ' ') + left->print() + " = " + right->print(indent+1) + ";\n"; 
        }
        if(kind == PRINT){
            return std::string(indent*4, ' ') +"std::cout << (" + left->print() + ") << \"\\n\";\n"; 
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
        
        if (parent_aritm)return "( " + left->print(indent, true) + " " + c + " " + right->print(indent, true) + " )";
        else return left->print(indent, true) + " " + c + " " + right->print(indent, true);
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