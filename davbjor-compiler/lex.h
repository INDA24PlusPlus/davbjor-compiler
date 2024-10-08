#pragma once

#include <string>
#include <vector>
#include <iostream>

enum TokenKind {
    END_OF_FILE,
    IDENTIFIER,
    NUMBER,
    STRING,

    ASSIGN,
    EQ,
    NOT,
    NOT_EQ,

    LESS,
    LESS_EQ,
    GREATER,
    GREATER_EQ,

    OR,
    AND,
    BIT_OR_EQ,
    BIT_AND_EQ,
    BIT_OR,
    BIT_AND,

    DOT,
    SEMICOLON,
    COLON,
    QUESTION,
    COMMA,

    PLUS_PLUS,
    MINUS_MINUS,
    PLUS_EQ,
    MINUS_EQ,
    SLASH_EQ,
    STAR_EQ,
    DIV_EQ,
    MOD_EQ,

    L_BRACKET,
    R_BRACKET,
    L_CURLY,
    R_CURLY,
    L_PAREN,
    R_PAREN,

    PLUS,
    MINUS,
    DASH,
    SLASH,
    STAR,
    DIV,
    MOD,

    // KEYWORDS
    LET,
    CONST,
    CLASS,
    NEW,
    IMPORT,
    FROM,
    FUNC,
    IF,
    ELSE,
    FOREACH,
    WHILE,
    FOR,
    EXPORT,
    TYPEOF,
    IN,

    NONE,
};


struct Token {
    enum TokenKind kind;
    std::string value;
    int index;
    Token(enum TokenKind k, std::string v, int i) :
        kind(k), value(v), index(i) {};
};

struct Tokenizer {
    std::string content;
    int pos;
    Tokenizer(std::string c, int p) :
        content(c), pos(p) {};
};

std::string token_as_string(enum TokenKind tokenKind);

bool is_whitespace(char c);

bool is_newline(char c);

bool is_digit(char c);

bool is_letter(char c);

bool is_arithmatic(char c);

void print_tokens(std::vector<Token> tokens);

void skip(Tokenizer* t);

Token read_id(Tokenizer* t, int i);

std::string read_num(Tokenizer* t);

std::string read_string(Tokenizer* t);

TokenKind read_arithmatic(Tokenizer* t);

void tokenize(Tokenizer* t, std::vector<Token>* tokens);

std::vector<Token> lex(std::string input);