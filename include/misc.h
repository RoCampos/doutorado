#ifndef __MISCELLANEOUS__
#define __MISCELLANEOUS__

#include <vector>

namespace rca {

namespace dstructure {
	
template <typename Type>
class Matrix {

public:
	Matrix (int order) {
		
		m_order = order;
		
		m_matrix = std::vector<std::vector<Type>> (order);
		for (size_t i = 0; i < m_matrix.size (); i++) {
			m_matrix[i] = std::vector<Type> (order);
		}
	}
	
	size_t order () {
		return m_order;
	}
	
	void add (int row, int col, Type & elem) {
		m_matrix [row][col] = elem;
	}

	const Type & get (int row, int col) const{
		return m_matrix[row][col];
	}
	
	bool compare (int row, int col, const Type & elem) {
	
		return m_matrix[row][col] == elem;
	}
	
	void fill (Type & elem) {
	
		for (size_t i = 0; i < this->m_order; i++) {
			for (size_t j = 0; j < this->m_order; j++)	{
				
				m_matrix[i][j] = elem;
				
			}
		}
		
	}
	
private:
	
	std::vector<std::vector<Type>> m_matrix;
	int m_order;
	
};	
	
};
	
};

#endif