#pragma once
#include <Windows.h>
class Shape {
protected:
	POINT p1, p2;

public:
	Shape();
	Shape(POINT _p1, POINT _p2);
	~Shape();

public:
	virtual void setPosition(POINT _p1, POINT _p2, bool isDrawing);
	POINT& getPoint1();
	POINT& getPoint2();
	bool isCanResize1(POINT mousePosition, POINT nowScreenXY, double zoomLevel);
	bool isCanResize2(POINT mousePosition, POINT nowScreenXY, double zoomLevel);

	virtual bool isCanClick(POINT mousePosition, POINT nowScreenXY, double zoomLevel) = 0;
	virtual void draw(HDC hdc, POINT nowScreenXY, double zoomLevel) = 0;
	virtual void selectDraw(HDC hdc, POINT nowScreenXY, double zoomLevel) = 0;
};