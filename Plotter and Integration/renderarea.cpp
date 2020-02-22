#include "renderarea.h"

#include <cmath>
#include <iostream>

double f1(double x) {
    return x * sin(x);
}

double f2(double x) {
    return sqrt(x);
}

RenderArea::RenderArea(QWidget *parent) : QWidget(parent) {
    leftOffset = 50;
    downOffset = 50;
    leftTextOffset = {15, 5};   //in x(first) and y(second),
    downTextOffset = {-12, 15}; //respectively

    dx = 0.05, dy = 0.05;

    xBorder = {1e15, -1e15};
    yBorder = {1e15, -1e15};

    gridSize = {11, 9};

    //addFunction(&f1, -5.0, 5.0);
    //addFunction(&f2, 0.0, 5.0);

    //addExpression("1/x*sin(x)", -5.0, 5.0);
}

void RenderArea::paintEvent(QPaintEvent *) {
    if (functions.size() == 0 && exprFunctions.size() == 0) {
        xBorder = {-10, 10};
        yBorder = {-10, 10};

        dx = std::abs(xBorder.second - xBorder.first) / (size().width() - leftOffset);
        dy = std::abs(yBorder.second - yBorder.first) / (size().height() - downOffset);
    }
    else {
        xBorder = {1e15, -1e15};
        yBorder = {1e15, -1e15};
    }
    for (size_t i = 0; i < functions.size(); ++i)
        calculate(i);
    for (size_t i = 0; i < exprFunctions.size(); ++i)
        calculateExpr(i);
    drawGrid();
    for (size_t i = 0; i < functions.size(); ++i)
        drawPlotOnPixmap(i);
    for (size_t i = 0; i < exprFunctions.size(); ++i)
        drawExprOnPixmap(i);
}

void RenderArea::addExpression(string expr, double a, double b) {
    exprFunctions.push_back(Expression());
    exprFunctions[exprFunctions.size() - 1].setExpression(expr);
    exprBorders.push_back(std::make_pair(a, b));
}

void RenderArea::addFunction(double (*f)(double), double a, double b) {
    functions.push_back(f);
    functionsBorders.push_back(std::make_pair(a, b));
}

void RenderArea::moveViewportLeft() {
    for (size_t i = 0; i < functionsBorders.size(); ++i) {
        if (std::abs(xBorder.first - functionsBorders[i].first) <= eps)
            functionsBorders[i].first -= shift;
        if (std::abs(xBorder.second - functionsBorders[i].second) <= eps)
            functionsBorders[i].second -= shift;
    }
    for (size_t i = 0; i < exprBorders.size(); ++i) {
        if (std::abs(xBorder.first - exprBorders[i].first) <= eps)
            exprBorders[i].first -= shift;
        if (std::abs(xBorder.second - exprBorders[i].second) <= eps)
            exprBorders[i].second -= shift;
    }
    xBorder.first -= shift;
    xBorder.second -= shift;

    repaint();
}

void RenderArea::moveViewportRight() {
    for (size_t i = 0; i < functionsBorders.size(); ++i) {
        if (std::abs(xBorder.first - functionsBorders[i].first) <= eps)
            functionsBorders[i].first += shift;
        if (std::abs(xBorder.second - functionsBorders[i].second) <= eps)
            functionsBorders[i].second += shift;
    }
    for (size_t i = 0; i < exprBorders.size(); ++i) {
        if (std::abs(xBorder.first - exprBorders[i].first) <= eps)
            exprBorders[i].first += shift;
        if (std::abs(xBorder.second - exprBorders[i].second) <= eps)
            exprBorders[i].second += shift;
    }
    xBorder.first += shift;
    xBorder.second += shift;

    repaint();
}

void RenderArea::zoomInViewport() {
    for (size_t i = 0; i < functionsBorders.size(); ++i) {
        if (std::abs(xBorder.first - functionsBorders[i].first) <= eps)
            functionsBorders[i].first += shift / 2;
        if (std::abs(xBorder.second - functionsBorders[i].second) <= eps)
            functionsBorders[i].second -= shift / 2;
    }
    for (size_t i = 0; i < exprBorders.size(); ++i) {
        if (std::abs(xBorder.first - exprBorders[i].first) <= eps)
            exprBorders[i].first += shift / 2;
        if (std::abs(xBorder.second - exprBorders[i].second) <= eps)
            exprBorders[i].second -= shift / 2;
    }
    xBorder.first += shift / 2;
    xBorder.second -= shift / 2;

    repaint();
}

void RenderArea::zoomOutViewport() {
    for (size_t i = 0; i < functionsBorders.size(); ++i) {
        if (std::abs(xBorder.first - functionsBorders[i].first) <= eps)
            functionsBorders[i].first -= shift / 2;
        if (std::abs(xBorder.second - functionsBorders[i].second) <= eps)
            functionsBorders[i].second += shift / 2;
    }
    for (size_t i = 0; i < exprBorders.size(); ++i) {
        if (std::abs(xBorder.first - exprBorders[i].first) <= eps)
            exprBorders[i].first -= shift / 2;
        if (std::abs(xBorder.second - exprBorders[i].second) <= eps)
            exprBorders[i].second += shift / 2;
    }
    xBorder.first -= shift / 2;
    xBorder.second += shift / 2;

    repaint();
}

void RenderArea::calculate(int ind) {
    if (functionsBorders[ind].first < xBorder.first)
        xBorder.first = functionsBorders[ind].first;
    if (functionsBorders[ind].second > xBorder.second)
        xBorder.second = functionsBorders[ind].second;

    if (std::abs(xBorder.first - xBorder.second) <= eps)
        xBorder.second += eps;

    dx = std::abs(xBorder.second - xBorder.first) / (size().width() - leftOffset);

    for (int xm = 0; xm < size().width() - leftOffset; ++xm) {
        double xr = xBorder.first + (xm - 0) * dx;

        if (std::isinf(functions[ind](xr)) || std::isnan(functions[ind](xr)))
            continue;

        if (functions[ind](xr) > yBorder.second)
            yBorder.second = functions[ind](xr);
        if (functions[ind](xr) < yBorder.first)
            yBorder.first = functions[ind](xr);
    }

    dy = std::abs(yBorder.second - yBorder.first) / (size().height() - downOffset);

    shift = 2 * ((size().width() - leftOffset) / gridSize.first) * dx;
}

void RenderArea::calculateExpr(int ind) {
    if (exprBorders[ind].first < xBorder.first)
        xBorder.first = exprBorders[ind].first;
    if (exprBorders[ind].second > xBorder.second)
        xBorder.second = exprBorders[ind].second;

    if (std::abs(xBorder.first - xBorder.second) <= eps)
        xBorder.second += eps;

    dx = std::abs(xBorder.second - xBorder.first) / (size().width() - leftOffset);

    for (double xr = exprBorders[ind].first; xr < exprBorders[ind].second; xr += dx) {
        //double xr = xBorder.first + (xm - 0) * dx;

        exprFunctions[ind].setVariable("x", xr);

        if (std::isinf(exprFunctions[ind].getValue()) || std::isnan(exprFunctions[ind].getValue()))
            continue;

        if (exprFunctions[ind].getValue() > yBorder.second)
            yBorder.second = exprFunctions[ind].getValue();
        if (exprFunctions[ind].getValue() < yBorder.first)
            yBorder.first = exprFunctions[ind].getValue();
    }

    dy = std::abs(yBorder.second - yBorder.first) / (size().height() - downOffset);

    shift = 2 * ((size().width() - leftOffset) / gridSize.first) * dx;
}

void RenderArea::drawGrid() {
    QPainter painter(this);
    QPen pen;

    pen.setColor(Qt::blue);
    pen.setStyle(Qt::DashLine);
    pen.setWidth(0);

    painter.setPen(pen);

    double startFrom;
    double cellWidth = ((size().width() - leftOffset) / gridSize.first) * dx;

    if (0 < xBorder.first) {
        double startFrom = cellWidth - (xBorder.first - cellWidth * (int)(xBorder.first / cellWidth));
        for (int i = startFrom / dx + leftOffset; i < size().width(); i += (size().width() - leftOffset) / gridSize.first, startFrom += cellWidth) {
            painter.drawText(i + downTextOffset.first, size().height() - downOffset + downTextOffset.second, (toString(round((xBorder.first + startFrom) * 100.0) / 100.0)).c_str());

            painter.drawLine(i, 0, i, size().height() - downOffset);
        }
    }
    else if (xBorder.second < 0) {
        startFrom = cellWidth - (std::abs(xBorder.second) - cellWidth * (int)(std::abs(xBorder.second) / cellWidth));
        startFrom = xBorder.second - startFrom;
        for (int i = std::abs(startFrom - xBorder.first) / dx + leftOffset; i > leftOffset; i -= (size().width() - leftOffset) / gridSize.first, startFrom -= cellWidth) {
            painter.drawText(i + downTextOffset.first, size().height() - downOffset + downTextOffset.second, (toString(round((startFrom) * 100.0) / 100.0)).c_str());

            painter.drawLine(i, 0, i, size().height() - downOffset);
        }
    }
    else {
        startFrom = 0;
        int i = std::abs(xBorder.first) / dx + leftOffset;

        pen.setStyle(Qt::SolidLine);
        pen.setWidth(2);
        pen.setColor(QColor(255, 127, 80));
        painter.setPen(pen);

        painter.drawText(i + downTextOffset.first, size().height() - downOffset + downTextOffset.second, (toString(round((startFrom) * 100.0) / 100.0)).c_str());
        painter.drawLine(i, 0, i, size().height() - downOffset);

        pen.setStyle(Qt::DashLine);
        pen.setWidth(0);
        pen.setColor(Qt::blue);
        painter.setPen(pen);

        for (i += (size().width() - leftOffset) / gridSize.first, startFrom += cellWidth;
             i < size().width(); i += (size().width() - leftOffset) / gridSize.first, startFrom += cellWidth) {
            painter.drawText(i + downTextOffset.first, size().height() - downOffset + downTextOffset.second, (toString(round((startFrom) * 100.0) / 100.0)).c_str());

            painter.drawLine(i, 0, i, size().height() - downOffset);
        }

        startFrom = 0;
        i = std::abs(xBorder.first) / dx + leftOffset;

        for (i -= (size().width() - leftOffset) / gridSize.first, startFrom -= cellWidth;
             i > leftOffset; i -= (size().width() - leftOffset) / gridSize.first, startFrom -= cellWidth) {
            painter.drawText(i + downTextOffset.first, size().height() - downOffset + downTextOffset.second, (toString(round((startFrom) * 100.0) / 100.0)).c_str());

            painter.drawLine(i, 0, i, size().height() - downOffset);
        }
    }

    cellWidth = ((size().height() - downOffset) / gridSize.second) * dy;

    if (0 < yBorder.first) {
        startFrom = cellWidth - (yBorder.first - cellWidth * (int)(yBorder.first / cellWidth));
        for (int i = size().height() - downOffset - startFrom / dy; i > 0; i -= (size().height() - downOffset) / gridSize.second, startFrom += cellWidth) {
            painter.drawText(leftTextOffset.first, i + leftTextOffset.second, (toString(round((yBorder.first + startFrom) * 100.0) / 100.0)).c_str());

            painter.drawLine(leftOffset, i, size().width(), i);
        }
    }
    else if (yBorder.second < 0) {
        startFrom = cellWidth - (std::abs(yBorder.second) - cellWidth * (int)(std::abs(yBorder.second) / cellWidth));
        startFrom = yBorder.second - startFrom;
        for (int i = size().height() - downOffset - std::abs(startFrom - yBorder.first) / dy; i < size().height() - downOffset;
             i += (size().height() - downOffset) / gridSize.second, startFrom -= cellWidth) {
            painter.drawText(leftTextOffset.first, i + leftTextOffset.second, (toString(round((startFrom) * 100.0) / 100.0)).c_str());

            painter.drawLine(leftOffset, i, size().width(), i);
        }
    }
    else {
        startFrom = 0;
        int i = size().height() - downOffset - std::abs(yBorder.first) / dy;

        pen.setStyle(Qt::SolidLine);
        pen.setWidth(2);
        pen.setColor(QColor(255, 127, 80));
        painter.setPen(pen);

        painter.drawText(leftTextOffset.first, i + leftTextOffset.second, (toString(round((startFrom) * 100.0) / 100.0)).c_str());

        painter.drawLine(leftOffset, i, size().width(), i);

        pen.setStyle(Qt::DashLine);
        pen.setWidth(0);
        pen.setColor(Qt::blue);
        painter.setPen(pen);

        for (i += (size().height() - downOffset) / gridSize.second, startFrom -= cellWidth;
             i < size().height() - downOffset; i += (size().height() - downOffset) / gridSize.second, startFrom -= cellWidth) {
            painter.drawText(leftTextOffset.first, i + leftTextOffset.second, (toString(round((startFrom) * 100.0) / 100.0)).c_str());

            painter.drawLine(leftOffset, i, size().width(), i);
        }

        startFrom = 0;
        i = size().height() - downOffset - std::abs(yBorder.first) / dy;

        for (i -= (size().height() - downOffset) / gridSize.second, startFrom += cellWidth;
             i > 0; i -= (size().height() - downOffset) / gridSize.second, startFrom += cellWidth) {
            painter.drawText(leftTextOffset.first, i + leftTextOffset.second, (toString(round((startFrom) * 100.0) / 100.0)).c_str());

            painter.drawLine(leftOffset, i, size().width(), i);
        }
    }
}

void RenderArea::drawPlotOnPixmap(int ind) {
    QPainter painter(this);
    QPainterPath path;
    QPen pen;

    pen.setColor(Qt::black);
    painter.setPen(pen);

    double xr = functionsBorders[ind].first;
    bool flag = false;

    if (std::isinf(functions[ind](xr)) || std::isnan(functions[ind](xr)))
        flag = true;
    else {
        path.moveTo(std::abs(functionsBorders[ind].first - xBorder.first) / dx + leftOffset,
                    size().height() - downOffset - (functions[ind](xr) - yBorder.first) / dy);
        flag = false;
    }

    for (int xm = 1 + leftOffset + std::abs(functionsBorders[ind].first - xBorder.first) / dx;
         xm < leftOffset + std::abs(functionsBorders[ind].second - xBorder.first) / dx; ++xm) {
        xr = xBorder.first + dx * (xm - leftOffset);

        if (std::isinf(functions[ind](xr)) || std::isnan(functions[ind](xr)))
            flag = true;
        else {
            if (flag)
                path.moveTo(xm, size().height() - downOffset - (functions[ind](xr) - yBorder.first) / dy );
            else
                path.lineTo(xm, size().height() - downOffset - (functions[ind](xr) - yBorder.first) / dy );
            flag = false;
        }
    }

    painter.drawPath(path);
}

void RenderArea::drawExprOnPixmap(int ind) {
    QPainter painter(this);
    QPainterPath path;
    QPen pen;

    pen.setColor(Qt::black);
    painter.setPen(pen);

    double xr = exprBorders[ind].first;
    bool flag = false;

    exprFunctions[ind].setVariable("x", xr);

    if (std::isinf(exprFunctions[ind].getValue()) || std::isnan(exprFunctions[ind].getValue()))
        flag = true;
    else {
        path.moveTo(std::abs(exprBorders[ind].first - xBorder.first) / dx + leftOffset,
                    size().height() - downOffset - (exprFunctions[ind].getValue() - yBorder.first) / dy);
        flag = false;
    }

    for (int xm = 1 + leftOffset + std::abs(exprBorders[ind].first - xBorder.first) / dx;
         xm < leftOffset + std::abs(exprBorders[ind].second - xBorder.first) / dx; ++xm) {
        xr = xBorder.first + dx * (xm - leftOffset);

        exprFunctions[ind].setVariable("x", xr);

        if (std::isinf(exprFunctions[ind].getValue()) || std::isnan(exprFunctions[ind].getValue()))
            flag = true;
        else {
            if (flag)
                path.moveTo(xm, size().height() - downOffset - (exprFunctions[ind].getValue() - yBorder.first) / dy );
            else
                path.lineTo(xm, size().height() - downOffset - (exprFunctions[ind].getValue() - yBorder.first) / dy );
            flag = false;
        }
    }

    painter.drawPath(path);
}

/*  utils   */
std::string RenderArea::toString(double x, int precision) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(precision);
    ss << x;
    return ss.str();
}
/**/
