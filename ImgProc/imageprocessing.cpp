#include <QImage>
#include <QRgb>
#include <qmath.h>
#include <QDebug>
#include <algorithm>
#include "matrix.cpp"

#include <vector>

namespace ImageProcessing {
    float horizontalSobelOperator[9] {
        -1, -2, -1,
        0, 0, 0,
        1, 2, 1
    };
    float verticalSobelOperator[9] {
        -1, 0, 1,
        -2, 0, 2,
        -1, 0, 1
    };
    float laplasianOperator[9] {
        -1, -1, -1,
        -1, 8, -1,
        -1, -1, -1
    };

    QImage Convolution(const QImage &image, const Matrix<float> &matrix, const float div) {
        QImage res(image.width(), image.height(), image.format());
        for (int i = 0; i < res.height(); ++i) {
            for (int j = 0; j < res.width(); ++j) {
                double r = 0, g = 0, b = 0;
                for (int di = 0, ni; di < matrix.height; ++di) {
                    // Translation into the coordinate system of the central (i, j) pixel
                    ni = i + di - matrix.height / 2 - 1 + (matrix.height & 1);
                    // Mirror addition of image borders
                    if (ni < 0)
                        ni = -(ni + 1);
                    else if (ni >= res.height())
                        ni = res.height() - (ni - (res.height() - 1));
                    for (int dj = 0, nj; dj < matrix.width; ++dj) {
                        // Translation into the coordinate system of the central (i, j) pixel
                        nj = j + dj - matrix.width / 2 - 1 + (matrix.width & 1);
                        // Mirror addition of image borders
                        if (nj < 0)
                            nj = -(nj + 1);
                        else if (nj >= res.width())
                            nj = res.width() - (nj - (res.width() - 1));

                        QRgb px = image.pixel(nj, ni);
                        r += qRed(px) * matrix(di, dj);
                        g += qGreen(px) * matrix(di, dj);
                        b += qBlue(px) * matrix(di, dj);
                    }
                }
//                qDebug() << r << " " << g << " " << b;
                r /= div;
                g /= div;
                b /= div;
                if (r < 0) r = 0;
                if (g < 0) g = 0;
                if (b < 0) b = 0;
                if (r > 255) r = 255;
                if (g > 255) g = 255;
                if (b > 255) b = 255;
                res.setPixel(j, i, qRgb(r, g, b));
            }
        }
        return res;
    }

    QImage MedianFilter(const QImage &image, const int radius) {
        QImage res(image.width(), image.height(), image.format());

        int h[4][256];

        for (int i = 0; i < res.height(); ++i) {
            for (int i = 0; i < 4; ++i)
                for (int j = 0; j < 256; ++j)
                    h[i][j] = 0;

            for (int di = -radius, ni; di <= radius; ++di) {
                ni = i + di;
                if (ni < 0)
                    ni = -(ni + 1);
                else if (ni >= res.height())
                    ni = res.height() - (ni - (res.height() - 1));
                for (int dj = -radius, nj; dj <= radius; ++dj) {
                    nj = 0 + dj;
                    if (nj < 0)
                        nj = -(nj + 1);
                    else if (nj >= res.width())
                        nj = res.width() - (nj - (res.width() - 1));

                    ++h[0][qBlue(image.pixel(nj, ni))];
                    ++h[1][qGreen(image.pixel(nj, ni))];
                    ++h[2][qRed(image.pixel(nj, ni))];
                    ++h[3][qAlpha(image.pixel(nj, ni))];
                }
            }

            QRgb median;
            int cnt[4] = {h[0][0], h[1][0], h[2][0], h[3][0]};

            for (int i = 1; i < 256; ++i) {
                if (cnt[0] > (radius * 2 + 1) * (radius * 2 + 1) / 2) median = qRgba(qRed(median), qGreen(median), i - 1, qAlpha(median)), cnt[0] = -1;
                else if (cnt[0] != -1) cnt[0] += h[0][i];
                if (cnt[1] > (radius * 2 + 1) * (radius * 2 + 1) / 2) median = qRgba(qRed(median), i - 1, qBlue(median), qAlpha(median)), cnt[1] = -1;
                else if (cnt[1] != -1) cnt[1] += h[1][i];
                if (cnt[2] > (radius * 2 + 1) * (radius * 2 + 1) / 2) median = qRgba(i - 1, qGreen(median), qBlue(median), qAlpha(median)), cnt[2] = -1;
                else if (cnt[2] != -1) cnt[2] += h[2][i];
                if (cnt[3] > (radius * 2 + 1) * (radius * 2 + 1) / 2) median = qRgba(qRed(median), qGreen(median), qBlue(median), i - 1), cnt[3] = -1;
                else if (cnt[3] != -1) cnt[3] += h[3][i];
            }
            if (cnt[0] != -1) median = qRgba(qRed(median), qGreen(median), 255, qAlpha(median));
            if (cnt[1] != -1) median = qRgba(qRed(median), 255, qBlue(median), qAlpha(median));
            if (cnt[2] != -1) median = qRgba(255, qGreen(median), qBlue(median), qAlpha(median));
            if (cnt[3] != -1) median = qRgba(qRed(median), qGreen(median), qBlue(median), 255);

            res.setPixel(0, i, median);

            for (int j = 1; j < res.width(); ++j) {

                for (int di = -radius, ni; di <= radius; ++di) {
                    ni = i + di;
                    if (ni < 0)
                        ni = -(ni + 1);
                    else if (ni >= res.height())
                        ni = res.height() - (ni - (res.height() - 1));

                    /*dj = -radius - 1*/ {
                        int nj = j - radius - 1;
                        if (nj < 0)
                            nj = -(nj + 1);
                        else if (nj >= res.width())
                            nj = res.width() - (nj - (res.width() - 1));

                        --h[0][qBlue(image.pixel(nj, ni))];
                        --h[1][qGreen(image.pixel(nj, ni))];
                        --h[2][qRed(image.pixel(nj, ni))];
                        --h[3][qAlpha(image.pixel(nj, ni))];
                    }

                    /*dj = radius*/ {
                        int nj = j + radius;
                        if (nj < 0)
                            nj = -(nj + 1);
                        else if (nj >= res.width())
                            nj = res.width() - (nj - (res.width() - 1));

                        ++h[0][qBlue(image.pixel(nj, ni))];
                        ++h[1][qGreen(image.pixel(nj, ni))];
                        ++h[2][qRed(image.pixel(nj, ni))];
                        ++h[3][qAlpha(image.pixel(nj, ni))];
                    }
                }

                median = qRgba(0, 0, 0, 0);
                cnt[0] = h[0][0];
                cnt[1] = h[1][0];
                cnt[2] = h[2][0];
                cnt[3] = h[3][0];

                for (int i = 1; i < 256; ++i) {
                    if (cnt[0] > (radius * 2 + 1) * (radius * 2 + 1) / 2) median = qRgba(qRed(median), qGreen(median), i - 1, qAlpha(median)), cnt[0] = -1;
                    else if (cnt[0] != -1) cnt[0] += h[0][i];
                    if (cnt[1] > (radius * 2 + 1) * (radius * 2 + 1) / 2) median = qRgba(qRed(median), i - 1, qBlue(median), qAlpha(median)), cnt[1] = -1;
                    else if (cnt[1] != -1) cnt[1] += h[1][i];
                    if (cnt[2] > (radius * 2 + 1) * (radius * 2 + 1) / 2) median = qRgba(i - 1, qGreen(median), qBlue(median), qAlpha(median)), cnt[2] = -1;
                    else if (cnt[2] != -1) cnt[2] += h[2][i];
                    if (cnt[3] > (radius * 2 + 1) * (radius * 2 + 1) / 2) median = qRgba(qRed(median), qGreen(median), qBlue(median), i - 1), cnt[3] = -1;
                    else if (cnt[3] != -1) cnt[3] += h[3][i];
                }
                if (cnt[0] != -1) median = qRgba(qRed(median), qGreen(median), 255, qAlpha(median));
                if (cnt[1] != -1) median = qRgba(qRed(median), 255, qBlue(median), qAlpha(median));
                if (cnt[2] != -1) median = qRgba(255, qGreen(median), qBlue(median), qAlpha(median));
                if (cnt[3] != -1) median = qRgba(qRed(median), qGreen(median), qBlue(median), 255);

                res.setPixel(j, i, median);
            }
        }

        return res;
    }

    QImage GaussianBlur(const QImage &image, const float stDev, int radius) {
        if (radius == -1) radius = qCeil(stDev * 3);

        const float PI = qAcos(-1);

        int width = radius * 2 + 1;

        float div = 0.0f;
        float gaussianMatrix[width];

        for (int i = -radius; i <= radius; ++i) {
            gaussianMatrix[i + radius] = exp(- (i * i) / (2 * stDev * stDev)) / (stDev * sqrt(2 * PI));
            div += gaussianMatrix[i + radius];
        }

        Matrix<float> matrix = Matrix<float>(gaussianMatrix, width, 1);
        QImage res = Convolution(image, matrix, div);
        matrix = Matrix<float>(gaussianMatrix, 1, width);
        res = Convolution(res, matrix, div);

        return res;
    }

    QImage HorizontalSobelFilter(const QImage &image) {
        Matrix<float> matrix(horizontalSobelOperator, 3, 3);
        return Convolution(image, matrix, 1.0f);
    }

    QImage LaplasianFilter(const QImage &image, bool sharpening = false) {
        Matrix<float> matrix(laplasianOperator, 3, 3);
        if (sharpening) {
            matrix(1, 1) += 1;
        }
        return Convolution(image, matrix, 1.0f);
    }

    QImage VerticalSobelFilter(const QImage &image) {
        Matrix<float> matrix(verticalSobelOperator, 3, 3);
        return Convolution(image, matrix, 1.0f);
    }

    QImage SobelFilter(const QImage &image) {
        QImage horizontalResult = ImageProcessing::HorizontalSobelFilter(image),
               verticalResult = ImageProcessing::VerticalSobelFilter(image);
        QImage result(image.width(), image.height(), image.format());

        for (int i = 0; i < result.height(); ++i) {
            for (int j = 0; j < result.width(); ++j) {
                int r1, g1, b1, a1;
                int r2, g2, b2, a2;
                QColor::fromRgb(verticalResult.pixel(j, i)).getRgb(&r1, &g1, &b1, &a1);
                QColor::fromRgb(horizontalResult.pixel(j, i)).getRgb(&r2, &g2, &b2, &a2);

                int r3, g3, b3, a3;

                r3 = qSqrt(r1 * r1 + r2 * r2);
                g3 = qSqrt(g1 * g1 + g2 * g2);
                b3 = qSqrt(b1 * b1 + b2 * b2);
                a3 = qSqrt(a1 * a1 + a2 * a2);
//                r3 = qAbs(r1 - r2);
//                g3 = qAbs(g1 - g2);
//                b3 = qAbs(b1 - b2);
//                a3 = qAbs(a1 - a2);

                int mid = (r3 + g3 + b3) / 3;
                r3 = mid;
                g3 = mid;
                b3 = mid;

                if (r3 > 255) r3 = 255;
                if (g3 > 255) g3 = 255;
                if (b3 > 255) b3 = 255;
                if (a3 > 255) a3 = 255;

                result.setPixel(j, i, qRgba(r3, g3, b3, a3));
            }
        }

        return result;
    }

    float unsharpMaskingOperator[25] {
        1, 4, 6, 4, 1,
        4, 16, 24, 16, 4,
        6, 24, -476, 24, 6,
        4, 16, 24, 16, 4,
        1, 4, 6, 4, 1
    };
    QImage UnsharpMasking(const QImage &image) {
        Matrix<float> matrix(unsharpMaskingOperator, 5, 5);
        return Convolution(image, matrix, -256);
    }
}
