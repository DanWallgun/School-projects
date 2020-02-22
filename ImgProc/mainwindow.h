#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDebug>

#include <QMainWindow>

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QScrollArea>
#include <QGuiApplication>
#include <QScreen>
#include <QMenuBar>
#include <QApplication>
#include <QFileDialog>
#include <QStandardPaths>
#include <QImageWriter>
#include <QImageReader>
#include <QStatusBar>
#include <QMessageBox>
#include <QScrollBar>
#include <QLineEdit>
#include <QTableWidget>
#include <QHeaderView>
#include <QCheckBox>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    double scaleFactor;

    QImage image, fullsizeImage;

    QWidget *mainWidget;
    QHBoxLayout *mainLayout;
    QVBoxLayout *toolLayout;
    QComboBox *chooseFilterBox;
    QLabel *imageLabel;
    QScrollArea *scrollArea;

    QTableWidget *convMatTable;

    QWidget *gaussianWidget;
    QVBoxLayout *gaussianLayout;
    QLineEdit *stdevEdit, *radiusEdit;

    QLineEdit *medianRadiusEdit;

    QCheckBox *rewrite;
    QPushButton *apply;

    bool saveFile(const QString&);
    bool loadFile(const QString&);
    void setImage(const QImage&);

    void scaleImage(double);
    void adjustScrollBar(QScrollBar*, double);

    void createMenuBar();
    void updateActions();

    QAction *saveAsAct;
    QAction *printAct;
    QAction *copyAct;
    QAction *zoomInAct;
    QAction *zoomOutAct;
    QAction *normalSizeAct;
    QAction *fitToWindowAct;

private slots:
    void open();
    void saveAs();
//    void copy();
//    void paste();
    void zoomIn();
    void zoomOut();
    void normalSize();
    void fitToWindow();

    void filterChosen();
    void applyClicked();
};

#endif // MAINWINDOW_H
