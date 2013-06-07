#include "matrix2d.h"

using namespace dl;

//template<class T>
//matrix2d<T>::matrix2d() : rows_(0), cols_(0), matrix(0) {
matrix2d::matrix2d() : rows_(0), cols_(0), matrix(0) {
	
}

//template<class T>
//matrix2d<T>::matrix2d(unsigned num_rows, unsigned num_cols)
matrix2d::matrix2d(unsigned num_rows, unsigned num_cols)
        : rows_(num_rows), cols_(num_cols), matrix(num_rows * num_cols) {
}

//template<class T>
//matrix2d<T>::matrix2d(unsigned num_rows, unsigned num_cols, const T& init_value)
matrix2d::matrix2d(unsigned num_rows, unsigned num_cols, const double& init_value)
        : rows_(num_rows), cols_(num_cols), matrix(num_rows * num_cols, init_value) {
}

//template<class T>
//T& matrix2d<T>::operator() ( unsigned row, unsigned col) {
double& matrix2d::operator() ( unsigned row, unsigned col) {
        return matrix[ index( row, col ) ];
}

//template<class T>
//const T& matrix2d<T>::operator() ( unsigned row, unsigned col) const {
const double& matrix2d::operator() ( unsigned row, unsigned col) const {
     return matrix[ index( row, col ) ];
}

//template<class T>
//T& matrix2d<T>::at ( unsigned row, unsigned col) {
double& matrix2d::at ( unsigned row, unsigned col) {
    //return matrix[ index( row, col ) ];
    return matrix[ (col * rows_ + row) ];
}

//template<class T>
//const T& matrix2d<T>::at ( unsigned row, unsigned col) const {
const double& matrix2d::at ( unsigned row, unsigned col) const {
	return matrix[ (col * rows_ + row) ];
}

//template<class T>
//T& matrix2d<T>::operator[] ( unsigned pos) {
double& matrix2d::operator[] ( unsigned pos) {
	return matrix[pos];
}
 
//template<class T>
//typename std::vector<T>::iterator matrix2d<T>::beginLine (unsigned pos) {
std::vector<double>::iterator matrix2d::beginLine (unsigned pos) {
	return matrix.begin () + pos;
}

///template<class T>
//typename std::vector<T>::iterator matrix2d<T>::endLine (unsigned pos) {
std::vector<double>::iterator matrix2d::endLine (unsigned pos) {
	return matrix.begin () + pos;
}
  
//template<class T>
//unsigned & matrix2d<T>::index(unsigned row, unsigned col) const {
unsigned matrix2d::index(unsigned row, unsigned col) const {
    return (col * rows_ + row);
}

//template<class T>
//void matrix2d<T>::assign (unsigned row, unsigned col, T& value) {
void matrix2d::assign (unsigned row, unsigned col, double& value) {
	//matrix[ index( row, col ) ] = value;
	matrix[ (col * rows_ + row) ] = value;
}
