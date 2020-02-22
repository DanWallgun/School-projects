#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <string>
#include <vector>
#include <stack>

#include "Token.cpp"
#include <node.h>

using namespace std;

bool isDecimal(char &c);
bool isLatinLetter(char &c);
const vector <string> allowedFunctions = {"sin", "cos", "sqrt", "log", "max", "integral"},
                      allowedOperators = {"+", "-", "*", "/", "^"};

void tokenization(string &input, vector <Token> &tokens);

void shuntingYardAlgorithm (vector <Token> &tokens, Node *&root);

class Expression {
public:
    Node *rootNode;
    map <string, double> var;
    Expression() {}
    void setExpression(string &input);
    void setVariable(string name, double value);
    void setExpression(Node *node);
    double getValue();
};

#endif // EXPRESSION_H
