#include <iostream>
#include <random>
#include <chrono>

namespace rca {

/**
 * Este struct é usada para facilitar a manipulação de 
 * de geração de números aleatórios
 * 
 * É possível usar diferentes tipos de geradores de números aleatórios
 * fornecidos pelo lib random do c++11
 * 
 * @author Romerito Campos
 * @version 1.0
 * @date 09/03/2014
 */
template<class Engine, class Dist, typename TypeValue>
struct myrandom {

	Engine engine;
	Dist distribution;
	
	/**
	* Construtor padrão.
	*
	*/
	myrandom(){}
	
	/**
	* Construtor de cópia.
	*
	* @param myrandom<Engine, Dist, TypeValue>
	*/
	myrandom(const myrandom<Engine, Dist, TypeValue> & ref) {
		
		engine = ref.engine;
		distribution = ref.distribution;
	}
	
	/**
	* Construtor que recebe uma semente e os limites
	* do intervalo para gerar os números aleatórios.
	*
	* @param long semente a ser utilizada.
	* @param TypeValue limite inferior do intervalo
	* @param TypeValue limite superior do intervalo
	*/
	myrandom (long seed, TypeValue a, TypeValue b) {
		
		//initializing the engine
		engine = Engine (seed);
		
		//initializing the distribution object
		distribution = Dist (a, b);
	}
	
	/**
	* Construtor que recebe uma engine e os limites
	* do intervalo.
	*
	* @param Engine engine a ser utilizada.
	* @param TypeValue limite inferior
	* @param TypeValue limite superior
	*/
	myrandom (const Engine & e, TypeValue a, TypeValue b) {
		engine = e;
		distribution = Dist (a, b);
	}
	
	/**
	* Método para redefinir o intervalo de geração de números
	* A semente utilizado continua a mesma.
	*
	* @param TypeValue limite inferior novo
	* @param TypeValue limite superior novo
	*/
	void reset_interval (TypeValue a, TypeValue b) {
		distribution = Dist (a,b);
	}
	
	/**
	* Método para obter novo valor do gerador.
	*
	* @return TypeValue novo valor aleatório gerado
	*/
	TypeValue rand () {
		TypeValue w = distribution(engine);
		return w;
	}
	
	/**
	* Método que acessa engine utilizada para gerar números.
	*
	* @return Engine engine utilizada para gerar números
	*/
	const Engine& get_engine () const{
		return engine;
	}
	
};

/**
 * This struct is used to generate 
 * a seed to the number generator.
 *
 * Estrutura utilizada para gerar uma semente para o
 * gerador de números aleatórios.
 * 
 * @author Romerito Campos.
 * @date 09/04/2014
 */
struct myseed {

	/**
	* Método utilizado para gerar uma semente para um
	* gerador aletatório. 
	*
	* Utiliza a api chrono para gerar a semente.
	*
	* @return long semente gerada
	*/
	static long seed () {		
		long value = std::chrono::system_clock::now (). time_since_epoch (). count ();
		return value;
	}
	
};
	
}

template class rca::myrandom<std::mt19937, std::uniform_int_distribution<int>, int>;
template class rca::myrandom<std::mt19937, std::uniform_real_distribution<double>, double>;