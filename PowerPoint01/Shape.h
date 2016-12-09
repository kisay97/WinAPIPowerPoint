#pragma once
#include <Windows.h>
class Shape {
protected:
	POINT p1, p2;

public:
	Shape();
	Shape(POINT _p1);
	Shape(POINT _p1, POINT _p2);
	~Shape();

public:
	void setPosition(POINT _p1, POINT _p2);
	POINT getPoint1();
	POINT getPoint2();

	virtual bool isCanClick(POINT mousePosition) = 0;
};