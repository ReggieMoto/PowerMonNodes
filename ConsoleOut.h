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

#include <mutex>
#include <queue>
#include <string>

namespace powermon {

	void consoleOutThreadProc(void);

	class ConsoleOut
	{
			std::queue<char *> screenLine;
			std::mutex *consoleOutMutex;

			ConsoleOut();

		public:

			static ConsoleOut& getConsoleOut(void);
			friend void consoleOutThreadProc(void);

			void printToConsole(void);
			bool isConsoleQueueEmpty(void);
			void queueOutput(const std::string& lineOfText);
			bool consoleIsActive(void);
			void releaseConsoleOut(void);

			~ConsoleOut();
	};

} // namespace powermon
