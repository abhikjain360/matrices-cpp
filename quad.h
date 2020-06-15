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

		return ar[x*orig_c + y + index];
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

	quad operator * (quad &B) {
		auto arr = mkspt<double[]>(new double[r * c], [](double *p) { delete[] p; });

		// scope to delete threadpool
		{
			threadpool tp(3);

			for (u_int i = 0; i < r; ++i) {
				for (u_int j = 0; j < B.c; ++j) {
					arr[i*r + j] = 0;
					tp.add([] (quad &A, quad &B, spt<double[]>& arr, u_int i, u_int j) {
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
		quad C = quad(r, c, 0, 0, 0, arr);
		return C;
	}

	void print() const {
		for (u_int i = 0; i < r; ++i) {
			for (u_int j = 0; j < c; ++j) {
				std::cout << (*this)[i * c + j] << "\t";
			}
			std::cout << "\n";
		}
		std::cout << "=====================\n";
	}



  public:
	u_int r, c, orig_c;
	u_int x, y;
	std::shared_ptr<double[]> ar;
};
