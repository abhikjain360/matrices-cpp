#include "matrix.h"
#include "quad.h"

/* threadpool for multithreading */
#include "threadpool.h"

void print_matrix_error(std::string condition, std::string file, int line) {
	std::cout
		<< file
		<< " [condition not met] : "
		<< line
		<< " : "
		<< condition
	        << std::endl;
}

/* constructors */
matrix::matrix(unsigned int rows, unsigned int cols)
	: rows(rows), cols(cols)
{
	this->arr = std::shared_ptr<double[]>( new double[rows * cols] );
}

matrix::matrix
(
	unsigned int rows,
	unsigned int cols,
	std::shared_ptr<double[]> arr
)
	: rows(rows), cols(cols), arr(arr) {}


/* deconstructor */
matrix::~matrix() {}


/* easy arr access */
double& matrix::operator [] (unsigned int index) const {
	return arr[index];
}

/* easy submatrixing */
quad matrix::subquad(unsigned int index1, unsigned int index2) const {
	unsigned int col_offset = index1 % this->cols,
		     row_offset = index1 / this->cols,
		     rows 	= (index2 / this->cols) - row_offset + 1,
		     cols 	= (index2 % this->cols) - col_offset + 1;

	printf("subquad params : %d %d %d %d\n", rows, cols, row_offset, col_offset);

	return quad(rows, cols, this->arr, this->cols, row_offset, col_offset);
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
	{
	threadpool tp(3);

	for (unsigned int i = 0; i < this->rows; ++i) {
		tp.add
		(
			[]
			(
				unsigned int i,
				const matrix& A,
				const matrix& B,
				const matrix& C
			)
			{
				for (unsigned int j = 0; j < A.cols; ++j)
					C[i*A.cols + j] =
						A[i*A.cols + j] + B[i*A.cols + j];
			},
			i, *this, B, C
		);
	};
	}

	return C;
}

/* subtraction */
matrix matrix::operator - (const matrix& B) const {
	ASSERT(this->rows == B.rows && this->cols == B.cols);

	matrix C(this->rows, this->cols);
	{
	threadpool tp(3);

	for (unsigned int i = 0; i < this->rows; ++i) {
		tp.add
		(
			[]
			(
				unsigned int i,
				const matrix& A,
				const matrix& B,
				const matrix& C
			)
			{
				for (unsigned int j = 0; j < A.cols; ++j)
					C[i*A.cols + j] =
						A[i*A.cols + j] - B[i*A.cols + j];
			},
			i, *this, B, C
		);
	};
	}

	return C;
}

/* multiplication */
matrix matrix::operator * (const matrix& B) const {
	ASSERT(this->cols == B.rows);

	matrix C(this->rows, B.cols);
	{
	threadpool tp(3);

	for (unsigned int i = 0; i < this->rows; ++i) {
		for (unsigned int j = 0; j < B.cols; ++j) {
			C[i*C.cols + j] = 0;
			tp.add
			(
				[]
				(
					unsigned int i,
					unsigned int j,
					const matrix& A,
					const matrix& B,
					const matrix& C
				)
				{
				       	for (unsigned int k = 0; k < B.rows; ++k)
				       		C[i*C.cols + j] +=
							A[i*A.cols + k] * B[k*B.cols + j];
				},
				i, j, *this, B, C
			);
		}
	}
	}

	return C;
}
