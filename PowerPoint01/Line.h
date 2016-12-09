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

	bool isCanClick(POINT mousePosition);
};