#include <expression.h>

using namespace std;

bool isDecimal(char &c) {
    return '0' <= c && c <= '9';
}

bool isLatinLetter(char &c) {
    return 'a' <= c && c <= 'z' || 'A' <= c && c <= 'Z';
}

void tokenization(string &input, vector <Token> &tokens) {
    for (size_t i = 0; i < input.length(); ) {
        if (isDecimal(input[i]) || input[i] == '.') {
            string currectToken = string(1, input[i++]);
            while (i < input.length() && (isDecimal(input[i]) || input[i] == '.'))
                currectToken.push_back(input[i++]);
            tokens.push_back(Token(currectToken, _number));
        }
        if (input[i] == ',')
            tokens.push_back(Token(string(1, input[i++]), _delimiter));
        for (auto &it : allowedOperators)
            if (input.compare(i, min(it.length(), input.length() - i), it) == 0) {
                tokens.push_back(Token(it, _operator));
                i += it.length();
                break;
            }
        if (input[i] == '(' || input[i] == ')')
            tokens.push_back(Token(string(1, input[i++]), _bracket));
        for (auto &it : allowedFunctions)
            if (input.compare(i, min(it.length(), input.length() - i), it) == 0) {
                tokens.push_back(Token(it, _function));
                i += it.length();
                break;
            }
        if (isLatinLetter(input[i])) {
            string currectToken = string(1, input[i++]);
            while (i < input.length() && isLatinLetter(input[i]))
                currectToken.push_back(input[i++]);
            tokens.push_back(Token(currectToken, _variable));
        }
        if (input[i] == ' ')
            ++i;
    }
}

void shuntingYardAlgorithm (vector <Token> &tokens, Node *&root) {
    stack <Token*> stck;
    stack <bool> wereValues;
    stack <int> argCount;

    bool w;
    int a;

    vector <Node*> output;

    for (auto &it : tokens) {
        switch (it.type) {
            case _number:
                // output.append(it.repr + " ");
                output.push_back(new Variable(stod(it.repr)));
                if (!wereValues.empty()) {
                    wereValues.pop();
                    wereValues.push(true);
                }
                break;
            case _variable:
                // output.append(it.repr + " ");
                output.push_back(new Variable(it.repr));
                if (!wereValues.empty()) {
                    wereValues.pop();
                    wereValues.push(true);
                }
                break;
            case _function:
                stck.push(&it);
                argCount.push(0);
                if (!wereValues.empty()) {
                    wereValues.pop();
                    wereValues.push(true);
                }
                wereValues.push(false);
                break;
            case _delimiter:
                while (stck.top()->repr != "(") {
                    //output.append(stck.top()->repr + " ");
                    switch (stck.top()->type) {
                        case _operator:
                            Node *leftOp, *rightOp;
                            leftOp = output[output.size() - 2];
                            rightOp = output[output.size() - 1];
                            output.erase(output.end() - 1);
                            output.erase(output.end() - 1);

                            output.push_back(new BinaryOperator(stck.top()->repr, leftOp, rightOp));
                            break;
                        case _function:
                            break;
                    }
                    stck.pop();
                }
                w = wereValues.top();
                wereValues.pop();
                if (w) {
                    a = argCount.top();
                    argCount.pop();
                    ++a;
                    argCount.push(a);
                }
                wereValues.push(false);
                //todo:	invalid expression
                break;
            case _operator:
                while (!stck.empty() &&
                        stck.top()->type == _operator
                        &&
                        (it.associativity == _leftAssociativity
                            && it.priority <= stck.top()->priority
                        ||
                         it.associativity == _rightAssociativity
                            && it.priority < stck.top()->priority)
                      ) {
                    //output.append(stck.top()->repr + " ");
                    Node *leftOp, *rightOp;
                    leftOp = output[output.size() - 2];
                    rightOp = output[output.size() - 1];
                    output.erase(output.end() - 1);
                    output.erase(output.end() - 1);

                    output.push_back(new BinaryOperator(stck.top()->repr, leftOp, rightOp));

                    stck.pop();
                }
                // if (stck.empty() || stck.top()->repr == "(") {
                stck.push(&it);
                // }
                break;
            case _bracket:
                if (it.repr == "(")
                    stck.push(&it);
                else if (it.repr == ")") {
                    while (!stck.empty() &&
                            stck.top()->repr != "(") {
                        // output.append(stck.top()->repr + " ");
                        Node *leftOp, *rightOp;
                        leftOp = output[output.size() - 2];
                        rightOp = output[output.size() - 1];
                        output.erase(output.end() - 1);
                        output.erase(output.end() - 1);

                        output.push_back(new BinaryOperator(stck.top()->repr, leftOp, rightOp));

                        stck.pop();
                    }
                    stck.pop(); //remove "("
                    if (!stck.empty() && stck.top()->type == _function) {
                        a = argCount.top();
                        argCount.pop();
                        w = wereValues.top();
                        wereValues.pop();
                        if (w)
                            ++a;
                        Token *f = stck.top();
                        //this is needed when building a tree for calculations
                        f->functionArgumentsCount = a;
                        // output.append(stck.top()->repr + " ");
                        Function *newNode = new Function(stck.top()->repr);
                        for (int i = output.size() - a; i < output.size(); ++i)
                            newNode->addOp(output[i]);
                        for (int i = 0; i < a; ++i)
                            output.erase(output.end() - 1);

                        output.push_back(newNode);

                        stck.pop();
                    }
                    //todo:	invalid expression
                }
                break;
        }
    }

    while (!stck.empty()) {
        if (stck.top()->type == _bracket) {
            //todo:	invalid expression
        }
        // output.append(stck.top()->repr + " ");
        Node *leftOp, *rightOp;
        leftOp = output[output.size() - 2];
        rightOp = output[output.size() - 1];
        output.erase(output.end() - 1);
        output.erase(output.end() - 1);

        output.push_back(new BinaryOperator(stck.top()->repr, leftOp, rightOp));

        stck.pop();
    }

    root = output[0];

}

void Expression::setExpression(string &input) {
    vector <Token> tokens;

    tokenization(input, tokens);
    shuntingYardAlgorithm(tokens, rootNode);
}
void Expression::setExpression(Node *node) {
    rootNode = node;
}
void Expression::setVariable(string name, double value) {
    var[name] = value;
}
double Expression::getValue() {
    return rootNode->calculate(var);
}
