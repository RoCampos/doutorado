#ifndef _MATRIX2D_H_
#define _MATRIX2D_H_

#include <ostream>
#include <vector>
#include <iterator>

namespace dl {

// Two-dimensional rectangular matrix.
//template <class T>
/**
* Classe que representa uma matrix de ordem numrows x numcolumns.
* 
* A matriz é representa como um vector. Para acessar uma posição faz
* o cálculo da posição referente a uma célula (linha, coluna).
*
* Para acessar a célula (linha, coluna) é necessário encontrar a posição
* da linha no vector: coluna * numrows + linha
*
* @author Dann Luciano
*/
class matrix2d {
  public:

    /**
    * Construtor padrão.
    */
    matrix2d();        

    /**
    * Construtor que recebe o número de colunas e linhas da matrix.
    *
    * @param unsigned número de linhas
    * @param unsigned número de colunas
    */
    matrix2d(unsigned num_rows, unsigned num_cols);

    /**
    * Construtor que recebe o número de colunas e o número de linhas
    * da matriz.
    *
    * Além disso, recebe um valor inicial para todas as células da matriz.
    *
    * @param unsigned número de linhas
    * @param unsigned número de colunas
    * @param double valor inicial
    */
    matrix2d(unsigned num_rows, unsigned num_cols, const double& init_value);
	
    /**
    * Método que acessa o número de linhas
    * @return unsigned número de linhas
    */
    unsigned num_rows() const;

    /**
    * Método que acessa o número de colunas
    * @return unsigned número de colunas
    */
    unsigned num_cols() const;

    // Accessors: return element by row and column.
    /**
    * Método para acessar o conteúdo de uma célula.
    * 
    * @param unsigned linha
    * @param unsigned coluna
    * @return double valor da célula (linha, coluna)
    */
    double& operator() ( unsigned row, unsigned col);

    /**
    * Método para acessar o conteúdo de uma célula.
    * 
    * @param unsigned linha
    * @param unsigned coluna
    * @return double valor da célula (linha, coluna) - const
    */
    const double& operator() ( unsigned row, unsigned col) const;

     /**
    * Método para acessar o conteúdo de uma célula.
    * 
    * @param unsigned linha
    * @param unsigned coluna
    * @return double valor da célula (linha, coluna)
    */
    double& at ( unsigned row, unsigned col);

     /**
    * Método para acessar o conteúdo de uma célula.
    * 
    * @param unsigned linha
    * @param unsigned coluna
    * @return double valor da célula (linha, coluna) - const
    */
    const double& at ( unsigned row, unsigned col) const;

     /**
    * Método para acessar o conteúdo de uma célula.
    * 
    * @param unsigned posição na represeção de vector 
    * @return double valor da célula (linha, coluna) associada a pos
    */
    double& operator[] ( unsigned pos);
  
    /**
    * Iterator na linha associada a pos
    * 
    * @param unsigned posição no vector
    * @return std::vector<double>::iterator
    */
    std::vector<double>::iterator beginLine (unsigned pos);
    
    /**
    * End Iterator na linha associada a pos
    * 
    * @param unsigned posição no vector
    * @return std::vector<double>::iterator
    */
    std::vector<double>::iterator endLine (unsigned pos);
    
    /**
    * Método utilizado para encontrar a posição de uma célula
    * no vector que representa o grafo.
    *
    * @param unsigned linha
    * @param unsigned coluna
    
    * @return unsigned indice do vector associado célula (linha, coluna)
    */
    unsigned index(unsigned row, unsigned col) const;
  

    /**
    * Método utilizado para atribuir um valor a célula (linha, coluna)
    *
    * @param unsigned linha
    * @param unsigned coluna
    * @param double valor para ser associado a (linha, coluna)
    */
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
