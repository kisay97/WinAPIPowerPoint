#pragma once
#include <Windows.h>
#include <string>
#include "Shape.h"

class Circle : public Shape {
private:
	POINT top, left, right, bottom;
	std::wstring txt;

public:
	Circle();
	Circle(POINT _p1, POINT _p2);
	Circle(POINT _p1, POINT _p2,
		POINT _top, POINT _left, POINT _right, POINT _bottom);
	~Circle();

public:
	void setPosition(POINT _p1, POINT _p2, bool isDrawing);
	void setCirclePosition(POINT _top, POINT _left, POINT _right, POINT _bottom);
	void setString(std::wstring _txt);
	POINT getTop();
	POINT getLeft();
	POINT getRIght();
	POINT getBottom();
	std::wstring& getString();

	virtual bool isCanClick(POINT mousePosition, POINT nowScreenXY, double zoomLevel);
	virtual void draw(HDC hdc, POINT nowScreenXY, double zoomLevel);
	virtual void selectDraw(HDC hdc, POINT nowScreenXY, double zoomLevel);
};