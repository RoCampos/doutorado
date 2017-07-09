#ifndef _MULTI_OBJECTIVE_IMPL_H_
#define _MULTI_OBJECTIVE_IMPL_H_

#include <cmath>
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

using std::cout;
using std::endl;

#include "problem_info.h"

namespace MultiObjective {

	/**
	 * Está função verifica se o vetor2 é dominado pelo vetor1
	 * A verificação é feita para todos os objetivos.
	 *
	 * Todos os objetivos são considerados como minimização.
	 * Portanto, caso algum objetivo do vetor1 tenha valor maior
	 * que o mesmo objetivo do vetor2 então é considerado que ele
	 * não domina.
	 *
	 * Se em todos os objetivos a propriedade acima não é encontrada
	 * então o vetor2 não é dominado pelo vetor1.
	 *
	 * @date 11/10/2012
	 * @author Romerito Campos
	 */
	int dominate (double *vetor1, double *vetor2, int *);

	/**
	 * Está função calcula a distância euclidiana entre dois
	 * pontos no espaço n-dimensinal.
	 *
	 * @date 11/10/2012
	 * @author Romerito Campos
	 */
	double distanceCalc (double *, double *, int);
	double distanceCalc (std::vector<double> &, std::vector<double> &, int);

	double thebycheffWeighet (
		std::vector<double> &obj, 
		std::vector<double> &lambda);

	void normalization (std::vector<double> &obj, 
		std::vector<double> &nadir, 
		std::vector<double> &ideal, int *objconf);

	/* Recebe um arquivo de texto filtra os não dominados*/
	void filter (std::string file_name, int, int*);

	bool equal (double *, double *);

	enum {DOMINATED = 1, NONDOMINTED = 0};

struct objective_t
{
	int index;
	std::vector<double> objectives;
};


struct pareto_t
{
	std::vector<std::vector<objective_t>> pareto;

	pareto_t (int size) {
		pareto.resize (size);
	}

	void update (int index, std::vector<double> & obj) {
		double * obj1 = new double[3];
		double * obj2 = new double[3];
		int * objconf = Info::Problem::get_instance ()->objconf;

		obj1[0] = obj.at (0);
		obj1[1] = obj.at (1);
		obj1[2] = obj.at (2);

		int counter = 0;
		for (size_t i = 0; i < pareto.size (); ++i)
		{
			std::vector<int> remove;
			std::vector<objective_t> & ref = pareto.at (i);
			for (size_t j = 0; j < ref.size(); ++j)
			{				
				obj2[0] = ref[j].objectives.at (0);
				obj2[1] = ref[j].objectives.at (1);
				obj2[2] = ref[j].objectives.at (2);
				bool rem = false;
				if (dominate(obj1, obj2, objconf))
				{
					rem = true;
				} if (dominate(obj2, obj1, objconf)) 
				{
					rem = false;
					return;
				}
				if (rem) remove.push_back (j);				
			}			

			//removing dominated by obj
			auto iter = remove.rbegin ();
			for (; iter != remove.rend (); iter++) {
				ref.erase (ref.begin() + * iter);	
			}

		}

		delete obj1;
		delete obj2;
		objconf = NULL;
	}	

};

	
}
#endif 

namespace MultiObjective {
	
	int dominate (double * vetor1, double * vetor2, int * objconf) {
		for (int i=0; i < Info::Problem::get_instance ()->OBJECTIVES; i++) {			
			//se algum vetor1[i] é maior que vetor2[i] então vetor1 não domina vetor2
			if (vetor1[i] * objconf[i] > vetor2[i] * objconf[i]) {
				return NONDOMINTED;
			}
		}		
		//se nenhum vetor[i] é maior que vetor2[i] então vetor1 domina vetor2
		return DOMINATED;
	}
	
	double distanceCalc (double * vector1, double * vector2, int OBJECTIVES) {
		double sum = 0.0;
		for (int i = 0; i < OBJECTIVES; i++) {
			sum += pow(vector1[i] - vector2[i], 2.0);
		}		
		
		return sqrt (sum);
	}

	double distanceCalc (
		std::vector<double> & vector1, 
		std::vector<double> & vector2, int OBJECTIVES) 
	{
		double sum = 0.0;
		for (int i = 0; i < OBJECTIVES; i++) {
			sum += pow(vector1[i] - vector2[i], 2.0);
		}		
		
		return sqrt (sum);
	}

	double thebycheffWeighet (
		std::vector<double> &obj, 
		std::vector<double> &lambda) {

		double max = std::numeric_limits<double>::min();

		for (size_t i = 0; i < obj.size (); ++i)
		{
			//considering obj vector is normalized
			double diff = obj.at (i);
			double fvall;

			if (lambda.at (i) == 0) {
				fvall = 0.0001*diff;
			} else {
				fvall = diff*lambda.at (i);
			}

			if (fvall > max) max = fvall;
		}
		//max as fitness
		return max;
	}

	void normalization (std::vector<double> &obj, 
		std::vector<double> &nadir, 
		std::vector<double> &ideal, int *objconf)
	{

		for (size_t i = 0; i < obj.size(); ++i)
		{
			double normaval = 0;
			if (objconf[i] < 0) {
				//when maximizes
				normaval = (ideal.at(i)-obj.at(i))/(ideal.at(i)-nadir.at(i));
			} else {
				//when minimizes
				normaval = (obj.at(i)-ideal.at (i))/(nadir.at(i)-ideal.at(i));
			}
			obj.at (i) = normaval;
		}
	}

	void filter(std::string file_name, int OBJECTIVES, int * objconf) {

		std::ifstream file (file_name.c_str(), std::ifstream::in);

		int size = 0;
		char l;
		std::string ss;
		double value = 0.0;

		double **individuals;

		while ( !file.get(l).eof() ) {

			if (l != ' ' && l != '\n') {
				ss.push_back(l);
			}

			if (l == ' ') {
				ss.clear();
			}

			if ( l == '\n') {
				ss.clear();
				size++;
			}
		}

		individuals = new double*[size];
		for (int i=0; i < size; ++i) {
			individuals[i] = new double[OBJECTIVES];
		}

		file.close();

		file.open(file_name.c_str(), std::ifstream::in);

		size = 0;
		int obj = 0;
		while ( !file.get(l).eof() ) {

			if (l != ' ' && l != '\n') {
				ss.push_back(l);
			}

			if (l == ' ') {

				value = atof(ss.c_str());
				individuals[size][obj] = value;
				obj++;
				ss.clear();
			}

			if ( l == '\n') {
				value = atof(ss.c_str());
				individuals[size][obj] = value;
				ss.clear();

				size++;
				obj = 0;
			}
		}


		for (int i=0; i < size; ++i) {

			int count = 0;
			int j = 0;
			for (; j < size; ++j) {

				if (j==i) continue;

				if (dominate(individuals[j],individuals[i], objconf)) {
					count++;
				}
			}

			if (count == 0) {
				cout << individuals[i][0] << " " << individuals[i][1] << endl;
			}

		}

		//freeing memory
		for (int i=0; i < size; ++i) {
			delete individuals[i];
		}
		delete [] individuals;
	}

	bool equals (double * obj1, double * obj2, int OBJECTIVES) {
		bool compare = true; 
		for (int i=0; i < OBJECTIVES; i++) {
			if (obj1[i] == obj2[i]) {
				compare = compare && true;
			} else {
				return false;
			}
		}
		return compare;
	}


}
