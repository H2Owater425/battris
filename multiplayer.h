#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include <conio.h>
#include <time.h>
#include <wchar.h>
#include <process.h>
#include <windows.h>
#include <winsock.h>
//#include <ws2tcpip.h>

#include "ManyLayer/ManyLayer.h"
#include "utility.h"
#include "define.h"

#pragma comment(lib, "ws2_32.lib")

#ifndef MULTIPLAYER_H_INCLUDED
#define MULTIPLAYER_H_INCLUDED

//
typedef struct {
	SOCKET _socket;
	int (*mapBuffer)[MAP_WIDTH];
	bool* _isGameEnded, * isWin;
} SendMapBufferThreadArgument;

//
typedef struct {
	SOCKET _socket;
	int score, combo;
	bool* _isGameEnded, *isWin;
} SendScoreAndComboThreadArgument;

//
typedef struct {
	SOCKET _socket;
	int* nextShapes;
	bool* _isGameEnded, * isWin;
} SendNextShapesThreadArgument;

//
typedef struct {
	SOCKET _socket;
	int holdingShape;
	bool* _isGameEnded, * isWin;
} SendHoldingShapeThreadArgument;

//
typedef struct {
	SOCKET _socket;
	int lineCount;
	bool* _isGameEnded, * isWin;
} SendAttackThreadArgument;

//
typedef struct {
	SOCKET _socket;
} SendEndingThreadArgument;

typedef struct {
	SOCKET _socket;
	ManyLayer* manyLayer;
	int (*mapBuffer)[MAP_WIDTH];
	bool* isAttacking, * _isGameEnded, * isWin;
} HandleReceivingThreadArgument;

const int const imageIndexOffsetMultiplayer = MAP_WIDTH * MAP_HEIGHT + 6, textIndexOffsetMultiplayer = 6;

inline void sendMapBufferThread(SendMapBufferThreadArgument* argument) {
	char buffer[404] = "MAP";
	int bufferIndex = 3;

	for(int i = 1; i < MAP_HEIGHT - 1; i++) {
		for(int j = 1; j < MAP_WIDTH - 1; j++) {
			sprintf_s(buffer + bufferIndex, sizeof(buffer) - bufferIndex, " %d", (argument->mapBuffer)[i][j] < 7 ? (argument->mapBuffer)[i][j] : (argument->mapBuffer)[i][j] - 7);

			bufferIndex += 2;
		}
	}

	if(send(argument->_socket, buffer, sizeof(buffer), 0) == WSAEWOULDBLOCK) {
		*(argument->_isGameEnded) = true;
		*(argument->isWin) = true;
	}


	_endthreadex(0);
}

inline void sendScoreAndComboThread(SendScoreAndComboThreadArgument* argument) {
	char buffer[18] = "SAC";

	sprintf_s(buffer + 3, sizeof(buffer) - 3, " %6d %6d", argument->score, argument->combo);

	if(send(argument->_socket, buffer, sizeof(buffer), 0) == WSAEWOULDBLOCK) {
		*(argument->_isGameEnded) = true;
		*(argument->isWin) = true;
	}

	_endthreadex(0);
}

inline void sendNextShapesThread(SendNextShapesThreadArgument* argument) {
	char buffer[14] = "NXS";
	int bufferIndex = 3;

	for(int i = 0; i < 5; i++) {
		sprintf_s(buffer + bufferIndex, sizeof(buffer) - bufferIndex, " %d", (argument->nextShapes)[i]);

		bufferIndex += 2;
	}

	if(send(argument->_socket, buffer, sizeof(buffer), 0) == WSAEWOULDBLOCK) {
		*(argument->_isGameEnded) = true;
		*(argument->isWin) = true;
	}

	_endthreadex(0);
}

inline void sendHoldingShapeThread(SendHoldingShapeThreadArgument* argument) {
	char buffer[6] = "HDS";

	sprintf_s(buffer + 3, sizeof(buffer) - 3, " %d", argument->holdingShape);

	if(send(argument->_socket, buffer, sizeof(buffer), 0) == WSAEWOULDBLOCK) {
		*(argument->_isGameEnded) = true;
		*(argument->isWin) = true;
	}

	_endthreadex(0);
}

inline void sendAttackThread(SendAttackThreadArgument* argument) {
	char buffer[6] = "ATK";

	sprintf_s(buffer + 3, sizeof(buffer) - 3, " %d", argument->lineCount);

	if(send(argument->_socket, buffer, sizeof(buffer), 0) == WSAEWOULDBLOCK) {
		*(argument->_isGameEnded) = true;
*(argument->isWin) = true;
	}


	_endthreadex(0);
}

inline void sendEndingThread(SendEndingThreadArgument* argument) {
	send(argument->_socket, "END", 4, 0);

	_endthreadex(0);
}

inline void handleReceivingThread(HandleReceivingThreadArgument* argument) {
	char buffer[512], action[4];
	int bufferIndex, temporary;

	action[3] = 0;

	while(!*(argument->_isGameEnded)) {
		bufferIndex = 3;

		if(recv(argument->_socket, buffer, sizeof(buffer), 0) == WSAEWOULDBLOCK) {
			*(argument->_isGameEnded) = true;
			*(argument->isWin) = true;

			break;
		}

		strncpy_s(action, sizeof(action), buffer, 3);

		if(strcmp(action, "MAP")) {
			for(int i = 1; i < MAP_HEIGHT - 1; i++) {
				for(int j = 1; j < MAP_WIDTH - 1; j++) {
					sscanf_s(buffer + bufferIndex, "%d", &temporary);
					bufferIndex += 2;

					argument->manyLayer->images[j + i * 12 + imageIndexOffsetMultiplayer].bitmapHandle = blockBitmapHandles[temporary];
				}
			}

			argument->manyLayer->renderAll(argument->manyLayer);
		} else if(strcmp(action, "SAC")) {
			sscanf_s(buffer + bufferIndex, "%d", &temporary);
			bufferIndex += 2;

			wsprintfW(argument->manyLayer->texts[textIndexOffsetMultiplayer].content, L"%06ld", temporary);

			sscanf_s(buffer + bufferIndex, "%d", &temporary);

			wsprintfW(argument->manyLayer->texts[textIndexOffsetMultiplayer + 1].content, L"%06ld", temporary);

			argument->manyLayer->renderAll(argument->manyLayer);
		} else if(strcmp(action, "NXS")) {
			for(int i = 0; i < 5; i++) {
				sscanf_s(buffer + bufferIndex, "%d", &temporary);
				bufferIndex += 2;

				argument->manyLayer->images[imageIndexOffsetMultiplayer + imageIndexOffsetSinglePlayer + i].bitmapHandle = tetrominoBItmapHandles[temporary];
			}

			argument->manyLayer->renderAll(argument->manyLayer);
		} else if(strcmp(action, "HDS")) {
			sscanf_s(buffer + bufferIndex, "%d", &temporary);
			
			argument->manyLayer->images[imageIndexOffsetMultiplayer + imageIndexOffsetSinglePlayer + 5].bitmapHandle = tetrominoBItmapHandles[temporary];

			argument->manyLayer->renderAll(argument->manyLayer);
		} else if(strcmp(action, "ATK")) {
			*(argument->isAttacking) = true;

			sscanf_s(buffer + bufferIndex, "%d", &temporary);

			for(int j = MAP_HEIGHT - 1; j > temporary + 1; j--) {
				for(int k = 1; k < MAP_WIDTH - 1; k++) {
					argument->mapBuffer[j][k] = argument->mapBuffer[j - temporary][k];
				}
			}

			for(int j = MAP_HEIGHT - temporary - 1; j < temporary + 1; j++) {
				int removedIndex = rand() % 10 + 1;

				for(int k = 1; k < MAP_WIDTH - 1; k++) {
					argument->mapBuffer[j][k] = removedIndex == k ? 15 : 14;
				}
			}

			*(argument->isAttacking) = false;

			argument->manyLayer->renderAll(argument->manyLayer);
		} else if(strcmp(action, "END")) {
			*(argument->_isGameEnded) = true;
			*(argument->isWin) = true;
		}

		Sleep(10);
	}

	_endthreadex(0);
}

void startThread(void* function, void* argument) {
	CloseHandle((HANDLE)_beginthreadex(NULL, 0, function, argument, 0, NULL));
}

inline void updateTetrominoMultiplayer(ManyLayer* manyLayer, int(*mapBuffer)[MAP_WIDTH], int shape, int* rotation, int* x, int* y, int direction, SOCKET _socket, bool* _isGameEnded, bool* isWin) {
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

	SendMapBufferThreadArgument sendMapBufferThreadArgument = { _socket };
	
	sendMapBufferThreadArgument.mapBuffer = mapBuffer;
	sendMapBufferThreadArgument._isGameEnded = _isGameEnded;
	sendMapBufferThreadArgument.isWin = isWin;

	startThread(sendMapBufferThread, &sendMapBufferThreadArgument);

	updateMapBufferSingleplayer(manyLayer, mapBuffer);

	manyLayer->renderAll(manyLayer);
}

inline void removeFullLineMultiplayer(ManyLayer* manyLayer, int(*mapBuffer)[MAP_WIDTH], int shape, long long int* score, long long int* combo, SOCKET _socket, bool* _isGameEnded, bool* isWin) {
	int lineCount = 0;
	
	for(int i = 2; i < MAP_HEIGHT - 1; i++) {
		bool isLineFull = true;

		for(int j = 1; j < MAP_WIDTH - 1; j++) {
			if(mapBuffer[i][j] == 15) {
				isLineFull = false;

				break;
			}
		}

		if(isLineFull) {
			lineCount++;

			for(int j = i; j > 1; j--) {
				for(int k = 1; k < MAP_WIDTH - 1; k++) {
					mapBuffer[j][k] = mapBuffer[j - 1][k];
				}
			}

			i--;
		}
	}

	if(lineCount != 0) {
		*combo += lineCount;
	} else {
		*combo = 0;
	}

	*score += lineCount * (10 + (*combo - 1) * 2);

	SendScoreAndComboThreadArgument sendScoreAndComboThreadArgument = { _socket };
	
	sendScoreAndComboThreadArgument.score = *score;
	sendScoreAndComboThreadArgument.combo = *combo;
	sendScoreAndComboThreadArgument._isGameEnded = _isGameEnded;
	sendScoreAndComboThreadArgument.isWin = isWin;

	startThread(sendScoreAndComboThread, &sendScoreAndComboThreadArgument);

	SendAttackThreadArgument sendAttackThreadArgument = { _socket };
	
	sendAttackThreadArgument.lineCount = lineCount;
	sendAttackThreadArgument._isGameEnded = _isGameEnded;
	sendAttackThreadArgument.isWin = isWin;

	startThread(sendAttackThread, &sendAttackThreadArgument);

	updateComboSingleplayer(manyLayer, *combo);

	updateScoreSingleplayer(manyLayer, *score);

	updateMapBufferSingleplayer(manyLayer, mapBuffer);

	manyLayer->renderAll(manyLayer);
}

inline void updateNextShapesMultiplayer(ManyLayer* manyLayer, int* nextShapes, SOCKET _socket, bool* _isGameEnded, bool* isWin) {
	for(int i = 0; i < 5; i++) {
		manyLayer->images[imageIndexOffsetSinglePlayer + i].bitmapHandle = tetrominoBItmapHandles[nextShapes[i]];
	}

	SendNextShapesThreadArgument sendNextShapesThreadArgument = { _socket };
	
	sendNextShapesThreadArgument.nextShapes = nextShapes;
	sendNextShapesThreadArgument._isGameEnded = _isGameEnded;
	sendNextShapesThreadArgument.isWin = isWin;

	startThread(sendNextShapesThread, &sendNextShapesThreadArgument);

	manyLayer->renderAll(manyLayer);
}

inline void startConnection(ManyLayer* manyLayer, SOCKET* _socket, SOCKADDR_IN* socketAddress, bool isChallenger) {
	WSADATA wsaData;
	wchar_t ip[16];
	int socketAddressSize = sizeof(*socketAddress);

	manyLayer->images = NULL;

	manyLayer->imageCount = 0;

	if(isChallenger) {
		if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {

			_getch();

			WSACleanup();

			throwError(manyLayer, "WSAStartup failed");
		}

		*_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

		if(*_socket == INVALID_SOCKET) {
			closesocket(*_socket);
			WSACleanup();

			throwError(manyLayer, "Socket creating failed");
		}

		manyLayer->texts = (Text[]){
			{ L"Type challenged's IP", 700, 448, 36, 96, 600, L"家具稠8", RGB(255, 255, 255), false },
			{ ip, 1104, 736, 20, 50, 600, L"家具稠8", RGB(255, 255, 255), false },
		};

		manyLayer->textCount = 2;

		ip[0] = 0;

		for(int i = 0; i < 16;) {
			manyLayer->renderAll(manyLayer);

			char character = _getch();

			switch(character) {
				case SUBMIT: {
					if(i > 6) {
						ip[i] = 0;

						i = 16;
					}

					break;
				}
				case DELETE: {
					ip[i] = 0;

					if(i > 0) {
						ip[i - 1] = 0;
						i -= 1;
					}

					break;
				}
				case 46:
				case 48:
				case 49:
				case 50:
				case 51:
				case 52:
				case 53:
				case 54:
				case 55:
				case 56:
				case 57: {
					if(i != 15) {
						wsprintfW(ip, L"%ls%hc", ip, character);

						i++;
					}
				}
			}
		}

		memset(socketAddress, 0, socketAddressSize);
		socketAddress->sin_family = AF_INET;
		socketAddress->sin_port = PORT;

		InetPtonW(socketAddress->sin_family, ip, &socketAddress->sin_addr.s_addr);

		if(connect(*_socket, socketAddress, socketAddressSize) == SOCKET_ERROR) {
			closesocket(*_socket);
			WSACleanup();

			throwError(manyLayer, "Socket connecting failed");
		}
	} else {
		SOCKET serverSocket;
		SOCKADDR_IN serverSocketAddress;

		if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
			WSACleanup();

			throwError(manyLayer, "WSAStartup failed");
		}

		serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

		if(serverSocket == INVALID_SOCKET) {
			closesocket(serverSocket);
			WSACleanup();

			throwError(manyLayer, "Socket creating failed");
		}

		memset(&serverSocketAddress, 0, socketAddressSize);
		serverSocketAddress.sin_family = AF_INET;
		serverSocketAddress.sin_port = PORT;
		InetPtonW(serverSocketAddress.sin_family, L"127.0.0.1", &serverSocketAddress.sin_addr.s_addr);

		if(bind(serverSocket, &serverSocketAddress, socketAddressSize) == SOCKET_ERROR) {
			closesocket(serverSocket);
			WSACleanup();

			throwError(manyLayer, "Socket binding failed");
		}

		if(listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
			closesocket(serverSocket);
			WSACleanup();

			throwError(manyLayer, "Socket listening failed");
		}

		manyLayer->texts = (Text[]){
			{ L"Waiting for challenger", 700, 448, 36, 96, 600, L"家具稠8", RGB(255, 255, 255), false },
		};

		manyLayer->textCount = 1;

		*_socket = accept(serverSocket, socketAddress, &socketAddressSize);

		if(*_socket == INVALID_SOCKET) {
			closesocket(*_socket);
			WSACleanup();

			throwError(manyLayer, "Socket accepting failed");
		}
	}
}

inline void startGameMultiplayer(ManyLayer* manyLayer, bool isChallenger) {
	initializeBlockBitmapHandles(blockBitmapHandles);
	initializeTetrominoBitmapHandles(tetrominoBItmapHandles);

	SOCKET _socket;
	SOCKADDR_IN socketAddress;

	PlaySoundW(NULL, NULL, NULL);

	startConnection(manyLayer, &_socket, &socketAddress, isChallenger);

	PlaySoundW(L"sounds/multiplayerGameEnter_fix.wav", NULL, SND_FILENAME | SND_ASYNC);

	Sleep(2000);

	PlaySoundW(L"sounds/multiplayerGameLoop_fix.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);

	int mapBuffer[MAP_HEIGHT][MAP_WIDTH];

	Image images[(MAP_WIDTH * MAP_HEIGHT + 6) * 2];

	for(int i = 0; i < MAP_HEIGHT; i++) {
		for(int j = 0; j < MAP_WIDTH; j++) {
			mapBuffer[i][j] = i == 0 || i == MAP_HEIGHT - 1 || j == 0 || j == MAP_WIDTH - 1 ? 16 : 15;
			images[i * 12 + j] = (Image){ NULL, 240 + 64 * j, 16 + 64 * i, 4, false };
			images[i * 12 + j + imageIndexOffsetMultiplayer] = (Image){ NULL, 1536 + 64 * j, 16 + 64 * i, 4, false };
		}
	}

	for(int i = 0; i < 5; i++) {
		images[MAP_WIDTH * MAP_HEIGHT + i] = (Image){ NULL, 1040, 128 + 160 * i, 3, false };
		images[MAP_WIDTH * MAP_HEIGHT + i + imageIndexOffsetMultiplayer] = (Image){ NULL, 2336, 128 + 160 * i, 3, false };
	}

	images[MAP_WIDTH * MAP_HEIGHT + 5] = (Image){ NULL, 20, 128, 3, true };
	images[MAP_WIDTH * MAP_HEIGHT + 5 + imageIndexOffsetMultiplayer] = (Image){ NULL, 1316, 128, 3, true };

	manyLayer->images = images;

	manyLayer->imageCount = sizeof(images) / sizeof(Image);

	updateMapBufferSingleplayer(manyLayer, mapBuffer, false);

	manyLayer->texts = (Text[]){
		{ malloc(sizeof(wchar_t) * 7), 1472 - 688, 1356, 20, 50, 600, L"家具稠8", RGB(255, 255, 255), false },
		{ malloc(sizeof(wchar_t) * 7), 760 - 688, 768, 20, 50, 600, L"家具稠8", RGB(255, 255, 255), true },
		{ L"Hold", 720 - 688, 64, 26, 65, 600, L"家具稠8", RGB(255, 255, 255), false },
		{ L"Next", 1040, 64, 26, 65, 600, L"家具稠8", RGB(255, 255, 255), false },
		{ L"Combo", 656 - 688, 672, 26, 65, 600, L"家具稠8", RGB(255, 255, 255), true },
		{ L"Localhost", 936 - 688, 1356, 20, 50, 600, L"家具稠8", RGB(255, 255, 255), false },
		{ malloc(sizeof(wchar_t) * 7), 1472 + 608, 1356, 20, 50, 600, L"家具稠8", RGB(255, 255, 255), false },
		{ malloc(sizeof(wchar_t) * 7), 760 + 608, 768, 20, 50, 600, L"家具稠8", RGB(255, 255, 255), true },
		{ L"Hold", 720 + 608, 64, 26, 65, 600, L"家具稠8", RGB(255, 255, 255), false },
		{ L"Next", 2336, 64, 26, 65, 600, L"家具稠8", RGB(255, 255, 255), false },
		{ L"Combo", 656 + 608, 672, 26, 65, 600, L"家具稠8", RGB(255, 255, 255), true },
		{ malloc(sizeof(wchar_t) * 16), 936 + 608, 1356, 20, 50, 600, L"家具稠8", RGB(255, 255, 255), false},
		{ L"|", 1264, -157, 32, 1297, 100, L"家具稠8", RGB(255, 255, 255), false }
	};

	InetNtopW(socketAddress.sin_family, &socketAddress.sin_addr.s_addr, manyLayer->texts[11].content, 22);

	manyLayer->textCount = 13;

	manyLayer->renderAll(manyLayer);

	int x = MAP_WIDTH / 2 - 2, y = 0, rotation = 0, shape = rand() % 7, nextShapes[5] = { rand() % 7, rand() % 7, rand() % 7, rand() % 7, rand() % 7 }, holdingShape = 8, lineCount = 0, score = 0, combo = 0;
	long long int _time = 0, checkingTime = 41, lastCheckTime = 0;
	bool _isGameEnded = false, _isCollided = false, isHoldingShape = false, isAttacking = false, isWin = false;

	HandleReceivingThreadArgument handleReceivingThreadArgument = { _socket };

	handleReceivingThreadArgument.manyLayer = manyLayer;
	handleReceivingThreadArgument.mapBuffer = mapBuffer;
	handleReceivingThreadArgument._isGameEnded = &_isGameEnded;
	handleReceivingThreadArgument.isAttacking = &isAttacking;
	handleReceivingThreadArgument.isWin = &isWin;

	startThread(handleReceivingThread, &handleReceivingThreadArgument);

	/*
	typedef struct {
		SOCKET _socket;
		int(*mapBuffer)[MAP_WIDTH], * isWin;
	} SendMapBufferThreadArgument;

	typedef struct {
		SOCKET _socket;
		int score, combo, * isWin;
	} SendScoreAndComboThreadArgument;

	typedef struct {
		SOCKET _socket;
		int* nextShapes, * isWin;
	} SendNextShapesThreadArgument;

	typedef struct {
		SOCKET _socket;
		int holdingShape, * isWin;
	} SendHoldingShapeThreadArgument;

	typedef struct {
		SOCKET _socket;
		int lineCount, * isWin;
	} SendAttackThreadArgument;

	typedef struct {
		SOCKET _socket;
	} SendEndingThreadArgument;

	typedef struct {
		SOCKET _socket;
		ManyLayer* manyLayer;
		int(*mapBuffer)[MAP_WIDTH];
		bool* _isGameEnded, * isAttacking, * isWin;
	} HandleReceivingThreadArgument;*/

	while(!_isGameEnded) {
		if(!isAttacking) {
			if(_time % checkingTime == 0) {
				_isCollided = isCollided(mapBuffer, shape, rotation, x, y + 1);

				if(!_isCollided) {
					updateTetrominoMultiplayer(manyLayer, mapBuffer, shape, &rotation, &x, &y, DOWN, _socket, &isGameEnded, &isWin);
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
							updateTetrominoMultiplayer(manyLayer, mapBuffer, shape, &rotation, &x, &y, LEFT, _socket, &isGameEnded, &isWin);
							lastCheckTime = _time + 20;
						}

						break;
					}
					case RIGHT: {
						if(!isCollided(mapBuffer, shape, rotation, x + 1, y)) {
							updateTetrominoMultiplayer(manyLayer, mapBuffer, shape, &rotation, &x, &y, RIGHT, _socket, &isGameEnded, &isWin);
							lastCheckTime = _time + 20;
						}

						break;
					}
					case DOWN: {
						if(!isCollided(mapBuffer, shape, rotation, x, y + 1)) {
							updateTetrominoMultiplayer(manyLayer, mapBuffer, shape, &rotation, &x, &y, DOWN, _socket, &isGameEnded, &isWin);
							lastCheckTime = _time + 20;
						}


						break;
					}
					case 72:
					case ROTATE_CLOCKWISE: {
						if(!isCollided(mapBuffer, shape, (rotation + 1) % 4, x, y)) {
							updateTetrominoMultiplayer(manyLayer, mapBuffer, shape, &rotation, &x, &y, ROTATE_CLOCKWISE, _socket, &isGameEnded, &isWin);
							lastCheckTime = _time + 20;
						}

						break;
					}
					case ROTATE_COUNTERCLOCKWISE: {
						if(!isCollided(mapBuffer, shape, (rotation + 3) % 4, x, y)) {
							updateTetrominoMultiplayer(manyLayer, mapBuffer, shape, &rotation, &x, &y, ROTATE_COUNTERCLOCKWISE, _socket, &isGameEnded, &isWin);
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

						updateTetrominoMultiplayer(manyLayer, mapBuffer, shape, &rotation, &x, &y, DOWN, _socket, &isGameEnded, &isWin);

						inactivateTetrominoSingleplayer(manyLayer, mapBuffer, shape, rotation, x, y);

						_isCollided = true;
						lastCheckTime = _time - 20;

						break;
					}
					case HOLD: {
						if(!isHoldingShape) {
							removeTetromino(mapBuffer, shape, rotation, x, y);

							/*typedef struct {
								SOCKET _socket;
								int* nextShapes, * isWin;
							} SendNextShapesThreadArgument;

							typedef struct {
								SOCKET _socket;
								int holdingShape, * isWin;
							} SendHoldingShapeThreadArgument;*/

							if(holdingShape == 8) {
								holdingShape = shape;
								shape = nextShapes[0];

								for(int i = 0; i < 4; i++) {
									nextShapes[i] = nextShapes[i + 1];
								}

								nextShapes[4] = rand() % 7;

								updateNextShapesMultiplayer(manyLayer, nextShapes, _socket, &_isGameEnded, &isWin);
							} else {
								int temporaryShape = shape;

								shape = holdingShape;
								holdingShape = temporaryShape;
							}

							SendHoldingShapeThreadArgument sendHoldingShapeThreadArgument = { _socket };

							
							sendHoldingShapeThreadArgument.holdingShape = holdingShape;
							sendHoldingShapeThreadArgument._isGameEnded = &_isGameEnded;
							sendHoldingShapeThreadArgument.isWin = &isWin;

							startThread(sendHoldingShapeThread, &sendHoldingShapeThreadArgument);

							isHoldingShape = true;

							x = MAP_WIDTH / 2 - 2;
							y = 0;
							rotation = 0;

							updateHoldingSingleplayer(manyLayer, holdingShape);

							updateTetrominoMultiplayer(manyLayer, mapBuffer, shape, &rotation, &x, &y, NULL, _socket, &isGameEnded, &isWin);

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
					//ManyLayer* manyLayer, int(*mapBuffer)[MAP_WIDTH], int shape, int* lineCount, int* isSending, long long int* score, long long int* combo
					removeFullLineMultiplayer(manyLayer, mapBuffer, shape, &score, &combo, _socket, &_isGameEnded, &isWin);

					x = MAP_WIDTH / 2 - 2;
					y = 0;
					rotation = 0;
					shape = nextShapes[0];

					for(int i = 0; i < 4; i++) {
						nextShapes[i] = nextShapes[i + 1];
					}

					nextShapes[4] = rand() % 7;
					isHoldingShape = false;

					updateNextShapesMultiplayer(manyLayer, nextShapes, _socket, &_isGameEnded, &isWin);

					updateTetrominoMultiplayer(manyLayer, mapBuffer, shape, &rotation, &x, &y, NULL, _socket, &isGameEnded, &isWin);
				} else {
					_isGameEnded = true;
				}
			}

			Sleep(10);
			_time++;
		}
	}

	if(!isWin) {
		SendEndingThreadArgument sendEndingThreadArgument = { _socket };

		startThread(sendEndingThread, &sendEndingThreadArgument);
	} else {
		printf("You fucking loser");
	}

	_getch();
}

#endif