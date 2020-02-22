#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>

#include <node.h>

using namespace std;

double _integrateNode_ (Node *expr, double a, double b, int n, string variable, map <string, double> &variables) {
    double h = (b - a) / n / 2;
    double result = 0;
    variables[variable] = a;
    result += expr->calculate(variables);
    for (int i = 1; i <= n; ++i) {
        variables[variable] = a + (i * 2 - 1) * h;
        result += 4 * expr->calculate(variables);
    }
    for (int i = 1; i <= n - 1; ++i) {
        variables[variable] = a + (i * 2) * h;
        result += 2 * expr->calculate(variables);
    }
    variables[variable] = a + (n * 2) * h;
    result += expr->calculate(variables);
    result *= h;
    result /= 3;
    return result;
}

double integrateNode (Node *expr, double a, double b, double eps, string variable, map <string, double> &variables) {
    const double runge = 1.0 / 15.0;
    int n = 1024;
    double sn = _integrateNode_(expr, a, b, n, variable, variables),
           s2n = _integrateNode_(expr, a, b, n * 2, variable, variables);
    while (std::abs(s2n - sn) * runge >= eps) {
        sn = s2n;
        n *= 2;
        s2n = _integrateNode_(expr, a, b, n * 2, variable, variables);
    }
    return s2n;
}

Node::Node() {}

Variable::Variable(double value) : isConstant(true), value(value) {}
Variable::Variable(double &value) : isConstant(true), value(value) {}
Variable::Variable(string &name) : isConstant(false), name(name) {}

double Variable::calculate (map <string, double> &variables) {
    return isConstant ? value : variables[name];
}

BinaryOperator::BinaryOperator(string &name) : name(name) {}
BinaryOperator::BinaryOperator(string &name, Node *leftOp, Node *rightOp)
                : name(name), leftOperand(leftOp), rightOperand(rightOp) {}

double BinaryOperator::calculate (map <string, double> &variables) {
    if (name == "+")
        return leftOperand->calculate(variables)
                        + rightOperand->calculate(variables);
    else if (name == "-")
            return leftOperand->calculate(variables)
                            - rightOperand->calculate(variables);
    else if (name == "*")
            return leftOperand->calculate(variables)
                            * rightOperand->calculate(variables);
    else if (name == "/")
            return leftOperand->calculate(variables)
                            / rightOperand->calculate(variables);
    else if (name == "^")
            return pow(leftOperand->calculate(variables),
                                    rightOperand->calculate(variables));
}

Function::Function (string &name) : name(name) {}

void Function::addOp(Node *node) {
        operands.push_back(node);
}

double Function::calculate (map <string, double> &variables) {
        if (name == "sin")
            return sin(operands[0]->calculate(variables));
        else if (name == "cos")
            return cos(operands[0]->calculate(variables));
        else if (name == "sqrt")
            return sqrt(operands[0]->calculate(variables));
        else if (name == "log")
            return log(operands[0]->calculate(variables));
        else if (name == "max") {
            vector <double> *temp = new vector <double>;
            for (auto &it : operands)
                    temp->push_back(it->calculate(variables));
            sort(temp->begin(), temp->end());
            double mx = temp->at(temp->size() - 1);
            delete temp;
            return mx;
        }
        else if (name == "integral") {
            return integrateNode(operands[2] , operands[0]->calculate(variables), operands[1]->calculate(variables), 1e-2, "y", variables);
        }
}

