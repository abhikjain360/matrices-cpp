#ifndef INPUT_H
#define INPUT_H

#include "matrix.h"

#include <vector>

#define spt std::shared_ptr
#define mkspt std::shared_ptr

matrix matrix_input(std::istream& stream) {
	std::string inputline, nums;
	int rows, cols;

	stream >> rows >> cols;

		spt<double[]> arr = mkspt<double[]>(new double[rows * cols]);

	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			stream >> arr[i * cols + j];
		}
	}

	matrix C = matrix(rows, cols, arr);
	return C;
}


#endif
