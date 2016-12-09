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

void Rect::setPosition(POINT _p1, POINT _p2, bool isDrawing) {
	Shape::setPosition(_p1, _p2, isDrawing);
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
	POINT _p1 = { (p1.x - nowScreenXY.x)*zoomLevel, (p1.y - nowScreenXY.y)*zoomLevel };
	POINT _p2 = { (p2.x - nowScreenXY.x)*zoomLevel, (p2.y - nowScreenXY.y)*zoomLevel };

	bool isXSatisfied = false;
	bool isYSatisfied = false;

	if (mousePosition.x >= _p1.x && mousePosition.x <= _p2.x) {
		isXSatisfied = true;
	}

	if (mousePosition.y >= _p1.y && mousePosition.y <= _p2.y) {
		isYSatisfied = true;
	}

	if (isXSatisfied && isYSatisfied) {
		puts("can select rect");
		return true;
	}
	else {
		return false;
	}
}

void Rect::draw(HDC hdc, POINT nowScreenXY, double zoomLevel) {
	POINT _p1 = { (p1.x - nowScreenXY.x)*zoomLevel, (p1.y - nowScreenXY.y)*zoomLevel };
	POINT _p2 = { (p2.x - nowScreenXY.x)*zoomLevel, (p2.y - nowScreenXY.y)*zoomLevel };

	Rectangle(hdc, _p1.x, _p1.y, _p2.x, _p2.y);

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

void Rect::selectDraw(HDC hdc, POINT nowScreenXY, double zoomLevel) {
	POINT _p1 = { (p1.x - nowScreenXY.x)*zoomLevel, (p1.y - nowScreenXY.y)*zoomLevel };
	POINT _p2 = { (p2.x - nowScreenXY.x)*zoomLevel, (p2.y - nowScreenXY.y)*zoomLevel };

	Rectangle(hdc, _p1.x, _p1.y, _p2.x, _p2.y);

	POINT _left = { _p1.x, _p2.y };
	POINT _right = { _p2.x, _p1.y };

	POINT d12 = { _p1.x + ((_p2.x - _p1.x) / 2) , _p1.y };
	POINT d9 = { _p1.x, _p1.y + ((_p2.y - _p1.y) / 2) };
	POINT d3 = { _p2.x, _p1.y + ((_p2.y - _p1.y) / 2) };
	POINT d6 = { _p1.x + (_p2.x - _p1.x) / 2 , _p2.y };

	Ellipse(hdc, _p1.x - 5, _p1.y - 5, _p1.x + 5, _p1.y + 5);
	Ellipse(hdc, d12.x - 5, d12.y - 5, d12.x + 5, d12.y + 5);
	Ellipse(hdc, _right.x - 5, _right.y - 5, _right.x + 5, _right.y + 5);
	Ellipse(hdc, d9.x - 5, d9.y - 5, d9.x + 5, d9.y + 5);
	Ellipse(hdc, d3.x - 5, d3.y - 5, d3.x + 5, d3.y + 5);
	Ellipse(hdc, _left.x - 5, _left.y - 5, _left.x + 5, _left.y + 5);
	Ellipse(hdc, d6.x - 5, d6.y - 5, d6.x + 5, d6.y + 5);
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