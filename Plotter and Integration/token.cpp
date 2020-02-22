#include <string>
using namespace std;

enum tokenType {
    _number, _function, _delimiter,
    _operator, _bracket, _variable
};

/*
Types of tokens:
    number
    function
    delimiter of function arguments (for example, comma)
    operator
    bracket
    variable (only latin letters)
*/

enum operatorAssociativity {
    _leftAssociativity,
    _rightAssociativity
};
enum operatorPriority {
    _plusPriority = -4, _minusPriority = -4,
    _multiplyPriority = -3, _dividePriority = -3,
    _exponentPriority = -1
};

struct Token {
    string repr;
    tokenType type;
    operatorAssociativity associativity;
    operatorPriority priority;
    int functionArgumentsCount;

    Token() {}
    Token(string repr, tokenType type) : repr(repr), type(type) {
        if (type == _operator) {
            if (repr == "+") {
                associativity = _leftAssociativity;
                priority = _plusPriority;
            }
            else if (repr == "-") {
                associativity = _leftAssociativity;
                priority = _minusPriority;
            }
            else if (repr == "*") {
                associativity = _leftAssociativity;
                priority = _multiplyPriority;
            }
            else if (repr == "/") {
                associativity = _leftAssociativity;
                priority = _dividePriority;
            }
            else if (repr == "^") {
                associativity = _rightAssociativity;
                priority = _exponentPriority;
            }
        }
    }

    string getType() {
        switch (type) {
            case _number:
                return "_number";
            case _function:
                return "_function";
            case _delimiter:
                return "_delimiter";
            case _operator:
                return "_operator";
            case _bracket:
                return "_bracket";
            case _variable:
                return "_variable";
        }
    }
};
