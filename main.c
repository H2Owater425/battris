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

// Ŀ���� ����� �ܼ��� ũ�⸦ �����ϸ�, �ܼ��� ũ�� ������ �巡���� ���� �̸��� �ٲ۴�
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

// ���� ȭ���� �����ϰ� �Է¿� ���� �̱��÷��̾�, ��Ƽ�÷��̾� ������ �����Ѵ�
// �⺻������ �Լ��� ȣ���ؼ� ����� ��ȯ�ϴ� ������� ����ȴ�
// ��Ƽ�÷��̾�� ������ Ŭ���̾�Ʈ�� ������ �������� �ִ�
int main(void) {
	initializeConsole();

	Text mainTexts[4] = {
		{ L"Singleplayer [1]", 960, 986, 24, 64, 500, L"�Ҿ߳�8", RGB(255, 255, 255), false },
		{ L"Multiplayer��[2]", 960, 1086, 24, 64, 500, L"�Ҿ߳�8", RGB(255, 255, 255), false },
		{ L"Record [r]", 64, 1296, 24, 64, 500, L"�Ҿ߳�8", RGB(255, 255, 255), false },
		{ L"Credit [c]", 2144, 1296, 24, 64, 500, L"�Ҿ߳�8", RGB(255, 255, 255), false },
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
						{ L"Challenged  [1]", 976, 986, 24, 64, 500, L"�Ҿ߳�8", RGB(255, 255, 255), false },
						{ L"Challenger  [2]", 976, 1086, 24, 64, 500, L"�Ҿ߳�8", RGB(255, 255, 255), false },
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
						{ L"�Ѱ�", 1264, 16 + 1424, 15, 40, 500, L"�Ҿ߳�8", RGB(255, 255, 255), false},
						{ L"�뷡", 1264, 208 + 1424, 15, 40, 500, L"�Ҿ߳�8", RGB(255, 255, 255), false},
						{ L"�׽���", 1248, 496 + 1424, 15, 40, 500, L"�Ҿ߳�8", RGB(255, 255, 255), false},
						{ L"���", 1264, 784 + 1424, 15, 40, 500, L"�Ҿ߳�8", RGB(255, 255, 255), false},
						{ L"1401 �谭��", 1116, 80 + 1424, 24, 64, 500, L"�Ҿ߳�8", RGB(255, 255, 255), false},
						{ L"�͸��� ����", 1128, 272 + 1424, 24, 64, 500, L"�Ҿ߳�8", RGB(255, 255, 255), false},
						{ L"C00MP3K", 1160, 368 + 1424, 24, 64, 500, L"�Ҿ߳�8", RGB(255, 255, 255), false},
						{ L"1421 ������", 1128, 560 + 1424, 24, 64, 500, L"�Ҿ߳�8", RGB(255, 255, 255), false},
						{ L"�� 1-4 �л���", 1088, 656 + 1424, 24, 64, 500, L"�Ҿ߳�8", RGB(255, 255, 255), false},
						{ L"��8��5��", 1176, 848 + 1424, 24, 64, 500, L"�Ҿ߳�8", RGB(255, 255, 255), false},
						{ L"����������21���ϱ�", 1008, 944 + 1424, 24, 64, 500, L"�Ҿ߳�8", RGB(255, 255, 255), false},
						{ L"�������������������8���ϱ�", 864, 1040 + 1424, 24, 64, 500, L"�Ҿ߳�8", RGB(255, 255, 255), false},
						{ L"���Ͻ�2", 1184, 1136 + 1424, 24, 64, 500, L"�Ҿ߳�8", RGB(255, 255, 255), false},
						{ L"����åȰ��2", 1128, 1232 + 1424, 24, 64, 500, L"�Ҿ߳�8", RGB(255, 255, 255), false},
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