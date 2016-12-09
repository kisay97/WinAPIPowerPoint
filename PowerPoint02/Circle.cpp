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

void Circle::setPosition(POINT _p1, POINT _p2, POINT nowScreenXY, bool isDrawing) {
	Shape::setPosition(_p1, _p2, nowScreenXY, isDrawing);
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
	mousePosition.x += nowScreenXY.x;
	mousePosition.y += nowScreenXY.y;

	double a = (p2.x - p1.x) / 2.0;
	double b = (p2.y - p1.y) / 2.0;
	double alpha = p1.x + a;
	double beta = p1.y + b;
	double lValue = (mousePosition.x - alpha)*(mousePosition.x - alpha) / (a*a)
		+
		(mousePosition.y - beta)*(mousePosition.y - beta) / (b*b);

	if (lValue <= 1) {
		return true;
	}
	else {
		return false;
	}
}

void Circle::draw(HDC hdc, POINT nowScreenXY, double zoomLevel) {
	Ellipse(hdc, p1.x - nowScreenXY.x, p1.y - nowScreenXY.y, p2.x - nowScreenXY.x, p2.y - nowScreenXY.y);

	if (txt.size() == 0) {
		printf("txt.size() : %d\n", txt.size());
		return;
	}
	else {
		RECT rect = { p1.x - nowScreenXY.x, p1.y - nowScreenXY.y, p2.x - nowScreenXY.x, p2.y - nowScreenXY.y };
		RECT rectResult = { p1.x - nowScreenXY.x, p1.y - nowScreenXY.y, p2.x - nowScreenXY.x, p2.y - nowScreenXY.y };

		DrawText(hdc, txt.c_str(), txt.size(), &rect, DT_CENTER | DT_WORDBREAK | DT_EDITCONTROL | DT_CALCRECT);
		int marginH = max(0, ((p2.y - p1.y) - (rect.bottom - rect.top)) / 2);
		int marginW = max(0, ((p2.x - p1.x) - (rect.right - rect.left)) / 2);
		rectResult.top += marginH;
		rectResult.bottom -= marginH;
		rectResult.left += marginW;
		rectResult.right -= marginW;
		
		DrawText(hdc, txt.c_str(), txt.size(), &rectResult, DT_CENTER | DT_WORDBREAK | DT_EDITCONTROL);
	}
}

void Circle::selectDraw(HDC hdc, POINT nowScreenXY, double zoomLevel) {
	Ellipse(hdc, p1.x - nowScreenXY.x, p1.y - nowScreenXY.y, p2.x - nowScreenXY.x, p2.y - nowScreenXY.y);

	HBRUSH myBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
	HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, myBrush);
	Rectangle(hdc, p1.x - nowScreenXY.x, p1.y - nowScreenXY.y, p2.x - nowScreenXY.x, p2.y - nowScreenXY.y);
	SelectObject(hdc, oldBrush);

	POINT d2 = { p2.x,p1.y };
	POINT d8 = { p1.x,p2.y };

	Ellipse(hdc, p1.x - 5 - nowScreenXY.x, p1.y - 5 - nowScreenXY.y, p1.x + 5 - nowScreenXY.x, p1.y + 5 - nowScreenXY.y);
	Ellipse(hdc, top.x - 5 - nowScreenXY.x, top.y - 5 - nowScreenXY.y, top.x + 5 - nowScreenXY.x, top.y + 5 - nowScreenXY.y);
	Ellipse(hdc, d2.x - 5 - nowScreenXY.x, d2.y - 5 - nowScreenXY.y, d2.x + 5 - nowScreenXY.x, d2.y + 5 - nowScreenXY.y);
	Ellipse(hdc, left.x - 5 - nowScreenXY.x, left.y - 5 - nowScreenXY.y, left.x + 5 - nowScreenXY.x, left.y + 5 - nowScreenXY.y);
	Ellipse(hdc, right.x - 5 - nowScreenXY.x, right.y - 5 - nowScreenXY.y, right.x + 5 - nowScreenXY.x, right.y + 5 - nowScreenXY.y);
	Ellipse(hdc, d8.x - 5 - nowScreenXY.x, d8.y - 5 - nowScreenXY.y, d8.x + 5 - nowScreenXY.x, d8.y + 5 - nowScreenXY.y);
	Ellipse(hdc, bottom.x - 5 - nowScreenXY.x, bottom.y - 5 - nowScreenXY.y, bottom.x + 5 - nowScreenXY.x, bottom.y + 5 - nowScreenXY.y);
	Ellipse(hdc, p2.x - 5 - nowScreenXY.x, p2.y - 5 - nowScreenXY.y, p2.x + 5 - nowScreenXY.x, p2.y + 5 - nowScreenXY.y);

	if (txt.size() == 0) {
		printf("txt.size() : %d\n", txt.size());
		return;
	}
	else {
		RECT rect = { p1.x - nowScreenXY.x, p1.y - nowScreenXY.y, p2.x - nowScreenXY.x, p2.y - nowScreenXY.y };
		RECT rectResult = { p1.x - nowScreenXY.x, p1.y - nowScreenXY.y, p2.x - nowScreenXY.x, p2.y - nowScreenXY.y };

		DrawText(hdc, txt.c_str(), txt.size(), &rect, DT_CENTER | DT_WORDBREAK | DT_EDITCONTROL | DT_CALCRECT);
		int marginH = max(0, ((p2.y - p1.y) - (rect.bottom - rect.top)) / 2);
		int marginW = max(0, ((p2.x - p1.x) - (rect.right - rect.left)) / 2);
		rectResult.top += marginH;
		rectResult.bottom -= marginH;
		rectResult.left += marginW;
		rectResult.right -= marginW;

		DrawText(hdc, txt.c_str(), txt.size(), &rectResult, DT_CENTER | DT_WORDBREAK | DT_EDITCONTROL);
	}
}