#pragma once

#include "matrix.h"
#include "quad.h"

quad getRandomQuad
(
	unsigned int rows,
	unsigned int cols,
	unsigned int limit
);

matrix getRandomMatrix
(
	unsigned int rows,
	unsigned int cols,
	unsigned int limit
);
