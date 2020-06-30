#pragma once

/* standard libraries */
#include <iostream>
#include <cstdlib>
#include <memory>

class quad;

class matrix {
  public:
	/* constructors */
	matrix(unsigned int rows, unsigned int cols);
	matrix(unsigned int rows, unsigned int cols, std::shared_ptr<double[]> arr);

	/* deconstructor */
	~matrix();

	/* easy indexing */
	double& operator [] (unsigned int index) const;

	/* easy submatrixing */
	quad subquad(unsigned int index1, unsigned int index2) const;

	/* basic operations */
	matrix operator + (const matrix& B) const;
	matrix operator - (const matrix& B) const;
	matrix operator * (const matrix& B) const;

  public:
	unsigned int rows, cols;
	std::shared_ptr<double[]> arr;
  private:
};

std::ostream& operator << (std::ostream& output, const matrix& A);
