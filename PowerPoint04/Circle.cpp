#include "Circle.h"

Circle::Circle() :
	Shape(),
	top({ 0,0 }), left({ 0,0 }), right({ 0,0 }), bottom({ 0,0 }) {
	txt = L"";
}

Circle::Circle(POINT _p1, POINT _p2) :
	Shape(_p1, _p2) {
	top = { p1.x + ((p2.x - p1.x) / 2), p1.y };
	left = { p1.x, p1.y + ((p2.y - p1.y) / 2) };
	right = { p2.x, p1.y + ((p2.y - p1.y) / 2) };
	bottom = { p1.x + ((p2.x - p1.x) / 2), p2.y };
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

void Circle::setPosition(POINT _p1, POINT _p2, bool isDrawing) {
	Shape::setPosition(_p1, _p2, isDrawing);
	top = { p1.x + ((p2.x - p1.x) / 2), p1.y };
	left = { p1.x, p1.y + ((p2.y - p1.y) / 2) };
	right = { p2.x, p1.y + ((p2.y - p1.y) / 2) };
	bottom = { p1.x + ((p2.x - p1.x) / 2), p2.y };
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

bool Circle::isCanClick(POINT mousePosition, POINT nowScreenXY, double zoomLevel) {
	POINT _p1 = { (p1.x - nowScreenXY.x)*zoomLevel, (p1.y - nowScreenXY.y)*zoomLevel };
	POINT _p2 = { (p2.x - nowScreenXY.x)*zoomLevel, (p2.y - nowScreenXY.y)*zoomLevel };

	double a = (_p2.x - _p1.x) / 2.0;
	double b = (_p2.y - _p1.y) / 2.0;
	double alpha = _p1.x + a;
	double beta = _p1.y + b;
	double lValue = (mousePosition.x - alpha)*(mousePosition.x - alpha) / (a*a)
		+
		(mousePosition.y - beta)*(mousePosition.y - beta) / (b*b);

	if (lValue <= 1) {
		puts("can select circle");
		return true;
	}
	else {
		return false;
	}
}

void Circle::draw(HDC hdc, POINT nowScreenXY, double zoomLevel) {
	POINT _p1 = { (p1.x - nowScreenXY.x)*zoomLevel, (p1.y - nowScreenXY.y)*zoomLevel };
	POINT _p2 = { (p2.x - nowScreenXY.x)*zoomLevel, (p2.y - nowScreenXY.y)*zoomLevel };

	Ellipse(hdc, _p1.x, _p1.y, _p2.x, _p2.y);

	if (txt.size() == 0) {
		return;
	}
	else {
		RECT rect = { _p1.x, _p1.y, _p2.y, _p2.y };
		RECT rectResult = { _p1.x, _p1.y, _p2.y, _p2.y };

		DrawText(hdc, txt.c_str(), txt.size(), &rect,
			DT_CENTER | DT_WORDBREAK | DT_EDITCONTROL | DT_CALCRECT);
		int marginH = max(0, ((_p2.y - _p1.y) - (rect.bottom - rect.top)) / 2);
		int marginW = max(0, ((_p2.x - _p1.x) - (rect.right - rect.left)) / 2);
		rectResult.top += marginH;
		rectResult.bottom -= marginH;
		rectResult.left += marginW;
		rectResult.right -= marginW;

		DrawText(hdc, txt.c_str(), txt.size(), &rectResult,
			DT_CENTER | DT_WORDBREAK | DT_EDITCONTROL);
	}
}

void Circle::selectDraw(HDC hdc, POINT nowScreenXY, double zoomLevel) {
	POINT _p1 = { (p1.x - nowScreenXY.x)*zoomLevel, (p1.y - nowScreenXY.y)*zoomLevel };
	POINT _p2 = { (p2.x - nowScreenXY.x)*zoomLevel, (p2.y - nowScreenXY.y)*zoomLevel };

	POINT _top = { _p1.x + ((_p2.x - _p1.x) / 2), _p1.y };
	POINT _left = { _p1.x, _p1.y + ((_p2.y - _p1.y) / 2) };
	POINT _right = { _p2.x, _p1.y + ((_p2.y - _p1.y) / 2) };
	POINT _bottom = { _p1.x + ((_p2.x - _p1.x) / 2) , _p2.y };

	Ellipse(hdc, _p1.x, _p1.y, _p2.x, _p2.y);

	HBRUSH myBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
	HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, myBrush);
	Rectangle(hdc, _p1.x, _p1.y, _p2.x, _p2.y);
	SelectObject(hdc, oldBrush);

	POINT d2 = { _p2.x,_p1.y };
	POINT d8 = { _p1.x,_p2.y };

	Ellipse(hdc, _p1.x - 5, _p1.y - 5, _p1.x + 5, _p1.y + 5);
	Ellipse(hdc, _top.x - 5, _top.y - 5, _top.x + 5, _top.y + 5);
	Ellipse(hdc, d2.x - 5, d2.y - 5, d2.x + 5, d2.y + 5);
	Ellipse(hdc, _left.x - 5, _left.y - 5, _left.x + 5, _left.y + 5);
	Ellipse(hdc, _right.x - 5, _right.y - 5, _right.x + 5, _right.y + 5);
	Ellipse(hdc, d8.x - 5, d8.y - 5, d8.x + 5, d8.y + 5);
	Ellipse(hdc, _bottom.x - 5, _bottom.y - 5, _bottom.x + 5, _bottom.y + 5);
	Ellipse(hdc, _p2.x - 5, _p2.y - 5, _p2.x + 5, _p2.y + 5);

	if (txt.size() == 0) {
		return;
	}
	else {
		RECT rect = { _p1.x, _p1.y, _p2.y, _p2.y };
		RECT rectResult = { _p1.x, _p1.y, _p2.y, _p2.y };

		DrawText(hdc, txt.c_str(), txt.size(), &rect,
			DT_CENTER | DT_WORDBREAK | DT_EDITCONTROL | DT_CALCRECT);
		int marginH = max(0, ((_p2.y - _p1.y) - (rect.bottom - rect.top)) / 2);
		int marginW = max(0, ((_p2.x - _p1.x) - (rect.right - rect.left)) / 2);
		rectResult.top += marginH;
		rectResult.bottom -= marginH;
		rectResult.left += marginW;
		rectResult.right -= marginW;

		DrawText(hdc, txt.c_str(), txt.size(), &rectResult,
			DT_CENTER | DT_WORDBREAK | DT_EDITCONTROL);
	}
}