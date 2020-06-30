#pragma once

/* standard libraries */
#include <iostream>
#include <cstdlib>
#include <memory>

#define ASSERT(x) if(!(x)) { print_quad_error(#x, __FILE__, __LINE__);exit(-1); };

void print_quad_error(std::string condition, std::string file, int line);

class quad {
  public:
	/* constructors */
	quad(unsigned int rows, unsigned int cols);
	quad
	(
	 	unsigned int rows,
		unsigned int cols,
		const std::shared_ptr<double[]>& arr,
		unsigned int row_offset = 0,
		unsigned int col_offset = 0
	);
	quad
	(
	 	unsigned int rows,
		unsigned int cols,
		const std::shared_ptr<double[]>& arr,
		unsigned int orig_cols,
		unsigned int row_offset = 0,
		unsigned int col_offset = 0
	);

	/* deconstructor */
	~quad();

	/* easy indexing */
	double& operator [] (unsigned int index) const;

	/* easy subquading */
	quad subquad(unsigned int index1, unsigned int index2) const;

	/* basic operations */
	quad operator + (const quad& B) const;
	quad operator - (const quad& B) const;
	quad operator * (const quad& B) const;

  public:
	unsigned int rows, cols;
  private:
	std::shared_ptr<double[]> arr;
	unsigned int orig_cols;
	unsigned int row_offset, col_offset;
};

std::ostream& operator << (std::ostream& output, const quad& A);
