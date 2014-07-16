#include "mppalgorithm.h"

using namespace rca;
using namespace std;

MPPAlgorithm::MPPAlgorithm () {
  
}

MPPAlgorithm::MPPAlgorithm (std::string algo, std::string problem, int objectives) {
  
  m_algo_info = algo;
  m_problem = problem;
  m_objectives = objectives;
  
}	

void MPPAlgorithm::init (std::string instance) {
  
  m_instance = instance;  
  m_net = make_shared<Network> ();  
  Reader r(instance);
  r.configNetwork ( m_net.get() );
  
}

