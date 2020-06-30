#pragma once

/* standard libraries */
#include <iostream>
#include <cstdlib>
#include <memory>

#define ASSERT(x) if(!(x)) { print_matrix_error(#x, __FILE__, __LINE__);exit(-1); };

void print_matrix_error(std::string condition, std::string file, int line);

class matrix {
  public:
	/* constructors */
	matrix(unsigned int rows, unsigned int cols, std::shared_ptr<double[]> arr);
	matrix(unsigned int rows, unsigned int cols);

	/* deconstructor */
	~matrix();

	/* easy indexing */
	double& operator [] (unsigned int index) const;

	/* basic operations */
	matrix operator + (const matrix& B) const;
	matrix operator - (const matrix& B) const;
	matrix operator * (const matrix& B) const;

  public:
	unsigned int rows, cols;
  private:
	std::shared_ptr<double[]> arr;
};

std::ostream& operator << (std::ostream& output, const matrix& A);
