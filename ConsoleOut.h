// ==============================================================
//
// class ConsoleOut
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

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

using namespace std;

class ConsoleOut
{
	queue<  char *> screenLine;
	HANDLE ghMutex;
	HANDLE consoleThread;
	bool threadActive;

	ConsoleOut();

public:

	static ConsoleOut& getConsoleOut(void);
	void queueOutput(string& lineOfText);
	void consoleThreadProc(void);
	inline bool consoleIsActive(void) { return threadActive; }

	~ConsoleOut();
};

