#pragma once

#include <ctime>
#include <stdlib.h>
#include <memory>

#include "matrix.h"

template <typename T>
matrix<T> getRandomMatrix(unsigned int rows, unsigned int cols, unsigned int limit);

template <typename T>
quad<T> getRandomQuad(unsigned int rows, unsigned int cols, unsigned int limit);



template <typename T>
matrix<T> getRandomMatrix(unsigned int rows, unsigned int cols, unsigned int limit)
{
	matrix<T> M(rows, cols);

	std::srand(time(0));

	for (unsigned int i = 0; i < rows; ++i)
		for (unsigned int j = 0; j < cols; ++j) {
			M[i*cols + j] = rand() % limit + 1;
			//M[i*cols + j] *= rand()%2 ? 1 : -1;
		}

	return M;
}

template <typename T>
quad<T> getRandomQuad(unsigned int rows,unsigned int cols,unsigned int limit)
{
	quad<T> M(rows, cols);

	std::srand(time(0));

	for (unsigned int i = 0; i < rows; ++i)
		for (unsigned int j = 0; j < cols; ++j) {
			M[i*cols + j] = rand() % limit + 1;
			//M[i*cols + j] *= rand()%2 ? 1 : -1;
		}

	return M;
}
