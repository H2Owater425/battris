#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <winsock2.h>
#include <conio.h>
#include <process.h>
#include <ws2tcpip.h>

#include "ManyLayer/ManyLayer.h"
#include "define.h"
#include "resource.h"
#include "singleplayer.h"
#include "multiplayer.h"

void initializeConsole(void) {
	const CONSOLE_CURSOR_INFO consoleCursorInformation = { 1, false };
	const HWND consoleWindow = GetConsoleWindow();
	char command[32] = "";
	
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &consoleCursorInformation);
	SetWindowLongW(consoleWindow, GWL_STYLE, GetWindowLongW(consoleWindow, GWL_STYLE) & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX);
	sprintf_s(command, 32, "mode con cols=%d lines=%d", CONSOLE_WIDTH, CONSOLE_HEIGHT);
	system(command);
	Sleep(COMMAND_DELAY);
}

int main(void) {
	initializeConsole();

	ManyLayer manyLayer = DEFAULT_MANY_LAYER;

	HBITMAP logoBitmapHandle = manyLayer.getBitmapHandleFromResource(logoBitmap);

	manyLayer.initialize(&manyLayer);

	manyLayer.images = (Image[]){
		{ logoBitmapHandle, 114, 96, 3, false }
	};

	manyLayer.imageCount = 1;

	manyLayer.texts = (Text[]){
		{ L"Singleplayer [1]", 608, 668, 23, 64, 600, L"家具稠8", RGB(255, 255, 255), false },
		{ L"Multiplayer  [2]", 608, 740, 23, 64, 600, L"家具稠8", RGB(255, 255, 255), false }
	};

	manyLayer.textCount = 2;
	
	manyLayer.renderAll(&manyLayer);

	int loopDepth = 0;

	while(loopDepth == 0) {
		if(_kbhit()) {
			switch(_getch()) {
				case 49: {
					startSinleplayerGame(&manyLayer);

					break;
				}

				case 50: {
					manyLayer.texts = (Text[]){
						{ L"Challenged  [1]", 1016, 1050, 23, 64, 600, L"家具稠8", RGB(255, 255, 255), false },
						{ L"Challenger  [2]", 1016, 1150, 23, 64, 600, L"家具稠8", RGB(255, 255, 255), false }
					};

					manyLayer.renderAll(&manyLayer);

					loopDepth = 1;

					while(loopDepth == 1) {
						if(_kbhit()) {
							switch(_getch()) {
								case 49: {
								}

								case 50: {
								}

								case 27: {
									loopDepth = 0;
								}
							}
						}
					}

					manyLayer.images = (Image[]){
						{ logoBitmapHandle, 352, 192, 4, false }
					};

					manyLayer.imageCount = 1;

					manyLayer.texts = (Text[]){
						{ L"Singleplayer [1]", 1000, 1050, 23, 64, 600, L"家具稠8", RGB(255, 255, 255), false },
						{ L"Multiplayer  [2]", 1000, 1150, 23, 64, 600, L"家具稠8", RGB(255, 255, 255), false }
					};

					manyLayer.textCount = 2;
					
					manyLayer.renderAll(&manyLayer);

					break;
				}
				
				case 27: {
					exit(0);
					break;
				}
			}
		}
	}

	return 0;
}