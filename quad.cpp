#include "quad.h"

/* threadpool for multithreading */
#include "threadpool.h"

void print_quad_error(std::string condition, std::string file, int line) {
	std::cout
		<< file
		<< " [condition not met] : "
		<< line
		<< " : "
		<< condition
	        << std::endl;
}

/* constructors */
quad::quad(unsigned int rows, unsigned int cols)
	: rows(rows), cols(cols)
{
	this->arr        = std::shared_ptr<double[]>( new double[rows * cols] );
	this->orig_cols  = cols;
	this->row_offset = this->col_offset = 0;
}

quad::quad
(
	unsigned int rows,
	unsigned int cols,
	const std::shared_ptr<double[]>& arr,
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

quad::quad
(
 	unsigned int rows,
	unsigned int cols,
	const std::shared_ptr<double[]>& arr,
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
quad::~quad() {}


/* easy arr access */
double& quad::operator [] (unsigned int index) const {
	unsigned int c = index % cols + col_offset,
		     r = index / cols + row_offset;
	return arr[ r*orig_cols + c ];
}

/* easy subquading */


/* easy printing */
std::ostream& operator << (std::ostream& output, const quad& A) {
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
quad quad::operator + (const quad& B) const {
	ASSERT(this->rows == B.rows && this->cols == B.cols);

	quad C(this->rows, this->cols);
	{
	threadpool tp(3);

	for (unsigned int i = 0; i < this->rows; ++i) {
		tp.add
		(
			[]
			(
				unsigned int i,
				const quad& A,
				const quad& B,
				const quad& C
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
quad quad::operator - (const quad& B) const {
	ASSERT(this->rows == B.rows && this->cols == B.cols);

	quad C(this->rows, this->cols);
	{
	threadpool tp(3);

	for (unsigned int i = 0; i < this->rows; ++i) {
		tp.add
		(
			[]
			(
				unsigned int i,
				const quad& A,
				const quad& B,
				const quad& C
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
quad quad::operator * (const quad& B) const {
	ASSERT(this->cols == B.rows);

	quad C(this->rows, B.cols);
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
					const quad& A,
					const quad& B,
					const quad& C
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
