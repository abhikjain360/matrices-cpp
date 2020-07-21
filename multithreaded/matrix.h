#pragma once

#include "quad.h"

template <typename T>
class matrix {
public:
    /* constructors */
    matrix(unsigned int rows, unsigned int cols);
    matrix(unsigned int rows, unsigned int cols, std::shared_ptr<T[]> arr);

    /* deconstructor */
    ~matrix();

    /* easy indexing */
    T& operator [] (unsigned int index) const;

    /* easy submatrixing */
    quad<T> subquad(unsigned int index1, unsigned int index2) const;

    /* strassen */
    matrix strassen(const matrix& B) const;

public:
    unsigned int rows, cols;
    std::shared_ptr<T[]> arr;
};

template<typename T>
std::ostream& operator << (std::ostream& output, const matrix<T>& A);

/* constructors */
template<typename T>
matrix<T>::matrix(unsigned int rows, unsigned int cols)
    : rows(rows), cols(cols)
{
    this->arr = std::shared_ptr<T[]>( new T[rows * cols] );
}

template<typename T>
matrix<T>::matrix(unsigned int rows, unsigned int cols, std::shared_ptr<T[]> arr)
    : rows(rows), cols(cols), arr(arr) {}


/* deconstructor */
template<typename T>
matrix<T>::~matrix() {}


/* easy arr access */
template<typename T>
inline
T& matrix<T>::operator [] (unsigned int index) const
{
    return arr[index];
}

/* easy submatrixing */
template<typename T>
quad<T> matrix<T>::subquad(unsigned int index1, unsigned int index2) const
{
    unsigned int col_offset = index1 % this->cols,
                 row_offset = index1 / this->cols,
                 rows 	= (index2 / this->cols) - row_offset + 1,
                   cols 	= (index2 % this->cols) - col_offset + 1;

    return quad<T>(rows, cols, this->arr, this->cols, row_offset, col_offset);
}

/* easy printing */
template<typename T>
std::ostream& operator << (std::ostream& output, const matrix<T>& A)
{
    output << "============================\n";

    for (unsigned int i = 0; i < A.rows; ++i) {
        for (unsigned int j = 0; j < A.cols; ++j)
            output << A[i*A.cols + j] << ' ';
        output << "\n";
    }

    output << "============================\n";

    return output;
}

// logical operations
//-------------------

/* equality check */
template <typename T>
bool operator == (const matrix<T>& A, const matrix<T>& B)
{
    if (!(A.rows == B.rows && A.cols == B.cols))
        return false;

    for (unsigned int i = 0; i < A.cols; ++i) {
        for (unsigned int j = 0; j < A.rows; ++j) {
            if (!(A[i*A.cols + j] == B[i*A.cols + j]))
                return false;
        }
    }

    return true;
}


// basic operations
//-----------------

/* addition */
template<typename T>
matrix<T> operator + (const matrix<T>& A, const matrix<T>& B)
{
    ASSERT(A.rows == B.rows && A.cols == B.cols);

    matrix<T> C(A.rows, A.cols);
    {
        threadpool tp(3);

        for (unsigned int i = 0; i < A.rows; ++i) {
            tp.add
            (
                []
                (
                    unsigned int i, unsigned int j,
                    const matrix<T>& A,const matrix<T>& B,const matrix<T>& C
            ) {
                for (unsigned int j = 0; j < A.cols; ++j)
                    C[i*A.cols + j] =
                        A[i*A.cols + j] + B[i*A.cols + j];
            },
            i, A, B, C
            );
        };
    }

    return C;
}

/* subtraction */
template<typename T>
matrix<T> operator - (const matrix<T>& A, const matrix<T>& B)
{
    ASSERT(A.rows == B.rows && A.cols == B.cols);

    matrix<T> C(A.rows, A.cols);
    {
        threadpool tp(3);

        for (unsigned int i = 0; i < A.rows; ++i) {
            tp.add
            (
                []
                (
                    unsigned int i, unsigned int j,
                    const matrix<T>& A,const matrix<T>& B,const matrix<T>& C
            ) {
                for (unsigned int j = 0; j < A.cols; ++j)
                    C[i*A.cols + j] =
                        A[i*A.cols + j] - B[i*A.cols + j];
            },
            i, A, B, C
            );
        };
    }

    return C;
}

/* multiplication */
template<typename T>
matrix<T> operator * (const matrix<T>& A, const matrix<T>& B)
{
    ASSERT(A.cols == B.rows);

    return normal_matmul(A, B);
}

template<typename T>
matrix<T> normal_matmul(const matrix<T>& A, const matrix<T>& B)
{
    matrix<T> C(A.rows, B.cols);
    {
        threadpool tp(3);

        for (unsigned int i = 0; i < A.rows; ++i) {
            for (unsigned int j = 0; j < B.cols; ++j) {
                C[i*C.cols + j] = 0;
                tp.add (
                    []
                    (
                        unsigned int i, unsigned int j,
                        const matrix<T>& A,const matrix<T>& B,const matrix<T>& C
                ) {
                    for (unsigned int k = 0; k < B.rows; ++k)
                        C[i*C.cols + j] +=
                            A[i*A.cols + k] * B[k*B.cols + j];
                },
                i, j, A, B, C
                );
            }
        }
    }

    return C;
}
