#ifndef MATRIX_H
#define MATRIX_H

#include "threadpool.h" 	// for multithreading

#include <iostream> 		// for debugging, remove later

#define spt std::shared_ptr
#define mkspt std::shared_ptr


class matrix {
  public:
	matrix(int rows, int cols, spt<double[]> &arr) : r(rows), c(cols), ar(arr) {}

	double &operator [] (int index) {
		return ar[index];
	}

	matrix operator + (matrix &B) {
		spt<double[]> arr = mkspt<double[]>(new double[r * c], [](double *p) { delete[] p; });

		// scope to delete threadpool
		{
			threadpool tp(3);

			for (int i = 0; i < r; ++i)
			{
				tp.add(
				[] (matrix &A, matrix &B, spt<double[]> &arr, int i) {
					for (int j = 0; j < A.c; ++j)
						arr[i*A.c + j] = A[i*A.c + j] + B[i*A.c + j];
				},
					std::ref(*this),
					std::ref(B),
					std::ref(arr),
					i
				);
			}
		}

		matrix C = matrix(r, c, arr);
		return C;
	}

	matrix operator - (matrix &B) {
		spt<double[]> arr = mkspt<double[]>(new double[r * c], [](double *p) { delete[] p; });

		// scope to delete threadpool
		{
			threadpool tp(3);

			for (int i = 0; i < r; ++i)
			{
				tp.add(
				[] (matrix &A, matrix &B, spt<double[]> &arr, int i) {
					for (int j = 0; j < A.c; ++j)
						arr[i*A.c + j] = A[i*A.c + j] - B[i*A.c + j];
				},
					std::ref(*this),
					std::ref(B),
					std::ref(arr),
					i
				);
			}
		}

		matrix C = matrix(r, c, arr);
		return C;
	}

	matrix operator * (matrix &B) {
		spt<double[]> arr = mkspt<double[]>(new double[r * c], [](double *p) { delete[] p; });

		// scope to delete threadpool
		{
			threadpool tp(3);

			for (int i = 0; i < r; ++i) {
				for (int j = 0; j < B.c; ++j) {
					arr[i*r + j] = 0;
					tp.add([] (matrix &A, matrix &B, spt<double[]>& arr, int i, int j) {
						for (int k = 0; k < A.c; ++k) {
							arr[i*B.c + j] += A[i*A.c + k] * B[k*B.c + j];
						}
					},
						std::ref(*this),
						std::ref(B),
						std::ref(arr),
						i, j
					);
				}
			}
		}

		matrix C = matrix(r, c, arr);
		return C;
	}


	void operator = (matrix other) {
		if (this == &other)
			return;

		r 	= other.r ;
		c 	= other.c ;
		ar 	= other.ar ;
	}

	void print() const {
		for (int i = 0; i < r; ++i) {
			for (int j = 0; j < c; ++j) {
				std::cout << ar[i * c + j] << "\t";
			}
			std::cout << "\n";
		}
		std::cout << "=====================\n";
	}

  public:
	int r, c;

  private:
	spt<double[]> ar;
};


matrix regular_add(matrix& A, matrix& B) {
	spt<double[]> arr = mkspt<double[]>(new double[A.r * A.c], [](double *p) { delete[] p; });
	matrix C(A.r, A.c, arr);

	for (int i = 0;  i < A.r; ++i) {
		for (int j = 0; j < B.c; ++j)
			C[i*A.c + j] = A[i*A.c + j] + B[i*A.c + j];
	}
	return C;
}

matrix regular_sub(matrix& A, matrix& B) {
	spt<double[]> arr = mkspt<double[]>(new double[A.r * A.c], [](double *p) { delete[] p; });
	matrix C(A.r, A.c, arr);

	for (int i = 0;  i < A.r; ++i) {
		for (int j = 0; j < B.c; ++j)
			C[i*A.c + j] = A[i*A.c + j] + B[i*A.c + j];
	}
	return C;

}

matrix regular_mul(matrix& A, matrix& B) {
	spt<double[]> arr = mkspt<double[]>(new double[A.r * A.c], [](double *p) { delete[] p; });
	matrix C(A.r, B.c, arr);

	for (int i = 0; i < A.r; ++i) {
		for (int j = 0; j < B.c; ++j) {
				C[i*A.r + j] = 0;
			for (int k = 0; k < A.c; ++k) {
				C[i*B.c + j] += A[i*A.c + k] * B[k*B.c + j];
			}
		}
	}

	return C;
}


#endif
