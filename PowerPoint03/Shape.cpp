#include "Shape.h"

Shape::Shape() : p1({ 0,0 }), p2({ 0,0 }) {
	/*empty code*/
}

Shape::Shape(POINT _p1, POINT _p2) {
	int x1, x2, y1, y2;

	if (_p1.x >= _p2.x) {
		x1 = _p2.x;
		x2 = _p1.x;
	}
	else {
		x1 = _p1.x;
		x2 = _p2.x;
	}

	if (_p1.y >= _p2.y) {
		y1 = _p2.y;
		y2 = _p1.y;
	}
	else {
		y1 = _p1.y;
		y2 = _p2.y;
	}

	p1 = { x1,y1 };
	p2 = { x2,y2 };
}

Shape::~Shape() {
	/*empty code*/
}

void Shape::setPosition(POINT _p1, POINT _p2, POINT nowScreenXY, bool isDrawing) {
	if (isDrawing) {
		p1 = { _p1.x + nowScreenXY.x, _p1.y + nowScreenXY.y };
		p2 = { _p2.x + nowScreenXY.x, _p2.y + nowScreenXY.y };
	}
	else {
		int x1, x2, y1, y2;

		if (_p1.x >= _p2.x) {
			x1 = _p2.x;
			x2 = _p1.x;
		}
		else {
			x1 = _p1.x;
			x2 = _p2.x;
		}

		if (_p1.y >= _p2.y) {
			y1 = _p2.y;
			y2 = _p1.y;
		}
		else {
			y1 = _p1.y;
			y2 = _p2.y;
		}

		p1 = { x1,y1 };
		p2 = { x2,y2 };
	}
}

POINT& Shape::getPoint1() {
	return p1;
}
POINT& Shape::getPoint2() {
	return p2;
}

bool Shape::isCanResize1(POINT mousePosition, POINT nowScreenXY, double zoomLevel) {
	/*mousePosition.x += nowScreenXY.x;
	mousePosition.y += nowScreenXY.y;*/
	POINT _p1 = { (p1.x - nowScreenXY.x)*zoomLevel, (p1.y - nowScreenXY.y)*zoomLevel };

	/*bool x = (mousePosition.x >= (p1.x*zoomLevel) - 5 && mousePosition.x <= (p1.x*zoomLevel) + 5);
	bool y = (mousePosition.y >= (p1.y*zoomLevel) - 5 && mousePosition.y <= (p1.y*zoomLevel) + 5);*/
	bool x = mousePosition.x >= _p1.x - 5 && mousePosition.x <= _p1.x + 5;
	bool y = mousePosition.y >= _p1.y - 5 && mousePosition.y <= _p1.y + 5;
	if (x && y) {
		return true;
	}
	else {
		return false;
	}
}

bool Shape::isCanResize2(POINT mousePosition, POINT nowScreenXY, double zoomLevel) {
	/*mousePosition.x += nowScreenXY.x;
	mousePosition.y += nowScreenXY.y;*/
	POINT _p2 = { (p2.x - nowScreenXY.x)*zoomLevel, (p2.y - nowScreenXY.y)*zoomLevel };

	/*bool x = (mousePosition.x >= (p2.x*zoomLevel) - 5 && mousePosition.x <= (p2.x*zoomLevel) + 5);
	bool y = (mousePosition.y >= (p2.y*zoomLevel) - 5 && mousePosition.y <= (p2.y*zoomLevel) + 5);*/
	bool x = mousePosition.x >= _p2.x - 5 && mousePosition.x <= _p2.x + 5;
	bool y = mousePosition.y >= _p2.y - 5 && mousePosition.y <= _p2.y + 5;
	if (x && y) {
		return true;
	}
	else {
		return false;
	}
}