/*
 * time.cpp
 *
 *  Created on: 22/10/2012
 *      Author: romerito
 */

#include "time.h"

namespace rca {

Time::Time() {

	gettimeofday(&m_begin,NULL);

}


Time::~Time() {

}

void Time::getElapsedTime() {

	timeval end;
	gettimeofday(&end,NULL);

	long int t_begin = (m_begin.tv_usec + (1000000 * m_begin.tv_sec));
	long int t_end = (end.tv_usec + (1000000 * end.tv_sec));
	long int diff = t_end - t_begin;

	printf ("%ld.%ld\n",diff/1000000, diff%1000000);
}

void Time::getElapsedTime(std::fstream &file) {

	timeval end;
	gettimeofday(&end,NULL);

	long int t_begin = (m_begin.tv_usec + (1000000 * m_begin.tv_sec));
	long int t_end = (end.tv_usec + (1000000 * end.tv_sec));
	long int diff = t_end - t_begin;

	file << std::setprecision(3) <<diff/1000000 <<".";
	file << std::setprecision(3)<<(diff%1000000) << "\n";

}

} /* namespace rca */
