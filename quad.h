#pragma once

#include <memory>
#include <iostream>

#include "threadpool.h"

#define spt std::shared_ptr
#define mkspt std::shared_ptr


class quad {
  public:
	quad(
		u_int rows,
		u_int cols,
		u_int off,
		u_int start_x,
		u_int start_y,
		spt<double[]> &arr
	) :
		r(rows),
		c(cols),
		orig_c{off},
		x(start_x),
		y(start_y),
		ar(arr)
	{}

	double operator [] (u_int index) const {
		if (index > r*c)
			return 0;

		if (orig_c == 0)
			return ar[index];

		auto i = index/c, j = index%c;
		return ar[(x+i)*orig_c + y + j];
	}

	quad operator + (quad &B) {
		auto arr = mkspt<double[]>(new double[r * c], [](double *p) { delete[] p; });

		// scope to delete threadpool
		{
			threadpool tp(3);

			for (u_int i = 0; i < r; ++i)
			{
				tp.add(
				[] (quad &A, quad &B, spt<double[]> &arr, int i) {
					for (u_int j = 0; j < A.c; ++j)
					{
						arr[i*A.c + j] = A[i*A.c + j] + B[i*A.c + j];
					}
				},
					std::ref(*this),
					std::ref(B),
					std::ref(arr),
					i
				);
			}
		}

		// assigning variable later to save space on stack
		quad C = quad(r, c, 0, 0, 0, arr);
		return C;
	}

	quad operator - (quad &B) {
		auto arr = mkspt<double[]>(new double[r * c], [](double *p) { delete[] p; });

		// scope to delete threadpool
		{
			threadpool tp(3);

			for (u_int i = 0; i < r; ++i)
			{
				tp.add(
				[] (quad &A, quad &B, spt<double[]> &arr, int i) {
					for (u_int j = 0; j < A.c; ++j)
					{
						arr[i*A.c + j] = A[i*A.c + j] - B[i*A.c + j];
					}
				},
					std::ref(*this),
					std::ref(B),
					std::ref(arr),
					i
				);
			}
		}

		// assigning variable later to save space on stack
		quad C = quad(r, c, 0, 0, 0, arr);
		return C;
	}

	//quad operator * (quad &B) {
		//auto arr = mkspt<double[]>(new double[r * c], [](double *p) { delete[] p; });
//
		//// scope to delete threadpool
		//{
			//threadpool tp(3);
//
			//for (u_int i = 0; i < r; ++i) {
				//for (u_int j = 0; j < B.c; ++j) {
					//arr[i*r + j] = 0;
					//tp.add([] (quad &A, quad &B, spt<double[]>& arr, u_int i, u_int j) {
						//for (u_int k = 0; k < A.c; ++k) {
							//arr[i*B.c + j] += A[i*A.c + k] * B[k*B.c + j];
						//}
					//},
						//std::ref(*this),
						//std::ref(B),
						//std::ref(arr),
						//i, j
					//);
				//}
			//}
		//}
//
		//// assigning variable later to save space on stack
		//quad C = quad(r, c, 0, 0, 0, arr);
		//return C;
	//}

	quad operator * (quad &B);

	void print() const {
		for (u_int i = 0; i < r; ++i) {
			for (u_int j = 0; j < c; ++j)
				std::cout << (*this)[i * c + j] << "\t";
			std::cout << "\n";
		}
		std::cout << "=====================\n";
	}

	quad get_quad(u_int n, u_int size) {
		u_int start_x = 0, start_y = 0;

		if 	(n == 2) { start_y = size; }
		else if (n == 3) { start_x = size; }
		else if (n == 4) { start_x = start_y = size; }

		quad q(size, size, orig_c, start_x, start_y, ar);
		return q;
	}



  public:
	u_int r, c, orig_c;
	u_int x, y;
	std::shared_ptr<double[]> ar;
};

quad combine_quads(quad &A, quad&B, quad&C, quad&D) {
	auto rows = A.r + C.r,
	     cols = A.c + B.c;
	auto arr  = mkspt<double[]>(new double[rows * cols], [](double *p) { delete[] p; });

	for (u_int i = 0; i < A.r; ++i) {
		for (u_int j = 0; j < A.c; ++j) {
			arr[i*cols + j] = A[i*A.c + j];
		}

		for (u_int j = 0; j < B.c; ++j) {
			arr[i*cols + A.c + j] = B[i*B.c + j];
		}
	}

	for (u_int i = 0; i < C.r; ++i) {
		for (u_int j = 0; j < C.c; ++j) {
			arr[i*(A.c + cols) + j] = C[i*C.c + j];
		}

		for (u_int j = 0; j < D.c; ++j) {
			arr[i*(A.c + cols) + C.c + j] = D[i*D.c + j];
		}
	}

	return quad(rows, cols, 0, 0, 0, arr);
}

quad strassen(quad &A, quad &B) {
	u_int size = A.r / 2;

	// getting submatrices
	// little overhead as pointer passed only
	quad a = A.get_quad(1, size);
	quad b = A.get_quad(2, size);
	quad c = A.get_quad(3, size);
	quad d = A.get_quad(4, size);
	quad e = B.get_quad(1, size);
	quad f = B.get_quad(2, size);
	quad g = B.get_quad(3, size);
	quad h = B.get_quad(4, size);

	quad temp = f - h;
	quad p1 = a * temp;
	temp = a + b;
	quad p2 = temp * h;
	temp = c + d;
	quad p3 = temp * e;
	temp = g - e;
	quad p4 = d * temp;
	temp = e + h;
	quad p5 = a + d;
	p5 = p5 * temp;
	temp = b - d;
	quad p6 = g + h;
	p6 = temp * p6;
	temp = a - c;
	quad p7 = e + f;
	p7 = temp * p7;

	quad W = p5 + p4 + p6 - p2;
	quad X = p1 + p2;
	quad Y = p3 + p4;
	quad Z = p1 + p5 - p3 - p7;

	return combine_quads(W, X, Y, Z);
}

quad quad::operator * (quad &B) {
	return strassen(*this, B);
}
