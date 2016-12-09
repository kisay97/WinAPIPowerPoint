#include "Rect.h"

Rect::Rect() : 
	Shape(), 
	left({ 0,0 }), right({ 0,0 }) {
	txt = L"";
}

Rect::Rect(POINT _p1, POINT _p2) :
	Shape(_p1, _p2),
	left({ 0,0 }), right({ 0,0 }) {
	txt = L"";
}

Rect::Rect(POINT _p1, POINT _p2, 
	POINT _left, POINT _right) : 
	Shape(_p1, _p2),
	left(_left), right(_right) {
	txt = L"";
}

Rect::~Rect() {
	/*empty code*/
}

void Rect::setRectPosition(
	POINT _left, POINT _right) {
	left = _left;
	right = _right;
}

void Rect::setString(std::wstring _txt) {
	txt = _txt;
}

POINT Rect::getLeft() {
	return left;
}

POINT Rect::getRIght() {
	return right;
}

std::wstring& Rect::getString() {
	return txt;
}

bool Rect::isCanClick(POINT mousePosition) {
	bool isXSatisfied = false;
	bool isYSatisfied = false;

	if (mousePosition.x >= p1.x && mousePosition.x <= p2.x) {
		isXSatisfied = true;
	}

	if (mousePosition.y >= p1.y && mousePosition.y <= p2.y) {
		isYSatisfied = true;
	}

	if (isXSatisfied && isYSatisfied) {
		return true;
	}
	else {
		return false;
	}
}