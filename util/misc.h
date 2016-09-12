#ifndef __MISCELLANEOUS__
#define __MISCELLANEOUS__

#include <vector>

namespace rca {

namespace func{

	std::vector<int> get_random_vector(int size) {
		std::vector<int> index(size, 0);
		iota (index.begin(), index.end(), 0);	
		std::random_shuffle (index.begin(), index.end());
		return index;
	}

	bool in (std::vector<int> & list, int value) {
		auto it = std::find (list.begin (), list.end(), value);
		return it != list.end();
	}

}


namespace dstructure {
	
/**
* Classe template utilizada para representar 
* uma matriz.
*
* A matriz é representa com um std::vector de std::vector
*
*
* @author Romerito C. Andrade
*/
template <typename Type>
class Matrix {

public:

	/**
	* Construtor da classe.
	*
	* 
	* @param int ordem da matriz,
	*/
	Matrix (int order) {
		
		m_order = order;
		
		m_matrix = std::vector<std::vector<Type>> (order);
		for (size_t i = 0; i < m_matrix.size (); i++) {
			m_matrix[i] = std::vector<Type> (order);
		}
	}
	
	/**
	* Método para obter a ordem da matriz.
	*
	* @return size_t ordem da matriz.
	*/
	size_t order () {
		return m_order;
	}
	
	/**
	* Método utilizado para adicionar um novo
	* elemento a matriz.
	*
	* @param int linha
	* @param int coluna
	* @param Type tipo de elemento adicionado a matriz.
	*/
	void add (int row, int col, Type & elem) {
		m_matrix [row][col] = elem;
	}

	/**
	* Método para acessar uma célula da matriz.
	* 
	* @param int linha da matriz.
	* @param int coluna da matriz.
	* @return Type valor da célula acessado
	*/
	const Type & get (int row, int col) const{
		return m_matrix[row][col];
	}
	
	/**
	* Método para comparar um elemento externo com um 
	* elemento da matriz por meio de linha, coluna passados
	* como parâmetro.
	*
	* @param int linha
	* @param int coluna
	* @param Type valor a ser comparado
	* @return bool resultado da comparação(verdadeiro ou falso)
	*
	*/
	bool compare (int row, int col, const Type & elem) {
	
		return m_matrix[row][col] == elem;
	}
	
	/**
	* Método para inicializar todas as células da matriz
	* com um valor específico que é passado por parâmetro
	*
	* @param Type elemento a ser adicionado a todas as células.
	*/	
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