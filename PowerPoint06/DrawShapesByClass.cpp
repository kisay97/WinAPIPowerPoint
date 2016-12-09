#include <windows.h>
#include <imm.h>
#include <list>
#include <iostream>
#include <string>
#include <fstream>
#include "Shape.h"
#include "Circle.h"
#include "Rect.h"
#include "Line.h"
#include "resource.h"
#import <msxml6.dll>

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
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	static HCURSOR nowCursor;

	HIMC hImc;

	OPENFILENAME OFN;
	wchar_t str[300] = { 0, };
	wchar_t lpstrFile[MAX_PATH] = TEXT("");

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

		case ID_File_Open:
			puts("열기");
			memset(&OFN, 0, sizeof(OPENFILENAME));
			OFN.lStructSize = sizeof(OPENFILENAME);
			OFN.hwndOwner = hWnd;
			OFN.lpstrFilter = TEXT("MindMaple File(*.mmp)\0*.mmp;\0XML File(*.xml)\0*.xml;\0");
			OFN.lpstrFile = lpstrFile;
			OFN.nMaxFile = 256;
			OFN.lpstrInitialDir = TEXT("D:\\");
			if (GetOpenFileName(&OFN) != 0) {
				wsprintf(str, TEXT("%s 파일을 선택했습니다."), OFN.lpstrFile);
				MessageBox(hWnd, str, TEXT("파일 열기 성공"), MB_OK);

				if (OFN.nFilterIndex == 1) { //open mmp file
					HANDLE hFile;
					DWORD size = 100000;
					hFile = CreateFile(
						OFN.lpstrFile,
						GENERIC_READ,
						FILE_SHARE_READ,
						NULL, OPEN_EXISTING, 0, 0);
					wchar_t *fileStr = new wchar_t[50000];
					memset(fileStr, 0, size);
					DWORD cbFile;
					ReadFile(hFile, fileStr, size, &cbFile, NULL);
					CloseHandle(hFile);

					wchar_t *token;				//fileStr tokener
					wchar_t delimit[2] = { L'\n',0 }; //filter
					wchar_t *context;
					wchar_t *pShapeContext;
					wchar_t *fileZoomLevel;
					wchar_t *fileNowScreenXY;
					wchar_t *fileShape;

					//get fileZoomLevel
					token = wcstok_s(fileStr, delimit, &context);
					if (token != NULL) {
						fileZoomLevel = token;
						wprintf(L"%c %c %c\n", fileZoomLevel[0], fileZoomLevel[1], fileZoomLevel[2]);

						wchar_t *pEndZoomLevel;
						zoomLevel = wcstod(fileZoomLevel + 1, &pEndZoomLevel);

						token = wcstok_s(context, delimit, &context);
					}

					//get fileNowScreenXY
					if (token != NULL) {
						fileNowScreenXY = token;
						wchar_t *pEnd;
						nowScreenXY.x = wcstol(fileNowScreenXY, &pEnd, 10);
						nowScreenXY.y = wcstol(pEnd, NULL, 10);

						token = wcstok_s(context, delimit, &context);
					}

					//get fileShapes
					shapeList.clear();
					while (token != NULL) {
						fileShape = token;

						wchar_t *shapeToken;
						wchar_t *shapeDelimit = L"\t";

						//get shapeType
						shapeToken = wcstok_s(fileShape, shapeDelimit, &pShapeContext);
						if (shapeToken != NULL) {
							if (wcscmp(shapeToken, L"class Circle") == 0) {
								Shape *fileShapePointer = new Circle();
								POINT tP1, tP2;
								std::wstring tTxt;

								shapeToken = wcstok_s(pShapeContext, shapeDelimit, &pShapeContext);
								wchar_t *pShapePoint;

								//get shapeP1
								if (shapeToken != NULL) {
									tP1.x = wcstol(shapeToken, &pShapePoint, 10);
									tP1.y = wcstol(pShapePoint, NULL, 10);

									shapeToken = wcstok_s(pShapeContext, shapeDelimit, &pShapeContext);
								}

								//get shapeP2
								if (shapeToken != NULL) {
									tP2.x = wcstol(shapeToken, &pShapePoint, 10);
									tP2.y = wcstol(pShapePoint, NULL, 10);

									shapeToken = wcstok_s(pShapeContext, shapeDelimit, &pShapeContext);
								}

								//get shapeText
								if (shapeToken != NULL) {
									wchar_t *tTxtWchar = new wchar_t[wcslen(shapeToken) + 1];
									wcsncpy_s(tTxtWchar, wcslen(shapeToken) + 1, shapeToken, wcslen(shapeToken));
									tTxtWchar[wcslen(shapeToken)] = L'\0';
									tTxt = tTxtWchar;
								}

								//set fileShape and push in list
								fileShapePointer->setPosition(tP1, tP2, false);
								((Circle*)fileShapePointer)->setString(tTxt);
								shapeList.push_back(fileShapePointer);
							}
							else if (wcscmp(shapeToken, L"class Rect") == 0) {
								Shape *fileShapePointer = new Rect();
								POINT tP1, tP2;
								std::wstring tTxt;

								shapeToken = wcstok_s(pShapeContext, shapeDelimit, &pShapeContext);
								wchar_t *pShapePoint;

								//get shapeP1
								if (shapeToken != NULL) {
									tP1.x = wcstol(shapeToken, &pShapePoint, 10);
									tP1.y = wcstol(pShapePoint, NULL, 10);

									shapeToken = wcstok_s(pShapeContext, shapeDelimit, &pShapeContext);
								}

								//get shapeP2
								if (shapeToken != NULL) {
									tP2.x = wcstol(shapeToken, &pShapePoint, 10);
									tP2.y = wcstol(pShapePoint, NULL, 10);

									shapeToken = wcstok_s(pShapeContext, shapeDelimit, &pShapeContext);
								}

								//get shapeText
								if (shapeToken != NULL) {
									wchar_t *tTxtWchar = new wchar_t[wcslen(shapeToken) + 1];
									wcsncpy_s(tTxtWchar, wcslen(shapeToken) + 1, shapeToken, wcslen(shapeToken));
									tTxtWchar[wcslen(shapeToken)] = L'\0';
									tTxt = tTxtWchar;
								}

								//set fileShape and push in list
								fileShapePointer->setPosition(tP1, tP2, false);
								((Rect*)fileShapePointer)->setString(tTxt);
								shapeList.push_back(fileShapePointer);
							}
							else if (wcscmp(shapeToken, L"class Line") == 0) {
								Shape *fileShapePointer = new Line();
								POINT tP1, tP2;

								shapeToken = wcstok_s(pShapeContext, shapeDelimit, &pShapeContext);
								wchar_t *pShapePoint;

								//get shapeP1
								if (shapeToken != NULL) {
									tP1.x = wcstol(shapeToken, &pShapePoint, 10);
									tP1.y = wcstol(pShapePoint, NULL, 10);

									shapeToken = wcstok_s(pShapeContext, shapeDelimit, &pShapeContext);
								}

								//get shapeP2
								if (shapeToken != NULL) {
									tP2.x = wcstol(shapeToken, &pShapePoint, 10);
									tP2.y = wcstol(pShapePoint, NULL, 10);

									shapeToken = wcstok_s(pShapeContext, shapeDelimit, &pShapeContext);
								}

								//set fileShape and push in list
								fileShapePointer->setPosition(tP1, tP2, false);
								shapeList.push_back(fileShapePointer);
							}
						}
						token = wcstok_s(context, delimit, &context);
					}

					delete[]fileStr;
				}
				else if (OFN.nFilterIndex == 2) { //open xml file
					if (SUCCEEDED(::CoInitialize(NULL))) {
						_bstr_t bstr1, bstr2;
						_variant_t variant;

						IXMLDOMDocumentPtr xmlDocument;
						IXMLDOMProcessingInstructionPtr xmlProcessingInstruction;
						IXMLDOMElementPtr rootElement;
						VARIANT_BOOL variantBool;

						if (!SUCCEEDED(xmlDocument.CreateInstance(__uuidof(DOMDocument)))) {
							::wprintf(L"xmlDocument가 생성되지 않았습니다.\n");
							::CoUninitialize();
							return -1;
						}

						/*xml을 불러와 XMLDOMDocument에 기억시킵니다.*/
						variant = OFN.lpstrFile;
						xmlDocument->load(variant, &variantBool);

						if (variantBool == VARIANT_TRUE) {
							//루트 엘리먼트 가져오기
							xmlDocument->get_documentElement(&rootElement);

							//줌 레벨 가져오기
							BSTR zoomLevelBstr;
							IXMLDOMNodePtr zoomLevelElement;
							rootElement->selectSingleNode(L"/file/zoomLevel", &zoomLevelElement);
							zoomLevelElement->get_text(&zoomLevelBstr);
							zoomLevel = wcstod(zoomLevelBstr, NULL);

							//원점 가져오기
							BSTR nowScreenXYBstr;
							IXMLDOMNodePtr nowScreenXYElement;
							rootElement->selectSingleNode(L"/file/nowScreenXY", &nowScreenXYElement);
							nowScreenXYElement->get_text(&nowScreenXYBstr);
							wchar_t *tNowScreenXY;
							nowScreenXY.x = wcstol(nowScreenXYBstr, &tNowScreenXY, 10);
							nowScreenXY.y = wcstol(tNowScreenXY, NULL, 10);

							//도형들 가져오기
							IXMLDOMNodeListPtr xmlShapeList;
							IXMLDOMNodePtr shapeListElement, shapeElement;
							rootElement->selectSingleNode(L"/file/shapeList", &shapeListElement);
							shapeListElement->get_childNodes(&xmlShapeList);

							//도형 순회
							shapeList.clear();
							int i;
							long xmlShapeListLength;
							xmlShapeList->get_length(&xmlShapeListLength);
							for (i = 0; i < xmlShapeListLength; i++) {
								xmlShapeList->get_item(i, &shapeElement);

								//도형 childNode 가져오기
								IXMLDOMNodeListPtr shapeChildNodes;
								shapeElement->get_childNodes(&shapeChildNodes);

								Shape* xmlShape = NULL;
								//class
								BSTR classBstr;
								IXMLDOMNodePtr classElement;
								shapeChildNodes->get_item(0, &classElement);
								classElement->get_text(&classBstr);
								if (wcscmp(classBstr, L"Circle") == 0) {
									xmlShape = new Circle();
								}
								else if (wcscmp(classBstr, L"Rect") == 0) {
									xmlShape = new Rect();
								}
								else if (wcscmp(classBstr, L"Line") == 0) {
									xmlShape = new Line();
								}

								//p1
								BSTR p1Bstr, tP1Bstr;
								IXMLDOMNodePtr p1Element;
								shapeChildNodes->get_item(1, &p1Element);
								p1Element->get_text(&p1Bstr);
								long p1X, p1Y;
								p1X = wcstol(p1Bstr, &tP1Bstr, 10);
								p1Y = wcstol(tP1Bstr, NULL, 10);

								//p2
								BSTR p2Bstr, tP2Bstr;
								IXMLDOMNodePtr p2Element;
								shapeChildNodes->get_item(2, &p2Element);
								p2Element->get_text(&p2Bstr);
								long p2X, p2Y;
								p2X = wcstol(p2Bstr, &tP2Bstr, 10);
								p2Y = wcstol(tP2Bstr, NULL, 10);

								xmlShape->setPosition({ p1X,p1Y }, { p2X,p2Y }, false);

								//txt
								BSTR txtBstr;
								IXMLDOMNodePtr txtElement;
								shapeChildNodes->get_item(3, &txtElement);
								txtElement->get_text(&txtBstr);
								std::wstring xmlTxt = txtBstr;
								if (wcscmp(classBstr, L"Circle") == 0) {
									((Circle*)(xmlShape))->setString(xmlTxt);
								}
								else if (wcscmp(classBstr, L"Rect") == 0) {
									((Rect*)(xmlShape))->setString(xmlTxt);
								}

								//push xmlShape in shapeList
								shapeList.push_back(xmlShape);
							}
						}
						else {
							::wprintf(L"xmlDocument가 생성되지 않았습니다.\n");
							::CoUninitialize();
							return -1;
						}

						::CoUninitialize();
					}
				}
				else {
					wsprintf(str, TEXT("오류"), OFN.lpstrFile);
					MessageBox(hWnd, str, TEXT("알 수 없는 확장자 입니다!!"), MB_OK);
				}
			}
			else {
				wsprintf(str, TEXT("그런 파일이 없습니다."));
				MessageBox(hWnd, str, TEXT("파일 열기 실패"), MB_OK);
			}

			InvalidateRect(hWnd, NULL, TRUE);
			break;

		case ID_File_Save:
			puts("저장");
			memset(&OFN, 0, sizeof(OPENFILENAME));
			OFN.lStructSize = sizeof(OPENFILENAME);
			OFN.hwndOwner = hWnd;
			OFN.lpstrFilter = TEXT("MindMaple File(*.mmp)\0*.mmp;\0XML File(*.xml)\0*.xml;\0");
			OFN.lpstrFile = lpstrFile;
			OFN.nMaxFile = 256;
			OFN.lpstrInitialDir = TEXT("D:\\");
			OFN.Flags = OFN_NOTESTFILECREATE | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;

			if (GetSaveFileName(&OFN) != 0) {
				if (OFN.nFilterIndex == 1) { //mmp File Save
					puts("mmp 저장하려 함");

					wchar_t *fileBuf = new wchar_t[50000]; //file write buffer
					memset(fileBuf, 0, 50000 * sizeof(wchar_t));
					wchar_t fileZoomLevel[100] = { 0, };
					wchar_t fileNowScreenXY[200] = { 0, };

					swprintf_s(fileZoomLevel, L"%.2lf\r\n", zoomLevel);
					wcscat_s(fileBuf, 50000, fileZoomLevel);
					wsprintf(fileNowScreenXY, L"%d %d\r\n", nowScreenXY.x, nowScreenXY.y);
					wcscat_s(fileBuf, 50000, fileNowScreenXY);
					
					list<Shape*>::iterator itor = shapeList.begin();
					for (itor = shapeList.begin(); itor != shapeList.end(); itor++) {
						wchar_t fileShapeInfo[1000] = { 0, };

						const char *elementTypeName = typeid(*(*itor)).name();
						const wchar_t *elementTxt = L"";
						if (strcmp(elementTypeName, "class Circle") == 0) {
							elementTxt = ((Circle*)(*itor))->getString().c_str();
							wcscat_s(fileShapeInfo, 1000, L"class Circle\t");
						}
						else if (strcmp(elementTypeName, "class Rect") == 0) {
							elementTxt = ((Rect*)(*itor))->getString().c_str();
							wcscat_s(fileShapeInfo, 1000, L"class Rect\t");
						}
						else {
							wcscat_s(fileShapeInfo, 1000, L"class Line\t");
						}

						wchar_t tFileShapeInfo[100] = { 0, };
						if (++itor == shapeList.end()) {
							itor--;
							swprintf_s(tFileShapeInfo, L"%d %d\t%d %d\t%s",
								(*itor)->getPoint1().x, (*itor)->getPoint1().y,
								(*itor)->getPoint2().x, (*itor)->getPoint2().y,
								elementTxt);
							wcscat_s(fileShapeInfo, 1000, tFileShapeInfo);
						}
						else {
							itor--;
							swprintf_s(tFileShapeInfo, L"%d %d\t%d %d\t%s\r\n",
								(*itor)->getPoint1().x, (*itor)->getPoint1().y,
								(*itor)->getPoint2().x, (*itor)->getPoint2().y,
								elementTxt);
							wcscat_s(fileShapeInfo, 1000, tFileShapeInfo);
						}

						wcscat_s(fileBuf, 50000, fileShapeInfo);
					}

					wchar_t *filePathAndNameAndExtension = 
						new wchar_t[wcslen(OFN.lpstrFile) + 5];
					wcscpy_s(filePathAndNameAndExtension, 
						wcslen(OFN.lpstrFile)+1, OFN.lpstrFile);
					wcscat_s(filePathAndNameAndExtension,
						wcslen(OFN.lpstrFile) + 5, L".mmp");
					
					HANDLE hFile = CreateFile(
						filePathAndNameAndExtension,
						GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
						FILE_ATTRIBUTE_NORMAL, NULL);

					DWORD dwWritten; //실제로 파일에 얼마나 써졌는지 확인용
					wchar_t szTmp[2];
					WORD wd = 0xfeff;

					WriteFile(hFile, &wd, sizeof(wd), &dwWritten, NULL);
					WriteFile(hFile, fileBuf, wcslen(fileBuf)*2, &dwWritten, NULL);
					CloseHandle(hFile);

					wsprintf(str, TEXT("%s 파일이 저장됬습니다."), 
						filePathAndNameAndExtension);
					MessageBox(hWnd, str, TEXT("파일 저장 성공"), MB_OK);
				}
				else if (OFN.nFilterIndex == 2) { //xml File Save
					puts("xml 저장하려 함");
					if (SUCCEEDED(::CoInitialize(NULL))) {
						{
							_bstr_t bstr1, bstr2, bstr3; //태그, 값, 최종 xml
							_variant_t variant; 
							IXMLDOMDocumentPtr xmlDocument; //xml 전체를 갖고 있는 객체
							IXMLDOMProcessingInstructionPtr xmlProcessingInstruction;
							IXMLDOMElementPtr xmlElement1, xmlElement2;

							if (!SUCCEEDED(xmlDocument.CreateInstance(__uuidof(DOMDocument)))) {
								::wprintf(L"xmlDocument가 생성되지 않았습니다.\n");
								::CoUninitialize();
								return -1;
							}

							bstr1 = L"xml";
							bstr2 = L"version=\"1.0\" encoding=\"UNICODE\"";
							xmlDocument->createProcessingInstruction(bstr1, bstr2, 
								&xmlProcessingInstruction);
							xmlDocument->appendChild(xmlProcessingInstruction, NULL);

							bstr1 = L"file";
							xmlDocument->createElement(bstr1, &xmlElement1);

							bstr1 = L"zoomLevel";
							bstr2 = zoomLevel;
							xmlDocument->createElement(bstr1, &xmlElement2);
							xmlElement2->put_text(bstr2);
							xmlDocument->appendChild(xmlElement1, NULL);
							xmlElement1->appendChild(xmlElement2, NULL);

							bstr1 = L"nowScreenXY";
							wchar_t nowScreenXYStr[30] = { 0, };
							swprintf_s(nowScreenXYStr, L"%d %d", nowScreenXY.x, nowScreenXY.y);
							bstr2 = nowScreenXYStr;
							xmlDocument->createElement(bstr1, &xmlElement2);
							xmlElement2->put_text(bstr2);
							xmlElement1->appendChild(xmlElement2, NULL);

							bstr1 = L"shapeList";
							xmlDocument->createElement(bstr1, &xmlElement2);
							
							list<Shape*>::iterator itor = shapeList.begin();
							for (itor = shapeList.begin(); itor != shapeList.end(); itor++) {
								_bstr_t tagName, value;
								IXMLDOMElementPtr xmlElement3, xmlElement4; //shape, shapeValue

								//make element shape
								tagName = L"shape";
								xmlDocument->createElement(tagName, &xmlElement3);

								//make element class in shape
								const char *elementTypeName = typeid(*(*itor)).name();
								if (strcmp(elementTypeName, "class Circle") == 0) {
									tagName = L"class";
									value = L"Circle";
									xmlDocument->createElement(tagName, &xmlElement4);
									xmlElement4->put_text(value);
									xmlElement3->appendChild(xmlElement4, NULL);
								}
								else if (strcmp(elementTypeName, "class Rect") == 0) {
									tagName = L"class";
									value = L"Rect";
									xmlDocument->createElement(tagName, &xmlElement4);
									xmlElement4->put_text(value);
									xmlElement3->appendChild(xmlElement4, NULL);
								}
								else if (strcmp(elementTypeName, "class Line") == 0) {
									tagName = L"class";
									value = L"Line";
									xmlDocument->createElement(tagName, &xmlElement4);
									xmlElement4->put_text(value);
									xmlElement3->appendChild(xmlElement4, NULL);
								}

								//make element p1 in shape
								tagName = L"p1";
								wchar_t pStr[30] = { 0, };
								swprintf_s(pStr, L"%d %d", (*itor)->getPoint1().x, (*itor)->getPoint1().y);
								value = pStr;
								xmlDocument->createElement(tagName, &xmlElement4);
								xmlElement4->put_text(value);
								xmlElement3->appendChild(xmlElement4, NULL);

								//make element p2 in shape
								tagName = L"p2";
								swprintf_s(pStr, L"%d %d", (*itor)->getPoint2().x, (*itor)->getPoint2().y);
								value = pStr;
								xmlDocument->createElement(tagName, &xmlElement4);
								xmlElement4->put_text(value);
								xmlElement3->appendChild(xmlElement4, NULL);

								//make element txt in shape
								if (strcmp(elementTypeName, "class Circle") == 0) {
									tagName = L"txt";
									value = ((Circle*)(*itor))->getString().c_str();
									xmlDocument->createElement(tagName, &xmlElement4);
									xmlElement4->put_text(value);
									xmlElement3->appendChild(xmlElement4, NULL);
								}
								else if (strcmp(elementTypeName, "class Rect") == 0) {
									tagName = L"txt";
									value = ((Rect*)(*itor))->getString().c_str();
									xmlDocument->createElement(tagName, &xmlElement4);
									xmlElement4->put_text(value);
									xmlElement3->appendChild(xmlElement4, NULL);
								}
								else if (strcmp(elementTypeName, "class Line") == 0) {
									tagName = L"txt";
									value = L"";
									xmlDocument->createElement(tagName, &xmlElement4);
									xmlElement4->put_text(value);
									xmlElement3->appendChild(xmlElement4, NULL);
								}

								//push element shape in shapeList
								xmlElement2->appendChild(xmlElement3, NULL);
							}

							//push element shapeList in file
							xmlElement1->appendChild(xmlElement2, NULL);

							//print Document
							xmlDocument->get_xml(bstr3.GetAddress());
							::wprintf(L"%s", (const wchar_t*)bstr3);

							//save Document
							wchar_t *filePathAndNameAndExtension =
								new wchar_t[wcslen(OFN.lpstrFile) + 5];
							wcscpy_s(filePathAndNameAndExtension,
								wcslen(OFN.lpstrFile) + 1, OFN.lpstrFile);
							wcscat_s(filePathAndNameAndExtension,
								wcslen(OFN.lpstrFile) + 5, L".xml");
							bstr1 = filePathAndNameAndExtension;
							variant = bstr1;
							xmlDocument->save(variant);

							wsprintf(str, TEXT("%s 파일이 저장됬습니다."),
								filePathAndNameAndExtension);
							MessageBox(hWnd, str, TEXT("파일 저장 성공"), MB_OK);
						}
						::CoUninitialize();
					}
				}
			}
			else {
				wsprintf(str, TEXT("파일 저장에 실패했습니다."));
				MessageBox(hWnd, str, TEXT("파일 저장 실패"), MB_OK);
			}

			break;
		}

		return 0;

	case WM_LBUTTONDOWN: {
		if (canDrawing) {
			puts("isDrawing");

			list<Shape*>::reverse_iterator ritor = shapeList.rbegin();
			POINT currentMousePos = { LOWORD(lParam), HIWORD(lParam) };

			POINT logicalMousePos = {
				(currentMousePos.x / zoomLevel) + nowScreenXY.x,
				(currentMousePos.y / zoomLevel) + nowScreenXY.y };

			printf("MouseX : %d, MouseY : %d\n", currentMousePos.x, currentMousePos.y);
			(*ritor)->setPosition(logicalMousePos, logicalMousePos, isDrawing = true);

			isMoving = false;
			oldX = logicalMousePos.x;
			oldY = logicalMousePos.y;
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
			POINT currentMousePos = { LOWORD(lParam), HIWORD(lParam) };
			POINT logicalMousePos = {
				(currentMousePos.x / zoomLevel) + nowScreenXY.x,
				(currentMousePos.y / zoomLevel) + nowScreenXY.y };
			(*itor)->setPosition({ oldX, oldY }, logicalMousePos, false);
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

			POINT vMouseMove = { currentMousePos.x - oldX, currentMousePos.y - oldY };
			double logicalMoveX = vMouseMove.x / zoomLevel;
			double logicalMoveY = vMouseMove.y / zoomLevel;
			double logicalMoveXIntegerParts;
			double logicalMoveYIntegerParts;
			double logicalMoveXFractionalParts =
				modf(logicalMoveX, &logicalMoveXIntegerParts);
			double logicalMoveYFractionalParts =
				modf(logicalMoveY, &logicalMoveYIntegerParts);
			POINT vMove = { logicalMoveXIntegerParts, logicalMoveYIntegerParts };

			POINT selectElementP1 = (*itor)->getPoint1();
			POINT selectElementP2 = (*itor)->getPoint2();
			POINT movedP1 = { selectElementP1.x + vMove.x, selectElementP1.y + vMove.y };
			POINT movedP2 = { selectElementP2.x + vMove.x, selectElementP2.y + vMove.y };
			(*itor)->setPosition(movedP1, movedP2, false);

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

				POINT logicalMousePos = {
					(currentMousePos.x / zoomLevel) + nowScreenXY.x,
					(currentMousePos.y / zoomLevel) + nowScreenXY.y };
				(*itor)->setPosition(logicalMousePos, (*itor)->getPoint2(), false);
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

				POINT logicalMousePos = {
					(currentMousePos.x / zoomLevel) + nowScreenXY.x,
					(currentMousePos.y / zoomLevel) + nowScreenXY.y };
				(*itor)->setPosition((*itor)->getPoint1(), logicalMousePos, false);
				break;
			}
			}

			oldX = 0;
			oldY = 0;
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
			POINT logicalMousePos = {
				(currentMousePos.x / zoomLevel) + nowScreenXY.x,
				(currentMousePos.y / zoomLevel) + nowScreenXY.y };

			(*itor)->setPosition({ oldX,oldY }, logicalMousePos, isDrawing);

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

			POINT vMouseMove = { currentMousePos.x - oldX, currentMousePos.y - oldY };
			double logicalMoveX = vMouseMove.x / zoomLevel;
			double logicalMoveY = vMouseMove.y / zoomLevel;
			double logicalMoveXIntegerParts;
			double logicalMoveYIntegerParts;
			double logicalMoveXFractionalParts =
				modf(logicalMoveX, &logicalMoveXIntegerParts);
			double logicalMoveYFractionalParts =
				modf(logicalMoveY, &logicalMoveYIntegerParts);
			POINT vMove = { logicalMoveXIntegerParts, logicalMoveYIntegerParts };

			POINT &selectElementP1 = (*itor)->getPoint1();
			POINT &selectElementP2 = (*itor)->getPoint2();
			POINT movedP1 = { selectElementP1.x + vMove.x, selectElementP1.y + vMove.y };
			POINT movedP2 = { selectElementP2.x + vMove.x, selectElementP2.y + vMove.y };
			(*itor)->setPosition(movedP1, movedP2, false);

			oldX = currentMousePos.x - (int)(logicalMoveXFractionalParts*zoomLevel);
			oldY = currentMousePos.y - (int)(logicalMoveYFractionalParts*zoomLevel);

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

				POINT logicalMousePos = {
					(currentMousePos.x / zoomLevel) + nowScreenXY.x,
					(currentMousePos.y / zoomLevel) + nowScreenXY.y };
				(*itor)->setPosition(logicalMousePos, (*itor)->getPoint2(), true);
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

				POINT logicalMousePos = {
					(currentMousePos.x / zoomLevel) + nowScreenXY.x,
					(currentMousePos.y / zoomLevel) + nowScreenXY.y };
				(*itor)->setPosition((*itor)->getPoint1(), logicalMousePos, true);
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
				RECT mRect = {
					(p1.x - nowScreenXY.x)*zoomLevel,
					(p1.y - nowScreenXY.y)*zoomLevel,
					(p2.x - nowScreenXY.x)*zoomLevel,
					(p2.y - nowScreenXY.y)*zoomLevel };
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
				RECT mRect = {
					(p1.x - nowScreenXY.x)*zoomLevel,
					(p1.y - nowScreenXY.y)*zoomLevel,
					(p2.x - nowScreenXY.x)*zoomLevel,
					(p2.y - nowScreenXY.y)*zoomLevel };
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
							RECT mRect = {
								(p1.x - nowScreenXY.x)*zoomLevel,
								(p1.y - nowScreenXY.y)*zoomLevel,
								(p2.x - nowScreenXY.x)*zoomLevel,
								(p2.y - nowScreenXY.y)*zoomLevel };
							InvalidateRect(hWnd, &mRect, TRUE);
						}
						else if (strcmp(elementTypeName, "class Rect") == 0) {
							std::wstring &mStr = ((Rect*)(*itor))->getString();
							int mOff = mStr.size() - 1;
							mStr[mOff] = tempChar[0];

							POINT p1 = (*itor)->getPoint1();
							POINT p2 = (*itor)->getPoint2();
							RECT mRect = {
								(p1.x - nowScreenXY.x)*zoomLevel,
								(p1.y - nowScreenXY.y)*zoomLevel,
								(p2.x - nowScreenXY.x)*zoomLevel,
								(p2.y - nowScreenXY.y)*zoomLevel };
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
							RECT mRect = {
								(p1.x - nowScreenXY.x)*zoomLevel,
								(p1.y - nowScreenXY.y)*zoomLevel,
								(p2.x - nowScreenXY.x)*zoomLevel,
								(p2.y - nowScreenXY.y)*zoomLevel };
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
						RECT mRect = {
							(p1.x - nowScreenXY.x)*zoomLevel,
							(p1.y - nowScreenXY.y)*zoomLevel,
							(p2.x - nowScreenXY.x)*zoomLevel,
							(p2.y - nowScreenXY.y)*zoomLevel };
						InvalidateRect(hWnd, &mRect, TRUE);
					}
					else if (strcmp(elementTypeName, "class Rect") == 0) {
						std::wstring &mStr = ((Rect*)(*itor))->getString();
						mStr.erase(mStr.size() - 1, 1);

						POINT p1 = (*itor)->getPoint1();
						POINT p2 = (*itor)->getPoint2();
						RECT mRect = {
							(p1.x - nowScreenXY.x)*zoomLevel,
							(p1.y - nowScreenXY.y)*zoomLevel,
							(p2.x - nowScreenXY.x)*zoomLevel,
							(p2.y - nowScreenXY.y)*zoomLevel };
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
					RECT mRect = {
						(p1.x - nowScreenXY.x)*zoomLevel,
						(p1.y - nowScreenXY.y)*zoomLevel,
						(p2.x - nowScreenXY.x)*zoomLevel,
						(p2.y - nowScreenXY.y)*zoomLevel };
					InvalidateRect(hWnd, &mRect, TRUE);
				}
				else if (strcmp(elementTypeName, "class Rect") == 0) {
					std::wstring &mStr = ((Rect*)(*itor))->getString();
					int mOff = mStr.size() - 1;
					mStr[mOff] = wParam;

					POINT p1 = (*itor)->getPoint1();
					POINT p2 = (*itor)->getPoint2();
					RECT mRect = {
						(p1.x - nowScreenXY.x)*zoomLevel,
						(p1.y - nowScreenXY.y)*zoomLevel,
						(p2.x - nowScreenXY.x)*zoomLevel,
						(p2.y - nowScreenXY.y)*zoomLevel };
					InvalidateRect(hWnd, &mRect, TRUE);
				}
			}
			ImmReleaseContext(hWnd, hImc);
		}
		return 0;
	}

	case WM_MOUSEWHEEL: {
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
	}

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

		HFONT myFont =
			CreateFont(16 * (zoomLevel), 0,
				0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"돋움");
		HFONT oldFont = (HFONT)SelectObject(hdc, myFont);
		DeleteObject(oldFont);

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

	case WM_DESTROY: {
		list<Shape*>::iterator itor;
		for (itor = shapeList.begin(); itor != shapeList.end(); itor++) {
			delete(*itor);
		}
		PostQuitMessage(0);
		return 0;
	}
	}

	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}