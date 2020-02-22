#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QGridLayout>
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

#include <renderarea.h>
#include <mathutils.h>

class Window : public QWidget {
    Q_OBJECT

public:
    Window(QWidget *parent = 0);
    ~Window();

private:   
    QWidget *toolWidget;
    QHBoxLayout *mainLayout,
                *bordersEditLayout,
                *solveBordersEditLayout,
                *integralBordersEditLayout;
    QGridLayout *controlsLayout;
    QVBoxLayout *toolLayout;
    QSplitter *splitter;
    QCheckBox *gridCheckBox;
    QLineEdit *expressionLineEdit,
              *integrateExpressionLineEdit,
              *solveExpressionLineEdit,
              *leftXBorderLineEdit,
              *rightXBorderLineEdit,
              *leftSolveBorderLineEdit,
              *rightSolveBorderLineEdit,
              *leftIntegralBorderLineEdit,
              *rightIntegralBorderLineEdit;
    QPushButton *addPlotButton,
                *removePlotButton,
                *zoomInButton,
                *zoomOutButton,
                *moveLeftButton,
                *moveRightButton,
                *solveOnIntervalButton,
                *integrateOnIntervalButton;
    QLabel *solveOnIntervalLabel,
           *integrateOnIntervalLabel,
           *solutionLabel,
           *integralLabel;
    RenderArea *plotView;

    Expression *integrateExpression,
               *solveExpression;
private slots:
    void removePlotClicked();
    void addPlotClicked();
    void solveEquationClicked();
    void integrateFunctionClicked();

};

#endif // WINDOW_H
