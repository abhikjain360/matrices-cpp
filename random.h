#ifndef RANDOM_H
#define RANDOM_H

#include "matrix.h"

#include <stdlib.h>
#include <ctime>
#include <iostream>

#define spt std::shared_ptr
#define mkspt std::shared_ptr


matrix get_random(int rows, int cols, int limit) {
	spt<double[]> arr = mkspt<double[]>(new double[rows * cols], [](double *p) { delete[] p; });
	std::srand(time(0));

	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			arr[i*cols + j] = (rand()%limit);
		}
	}

	matrix C(rows, cols, arr);
	return C;
}

#endif
