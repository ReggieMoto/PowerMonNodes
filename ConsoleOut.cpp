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

//#include "stdafx.h"
#include "ConsoleOut.h"
#include <iostream>
#include <string>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds

using namespace std;

extern "C" {
	DWORD WINAPI ConsoleThreadProc(_In_ LPVOID console)
	{
		DWORD retValue = 0;
		((ConsoleOut *)console)->consoleThreadProc();
		return retValue;
	}
}

ConsoleOut& ConsoleOut::getConsoleOut(void)
{
	static ConsoleOut consoleOut;

	return consoleOut;
}

ConsoleOut::ConsoleOut() :
	threadActive(TRUE)
{
	// Create a mutex with no initial owner
	ghMutex = CreateMutex(
		NULL,              // default security attributes
		FALSE,             // initially not owned
		NULL);

	SECURITY_ATTRIBUTES securityAttrs;

	securityAttrs.nLength = sizeof(SECURITY_ATTRIBUTES);
	securityAttrs.lpSecurityDescriptor = NULL;
	securityAttrs.bInheritHandle = FALSE;

	// Create this node's thread
	consoleThread = CreateThread(
		&securityAttrs,
		0, // default dwStackSize
		ConsoleThreadProc, // lpStartAddress,
		(void *)this, //lpParameter,
		0x00000000, // CREATE_SUSPENDED, dwCreationFlags,
		NULL //lpThreadId
	);

	if (consoleThread == NULL)
		cout << "Failed to create console output thread." << endl;
}

void ConsoleOut::queueOutput(string& lineOfText)
{
	DWORD semWaitResult;
	char *text = new char[lineOfText.length() + 1];
	strcpy_s(text, lineOfText.length() + 1, lineOfText.c_str());
	
	semWaitResult = WaitForSingleObject(ghMutex, INFINITE);
	screenLine.push(text);
	ReleaseMutex(ghMutex);
}

void ConsoleOut::consoleThreadProc(void)
{
	DWORD semWaitResult;

	do {

		while (!screenLine.empty())
		{
			semWaitResult = WaitForSingleObject(ghMutex, INFINITE);
			char *lineOfText = screenLine.front();
			screenLine.pop();
			ReleaseMutex(ghMutex);

			cout << lineOfText;
			free(lineOfText);
		}

		this_thread::sleep_for(chrono::seconds(1));

	} while (threadActive);
}

ConsoleOut::~ConsoleOut()
{
	DWORD semWaitResult;

	while (!screenLine.empty())
	{
		semWaitResult = WaitForSingleObject(ghMutex, INFINITE);
		char *lineOfText = screenLine.front();
		screenLine.pop();
		ReleaseMutex(ghMutex);

		cout << lineOfText << endl;
		free(lineOfText);
	}
}
