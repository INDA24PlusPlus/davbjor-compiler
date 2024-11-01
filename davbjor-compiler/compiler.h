#include <string>
#include <vector>
#include <map>
#include <set>
#include <fstream>

#include "lex.cpp"
#include "parser.cpp"

std::map<std::string, int> table;

int interpret(Node* node);
int interpret(std::string code);

void debug_node(Node* node, int i = 0);

bool lexical(Node* node, std::set<std::string>* variables = new std::set<std::string>());

Node* constant_folding(Node* node);

Node* dead_code(Node* node);

std::string compile(std::string code);

std::string read_file(std::string path);