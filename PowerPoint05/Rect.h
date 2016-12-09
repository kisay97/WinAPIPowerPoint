#pragma once
#include <Windows.h>
#include <string>
#include "Shape.h"

class Rect : public Shape {
private:
	POINT left, right;
	std::wstring txt;

public:
	Rect();
	Rect(POINT _p1, POINT _p2);
	Rect(POINT _p1, POINT _p2,
		POINT _left, POINT _right);
	~Rect();

public:
	void setPosition(POINT _p1, POINT _p2, bool isDrawing);
	void setRectPosition(POINT _left, POINT _right);
	void setString(std::wstring _txt);
	POINT getLeft();
	POINT getRIght();
	std::wstring& getString();

	virtual bool isCanClick(POINT mousePosition, POINT nowScreenXY, double zoomLevel);
	virtual void draw(HDC hdc, POINT nowScreenXY, double zoomLevel);
	virtual void selectDraw(HDC hdc, POINT nowScreenXY, double zoomLevel);
};