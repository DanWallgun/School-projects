#ifndef MATHUTILS_H
#define MATHUTILS_H

#include "expression.h"

class MathUtils
{
public:
    MathUtils();

    double _integrate_ (Expression *expr, double a, double b, int n, string variable = "x");
    double integrate (Expression *expr, double a, double b, double eps, string variable = "x");
    double solve (Expression *expr, double a, double b, double eps);
};

#endif // MATHUTILS_H
