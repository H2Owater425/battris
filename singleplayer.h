#pragma once

#include <conio.h>
#include "ManyLayer/ManyLayer.h"
#include "utility.h"
#include "define.h"

#ifndef SINGLEPLAYER_H_INCLUDED
#define SINGLEPLAYER_H_INCLUDED

HBITMAP mapBitmapHandle, blockBitmapHandles[9];

inline void applyMap(int (*mapBuffer)[MAP_WIDTH], ManyLayer* manyLayer) {
	for(int y = 0; y < MAP_HEIGHT; y++) {
		for(int x = 0; x < MAP_WIDTH; x++) {
			manyLayer->images[x + y * 10 + 1].bitmapHandle = blockBitmapHandles[mapBuffer[y][x] < 7 ? mapBuffer[y][x] : mapBuffer[y][x] - 7];
		}
	}
}

inline void startSinleplayerGame(ManyLayer* manyLayer) {
	initializeBitmapHandles(manyLayer, &mapBitmapHandle, blockBitmapHandles);

	Image images[MAP_WIDTH * MAP_HEIGHT + 1];

	images[0] = (Image){ mapBitmapHandle, 928, 16, 4, false };

	int mapBuffer[MAP_HEIGHT][MAP_WIDTH];

	for(int y = 0; y < MAP_HEIGHT; y++) {
		for(int x = 0; x < MAP_WIDTH; x++) {
			mapBuffer[y][x] = 15;
			images[x + y * 10 + 1] = (Image){ blockBitmapHandles[8], 992 + 64 * x, 80 + 64 * y, 4, false};
		}
	}

	manyLayer->images = images;

	manyLayer->imageCount = 201;

	manyLayer->renderAll(manyLayer);


	/*typedef struct {
		wchar_t* content;
		int x, y, width, height, weight;
		wchar_t* font;
		COLORREF color;
		bool isHidden;
	} Text;*/

	manyLayer->texts = (Text[]){
		{ L"000000", 1472, 1356, 20, 50, 600, L"家具稠8", RGB(255, 255, 255), false },
		{ L"Next", 1200, 64, 20, 50, 600, L"家具稠8", RGB(255, 255, 255), false },
		{ L"localhost", 936, 1356, 20, 50, 600, L"家具稠8", RGB(255, 255, 255), false },
	};

	manyLayer->textCount = 3;

	manyLayer->renderAll(manyLayer);

	_getch();
}

#endif