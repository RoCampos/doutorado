#ifndef MYTIME_H_
#define MYTIME_H_

#include <sys/time.h>
#include <sstream>
#include <string>
#include <fstream>
#include <iomanip>

namespace rca {

class Time {
public:
	Time ();
	~Time();

	void getElapsedTime ();
	void getElapsedTime (std::fstream &);

private:
	struct timeval m_begin;

};

} /* namespace rca */
#endif /* TIME_H_ */
