#include "mainwindow.h"
#include "matrix.cpp"
#include "imageprocessing.cpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    imageLabel = new QLabel();
    imageLabel->setScaledContents(true);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    scrollArea = new QScrollArea();
    scrollArea->setWidget(imageLabel);

    chooseFilterBox = new QComboBox();
    chooseFilterBox->setEnabled(false);
    chooseFilterBox->addItem("Normal");
    chooseFilterBox->addItem("Convolution");
    chooseFilterBox->addItem("Laplasian");
    chooseFilterBox->addItem("Laplasian (Sharpening)");
    chooseFilterBox->addItem("Gaussian");
    chooseFilterBox->addItem("Median");
    chooseFilterBox->addItem("Sobel");
    chooseFilterBox->addItem("Unsharp masking");
    connect(chooseFilterBox, SIGNAL(currentIndexChanged(int)), this, SLOT(filterChosen()));

    convMatTable = new QTableWidget(3, 3);
    convMatTable->horizontalHeader()->hide();
    convMatTable->verticalHeader()->hide();
    convMatTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    convMatTable->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    convMatTable->setVisible(false);

    stdevEdit = new QLineEdit();
    stdevEdit->setValidator(new QDoubleValidator(0, 100, 2, this));
    stdevEdit->setPlaceholderText("Standard Deviation");
    radiusEdit = new QLineEdit();
    radiusEdit->setValidator(new QIntValidator(0, 100, this));
    radiusEdit->setPlaceholderText("Radius");
    gaussianLayout = new QVBoxLayout();
    gaussianLayout->addWidget(stdevEdit);
    gaussianLayout->addWidget(radiusEdit);
    gaussianWidget = new QWidget();
    gaussianWidget->setLayout(gaussianLayout);
    gaussianWidget->setVisible(false);

    medianRadiusEdit = new QLineEdit();
    medianRadiusEdit->setValidator(new QIntValidator(0, 100, this));
    medianRadiusEdit->setPlaceholderText("Radius");
    medianRadiusEdit->setVisible(false);

    rewrite = new QCheckBox();
    rewrite->setText("Rewrite");
    rewrite->setEnabled(false);

    apply = new QPushButton();
    apply->setText("Apply");
    apply->setEnabled(false);
    connect(apply, SIGNAL(clicked()), this, SLOT(applyClicked()));

    toolLayout = new QVBoxLayout();
    toolLayout->addWidget(chooseFilterBox, 0, Qt::AlignTop);
    toolLayout->addWidget(convMatTable, 0, Qt::AlignTop);
    toolLayout->addWidget(gaussianWidget, 0, Qt::AlignTop);
    toolLayout->addWidget(medianRadiusEdit, 0, Qt::AlignTop);
    toolLayout->addWidget(rewrite, 0, Qt::AlignBottom);
    toolLayout->addWidget(apply, 0, Qt::AlignBottom);

    mainLayout = new QHBoxLayout();
    mainLayout->addWidget(scrollArea, 5);
    mainLayout->addLayout(toolLayout, 1);

    mainWidget = new QWidget();
    mainWidget->setLayout(mainLayout);

    setCentralWidget(mainWidget);

    createMenuBar();

    resize(QGuiApplication::primaryScreen()->availableSize() * 4 / 6);
}

MainWindow::~MainWindow() {

}

void MainWindow::filterChosen() {
    convMatTable->setVisible(chooseFilterBox->currentText() == "Convolution");
    gaussianWidget->setVisible(chooseFilterBox->currentText() == "Gaussian");
    medianRadiusEdit->setVisible(chooseFilterBox->currentText() == "Median");
}

void MainWindow::applyClicked() {
    QImage buffer;
    if (chooseFilterBox->currentText() == "Normal") {
        buffer = image;
    }
    else if (chooseFilterBox->currentText() == "Convolution") {
        Matrix<float> mat(convMatTable->columnCount(), convMatTable->rowCount());
        float div = 0;
        for (int i = 0; i < convMatTable->rowCount(); ++i) {
            for (int j = 0; j < convMatTable->columnCount(); ++j) {
                float k = convMatTable->item(i, j)->text().toFloat();
                mat(j, i) = k;
                div += k;
            }
        }
        if (div == 0.0f) div = 1;
        buffer = ImageProcessing::Convolution(image, mat, div);
    }
    else if (chooseFilterBox->currentText() == "Laplasian") {
        buffer = ImageProcessing::LaplasianFilter(image, false);
    }
    else if (chooseFilterBox->currentText() == "Laplasian (Sharpening)") {
        buffer = ImageProcessing::LaplasianFilter(image, true);
    }
    else if (chooseFilterBox->currentText() == "Gaussian") {
        float stdev = stdevEdit->text().toFloat();
        int rad = radiusEdit->text().toInt();
        buffer = ImageProcessing::GaussianBlur(image, stdev, rad);
    }
    else if (chooseFilterBox->currentText() == "Median") {
        int rad = medianRadiusEdit->text().toInt();
        buffer = ImageProcessing::MedianFilter(image, rad);
    }
    else if (chooseFilterBox->currentText() == "Sobel") {
        buffer = ImageProcessing::SobelFilter(image);
    }
    else if (chooseFilterBox->currentText() == "Unsharp masking") {
        buffer = ImageProcessing::UnsharpMasking(image);
    }
    if (rewrite->checkState() == Qt::Checked) {
        image = buffer;
    }
    imageLabel->setPixmap(QPixmap::fromImage(buffer));
}

static void initializeImageFileDialog(QFileDialog &dialog, QFileDialog::AcceptMode acceptMode) {
    static bool firstDialog = true;

    if (firstDialog) {
        firstDialog = false;
        const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
        dialog.setDirectory(picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.last());
    }

//    QStringList mimeTypeFilters;
//    const QByteArrayList supportedMimeTypes = acceptMode == QFileDialog::AcceptOpen
//        ? QImageReader::supportedMimeTypes() : QImageWriter::supportedMimeTypes();
//    foreach (const QByteArray &mimeTypeName, supportedMimeTypes)
//        mimeTypeFilters.append(mimeTypeName);
//    mimeTypeFilters.sort();
//    dialog.setMimeTypeFilters(mimeTypeFilters);
    QStringList nameFilters;
    nameFilters.append("Images (*.jpg; *.png; *.bmp)");
    nameFilters.append("All Files (*.*)");
    dialog.setNameFilters(nameFilters);
//    dialog.selectMimeTypeFilter("image/jpeg");
    if (acceptMode == QFileDialog::AcceptSave)
        dialog.setDefaultSuffix("jpg");
}

bool MainWindow::loadFile(const QString &fileName) {
    QImageReader reader(fileName);
    reader.setAutoTransform(true);
    const QImage newImage = reader.read();
    if (newImage.isNull()) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot load %1: %2")
                                 .arg(QDir::toNativeSeparators(fileName), reader.errorString()));
        return false;
    }

    setImage(newImage);

    setWindowFilePath(fileName);

    const QString message = tr("Opened \"%1\", %2x%3, Depth: %4")
        .arg(QDir::toNativeSeparators(fileName)).arg(image.width()).arg(image.height()).arg(image.depth());
    statusBar()->showMessage(message);
    return true;
}

void MainWindow::setImage(const QImage &newImage) {
//    fullsizeImage = newImage;
//    image = fullsizeImage.scaled(800, 600, Qt::KeepAspectRatio);

    image = newImage;

    imageLabel->setPixmap(QPixmap::fromImage(image));
    scaleFactor = 1.0;

    scrollArea->setVisible(true);
    fitToWindowAct->setEnabled(true);
    updateActions();

    chooseFilterBox->setEnabled(true);
    apply->setEnabled(true);
    rewrite->setEnabled(true);

    if (!fitToWindowAct->isChecked())
        imageLabel->adjustSize();
}

bool MainWindow::saveFile(const QString &fileName) {
    QImage savedImage;
    //
    if (chooseFilterBox->currentText() == "Normal") {
        savedImage = image;
    }
    else if (chooseFilterBox->currentText() == "Convolution") {
        Matrix<float> mat(convMatTable->columnCount(), convMatTable->rowCount());
        float div = 0;
        for (int i = 0; i < convMatTable->rowCount(); ++i) {
            for (int j = 0; j < convMatTable->columnCount(); ++j) {
                float k = convMatTable->item(i, j)->text().toFloat();
                mat(j, i) = k;
                div += k;
            }
        }
        if (div == 0.0f) div = 1;
        savedImage = ImageProcessing::Convolution(image, mat, div);
    }
    else if (chooseFilterBox->currentText() == "Laplasian") {
        savedImage = ImageProcessing::LaplasianFilter(image, false);
    }
    else if (chooseFilterBox->currentText() == "Laplasian (Sharpening)") {
        savedImage = ImageProcessing::LaplasianFilter(image, true);
    }
    else if (chooseFilterBox->currentText() == "Gaussian") {
        float stdev = stdevEdit->text().toFloat();
        int rad = radiusEdit->text().toInt();
        savedImage = ImageProcessing::GaussianBlur(image, stdev, rad);
    }
    else if (chooseFilterBox->currentText() == "Median") {
        int rad = medianRadiusEdit->text().toInt();
        savedImage = ImageProcessing::MedianFilter(image, rad);
    }
    else if (chooseFilterBox->currentText() == "Sobel") {
        savedImage = ImageProcessing::SobelFilter(image);
    }
    else if (chooseFilterBox->currentText() == "Unsharp masking") {
        savedImage = ImageProcessing::UnsharpMasking(image);
    }
    //

    QImageWriter writer(fileName);

    if (!writer.write(savedImage)) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot write %1: %2")
                                 .arg(QDir::toNativeSeparators(fileName)), writer.errorString());
        return false;
    }
    const QString message = tr("Wrote \"%1\"").arg(QDir::toNativeSeparators(fileName));
    statusBar()->showMessage(message);
    return true;
}

void MainWindow::open() {
    QFileDialog dialog(this, tr("Open File"));
    initializeImageFileDialog(dialog, QFileDialog::AcceptOpen);

    while (dialog.exec() == QDialog::Accepted && !loadFile(dialog.selectedFiles().first())) {}
}

void MainWindow::saveAs() {
    QFileDialog dialog(this, tr("Save File As"));
    initializeImageFileDialog(dialog, QFileDialog::AcceptSave);

    while (dialog.exec() == QDialog::Accepted && !saveFile(dialog.selectedFiles().first())) {}
}

void MainWindow::fitToWindow() {
    bool fitToWindow = fitToWindowAct->isChecked();
    scrollArea->setWidgetResizable(fitToWindow);
    if (!fitToWindow)
        normalSize();
    updateActions();
}

void MainWindow::updateActions() {
    saveAsAct->setEnabled(!image.isNull());
    zoomInAct->setEnabled(!fitToWindowAct->isChecked());
    zoomOutAct->setEnabled(!fitToWindowAct->isChecked());
    normalSizeAct->setEnabled(!fitToWindowAct->isChecked());
}

void MainWindow::adjustScrollBar(QScrollBar *scrollBar, double factor) {
    scrollBar->setValue(int(factor * scrollBar->value()
                            + ((factor - 1) * scrollBar->pageStep()/2)));
}

void MainWindow::scaleImage(double factor) {
    Q_ASSERT(imageLabel->pixmap());
    scaleFactor *= factor;
    imageLabel->resize(scaleFactor * imageLabel->pixmap()->size());

    adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(scrollArea->verticalScrollBar(), factor);

    zoomInAct->setEnabled(scaleFactor < 3.0);
    zoomOutAct->setEnabled(scaleFactor > 0.333);
}

void MainWindow::zoomIn() {
    scaleImage(1.25);
}

void MainWindow::zoomOut() {
    scaleImage(0.8);
}

void MainWindow::normalSize() {
    imageLabel->adjustSize();
    scaleFactor = 1.0;
}

void MainWindow::createMenuBar() {
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

    QAction *openAct = fileMenu->addAction(tr("&Open..."), this, &MainWindow::open);
    openAct->setShortcut(QKeySequence::Open);

    saveAsAct = fileMenu->addAction(tr("&Save As..."), this, &MainWindow::saveAs);
    saveAsAct->setEnabled(false);
    saveAsAct->setShortcut(QKeySequence::Save);

    fileMenu->addSeparator();

    QAction *exitAct = fileMenu->addAction(tr("E&xit"), this, &QWidget::close);
    exitAct->setShortcut(tr("Ctrl+Q"));

    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));

    zoomInAct = viewMenu->addAction(tr("Zoom &In (25%)"), this, &MainWindow::zoomIn);
    zoomInAct->setShortcut(QKeySequence::ZoomIn);
    zoomInAct->setEnabled(false);

    zoomOutAct = viewMenu->addAction(tr("Zoom &Out (25%)"), this, &MainWindow::zoomOut);
    zoomOutAct->setShortcut(QKeySequence::ZoomOut);
    zoomOutAct->setEnabled(false);

    normalSizeAct = viewMenu->addAction(tr("&Normal Size"), this, &MainWindow::normalSize);
    normalSizeAct->setShortcut(tr("Ctrl+N"));
    normalSizeAct->setEnabled(false);

    viewMenu->addSeparator();

    fitToWindowAct = viewMenu->addAction(tr("&Fit to Window"), this, &MainWindow::fitToWindow);
    fitToWindowAct->setEnabled(false);
    fitToWindowAct->setCheckable(true);
    fitToWindowAct->setShortcut(tr("Ctrl+F"));
}
