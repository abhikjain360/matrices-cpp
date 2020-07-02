#pragma once

/* standard libraries */
#include <iostream>
#include <cstdlib>
#include <memory>

#define ASSERT(x) if(!(x)) { print_matrix_error(#x, __FILE__, __LINE__);exit(-1); };

void print_matrix_error(std::string condition, std::string file, int line) {
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
	 	unsigned int rows,
		unsigned int cols,
		const std::shared_ptr<T[]>& arr,
		unsigned int row_offset = 0,
		unsigned int col_offset = 0
	);
	quad
	(
	 	unsigned int rows,
		unsigned int cols,
		const std::shared_ptr<T[]>& arr,
		unsigned int orig_cols,
		unsigned int row_offset = 0,
		unsigned int col_offset = 0
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

	/* basic operations */
	quad operator + (const quad& B) const;
	quad operator - (const quad& B) const;
	quad operator * (const quad& B) const;

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
quad<T>::quad(unsigned int rows, unsigned int cols)
	: rows(rows), cols(cols)
{
	this->arr        = std::shared_ptr<T[]>( new int[rows * cols] );
	this->orig_cols  = cols;
	this->row_offset = this->col_offset = 0;
}

template <typename T>
quad<T>::quad
(
	unsigned int rows,
	unsigned int cols,
	const std::shared_ptr<T[]>& arr,
	unsigned int row_offset,
	unsigned int col_offset
)
	: rows(rows),
	  cols(cols),
	  arr(arr),
	  row_offset(row_offset),
	  col_offset(col_offset)
{
	this->orig_cols = cols;
}

template <typename T>
quad<T>::quad
(
 	unsigned int rows,
	unsigned int cols,
	const std::shared_ptr<T[]>& arr,
	unsigned int orig_cols,
	unsigned int row_offset,
	unsigned int col_offset
)
	: rows(rows),
	  cols(cols),
	  arr(arr),
	  orig_cols(orig_cols),
	  row_offset(row_offset),
	  col_offset(col_offset)
{}


/* deconstructor */
template <typename T>
quad<T>::~quad() {}


/* easy arr access */
template <typename T>
T& quad<T>::operator [] (unsigned int index) const {
	unsigned int c = index % cols + col_offset,
		     r = index / cols + row_offset;
	return arr[ r*orig_cols + c ];
}

/* easy subquading */


/* easy printing */
template <typename T>
std::ostream& operator << (std::ostream& output, const quad<T>& A) {
	output << "============================\n";

	for (unsigned int i = 0; i < A.rows; ++i) {
		for (unsigned int j = 0; j < A.cols; ++j)
			output << A[i*A.cols + j] << "\t";
		output << "\n";
	}

	output << "============================\n";

	return output;
}


/* basic operations */
/* addition */
template <typename T>
quad<T> quad<T>::operator + (const quad<T>& B) const {
	ASSERT(this->rows == B.rows && this->cols == B.cols);

	quad<T> C(this->rows, this->cols);

	for (unsigned int i = 0; i < this->rows; ++i) {
		for (unsigned int j = 0; j < this->cols; ++j)
			C[i*this->cols + j] =
				arr[i*this->cols + j] + B[i*this->cols + j];
	}

	return C;
}

/* subtraction */
template <typename T>
quad<T> quad<T>::operator - (const quad<T>& B) const {
	ASSERT(this->rows == B.rows && this->cols == B.cols);

	quad<T> C(this->rows, this->cols);

	for (unsigned int i = 0; i < this->rows; ++i) {
		for (unsigned int j = 0; j < this->cols; ++j)
			C[i*this->cols + j] =
				arr[i*this->cols + j] - B[i*this->cols + j];
	}

	return C;
}

/* multiplication */
template <typename T>
quad<T> quad<T>::operator * (const quad<T>& B) const {
	ASSERT(this->cols == B.rows);

	quad<T> C(this->rows, B.cols);

	for (unsigned int i = 0; i < this->rows; ++i) {
		for (unsigned int j = 0; j < B.cols; ++j) {
			for (unsigned int k = 0; k < B.rows; ++k)
				C[i*C.cols + j] +=
					arr[i*this->cols + k] * B[k*B.cols + j];
		}
	}

	return C;
}
