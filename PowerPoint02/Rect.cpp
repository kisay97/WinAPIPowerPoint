#include "Rect.h"

Rect::Rect() :
	Shape(),
	left({ 0,0 }), right({ 0,0 }) {
	txt = L"";
}

Rect::Rect(POINT _p1, POINT _p2) :
	Shape(_p1, _p2) {
	left = { p1.x,p2.y };
	right = { p2.x,p1.y };
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

void Rect::setPosition(POINT _p1, POINT _p2, POINT nowScreenXY, bool isDrawing) {
	Shape::setPosition(_p1, _p2, nowScreenXY, isDrawing);
	left = { p1.x,p2.y };
	right = { p2.x,p1.y };
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

bool Rect::isCanClick(POINT mousePosition, POINT nowScreenXY, double zoomLevel) {
	mousePosition.x += nowScreenXY.x;
	mousePosition.y += nowScreenXY.y;
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

void Rect::draw(HDC hdc, POINT nowScreenXY, double zoomLevel) {
	Rectangle(hdc, p1.x - nowScreenXY.x, p1.y - nowScreenXY.y, p2.x - nowScreenXY.x, p2.y - nowScreenXY.y);

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

void Rect::selectDraw(HDC hdc, POINT nowScreenXY, double zoomLevel) {
	Rectangle(hdc, p1.x - nowScreenXY.x, p1.y - nowScreenXY.y, p2.x - nowScreenXY.x, p2.y - nowScreenXY.y);

	printf("left.x : %d, left.y : %d\n", left.x, left.y);
	printf("right.x : %d, right.y : %d\n", right.x, right.y);

	POINT d12 = { p1.x + ((p2.x - p1.x)/2) , p1.y };
	POINT d9 = { p1.x, p1.y + ((p2.y - p1.y)/2) };
	POINT d3 = { p2.x, p1.y + ((p2.y - p1.y)/2) };
	POINT d6 = { p1.x + (p2.x - p1.x) / 2 , p2.y };
	
	Ellipse(hdc, p1.x - 5 - nowScreenXY.x, p1.y - 5 - nowScreenXY.y, p1.x + 5 - nowScreenXY.x, p1.y + 5 - nowScreenXY.y);
	Ellipse(hdc, d12.x - 5 - nowScreenXY.x, d12.y - 5 - nowScreenXY.y, d12.x + 5 - nowScreenXY.x, d12.y + 5 - nowScreenXY.y);
	Ellipse(hdc, right.x - 5 - nowScreenXY.x, right.y - 5 - nowScreenXY.y, right.x + 5 - nowScreenXY.x, right.y + 5 - nowScreenXY.y);
	Ellipse(hdc, d9.x - 5 - nowScreenXY.x, d9.y - 5 - nowScreenXY.y, d9.x + 5 - nowScreenXY.x, d9.y + 5 - nowScreenXY.y);
	Ellipse(hdc, d3.x - 5 - nowScreenXY.x, d3.y - 5 - nowScreenXY.y, d3.x + 5 - nowScreenXY.x, d3.y + 5 - nowScreenXY.y);
	Ellipse(hdc, left.x - 5 - nowScreenXY.x, left.y - 5 - nowScreenXY.y, left.x + 5 - nowScreenXY.x, left.y + 5 - nowScreenXY.y);
	Ellipse(hdc, d6.x - 5 - nowScreenXY.x, d6.y - 5 - nowScreenXY.y, d6.x + 5 - nowScreenXY.x, d6.y + 5 - nowScreenXY.y);
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