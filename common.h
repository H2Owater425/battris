#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <mmsystem.h>
#include "windows.h"
#include "ManyLayer/ManyLayer.h"
#include "resource.h"
#include "define.h"

#pragma comment(lib, "winmm.lib")

#ifndef UTILITY_H_INCLUDED
#define UTILITY_H_INCLUDED

// 블록 비트맵을 로드해온다
inline void initializeBlockBitmapHandles(HBITMAP* blockBitmapHandles) {
	int index = 0;

	blockBitmapHandles[index++] = DEFAULT_MANY_LAYER.getBitmapHandleFromResource(block0Bitmap);
	blockBitmapHandles[index++] = DEFAULT_MANY_LAYER.getBitmapHandleFromResource(block1Bitmap);
	blockBitmapHandles[index++] = DEFAULT_MANY_LAYER.getBitmapHandleFromResource(block2Bitmap);
	blockBitmapHandles[index++] = DEFAULT_MANY_LAYER.getBitmapHandleFromResource(block3Bitmap);
	blockBitmapHandles[index++] = DEFAULT_MANY_LAYER.getBitmapHandleFromResource(block4Bitmap);
	blockBitmapHandles[index++] = DEFAULT_MANY_LAYER.getBitmapHandleFromResource(block5Bitmap);
	blockBitmapHandles[index++] = DEFAULT_MANY_LAYER.getBitmapHandleFromResource(block6Bitmap);
	blockBitmapHandles[index++] = DEFAULT_MANY_LAYER.getBitmapHandleFromResource(blockGarbageBitmap);
	blockBitmapHandles[index++] = DEFAULT_MANY_LAYER.getBitmapHandleFromResource(blockBackgroundBitmap);
	blockBitmapHandles[index++] = DEFAULT_MANY_LAYER.getBitmapHandleFromResource(blockMapBitmap);
}

// 테트로미노 비트맵을 로드해온다
inline void initializeTetrominoBitmapHandles(HBITMAP* tetrominoBitmapHandles) {
	int index = 0;

	tetrominoBitmapHandles[index++] = DEFAULT_MANY_LAYER.getBitmapHandleFromResource(tetromino0Bitmap);
	tetrominoBitmapHandles[index++] = DEFAULT_MANY_LAYER.getBitmapHandleFromResource(tetromino1Bitmap);
	tetrominoBitmapHandles[index++] = DEFAULT_MANY_LAYER.getBitmapHandleFromResource(tetromino2Bitmap);
	tetrominoBitmapHandles[index++] = DEFAULT_MANY_LAYER.getBitmapHandleFromResource(tetromino3Bitmap);
	tetrominoBitmapHandles[index++] = DEFAULT_MANY_LAYER.getBitmapHandleFromResource(tetromino4Bitmap);
	tetrominoBitmapHandles[index++] = DEFAULT_MANY_LAYER.getBitmapHandleFromResource(tetromino5Bitmap);
	tetrominoBitmapHandles[index++] = DEFAULT_MANY_LAYER.getBitmapHandleFromResource(tetromino6Bitmap);
}

// 기록을 저장한다
inline void updateRecord(wchar_t* content) {
	FILE* recordFile = _wfsopen(L"record.txt", L"a", SH_DENYWR);

	fputws(content, recordFile);

	fclose(recordFile);
}

// 소리를 재생한다
inline void playSound(int soundName, bool isLoop) {
	PlaySoundW(soundName, GetModuleHandleW(NULL), SND_RESOURCE | SND_ASYNC | (isLoop ? SND_LOOP : 0));
}

// 소리를 멈춘다
inline void stopSound(void) {
	PlaySoundW(NULL, NULL, NULL);
}

// 맵을 표시한다
inline void updateMapBuffer(ManyLayer* manyLayer, int(*mapBuffer)[MAP_WIDTH]) {
	for(int i = 0; i < MAP_HEIGHT; i++) {
		for(int j = 0; j < MAP_WIDTH; j++) {
			manyLayer->images[j + i * 12].bitmapHandle = blockBitmapHandles[mapBuffer[i][j] < 7 ? mapBuffer[i][j] : mapBuffer[i][j] - 7];
		}
	}
}

// 다음 블럭들을 표시한다
inline void updateNextShapes(ManyLayer* manyLayer, int* nextShapes) {
	for(int i = 0; i < 5; i++) {
		manyLayer->images[MAP_WIDTH * MAP_HEIGHT + i].bitmapHandle = tetrominoBItmapHandles[nextShapes[i]];
	}

	manyLayer->renderAll(manyLayer);
}

// 점수를 표시한다
inline void updateScore(ManyLayer* manyLayer, int score) {
	wsprintfW(manyLayer->texts[0].content, L"%06d", score);

	manyLayer->renderAll(manyLayer);
}

// 콤보를 표시한다
inline void updateCombo(ManyLayer* manyLayer, int combo, bool isTetris) {
	if(combo == 0) {
		manyLayer->texts[1].isHidden = true;
		manyLayer->texts[4].isHidden = true;
	} else {
		manyLayer->texts[1].isHidden = false;
		manyLayer->texts[4].isHidden = false;
	}

	wsprintfW(manyLayer->texts[1].content, L"%d", combo);

	manyLayer->renderAll(manyLayer);
}

// 홀드를 표시한다
inline void updateHolding(ManyLayer* manyLayer, int holdingShape) {
	bool isHidden = holdingShape == 8;

	manyLayer->images[MAP_WIDTH * MAP_HEIGHT + 5].isHidden = isHidden;

	if(!isHidden) {
		manyLayer->images[MAP_WIDTH * MAP_HEIGHT + 5].bitmapHandle = tetrominoBItmapHandles[holdingShape];
	}

	manyLayer->renderAll(manyLayer);
}

// 게임 종료 여부를 확인한다
inline bool isGameEnded(int(*mapBuffer)[MAP_WIDTH]) {
	bool _isGameEnd = false;

	for(int i = 1; i < 3; i++) {
		for(int j = 1; j < MAP_WIDTH - 2; j++) {
			if(mapBuffer[i][j] != 15) {
				_isGameEnd = true;

				break;
			}
		}
	}

	return _isGameEnd;
}

// 테트로미노를 삭제한다
inline void removeTetromino(int(*mapBuffer)[MAP_WIDTH], int shape, int rotation, int x, int y) {
	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 4; j++) {
			if(tetrominos[shape][rotation][i][j] && mapBuffer[y + i][x + j] != 16) {
				mapBuffer[y + i][x + j] = 15;
			}
		}
	}
}

// 충돌 여부를 확인한다
inline bool isCollided(int(*mapBuffer)[MAP_WIDTH], int shape, int rotation, int x, int y) {
	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 4; j++) {
			if(tetrominos[shape][rotation][i][j] & (7 <= mapBuffer[y + i][x + j] && mapBuffer[y + i][x + j] <= 14 || mapBuffer[y + i][x + j] == 16)) {
				return true;
			}
		}
	}

	return false;
}

// 테트로미노를 움직이고 표시한다
inline void updateTetromino(ManyLayer* manyLayer, int(*mapBuffer)[MAP_WIDTH], int shape, int* rotation, int* x, int* y, int direction) {
	removeTetromino(mapBuffer, shape, *rotation, *x, *y);

	switch(direction) {
		case LEFT: {
			(*x)--;

			break;
		}
		case RIGHT: {
			(*x)++;

			break;
		}
		case DOWN: {
			(*y)++;

			break;
		}
		case ROTATE_CLOCKWISE: {
			*rotation = (*rotation + 1) % 4;

			break;
		}
		case ROTATE_COUNTERCLOCKWISE: {
			*rotation = (*rotation + 3) % 4;

			break;
		}
	}

	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 4; j++) {
			if(tetrominos[shape][*rotation][i][j]) {
				mapBuffer[*y + i][*x + j]
					= shape;
			}
		}
	}

	updateMapBuffer(manyLayer, mapBuffer);

	manyLayer->renderAll(manyLayer);
}

// 테트로미노를 비활성 블럭으로 바꾸고 표시한다
inline void inactivateTetromino(ManyLayer* manyLayer, int(*mapBuffer)[MAP_WIDTH], int shape, int rotation, int x, int y) {
	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 4; j++) {
			if(tetrominos[shape][rotation][i][j]) {
				mapBuffer[y + i][x + j] = shape + 7;
			}
		}
	}

	updateMapBuffer(manyLayer, mapBuffer);

	manyLayer->renderAll(manyLayer);
}

// 꽉 찬 줄을 없애고 표시한다
inline void removeFullLine(ManyLayer* manyLayer, int(*mapBuffer)[MAP_WIDTH], int shape, int* score, int* combo) {
	int fullLineCount = 0;

	for(int i = 2; i < MAP_HEIGHT - 1; i++) {
		bool isLineFull = true;

		for(int j = 1; j < MAP_WIDTH - 1; j++) {
			if(mapBuffer[i][j] == 15) {
				isLineFull = false;

				break;
			}
		}

		if(isLineFull) {
			fullLineCount++;

			for(int j = i; j > 1; j--) {
				for(int k = 1; k < MAP_WIDTH - 1; k++) {
					mapBuffer[j][k] = mapBuffer[j - 1][k];
				}
			}

			i--;
		}
	}

	if(fullLineCount != 0) {
		*combo += fullLineCount;
	} else {
		*combo = 0;
	}

	*score += fullLineCount * (10 + (*combo - 1) * 2);

	updateCombo(manyLayer, *combo, fullLineCount != 4);

	updateScore(manyLayer, *score);

	updateMapBuffer(manyLayer, mapBuffer);

	manyLayer->renderAll(manyLayer);
}

// 콘솔 커서 위치를 바꾼다
inline void setCoordinate(int x, int y) {
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), (COORD){ x, y });
}

// 오류를 출력한다
inline void throwError(ManyLayer* manyLayer, const char* message) {
	system("cls && color 04");

	Sleep(COMMAND_DELAY);
	
	setCoordinate(72, 21);

	puts("※ Error ※");


	setCoordinate((int)(80 - strlen(message) / 1.8), 22);

	puts(message);

	_getch();

	exit(1);
}

// 타이머를 표시한다
inline void startStartingTimer(ManyLayer* manyLayer) {
	manyLayer->images = NULL;

	manyLayer->imageCount = 0;

	manyLayer->texts = (Text[]){
		{ L"Game starts in", 848, 576, 36, 96, 600, L"소야논8", RGB(255, 255, 255), false },
		{ malloc(sizeof(wchar_t) * 5), 1200, 736, 20, 50, 600, L"소야논8", RGB(255, 255, 255), false},
	};

	manyLayer->textCount = 2;

	for(int i = 3; i != 0; i--) {
		wsprintfW(manyLayer->texts[1].content, L" %d", i);

		manyLayer->renderAll(manyLayer);

		Sleep(480);
	}

	wsprintfW(manyLayer->texts[1].content, L"GO!");

	manyLayer->renderAll(manyLayer);

	Sleep(320);

	free(manyLayer->texts[1].content);
}

// sevenBag 시스템을 초기화한다
inline int initializeSevenBag(int* sevenBag) {
	for(int i = 0; i < 7; i++) {
		sevenBag[i] = i;
	}

	int temporary1, temporary2;

	for(int i = 0; i < 7; i++) {
		temporary1 = rand() % 7;
		temporary2 = sevenBag[temporary1];
		sevenBag[temporary1] = sevenBag[i];
		sevenBag[i] = temporary2;
	}
}

// sevenBag에서 다음 블럭을 가져온다
inline int getSevenBagShape(int* sevenBag) {
	int index = 0, temporary;

	while(index < 8 && sevenBag[index] == -1) {
		index++;
	}

	if(index == 7) {
		initializeSevenBag(sevenBag);

		return getSevenBagShape(sevenBag);
	} else {
		temporary = sevenBag[index];

		sevenBag[index] = -1;

		return temporary;
	}
}

#endif