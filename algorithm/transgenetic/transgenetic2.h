#ifndef _TRANSGENET2_H_
#define _TRANSGENET2_H_

class Transgenetic2 {

public:
	
	Transgenetic2 (int pop_size) : m_pop_size (pop_size) {
	
	void create_population ();
	void run ();
	

private:
	
	int m_pop_size;
};


#endif /* _TRANSGENET2_H_ */