#pragma once


#include "quad.h"

template <typename T> class matrix {
public:
	/* constructors */
	matrix(unsigned int rows, unsigned int cols, std::shared_ptr<T[]> arr);
	matrix(unsigned int rows, unsigned int cols);

	/* deconstructor */
	~matrix();

	/* easy indexing */
	T &operator[](unsigned int index) const;

	/* sub-matricing */
	quad<T> subquad(unsigned int index1, unsigned int index2) const;

	static matrix<T> eye(unsigned int dim);
	static matrix<T> zero(unsigned int dim);
	static matrix<T> zero(unsigned int dim1, unsigned int dim2);

	/* elementary operations */
	void rowOp(int src, unsigned int dest);
	void colOp(int src, unsigned int dest);

	void rowOp(int src, T coeff, unsigned int dest);
	void colOp(int src, T coeff, unsigned int dest);

	void rowOp(int* src, unsigned int size, unsigned int dest);
	void colOp(int* src, unsigned int size, unsigned int dest);

	void rowOp(int* src, T* coeff, unsigned int size, unsigned int dest);
	void colOp(int* src, T* coeff, unsigned int size, unsigned int dest);

public:
	unsigned int rows, cols;

private:
	std::shared_ptr<int[]> arr;
};

/* constructors */
template <typename T>
inline matrix<T>::matrix(unsigned int rows, unsigned int cols,
                         std::shared_ptr<T[]> arr)
	: rows(rows), cols(cols), arr(arr) {}

template <typename T>
inline matrix<T>::matrix(unsigned int rows, unsigned int cols)
	: rows(rows), cols(cols)
{
	this->arr = std::shared_ptr<T[]>(new int[rows * cols]);
}

/* deconstructor */
template <typename T> inline matrix<T>::~matrix() {}

// utility functions
//------------------

/* easy indexing */
template <typename T>
inline T &matrix<T>::operator[](unsigned int index) const
{
	return arr[index];
}

/* sub-matricing */
template <typename T>
quad<T> matrix<T>::subquad(unsigned int index1, unsigned int index2) const
{
	unsigned int row_offset = index1 / this->cols,
	             col_offset = index1 % this->cols,
	             rows = (index2 / this->cols) - row_offset + 1,
	             cols = (index2 % this->cols) - col_offset + 1;

	return quad<T>(rows, cols, arr, this->cols, row_offset, col_offset);
}

template <typename T>
std::ostream &operator<<(std::ostream &output, const matrix<T> &A)
{
	output << "============================\n";

	for (unsigned int i = 0; i < A.rows; ++i) {
		for (unsigned int j = 0; j < A.cols; ++j)
			output << A[i * A.cols + j] << "\t";
		output << "\n";
	}

	output << "============================\n";

	return output;
}


// default matrices
//-----------------

/* identity matrix */
template <typename T>
matrix<T> matrix<T>::eye(unsigned int dim)
{
	matrix<T> m(dim, dim);
	memset(&(m.arr), 0, sizeof(T) * dim * dim);

	for (unsigned int i = 0; i < dim; ++i)
		m[(dim + 1) * i] = 1;

	return m;
}

/* zero matrix */
template <typename T>
matrix<T> matrix<T>::zero(unsigned int dim1, unsigned int dim2)
{
	matrix<T> m(dim1, dim2);
	memset(&(m.arr), 0, sizeof(T) * dim1 * dim2);
	return m;
}
template <typename T>
matrix<T> matrix<T>::zero(unsigned int dim)
{
	matrix<T> m(dim, dim);
	memset(&(m.arr), 0, sizeof(T) * dim * dim);
	return m;
}

// logical operators
//------------------

/* equality */
template <typename T> bool operator==(const matrix<T> &A, const matrix<T> &B)
{
	if (!(A.rows == B.rows && A.cols == B.cols))
		return false;

	for (unsigned int i = 0; i < A.rows * A.cols; ++i) {
		if (A[i] != B[i])
			return false;
	}
	return true;
}

template <typename T> matrix<T> quad<T>::tomatrix() const
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
matrix<T> operator+(const matrix<T> &A, const matrix<T> &B)
{
	ASSERT(A.rows == B.rows && A.cols == B.cols);

	matrix<T> C(A.rows, A.cols);

	for (unsigned int i = 0; i < A.rows; ++i) {
		for (unsigned int j = 0; j < A.cols; ++j)
			C[i * A.cols + j] = A[i * A.cols + j] + B[i * A.cols + j];
	}

	return C;
}

/* subtraction */
template <typename T>
matrix<T> operator-(const matrix<T> &A, const matrix<T> &B)
{
	ASSERT(A.rows == B.rows && A.cols == B.cols);

	matrix<T> C(A.rows, A.cols);

	for (unsigned int i = 0; i < A.rows; ++i) {
		for (unsigned int j = 0; j < A.cols; ++j)
			C[i * A.cols + j] = A[i * A.cols + j] + B[i * A.cols + j];
	}

	return C;
}

/* multiplication */
template <typename T>
matrix<T> operator*(const matrix<T> &A, const matrix<T> &B)
{
	ASSERT(A.cols == B.rows);
	if (A.cols > 4096 && float(A.rows) / A.cols > 0.7)
		return strassen(A, B);
	return normal_matmul(A, B);
}

template <typename T>
matrix<T> normal_matmul(const matrix<T> &A, const matrix<T> &B)
{
	matrix<T> C(A.rows, B.cols);

	for (unsigned int i = 0; i < A.rows; ++i) {
		for (unsigned int j = 0; j < B.cols; ++j) {
			C[i * C.cols + j] = 0;
			for (unsigned int k = 0; k < B.rows; ++k)
				C[i * C.cols + j] += A[i * A.cols + k] * B[k * B.cols + j];
		}
	}

	return C;
}

template <typename T>
matrix<T> strassen(const matrix<T> &A, const matrix<T> &B)
{
	ASSERT(A.cols == B.rows);

	// getting the quater submatrices
	quad<T> a = A.subquad(0, (A.rows - 1) / 2 * A.cols + A.cols / 2 - 1),
	        b = A.subquad(A.cols / 2, (A.rows - 1) / 2 * A.cols + A.cols - 1),
	        c = A.subquad(A.rows * A.cols / 2,
	                      (A.rows - 1) * A.cols + A.cols / 2 - 1),
	            d = A.subquad(A.rows * A.cols / 2 + A.cols / 2, A.rows * A.cols - 1);

	quad<T> e = B.subquad(0, (B.rows - 1) / 2 * B.cols + B.cols / 2 - 1),
	        f = B.subquad(B.cols / 2, (B.rows - 1) / 2 * B.cols + B.cols - 1),
	        g = B.subquad(B.rows * B.cols / 2,
	                      (B.rows - 1) * B.cols + B.cols / 2 - 1),
	            h = B.subquad(B.rows * B.cols / 2 + B.cols / 2, B.rows * B.cols - 1);

	// matrices needed for strassen
	quad<T> p1 = (a + d) * (e + h), p2 = (c + d) * e, p3 = a * (f - h),
	        p4 = d * (g - e), p5 = (a + b) * h, p6 = (c - a) * (e + f),
	        p7 = (b - d) * (g + h);

	quad<T> c1 = p1 + p4 + p7 - p5, c2 = p3 + p5, c3 = p2 + p4,
	        c4 = p1 + p3 + p6 - p2;

	matrix<T> C(A.rows, B.cols);

	for (unsigned int i = 0; i < c1.rows; ++i) {
		for (unsigned int j = 0; j < c1.cols; ++j) {
			C[i * C.cols + j] = c1[i * c1.cols + j];
			C[i * C.cols + j + c1.cols] = c2[i * c2.cols + j];
			C[(i + c1.rows) * C.cols + j] = c3[i * c3.cols + j];
			C[(i + c1.rows) * C.cols + j + c1.cols] = c4[i * c4.cols + j];
		}
	}

	return C;
}

//  scalar operations
//-------------------

template <typename T>
matrix<T> operator * (const matrix<T> &A, const float t)
{
	matrix<T> C(A.rows, A.cols);

	for (unsigned int i = 0; i < A.rows; ++i) {
		for (unsigned int j = 0; j < A.cols; ++j) {
			C[i * A.cols + j] = A[i * A.cols + j] * t;
		}
	}

	return C;
}

template <typename T>
matrix<T> operator * (const float t, const matrix<T> &A)
{
	matrix<T> C(A.rows, A.cols);

	for (unsigned int i = 0; i < A.rows; ++i) {
		for (unsigned int j = 0; j < A.cols; ++j) {
			C[i * A.cols + j] = t * A[i * A.cols + j];
		}
	}

	return C;
}

template <typename T>
matrix<T> operator / (const matrix<T> &A, const float t)
{
	matrix<T> C(A.rows, A.cols);

	for (unsigned int i = 0; i < A.rows; ++i) {
		for (unsigned int j = 0; j < A.cols; ++j) {
			C[i * A.cols + j] = A[i * A.cols + j] / t;
		}
	}

	return C;
}

template <typename T>
matrix<T> operator / (const float t, const matrix<T> &A)
{
	matrix<T> C(A.rows, A.cols);

	for (unsigned int i = 0; i < A.rows; ++i) {
		for (unsigned int j = 0; j < A.cols; ++j) {
			C[i * A.cols + j] = t / A[i * A.cols + j];
		}
	}

	return C;
}

// elementary row operations
