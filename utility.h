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

inline void updateRecord(char* content) {
	FILE* recordFile = _fsopen("./record.txt", "a", SH_DENYWR);

	fputs(content, recordFile);

	fclose(recordFile);
}

inline void updateRecordW(wchar_t* content) {
	FILE* recordFile = _wfsopen("./record.txt", "a", SH_DENYWR);

	fputws(content, recordFile);

	fclose(recordFile);
}

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

inline void removeTetromino(int(*mapBuffer)[MAP_WIDTH], int shape, int rotation, int x, int y) {
	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 4; j++) {
			if(tetrominos[shape][rotation][i][j] && mapBuffer[y + i][x + j] != 16) {
				mapBuffer[y + i][x + j] = 15;
			}
		}
	}
}

inline void setCoordinate(int x, int y) {
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), (COORD){ x, y });
}

inline void throwError(ManyLayer* manyLayer, const char* message) {
	system("cls && color 04");

	Sleep(COMMAND_DELAY);
	
	setCoordinate(72, 21);

	puts("∝ Error ∝");


	setCoordinate((int)(80 - strlen(message) / 1.8), 22);

	puts(message);

	_getch();

	exit(1);
}

inline void startStartingTimer(ManyLayer* manyLayer) {
	manyLayer->images = NULL;

	manyLayer->imageCount = 0;

	manyLayer->texts = (Text[]){
		{ L"Game starts in", 848, 576, 36, 96, 600, L"家具稠8", RGB(255, 255, 255), false },
		{ malloc(sizeof(wchar_t) * 5), 1200, 736, 20, 50, 600, L"家具稠8", RGB(255, 255, 255), false},
	};

	manyLayer->textCount = 2;

	for(int i = 5; i != 0; i--) {
		wsprintfW(manyLayer->texts[1].content, L" %d", i);

		manyLayer->renderAll(manyLayer);

		Sleep(1000);
	}

	wsprintfW(manyLayer->texts[1].content, L"GO!");

	manyLayer->renderAll(manyLayer);

	Sleep(640);

	free(manyLayer->texts[1].content);
}

inline int getIntegerLength(int a) {
	int r = 0;
	
	while(a != 0) {
		a /= 10;
		r++;
	}

	return r;
}

#endif