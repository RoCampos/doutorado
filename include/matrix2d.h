#ifndef _MATRIX2D_H_
#define _MATRIX2D_H_

#include <ostream>
#include <vector>
#include <iterator>

namespace dl {

// Two-dimensional rectangular matrix.
//template <class T>
class matrix2d {
  public:

    matrix2d();        

    matrix2d(unsigned num_rows, unsigned num_cols);

    matrix2d(unsigned num_rows, unsigned num_cols, const double& init_value);
	
    unsigned num_rows() const;
    unsigned num_cols() const;

    // Accessors: return element by row and column.
    double& operator() ( unsigned row, unsigned col);

    const double& operator() ( unsigned row, unsigned col) const;

    double& at ( unsigned row, unsigned col);

    const double& at ( unsigned row, unsigned col) const;

    double& operator[] ( unsigned pos);
  
    std::vector<double>::iterator beginLine (unsigned pos);
    
    std::vector<double>::iterator endLine (unsigned pos);
    
    unsigned index(unsigned row, unsigned col) const;
  
	void assign (unsigned row, unsigned col, double& value);
    
private:
    unsigned rows_, cols_;
    std::vector<double> matrix;
};

}  // namespace dl

//template <class T>
inline
std::ostream& operator << (std::ostream& os, const dl::matrix2d& m) {
    for( unsigned r = 0; r < m.num_rows(); ++r ) {
        for( unsigned c = 0; c < m.num_cols(); ++c ) {
            os << m(r,c);
        }
        os << std::endl;
    }
    return os;
}

#endif /* _MATRIX2D_HPP_ */
