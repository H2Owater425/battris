#pragma once

#define WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <conio.h>
#include <time.h>
#include <wchar.h>
#include <process.h>
#include <ws2tcpip.h>
#include <winsock.h>
#include <windows.h>

#include "ManyLayer/ManyLayer.h"
#include "utility.h"
#include "define.h"
#include "singleplayer.h"

#pragma comment(lib, "ws2_32.lib")

#ifndef MULTIPLAYER_H_INCLUDED
#define MULTIPLAYER_H_INCLUDED


const int const imageIndexOffsetMultiplayer = MAP_WIDTH * MAP_HEIGHT + 6, textIndexOffsetMultiplayer = 6;

//void updateMapBufferSingleplayer(ManyLayer* manyLayer, int(*mapBuffer)[MAP_WIDTH]);
//void updateTetrominoSingleplayer(ManyLayer* manyLayer, int(*mapBuffer)[MAP_WIDTH], int shape, int* rotation, int* x, int* y, int direction);
//void inactivateTetrominoSingleplayer(ManyLayer* manyLayer, int(*mapBuffer)[MAP_WIDTH], int shape, int rotation, int x, int y);
//void updateHoldingSingleplayer(ManyLayer* manyLayer, int holdingShape);
//void updateNextShapesSingleplayer(ManyLayer* manyLayer, int* nextShapes);
//void updateScoreSingleplayer(ManyLayer* manyLayer, int score);
//void updateComboSingleplayer(ManyLayer* manyLayer, int combo);
//void removeFullLineSingleplayer(ManyLayer* manyLayer, int(*mapBuffer)[MAP_WIDTH], int shape, long long int* score, long long int* combo);

typedef struct {
	SOCKET* _socket;
	ManyLayer* manyLayer;
	int(*mapBuffer)[MAP_WIDTH];
	bool* _isGameEnded, * isReceiving, * isWin;
} HandleReceivingMultiplayerArgument;

typedef struct {
	SOCKET* _socket;
	int(*mapBuffer)[MAP_WIDTH], * nextShapes, * holdingShape, * score, * combo;
	bool* _isGameEnded;
} HandleSendingMultiplayerArgument;

inline void removeFullLineMultiplayer(ManyLayer* manyLayer, int(*mapBuffer)[MAP_WIDTH], int shape, long long int* score, long long int* combo) {
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

inline void handleReceivingMultiplayer(HandleReceivingMultiplayerArgument* argument) {
	char buffer[1024];
	int bufferIndex, temporaryInteger;

	while(!*(argument->_isGameEnded)) {
		/*recv(*(argument->_socket), buffer, sizeof(buffer), 0);*/

		if(recv(*(argument->_socket), buffer, sizeof(buffer), 0) == WSAEWOULDBLOCK || buffer[0] == 0) {
			*(argument->_isGameEnded) = true;
			*(argument->isWin) = true;

			break;
		}

		*(argument->isReceiving) = true;
		
		puts(buffer);

		bufferIndex = 0;

		for(int i = 1; i < MAP_HEIGHT - 1; i++) {
			for(int j = 1; j < MAP_WIDTH - 1; j++) {
				sscanf_s(buffer + bufferIndex, "%d", &temporaryInteger);
				bufferIndex += 2;

				argument->manyLayer->images[j + i * 12 + imageIndexOffsetMultiplayer].bitmapHandle = blockBitmapHandles[temporaryInteger];

			}
		}

		for(int i = 0; i < 5; i++) {
			sscanf_s(buffer + bufferIndex, "%d", &temporaryInteger);
			bufferIndex += 2;

			argument->manyLayer->images[MAP_WIDTH * MAP_HEIGHT + i + imageIndexOffsetMultiplayer].bitmapHandle = tetrominoBItmapHandles[temporaryInteger];
		}

		sscanf_s(buffer + bufferIndex, "%d", &temporaryInteger);
		bufferIndex += 2;

		argument->manyLayer->images[imageIndexOffsetMultiplayer + MAP_WIDTH * MAP_HEIGHT + 5].isHidden = temporaryInteger == 8;

		if(!(argument->manyLayer->images[imageIndexOffsetMultiplayer + MAP_WIDTH * MAP_HEIGHT + 5].isHidden)) {
			argument->manyLayer->images[imageIndexOffsetMultiplayer + MAP_WIDTH * MAP_HEIGHT + 5].bitmapHandle = tetrominoBItmapHandles[temporaryInteger];
		}

		sscanf_s(buffer + bufferIndex, "%d", &temporaryInteger);
		bufferIndex += getIntegerLength(temporaryInteger) + 1;

		wsprintfW(argument->manyLayer->texts[textIndexOffsetMultiplayer].content, L"%06ld", temporaryInteger);

		sscanf_s(buffer + bufferIndex, "%d", &temporaryInteger);
		bufferIndex += getIntegerLength(temporaryInteger) + 1;

		argument->manyLayer->texts[textIndexOffsetMultiplayer + 1].isHidden = temporaryInteger == 0;
		argument->manyLayer->texts[textIndexOffsetMultiplayer + 4].isHidden = argument->manyLayer->texts[textIndexOffsetMultiplayer + 1].isHidden;
		

		if(argument->manyLayer->texts[textIndexOffsetMultiplayer + 1].isHidden) {
			int previousCombo = _wtoi(argument->manyLayer->texts[textIndexOffsetMultiplayer + 1].content);

			printf("%d", previousCombo);

			exit(0);

			if(previousCombo != 0) {
				for(int j = MAP_HEIGHT - 2; j > previousCombo + 1; j--) {
					for(int k = 1; k < MAP_WIDTH - 1; k++) {
						argument->mapBuffer[j][k] = argument->mapBuffer[j - previousCombo][k];
					}
				}

				for(int j = MAP_HEIGHT - previousCombo - 1; j < previousCombo + 1; j++) {
					int removedIndex = rand() % 10 + 1;

					for(int k = 1; k < MAP_WIDTH - 1; k++) {
						argument->mapBuffer[j][k] = 14;
					}
				}
			}
		}

		wsprintfW(argument->manyLayer->texts[textIndexOffsetMultiplayer + 1].content, L"%ld", temporaryInteger);

		//*(argument->isReceiving) = false;
	}


	_endthreadex(0);
}

inline void handleSendingMultiplayer(HandleSendingMultiplayerArgument* argument) {
	char buffer[1024];
	int bufferIndex;

	while(!*(argument->_isGameEnded)) {
		memset(buffer, 0, sizeof(buffer));

		bufferIndex = 0;

		for(int i = 1; i < MAP_HEIGHT - 1; i++) {
			for(int j = 1; j < MAP_WIDTH - 1; j++) {
				sprintf_s(buffer + bufferIndex, sizeof(buffer) - bufferIndex, " %d", (argument->mapBuffer)[i][j] < 7 ? (argument->mapBuffer)[i][j] : (argument->mapBuffer)[i][j] - 7);

				bufferIndex += 2;
			}
		}

		for(int i = 0; i < 5; i++) {
			sprintf_s(buffer + bufferIndex, sizeof(buffer) - bufferIndex, " %d", (argument->nextShapes)[i]);
			bufferIndex += 2;
		}

		sprintf_s(buffer + bufferIndex, sizeof(buffer) - bufferIndex, " %d %d %d", *(argument->holdingShape), *(argument->score), *(argument->combo));

		if(send(*argument->_socket, buffer, sizeof(buffer), 0) == WSAEWOULDBLOCK) {
			break;
		}

		Sleep(10);
	}

	send(*argument->_socket, "", 1, 0);

	_endthreadex(0);
}

inline void startConnection(ManyLayer* manyLayer, SOCKET* _socket, SOCKADDR_IN* socketAddress, bool isChallenger) {
	WSADATA wsaData;
	wchar_t ip[16];
	int socketAddressSize = sizeof(*socketAddress);

	manyLayer->images = NULL;

	manyLayer->imageCount = 0;
	
	TIMEVAL timeout = { 10, 0 };

	if(isChallenger) {
		if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
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
			{ ip, 1104, 736, 20, 50, 600, L"家具稠8", RGB(255, 255, 255), false }
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

		if(connect(*_socket, (SOCKADDR *)socketAddress, socketAddressSize) == SOCKET_ERROR) {
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

		if(bind(serverSocket, (SOCKADDR *)&serverSocketAddress, socketAddressSize) == SOCKET_ERROR) {
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

		*_socket = accept(serverSocket, (SOCKADDR *)socketAddress, &socketAddressSize);

		if(*_socket == INVALID_SOCKET) {
			closesocket(*_socket);
			WSACleanup();

			throwError(manyLayer, "Socket accepting failed");
		}
	}

	if(setsockopt(*_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof timeout) < 0) {
		closesocket(*_socket);
		WSACleanup();

		throwError(manyLayer, "Socket setting failed");
	}

	if(setsockopt(*_socket, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof timeout) < 0) {
		closesocket(*_socket);
		WSACleanup();

		throwError(manyLayer, "Socket setting failed");
	}
}

inline void startGameMultiplayer(ManyLayer* manyLayer, bool isChallenger) {
	initializeBlockBitmapHandles(blockBitmapHandles);
	initializeTetrominoBitmapHandles(tetrominoBItmapHandles);

	SOCKET _socket;
	SOCKADDR_IN socketAddress;

	startConnection(manyLayer, &_socket, &socketAddress, isChallenger);

	PlaySoundW(L"sounds/multiplayerGameEnter_fix.wav", NULL, SND_FILENAME | SND_ASYNC);

	Sleep(2000);

	PlaySoundW(L"sounds/multiplayerGameLoop_fix.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);

	/*SOCKET* _socket;
	ManyLayer* manyLayer;
	int(*mapBuffer)[MAP_WIDTH];
	bool* isAttacked, * _isGameEnded;*/

	//startStartingTimer(manyLayer);

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

	updateMapBufferSingleplayer(manyLayer, mapBuffer);

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

	int x = MAP_WIDTH / 2 - 2, y = 0, rotation = 0, shape = rand() % 7, nextShapes[5] = { rand() % 7, rand() % 7, rand() % 7, rand() % 7, rand() % 7 }, holdingShape = 8, score = 0, combo = 0;
	long long int _time = 0, checkingTime = 41, lastCheckTime = 0;
	bool _isGameEnded = false, _isCollided = false, isHoldingShape = false, isReceiving = false, isWin = false;

	updateScoreSingleplayer(manyLayer, score);
	updateComboSingleplayer(manyLayer, combo);
	updateNextShapesSingleplayer(manyLayer, nextShapes);
	updateTetrominoSingleplayer(manyLayer, mapBuffer, shape, &rotation, &x, &y, NULL);

	for(int i = 0; i < MAP_HEIGHT; i++) {
		for(int j = 0; j < MAP_WIDTH; j++) {
			manyLayer->images[j + i * 12 + imageIndexOffsetMultiplayer].bitmapHandle = blockBitmapHandles[mapBuffer[i][j] < 7 ? mapBuffer[i][j] : mapBuffer[i][j] - 7];
		}
	}

	HandleReceivingMultiplayerArgument handleReceivingMultiplayerArgument;
	HandleSendingMultiplayerArgument handleSendingMultiplayerArgument;

	handleSendingMultiplayerArgument._socket = &_socket;
	handleSendingMultiplayerArgument.mapBuffer = mapBuffer;
	handleSendingMultiplayerArgument.nextShapes = nextShapes;
	handleSendingMultiplayerArgument.holdingShape = &holdingShape;
	handleSendingMultiplayerArgument.score = &score;
	handleSendingMultiplayerArgument.combo = &combo;
	handleSendingMultiplayerArgument._isGameEnded = &_isGameEnded;

	handleReceivingMultiplayerArgument._socket = &_socket;
	handleReceivingMultiplayerArgument.manyLayer = manyLayer;
	handleReceivingMultiplayerArgument.mapBuffer = mapBuffer;
	handleReceivingMultiplayerArgument._isGameEnded = &_isGameEnded;
	handleReceivingMultiplayerArgument.isReceiving = &isReceiving;
	handleReceivingMultiplayerArgument.isWin = &isWin;

	CloseHandle((HANDLE)_beginthreadex(NULL, 0, (_beginthreadex_proc_type)handleReceivingMultiplayer, &handleReceivingMultiplayerArgument, 0, NULL));
	CloseHandle((HANDLE)_beginthreadex(NULL, 0, (_beginthreadex_proc_type)handleSendingMultiplayer, &handleSendingMultiplayerArgument, 0, NULL));

	while(!_isGameEnded) {
		if(!isReceiving) {
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

					updateNextShapesSingleplayer(manyLayer, nextShapes);

					updateTetrominoSingleplayer(manyLayer, mapBuffer, shape, &rotation, &x, &y, NULL);
				} else {
					_isGameEnded = true;
				}
			}

			Sleep(10);
			_time++;
		}
	}


	free(manyLayer->texts[0].content);
	free(manyLayer->texts[1].content);
	free(manyLayer->texts[6].content);
	free(manyLayer->texts[7].content);
	free(manyLayer->texts[11].content);

	for(int i = 0; i < 10; i++) {
		DeleteObject(blockBitmapHandles[i]);
	}

	for(int i = 0; i < 7; i++) {
		DeleteObject(tetrominoBItmapHandles[i]);
	}

	wchar_t content[128];

	wsprintfW(content, L"Multiplayer Localhost %06ld %s %s\n", score);

	updateRecordW(content);

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

	if(isWin) {
		puts("winner");
	} else {
		puts("loser");
	}

	_getch();
}

#endif