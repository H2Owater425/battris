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

inline void updateRecord(wchar_t* content) {
	FILE* recordFile = _wfsopen(L"record.txt", L"a", SH_DENYWR);

	fputws(content, recordFile);

	fclose(recordFile);
}

inline void playSound(int soundName, bool isLoop) {
	PlaySoundW(soundName, GetModuleHandleW(NULL), SND_RESOURCE | SND_ASYNC | (isLoop ? SND_LOOP : 0));
}

inline void stopSound(void) {
	PlaySoundW(NULL, NULL, NULL);
}

inline void updateMapBuffer(ManyLayer* manyLayer, int(*mapBuffer)[MAP_WIDTH]) {
	for(int i = 0; i < MAP_HEIGHT; i++) {
		for(int j = 0; j < MAP_WIDTH; j++) {
			manyLayer->images[j + i * 12].bitmapHandle = blockBitmapHandles[mapBuffer[i][j] < 7 ? mapBuffer[i][j] : mapBuffer[i][j] - 7];
		}
	}
}

inline void updateNextShapes(ManyLayer* manyLayer, int* nextShapes) {
	for(int i = 0; i < 5; i++) {
		manyLayer->images[MAP_WIDTH * MAP_HEIGHT + i].bitmapHandle = tetrominoBItmapHandles[nextShapes[i]];
	}

	manyLayer->renderAll(manyLayer);
}

inline void updateScore(ManyLayer* manyLayer, int score) {
	wsprintfW(manyLayer->texts[0].content, L"%06d", score);

	manyLayer->renderAll(manyLayer);
}


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

inline void updateHolding(ManyLayer* manyLayer, int holdingShape) {
	bool isHidden = holdingShape == 8;

	manyLayer->images[MAP_WIDTH * MAP_HEIGHT + 5].isHidden = isHidden;

	if(!isHidden) {
		manyLayer->images[MAP_WIDTH * MAP_HEIGHT + 5].bitmapHandle = tetrominoBItmapHandles[holdingShape];
	}

	manyLayer->renderAll(manyLayer);
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

	for(int i = 3; i != 0; i--) {
		wsprintfW(manyLayer->texts[1].content, L" %d", i);

		manyLayer->renderAll(manyLayer);

		Sleep(640);
	}

	wsprintfW(manyLayer->texts[1].content, L"GO!");

	manyLayer->renderAll(manyLayer);

	Sleep(480);

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