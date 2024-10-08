#include <string>
#include <iostream>
#include <map>

class Node {
    Node *value;
    public:
        Node *left;
        Node *right;
        Node() {}
        Node(Node *v) : value(v) {}
        Node(Node *l, Node* r) : left(l), right(r) {}

        virtual int eval() {
            int x = 0;
            if (left != nullptr)x = left->eval();
            if (right != nullptr)x = right->eval();
            if (value != nullptr)x = value->eval();
            return x;
        }

        virtual std::string print() {
            std::string s = "";
            
            if (left != nullptr)s += left->print();
            if (right != nullptr)s += right->print();
            if (value != nullptr)s += value->print();
            return s;
        }
        virtual void update(int v) {
            return;
        }
};


class Id : public Node {
    public:
        std::string name;
        std::map<std::string, int,std::less<>>* table;
        Id() { };
        Id(std::string n, std::map<std::string, int, std::less<>>* t) : name(n), table(t) {};
        
        int eval() override {
            int v = table->at(name);
            
            return v;
        }
        std::string print() override {
            int v = table->at(name);

            std::string s = (std::to_string(v));
            return "(" + s + ")";
        }
        void update(int v) override {
            table->at(name) = v;
        }
};



class Print : public Node {
    public:
        Node *value;
        Print() {};
        Print(Node *v) : value(v) {};
        
        int eval() override {
            int v = (*value).eval();
            std::cout << v << "\n";
            return v;
        }

        std::string print() override {
            std::string v = (*value).print();
            return "(print(" + v + "))";
        }
};

/*
Algebraic Operations

Add, Minus, Mult, Div
*/
class Add : public Node {
    public:
        Node *left;
        Node *right;
        Add() {};
        Add(Node *l, Node *r) : left(l), right(r) {};
        
        int eval() override {
            int l = (*left).eval();
            int r = (*right).eval();
            return l + r;
        }

        std::string print() override {
            std::string l = (*left).print();
            std::string r = (*right).print();
            return "(" + l + " + " + r + ")";
        }
};

class Minus : public Node {
    public:
        Node *left;
        Node *right;
        Minus() {};
        Minus(Node *l, Node *r) : left(l), right(r) {};
        
        int eval() override {
            int l = (*left).eval();
            int r = (*right).eval();
            return l - r;
        }

        std::string print() override {
            std::string l = (*left).print();
            std::string r = (*right).print();
            return "(" + l + " - " + r + ")";
        }
};

class Mult : public Node {
    public:
        Node *left;
        Node *right;
        Mult() {};
        Mult(Node *l, Node *r) : left(l), right(r) {};
        
        int eval() override {
            int l = (*left).eval();
            int r = (*right).eval();
            return l * r;
        }

        std::string print() override {
            std::string l = (*left).print();
            std::string r = (*right).print();
            return "(" + l + " * " + r + ")";
        }
};

class Div : public Node {
    public:
        Node *left;
        Node *right;
        Div() {};
        Div(Node *l, Node *r) : left(l), right(r) {};
        
        int eval() override {
            int l = (*left).eval();
            int r = (*right).eval();
            return l / r;
        }

        std::string print() override {
            std::string l = (*left).print();
            std::string r = (*right).print();
            return "(" + l + " / " + r + ")";
        }
};

/*
Unary Operations

Not, Assign
*/
class Not : public Node {
    public:
        Node *value;
        Not() {};
        Not(Node *v) : value(v) {};
        
        int eval() override {
            int v = !(*value).eval();
            return v;
        }

        std::string print() override {
            std::string v = (*value).print();
            return "!(" + v + ")";
        }
};


class Neg : public Node {
    public:
        Node *value;
        Neg() {};
        Neg(Node *v) : value(v) {};
        
        int eval() override {
            int v = -(*value).eval();
            return v;
        }

        std::string print() override {
            std::string v = (*value).print();
            return "-(" + v + ")";
        }
};

class Assign : public Node {
    public:
        Node *id;
        Node *value;
        Assign() {};
        Assign(Node *id, Node *v) : id(id), value(v) {};
        
        int eval() override {
            id->update(value->eval());
            
            return id->eval();
        }


        std::string print() override {
            std::string v = (*value).print();
            return "(=" + v + ")";
        }
};


/*
CONDITIONs, LOOPS

IF, (ELSE), (ELSE (IF))
WHILE
*/
class If : public Node {
    public:
        Node *condition;
        Node *then;
        Node *otherwise;
        If() {};
        If(Node *c, Node *t) : condition(c), then(t) {};
        If(Node *c, Node *t, Node *o) : condition(c), then(t), otherwise(o) {};
        
        int eval() override {
            if ((*condition).eval()) {
                return (*then).eval();
            }
            if (otherwise != nullptr) {
                return (*otherwise).eval();
            }
            return 0;
        }

        std::string print() override {
            std::string c = (*condition).print();
            std::string t = (*then).print();
            std::string o = "";
            if (otherwise != nullptr) {
                o = (*otherwise).print();
            }
            return "( if(" + c + "){" + t + "} else {" + o + "} )";
        }
};


class While : public Node {
    public:
        Node *condition;
        Node *then;
        While() {};
        While(Node *c, Node *t) : condition(c), then(t) {};
        
        int eval() override {
            while ((*condition).eval()) {
                return (*then).eval();
            }
            return 0;
        }

        std::string print() override {
            std::string c = (*condition).print();
            std::string t = (*then).print();

            return "( while (" + c + ") do " + t + " )";
        }
};

/*
COMPARISIONS

NotEq, Eq, Less, LessEq, Greater, GreaterEq
*/
class NotEq : public Node {
    public:
        Node *left;
        Node *right;
        NotEq() {};
        NotEq(Node *l, Node *r) : left(l), right(r) {};
        
        int eval() override {
            int l = (*left).eval();
            int r = (*right).eval();
            return l != r;
        }

        std::string print() override {
            std::string l = (*left).print();
            std::string r = (*right).print();
            return "(" + l + " != " + r + ")";
        }
};

class Eq : public Node {
    public:
        Node *left;
        Node *right;
        Eq() {};
        Eq(Node *l, Node *r) : left(l), right(r) {};
        
        int eval() override {
            int l = (*left).eval();
            int r = (*right).eval();
            return l == r;
        }

        std::string print() override {
            std::string l = (*left).print();
            std::string r = (*right).print();
            return "(" + l + " == " + r + ")";
        }
};

class Less : public Node {
    public:
        Node *left;
        Node *right;
        Less() {};
        Less(Node *l, Node *r) : left(l), right(r) {};
        
        int eval() override {
            int l = (*left).eval();
            int r = (*right).eval();
            return l < r;
        }

        std::string print() override {
            std::string l = (*left).print();
            std::string r = (*right).print();
            return "(" + l + " < " + r + ")";
        }
};

class LessEq : public Node {
    public:
        Node *left;
        Node *right;
        LessEq() {};
        LessEq(Node *l, Node *r) : left(l), right(r) {};
        
        int eval() override {
            int l = (*left).eval();
            int r = (*right).eval();
            return l <= r;
        }

        std::string print() override {
            std::string l = (*left).print();
            std::string r = (*right).print();
            return "(" + l + " <= " + r + ")";
        }
};

class Greater : public Node {
    public:
        Node *left;
        Node *right;
        Greater() {};
        Greater(Node *l, Node *r) : left(l), right(r) {};
        
        int eval() override {
            int l = (*left).eval();
            int r = (*right).eval();
            return l > r;
        }

        std::string print() override {
            std::string l = (*left).print();
            std::string r = (*right).print();
            return "(" + l + " > " + r + ")";
        }
};

class GreaterEq : public Node {
    public:
        Node *left;
        Node *right;
        GreaterEq() {};
        GreaterEq(Node *l, Node *r) : left(l), right(r) {};
        
        int eval() override {
            int l = (*left).eval();
            int r = (*right).eval();
            return l >= r;
        }

        std::string print() override {
            std::string l = (*left).print();
            std::string r = (*right).print();
            return "(" + l + " >= " + r + ")";
        }
};


/*
TYPES OF VARIABLES

Str, Number
*/
class Str : public Node {
    public:
        std::string value;
        Str() {};
        Str(std::string v) : value(v) {};
        
        int eval() override {
            return 0;
        }

        std::string print() override {
            return "\"" + value + "\"";
        }
};

class Number : public Node {
    public:
        int value;
        Number() {};
        Number(int v) : value(v) {};
        
        int eval() override {
            return value;
        }

        std::string print() override {
            return std::to_string(value);
        }
};