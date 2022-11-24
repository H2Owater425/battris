#pragma once

#include "windows.h"
#include "ManyLayer/ManyLayer.h"
#include "resource.h"

#ifndef UTILITY_H_INCLUDED
#define UTILITY_H_INCLUDED

inline void initializeBitmapHandles(ManyLayer* manyLayer, HBITMAP* mapBitmapHandle, HBITMAP* blockBitmapHandles) {
	*mapBitmapHandle = manyLayer->getBitmapHandleFromResource(mapBitmap);
	
	int index = 0;
	
	blockBitmapHandles[index++] = manyLayer->getBitmapHandleFromResource(block0Bitmap);
	blockBitmapHandles[index++] = manyLayer->getBitmapHandleFromResource(block1Bitmap);
	blockBitmapHandles[index++] = manyLayer->getBitmapHandleFromResource(block2Bitmap);
	blockBitmapHandles[index++] = manyLayer->getBitmapHandleFromResource(block3Bitmap);
	blockBitmapHandles[index++] = manyLayer->getBitmapHandleFromResource(block4Bitmap);
	blockBitmapHandles[index++] = manyLayer->getBitmapHandleFromResource(block5Bitmap);
	blockBitmapHandles[index++] = manyLayer->getBitmapHandleFromResource(block6Bitmap);
	blockBitmapHandles[index++] = manyLayer->getBitmapHandleFromResource(blockGarbageBitmap);
	blockBitmapHandles[index++] = manyLayer->getBitmapHandleFromResource(blockBackgroundBitmap);
}

#endif