

#ifndef GROUP_H_
#define GROUP_H_

#include <vector>
#include <ostream>
#include <algorithm>

namespace rca {

/**
 * Esta classe representa um grupo multicast.
 * Um grupo multicast consiste de um conjunto de nós na
 * rede que estabelecem um troca de informações entre eles.
 *
 * Os elementos que forma um grupo são: nó fonte (transmissor de
 * informação), e nós de destino (receptores de informação).
 *
 * Além disso, há um outro parâmetro de um grupo que é a requisição
 * de tráfego que o mesmo tem para estabelecer a comunicação entre os
 * seus membros.
 *
 * @date 27/09/2012
 * @author Roemrito Campos
 * @version 0.2
 */
class Group {
public:

	/**
	 * Construtor padrão.
	 */
	Group();

	/**
	 * Construtor com parâmetros. Este construtor passa
	 * os parâmetros necessários a construção de um grupo
	 * multicast. Apenas os membros de destino não são
	 * passados como parâmetro.
	 *
	 * O parâmetro id indica algum identificar de grupos
	 * na definiação de um problema com vários grupos multicast como
	 * é o caso do multicast Packing problem.
	 *
	 * @see MulticastPacking
	 *
	 * @param int id
	 * @param int source
	 * @param int requisição de tráfego
	 */
	Group(int id,int source,double trequest);

	/**
	* Destrutor.
	*/
	~Group();

	/**
	 * Método que imprime na saída padrão um grupo multicast.
	 * @param ostream
	 * @param Group
	 * @return ostream
	 */
	friend std::ostream& operator << (std::ostream & os, const Group & group);

	/**
	 * Método para retornar o id de um grupo.
	 * @return int
	 */
	int getId() const;

	/**
	 * Método para definir um id.
	 * @para int
	 */
	void setId(int id);

	/**
	 * Método para retornar o tamanho de um grupo.
	 * @return int
	 */
	int getSize() const;

	/**
	 * Método para definir um tamanho de um grupo.
	 * @param int número de membros do grupo excluíndo nó fonte.
	 */
	void setSize(int size);

	/**
	 * Método para retornar o nó fonte de um grupo.
	 * @return int nó fonte.
	 */
	int getSource() const;

	/**
	 * Método para definir o nó fonte de um grupo.
	 * @param int nó fonte.
	 */
	void setSource(int source);

	/**
	 * Método para retornar o valor de requisição de tráfego.
	 * @return double consumo de banda do grupo
	 */
	double getTrequest() const;

	/**
	 * Método para definir um valor de requisição de tráfego
	 * @param double consumo de banda do grupo
	 */
	void setTrequest(double trequest);

	/**
	 * Método para adicionar membros ao grupo multicast.
	 * Este método adiciona os nós de destinos.
	 *
	 * @param int
	 */
	void addMember (int member);

	/**
	 * Retorna o membro que está na posição poss.
	 * Os membros do grupo multicast são armazendo em um std::vector
	 * @param int posição do nó no vector
	 * @param int valor identificador do nó
	 */
	int getMember (int poss);

	/**
	 * Retorna um vector com todos os nós membros.
	 *
	 * @return vector<int> membros do grupo multicast
	 */
	const std::vector<int> & getMembers () const {return m_members;}
	
	/**
	 * Retornar um const_iterator para os mebros do grupo.
	 * O const_iterator corresponde ao primeiro elemento da
	 * lista.
	 *
	 * @return std::vector<int>::const_iterator begin iterator
	 */
	const std::vector<int>::const_iterator begin() {
	  return m_members.begin();
	}
	
	/**
	 * Retorna um const_iterator para os membros do grupo.
	 * O const_iterator corresponde ao último elemento da
	 * lista.
	 *
	 * return std::vector<int>::const_iterator ende iterator
	 */
	const std::vector<int>::const_iterator end() {
	  return m_members.end();
	}

	/**
	 * Verifica se um nó é membro do grupo multicast.
	 * @param int
	 * @return bool
	 */
	bool isMember (int);

private:
	int m_id;
	int m_source;
	double m_trequest; //traffic request
	int m_size;
	std::vector<int> m_members;

};

} /* namespace rca */
#endif /* GROUP_H_ */
