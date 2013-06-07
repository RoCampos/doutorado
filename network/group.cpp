#include "group.h"

namespace rca {

Group::Group() : m_id(0), m_source(0), m_trequest(0), m_size(0){

}

Group::~Group() {

}

int Group::getId() const {
	return m_id;
}

void Group::setId(int id) {
	m_id = id;
}

int Group::getSize() const {
	return m_size;
}

void Group::setSize(int size) {
	m_size = size;
}

int Group::getSource() const{
	return m_source;
}

void Group::setSource(int source) {
	m_source = source;
}

double Group::getTrequest() const {
	return m_trequest;
}

Group::Group(int id, int source, double trequest)
: m_id(id),m_source(source), m_trequest(trequest), m_size(0) {

}

void Group::setTrequest(double trequest) {
	m_trequest = trequest;
}

void Group::addMember(int member) {
	m_members.push_back(member);
	m_size++;
}

std::ostream& operator <<(std::ostream& os, const Group & group) {

	os << "Id: " << group.m_id << "\n";
	os << "Source: "<< group.m_source << "\n";
	os << "Size: " << group.m_size << "\n";
	os << "Request: " << group.m_trequest << "\n";

	os << "Group Members [ ";
	for (unsigned int i=0; i < group.m_members.size()-1; i++) {
		os << group.m_members[i] << " , ";
	}
	os << group.m_members[group.m_size-1];
	os << " ]\n";

	return os;
}

int Group::getMember(int poss) {
	return m_members[poss];
}


bool Group::isMember(int node) {
	return std::find (m_members.begin (), m_members.end(), node) != m_members.end();
}

} /* namespace rca */
