#include <stdio.h>
#include <time.h>
#include <conio.h>

#define WIN32_LEAN_AND_MEAN

#include "ManyLayer/ManyLayer.h"
#include "define.h"
#include "resource.h"
#include "common.h"
#include "singleplayer.h"
#include "multiplayer.h"

// 커서를 지우고 콘솔의 크기를 조절하며, 콘솔의 크기 조절과 드래그을 막고 이름을 바꾼다
void initializeConsole(void) {
	const CONSOLE_CURSOR_INFO consoleCursorInformation = { 1, false };
	const HWND consoleWindow = GetConsoleWindow();
	char command[32] = "";
	DWORD prevMode;

	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &consoleCursorInformation);
	SetWindowLongW(consoleWindow, GWL_STYLE, GetWindowLongW(consoleWindow, GWL_STYLE) & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX);
	GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &prevMode);
	SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), prevMode & ~ENABLE_QUICK_EDIT_MODE);
	sprintf_s(command, 32, "mode con cols=%d lines=%d", CONSOLE_WIDTH, CONSOLE_HEIGHT);
	system(command);

	SetConsoleTitleW(L"Battris");

	Sleep(COMMAND_DELAY);
}

// 메인 화면을 구성하고 입력에 따라 싱글플레이어, 멀티플레이어 게임을 실행한다
// 기본적으로 함수를 호출해서 장면을 전환하는 방식으로 진행된다
// 멀티플레이어는 서버와 클라이언트의 역할이 나뉘어져 있다
int main(void) {
	initializeConsole();

	Text mainTexts[4] = {
		{ L"Singleplayer [1]", 960, 986, 24, 64, 500, L"소야논8", RGB(255, 255, 255), false },
		{ L"Multiplayer　[2]", 960, 1086, 24, 64, 500, L"소야논8", RGB(255, 255, 255), false },
		{ L"Record [r]", 64, 1296, 24, 64, 500, L"소야논8", RGB(255, 255, 255), false },
		{ L"Credit [c]", 2144, 1296, 24, 64, 500, L"소야논8", RGB(255, 255, 255), false },
	};

	srand(time(NULL));

	ManyLayer manyLayer = DEFAULT_MANY_LAYER;

	HBITMAP logoBitmapHandle = manyLayer.getBitmapHandleFromResource(logoBitmap);

	manyLayer.initialize(&manyLayer);

	manyLayer.images = (Image[]){
		{ logoBitmapHandle, 352, 192, 4, false }
	};

	manyLayer.imageCount = 1;

	manyLayer.texts = mainTexts;

	manyLayer.textCount = 4;
	
	manyLayer.renderAll(&manyLayer);

	int loopDepth = 0;

	playSound(mainWave, true);

	while(loopDepth == 0) {
		if(_kbhit()) {
			switch(_getch()) {
				case FIRST: {
					startGameSingleplayer(&manyLayer);

					manyLayer.images = (Image[]){
						{ logoBitmapHandle, 352, 192, 4, false }
					};

					manyLayer.imageCount = 1;

					manyLayer.texts = mainTexts;

					manyLayer.textCount = 4;

					manyLayer.renderAll(&manyLayer);

					break;
				}

				case SECOND: {
					manyLayer.texts = (Text[]){
						{ L"Challenged  [1]", 976, 986, 24, 64, 500, L"소야논8", RGB(255, 255, 255), false },
						{ L"Challenger  [2]", 976, 1086, 24, 64, 500, L"소야논8", RGB(255, 255, 255), false },
					};

					manyLayer.textCount = 2;

					manyLayer.renderAll(&manyLayer);

					loopDepth = 1;

					while(loopDepth == 1) {
						if(_kbhit()) {
							switch(_getch()) {
								case FIRST: {
									PlaySoundW(NULL, NULL, NULL);
									startGameMultiplayer(&manyLayer, false);

									loopDepth = 0;

									break;
								}

								case SECOND: {
									PlaySoundW(NULL, NULL, NULL);
									startGameMultiplayer(&manyLayer, true);
									loopDepth = 0;

									break;
								}

								case EXIT: {
									loopDepth = 0;

									break;
								}
							}
						}
					}

					manyLayer.images = (Image[]){
						{ logoBitmapHandle, 352, 192, 4, false }
					};

					manyLayer.imageCount = 1;

					manyLayer.texts = mainTexts;

					manyLayer.textCount = 4;
					
					manyLayer.renderAll(&manyLayer);

					break;
				}

				case RECORD: {

					break;
				}

				case CREDIT: {
					stopSound();

					playSound(creditWave, false);

					manyLayer.images = NULL;

					manyLayer.imageCount = 0;

					manyLayer.texts = (Text[]){
						{ L"총괄", 1264, 16 + 1424, 15, 40, 500, L"소야논8", RGB(255, 255, 255), false},
						{ L"노래", 1264, 208 + 1424, 15, 40, 500, L"소야논8", RGB(255, 255, 255), false},
						{ L"테스터", 1248, 496 + 1424, 15, 40, 500, L"소야논8", RGB(255, 255, 255), false},
						{ L"장비", 1264, 784 + 1424, 15, 40, 500, L"소야논8", RGB(255, 255, 255), false},
						{ L"1401 김강민", 1116, 80 + 1424, 24, 64, 500, L"소야논8", RGB(255, 255, 255), false},
						{ L"익명의 지인", 1128, 272 + 1424, 24, 64, 500, L"소야논8", RGB(255, 255, 255), false},
						{ L"C00MP3K", 1160, 368 + 1424, 24, 64, 500, L"소야논8", RGB(255, 255, 255), false},
						{ L"1421 이진서", 1128, 560 + 1424, 24, 64, 500, L"소야논8", RGB(255, 255, 255), false},
						{ L"외 1-4 학생들", 1088, 656 + 1424, 24, 64, 500, L"소야논8", RGB(255, 255, 255), false},
						{ L"핵8나5배", 1176, 848 + 1424, 24, 64, 500, L"소야논8", RGB(255, 255, 255), false},
						{ L"은하초지적21더하기", 1008, 944 + 1424, 24, 64, 500, L"소야논8", RGB(255, 255, 255), false},
						{ L"은하판형전산기초지적8더하기", 864, 1040 + 1424, 24, 64, 500, L"소야논8", RGB(255, 255, 255), false},
						{ L"은하싹2", 1184, 1136 + 1424, 24, 64, 500, L"소야논8", RGB(255, 255, 255), false},
						{ L"은하책활성2", 1128, 1232 + 1424, 24, 64, 500, L"소야논8", RGB(255, 255, 255), false},
					};
					
					manyLayer.textCount = 14;
					
					manyLayer.renderAll(&manyLayer);

					Sleep(5120);

					while(manyLayer.texts[13].y != -128) {
						for(int i = 0; i < 14; i++) {
							manyLayer.texts[i].y -= 1;
						}

						manyLayer.renderAll(&manyLayer);

						Sleep(11);
					}

					Sleep(5120);

					stopSound();

					manyLayer.images = (Image[]){
						{ logoBitmapHandle, 352, 192, 4, false }
					};

					manyLayer.imageCount = 1;

					manyLayer.texts = mainTexts;

					manyLayer.textCount = 4;

					manyLayer.renderAll(&manyLayer);

					playSound(mainWave, true);

					break;
				}
				
				case EXIT: {
					loopDepth = -1;

					break;
				}
			}
		}
	}

	return 0;
}