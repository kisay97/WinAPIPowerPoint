#include "Line.h"

Line::Line() : Shape(){
	/*empty code*/
}

Line::Line(POINT _p1, POINT _p2) : Shape(_p1, _p2) {
	/*empty code*/
}

Line::~Line() {
	/*empty code*/
}

bool Line::isCanClick(POINT mousePosition) {
	if (p1.x == p2.x) { // l ���� ����
		if (mousePosition.x <= p1.x + 1 && 
			mousePosition.x >= p1.x - 1 && 
			mousePosition.y >= p1.y - 1 &&
			mousePosition.y <= p2.y + 1) {
			return true;
		}
		else {
			return false;
		}
	}
	else if (p1.y == p2.y) { // �� ���� ����
		if (mousePosition.y <= p1.y + 1 &&
			mousePosition.y >= p1.y - 1 &&
			mousePosition.x >= p1.x - 1 &&
			mousePosition.x <= p2.x + 1) {
			return true;
		}
		else {
			return false;
		}
	}
	else { // ������ ����
		double lineSlope; //���� ����
		double mouseSlope; //���콺�� �������� ����

		//if( !(p1.x-1<=mousePosition.x && p2.x+1>=mousePosition.x && p1.y) )

		//lineSlope = (double)(p2.y - p1.y) / (p2.x - p1.x);
		//if (mousePosition.x == p1.x) {
		//	mouseSlope = (double)(p2.y - mousePosition.y) / (p2.x - mousePosition.x);
		//}
		//else {
		//	mouseSlope = (double)(p1.y - mousePosition.y) / (p1.x - mousePosition.x);
		//}

		return false;
	}
}