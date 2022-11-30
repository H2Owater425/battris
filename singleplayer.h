#pragma once

#include <stdbool.h>
#include <conio.h>
#include <time.h>
#include <windows.h>
#include <wchar.h>
#include "ManyLayer/ManyLayer.h"
#include "common.h"
#include "define.h"

#ifndef SINGLEPLAYER_H_INCLUDED
#define SINGLEPLAYER_H_INCLUDED

// 싱글플레이어 게임을 진행하고 표시한다
// 매인 루프를 돌리며 키 입력을 확인하고 종료 여부를 확인한다
inline void startGameSingleplayer(ManyLayer* manyLayer) {
	initializeBlockBitmapHandles(blockBitmapHandles);
	initializeTetrominoBitmapHandles(tetrominoBItmapHandles);

	stopSound();

	startStartingTimer(manyLayer);

	playSound(singleplayerGameWave, true);

	Image images[MAP_WIDTH * MAP_HEIGHT + 6];

	int mapBuffer[MAP_HEIGHT][MAP_WIDTH];

	for(int i = 0; i < MAP_HEIGHT; i++) {
		for(int j = 0; j < MAP_WIDTH; j++) {
			mapBuffer[i][j] = i == 0 || i == MAP_HEIGHT - 1 || j == 0 || j == MAP_WIDTH - 1 ? 16 : 15;
			images[i * 12 + j] = (Image){ NULL, 928 + 64 * j, 16 + 64 * i, 4, false };
		}
	}

	for(int i = 0; i < 5; i++) {
		images[MAP_WIDTH * MAP_HEIGHT + i] = (Image){ NULL, 1728, 128 + 160 * i, 3, false };
	}

	images[MAP_WIDTH * MAP_HEIGHT + 5] = (Image){ NULL, 708, 128, 3, true };

	manyLayer->images = images;

	manyLayer->imageCount = sizeof(images) / sizeof(Image);

	updateMapBuffer(manyLayer, mapBuffer);

	manyLayer->texts = (Text[]){
		{ malloc(sizeof(wchar_t) * 7), 1482, 1356, 20, 50, 500, L"소야논8", RGB(255, 255, 255), false },
		{ malloc(sizeof(wchar_t) * 7), 760, 768, 20, 50, 500, L"소야논8", RGB(255, 255, 255), true },
		{ L"Hold", 720, 64, 26, 65, 600, L"소야논8", RGB(255, 255, 255), false },
		{ L"Next", 1728, 64, 26, 65, 600, L"소야논8", RGB(255, 255, 255), false },
		{ L"Combo", 656, 672, 26, 65, 600, L"소야논8", RGB(255, 255, 255), true },
		{ L"Localhost", 936, 1356, 20, 50, 500, L"소야논8", RGB(255, 255, 255), false },
	};

	manyLayer->textCount = 6;

	int x = MAP_WIDTH / 2 - 2, y = 0, rotation = 0, holdingShape = 8, score = 0, combo = 0, checkingTime = 41, sevenBag[7];
	long long int _time = 0, lastCheckTime = 0;
	bool _isGameEnded = false, _isCollided = false, isHoldingShape = false;

	initializeSevenBag(sevenBag);

	int shape = getSevenBagShape(sevenBag), nextShapes[5] = { getSevenBagShape(sevenBag), getSevenBagShape(sevenBag), getSevenBagShape(sevenBag), getSevenBagShape(sevenBag), getSevenBagShape(sevenBag) };

	updateScore(manyLayer, score);
	updateCombo(manyLayer, combo, false);
	updateNextShapes(manyLayer, nextShapes);
	updateTetromino(manyLayer, mapBuffer, shape, &rotation, &x, &y, NULL);

	while(!_isGameEnded) {
		if(_time % checkingTime == 0) {
			_isCollided = isCollided(mapBuffer, shape, rotation, x, y + 1);

			if(!_isCollided) {
				updateTetromino(manyLayer, mapBuffer, shape, &rotation, &x, &y, DOWN);
			} else {
				lastCheckTime -= 20;
			}
		}

		if(_time % 10000 == 0) {
			if(checkingTime > 1) {
				checkingTime--;
			}
		}

		if(_kbhit()) {
			switch(_getch()) {
				case LEFT: {
					if(!isCollided(mapBuffer, shape, rotation, x - 1, y)) {
						updateTetromino(manyLayer, mapBuffer, shape, &rotation, &x, &y, LEFT);
						lastCheckTime = _time + 20;
					}

					break;
				}
				case RIGHT: {
					if(!isCollided(mapBuffer, shape, rotation, x + 1, y)) {
						updateTetromino(manyLayer, mapBuffer, shape, &rotation, &x, &y, RIGHT);
						lastCheckTime = _time + 20;
					}

					break;
				}
				case DOWN: {
					if(!isCollided(mapBuffer, shape, rotation, x, y + 1)) {
						updateTetromino(manyLayer, mapBuffer, shape, &rotation, &x, &y, DOWN);
						lastCheckTime = _time + 20;
					}


					break;
				}
				case 72:
				case ROTATE_CLOCKWISE: {
					if(!isCollided(mapBuffer, shape, (rotation + 1) % 4, x, y)) {
						updateTetromino(manyLayer, mapBuffer, shape, &rotation, &x, &y, ROTATE_CLOCKWISE);
						lastCheckTime = _time + 20;
					}

					break;
				}
				case ROTATE_COUNTERCLOCKWISE: {
					if(!isCollided(mapBuffer, shape, (rotation + 3) % 4, x, y)) {
						updateTetromino(manyLayer, mapBuffer, shape, &rotation, &x, &y, ROTATE_COUNTERCLOCKWISE);
						lastCheckTime = _time + 20;
					}

					break;
				}
				case SPACE: {
					int downCount = 0;

					while(!isCollided(mapBuffer, shape, rotation, x, y + downCount + 1)) {
						downCount++;
					}

					removeTetromino(mapBuffer, shape, rotation, x, y);

					y += downCount - 1;

					updateTetromino(manyLayer, mapBuffer, shape, &rotation, &x, &y, DOWN);

					inactivateTetromino(manyLayer, mapBuffer, shape, rotation, x, y);

					_isCollided = true;
					lastCheckTime = _time - 20;

					break;
				}
				case HOLD: {
					if(!isHoldingShape) {
						removeTetromino(mapBuffer, shape, rotation, x, y);

						if(holdingShape == 8) {
							holdingShape = shape;
							shape = nextShapes[0];

							for(int i = 0; i < 4; i++) {
								nextShapes[i] = nextShapes[i + 1];
							}

							nextShapes[4] = getSevenBagShape(sevenBag);

							updateNextShapes(manyLayer, nextShapes);
						} else {
							int temporaryShape = shape;

							shape = holdingShape;
							holdingShape = temporaryShape;
						}

						isHoldingShape = true;

						x = MAP_WIDTH / 2 - 2;
						y = 0;
						rotation = 0;

						updateHolding(manyLayer, holdingShape);

						updateTetromino(manyLayer, mapBuffer, shape, &rotation, &x, &y, NULL);

						continue;
					}

					break;
				}
				case EXIT: {
					_isGameEnded = true;

					break;
				}
			}
		}

		if(_isCollided && lastCheckTime <= _time - 20) {
			_isCollided = false;
			lastCheckTime = _time;

			if(!isGameEnded(mapBuffer)) {
				inactivateTetromino(manyLayer, mapBuffer, shape, rotation, x, y);
				removeFullLine(manyLayer, mapBuffer, shape, &score, &combo);

				x = MAP_WIDTH / 2 - 2;
				y = 0;
				rotation = 0;
				shape = nextShapes[0];

				for(int i = 0; i < 4; i++) {
					nextShapes[i] = nextShapes[i + 1];
				}

				nextShapes[4] = getSevenBagShape(sevenBag);
				isHoldingShape = false;

				updateNextShapes(manyLayer, nextShapes);

				updateTetromino(manyLayer, mapBuffer, shape, &rotation, &x, &y, NULL);
			} else {
				_isGameEnded = true;
			}
		}

		if(score > 999999) {
			_isGameEnded = true;
		}
		
		Sleep(10);
		_time++;
	}

	free(manyLayer->texts[1].content);

	for(int i = 0; i < 10; i++) {
		DeleteObject(blockBitmapHandles[i]);
	}
	for(int i = 0; i < 7; i++) {
		DeleteObject(tetrominoBItmapHandles[i]);
	}

	wchar_t content[31];

	wsprintfW(content, L"Singleplayer Localhost %06ld\n", score);

	updateRecord(content);

	manyLayer->texts = (Text[]){
		{ manyLayer->texts[0].content, 1180, 896, 20, 50, 500, L"소야논8", RGB(255, 255, 255), false},
		{ L"Game over", 992, 416, 36, 96, 600, L"소야논8", RGB(255, 255, 255), false },
		{ L"Score", 1216, 838, 15, 40, 500, L"소야논8", RGB(255, 255, 255), false}
	};

	manyLayer->textCount = 3;

	manyLayer->images = NULL;

	manyLayer->imageCount = 0;

	manyLayer->renderAll(manyLayer);

	free(manyLayer->texts[0].content);

	stopSound();

	playSound(singleplayerGameoverAndMultiplayerLoseWave, false);

	Sleep(5000);

	_getch();

	playSound(mainWave, true);
}

#endif