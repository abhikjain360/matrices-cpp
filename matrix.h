#ifndef MATRIX_H
#define MATRIX_H

#include "threadpool.h" 	// for multithreading

#include <iostream> 		// for debugging, remove later

#define spt std::shared_ptr
#define mkspt std::shared_ptr


class matrix {
  public:
	matrix(u_int rows, u_int cols, spt<double[]> &arr) : r(rows), c(cols), ar(arr) {}

	double &operator [] (u_int index) {
		return ar[index];
	}

	matrix operator + (matrix &B) {
		auto arr = mkspt<double[]>(new double[r * c], [](double *p) { delete[] p; });

		// scope to delete threadpool
		{
			threadpool tp(3);

			for (u_int i = 0; i < r; ++i)
			{
				tp.add(
				[] (matrix &A, matrix &B, spt<double[]> &arr, int i) {
					for (u_int j = 0; j < A.c; ++j)
						arr[i*A.c + j] = A[i*A.c + j] + B[i*A.c + j];
				},
					std::ref(*this),
					std::ref(B),
					std::ref(arr),
					i
				);
			}
		}

		// assigning variable later to save space on stack
		matrix C = matrix(r, c, arr);
		return C;
	}

	matrix operator - (matrix &B) {
		auto arr = mkspt<double[]>(new double[r * c], [](double *p) { delete[] p; });

		// scope to delete threadpool
		{
			threadpool tp(3);

			for (u_int i = 0; i < r; ++i)
			{
				tp.add(
				[] (matrix &A, matrix &B, spt<double[]> &arr, u_int i) {
					for (u_int j = 0; j < A.c; ++j)
						arr[i*A.c + j] = A[i*A.c + j] - B[i*A.c + j];
				},
					std::ref(*this),
					std::ref(B),
					std::ref(arr),
					i
				);
			}
		}

		// assigning variable later to save space on stack
		matrix C = matrix(r, c, arr);
		return C;
	}

	matrix operator * (matrix &B) {
		auto arr = mkspt<double[]>(new double[r * c], [](double *p) { delete[] p; });

		// scope to delete threadpool
		{
			threadpool tp(3);

			for (u_int i = 0; i < r; ++i) {
				for (u_int j = 0; j < B.c; ++j) {
					arr[i*r + j] = 0;
					tp.add([] (matrix &A, matrix &B, spt<double[]>& arr, u_int i, u_int j) {
						for (u_int k = 0; k < A.c; ++k) {
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

		// assigning variable later to save space on stack
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
		for (u_int i = 0; i < r; ++i) {
			for (u_int j = 0; j < c; ++j) {
				std::cout << ar[i * c + j] << "\t";
			}
			std::cout << "\n";
		}
		std::cout << "=====================\n";
	}

  public:
	u_int r, c;

  private:
	spt<double[]> ar;
};


matrix regular_add(matrix& A, matrix& B) {
	auto arr = mkspt<double[]>(new double[A.r * A.c], [](double *p) { delete[] p; });

	for (u_int i = 0;  i < A.r; ++i) {
		for (u_int j = 0; j < B.c; ++j)
			arr[i*A.c + j] = A[i*A.c + j] + B[i*A.c + j];
	}

	// assigning variable later to save space on stack
	matrix C(A.r, A.c, arr);
	return C;
}

matrix regular_sub(matrix& A, matrix& B) {
	auto arr = mkspt<double[]>(new double[A.r * A.c], [](double *p) { delete[] p; });

	for (u_int i = 0;  i < A.r; ++i) {
		for (u_int j = 0; j < B.c; ++j)
			arr[i*A.c + j] = A[i*A.c + j] + B[i*A.c + j];
	}

	// assigning variable later to save space on stack
	matrix C(A.r, A.c, arr);
	return C;

}

matrix regular_mul(matrix& A, matrix& B) {
	auto arr = mkspt<double[]>(new double[A.r * A.c], [](double *p) { delete[] p; });

	for (u_int i = 0; i < A.r; ++i) {
		for (u_int j = 0; j < B.c; ++j) {
				arr[i*A.r + j] = 0;
			for (u_int k = 0; k < A.c; ++k) {
				arr[i*B.c + j] += A[i*A.c + k] * B[k*B.c + j];
			}
		}
	}

	// assigning variable later to save space on stack
	matrix C(A.r, B.c, arr);
	return C;
}


#endif
