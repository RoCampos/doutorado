#include "rcatime.h"


using namespace rca;


void elapsed_time::started () {
	
	m_start = std::chrono::high_resolution_clock::now ();
	
}

void elapsed_time::finished () {
	
	m_finish = std::chrono::high_resolution_clock::now ();
	
}	

double elapsed_time::get_elapsed () {

	std::chrono::duration<double> time_span;
	time_span = std::chrono::duration_cast<std::chrono::duration<double>> (m_finish - m_start);
	
	return time_span.count ();
}