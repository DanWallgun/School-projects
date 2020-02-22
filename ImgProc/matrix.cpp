#pragma once

#include <memory>
#include <assert.h>
#include <initializer_list>

template<typename ElementType>
class Matrix {
private:
        std::unique_ptr<ElementType[]> rawdata;

public:
        int width, height;
        inline Matrix(int width, int height) {
                assert(width > 0 && height > 0);

                this->width = width;
                this->height = height;
                this->rawdata.reset(new ElementType[width * height]);
        }

        inline Matrix(const ElementType initList[], int width, int height) {
                assert(width > 0 && height > 0);

                this->width = width;
                this->height = height;
                this->rawdata.reset(new ElementType[width * height]);

                for (int i = 0; i < width * height; ++i) rawdata[i] = initList[i];
        }

        // --- The code is needed for C++11 compatibility (for VS2013) ---

        Matrix(const Matrix&) = default;
        Matrix(Matrix&&) = default;

        Matrix& operator = (const Matrix&) = default;
        Matrix& operator = (Matrix&&) = default;

        // --- End of code region ---

        inline ElementType operator() (int i, int j) const {
                assert(j >= 0 && j < width && i >= 0 && i < height);
                return rawdata[i * width + j];
        }

        inline ElementType& operator() (int i, int j) {
                assert(j >= 0 && j < width && i >= 0 && i < height);
                return rawdata[i * width + j];
        }

        inline Matrix<ElementType> Copy() const {
                Matrix<ElementType> res(width, height);

                if (rawdata)
                        memcpy(res.rawdata.get(), rawdata.get(), width * height * sizeof(ElementType));

                return res;
        }
};
