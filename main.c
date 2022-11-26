#include <stdio.h>
#include <time.h>
#include <conio.h>

#define WIN32_LEAN_AND_MEAN

#include "ManyLayer/ManyLayer.h"
#include "define.h"
#include "resource.h"
#include "singleplayer.h"
#include "multiplayer.h"

// 콘솔 초기화 함수
// 커서를 지우고 콘솔의 크기를 조절하며, 콘솔의 크기 조절을 막고 이름을 바꾼다
void initializeConsole(void) {
	const CONSOLE_CURSOR_INFO consoleCursorInformation = { 1, false };
	const HWND consoleWindow = GetConsoleWindow();
	char command[32] = "";
	
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &consoleCursorInformation);
	SetWindowLongW(consoleWindow, GWL_STYLE, GetWindowLongW(consoleWindow, GWL_STYLE) & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX);
	sprintf_s(command, 32, "mode con cols=%d lines=%d", CONSOLE_WIDTH, CONSOLE_HEIGHT);
	system(command);

	SetConsoleTitleW(L"Battris");

	Sleep(COMMAND_DELAY);
}

// 메인 함수
// 메인 화면을 구성하고 입력에 따라 싱글플레이어, 멀티플레이어 게임을 실행한다
int main(void) {
	initializeConsole();

	srand(time(NULL));

	ManyLayer manyLayer = DEFAULT_MANY_LAYER;

	HBITMAP logoBitmapHandle = manyLayer.getBitmapHandleFromResource(logoBitmap);

	manyLayer.initialize(&manyLayer);

	if(RESOLUTION_MULTIPLIER != 0.625) {
		throwError(&manyLayer, "Please set your screen magnification to 125%");

		return 1;
	}

	manyLayer.images = (Image[]){
		{ logoBitmapHandle, 352, 192, 4, false }
	};

	manyLayer.imageCount = 1;

	manyLayer.texts = (Text[]){
		{ L"Singleplayer [1]", 960, 1050, 24, 64, 500, L"소야논8", RGB(255, 255, 255), false },
		{ L"Multiplayer　[2]", 960, 1150, 24, 64, 500, L"소야논8", RGB(255, 255, 255), false }
	};

	manyLayer.textCount = 2;
	
	manyLayer.renderAll(&manyLayer);

	int loopDepth = 0;

	PlaySoundW(L"sounds/main_fix.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);

	while(loopDepth == 0) {
		if(_kbhit()) {
			switch(_getch()) {
				case FIRST: {
					startGameSingleplayer(&manyLayer);

					manyLayer.images = (Image[]){
						{ logoBitmapHandle, 352, 192, 4, false }
					};

					manyLayer.imageCount = 1;

					manyLayer.texts = (Text[]){
						{ L"Singleplayer [1]", 960, 1050, 24, 64, 500, L"소야논8", RGB(255, 255, 255), false },
						{ L"Multiplayer　[2]", 960, 1150, 24, 64, 500, L"소야논8", RGB(255, 255, 255), false }
					};

					manyLayer.textCount = 2;

					manyLayer.renderAll(&manyLayer);

					break;
				}

				case SECOND: {
					manyLayer.texts[0].content = L"Challenged  [1]";
					manyLayer.texts[1].content = L"Challenger  [2]";
					manyLayer.texts[0].x = 976;
					manyLayer.texts[1].x = 976;

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

					manyLayer.texts = (Text[]){
						{ L"Singleplayer [1]", 960, 1050, 24, 64, 500, L"소야논8", RGB(255, 255, 255), false },
						{ L"Multiplayer　[2]", 960, 1150, 24, 64, 500, L"소야논8", RGB(255, 255, 255), false }
					};

					manyLayer.textCount = 2;
					
					manyLayer.renderAll(&manyLayer);

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