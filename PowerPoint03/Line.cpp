#include "Line.h"

Line::Line() : Shape() {
	/*empty code*/
}

Line::Line(POINT _p1, POINT _p2) {
	p1 = _p1;
	p2 = _p2;
}

Line::~Line() {
	/*empty code*/
}

void Line::setPosition(POINT _p1, POINT _p2, POINT nowScreenXY, bool isDrawing) {
	if (isDrawing) {
		p1 = { _p1.x + nowScreenXY.x, _p1.y + nowScreenXY.y };
		p2 = { _p2.x + nowScreenXY.x, _p2.y + nowScreenXY.y };
	}
	else {
		p1 = { _p1.x, _p1.y };
		p2 = { _p2.x, _p2.y };
	}
}

bool Line::isCanClick(POINT mousePosition, POINT nowScreenXY, double zoomLevel) {
	/*mousePosition.x += nowScreenXY.x;
	mousePosition.y += nowScreenXY.y;*/

	/*POINT _p1 = { p1.x*zoomLevel, p1.y*zoomLevel };
	POINT _p2 = { p2.x*zoomLevel, p2.y*zoomLevel };*/
	POINT _p1 = { (p1.x - nowScreenXY.x)*zoomLevel, (p1.y - nowScreenXY.y)*zoomLevel };
	POINT _p2 = { (p2.x - nowScreenXY.x)*zoomLevel, (p2.y - nowScreenXY.y)*zoomLevel };

	if (_p1.x == _p2.x) { // l 같은 선분
		if (_p1.y <= _p2.y) {
			if (mousePosition.x <= _p1.x + 5 &&
				mousePosition.x >= _p1.x - 5 &&
				mousePosition.y >= _p1.y - 5 &&
				mousePosition.y <= _p2.y + 5) {
				return true;
			}
			else {
				return false;
			}
		}
		else {
			if (mousePosition.x <= _p1.x + 5 &&
				mousePosition.x >= _p1.x - 5 &&
				mousePosition.y <= _p1.y + 5 &&
				mousePosition.y >= _p2.y - 5) {
				return true;
			}
			else {
				return false;
			}
		}
	}
	else if (_p1.y == _p2.y) { // ㅡ 같은 선분
		if (_p1.x <= _p2.x) {
			if (mousePosition.y <= _p1.y + 5 &&
				mousePosition.y >= _p1.y - 5 &&
				mousePosition.x >= _p1.x - 5 &&
				mousePosition.x <= _p2.x + 5) {
				return true;
			}
			else {
				return false;
			}
		}
		else {
			if (mousePosition.y <= _p1.y + 5 &&
				mousePosition.y >= _p1.y - 5 &&
				mousePosition.x <= _p1.x + 5 &&
				mousePosition.x >= _p2.x - 5) {
				return true;
			}
			else {
				return false;
			}
		}
	}
	else { // 나머지 선분
		double m1 = (double)(_p1.y - _p2.y) / (double)(_p1.x - _p2.x);
		double k1 = -m1 * _p1.x + _p1.y;
		double m2 = -1.0 / m1;
		double k2 = mousePosition.y - m2 * mousePosition.x;
		double ax = (k2 - k1) / (m1 - m2);
		double ay = m1 * ax + k1;

		double minPx = (_p1.x <= _p2.x) ? _p1.x : _p2.x;
		double maxPx = (_p1.x <= _p2.x) ? _p2.x : _p1.x;
		double minPy = (_p1.y <= _p2.y) ? _p1.y : _p2.y;
		double maxPy = (_p1.y <= _p2.y) ? _p2.y : _p1.y;
		if (ax >= minPx && ax <= maxPx &&
			ay >= minPy && ay <= maxPy) {
			double dis = 
				sqrt(((ax - mousePosition.x)*(ax - mousePosition.x)) 
				+ 
				((ay - mousePosition.y)*(ay - mousePosition.y)));

			if (dis <= 5) {
				puts("can select line");
				return true;
			}
			else return false;
		}
		else {
			double dis1 = 
				sqrt(((_p1.x - mousePosition.x)*(_p1.x - mousePosition.x)) 
				+ 
				((_p1.y - mousePosition.y)*(_p1.y - mousePosition.y)));
			double dis2 = 
				sqrt(((_p2.x - mousePosition.x)*(_p2.x - mousePosition.x)) 
				+ 
				((_p2.y - mousePosition.y)*(_p2.y - mousePosition.y)));
			double minDis = (dis1 <= dis2) ? dis1 : dis2;

			if (minDis <= 5) {
				puts("can select line");
				return true;
			}
			else return false;
		}
	}
}

void Line::draw(HDC hdc, POINT nowScreenXY, double zoomLevel) {
	POINT _p1 = { (p1.x - nowScreenXY.x)*zoomLevel, (p1.y - nowScreenXY.y)*zoomLevel };
	POINT _p2 = { (p2.x - nowScreenXY.x)*zoomLevel, (p2.y - nowScreenXY.y)*zoomLevel };

	MoveToEx(hdc, _p1.x, _p1.y, NULL);
	LineTo(hdc, _p2.x, _p2.y);
}

void Line::selectDraw(HDC hdc, POINT nowScreenXY, double zoomLevel) {
	POINT _p1 = { (p1.x - nowScreenXY.x)*zoomLevel, (p1.y - nowScreenXY.y)*zoomLevel };
	POINT _p2 = { (p2.x - nowScreenXY.x)*zoomLevel, (p2.y - nowScreenXY.y)*zoomLevel };

	MoveToEx(hdc, _p1.x, _p1.y, NULL);
	LineTo(hdc, _p2.x, _p2.y);

	Ellipse(hdc, _p1.x - 5, _p1.y - 5, _p1.x + 5, _p1.y + 5);
	Ellipse(hdc, _p2.x - 5, _p2.y - 5, _p2.x + 5, _p2.y + 5);
}