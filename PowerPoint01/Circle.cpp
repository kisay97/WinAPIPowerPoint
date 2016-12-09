#include "Circle.h"

Circle::Circle() : 
	Shape(), 
	top({ 0,0 }), left({ 0,0 }), right({ 0,0 }), bottom({ 0,0 }) {
	txt = L"";
}

Circle::Circle(POINT _p1, POINT _p2) :
	Shape(_p1, _p2),
	top({ 0,0 }), left({ 0,0 }), right({ 0,0 }), bottom({ 0,0 }) {
	txt = L"";
}

Circle::Circle(POINT _p1, POINT _p2,
	POINT _top, POINT _left, POINT _right, POINT _bottom) : 
	Shape(_p1, _p2),
	top(_top), left(_left), right(_right), bottom(_bottom) {
	txt = L"";
}

Circle::~Circle() {
	/*empty code*/
}

void Circle::setCirclePosition(
	POINT _top, POINT _left, POINT _right, POINT _bottom) {
	top = _top;
	left = _left;
	right = _right;
	bottom = _bottom;
}

void Circle::setString(std::wstring _txt) {
	txt = _txt;
}

POINT Circle::getTop() {
	return top;
}

POINT Circle::getLeft() {
	return left;
}

POINT Circle::getRIght() {
	return right;
}

POINT Circle::getBottom() {
	return bottom;
}

std::wstring& Circle::getString() {
	return txt;
}

bool Circle::isCanClick(POINT mousePosition) {
	int lValue =
		(((mousePosition.x - (p1.x + ((p2.x - p1.x) / 2))) * (mousePosition.x - (p1.x + ((p2.x - p1.x) / 2)))) / (((p2.x - p1.x) / 2)*((p2.x - p1.x) / 2)))
		+
		(((mousePosition.y - (p1.y + ((p2.y - p1.y) / 2))) * (mousePosition.y - (p1.y + ((p2.y - p1.y) / 2)))) / (((p2.y - p1.y) / 2)*((p2.y - p1.y) / 2)));
	
	if (lValue <= 1) {
		return true;
	}
	else {
		return false;
	}
}