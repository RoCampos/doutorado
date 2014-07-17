#include "mppalgorithm.h"

using namespace rca;
using namespace std;

MPPAlgorithm::MPPAlgorithm () {
  
}

MPPAlgorithm::MPPAlgorithm (std::string algo, std::string problem, 
			    int objectives, std::string instance) {
  
  m_algo_info = algo;
  m_problem = problem;
  m_objectives = objectives;
  m_instance = instance;
  
}	

void MPPAlgorithm::init (rca::Network & network, 
			 std::vector<std::shared_ptr<Group>> groups) {
  
#ifdef DEBUG
  cout << "Method Init: Line " <<__LINE__ << "." << endl;
  cout << "Date: "<< __DATE__ << endl;
  cout << "Instance used: " << m_instance << endl;
#endif
  
  m_net = make_shared<rca::Network> (network);
  m_groups = groups;
  
}


