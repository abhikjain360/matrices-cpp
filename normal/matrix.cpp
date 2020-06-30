#include "matrix.h"

/* threadpool for multithreading */

void print_matrix_error(std::string condition, std::string file, int line) {
	std::cout
		<< file
		<< " [condition not met] : "
		<< line
		<<" : "
		<< condition
	        << std::endl;
}

/* constructors */
matrix::matrix
(
	unsigned int rows,
	unsigned int cols,
	std::shared_ptr<double[]> arr
)
	: rows(rows), cols(cols), arr(arr) {}

matrix::matrix(unsigned int rows, unsigned int cols)
	: rows(rows), cols(cols)
{
	this->arr = std::shared_ptr<double[]>( new double[rows * cols] );
}


/* deconstructor */
matrix::~matrix() {}


/* easy arr access */
double& matrix::operator [] (unsigned int index) const {
	return arr[index];
}

/* easy printing */
std::ostream& operator << (std::ostream& output, const matrix& A) {
	output << "============================\n";

	for (unsigned int i = 0; i < A.rows; ++i) {
		for (unsigned int j = 0; j < A.cols; ++j)
			output << A[i*A.cols + j] << "\t";
		output << "\n";
	}

	output << "============================\n";

	return output;
}


/* basic operations
	TODO: test for matrices
	      of different sizes
	      apart from 2x2
*/
/* addition */
matrix matrix::operator + (const matrix& B) const {
	ASSERT(this->rows == B.rows && this->cols == B.cols);

	matrix C(this->rows, this->cols);

	for (unsigned int i = 0; i < this->rows; ++i) {
		for (unsigned int j = 0; j < this->cols; ++j)
			C[i*this->cols + j] =
				arr[i*this->cols + j] + B[i*this->cols + j];
	}

	return C;
}

/* subtraction */
matrix matrix::operator - (const matrix& B) const {
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
matrix matrix::operator * (const matrix& B) const {
	ASSERT(this->cols == B.rows);

	matrix C(this->rows, B.cols);

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
