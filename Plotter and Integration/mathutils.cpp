#include "mathutils.h"
#include <cmath>

MathUtils::MathUtils(){
}

double MathUtils::_integrate_ (Expression *expr, double a, double b, int n, string variable) {
    double h = (b - a) / n / 2;
    double result = 0;
    expr->setVariable(variable, a);
    result += expr->getValue();
    for (int i = 1; i <= n; ++i) {
        expr->setVariable(variable, a + (i * 2 - 1) * h);
        result += 4 * expr->getValue();
    }
    for (int i = 1; i <= n - 1; ++i) {
        expr->setVariable(variable, a + (i * 2) * h);
        result += 2 * expr->getValue();
    }
    expr->setVariable(variable, a + (n * 2) * h);
    result += expr->getValue();
    result *= h;
    result /= 3;
    return result;
}

double MathUtils::integrate (Expression *expr, double a, double b, double eps, string variable) {
    const double runge = 1.0 / 15.0;
    int n = 1024;
    double sn = _integrate_(expr, a, b, n, variable),
           s2n = _integrate_(expr, a, b, n * 2, variable);
    while (std::abs(s2n - sn) * runge >= eps) {
        sn = s2n;
        n *= 2;
        s2n = _integrate_(expr, a, b, n * 2, variable);
    }
    return s2n;
}

double MathUtils::solve (Expression *expr, double a, double b, double eps) {
    expr->setVariable("x", b);
    double fb = expr->getValue();
    expr->setVariable("x", a);
    double fa = expr->getValue();
    double c = (a * fb - b * fa) / (fb - fa);
    expr->setVariable("x", c);
    double fc = expr->getValue();
    expr->setVariable("x", c + eps);
    double fceps = expr->getValue();
    if ((fc < 0 && fa < fb) || (fc > 0 && fa > fb)) {
        while (fc * fceps > 0) {
            a = c;
            expr->setVariable("x", b);
            fb = expr->getValue();
            expr->setVariable("x", a);
            fa = expr->getValue();
            c = (a * fb - b * fa) / (fb - fa);

            expr->setVariable("x", c);
            fc = expr->getValue();
            expr->setVariable("x", c + eps);
            fceps = expr->getValue();
        }
    } else {
        while (fc * fceps > 0) {
            b = c;
            expr->setVariable("x", b);
            fb = expr->getValue();
            expr->setVariable("x", a);
            fa = expr->getValue();
            c = (a * fb - b * fa) / (fb - fa);

            expr->setVariable("x", c);
            fc = expr->getValue();
            expr->setVariable("x", c + eps);
            fceps = expr->getValue();
        }
    }
    return c;
}
