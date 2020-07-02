#pragma once

#include "quad.h"

template <typename T>
class matrix {
  public:
	/* constructors */
	matrix(unsigned int rows, unsigned int cols, std::shared_ptr<T[]> arr);
	matrix(unsigned int rows, unsigned int cols);

	/* deconstructor */
	~matrix();

	/* easy indexing */
	T& operator [] (unsigned int index) const;

	/* sub-matricing */
	quad<T> subquad(unsigned int index1, unsigned int index2) const;

	/* basic operations */
	matrix operator + (const matrix& B) const;
	matrix operator - (const matrix& B) const;
	matrix operator * (const matrix& B) const;

	matrix strassen(const matrix& B) const;

  public:
	unsigned int rows, cols;
  private:
	std::shared_ptr<int[]> arr;
};

template <typename T>
std::ostream& operator << (std::ostream& output, const matrix<T>& A);

/* constructors */
template <typename T>
inline
matrix<T>::matrix
(
	unsigned int rows,
	unsigned int cols,
	std::shared_ptr<T[]> arr
)
	: rows(rows), cols(cols), arr(arr) {}

template <typename T>
matrix<T>::matrix(unsigned int rows, unsigned int cols)
	: rows(rows), cols(cols)
{
	this->arr = std::shared_ptr<T[]>( new int[rows * cols] );
}


/* deconstructor */
template <typename T>
inline
matrix<T>::~matrix() {}

/* easy indexing */
template <typename T>
inline
T& matrix<T>::operator [] (unsigned int index) const {
	return arr[index];
}

/* sub-matricing */
template <typename T>
inline
quad<T> matrix<T>::subquad(unsigned int index1, unsigned int index2) const {
	unsigned int row_offset = index1 / this->cols,
		     col_offset = index1 % this->cols,
		     rows = (index2 / this->cols) - row_offset + 1,
		     cols = (index2 % this->cols) - col_offset + 1;

	// For debugging perposes
	// TODO: comment out when not in use anymore
	//printf("quad params : %d %d %d %d\n", rows, cols, row_offset, col_offset);

	return quad<T>(rows, cols, arr, this->cols, row_offset, col_offset);
}

/* easy printing */
template <typename T>
inline
std::ostream& operator << (std::ostream& output, const matrix<T>& A) {
	output << "============================\n";

	for (unsigned int i = 0; i < A.rows; ++i) {
		for (unsigned int j = 0; j < A.cols; ++j)
			output << A[i*A.cols + j] << "\t";
		output << "\n";
	}

	output << "============================\n";

	return output;
}


/* basic operation */
/* addition */
template <typename T>
inline
matrix<T> matrix<T>::operator + (const matrix<T>& B) const {
	ASSERT(this->rows == B.rows && this->cols == B.cols);

	matrix<T> C(this->rows, this->cols);

	for (unsigned int i = 0; i < this->rows; ++i) {
		for (unsigned int j = 0; j < this->cols; ++j)
			C[i*this->cols + j] =
				arr[i*this->cols + j] + B[i*this->cols + j];
	}

	return C;
}

/* subtraction */
template <typename T>
inline
matrix<T> matrix<T>::operator - (const matrix<T>& B) const {
	ASSERT(this->rows == B.rows && this->cols == B.cols);

	matrix C(this->rows, this->cols);

	for (unsigned int i = 0; i < this->rows; ++i) {
		for (unsigned int j = 0; j < this->cols; ++j)
			C[i*this->cols + j] =
				arr[i*this->cols + j] - B[i*this->cols + j];
	}

	return C;
}

/* multiplication */
template <typename T>
inline
matrix<T> matrix<T>::operator * (const matrix<T>& B) const {
	ASSERT(this->cols == B.rows);

	matrix<T> C(this->rows, B.cols);

	for (unsigned int i = 0; i < this->rows; ++i) {
		for (unsigned int j = 0; j < B.cols; ++j) {
			C[i*C.cols + j] = 0;
		       	for (unsigned int k = 0; k < B.rows; ++k)
		       		C[i*C.cols + j] +=
					arr[i*this->cols + k] * B[k*B.cols + j];
		}
	}

	return C;
}

/* from quad.h as matrices
   weren't defined then */
template <typename T>
matrix<T> quad<T>::tomatrix() const {
	matrix<T> C(this->rows, this->cols);

	for (unsigned int i = 0; i < this->rows * this->cols; ++i)
		C[i] = this[i];

	return C;
}

template <typename T>
inline
matrix<T> matrix<T>::strassen(const matrix<T>& B) const {
	ASSERT(this->cols == B.rows);

	// getting the quater submatrices
	quad<T> a = this->subquad(0,
			(this->rows - 1)/2 * this->cols + this->cols / 2 - 1),
		b = this->subquad(this->cols / 2,
		 		(this->rows - 1)/2 * this->cols + this->cols - 1),
		c = this->subquad(this->rows * this->cols / 2,
				(this->rows - 1) * this->cols + this->cols / 2 - 1),
		d = this->subquad(this->rows * this->cols / 2 + this->cols / 2,
			     	this->rows * this->cols - 1);

	quad<T> e = B.subquad(0, (B.rows - 1)/2 * B.cols + B.cols / 2 - 1),
		f = B.subquad(B.cols / 2, (B.rows - 1)/2 * B.cols + B.cols - 1),
		g = B.subquad(B.rows * B.cols / 2,
				(B.rows - 1) * B.cols + B.cols / 2 - 1),
		h = B.subquad(B.rows * B.cols / 2 + B.cols / 2, B.rows * B.cols - 1);

	quad<T> temp = f - h;
	std::cout << temp;
	temp = temp*a;
	std::cout << temp;

	// matrices needed for strassen
	quad<T> p1 = a * (f - h),
		p2 = (a + b) * h,
		p3 = (c + d) * e,
		p4 = d * (g - e),
		p5 = (a + d) * (e + h),
		p6 = (b - d) * (g + h),
		p7 = (a - c) * (e + f);

	std::cout << "ps\n";

	std::cout << p1 << p2 << p3 << p4 << p5 << p6 << p7 << std::endl;

	quad<T> c1 = p4 + p5 + p6 - p2,
		c2 = p1 + p2,
		c3 = p3 + p4,
		c4 = p1 + p5 - p3 - p7;

	std::cout << "cs\n";
	std::cout << c1[0] << c2[0] << c3[0] << c4[0] << std::endl;

	matrix C(this->rows, B.cols);
	unsigned int index = 0,
		     limit = C.rows * C.cols / 4;

	for (unsigned int i = 0; i < limit; ++i, ++index)
		C[index] = c1[i];

	for (unsigned int i = 0; i < limit; ++i, ++index)
		C[index] = c2[i];

	for (unsigned int i = 0; i < limit; ++i, ++index)
		C[index] = c3[i];

	for (unsigned int i = 0; i < limit; ++i, ++index)
		C[index] = c4[i];

	return C;
}
