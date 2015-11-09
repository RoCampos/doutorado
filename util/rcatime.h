#ifndef _MY_UTIL_H_
#define _MY_UTIL_H_

#include <chrono>

namespace rca {

/**
* Estrutura utilizada para marcar tempo de processamento
* de partes do código fonte.
*
* Este código utiliza a api chrono do c++.
* 
* @author Romerito C. Andrade
*/
struct elapsed_time {

	std::chrono::high_resolution_clock::time_point m_start;
	std::chrono::high_resolution_clock::time_point m_finish;
	
	/**
	* Método que registra o momento inicial da execução
	* do trecho do código.
	*
	*/
	void started ();

	/**
	* Método utilizado para registrar o momento final de execução
	* do trecho de código que se prentende cronometrar.
	*/
	void finished ();
	
	/**
	* Método que retona a duração da marcação do tempo.
	* 
	* O método retorna a contagem do clock que pode ser utilizado
	* para que se faça o cálculo do tempo.
	* @return double contagem do clock
	*/
	double get_elapsed ();
		
};

};

#endif 
