/*
 * link.cpp
 *
 *  Created on: 13/09/2012
 *      Author: romerito
 */

#include "link.h"

namespace rca {

Link::Link() : x(0), y(0), p(0.0 ){

}

Link::Link(int _v, int _w, double _p) {

	bool larger = false;
	larger = (_w > _v ? true : false);

	if (larger) {
		x = _w;
		y = _v;
	} else {
		x = _v;
		y = _w;
	}
	p=_p;
}

Link::~Link() {
}

bool Link::operator ==(const Link& link) const{
	return (x == link.x && y == link.y);
}

bool Link::operator !=(const Link& link) const {
	return (x != link.x || y != link.y);
}

bool Link::operator <(const Link& link) const {

	//return ((p < link.p) && (link != *this)) || (link != *this);
	return (p < link.p);

}

bool Link::operator >(const Link& link) const {

	//return ((p < link.p) && (link != *this)) || (link != *this);
	return (p > link.p);

}


Link& Link::operator =(const Link& link) {

	this->x = link.getX();
	this->y = link.getY();
	this->p = link.getValue();

	return (*this);
}

Link::Link(const Link& link) {
	x = link.getX();
	y = link.getY();
	p = link.getValue();

}

std::ostream& operator <<(std::ostream& out, const Link& obj) {

	out << obj.x;
	out << "--" ;
	out << obj.y;
	return out;

}

} /* namespace rca */
