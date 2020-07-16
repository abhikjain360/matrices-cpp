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
	        <<" : "
	        << condition
	        << std::endl;
}

template <typename T>
class matrix;

template <typename T>
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
	quad subquad(unsigned int index1, unsigned int index2) const;

	/* quad to matrices, copies values
	   instead of shared pointer sharing */
	matrix<T> tomatrix() const;

public:
	unsigned int rows, cols;
private:
	std::shared_ptr<T[]> arr;
	unsigned int orig_cols;
	unsigned int row_offset, col_offset;
};

template <typename T>
std::ostream& operator << (std::ostream& output, const quad<T>& A);

/* constructors */
template <typename T>
inline
quad<T>::quad(unsigned int rows, unsigned int cols)
	: rows(rows), cols(cols)
{
	arr        = std::shared_ptr<T[]>( new int[rows * cols] );
	orig_cols  = cols;
	row_offset = 0;
	col_offset = 0;
}

template <typename T>
inline
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
	orig_cols = cols;
}

template <typename T>
inline
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
template <typename T>
inline
quad<T>::~quad() {}


// utility functions
//------------------

// easy indexing */
template <typename T>
inline
T& quad<T>::operator [] (unsigned int index) const
{
	unsigned int c = index % cols + col_offset,
	             r = index / cols + row_offset;
	return arr[ r*orig_cols + c ];
}

/* easy sub-quading */
template <typename T>
quad<T> quad<T>::subquad(unsigned int index1, unsigned int index2) const
{
	unsigned int row_offset = index1 / this->cols,
	             col_offset = index1 % this->cols,
	             rows = (index2 / this->cols) - row_offset + 1,
	             cols = (index2 % this->cols) - col_offset + 1;

	return quad<T>(rows, cols, arr, this->cols, row_offset, col_offset);
}

/* easy printing */
template <typename T>
std::ostream& operator << (std::ostream& output, const quad<T>& A)
{
	output << "~~~~~~~~~~~~~~\n";

	for (unsigned int i = 0; i < A.rows; ++i) {
		for (unsigned int j = 0; j < A.cols; ++j)
			output << A[i*A.cols + j] << "\t";
		output << "\n";
	}


	return output;
}


// logical operators
//------------------

/* equality */
template <typename T>
bool operator == (const matrix<T>& A, const matrix<T>& B)
{
	if (!(A.rows == B.rows && A.cols == B.cols))
		return false;

	for (unsigned int i = 0; i < A.rows*A.cols; ++i) {
		if(A[i] != B[i])
			return false;
	}
	return true;
}

template <typename T>
matrix<T> quad<T>::tomatrix() const
{
	matrix<T> C(this->rows, this->cols);

	for (unsigned int i = 0; i < this->rows * this->cols; ++i)
		C[i] = this[i];

	return C;
}


// basic operation
//----------------

/* addition */
template <typename T>
quad<T> operator + (const quad<T>& A, const quad<T>& B)
{
	ASSERT(A.rows == B.rows && A.cols == B.cols);

	quad<T> C(A.rows, A.cols);

	for (unsigned int i = 0; i < A.rows; ++i) {
		for (unsigned int j = 0; j < A.cols; ++j)
			C[i*A.cols + j] =
			    A[i*A.cols + j] + B[i*A.cols + j];
	}

	return C;
}

/* subtraction */
template <typename T>
quad<T> operator - (const quad<T>& A, const quad<T>& B)
{
	ASSERT(A.rows == B.rows && A.cols == B.cols);

	quad<T> C(A.rows, A.cols);

	for (unsigned int i = 0; i < A.rows; ++i) {
		for (unsigned int j = 0; j < A.cols; ++j)
			C[i*A.cols + j] =
			    A[i*A.cols + j] - B[i*A.cols + j];
	}

	return C;
}

/* multiplication */
template <typename T>
quad<T> operator * (const quad<T>& A, const quad<T>& B)
{
	ASSERT(A.cols == B.rows);

	if (A.cols > 1024 && float(A.rows) / A.cols > 0.7)
		return strassen(A, B);
	return normal_matmul(A, B);
}

template <typename T>
quad<T> normal_matmul(const quad<T>& A, const quad<T>& B)
{
	quad<T> C(A.rows, B.cols);

	for (unsigned int i = 0; i < A.rows; ++i) {
		for (unsigned int j = 0; j < B.cols; ++j) {
			C[i*C.cols + j] = 0;
			for (unsigned int k = 0; k < B.rows; ++k)
				C[i*C.cols + j] += A[i*A.cols + k] * B[k*B.cols + j];
		}
	}

	return C;
}

template <typename T>
quad<T> strassen(const quad<T>& A, const quad<T>& B)
{
	ASSERT(A.cols == B.rows);

	// getting the quater submatrices
	quad<T> a = A.subquad(0, (A.rows - 1)/2 * A.cols + A.cols / 2 - 1),
	        b = A.subquad(A.cols / 2, (A.rows - 1)/2 * A.cols + A.cols - 1),
	        c = A.subquad(A.rows * A.cols / 2, (A.rows - 1) * A.cols + A.cols / 2 - 1),
	        d = A.subquad(A.rows * A.cols / 2 + A.cols / 2, A.rows * A.cols - 1);

	quad<T> e = B.subquad(0, (B.rows - 1)/2 * B.cols + B.cols / 2 - 1),
	        f = B.subquad(B.cols / 2, (B.rows - 1)/2 * B.cols + B.cols - 1),
	        g = B.subquad(B.rows * B.cols / 2, (B.rows - 1) * B.cols + B.cols / 2 - 1),
	        h = B.subquad(B.rows * B.cols / 2 + B.cols / 2, B.rows * B.cols - 1);

	// matrices needed for strassen
	quad<T> p1 = (a + d) * (e + h),
	        p2 = (c + d) * e,
	        p3 = a * (f - h),
	        p4 = d * (g - e),
	        p5 = (a + b) * h,
	        p6 = (c - a) * (e + f),
	        p7 = (b - d) * (g + h);

	quad<T> c1 = p1 + p4 + p7 - p5,
	        c2 = p3 + p5,
	        c3 = p2 + p4,
	        c4 = p1 + p3 + p6 - p2;

	quad<T> C(A.rows, B.cols);

	for (unsigned int i = 0; i < c1.rows; ++i) {
		for (unsigned int j = 0; j < c1.cols; ++j) {
			C[i * C.cols + j]                       = c1[i * c1.cols + j];
			C[i * C.cols + j + c1.cols]             = c2[i * c2.cols + j];
			C[(i + c1.rows) * C.cols + j]           = c3[i * c3.cols + j];
			C[(i + c1.rows) * C.cols + j + c1.cols] = c4[i * c4.cols + j];
		}
	}

	return C;
}
