#include "window.h"

Window::Window(QWidget *parent)
    : QWidget(parent) {

    mainLayout = new QHBoxLayout;
    plotView = new RenderArea;

    toolWidget = new QWidget;
    bordersEditLayout = new QHBoxLayout;
    toolLayout = new QVBoxLayout;
    controlsLayout = new QGridLayout;
    splitter = new QSplitter;
    gridCheckBox = new QCheckBox;
    expressionLineEdit = new QLineEdit;
    leftXBorderLineEdit = new QLineEdit;
    rightXBorderLineEdit = new QLineEdit;
    addPlotButton = new QPushButton;
    zoomInButton = new QPushButton;
    zoomOutButton = new QPushButton;
    moveLeftButton = new QPushButton;
    moveRightButton = new QPushButton;

    /*upd*/
    solveBordersEditLayout = new QHBoxLayout;
    integralBordersEditLayout = new QHBoxLayout;
    solveOnIntervalLabel = new QLabel;
    integrateOnIntervalLabel = new QLabel;
    solveOnIntervalButton = new QPushButton;
    integrateOnIntervalButton = new QPushButton;
    leftIntegralBorderLineEdit = new QLineEdit;
    rightIntegralBorderLineEdit = new QLineEdit;
    leftSolveBorderLineEdit = new QLineEdit;
    rightSolveBorderLineEdit = new QLineEdit;
    solutionLabel = new QLabel;
    integralLabel = new QLabel;
    integrateExpressionLineEdit = new QLineEdit;
    solveExpressionLineEdit = new QLineEdit;
    removePlotButton = new QPushButton;
    /**/
    integrateExpression = new Expression;
    solveExpression = new Expression;
    /**/

    mainLayout->addWidget(splitter);
    splitter->addWidget(plotView);
    splitter->addWidget(toolWidget);
    toolWidget->setLayout(toolLayout);
    //toolLayout->addWidget(gridCheckBox);
    toolLayout->addWidget(expressionLineEdit);
    toolLayout->addLayout(bordersEditLayout);
    bordersEditLayout->addWidget(leftXBorderLineEdit);
    bordersEditLayout->addWidget(rightXBorderLineEdit);
    toolLayout->addWidget(addPlotButton);
    toolLayout->addWidget(removePlotButton);
    toolLayout->addLayout(controlsLayout);
    controlsLayout->addWidget(moveLeftButton, 0, 0);
    controlsLayout->addWidget(moveRightButton, 0, 1);
    controlsLayout->addWidget(zoomOutButton, 1, 0);
    controlsLayout->addWidget(zoomInButton, 1, 1);

    /*upd*/
    toolLayout->addWidget(integrateOnIntervalLabel);
    toolLayout->addWidget(integrateExpressionLineEdit);
    toolLayout->addLayout(integralBordersEditLayout);
    integralBordersEditLayout->addWidget(leftIntegralBorderLineEdit);
    integralBordersEditLayout->addWidget(rightIntegralBorderLineEdit);
    toolLayout->addWidget(integrateOnIntervalButton);
    toolLayout->addWidget(integralLabel);

    toolLayout->addWidget(solveOnIntervalLabel);
    toolLayout->addWidget(solveExpressionLineEdit);
    toolLayout->addLayout(solveBordersEditLayout);
    solveBordersEditLayout->addWidget(leftSolveBorderLineEdit);
    solveBordersEditLayout->addWidget(rightSolveBorderLineEdit);
    toolLayout->addWidget(solveOnIntervalButton);
    toolLayout->addWidget(solutionLabel);
    /**/

    /**/
    QFont font;
    font.setFamily("Times New Roman");
    font.setPixelSize(20);
    /**/

    splitter->setSizes({size().width() * 2 / 3, size().width() / 3});
    toolLayout->setAlignment(Qt::AlignTop);
    gridCheckBox->setText("Coordinaeqte Grid");
    gridCheckBox->setFont(font);
    expressionLineEdit->setPlaceholderText("Expression");
    expressionLineEdit->setFont(font);
    leftXBorderLineEdit->setPlaceholderText("Left x-border");
    leftXBorderLineEdit->setFont(font);
    //leftXBorderLineEdit->setInputMask("9999");
    rightXBorderLineEdit->setPlaceholderText("Right x-border");
    rightXBorderLineEdit->setFont(font);
    //rightXBorderLineEdit->setInputMask("9999");
    addPlotButton->setText("Add function graph");
    addPlotButton->setFont(font);
    removePlotButton->setText("Remove function graph");
    removePlotButton->setFont(font);
    moveLeftButton->setText("<");
    moveLeftButton->setFont(font);
    moveRightButton->setText(">");
    moveRightButton->setFont(font);
    zoomOutButton->setText("-");
    zoomOutButton->setFont(font);
    zoomInButton->setText("+");
    zoomInButton->setFont(font);

    /*upd*/
    leftIntegralBorderLineEdit->setPlaceholderText("Left x-border");
    leftIntegralBorderLineEdit->setFont(font);
    leftSolveBorderLineEdit->setPlaceholderText("Left x-border");
    leftSolveBorderLineEdit->setFont(font);
    rightIntegralBorderLineEdit->setPlaceholderText("Right x-border");
    rightIntegralBorderLineEdit->setFont(font);
    rightSolveBorderLineEdit->setPlaceholderText("Right x-border");
    rightSolveBorderLineEdit->setFont(font);
    solveOnIntervalLabel->setText("Solve the equation on the interval");
    solveOnIntervalLabel->setFont(font);
    integrateOnIntervalLabel->setText("Integrate the function on the interval");
    integrateOnIntervalLabel->setFont(font);
    solveOnIntervalButton->setText("Solve");
    solveOnIntervalButton->setFont(font);
    integrateOnIntervalButton->setText("Integrate");
    integrateOnIntervalButton->setFont(font);
    integralLabel->setText("Integral = ");
    integralLabel->setFont(font);
    solutionLabel->setText("x = ");
    solutionLabel->setFont(font);
    integrateExpressionLineEdit->setPlaceholderText("Expression");
    integrateExpressionLineEdit->setFont(font);
    solveExpressionLineEdit->setPlaceholderText("Expression");
    solveExpressionLineEdit->setFont(font);
    /**/
    integrateOnIntervalLabel->setFrameStyle(QFrame::Panel | QFrame::Raised);
    solveOnIntervalLabel->setFrameStyle(QFrame::Panel | QFrame::Raised);
    integralLabel->setFrameStyle(QFrame::Panel | QFrame::Raised);
    solutionLabel->setFrameStyle(QFrame::Panel | QFrame::Raised);
    /**/

    connect(moveLeftButton, SIGNAL(clicked(bool)), plotView, SLOT(moveViewportLeft()));
    connect(moveRightButton, SIGNAL(clicked(bool)), plotView, SLOT(moveViewportRight()));
    connect(zoomInButton, SIGNAL(clicked(bool)), plotView, SLOT(zoomInViewport()));
    connect(zoomOutButton, SIGNAL(clicked(bool)), plotView, SLOT(zoomOutViewport()));
    connect(addPlotButton, SIGNAL(clicked(bool)), this, SLOT(addPlotClicked()));

    /*upd*/
    connect(integrateOnIntervalButton, SIGNAL(clicked(bool)), this, SLOT(integrateFunctionClicked()));
    connect(solveOnIntervalButton, SIGNAL(clicked(bool)), this, SLOT(solveEquationClicked()));
    /**/
    connect(removePlotButton, SIGNAL(clicked(bool)), this, SLOT(removePlotClicked()));

    setLayout(mainLayout);
    setWindowTitle("Plotter");
    resize(1366, 768);
}

Window::~Window() {

}

void Window::removePlotClicked() {
    if (plotView->exprFunctions.size() > 0) {
        plotView->exprFunctions.erase(plotView->exprFunctions.end() - 1);
        plotView->exprBorders.erase(plotView->exprBorders.end() - 1);
    }
    plotView->repaint();
}

void Window::addPlotClicked() {
    plotView->addExpression(expressionLineEdit->text().toStdString(),
                            stod(leftXBorderLineEdit->text().toStdString()), stod(rightXBorderLineEdit->text().toStdString()));
    plotView->repaint();
}

double f3(double x) {
    return x * sin(x);
}

void Window::integrateFunctionClicked() {
    MathUtils mthUt;

    string tmp = integrateExpressionLineEdit->text().toStdString();

    integrateExpression->setExpression(tmp);

    tmp = "Integral = " + plotView->toString(mthUt.integrate(integrateExpression,
                                 stod(leftIntegralBorderLineEdit->text().toStdString()),
                                 stod(rightIntegralBorderLineEdit->text().toStdString()), 1e-6), 6);
    integralLabel->setText(tmp.c_str());
}

void Window::solveEquationClicked() {
    MathUtils mthUt;

    string tmp = solveExpressionLineEdit->text().toStdString();

    solveExpression->setExpression(tmp);

    tmp = "x = " + plotView->toString(mthUt.solve(solveExpression,
                              stod(leftSolveBorderLineEdit->text().toStdString()),
                              stod(rightSolveBorderLineEdit->text().toStdString()), 1e-6), 6);
    solutionLabel->setText(tmp.c_str());
}
