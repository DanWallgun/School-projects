#ifndef RENDERAREA_H
#define RENDERAREA_H

#include <QWidget>
#include <QPainter>

#include <vector>
#include <sstream>
#include <iomanip>

#include "Expression.h"

class RenderArea : public QWidget {
    Q_OBJECT
public:
    explicit RenderArea(QWidget *parent = nullptr);

    void addFunction(double (*f)(double), double, double);
    void addExpression(string expr, double a, double b);
    std::string toString(double, int precision = 2);
signals:

public slots:
    void moveViewportLeft();
    void moveViewportRight();
    void zoomInViewport();
    void zoomOutViewport();
protected:
    void paintEvent(QPaintEvent *event) override;

private:
    const double eps = 1e-6;

    int leftOffset, downOffset;
    std::pair<int, int> leftTextOffset, downTextOffset;
    double dx, dy;
    double shift;
    std::pair <double, double> xBorder, yBorder;
    std::pair <int, int> gridSize;
public:
    std::vector <Expression> exprFunctions;
    std::vector <std::pair<double, double>> exprBorders;
    std::vector <double (*)(double)> functions;
    std::vector <std::pair<double, double>> functionsBorders;

private:
    void calculate(int);
    void drawPlotOnPixmap(int);
    void calculateExpr(int);
    void drawExprOnPixmap(int);
    void drawGrid();
};

#endif // RENDERAREA_H
