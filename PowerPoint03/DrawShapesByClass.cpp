#include <windows.h>
#include <imm.h>
#include <list>
#include <iostream>
#include <string>
#include "Shape.h"
#include "Circle.h"
#include "Rect.h"
#include "Line.h"
#include "resource.h"

#ifdef _DEBUG
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif

using std::list;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
HWND hWndMain;
LPCWSTR lpszClass = L"DrawShapesByClass";

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance
	, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;

	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW,
		400, 200, 500, 500,
		NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);

	while (GetMessage(&Message, 0, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;
}

list<Shape*> shapeList;		//그린 도형들의 리스트
bool canDrawing = false;	//도형을 그릴 수 있는 상태
bool isDrawing = false;		//도형을 그리고 있는 중인 상태
int selectIndex = -1;		//선택한 도형의 리스트상의 인덱스
bool isMoving = false;		//이동중인 상태
int oldX = 0, oldY = 0;		//이동중인 상태에서 이전 마우스 좌표
bool bComp = false;			//조합중인 글자가 있는지
int resizePoint = 0;		//리사이즈중인 점의 번호(원,사각형 : 1~9, 선 : 1~2)
bool isPanning = false;		//패닝중인 상태
POINT nowScreenXY = { 0,0 };//패닝해서 현재 클라이언트 좌상단 위치
double zoomLevel = 1.0;		//확대 정도
HPEN MyPen, OldPen;
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	static HCURSOR nowCursor;

	HIMC hImc;

	switch (iMessage) {
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case ID_Shape_Circle: //원을 그리려함
			puts("select Circle");
			nowCursor = LoadCursor(NULL, IDC_CROSS);
			canDrawing = true;
			shapeList.push_back(new Circle());
			break;

		case ID_Shape_Rect: //직사각형을 그리려함
			puts("select Rect");
			nowCursor = LoadCursor(NULL, IDC_CROSS);
			canDrawing = true;
			shapeList.push_back(new Rect());
			break;

		case ID_Shape_Line:
			puts("select Line");
			nowCursor = LoadCursor(NULL, IDC_CROSS);
			canDrawing = true;
			shapeList.push_back(new Line());
			break;
		}

		return 0;

	case WM_LBUTTONDOWN: {
		if (canDrawing) {
			puts("isDrawing");

			list<Shape*>::reverse_iterator ritor = shapeList.rbegin();
			POINT currentMousePos = { LOWORD(lParam), HIWORD(lParam) };
			printf("MouseX : %d, MouseY : %d\n", currentMousePos.x, currentMousePos.y);
			(*ritor)->setPosition(currentMousePos, currentMousePos, nowScreenXY, isDrawing = true);

			isMoving = false;
			oldX = currentMousePos.x;
			oldY = currentMousePos.y;
			selectIndex = -1;
		}
		else {
			POINT currentMousePos = { LOWORD(lParam), HIWORD(lParam) };
			list<Shape*>::reverse_iterator ritor = shapeList.rbegin();
			int listIndex = shapeList.size() - 1;

			if (selectIndex >= 0) { //이미 선택한게 있음
				for (ritor = shapeList.rbegin(); ritor != shapeList.rend(); ritor++) { //find select element
					if (listIndex != selectIndex && (*ritor)->isCanClick(currentMousePos, nowScreenXY, zoomLevel)) {
						selectIndex = listIndex;
						oldX = currentMousePos.x;
						oldY = currentMousePos.y;
						isMoving = true;
						return 0;
					}

					if (listIndex == selectIndex) {
						break;
					}
					listIndex--;
				}

				if ((*ritor)->isCanResize1(currentMousePos, nowScreenXY, zoomLevel)) {
					resizePoint = 1;
					oldX = currentMousePos.x;
					oldY = currentMousePos.y;
					return 0;
				}
				else if ((*ritor)->isCanResize2(currentMousePos, nowScreenXY, zoomLevel)) {
					resizePoint = 2;
					oldX = currentMousePos.x;
					oldY = currentMousePos.y;
					return 0;
				}
				else if ((*ritor)->isCanClick(currentMousePos, nowScreenXY, zoomLevel)) { //선택한걸 움직이려고 함
					oldX = currentMousePos.x;
					oldY = currentMousePos.y;
					isMoving = true;
					return 0;
				}
			}

			//선택한게 없거나, 다른 도형을 선택하려고 했을때
			listIndex = shapeList.size() - 1;
			for (ritor = shapeList.rbegin(); ritor != shapeList.rend(); ritor++) {
				if ((*ritor)->isCanClick(currentMousePos, nowScreenXY, zoomLevel)) {
					selectIndex = listIndex;
					isMoving = true;
					oldX = currentMousePos.x;
					oldY = currentMousePos.y;
					break;
				}
				listIndex--;
			}

			if (ritor == shapeList.rend()) { //배경을 클릭해 선택 해제
				selectIndex = -1;
			}
			printf("selectIndex : %d\n", selectIndex); //어떻게 선택됐는지 디버깅
		}

		InvalidateRect(hWnd, NULL, TRUE);
		return 0;
	}

	case WM_LBUTTONUP: {
		nowCursor = LoadCursor(NULL, IDC_ARROW);
		if (isDrawing) {
			puts("endDrawing");
			isDrawing = false;
			canDrawing = false;

			list<Shape*>::iterator itor = shapeList.end(); itor--;
			POINT currentMousePos = { LOWORD(lParam) + nowScreenXY.x, HIWORD(lParam) + nowScreenXY.y };
			printf("MouseX : %d, MouseY : %d\n", currentMousePos.x, currentMousePos.y);
			(*itor)->setPosition({ oldX + nowScreenXY.x, oldY + nowScreenXY.y }, currentMousePos, nowScreenXY, false);
			selectIndex = shapeList.size() - 1;
		}
		else if (isMoving) {
			puts("move end");
			printf("x : %d, y : %d\n", LOWORD(lParam), HIWORD(lParam));

			list<Shape*>::iterator itor = shapeList.end(); itor--;
			POINT currentMousePos = { LOWORD(lParam), HIWORD(lParam) };
			int listIndex = 0;
			for (itor = shapeList.begin(); itor != shapeList.end(); itor++) {
				if (listIndex == selectIndex) {
					break;
				}
				listIndex++;
			}

			POINT vMove = { currentMousePos.x - oldX, currentMousePos.y - oldY }; //이동량(벡터)
			printf("vMove.x : %d, vMove.y : %d\n", vMove.x, vMove.y);
			POINT selectElementP1 = (*itor)->getPoint1();
			POINT selectElementP2 = (*itor)->getPoint2();
			POINT movedP1 = { selectElementP1.x + vMove.x, selectElementP1.y + vMove.y };
			POINT movedP2 = { selectElementP2.x + vMove.x, selectElementP2.y + vMove.y };
			(*itor)->setPosition(movedP1, movedP2, nowScreenXY, false);

			isMoving = false;
			oldX = 0;
			oldY = 0;
		}
		else if (resizePoint > 0) {
			switch (resizePoint)
			{
			case 1: {
				list<Shape*>::iterator itor = shapeList.end(); itor--;
				POINT currentMousePos = { LOWORD(lParam), HIWORD(lParam) };
				int listIndex = 0;
				for (itor = shapeList.begin(); itor != shapeList.end(); itor++) {
					if (listIndex == selectIndex) {
						break;
					}
					listIndex++;
				}

				POINT vMove = { currentMousePos.x - oldX, currentMousePos.y - oldY };
				POINT selectElementP1 = (*itor)->getPoint1();
				POINT selectElementP2 = (*itor)->getPoint2();
				POINT movedP1 = { selectElementP1.x + vMove.x, selectElementP1.y + vMove.y };
				(*itor)->setPosition(movedP1, selectElementP2, nowScreenXY, false);
				break;
			}

			case 2: {
				list<Shape*>::iterator itor = shapeList.end(); itor--;
				POINT currentMousePos = { LOWORD(lParam), HIWORD(lParam) };
				int listIndex = 0;
				for (itor = shapeList.begin(); itor != shapeList.end(); itor++) {
					if (listIndex == selectIndex) {
						break;
					}
					listIndex++;
				}

				POINT vMove = { currentMousePos.x - oldX, currentMousePos.y - oldY };
				POINT selectElementP1 = (*itor)->getPoint1();
				POINT selectElementP2 = (*itor)->getPoint2();
				POINT movedP2 = { selectElementP2.x + vMove.x, selectElementP2.y + vMove.y };
				(*itor)->setPosition(selectElementP1, movedP2, nowScreenXY, false);
				break;
			}
			}

			resizePoint = 0;
		}

		InvalidateRect(hWnd, NULL, true);
		return 0;
	}

	case WM_RBUTTONDOWN: {
		isPanning = true;
		oldX = LOWORD(lParam);
		oldY = HIWORD(lParam);
		nowCursor = LoadCursor(NULL, IDC_HAND);

		return 0;
	}

	case WM_RBUTTONUP: {
		POINT currentMousePos = { LOWORD(lParam),HIWORD(lParam) };
		POINT vMove = { currentMousePos.x - oldX, currentMousePos.y - oldY };
		nowScreenXY.x -= vMove.x;
		nowScreenXY.y -= vMove.y;

		printf("nowScreenXY.x : %d, nowScreenXY.y : %d\n", nowScreenXY.x, nowScreenXY.y);

		isPanning = false;
		oldX = 0;
		oldY = 0;

		InvalidateRect(hWnd, NULL, TRUE);
		return 0;
	}

	case WM_MOUSEMOVE: {
		if (isDrawing) {
			puts("isDrawing");
			list<Shape*>::iterator itor = shapeList.end(); itor--;
			POINT currentMousePos = { LOWORD(lParam), HIWORD(lParam) };
			(*itor)->setPosition({ oldX,oldY }, currentMousePos, nowScreenXY, isDrawing);

			InvalidateRect(hWnd, NULL, true);
		}
		else if (isPanning) {
			POINT currentMousePos = { LOWORD(lParam),HIWORD(lParam) };
			POINT vMove = { currentMousePos.x - oldX, currentMousePos.y - oldY };
			nowScreenXY.x -= vMove.x;
			nowScreenXY.y -= vMove.y;

			oldX = currentMousePos.x;
			oldY = currentMousePos.y;

			InvalidateRect(hWnd, NULL, TRUE);
			return 0;
		}
		else if (isMoving) {
			nowCursor = LoadCursor(NULL, IDC_SIZEALL);
			list<Shape*>::iterator itor = shapeList.begin();
			POINT currentMousePos = { LOWORD(lParam), HIWORD(lParam) };
			int listIndex = 0;
			for (itor = shapeList.begin(); itor != shapeList.end(); itor++) { //find select element
				if (listIndex == selectIndex) {
					break;
				}
				listIndex++;
			}

			POINT vMove = { currentMousePos.x - oldX, currentMousePos.y - oldY };
			POINT &selectElementP1 = (*itor)->getPoint1();
			POINT &selectElementP2 = (*itor)->getPoint2();

			POINT movedP1 = { selectElementP1.x + vMove.x, selectElementP1.y + vMove.y };
			POINT movedP2 = { selectElementP2.x + vMove.x, selectElementP2.y + vMove.y };
			(*itor)->setPosition(movedP1, movedP2, nowScreenXY, false);

			oldX = currentMousePos.x;
			oldY = currentMousePos.y;

			InvalidateRect(hWnd, NULL, true);
		}
		else if (resizePoint > 0) {
			nowCursor = LoadCursor(NULL, IDC_SIZENWSE);
			switch (resizePoint)
			{
			case 1: {
				list<Shape*>::iterator itor = shapeList.end(); itor--;
				POINT currentMousePos = { LOWORD(lParam), HIWORD(lParam) };
				int listIndex = 0;
				for (itor = shapeList.begin(); itor != shapeList.end(); itor++) {
					if (listIndex == selectIndex) {
						break;
					}
					listIndex++;
				}

				POINT vMove = { currentMousePos.x - oldX, currentMousePos.y - oldY };
				//POINT vMove = { (currentMousePos.x - oldX) / zoomLevel, (currentMousePos.y - oldY) / zoomLevel };
				POINT selectElementP1 = (*itor)->getPoint1();
				POINT selectElementP2 = (*itor)->getPoint2();
				POINT movedP1 = { selectElementP1.x + vMove.x - nowScreenXY.x, selectElementP1.y + vMove.y - nowScreenXY.y };
				POINT movedP2 = { selectElementP2.x - nowScreenXY.x, selectElementP2.y - nowScreenXY.y };
				(*itor)->setPosition(movedP1, movedP2, nowScreenXY, true);

				oldX = currentMousePos.x;
				oldY = currentMousePos.y;
				break;
			}

			case 2: {
				list<Shape*>::iterator itor = shapeList.end(); itor--;
				POINT currentMousePos = { LOWORD(lParam), HIWORD(lParam) };
				int listIndex = 0;
				for (itor = shapeList.begin(); itor != shapeList.end(); itor++) {
					if (listIndex == selectIndex) {
						break;
					}
					listIndex++;
				}

				POINT vMove = { currentMousePos.x - oldX, currentMousePos.y - oldY };
				//POINT vMove = { (currentMousePos.x - oldX) / zoomLevel, (currentMousePos.y - oldY) / zoomLevel };
				POINT selectElementP1 = (*itor)->getPoint1();
				POINT selectElementP2 = (*itor)->getPoint2();
				POINT movedP1 = { selectElementP1.x - nowScreenXY.x, selectElementP1.y - nowScreenXY.y };
				POINT movedP2 = { selectElementP2.x + vMove.x - nowScreenXY.x, selectElementP2.y + vMove.y - nowScreenXY.y };
				(*itor)->setPosition(movedP1, movedP2, nowScreenXY, true);

				oldX = currentMousePos.x;
				oldY = currentMousePos.y;
				break;
			}
			}
			InvalidateRect(hWnd, NULL, true);
		}
		else if (!canDrawing) {
			list<Shape*>::iterator itor = shapeList.begin();
			POINT currentMousePos = { LOWORD(lParam), HIWORD(lParam) };

			int listIndex = 0;
			for (itor = shapeList.begin(); itor != shapeList.end(); itor++) {
				if (selectIndex == listIndex) {
					if ((*itor)->isCanResize1(currentMousePos, nowScreenXY, zoomLevel) || (*itor)->isCanResize2(currentMousePos, nowScreenXY, zoomLevel)) {
						nowCursor = LoadCursor(NULL, IDC_SIZENWSE);
						break;
					}
				}

				if ((*itor)->isCanClick(currentMousePos, nowScreenXY, zoomLevel)) {
					if (selectIndex == listIndex) {
						nowCursor = LoadCursor(NULL, IDC_SIZEALL);
						break;
					}
					else {
						nowCursor = LoadCursor(NULL, IDC_HAND);
						break;
					}
				}
				else {
					nowCursor = LoadCursor(NULL, IDC_ARROW);
				}
				listIndex++;
			}
		}

		return 0;
	}

	case WM_CHAR: {
		if (selectIndex >= 0) {
			TCHAR tempChar[] = { wParam,0 };

			list<Shape*>::iterator itor;
			int listIndex = 0;
			for (itor = shapeList.begin(); itor != shapeList.end(); itor++) {
				if (listIndex == selectIndex) break;
				listIndex++;
			}

			const char *elementTypeName = typeid(*(*itor)).name();
			if (strcmp(elementTypeName, "class Circle") == 0) {
				std::wstring &mStr = ((Circle*)(*itor))->getString();
				if (wParam == 8) {
					if (mStr.size() > 0) {
						mStr.erase(mStr.size() - 1, 1);
					}
				}
				else {
					mStr.append(tempChar);
				}

				POINT p1 = (*itor)->getPoint1();
				POINT p2 = (*itor)->getPoint2();
				RECT mRect = { p1.x,p1.y,p2.x,p2.y };
				InvalidateRect(hWnd, &mRect, TRUE);
			}
			else if (strcmp(elementTypeName, "class Rect") == 0) {
				std::wstring &mStr = ((Rect*)(*itor))->getString();
				if (wParam == 8) {
					if (mStr.size() > 0) {
						mStr.erase(mStr.size() - 1, 1);
					}
				}
				else {
					mStr.append(tempChar);
				}

				POINT p1 = (*itor)->getPoint1();
				POINT p2 = (*itor)->getPoint2();
				RECT mRect = { p1.x,p1.y,p2.x,p2.y };
				InvalidateRect(hWnd, &mRect, TRUE);
			}
		}

		return 0;
	}

	case WM_IME_COMPOSITION: {
		if (selectIndex >= 0) {
			list<Shape*>::iterator itor;
			int listIndex = 0;
			for (itor = shapeList.begin(); itor != shapeList.end(); itor++) {
				if (listIndex == selectIndex) break;
				listIndex++;
			}

			const char *elementTypeName = typeid(*(*itor)).name();

			hImc = ImmGetContext(hWnd);
			if (lParam & GCS_COMPSTR) {
				int len = ImmGetCompositionString(hImc, GCS_COMPSTR, NULL, 0);

				if (len > 0) {
					TCHAR tempChar[] = { wParam, 0 };

					if (bComp) {
						if (strcmp(elementTypeName, "class Circle") == 0) {
							std::wstring &mStr = ((Circle*)(*itor))->getString();
							int mOff = mStr.size() - 1;
							mStr[mOff] = tempChar[0];

							POINT p1 = (*itor)->getPoint1();
							POINT p2 = (*itor)->getPoint2();
							RECT mRect = { p1.x,p1.y,p2.x,p2.y };
							InvalidateRect(hWnd, &mRect, TRUE);
						}
						else if (strcmp(elementTypeName, "class Rect") == 0) {
							std::wstring &mStr = ((Rect*)(*itor))->getString();
							int mOff = mStr.size() - 1;
							mStr[mOff] = tempChar[0];

							POINT p1 = (*itor)->getPoint1();
							POINT p2 = (*itor)->getPoint2();
							RECT mRect = { p1.x,p1.y,p2.x,p2.y };
							InvalidateRect(hWnd, &mRect, TRUE);
						}
					}
					else {
						if (strcmp(elementTypeName, "class Circle") == 0) {
							std::wstring &mStr = ((Circle*)(*itor))->getString();
							mStr.append(tempChar);

							POINT p1 = (*itor)->getPoint1();
							POINT p2 = (*itor)->getPoint2();
							RECT mRect = { p1.x,p1.y,p2.x,p2.y };
							InvalidateRect(hWnd, &mRect, TRUE);
						}
						else if (strcmp(elementTypeName, "class Rect") == 0) {
							std::wstring &mStr = ((Rect*)(*itor))->getString();
							mStr.append(tempChar);

							POINT p1 = (*itor)->getPoint1();
							POINT p2 = (*itor)->getPoint2();
							RECT mRect = { p1.x,p1.y,p2.x,p2.y };
							InvalidateRect(hWnd, &mRect, TRUE);
						}
					}

					bComp = true;
				}
				else {
					if (strcmp(elementTypeName, "class Circle") == 0) {
						std::wstring &mStr = ((Circle*)(*itor))->getString();
						mStr.erase(mStr.size() - 1, 1);

						POINT p1 = (*itor)->getPoint1();
						POINT p2 = (*itor)->getPoint2();
						RECT mRect = { p1.x,p1.y,p2.x,p2.y };
						InvalidateRect(hWnd, &mRect, TRUE);
					}
					else if (strcmp(elementTypeName, "class Rect") == 0) {
						std::wstring &mStr = ((Rect*)(*itor))->getString();
						mStr.erase(mStr.size() - 1, 1);

						POINT p1 = (*itor)->getPoint1();
						POINT p2 = (*itor)->getPoint2();
						RECT mRect = { p1.x,p1.y,p2.x,p2.y };
						InvalidateRect(hWnd, &mRect, TRUE);
					}

					bComp = false;
				}
			}
			else if (lParam & GCS_RESULTSTR) {
				bComp = false;
				if (strcmp(elementTypeName, "class Circle") == 0) {
					std::wstring &mStr = ((Circle*)(*itor))->getString();
					int mOff = mStr.size() - 1;
					mStr[mOff] = wParam;

					POINT p1 = (*itor)->getPoint1();
					POINT p2 = (*itor)->getPoint2();
					RECT mRect = { p1.x,p1.y,p2.x,p2.y };
					InvalidateRect(hWnd, &mRect, TRUE);
				}
				else if (strcmp(elementTypeName, "class Rect") == 0) {
					std::wstring &mStr = ((Rect*)(*itor))->getString();
					int mOff = mStr.size() - 1;
					mStr[mOff] = wParam;

					POINT p1 = (*itor)->getPoint1();
					POINT p2 = (*itor)->getPoint2();
					RECT mRect = { p1.x,p1.y,p2.x,p2.y };
					InvalidateRect(hWnd, &mRect, TRUE);
				}
			}
			ImmReleaseContext(hWnd, hImc);
		}
		return 0;
	}

	case WM_MOUSEWHEEL:
		if ((short)HIWORD(wParam) < 0) {
			zoomLevel -= 0.1;
			if (zoomLevel <= 0.1) {
				zoomLevel = 0.1;
			}
		}
		else {
			zoomLevel += 0.1;
		}

		printf("zoomLevel : %.2lf\n", zoomLevel);
		InvalidateRect(hWnd, NULL, TRUE);
		return 0;

	case WM_CREATE:
		hWndMain = hWnd;
		nowCursor = LoadCursor(NULL, IDC_ARROW);
		return 0;

	case WM_SETCURSOR:
		if (LOWORD(lParam) == HTCLIENT) {
			SetCursor(nowCursor);
			return TRUE;
		}
		break;

	case WM_PAINT: {
		hdc = BeginPaint(hWnd, &ps);

		list<Shape*>::iterator itor = shapeList.begin();
		int listIndex = 0;
		SetBkMode(hdc, TRANSPARENT);
		for (itor = shapeList.begin(); itor != shapeList.end(); itor++) {
			if (listIndex == selectIndex) {
				(*itor)->selectDraw(hdc, nowScreenXY, zoomLevel);
			}
			else {
				(*itor)->draw(hdc, nowScreenXY, zoomLevel);
			}
			listIndex++;
		}

		EndPaint(hWnd, &ps);
		return 0;
	}

	case WM_DESTROY:
		list<Shape*>::iterator itor;
		for (itor = shapeList.begin(); itor != shapeList.end(); itor++) {
			delete(*itor);
		}
		PostQuitMessage(0);
		return 0;
	}

	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}