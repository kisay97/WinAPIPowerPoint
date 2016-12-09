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
	mousePosition.x += nowScreenXY.x;
	mousePosition.y += nowScreenXY.y;

	if (p1.x == p2.x) { // l 같은 선분
		if (p1.y <= p2.y) {
			if (mousePosition.x <= p1.x + 5 &&
				mousePosition.x >= p1.x - 5 &&
				mousePosition.y >= p1.y - 5 &&
				mousePosition.y <= p2.y + 5) {
				return true;
			}
			else {
				return false;
			}
		}
		else {
			if (mousePosition.x <= p1.x + 5 &&
				mousePosition.x >= p1.x - 5 &&
				mousePosition.y <= p1.y + 5 &&
				mousePosition.y >= p2.y - 5) {
				return true;
			}
			else {
				return false;
			}
		}
	}
	else if (p1.y == p2.y) { // ㅡ 같은 선분
		if (p1.x <= p2.x) {
			if (mousePosition.y <= p1.y + 5 &&
				mousePosition.y >= p1.y - 5 &&
				mousePosition.x >= p1.x - 5 &&
				mousePosition.x <= p2.x + 5) {
				return true;
			}
			else {
				return false;
			}
		}
		else {
			if (mousePosition.y <= p1.y + 5 &&
				mousePosition.y >= p1.y - 5 &&
				mousePosition.x <= p1.x + 5 &&
				mousePosition.x >= p2.x - 5) {
				return true;
			}
			else {
				return false;
			}
		}
	}
	else { // 나머지 선분
		double m1 = (double)(p1.y - p2.y) / (double)(p1.x - p2.x);
		double k1 = -m1 * p1.x + p1.y;
		double m2 = -1.0 / m1;
		double k2 = mousePosition.y - m2 * mousePosition.x;
		double ax = (k2 - k1) / (m1 - m2);
		double ay = m1 * ax + k1;

		double minPx = (p1.x <= p2.x) ? p1.x : p2.x;
		double maxPx = (p1.x <= p2.x) ? p2.x : p1.x;
		double minPy = (p1.y <= p2.y) ? p1.y : p2.y;
		double maxPy = (p1.y <= p2.y) ? p2.y : p1.y;
		if (ax >= minPx && ax <= maxPx &&
			ay >= minPy && ay <= maxPy) {
			double dis = sqrt(((ax - mousePosition.x)*(ax - mousePosition.x)) + ((ay - mousePosition.y)*(ay - mousePosition.y)));
			
			if (dis <= 5) {
				puts("aaaaaaaaaaaaaa");
				return true;
			}
			else return false;
		}
		else {
			double dis1 = sqrt(((p1.x - mousePosition.x)*(p1.x - mousePosition.x)) + ((p1.y - mousePosition.y)*(p1.y - mousePosition.y)));
			double dis2 = sqrt(((p2.x - mousePosition.x)*(p2.x - mousePosition.x)) + ((p2.y - mousePosition.y)*(p2.y - mousePosition.y)));
			double minDis = (dis1 <= dis2) ? dis1 : dis2;
			
			if (minDis <= 5) {
				puts("aaaaaaaaaaaaaa");
				return true;
			}
			else return false;
		}
	}
}

void Line::draw(HDC hdc, POINT nowScreenXY, double zoomLevel) {
	MoveToEx(hdc, p1.x - nowScreenXY.x, p1.y - nowScreenXY.y, NULL);
	LineTo(hdc, p2.x - nowScreenXY.x, p2.y - nowScreenXY.y);
}

void Line::selectDraw(HDC hdc, POINT nowScreenXY, double zoomLevel) {
	MoveToEx(hdc, p1.x - nowScreenXY.x, p1.y - nowScreenXY.y, NULL);
	LineTo(hdc, p2.x - nowScreenXY.x, p2.y - nowScreenXY.y);

	Ellipse(hdc, p1.x - 5 - nowScreenXY.x, p1.y - 5 - nowScreenXY.y, p1.x + 5 - nowScreenXY.x, p1.y + 5 - nowScreenXY.y);
	Ellipse(hdc, p2.x - 5 - nowScreenXY.x, p2.y - 5 - nowScreenXY.y, p2.x + 5 - nowScreenXY.x, p2.y + 5 - nowScreenXY.y);
}