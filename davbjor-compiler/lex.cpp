#include "lex.h"
#include <string>
#include <vector>
#include <iostream>


std::string token_as_string(enum TokenKind tokenKind) {
    switch (tokenKind) {
        case END_OF_FILE: return "END_OF_FILE";
        case IDENTIFIER: return "IDENTIFIER";
        case NUMBER: return "NUMBER";
        case STRING: return "STRING";
        case ASSIGN: return "ASSIGN";
        case EQ: return "EQ";
        case NOT: return "NOT";
        case NOT_EQ: return "NOT_EQ";
        case LESS: return "LESS";
        case LESS_EQ: return "LESS_EQ";
        case GREATER: return "GREATER";
        case GREATER_EQ: return "GREATER_EQ";
        case OR: return "OR";
        case AND: return "AND";
        case BIT_OR: return "BIT_OR";
        case BIT_AND: return "BIT_AND";
        case BIT_OR_EQ: return "BIT_OR_EQ";
        case BIT_AND_EQ: return "BIT_AND_EQ";
        case DOT: return "DOT";
        case SEMICOLON: return "SEMICOLON";
        case COLON: return "COLON";
        case QUESTION: return "QUESTION";
        case COMMA: return "COMMA";
        case PLUS_PLUS: return "PLUS_PLUS";
        case MINUS_MINUS: return "MINUS_MINUS";
        case PLUS_EQ: return "PLUS_EQ";
        case MINUS_EQ: return "MINUS_EQ";
        case SLASH_EQ: return "SLASH_EQ";
        case STAR_EQ: return "STAR_EQ";
        case DIV_EQ: return "DIV_EQ";
        case MOD_EQ: return "MOD_EQ";
        case L_BRACKET: return "L_BRACKET";
        case R_BRACKET: return "R_BRACKET";
        case L_CURLY: return "L_CURLY";
        case R_CURLY: return "R_CURLY";
        case L_PAREN: return "L_PAREN";
        case R_PAREN: return "R_PAREN";
        case PLUS: return "PLUS";
        case MINUS: return "MINUS";
        case DASH: return "DASH";
        case SLASH: return "SLASH";
        case STAR: return "STAR";
        case DIV: return "DIV";
        case MOD: return "MOD";
        case LET: return "LET";
        case CONST: return "CONST";
        case CLASS: return "CLASS";
        case NEW: return "NEW";
        case IMPORT: return "IMPORT";
        case FROM: return "FROM";
        case FUNC: return "FUNC";
        case IF: return "IF";
        case ELSE: return "ELSE";
        case FOREACH: return "FOREACH";
        case WHILE: return "WHILE";
        case FOR: return "FOR";
        case EXPORT: return "EXPORT";
        case TYPEOF: return "TYPEOF";
        case IN: return "IN";
        case PRINT: return "PRINT";
        default: return "NONE";
    }
}

bool is_whitespace(char c) {
    return (c == ' ' || c == '\f' || c == '\t' || c == '\v');
}

bool is_newline(char c){
    return (c == '\n' || c == '\r');
}

bool is_digit(char c) {
    return (c >= '0' && c <= '9');
}

//Letter, including '_'
bool is_letter(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool is_arithmatic(char c) {
    return (c == '=' || c == '*' || c == '/' || c == '+' || c == '-' || c == '<' || c == '>' || c == '%' || '|' || '&');
}


/*
PRINTS A LIST OF TOKENS, AND VALUE OF THAT TOKEN IF IT IS OF RIGHT TYPE
*/
void print_tokens(std::vector<Token> tokens) {
    for (auto t : tokens) {
        std::string s = token_as_string(t.kind);

        if (t.kind == IDENTIFIER || t.kind == NUMBER || t.kind == STRING) {
            std::cout << s << "(" << t.value << ") ";
        }
        else std::cout << s << " ";
    }
    std::cout << "\n";
}

void skip(Tokenizer* t) {
    char c = t->content[t->pos];
    int n = t->content.size();

    // while next char exists, and is whitespace -> skip
    while (t->pos < n && (is_whitespace(c) || is_newline(c))) {
        c = t->content[++(t->pos)];
    }

    // check for comments
    if (c == '/') {
        if (t->pos + 1 == n)return;
        c = t->content[t->pos+1];

        // read "//"
        if (c == '/'){
            // skip to blank line
            while (++(t->pos) < n && !is_newline(c)) {
                c = t->content[++(t->pos)];
            }
            // skip past new line
            if (t->pos < n && is_newline(c))c = t->content[++(t->pos)];
            
            // Continue to skip if neccesary
            skip(t);
            return;
        }

        // read "/*"
        if (c == '*'){
            // until comment is ended
            while (++(t->pos) < n){

                // skip to next star
                while (t->pos < n && c != '*') {
                    c = t->content[++(t->pos)];
                }
                // skip past star
                if (t->pos < n && c == '*')c = t->content[++(t->pos)];

                // if comment is not ended => continue searching for end
                if (c != '/')continue;

                // comment is ended
                // Continue to skip if neccesary
                skip(t);
                return;
            }
        }
    }
}

Token read_id(Tokenizer* t, int i){
    std::string s = "";

    char c = t->content[t->pos];
    int n = t->content.size();
    
    if (is_letter(c)) {
        s.push_back(c);
        
        if (t->pos + 1 == n || !is_letter(t->content[t->pos+1])){
            return Token(IDENTIFIER, s, i);
        }
        c = t->content[++(t->pos)];

        while(t->pos < n && is_letter(c)){
            s.push_back(c);
            
            if (t->pos + 1 == n || !is_letter(t->content[t->pos+1])) break;
            c = t->content[++(t->pos)];
        }
    }

    if (s == "let") return Token(LET, "", i);
    if (s == "const") return Token(CONST, "", i);
    if (s == "class") return Token(CLASS, "", i);
    if (s == "new") return Token(NEW, "", i);
    if (s == "import") return Token(IMPORT, "", i);
    if (s == "from") return Token(FROM, "", i);
    if (s == "func") return Token(FUNC, "", i);
    if (s == "if") return Token(IF, "", i);
    if (s == "else") return Token(ELSE, "", i);
    if (s == "foreach") return Token(FOREACH, "", i);
    if (s == "while") return Token(WHILE, "", i);
    if (s == "for") return Token(FOR, "", i);
    if (s == "export") return Token(EXPORT, "", i);
    if (s == "typeof") return Token(TYPEOF, "", i);
    if (s == "in") return Token(IN, "", i);
    if (s == "print") return Token(PRINT, "", i);

    --(t->pos);
    return Token(IDENTIFIER, s, i);

}

std::string read_num(Tokenizer* t){
    std::string s = "";

    char c = t->content[t->pos];
    int n = t->content.size();
    
    if (is_digit(c)) {
        s.push_back(c);
        if (t->pos + 1 == n || !is_digit(t->content[t->pos+1])){
            return s;
        }

        c = t->content[++(t->pos)];

        while(t->pos < n && is_digit(c)){
            s.push_back(c);
            c = t->content[++(t->pos)];
        }
    }

    --(t->pos);
    return s;
}


std::string read_string(Tokenizer* t){
    std::string s = "";

    char c = t->content[t->pos];
    int n = t->content.size();
    
    if (c=='\"') {
        c = t->content[++(t->pos)];
        // Read content of string (in between "")
        while(t->pos < n && c != '\"'){
            s.push_back(c);
            c = t->content[++(t->pos)];
        }
        t->pos++;
    }

    return s;
}

TokenKind read_arithmatic(Tokenizer* t){
    TokenKind kind = NONE;

    char c = t->content[t->pos];
    int n = t->content.size();
    
    if (t->pos + 1 < n && is_arithmatic(t->content[t->pos + 1])){
        char d = t->content[t->pos + 1];
        std::string s = "";
        s.push_back(c);
        s.push_back(d);

        if (s == "++") kind = PLUS_PLUS;
        if (s == "+=") kind = PLUS_EQ;
        if (s == "--") kind = MINUS_MINUS;
        if (s == "-=") kind = MINUS_EQ;
        if (s == "*=") kind = STAR_EQ;
        if (s == "/=") kind = DIV_EQ;

        if (s == ">=") kind = GREATER_EQ;
        if (s == "<=") kind = LESS_EQ;
        if (s == "%=") kind = MOD_EQ;
        if (s == "==") kind = EQ;
        if (s == "!=") kind = NOT_EQ;

        if (s == "||") kind = OR;
        if (s == "&&") kind = AND;
        if (s == "|=") kind = BIT_OR_EQ;
        if (s == "&=") kind = BIT_AND_EQ;

        if (kind != NONE) {
            t->pos++;
            return kind;
        }
    }

    if (c == '+') return PLUS;
    if (c == '-') return MINUS;
    if (c == '*') return STAR;
    if (c == '/') return DIV;

    if (c == '>') return GREATER;
    if (c == '<') return LESS;
    if (c == '=') return ASSIGN;
    if (c == '%') return MOD;

    if (c == '|') return BIT_OR;
    if (c == '&') return BIT_AND;

    return kind;
}



void tokenize(Tokenizer* t, std::vector<Token>* tokens) {
    int n = t->content.size();
    while (t->pos < n) {
        // Skip whitespace and comments
        skip(t);

        char c = t->content[t->pos];
        int i = t->pos;

        if (is_letter(c))           (*tokens).push_back(read_id(t, i));
        else if (is_digit(c))       (*tokens).push_back(Token(NUMBER, read_num(t), i));
        else if (c == '\"')         (*tokens).push_back(Token(STRING, read_string(t), i));
        else if (c == ';')          (*tokens).push_back(Token(SEMICOLON, "", i));
        else if (c == ':')          (*tokens).push_back(Token(COLON, "", i));
        else if (c == '.')          (*tokens).push_back(Token(DOT, "", i));
        else if (c == ',')          (*tokens).push_back(Token(COMMA, "", i));
        else if (c == '(')          (*tokens).push_back(Token(L_PAREN, read_string(t), i));
        else if (c == ')')          (*tokens).push_back(Token(R_PAREN, "", i));
        else if (c == '{')          (*tokens).push_back(Token(L_CURLY, "", i));
        else if (c == '}')          (*tokens).push_back(Token(R_CURLY, "", i));
        else if (c == '[')          (*tokens).push_back(Token(L_BRACKET, "", i));
        else if (c == ']')          (*tokens).push_back(Token(R_BRACKET, "", i));
        else if (is_arithmatic(c))  (*tokens).push_back(Token(read_arithmatic(t), "", i));
        else std::cout << c << "\n";

        t->pos++;
    }
}

std::vector<Token> lex(std::string input) {
    std::vector<Token> tokens {};

    Tokenizer* t = new Tokenizer(input, 0);

    tokenize(t, &tokens);

    tokens.push_back(Token(END_OF_FILE,"",input.size()));

    return tokens;
}
