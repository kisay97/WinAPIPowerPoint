#include "Shape.h"

Shape::Shape() : p1({ 0,0 }), p2({ 0,0 }) {
	/*empty code*/
}

Shape::Shape(POINT _p1) : p1(_p1), p2({ 0,0 }) {
	/*empty code*/
}

Shape::Shape(POINT _p1, POINT _p2) : p1(_p1), p2(_p2) {
	/*empty code*/
}

Shape::~Shape() {
	/*empty code*/
}

void Shape::setPosition(POINT _p1, POINT _p2) {
	p1 = _p1;
	p2 = _p2;
}

POINT Shape::getPoint1() {
	return p1;
}
POINT Shape::getPoint2() {
	return p2;
}