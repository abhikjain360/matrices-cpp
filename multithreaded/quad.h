#pragma once

/* standard libraries */
#include <iostream>
#include <cstdlib>
#include <memory>

#define ASSERT(x) if(!(x)) { print_matrix_error(#x, __FILE__, __LINE__);exit(-1); };

void print_matrix_error(std::string condition, std::string file, int line)
{
    std::cout
            << file
            << " [condition not met] : "
            << line
            << " : "
            << condition
            << std::endl;
}

template<typename T>
class quad {
public:
    /* constructors */
    quad(unsigned int rows, unsigned int cols);
    quad
    (
        unsigned int rows, unsigned int cols,
        const std::shared_ptr<T[]>& arr,
        unsigned int row_offset = 0, unsigned int col_offset = 0
    );
    quad
    (
        unsigned int rows, unsigned int cols,
        const std::shared_ptr<T[]>& arr,
        unsigned int orig_cols,
        unsigned int row_offset = 0, unsigned int col_offset = 0
    );

    /* deconstructor */
    ~quad();

    /* easy indexing */
    T& operator [] (unsigned int index) const;

    /* easy subquading */
    quad<T> subquad(unsigned int index1, unsigned int index2) const;

public:
    unsigned int rows, cols;
private:
    std::shared_ptr<T[]> arr;
    unsigned int orig_cols;
    unsigned int row_offset, col_offset;
};

template<typename T>
std::ostream& operator << (std::ostream& output, const quad<T>& A);

/* threadpool for multithreading */
#include "threadpool.h"

/* constructors */
template<typename T>
quad<T>::quad(unsigned int rows, unsigned int cols)
    : rows(rows), cols(cols)
{
    this->arr        = std::shared_ptr<T[]>( new T[rows * cols] );
    this->orig_cols  = cols;
    this->row_offset = this->col_offset = 0;
}

template<typename T>
quad<T>::quad
(
    unsigned int rows, unsigned int cols,
    const std::shared_ptr<T[]>& arr,
    unsigned int row_offset, unsigned int col_offset
)
    : rows(rows), cols(cols),
      arr(arr),
      row_offset(row_offset), col_offset(col_offset)
{
    this->orig_cols = cols;
}

template<typename T>
quad<T>::quad
(
    unsigned int rows, unsigned int cols,
    const std::shared_ptr<T[]>& arr,
    unsigned int orig_cols,
    unsigned int row_offset, unsigned int col_offset
)
    : rows(rows), cols(cols),
      arr(arr),
      orig_cols(orig_cols),
      row_offset(row_offset), col_offset(col_offset)
{}


/* deconstructor */
template<typename T>
quad<T>::~quad() {}


// utility functions
//------------------

/* indexing */
template<typename T>
inline
T& quad<T>::operator [] (unsigned int index) const
{
    unsigned int c = index % cols + col_offset,
                 r = index / cols + row_offset;
    return arr[ r*orig_cols + c ];
}

/* subquading */


/* easy printing */
template<typename T>
std::ostream& operator << (std::ostream& output, const quad<T>& A)
{
    output << "============================\n";

    for (unsigned int i = 0; i < A.rows; ++i) {
        for (unsigned int j = 0; j < A.cols; ++j)
            output << A[i*A.cols + j] << "\t";
        output << "\n";
    }

    output << "============================\n";

    return output;
}


// basic operations
//-----------------

/* addition */
template<typename T>
quad<T> operator + (const quad<T>& A, const quad<T>& B)
{
    ASSERT(A.rows == B.rows && A.cols == B.cols);

    quad<T> C(A.rows, A.cols);
    {
        threadpool tp(3);

        for (unsigned int i = 0; i < A.rows; ++i) {
            tp.add
            (
                []
                (
                    unsigned int i,
                    const quad<T>& A,
                    const quad<T>& B,
                    const quad<T>& C
            ) {
                for (unsigned int j = 0; j < A.cols; ++j)
                    C[i*A.cols + j] =
                        A[i*A.cols + j] + B[i*A.cols + j];
            },
            i, *A, B, C
            );
        };
    }

    return C;
}

/* subtraction */
template<typename T>
quad<T> operator - (const quad<T>& A, const quad<T>& B)
{
    ASSERT(A.rows == B.rows && A.cols == B.cols);

    quad<T> C(A.rows, A.cols);
    {
        threadpool tp(3);

        for (unsigned int i = 0; i < A.rows; ++i) {
            tp.add
            (
                []
                (
                    unsigned int i,
                    const quad<T>& A,
                    const quad<T>& B,
                    const quad<T>& C
            ) {
                for (unsigned int j = 0; j < A.cols; ++j)
                    C[i*A.cols + j] =
                        A[i*A.cols + j] - B[i*A.cols + j];
            },
            i, *A, B, C
            );
        };
    }

    return C;
}

/* multiplication */
template<typename T>
quad<T> operator * (const quad<T>& A, const quad<T>& B)
{
    ASSERT(A.cols == B.rows);

    return normal_matmul(A, B);
}

template <typename T>
quad<T> normal_matmul(const quad<T>& A, const quad<T>& B)
{
    quad<T> C(A.rows, B.cols);
    {
        threadpool tp(3);

        for (unsigned int i = 0; i < A.rows; ++i) {
            for (unsigned int j = 0; j < B.cols; ++j) {
                C[i*C.cols + j] = 0;
                tp.add
                (
                    []
                    (
                        unsigned int i,
                        unsigned int j,
                        const quad<T>& A,
                        const quad<T>& B,
                        const quad<T>& C
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
