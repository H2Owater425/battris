#pragma once

#include <stdbool.h>
#include <conio.h>
#include <time.h>
#include <windows.h>
#include <wchar.h>
#include "ManyLayer/ManyLayer.h"
#include "utility.h"
#include "define.h"

#ifndef SINGLEPLAYER_H_INCLUDED
#define SINGLEPLAYER_H_INCLUDED

const int const imageIndexOffsetSinglePlayer = MAP_WIDTH * MAP_HEIGHT;

inline void updateMapBufferSingleplayer(ManyLayer* manyLayer, int(*mapBuffer)[MAP_WIDTH]) {
	for(int i = 0; i < MAP_HEIGHT; i++) {
		for(int j = 0; j < MAP_WIDTH; j++) {
			manyLayer->images[j + i * 12].bitmapHandle = blockBitmapHandles[mapBuffer[i][j] < 7 ? mapBuffer[i][j] : mapBuffer[i][j] - 7];
		}
	}
}

inline void updateNextShapesSingleplayer(ManyLayer* manyLayer, int* nextShapes) {
	for(int i = 0; i < 5; i++) {
		manyLayer->images[imageIndexOffsetSinglePlayer + i].bitmapHandle = tetrominoBItmapHandles[nextShapes[i]];
	}

	manyLayer->renderAll(manyLayer);
}

inline void updateScoreSingleplayer(ManyLayer* manyLayer, int score) {
	wsprintfW(manyLayer->texts[0].content, L"%06ld", score);

	manyLayer->renderAll(manyLayer);
}

inline void updateComboSingleplayer(ManyLayer* manyLayer, int combo) {
	manyLayer->texts[1].isHidden = combo == 0;
	manyLayer->texts[4].isHidden = manyLayer->texts[1].isHidden;
	wsprintfW(manyLayer->texts[1].content, L"%ld", combo);

	manyLayer->renderAll(manyLayer);
}

inline void updateHoldingSingleplayer(ManyLayer* manyLayer, int holdingShape) {
	bool isHidden = holdingShape == 8;

	manyLayer->images[imageIndexOffsetSinglePlayer + 5].isHidden = isHidden;

	if(!isHidden) {
		manyLayer->images[imageIndexOffsetSinglePlayer + 5].bitmapHandle = tetrominoBItmapHandles[holdingShape];
	}

	manyLayer->renderAll(manyLayer);
}

inline void updateTetrominoSingleplayer(ManyLayer* manyLayer, int(*mapBuffer)[MAP_WIDTH], int shape, int* rotation, int* x, int* y, int direction) {
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

	updateMapBufferSingleplayer(manyLayer, mapBuffer);

	manyLayer->renderAll(manyLayer);
}

inline void inactivateTetrominoSingleplayer(ManyLayer* manyLayer, int(*mapBuffer)[MAP_WIDTH], int shape, int rotation, int x, int y) {
	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 4; j++) {
			if(tetrominos[shape][rotation][i][j]) {
				mapBuffer[y + i][x + j] = shape + 7;
			}
		}
	}

	updateMapBufferSingleplayer(manyLayer, mapBuffer);

	manyLayer->renderAll(manyLayer);
}

inline void removeFullLineSingleplayer(ManyLayer* manyLayer, int(*mapBuffer)[MAP_WIDTH], int shape, int* score, int* combo) {
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

	updateComboSingleplayer(manyLayer, *combo);

	updateScoreSingleplayer(manyLayer, *score);

	updateMapBufferSingleplayer(manyLayer, mapBuffer);

	manyLayer->renderAll(manyLayer);
}

inline void startGameSingleplayer(ManyLayer* manyLayer) {
	initializeBlockBitmapHandles(blockBitmapHandles);
	initializeTetrominoBitmapHandles(tetrominoBItmapHandles);

	PlaySoundW(NULL, NULL, NULL);

	startStartingTimer(manyLayer);

	PlaySoundW(L"sounds/singleplayerGame_fix.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);

	Image images[MAP_WIDTH * MAP_HEIGHT + 6];

	int mapBuffer[MAP_HEIGHT][MAP_WIDTH];

	for(int i = 0; i < MAP_HEIGHT; i++) {
		for(int j = 0; j < MAP_WIDTH; j++) {
			mapBuffer[i][j] = i == 0 || i == MAP_HEIGHT - 1 || j == 0 || j == MAP_WIDTH - 1 ? 16 : 15;
			images[i * 12 + j] = (Image){ NULL, 928 + 64 * j, 16 + 64 * i, 4, false };
		}
	}

	for(int i = 0; i < 5; i++) {
		images[imageIndexOffsetSinglePlayer + i] = (Image){ NULL, 1728, 128 + 160 * i, 3, false };
	}

	images[imageIndexOffsetSinglePlayer + 5] = (Image){ NULL, 708, 128, 3, true };

	manyLayer->images = images;

	manyLayer->imageCount = sizeof(images) / sizeof(Image);

	updateMapBufferSingleplayer(manyLayer, mapBuffer);

	manyLayer->texts = (Text[]){
		{ malloc(sizeof(wchar_t) * 7), 1482, 1356, 20, 50, 500, L"家具稠8", RGB(255, 255, 255), false },
		{ malloc(sizeof(wchar_t) * 7), 760, 768, 20, 50, 500, L"家具稠8", RGB(255, 255, 255), true },
		{ L"Hold", 720, 64, 26, 65, 600, L"家具稠8", RGB(255, 255, 255), false },
		{ L"Next", 1728, 64, 26, 65, 600, L"家具稠8", RGB(255, 255, 255), false },
		{ L"Combo", 656, 672, 26, 65, 600, L"家具稠8", RGB(255, 255, 255), true },
		{ L"Localhost", 936, 1356, 20, 50, 500, L"家具稠8", RGB(255, 255, 255), false },
	};

	manyLayer->textCount = 6;

	int x = MAP_WIDTH / 2 - 2, y = 0, rotation = 0, shape = rand() % 7, nextShapes[5] = { rand() % 7, rand() % 7, rand() % 7, rand() % 7, rand() % 7 }, holdingShape = 8, score = 0, combo = 0, checkingTime = 41;
	long long int _time = 0, lastCheckTime = 0;
	bool _isGameEnded = false, _isCollided = false, isHoldingShape = false;

	updateScoreSingleplayer(manyLayer, score);
	updateComboSingleplayer(manyLayer, combo);
	updateNextShapesSingleplayer(manyLayer, nextShapes);
	updateTetrominoSingleplayer(manyLayer, mapBuffer, shape, &rotation, &x, &y, NULL);

	while(!_isGameEnded) {
		if(_time % checkingTime == 0) {
			_isCollided = isCollided(mapBuffer, shape, rotation, x, y + 1);

			if(!_isCollided) {
				updateTetrominoSingleplayer(manyLayer, mapBuffer, shape, &rotation, &x, &y, DOWN);
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
						updateTetrominoSingleplayer(manyLayer, mapBuffer, shape, &rotation, &x, &y, LEFT);
						lastCheckTime = _time + 20;
					}

					break;
				}
				case RIGHT: {
					if(!isCollided(mapBuffer, shape, rotation, x + 1, y)) {
						updateTetrominoSingleplayer(manyLayer, mapBuffer, shape, &rotation, &x, &y, RIGHT);
						lastCheckTime = _time + 20;
					}

					break;
				}
				case DOWN: {
					if(!isCollided(mapBuffer, shape, rotation, x, y + 1)) {
						updateTetrominoSingleplayer(manyLayer, mapBuffer, shape, &rotation, &x, &y, DOWN);
						lastCheckTime = _time + 20;
					}


					break;
				}
				case 72:
				case ROTATE_CLOCKWISE: {
					if(!isCollided(mapBuffer, shape, (rotation + 1) % 4, x, y)) {
						updateTetrominoSingleplayer(manyLayer, mapBuffer, shape, &rotation, &x, &y, ROTATE_CLOCKWISE);
						lastCheckTime = _time + 20;
					}

					break;
				}
				case ROTATE_COUNTERCLOCKWISE: {
					if(!isCollided(mapBuffer, shape, (rotation + 3) % 4, x, y)) {
						updateTetrominoSingleplayer(manyLayer, mapBuffer, shape, &rotation, &x, &y, ROTATE_COUNTERCLOCKWISE);
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

					updateTetrominoSingleplayer(manyLayer, mapBuffer, shape, &rotation, &x, &y, DOWN);

					inactivateTetrominoSingleplayer(manyLayer, mapBuffer, shape, rotation, x, y);

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

							nextShapes[4] = rand() % 7;

							updateNextShapesSingleplayer(manyLayer, nextShapes);
						} else {
							int temporaryShape = shape;

							shape = holdingShape;
							holdingShape = temporaryShape;
						}

						isHoldingShape = true;

						x = MAP_WIDTH / 2 - 2;
						y = 0;
						rotation = 0;

						updateHoldingSingleplayer(manyLayer, holdingShape);

						updateTetrominoSingleplayer(manyLayer, mapBuffer, shape, &rotation, &x, &y, NULL);

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
				inactivateTetrominoSingleplayer(manyLayer, mapBuffer, shape, rotation, x, y);
				removeFullLineSingleplayer(manyLayer, mapBuffer, shape, &score, &combo);

				x = MAP_WIDTH / 2 - 2;
				y = 0;
				rotation = 0;
				shape = nextShapes[0];

				for(int i = 0; i < 4; i++) {
					nextShapes[i] = nextShapes[i + 1];
				}

				nextShapes[4] = rand() % 7;
				isHoldingShape = false;

				// 咯扁辑 茄锅 send

				updateNextShapesSingleplayer(manyLayer, nextShapes);

				updateTetrominoSingleplayer(manyLayer, mapBuffer, shape, &rotation, &x, &y, NULL);
			} else {
				_isGameEnded = true;
			}
		}
		
		Sleep(10);
		_time++;
	}

	free(manyLayer->texts[0].content);
	free(manyLayer->texts[1].content);

	for(int i = 0; i < 10; i++) {
		DeleteObject(blockBitmapHandles[i]);
	}
	for(int i = 0; i < 7; i++) {
		DeleteObject(tetrominoBItmapHandles[i]);
	}

	char content[31];

	sprintf_s(content, sizeof(content), "Singleplayer Localhost %06ld\n", score);

	updateRecord(content);

	manyLayer->texts = (Text[]){
		{ malloc(sizeof(wchar_t) * 7), 1180, 896, 20, 50, 500, L"家具稠8", RGB(255, 255, 255), false},
		{ L"Game over", 992, 416, 36, 96, 600, L"家具稠8", RGB(255, 255, 255), false },
		{ L"Score", 1216, 838, 15, 40, 500, L"家具稠8", RGB(255, 255, 255), false}
	};

	wsprintfW(manyLayer->texts[0].content, L"%06ld", score);

	manyLayer->textCount = 3;

	manyLayer->images = NULL;

	manyLayer->imageCount = 0;

	manyLayer->renderAll(manyLayer);
	PlaySoundW(NULL, NULL, NULL);
	PlaySoundW(L"sounds/singleplayerGameoverAndMultiplayerLose.wav", NULL, SND_FILENAME | SND_ASYNC);

	Sleep(5000);

	_getch();

	PlaySoundW(L"sounds/main_fix.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
}

#endif