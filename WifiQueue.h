// ==============================================================
//
// class WifiQueue
//
// Copyright (c) 2017 David Hammond 
// All Rights Reserved.
// 
// ==============================================================
// NOTICE:  All information contained herein is and remains the
// property of David Hammond. The intellectual and technical
// concepts contained herein are proprietary to David Hammond
// and may be covered by U.S.and Foreign Patents, patents in
// process, and are protected by trade secret or copyright law.
// Dissemination of this information or reproduction of this
// material is strictly forbidden unless prior written permission
// is obtained David Hammond.
// ==============================================================
#pragma once

#include <string>
#include <queue>
#include "ConsoleOut.h"
#include "WifiSocket.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

using namespace std;

class WifiQueue
{
	HANDLE wifiQueueThread;
	bool threadIsActive;
	ConsoleOut& console;
	WifiSocket& socket;

	queue<  char *> packetQueue;
	HANDLE ghMutex;

	WifiQueue();

public:

	static WifiQueue& getWifiQueue(void);
	void queueOutput(string& packetData);
	void threadProc(void);
	inline bool wifiQueueIsActive(void) { return threadIsActive; }
	inline void releaseWifiQueue(void) { threadIsActive = FALSE; }

	~WifiQueue();
};
