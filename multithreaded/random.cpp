#include <ctime>
#include <stdlib.h>
#include <memory>

#include "random.h"

quad getRandomQuad
(
	unsigned int rows,
	unsigned int cols,
	unsigned int limit
)
{
	quad M(rows, cols);

	std::srand(time(0));

	for (unsigned int i = 0; i < rows; ++i)
		for (unsigned int j = 0; j < cols; ++j)
			M[i*cols + j] = rand() % limit + 1;

	return M;
}

matrix getRandomMatrix
(
	unsigned int rows,
	unsigned int cols,
	unsigned int limit
)
{
	matrix M(rows, cols);

	std::srand(time(0));

	for (unsigned int i = 0; i < rows; ++i)
		for (unsigned int j = 0; j < cols; ++j)
			M[i*cols + j] = rand() % limit + 1;

	return M;
}
