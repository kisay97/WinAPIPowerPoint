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
	void setRectPosition(POINT _left, POINT _right);
	void setString(std::wstring _txt);
	POINT getLeft();
	POINT getRIght();
	std::wstring& getString();

	bool isCanClick(POINT mousePosition);
};