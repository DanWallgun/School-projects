#ifndef NODE_H
#define NODE_H

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>

using namespace std;

class Node {
public:
        string name;
        Node();
        virtual double calculate (map <string, double> &variables) = 0;
};

double _integrateNode_ (Node *expr, double a, double b, int n, string variable);

double integrateNode (Node *expr, double a, double b, double eps, string variable);

class Variable : public Node {
public:
        bool isConstant;
        double value;
        string name;

        Variable(double value);
        Variable(double &value);
        Variable(string &name);

        double calculate (map <string, double> &variables) override;
};

class BinaryOperator : public Node {
public:
        string name;
        // operatorAssociativity associativity;
        // operatorPriority priority;
        Node *leftOperand, *rightOperand;

        BinaryOperator(string &name);
        BinaryOperator(string &name, Node *leftOp, Node *rightOp);

        double calculate (map <string, double> &variables) override;
};

class Function : public Node {
public:
        string name;
        vector <Node*> operands;

        Function (string &name);

        void addOp(Node *node);

        double calculate (map <string, double> &variables) override;
};

#endif // NODE_H
