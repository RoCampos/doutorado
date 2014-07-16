#include "mppalgorithm.h"

MPPAlgorithm::MPPAlgorithm () {

}

MPPAlgorithm::MPPAlgorithm (std::string algo, std::string problem, int objectives) {
  
  m_algo_info = algo;
  m_problem = problem;
  m_objectives = objectives;
  
}

