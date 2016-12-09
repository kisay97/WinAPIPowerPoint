#pragma once
#include <Windows.h>
#include <string>
#include "Shape.h"

class Line : public Shape {
private:

public:
	Line();
	Line(POINT _p1, POINT _p2);
	~Line();

	void setPosition(POINT _p1, POINT _p2, POINT nowScreenXY, bool isDrawing);

	virtual bool isCanClick(POINT mousePosition, POINT nowScreenXY, double zoomLevel);
	virtual void draw(HDC hdc, POINT nowScreenXY, double zoomLevel);
	virtual void selectDraw(HDC hdc, POINT nowScreenXY, double zoomLevel);
};