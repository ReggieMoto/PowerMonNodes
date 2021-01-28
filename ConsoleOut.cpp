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

#include "ConsoleOut.h"

#include <chrono>         // std::chrono::seconds
#include <cstring>
#include <iostream>
#include <thread>         // std::this_thread::sleep_for

using namespace std;

namespace powermon {

	bool consoleOutThreadIsActive;

	void consoleOutThreadProc(void)
	{
		// Initialize the singleton
		ConsoleOut &console = ConsoleOut::getConsoleOut();

		console.consoleOutMutex->lock();
		consoleOutThreadIsActive = true;
		console.consoleOutMutex->unlock();

		do {
			while (!console.isConsoleQueueEmpty()) {
				console.printToConsole();
			}

			this_thread::sleep_for(chrono::seconds(1));

		} while (console.consoleIsActive());

		cout << "Leaving the Console Out ThreadProc" << endl;
	}

	ConsoleOut& ConsoleOut::getConsoleOut(void)
	{
		static ConsoleOut consoleOut;

		return (consoleOut);
	}

	ConsoleOut::ConsoleOut()
	{
		consoleOutThreadIsActive = false;
		consoleOutMutex = new std::mutex;
	}

	void ConsoleOut::queueOutput(const string& lineOfText)
	{
		char *text = new char[lineOfText.length() + 1];
		strncpy(text, lineOfText.c_str(), lineOfText.length() + 1);

		consoleOutMutex->lock();
		screenLine.push(text);
		consoleOutMutex->unlock();
	}

	bool ConsoleOut::consoleIsActive(void)
	{
		consoleOutMutex->lock();
		bool threadIsActive = consoleOutThreadIsActive;
		consoleOutMutex->unlock();
		return (threadIsActive);
	}

	void ConsoleOut::releaseConsoleOut(void)
	{
		consoleOutMutex->lock();
		consoleOutThreadIsActive = false;
		consoleOutMutex->unlock();
	}

	bool ConsoleOut::isConsoleQueueEmpty(void)
	{
		consoleOutMutex->lock();
		bool consoleIsEmpty = screenLine.empty();
		consoleOutMutex->unlock();
		return consoleIsEmpty;
	}

	void ConsoleOut::printToConsole(void)
	{
		consoleOutMutex->lock();
		char *lineOfText = screenLine.front();
		screenLine.pop();
		consoleOutMutex->unlock();

		cout << lineOfText;
		free(lineOfText);
	}

	ConsoleOut::~ConsoleOut()
	{
		while (!screenLine.empty())
		{
			char *lineOfText = screenLine.front();
			screenLine.pop();

			cout << lineOfText << endl;
			free(lineOfText);
		}

		free(consoleOutMutex);
	}

} // namespace powermon
